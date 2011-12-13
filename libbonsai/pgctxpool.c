/**
 * Bonsai - open source group collaboration and application lifecycle management
 * Copyright (c) 2011 Bob Carroll
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

/**
 * @brief   Postgres context pool functions
 *
 * @author  Bob Carroll (bob.carroll@alum.rit.edu)
 */

#include <string.h>
#include <stdlib.h>
#include <pthread.h>

#include <pgctxpool.h>
#include <log.h>

static pgctx **_ctxpool = NULL;
static int _ctxcount = 0;
static pthread_mutex_t _ctxmtx = PTHREAD_MUTEX_INITIALIZER;
static char *_nulltag = NULL;

/**
 * Initialises the database connection pool.
 *
 * @param count     the size of the context pool
 *
 * @return the actual size of the context pool
 */
int pg_pool_init(int count)
{
    if (_ctxpool) {
        log_warn("context pool is already initialised");
        return 0;
    } else if (count < 1) {
        log_error("context count must be at least 1 (was %d)", count);
        return 0;
    }

    pthread_mutex_lock(&_ctxmtx);
    _ctxpool = (pgctx **)calloc(count, sizeof(pgctx *));
    _ctxcount = count;
    pthread_mutex_unlock(&_ctxmtx);

    log_debug("initialised PG context pool with size %d", count);
    return count;
}

/**
 * Deallocates the database connection pool.
 */
void pg_pool_free()
{
    pthread_mutex_lock(&_ctxmtx);

    if (_ctxpool) {
        int i;
        for (i = 0; i < _ctxcount; i++) {
            if (_ctxpool[i]) {
                if (_ctxpool[i]->tag)
                    free(_ctxpool[i]->tag);

                free(_ctxpool[i]->conn);
                free(_ctxpool[i]);
            }

            _ctxpool[i] = NULL;
            log_debug("freed PG context %d", i);
        }

        free(_ctxpool);
    }

    _ctxpool = NULL;
    _ctxcount = 0;

    pthread_mutex_unlock(&_ctxmtx);

    log_debug("freed PG context pool");
}

/**
 * Gets the size of the context pool.
 *
 * @return the pool size
 */
int pg_pool_size()
{
    pthread_mutex_lock(&_ctxmtx);
    int result = _ctxcount;
    pthread_mutex_unlock(&_ctxmtx);

    return result;
}

/**
 * Allocates a new database context.
 *
 * @param conn  the PG connection name
 * @param tag   a marker for PG contexts for targeting queries
 *
 * @return 1 on success, 0 on failure
 */
int pg_context_alloc(const char *conn, const char *tag)
{
    if (!conn)
        return 0;

    pthread_mutex_lock(&_ctxmtx);

    int i;
    for (i = 0; i < _ctxcount; i++) {
        if (_ctxpool[i] && strcmp(_ctxpool[i]->conn, conn) == 0) {
            pthread_mutex_unlock(&_ctxmtx);
            log_warn("a PG context is already allocated for %s", conn);
            return 1;
        } else if (_ctxpool[i])
            continue;

        _ctxpool[i] = (pgctx *)malloc(sizeof(pgctx));
        bzero(_ctxpool[i], sizeof(pgctx));

        _ctxpool[i]->conn = strdup(conn);

        if (tag)
            _ctxpool[i]->tag = strdup(tag);

        pthread_mutex_unlock(&_ctxmtx);
        log_debug("allocated PG connection %s as context %d", conn, i);

        return 1;
    }

    pthread_mutex_unlock(&_ctxmtx);
    log_warn("all context slots are in use");

    return 0;
}

/**
 * Gets the count of allocated PG contexts.
 *
 * @return the context count
 */
int pg_context_count()
{
    int count;

    pthread_mutex_lock(&_ctxmtx);
    for (count = 0; count < _ctxcount && _ctxpool[count]; count++);
    pthread_mutex_unlock(&_ctxmtx);

    return count;
}

