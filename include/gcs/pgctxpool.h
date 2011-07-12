
#pragma once

typedef struct {
	char *conn;
	int refcount;
	unsigned long owner;
} gcs_pgctx_t;

void gcs_ctxpool_init(int);
void gcs_ctxpool_free();

int gcs_pgctx_alloc(const char *);
int gcs_pgctx_free(int);
gcs_pgctx_t *gcs_pgctx_acquire();
void gcs_pgctx_release(gcs_pgctx_t *);

