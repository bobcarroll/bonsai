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
 * @brief   Team Foundation project collection setup tool
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
    int opt, err = 0;
    char *cfgfile = NULL;
    char *logfile = NULL;
    char *pcdbdsn = NULL;
    char *pcname = NULL;
    config_t config;
    const char *cfgdbdsn = NULL;
    const char *pguser = NULL;
    const char *pgpasswd = NULL;
    wordexp_t expresult;
    tf_host **hostarr = NULL;
    tf_error dberr;
    int result = 0;

    while (err == 0 && (opt = getopt(argc, argv, "c:d:l:u:w")) != -1) {

        switch (opt) {

        case 'c':
            cfgfile = strdup(optarg);
            break;

        case 'd':
            lev = atoi(optarg);
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

    if (argc < 2 || err) {
        printf("USAGE: tfprep [options] <dsn> <name>\n");
        printf("\n");
        printf("Example DSN: tfsfoo@dbserver.example.com\n");
        printf("\n");
        printf("Options:\n");
        printf("  -c <file>             configuration file (required)\n");
        printf("  -d <level>            log level (default: 5)\n");
        printf("  -l <file>             log file (default: ~/pcnew.log)\n");
        printf("\n");
        return 1;
    }

    if (!logfile) {
        wordexp("~/pcnew.log", &expresult, 0);
        logfile = strdup(expresult.we_wordv[0]);
        wordfree(&expresult);
    }

    config_init(&config);
    if (config_read_file(&config, cfgfile) != CONFIG_TRUE) {
        fprintf(stderr, "pcnew: failed to read config file!\n");
        free(cfgfile);
        return 1;
    }

    free(cfgfile);

    config_lookup_string(&config, "configdsn", &cfgdbdsn);
    config_lookup_string(&config, "pguser", &pguser);
    config_lookup_string(&config, "pgpasswd", &pgpasswd);

    pcdbdsn = strdup(argv[0]);
    pcname = strdup(argv[1]);

    if (!log_open(logfile, lev, 1, 1)) {
        fprintf(stderr, "pcnew: failed to open log file!\n");
        return 1;
    }

    if (pg_pool_init(2) != 2) {
        log_fatal("failed to initialise PG context pool");
        log_close();
        return 1;
    }

    if (!pg_connect(cfgdbdsn, pguser, pgpasswd, 1, "configdb")) {
        log_fatal("failed to connect to configuration database");
        log_close();
        return 1;
    }

    if (!pg_connect(pcdbdsn, pguser, pgpasswd, 1, "pcdb")) {
        pg_disconnect();
        log_fatal("failed to connect to project collection database");
        log_close();
        return 1;
    }

    pgctx *cfgctx = pg_acquire_trans("configdb");
    pgctx *pcctx = pg_acquire_trans("pcdb");

    log_notice("creating project collection %s", pcname);
    if (tf_create_collection(pcctx) != TF_ERROR_SUCCESS) {
        log_fatal("failed to create new project collection!");
        goto error;
    }

    dberr = tf_fetch_hosts(cfgctx, NULL, &hostarr);

    if (dberr != TF_ERROR_SUCCESS || !hostarr[0]) {
        log_fatal("no Team Foundation instances were found!");
        hostarr = tf_free_host_array(hostarr);
        goto error;
    }

    /* TODO support more than one host */
    if (tf_attach_collection(pcctx, pcname, cfgctx, hostarr[0]) != TF_ERROR_SUCCESS) {
        log_fatal("failed to attach project collection to server instance!");
        hostarr = tf_free_host_array(hostarr);
        goto error;
    }

    hostarr = tf_free_host_array(hostarr);

    log_notice("project collection %s is initialised", pcname);
    pg_release_commit(cfgctx);
    pg_release_commit(pcctx);
    goto cleanup;

error:
    pg_release_rollback(cfgctx);
    pg_release_rollback(pcctx);
    result = 1;

cleanup:
    free(logfile);

    pg_disconnect();
    log_close();
    config_destroy(&config);

    return result;
}