/**
 * Acquires a thread-exclusive database connection. This function will
 * block until a connection becomes available.
 *
 * Passing NULL for the tag argument will always return bootstrapping
 * contexts (as in, contexts created without a tag) even if the contexts
 * were retagged by calling pg_context_retag_default().
 *
 * @param tag   an optional marker for PG contexts for targeting queries

 * @return a connection context
 */
pgctx *pg_context_acquire(const char *tag)
{
    pgctx *result = NULL;
    int i = 0, m;

    log_debug("acquiring PG context with tag %s", tag);

    pthread_mutex_lock(&_ctxmtx);
    for (i = 0; i < _ctxcount && _ctxpool[i]; i++) {
        m = ((!tag && !_ctxpool[i]->tag) || 
             (!tag && _nulltag && _ctxpool[i]->tag && strcmp(_ctxpool[i]->tag, _nulltag) == 0) || 
             (tag && _ctxpool[i]->tag && strcmp(_ctxpool[i]->tag, tag) == 0));

        if (_ctxpool[i] && _ctxpool[i]->owner == (unsigned long)pthread_self() && m) {
            log_debug("got PG context %d (reused)", i);

            _ctxpool[i]->refcount++;
            result = _ctxpool[i];

            break;
        }
    }
    pthread_mutex_unlock(&_ctxmtx);

    if (result)
        return result;

    i = 0;
    while (1) {
        pthread_mutex_lock(&_ctxmtx);
        m = ((!tag && !_ctxpool[i]->tag) || 
             (!tag && _nulltag && _ctxpool[i]->tag && strcmp(_ctxpool[i]->tag, _nulltag) == 0) || 
             (tag && _ctxpool[i]->tag && strcmp(_ctxpool[i]->tag, tag) == 0));

        if (_ctxpool[i] && _ctxpool[i]->owner == 0 && m) {
            log_debug("got PG context %d", i);

            _ctxpool[i]->owner = (unsigned long)pthread_self();
            _ctxpool[i]->refcount++;
            result = _ctxpool[i];
        }
        pthread_mutex_unlock(&_ctxmtx);

        if (result)
            break;

        i++;
        if (i == _ctxcount || !_ctxpool[i]) {
            log_info("No available PG context, sleeping...");
            sleep(5);
            i = 0;
        }
    }

    return result;
}

/**
 * Releases the given database connection. If no other callers in the
 * thread hold a lock then the context is released back into the pool.
 *
 * @param context
 */
void pg_context_release(pgctx *context)
{
    int i;

    if (!context)
        return;

    pthread_mutex_lock(&_ctxmtx);
    for (i = 0; i < _ctxcount; i++) {
        if (_ctxpool[i] && strcmp(_ctxpool[i]->conn, context->conn) == 0) {
            context->refcount--;

            if (context->refcount == 0) {
                context->owner = 0;
                log_debug("released PG context %d", i);
            } else
                log_debug("PG context %d is still in use", i);

            break;
        }
    }
    pthread_mutex_unlock(&_ctxmtx);
}

/**
 * Sets the tag for bootstrapping connections.
 *
 * @param tag   the new tag
 *
 * @return true on success, false otherwise
 */
int pg_context_retag_default(const char *tag)
{
    if (!tag)
        return 0;

    pthread_mutex_lock(&_ctxmtx);

    int i;
    for (i = 0; i < _ctxcount && _ctxpool[i]; i++) {
        if (!_ctxpool[i]->tag)
            _ctxpool[i]->tag = strdup(tag);
        else if (_ctxpool[i]->tag && _nulltag && strcmp(_nulltag, tag) == 0) {
            free(_ctxpool[i]->tag);
            _ctxpool[i]->tag = strdup(tag);
        }
    }

    if (_nulltag)
        free(_nulltag);
    _nulltag = strdup(tag);

    pthread_mutex_unlock(&_ctxmtx);

    return 1;
}

