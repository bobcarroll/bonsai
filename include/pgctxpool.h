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

#pragma once

typedef struct {
    char *conn;
    int refcount;
    unsigned long owner;
    char *tag;
} pgctx;

int gcs_ctxpool_init(int);
void gcs_ctxpool_free();
int gcs_ctxpool_size();

int gcs_pgctx_alloc(const char *, const char *);
int gcs_pgctx_count();
pgctx *gcs_pgctx_acquire(const char *);
void gcs_pgctx_release(pgctx *);
int gcs_pgctx_retag_default(const char *);

