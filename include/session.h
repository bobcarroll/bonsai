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

#include <time.h>

#include <ntlmauth.h>

typedef struct {
    char *id;
    int refcount;
    time_t lastseen;
    char *userid;
    ntlmctx_t *authctx;
} session_t;

session_t *session_init(const char *);
void session_close(session_t *);
void session_bind_user(session_t *, const char *);
int session_auth_init(session_t *, const char *, ntlmctx_t **);
int session_auth_check(session_t *);

