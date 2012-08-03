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
 * @brief   Team Foundation server setup tool
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

#include <tf/schema.h>
#include <tf/catalog.h>
#include <tf/location.h>
#include <tf/servicehost.h>
#include <tf/webservices.h>

int main(int argc, char **argv)
{
    int lev = LOG_NOTICE;
    int opt, fg = 0, err = 0, nopass = 0, nport;
    char *cfgfile = NULL;
    char *logfile = NULL;
    const char *shuser = getenv("USER");
    const char *dbdsn = NULL;
    char *dbuser = NULL;
    char *dbpasswd = NULL;
    char *port = NULL;
    char *prefix = NULL;
    char prompt[64];
    char hostname[_POSIX_HOST_NAME_MAX];
    char serveruri[_POSIX_HOST_NAME_MAX * 2];
    wordexp_t expresult;
    config_t config;
    tf_node *orgroot = NULL;
    tf_node *infroot = NULL;
    tf_node *servinst = NULL;
    tf_host *host = NULL;
    tf_access_map *accmap = NULL;
    tf_property *instprop = NULL;
    tf_host **hostarr = NULL;
    tf_error dberr;
    int result = 0;

    hostname[0] = '\0';

    while (err == 0 && (opt = getopt(argc, argv, "c:d:fh:l:p:P:t:u:w")) != -1) {

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

        case 'h':
            strncpy(hostname, optarg, _POSIX_HOST_NAME_MAX);
            break;

        case 'l':
            logfile = strdup(optarg);
            break;

        case 'p':
            dbpasswd = strdup(optarg);
            break;

        case 'P':
            prefix = strdup(optarg);
            break;

        case 't':
            port = strdup(optarg);
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

    if (argc != 0 || err || !cfgfile) {
        printf("USAGE: tfadmin setup [options] -c <file>\n");
        printf("\n");
        printf("Common Options:\n");
        printf("  -c <file>             configuration file (required)\n");
        printf("  -d <level>            log level (default: 5)\n");
        printf("  -f                    write log messages to standard out\n");
        printf("  -l <file>             log file (default: ~/tfadmin.log)\n");
        printf("\n");
        printf("Team Services Options:\n");
        printf("  -h <host>             hostname where HAProxy is installed (default: localhost)\n");
        printf("  -t <port>             TCP port HAProxy listens on (default: 8080)\n");
        printf("  -P <prefix>           URL prefix with leading forward slash (default: /tfs)\n");
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

    config_lookup_string(&config, "configdsn", &dbdsn);

    nport = (port) ? atoi(port) : 0;
    if (port && (nport == 0 || nport != (nport & 0xffff))) {
        fprintf(stderr, "listening port must be a valid TCP port number (was %d)\n", nport);
        result = 1;
        goto cleanup_config;
    } else if (!port)
        port = strdup("8080"); /* default */

    if (prefix && (strcmp(prefix, "") == 0 || prefix[0] != '/')) {
        fprintf(stderr, "prefix must be a valid URI (was %s)\n", prefix);
        result = 1;
        goto cleanup_config;
    } else if (!prefix)
        prefix = strdup("/tfs"); /* default */

    if (!dbuser && shuser)
        dbuser = strdup(shuser);
    else if (!dbuser) {
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
        log_fatal("failed to connect to PG");
        fprintf(stderr, "tfadmin: failed to connect to the database (see %s for details)\n", logfile);
        result = 1;
        goto cleanup_db;
    }

    pgctx *ctx = pg_context_acquire(NULL);

    if (tf_fetch_hosts(ctx, &hostarr) == TF_ERROR_SUCCESS) {
        hostarr = tf_free_host_array(hostarr);
        pg_context_release(ctx);
        printf("Team Foundation deployment is already initialised\n");
        goto cleanup_db;
    }

    pg_context_release(ctx);
    ctx = pg_acquire_trans(NULL);

    if (tf_init_configdb(ctx) != TF_ERROR_SUCCESS)
        goto error;

    printf("Building initial catalog\n");

    orgroot = tf_new_node(
        NULL, 
        TF_CATALOG_TYPE_ORGANIZATIONAL_ROOT, 
        "Organizational Root", 
        "The root of the catalog tree that describes the organizational makeup of the TFS deployment.");
    sprintf(orgroot->child, TF_CATALOG_ORGANIZATION_ROOT);
    
    if ((dberr = tf_add_node(ctx, orgroot)) != TF_ERROR_SUCCESS)
        goto error;

    infroot = tf_new_node(
        NULL, 
        TF_CATALOG_TYPE_INFRASTRUCTURE_ROOT, 
        "Infrastructure Root", 
        "The root of the catalog tree that describes the physical makeup of the TFS deployment.");
    sprintf(infroot->child, TF_CATALOG_INFRASTRUCTURE_ROOT);
    
    if ((dberr = tf_add_node(ctx, infroot)) != TF_ERROR_SUCCESS)
        goto error;

    servinst = tf_new_node(
        orgroot, 
        TF_CATALOG_TYPE_SERVER_INSTANCE, 
        "Team Foundation Server Instance", 
        NULL);
    dberr = tf_add_node(ctx, servinst);

    if (dberr != TF_ERROR_SUCCESS)
        goto error;

    printf("Registering services\n");

    if (!hostname[0])
        gethostname(hostname, _POSIX_HOST_NAME_MAX);

    snprintf(serveruri, _POSIX_HOST_NAME_MAX * 2, "http://%s:%s/%s", hostname, port, prefix);
    accmap = tf_new_access_map(TF_ACCESSMAP_PUBLIC_MONIKER, TF_ACCESSMAP_PUBLIC_DISPLNAME, serveruri);
    dberr = tf_add_access_map(ctx, accmap);

    if (dberr != TF_ERROR_SUCCESS)
        goto error;

    dberr = tf_set_default_access_map(ctx, accmap);
    accmap = tf_free_access_map(accmap);

    if (dberr != TF_ERROR_SUCCESS)
        goto error;

    tf_service *service = tf_new_service(
        TF_SERVICE_LOCATION_ID, 
        TF_SERVICE_LOCATION_TYPE, 
        TF_SERVICE_LOCATION_NAME, 
        TF_CATALOG_TOOL_FRAMEWORK);
    tf_set_service_url(service, TF_LOCATION_SERVICE_ENDPOINT, TF_SERVICE_RELTO_CONTEXT);
    dberr = tf_add_service(ctx, service);

    if (dberr != TF_ERROR_SUCCESS)
        goto error;

    tf_service_ref *ref = tf_new_service_ref(&servinst->resource, service, "Location");
    service = tf_free_service(service);
    dberr = tf_add_service_ref(ctx, ref);
    ref = tf_free_service_ref(ref);

    if (dberr != TF_ERROR_SUCCESS)
        goto error;

    service = tf_new_service(
        TF_SERVICE_CATALOG_ID, 
        TF_SERVICE_CATALOG_TYPE, 
        TF_SERVICE_CATALOG_NAME, 
        TF_CATALOG_TOOL_FRAMEWORK);
    tf_set_service_url(service, TF_CATALOG_SERVICE_ENDPOINT, TF_SERVICE_RELTO_CONTEXT);
    dberr = tf_add_service(ctx, service);

    if (dberr != TF_ERROR_SUCCESS)
        goto error;

    ref = tf_new_service_ref(&servinst->resource, service, "Catalog");
    service = tf_free_service(service);
    dberr = tf_add_service_ref(ctx, ref);
    ref = tf_free_service_ref(ref);

    if (dberr != TF_ERROR_SUCCESS)
        goto error;

    printf("Registering Team Foundation service host\n");
    host = tf_new_host(TF_TEAM_FOUNDATION_SERVICE_NAME, dbdsn);
    dberr = tf_add_host(ctx, host);

    if (dberr != TF_ERROR_SUCCESS)
        goto error;

    instprop = tf_new_property(
        TF_PROPERTY_INSTANCE_ID_ID,
        servinst->resource.propertyid,
        host->id);
    dberr = tf_add_property(ctx, instprop);

    if (dberr != TF_ERROR_SUCCESS)
        goto error;

    printf("Team Foundation deployment is initialised\n");
    pg_release_commit(ctx);
    goto cleanup_db;

error:
    pg_release_rollback(ctx);
    result = 1;
    fprintf(stderr, "tfadmin: the operation failed (see %s for details)\n", logfile);

cleanup_db:
    orgroot = tf_free_node(orgroot);
    infroot = tf_free_node(infroot);
    servinst = tf_free_node(servinst);
    host = tf_free_host(host);

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

    if (dbuser)
        free(dbuser);

    if (dbpasswd)
        free(dbpasswd);

    if (port)
        free(port);

    if (prefix)
        free(prefix);

    return result;
}

