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

#include <unistd.h>
#include <sys/types.h>

#define NTLM_RESET      0
#define NTLM_NEGOTIATE  1
#define NTLM_RESPONSE   2
#define NTLM_SUCCESS    3

typedef struct ntlmctx {
    char *scope;
    char *helper;
    pid_t pid;
    int infd;
    int outfd;
    int errfd;
    int state;
    struct ntlmctx *next;
} ntlmctx_t;

ntlmctx_t *ntlm_auth_init(const char *, const char *);
void ntlm_auth_free(ntlmctx_t *);
int ntlm_auth_challenge(ntlmctx_t *, const char *, char **);

