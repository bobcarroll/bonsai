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
    gcs_ntlmctx *authctx;
} gcs_session;

gcs_session *gcs_session_init(const char *);
void gcs_session_close(gcs_session *);
void gcs_session_bind_user(gcs_session *, const char *);
int gcs_session_auth_init(gcs_session *, gcs_ntlmctx **);
int gcs_session_auth_check(gcs_session *);

