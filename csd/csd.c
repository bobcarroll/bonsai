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
 * @brief   core services daemon
 *
 * @author  Bob Carroll (bob.carroll@alum.rit.edu)
 */

#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <libconfig.h>

#include <gcs/log.h>
#include <gcs/pgcommon.h>
#include <gcs/pgctxpool.h>

#include <cs/location.h>
#include <cs/catalog.h>

int main(int argc, char **argv)
{
    char **soapargs;
    herror_t soaperr;
    SoapRouter *locrouter;
    SoapRouter *catrouter;
    const char *logfile = NULL;
    int lev = GCS_LOG_WARN, levovrd = 0;
    int opt, fg = 0, err = 0;
    char *cfgfile = NULL;
    config_t config;
    const char *pgdsn = NULL;
    const char *pguser = NULL;
    const char *pgpasswd = NULL;
    int maxconns = 1, dbconns = 1, nport;
    const char *port = NULL;
    const char *prefix = NULL;

    while (err == 0 && (opt = getopt(argc, argv, "c:fd:")) != -1) {

        switch (opt) {

        case 'c':
            cfgfile = strdup(optarg);
            break;

        case 'f':
            fg = 1;
            break;

        case 'd':
            lev = atoi(optarg);
            levovrd = 1;
            break;

        default:
            err = 1;
        }
    }

    if (argc < 2 || cfgfile == NULL || err) {
        printf("USAGE: csd -c <file> [-f] [-d <level>]\n");
        return 1;
    }

    config_init(&config);
    if (config_read_file(&config, cfgfile) != CONFIG_TRUE) {
        fprintf(stderr, "csd: failed to read config file!\n");
        free(cfgfile);
        return 1;
    }

    free(cfgfile);

    config_lookup_string(&config, "logfile", &logfile);
    if (!levovrd)
        config_lookup_int(&config, "loglevel", &lev);

    if (!gcs_log_open(logfile, lev, fg)) {
        fprintf(stderr, "csd: failed to open log file!\n");
        config_destroy(&config);
        return 1;
    }

    config_lookup_string(&config, "configdsn", &pgdsn);
    config_lookup_string(&config, "pguser", &pguser);
    config_lookup_string(&config, "pgpasswd", &pgpasswd);

    config_lookup_int(&config, "maxconns", &maxconns);
    if (maxconns < 1) {
        gcslog_warn("maxconns must be at least 1 (was %d)", maxconns);
        maxconns = 1;
    }

    config_lookup_int(&config, "dbconns", &dbconns);
    if (dbconns < 1) {
        gcslog_warn("dbconns must be at least 1 (was %d)", dbconns);
        dbconns = 1;
    }

    config_lookup_string(&config, "bindport", &port);
    nport = (port != NULL) ? atoi(port) : 0;
    if (nport == 0 || nport != (nport & 0xffff)) {
        gcslog_fatal("bindport must be a valid TCP port number (was %d)", nport);

        config_destroy(&config);
        gcs_log_close();

        return 1;
    }

    config_lookup_string(&config, "prefix", &prefix);
    if (prefix == NULL || strcmp(prefix, "") == 0 || prefix[0] != '/') {
        gcslog_fatal("prefix must be a valid URI (was %s)", prefix);

        config_destroy(&config);
        gcs_log_close();

        return 1;
    }

    if (gcs_ctxpool_init(maxconns) != maxconns) {
        gcslog_fatal("failed to initialise PG context pool");

        config_destroy(&config);
        gcs_log_close();

        return 1;
    }

    if (!gcs_pg_connect(pgdsn, pguser, pgpasswd, dbconns)) {
        gcslog_fatal("failed to connect to PG");

        config_destroy(&config);
        gcs_log_close();

        return 1;
    }

    httpd_set_timeout(10);
    soapargs = (char **)calloc(3, sizeof(char *));
    soapargs[0] = argv[0];
    soapargs[1] = "-NHTTPport";
    soapargs[2] = strdup(port);
    soaperr = soap_server_init_args(3, soapargs);

    location_service_init(&locrouter, prefix);
    catalog_service_init(&catrouter, prefix);

    gcslog_notice("starting SOAP server");
    soap_server_run();

    gcslog_notice("shutting down");
    soap_server_destroy();

    free(soapargs[2]);
    free(soapargs);

    config_destroy(&config);

    gcs_pg_disconnect();
    gcs_log_close();

    return 0;
}

