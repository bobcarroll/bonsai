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
#include <string.h>
#include <unistd.h>
#include <wordexp.h>
#include <limits.h>

#include <log.h>
#include <pgcommon.h>
#include <pgctxpool.h>

#include <tf/schema.h>
#include <tf/catalog.h>
#include <tf/location.h>
#include <tf/servicehost.h>

int main(int argc, char **argv)
{
    int lev = LOG_NOTICE;
    int opt, err = 0, nopass = 0;
    char *cfgfile = NULL;
    char *logfile = NULL;
    char *dbdsn = NULL;
    char *dbuser = NULL;
    char *dbpasswd = NULL;
    char prompt[64];
    char hostname[_POSIX_HOST_NAME_MAX];
    char serveruri[_POSIX_HOST_NAME_MAX * 2];
    wordexp_t expresult;
    tf_node *orgroot = NULL;
    tf_node *infroot = NULL;
    tf_node *servinst = NULL;
    tf_host *host = NULL;
    tf_access_map *accmap = NULL;
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

    if (argc < 1 || err) {
        printf("USAGE: tfprep [options] <dsn>\n");
        printf("\n");
        printf("Example DSN: tfs_config@dbserver.example.com\n");
        printf("\n");
        printf("Options:\n");
        printf("  -c <file>             configuration file\n");
        printf("  -d <level>            log level (default: 5)\n");
        printf("  -l <file>             log file (default: ~/tfprep.log)\n");
        printf("  -u <username>         database user to connect as\n");
        printf("  -w                    never prompt for password\n");
        printf("\n");
        return 1;
    }

    if (!logfile) {
        wordexp("~/tfprep.log", &expresult, 0);
        logfile = strdup(expresult.we_wordv[0]);
    }

    dbdsn = strdup(argv[0]);

    if (!nopass && dbuser) {
        snprintf(prompt, 64, "Password for user %s: ", dbuser);
        dbpasswd = getpass(prompt);
    } else
        dbpasswd = strdup("");

    if (!dbuser)
        dbuser = strdup("");

    if (!log_open(logfile, lev, 1, 1)) {
        fprintf(stderr, "tfprep: failed to open log file!\n");
        return 1;
    }

    if (pg_pool_init(1) != 1) {
        log_fatal("failed to initialise PG context pool");
        log_close();
        return 1;
    }

    if (!pg_connect(dbdsn, dbuser, dbpasswd, 1, NULL)) {
        log_fatal("failed to connect to PG");
        log_close();
        return 1;
    }

    pgctx *ctx = pg_acquire_trans(NULL);

    log_notice("initialising configuration database (schema revision %d)", TF_SCHEMA_REVISION);
    if (tf_init_configdb(ctx) != TF_ERROR_SUCCESS)
        goto error;

    log_notice("building initial catalog");

    orgroot = tf_new_node(
        NULL, 
        TF_CATALOG_TYPE_ORGANIZATIONAL_ROOT, 
        "Organizational Root", 
        "The root of the catalog tree that describes the organizational makeup of the TFS deployment.");
    sprintf(orgroot->child, TF_CATALOG_ORGANIZATION_ROOT);
    
    if ((dberr = tf_add_node(ctx, orgroot)) != TF_ERROR_SUCCESS)
        return dberr;

    infroot = tf_new_node(
        NULL, 
        TF_CATALOG_TYPE_INFRASTRUCTURE_ROOT, 
        "Infrastructure Root", 
        "The root of the catalog tree that describes the physical makeup of the TFS deployment.");
    sprintf(infroot->child, TF_CATALOG_INFRASTRUCTURE_ROOT);
    
    if ((dberr = tf_add_node(ctx, infroot)) != TF_ERROR_SUCCESS)
        return dberr;

    servinst = tf_new_node(
        orgroot, 
        TF_CATALOG_TYPE_SERVER_INSTANCE, 
        "Team Foundation Server Instance", 
        NULL);
    dberr = tf_add_node(ctx, servinst);

    if (dberr != TF_ERROR_SUCCESS)
        goto error;

    log_notice("registering services");

    gethostname(hostname, _POSIX_HOST_NAME_MAX);
    snprintf(serveruri, _POSIX_HOST_NAME_MAX * 2, "http://%s:8080/tfs", hostname);
    accmap = tf_new_access_map("public", "Public Access Mapping", serveruri);
    accmap->fdefault = 1;
    dberr = tf_add_access_map(ctx, accmap);
    tf_free_access_map(accmap);
    free(accmap); /* TODO */

    if (dberr != TF_ERROR_SUCCESS)
        goto error;

    tf_service *service = tf_new_service(TF_SERVICE_ID_LOCATION, "LocationService", "Location Service", "Framework");
    tf_set_service_url(service, "/TeamFoundation/Administration/v3.0/LocationService.asmx");
    dberr = tf_add_service(ctx, service);
    /* TODO free service */

    if (dberr != TF_ERROR_SUCCESS)
        goto error;

    tf_service_ref *ref = tf_new_service_ref(&servinst->resource, service, "Location");
    dberr = tf_add_service_ref(ctx, ref);
    /* TODO free ref */

    if (dberr != TF_ERROR_SUCCESS)
        goto error;

    service = tf_new_service(TF_SERVICE_ID_CATALOG, "CatalogService", "Catalog Service", "Framework");
    tf_set_service_url(service, "/TeamFoundation/Administration/v3.0/CatalogService.asmx");
    dberr = tf_add_service(ctx, service);
    /* TODO free service */

    if (dberr != TF_ERROR_SUCCESS)
        goto error;

    ref = tf_new_service_ref(&servinst->resource, service, "Catalog");
    dberr = tf_add_service_ref(ctx, ref);
    /* TODO free ref */

    if (dberr != TF_ERROR_SUCCESS)
        goto error;

    log_notice("registering Team Foundation service host");
    host = tf_new_host(NULL, "TEAM FOUNDATION", dbdsn, &servinst->resource);
    dberr = tf_add_host(ctx, host);
    tf_free_host(host);
    free(host); /* TODO */

    if (dberr != TF_ERROR_SUCCESS)
        goto error;

    log_notice("TFS deployment is initialised");
    pg_release_commit(ctx);
    goto cleanup;

error:
    pg_release_rollback(ctx);
    result = 1;

cleanup:
    orgroot = tf_free_node(orgroot);
    infroot = tf_free_node(infroot);
    servinst = tf_free_node(servinst);

    pg_disconnect();
    log_close();

    return result;
}

