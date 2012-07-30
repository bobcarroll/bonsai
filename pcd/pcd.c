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
 * @brief   team project collection daemon
 *
 * @author  Bob Carroll (bob.carroll@alum.rit.edu)
 */

#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <libconfig.h>

#include <libcsoap/soap-server.h>

#include <log.h>
#include <pgcommon.h>
#include <pgctxpool.h>
#include <authz.h>
#include <util.h>

#include <pcd.h>

#define MAXCONNS 100

int main(int argc, char **argv)
{
    char **soapargs;
    herror_t soaperr;
    const char *logdir = NULL;
    char *logfile = NULL;
    char logname[1024];
    int lev = LOG_WARN, levovrd = 0;
    int opt, fg = 0, err = 0;
    char *cfgfile = NULL;
    char confgroup[1024];
    char confitem[1024];
    config_t config;
    const char *pgdsn = NULL;
    const char *pguser = NULL;
    const char *pgpasswd = NULL;
    int maxconns = MAXCONNS, dbconns = 1, nport;
    char maxconns_str[3];
    const char *port = NULL;
    const char *prefix = NULL;
    const char *ntlmhelper = NULL;
    const char *smbhost = NULL;
    const char *smbuser = NULL;
    const char *smbpasswd = NULL;
    char *tpcname = NULL;

    while (err == 0 && (opt = getopt(argc, argv, "c:fd:n:")) != -1) {

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

        case 'n':
            tpcname = strdup(optarg);
            break;

        default:
            err = 1;
        }
    }

    if (argc < 2 || !cfgfile || !tpcname || err) {
        printf("USAGE: csd -c <file> -n <tpc name> [-f] [-d <level>]\n");
        return 1;
    }

    config_init(&config);
    if (config_read_file(&config, cfgfile) != CONFIG_TRUE) {
        fprintf(stderr, "csd: failed to read config file!\n");
        free(cfgfile);
        return 1;
    }

    free(cfgfile);
    snprintf(confgroup, 1024, "tpc-%s", tpcname);

    config_lookup_string(&config, "logdir", &logdir);
    if (!levovrd)
        config_lookup_int(&config, "loglevel", &lev);

    snprintf(logname, 1024, "tpc-%s.log", tpcname);
    logfile = combine(logdir, logname);
    if (!log_open(logfile, lev, fg)) {
        fprintf(stderr, "csd: failed to open log file!\n");
        goto cleanup_cfg;
    }

    config_lookup_string(&config, "configdsn", &pgdsn);
    config_lookup_string(&config, "pguser", &pguser);
    config_lookup_string(&config, "pgpasswd", &pgpasswd);

    config_lookup_string(&config, "ntlmhelper", &ntlmhelper);
    if (!ntlmhelper) {
        log_warn("ntlmhelper is not set");
        ntlmhelper = "";
    }

    config_lookup_string(&config, "smbhost", &smbhost);
    if (!smbhost) {
        log_warn("smbhost is not set");
        smbhost = "";
    }

    config_lookup_string(&config, "smbuser", &smbuser);
    if (!smbuser) {
        log_warn("smbuser is not set");
        smbuser = "";
    }

    config_lookup_string(&config, "smbpasswd", &smbpasswd);
    if (!smbpasswd) {
        log_warn("smbpasswd is not set");
        smbpasswd = "";
    }

    snprintf(confitem, 1024, "%s.maxconns", confgroup);
    config_lookup_int(&config, confitem, &maxconns);
    if (maxconns < 1) {
        log_warn("maxconns must be at least 1 (was %d)", maxconns);
        maxconns = 1;
    }
    snprintf(maxconns_str, 3, "%d", maxconns);

    snprintf(confitem, 1024, "%s.dbconns", confgroup);
    config_lookup_int(&config, confitem, &dbconns);
    if (dbconns < 1) {
        log_warn("dbconns must be at least 1 (was %d)", dbconns);
        dbconns = 1;
    }
    dbconns++;

    snprintf(confitem, 1024, "%s.listen", confgroup);
    config_lookup_string(&config, confitem, &port);
    nport = (port) ? atoi(port) : 0;
    if (nport == 0 || nport != (nport & 0xffff)) {
        log_fatal("listen must be a valid TCP port number (was %d)", nport);
        goto cleanup_log;
    }

    snprintf(confitem, 1024, "%s.prefix", confgroup);
    config_lookup_string(&config, confitem, &prefix);
    if (!prefix || strcmp(prefix, "") == 0 || prefix[0] != '/') {
        log_fatal("prefix must be a valid URI (was %s)", prefix);
        goto cleanup_log;
    }

    if (pg_pool_init(dbconns) != dbconns) {
        log_fatal("failed to initialise PG context pool");
        goto cleanup_log;
    }

    if (!pg_connect(pgdsn, pguser, pgpasswd, 1, NULL)) {
        log_fatal("failed to connect to PG");
        goto cleanup_log;
    }

    httpd_set_timeout(10);
    soapargs = (char **)calloc(7, sizeof(char *));
    soapargs[0] = argv[0];
    soapargs[1] = "-NHTTPport";
    soapargs[2] = strdup(port);
    soapargs[3] = "-NHTTPmaxconn";
    soapargs[4] = strdup(maxconns_str);
    soapargs[5] = "-NHTTPntlmhelper";
    soapargs[6] = strdup(ntlmhelper);
    soaperr = soap_server_init_args(7, soapargs);

    if (!tpc_services_init(prefix, tpcname, pguser, pgpasswd, dbconns - 1)) {
        log_fatal("team project collection services failed to start!");
        goto cleanup_db;
    }

    authz_init(smbhost, smbuser, smbpasswd);

    log_notice("starting SOAP server");
    soap_server_run();

    log_notice("shutting down");
    soap_server_destroy();

    free(soapargs[2]);
    free(soapargs);

    authz_free();
    free(tpcname);

cleanup_db:
    pg_disconnect();

cleanup_log:
    log_close();

cleanup_cfg:
    config_destroy(&config);

    if (logfile)
        free(logfile);

    return 0;
}

