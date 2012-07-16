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
 * @brief   team project collection creation tool
 *
 * @author  Bob Carroll (bob.carroll@alum.rit.edu)
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <wordexp.h>
#include <limits.h>

#include <libconfig.h>

#include <log.h>
#include <pgcommon.h>
#include <pgctxpool.h>

#include <tf/collection.h>
#include <tf/schema.h>
#include <tf/catalog.h>
#include <tf/location.h>
#include <tf/servicehost.h>

int main(int argc, char **argv)
{
    int lev = LOG_NOTICE;
    int opt, fg = 0, err = 0, nopass = 0;
    char *cfgfile = NULL;
    char *logfile = NULL;
    const char *shuser = getenv("USER");
    char *dbdsn = NULL;
    char *dbuser = NULL;
    char *dbpasswd = NULL;
    char prompt[64];
    config_t config;
    wordexp_t expresult;
    tf_access_map **accmaparr = NULL;
    tf_error dberr;
    int result = 0;

    while (err == 0 && (opt = getopt(argc, argv, "c:d:fl:p:u:w")) != -1) {

        switch (opt) {

        case 'c':
            cfgfile = strdup(optarg);
            break;

        case 'd':
            lev = atoi(optarg);
            break;

        case 'f':
            fg = 1;
            break;

        case 'l':
            logfile = strdup(optarg);
            break;

        case 'p':
            dbpasswd = strdup(optarg);
            break;

        case 'u':
            dbuser = strdup(optarg);
            break;

        case 'w':
            nopass = 1;
            break;

        default:
            err = 1;
        }
    }

    argc -= optind;
    argv += optind;

    if (argc != 1 || err || !cfgfile) {
        printf("USAGE: tfadmin tpc-create [options] -c <file> <tpc dsn>\n");
        printf("\n");
        printf("Example DSN: tfsfoo@dbserver.example.com\n");
        printf("\n");
        printf("Common Options:\n");
        printf("  -c <file>             configuration file (required)\n");
        printf("  -d <level>            log level (default: 5)\n");
        printf("  -f                    write log messages to standard out\n");
        printf("  -l <file>             log file (default: ~/tfadmin.log)\n");
        printf("\n");
        printf("Database Options:\n");
        printf("  -u <username>         database user to connect as (default: shell user)\n");
        printf("  -p <password>         password for the database user (will prompt if omitted)\n");
        printf("  -w                    never prompt for password\n");
        printf("\n");

        result = 1;
        goto cleanup;
    }

    if (!logfile) {
        wordexp("~/tfadmin.log", &expresult, 0);
        logfile = strdup(expresult.we_wordv[0]);
        wordfree(&expresult);
    }

    config_init(&config);
    if (config_read_file(&config, cfgfile) != CONFIG_TRUE) {
        fprintf(stderr, "tfadmin: failed to read config file!\n");
        result = 1;
        goto cleanup;
    }

    dbdsn = strdup(argv[0]);

    if (!dbuser && shuser)
        dbuser = strdup(shuser);
    else if (!dbuser) {
        log_fatal("empty database username");
        fprintf(stderr, "tfadmin: no database user specified!\n");
        result = 1;
        goto cleanup_config;
    }

    if (!nopass && !dbpasswd) {
        snprintf(prompt, 64, "Password for user %s: ", dbuser);
        dbpasswd = getpass(prompt);
    } else if (!dbpasswd)
        dbpasswd = strdup("");

    if (!log_open(logfile, lev, fg)) {
        fprintf(stderr, "tfadmin: failed to open log file!\n");
        result = 1;
        goto cleanup_config;
    }

    if (pg_pool_init(1) != 1) {
        log_fatal("failed to initialise PG context pool");
        fprintf(stderr, "tfadmin: failed to initialise (see %s for details)\n", logfile);
        result = 1;
        goto cleanup_log;
    }

    if (!pg_connect(dbdsn, dbuser, dbpasswd, 1, NULL)) {
        log_fatal("failed to connect to project collection database");
        fprintf(stderr, "tfadmin: failed to connect to the database (see %s for details)\n", logfile);
        result = 1;
        goto cleanup_db;
    }

    pgctx *ctx = pg_context_acquire(NULL);

    if (tf_fetch_access_map(ctx, &accmaparr) == TF_ERROR_SUCCESS) {
        accmaparr = tf_free_access_map_array(accmaparr);
        pg_context_release(ctx);
        printf("Database for team project collection is already initialised\n");
        goto cleanup_db;
    }

    pg_context_release(ctx);
    ctx = pg_acquire_trans(NULL);

    if (tf_create_collection(ctx) != TF_ERROR_SUCCESS) {
        log_fatal("failed to create new project collection!");
        goto error;
    }

    printf("Database for team project collection created successfully\n");
    pg_release_commit(ctx);
    goto cleanup_db;

error:
    pg_release_rollback(ctx);
    result = 1;
    fprintf(stderr, "tfadmin: the operation failed (see %s for details)\n", logfile);

cleanup_db:
    pg_disconnect();

cleanup_log:
    log_close();

cleanup_config:
    config_destroy(&config);

cleanup:
    if (cfgfile)
        free(cfgfile);

    if (logfile)
        free(logfile);

    if (dbdsn)
        free(dbdsn);

    return result;
}

