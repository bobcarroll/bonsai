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
 * @brief   session management
 *
 * @author  Bob Carroll (bob.carroll@alum.rit.edu)
 */

#include <string.h>
#include <stdlib.h>
#include <pthread.h>

#include <gcs/session.h>
#include <gcs/log.h>

#define MAX_SESSIONS    10240

static gcs_session **_sessions = NULL;
static pthread_mutex_t _sessionmtx = PTHREAD_MUTEX_INITIALIZER;
static int _tail = -1;

/**
 * Creates or resumes a client session.
 *
 * @param id    a unique session ID
 *
 * @return a session structure, or NULL on error
 */
gcs_session *gcs_session_init(const char *id)
{
    gcs_session *result = NULL;
    int i;

    if (!id)
        return NULL;

    pthread_mutex_lock(&_sessionmtx);

    if (!_sessions) {
        _sessions = (gcs_session **)calloc(MAX_SESSIONS, sizeof(gcs_session *));
        gcslog_debug("initialised session storage with size %d", MAX_SESSIONS);
    }

    for (i = 0; i <= _tail && i < MAX_SESSIONS; i++) {
        if (strcmp(_sessions[i]->id, id) == 0) {
            result = _sessions[i];
            break;
        }
    }

    if (!result && i == MAX_SESSIONS)
        gcslog_error("no session slots available!");
    else if (!result) {
        gcslog_info("allocating session %d with ID %s", i, id);
        result = _sessions[i] = (gcs_session *)malloc(sizeof(gcs_session));
        bzero(result, sizeof(gcs_session));
        _tail = i;

        result->id = strdup(id);
        result->refcount++;
        result->lastseen = time(NULL);
    } else {
        gcslog_debug("re-using session %d with ID %s", i, id);
        result->refcount++;
        result->lastseen = time(NULL);
    }

    pthread_mutex_unlock(&_sessionmtx);

    return result;
}

/**
 * Releases a handle to the given session.
 *
 * @param session
 */
void gcs_session_close(gcs_session *session)
{
    if (!session)
        return;

    pthread_mutex_lock(&_sessionmtx);
    session->refcount--;
    gcslog_debug("released session handle for %s (%d remaining)", session->id, session->refcount);
    pthread_mutex_unlock(&_sessionmtx);
}

/**
 * Sets the user ID for the given session.
 *
 * @param session   a session structure
 * @param userid    the new user ID (cannot be NULL)
 */
void gcs_session_bind_user(gcs_session *session, const char *userid)
{
    if (!session || session->userid || !userid)
        return;

    pthread_mutex_lock(&_sessionmtx);
    gcslog_info("binding session %s to user %s", session->id, userid);
    session->userid = strdup(userid);
    pthread_mutex_unlock(&_sessionmtx);
}


