
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

int gcs_log_open(const char *filename, int ll, int fg)
{
	_loglevel = (ll >= GCS_LOG_FATAL) ? ll : GCS_LOG_FATAL;
	_foreground = fg;

	if (filename != NULL && (_logfile = fopen(filename, "a")) == NULL) {
		gcslog_fatal("failed to open log file");
		return 0;
	}

	gcslog_notice("LOG OPEN");
	return 1;
}

void gcs_log_close()
{
	if (_logfile == NULL)
		return;

	fclose(_logfile);
	_logfile = NULL;
}

int gcs_log_level()
{
	return _loglevel;
}

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

	if (_logfile != NULL) {
		timestamp = time(NULL);
		datestr = ctime(&timestamp);
		datestr[strlen(datestr) - 1] = '\0';

		fprintf(_logfile, "%s %s", datestr, logstr);
		fflush(_logfile);
	}
}

