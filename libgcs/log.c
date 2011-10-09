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
 * @brief   system logging
 *
 * @author  Bob Carroll (bob.carroll@alum.rit.edu)
 */

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <time.h>
#include <pthread.h>

#include <gcs/log.h>

#define MAX_LOG 8192

static int _loglevel = 0;
static int _foreground = 0;
static FILE *_logfile = NULL;

/**
 * Gets the fix-length string representation of the given
 * log level. The result is guaranteed to always be an
 * eight character string. Calling functions should not
 * free the result.
 *
 * @param lev   the log level value
 *
 * @return the log level name
 */
static char *_leveltostr(int lev)
{
    switch (lev) {

    case GCS_LOG_FATAL:
        return "Fatal   ";

    case GCS_LOG_CRITICAL:
        return "Critical";

    case GCS_LOG_ERROR:
        return "Error   ";

    case GCS_LOG_WARN:
        return "Warn    ";

    case GCS_LOG_NOTICE:
        return "Notice  ";

    case GCS_LOG_INFO:
        return "Info    ";

    case GCS_LOG_DEBUG:
        return "Debug   ";

    case GCS_LOG_TRACE:
        return "Trace   ";

    default:
        return "Unknown ";
    }
}

/**
 * Opens the log for writing. If "filename" is NULL then the log
 * output is written to stdout.
 *
 * @param filename  the file to write the log to (optionally NULL)
 * @param ll        the logging level
 * @param fg        flag to enable writing to stdout even if filename isn't NULL
 *
 * @return 1 on success, 0 on failure
 */
int gcs_log_open(const char *filename, int ll, int fg)
{
    _loglevel = (ll >= GCS_LOG_FATAL) ? ll : GCS_LOG_FATAL;
    _foreground = fg;

    if (filename && !(_logfile = fopen(filename, "a"))) {
        gcslog_fatal("failed to open log file");
        return 0;
    }

    gcslog_notice("LOG OPEN");
    return 1;
}

/**
 * Closes the log file. Subsequent calls to gcs_log_write() will
 * write to stdout.
 */
void gcs_log_close()
{
    if (!_logfile)
        return;

    fclose(_logfile);
    _logfile = NULL;
}

/**
 * Gets the current logging level.
 *
 * @return the log level value
 */
int gcs_log_level()
{
    return _loglevel;
}

/**
 * Writes a message to the log file.
 *
 * @param lev       the message priority
 * @param fn        the calling function name
 * @param ln        the calling line number
 * @param format    message format string followed by format args
 */
void gcs_log_write(int lev, const char *fn, int ln, const char *format, ...)
{
    va_list args;
    char *msgstr;
    time_t timestamp;
    char *datestr;
    char *logstr;

    if (lev > _loglevel)
        return;

    msgstr = (char *)alloca(MAX_LOG + 1);
    logstr = (char *)alloca(MAX_LOG + 1);

    va_start(args, format);
    vsnprintf(msgstr, MAX_LOG, format, args);
    va_end(args);

    snprintf(logstr, 
         MAX_LOG, 
         "%d %15lu %s %s [%s:%d]\n", 
         (int)getpid(), 
         (unsigned long)pthread_self(), 
         _leveltostr(lev), 
         msgstr, 
         fn, 
         ln);

    if (_foreground) {
        printf("%s", logstr);
        fflush(stdout);
    }

    if (_logfile) {
        timestamp = time(NULL);
        datestr = ctime(&timestamp);
        datestr[strlen(datestr) - 1] = '\0';

        fprintf(_logfile, "%s %s", datestr, logstr);
        fflush(_logfile);
    }
}

