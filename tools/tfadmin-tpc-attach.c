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
 * @brief   team project collection attach tool
 *
 * @author  Bob Carroll (bob.carroll@alum.rit.edu)
 */

#include <stdio.h>
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
    int opt, fg = 0, err = 0;
    char *cfgfile = NULL;
    char *logfile = NULL;
    char *tpcdbdsn = NULL;
    char *tpcname = NULL;
    config_t config;
    const char *cfgdbdsn = NULL;
    const char *pguser = NULL;
    const char *pgpasswd = NULL;
    wordexp_t expresult;
    tf_host **hostarr = NULL;
    tf_error dberr;
    int result = 0;

    while (err == 0 && (opt = getopt(argc, argv, "c:d:fl:")) != -1) {

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

        default:
            err = 1;
        }
    }

    argc -= optind;
    argv += optind;

    if (argc != 2 || err || !cfgfile) {
        printf("USAGE: tfadmin tpc-attach [options] -c <file> <dsn> <tpc name>\n");
        printf("\n");
        printf("Example DSN: tfsfoo@dbserver.example.com\n");
        printf("\n");
        printf("Options:\n");
        printf("  -c <file>             configuration file (required)\n");
        printf("  -d <level>            log level (default: 5)\n");
        printf("  -f                    write log messages to standard out\n");
        printf("  -l <file>             log file (default: ~/tfadmin.log)\n");
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

    config_lookup_string(&config, "configdsn", &cfgdbdsn);
    config_lookup_string(&config, "pguser", &pguser);
    config_lookup_string(&config, "pgpasswd", &pgpasswd);

    tpcdbdsn = strdup(argv[0]);
    tpcname = strdup(argv[1]);

    if (!log_open(logfile, lev, fg)) {
        fprintf(stderr, "tfadmin: failed to open log file!\n");
        result = 1;
        goto cleanup_config;
    }

    if (pg_pool_init(2) != 2) {
        log_fatal("failed to initialise PG context pool");
        fprintf(stderr, "tfadmin: failed to initialise (see %s for details)\n", logfile);
        result = 1;
        goto cleanup_log;
    }

    if (!pg_connect(cfgdbdsn, pguser, pgpasswd, 1, "configdb")) {
        log_fatal("failed to connect to configuration database");
        fprintf(stderr, "tfadmin: failed to connect to the configuration database (see %s for details)\n", logfile);
        result = 1;
        goto cleanup_db;
    }

    if (!pg_connect(tpcdbdsn, pguser, pgpasswd, 1, "tpcdb")) {
        log_fatal("failed to connect to team project collection database");
        fprintf(stderr, "tfadmin: failed to connect to the team project collection database (see %s for details)\n", logfile);
        result = 1;
        goto cleanup_db;
    }

    pgctx *cfgctx = pg_acquire_trans("configdb");
    pgctx *tpcctx = pg_acquire_trans("tpcdb");

    dberr = tf_fetch_hosts(cfgctx, NULL, &hostarr);

    if (dberr != TF_ERROR_SUCCESS || !hostarr[0]) {
        log_fatal("no Team Foundation instances were found!");
        hostarr = tf_free_host_array(hostarr);
        goto error;
    }

    /* TODO support more than one host */
    if (tf_attach_collection(tpcctx, tpcname, cfgctx, hostarr[0]) != TF_ERROR_SUCCESS) {
        log_fatal("failed to attach project collection to server instance!");
        hostarr = tf_free_host_array(hostarr);
        goto error;
    }

    hostarr = tf_free_host_array(hostarr);

    printf("Team project collection '%s' is now attached to this server instance\n", tpcname);
    pg_release_commit(cfgctx);
    pg_release_commit(tpcctx);
    goto cleanup_db;

error:
    pg_release_rollback(cfgctx);
    pg_release_rollback(tpcctx);
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

    if (tpcdbdsn)
        free(tpcdbdsn);

    if (tpcname)
        free(tpcname);

    return result;
}

