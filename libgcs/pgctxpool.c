
#include <string.h>
#include <stdlib.h>
#include <pthread.h>

#include <gcs/pgctxpool.h>
#include <gcs/log.h>

static gcs_pgctx_t **_ctxpool = NULL;
static int _ctxcount = 0;
static pthread_mutex_t _ctxmtx;

void gcs_ctxpool_init(int count)
{
	pthread_mutexattr_t mattr;

	if (_ctxpool != NULL) {
		gcslog_warn("context pool is already initialised");
		return;
	} else if (count < 1) {
		gcslog_error("context count must be at least 1 (was %d)", count);
		return;
	}

	_ctxpool = (gcs_pgctx_t **)calloc(count, sizeof(gcs_pgctx_t *));
	_ctxcount = count;

	pthread_mutexattr_init(&mattr);
	pthread_mutexattr_setpshared(&mattr, PTHREAD_PROCESS_SHARED);

	pthread_mutex_init(&_ctxmtx, &mattr);
	pthread_mutexattr_destroy(&mattr);
}

void gcs_ctxpool_free()
{
	pthread_mutex_lock(&_ctxmtx);

	if (_ctxpool != NULL) {
		int i;
		for (i = 0; i < _ctxcount; i++) {
			if (_ctxpool[i] != NULL) {
				free(_ctxpool[i]->conn);
				free(_ctxpool[i]);
			}

			_ctxpool[i] = NULL;
			gcslog_debug("freed PG context %d", i);
		}

		free(_ctxpool);
	}

	_ctxpool = NULL;
	_ctxcount = 0;

	pthread_mutex_unlock(&_ctxmtx);
	pthread_mutex_destroy(&_ctxmtx);

	gcslog_debug("freed PG context pool");
}

int gcs_pgctx_alloc(const char *conn)
{
	pthread_mutex_lock(&_ctxmtx);

	int i;
	for (i = 0; i < _ctxcount; i++) {
		if (_ctxpool[i] != NULL && strcmp(_ctxpool[i]->conn, conn) == 0) {
			pthread_mutex_unlock(&_ctxmtx);
			gcslog_warn("a PG context is already allocated for %s", conn);
			return 1;
		} else if (_ctxpool[i] != NULL)
			continue;

		_ctxpool[i] = (gcs_pgctx_t *)malloc(sizeof(gcs_pgctx_t));
		memset(_ctxpool[i], 0, sizeof(gcs_pgctx_t));

		_ctxpool[i]->conn = strdup(conn);

		pthread_mutex_unlock(&_ctxmtx);
		gcslog_debug("allocated PG connection %s as context %d", conn, i);

		return 1;
	}

	pthread_mutex_unlock(&_ctxmtx);
	gcslog_warn("all context slots are in use");

	return 0;
}

gcs_pgctx_t *gcs_pgctx_acquire()
{
	gcs_pgctx_t *result = NULL;
	int i = 0;

	pthread_mutex_lock(&_ctxmtx);
	for (i = 0; i < _ctxcount; i++) {
		if (_ctxpool[i] != NULL && _ctxpool[i]->owner == (unsigned long)pthread_self()) {
			gcslog_debug("got PG context %d (reused)", i);

			_ctxpool[i]->refcount++;
			result = _ctxpool[i];

			break;
		}
	}
	pthread_mutex_unlock(&_ctxmtx);

	if (result != NULL)
		return result;

	i = 0;
	while (1) {
		pthread_mutex_lock(&_ctxmtx);
		if (_ctxpool[i] != NULL && _ctxpool[i]->owner == 0) {
			gcslog_debug("got PG context %d", i);

			_ctxpool[i]->owner = (unsigned long)pthread_self();
			_ctxpool[i]->refcount++;
			result = _ctxpool[i];
		}
		pthread_mutex_unlock(&_ctxmtx);

		if (result != NULL)
			break;

		i++;
		if (i == _ctxcount) {
			gcslog_info("No available PG context, sleeping...");
			sleep(5);
			i = 0;
		}
	}

	return result;
}

void gcs_pgctx_release(gcs_pgctx_t *context)
{
	int i;

	pthread_mutex_lock(&_ctxmtx);
	for (i = 0; i < _ctxcount; i++) {
		if (_ctxpool[i] != NULL && strcmp(_ctxpool[i]->conn, context->conn) == 0) {
			context->refcount--;

			if (context->refcount == 0) {
				context->owner = 0;
				gcslog_debug("released PG context %d", i);
			} else
				gcslog_debug("PG context %d is still in use", i);

			break;
		}
	}
	pthread_mutex_unlock(&_ctxmtx);
}

