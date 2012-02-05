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
 * @brief   project collection functions
 *
 * @author  Bob Carroll (bob.carroll@alum.rit.edu)
 */

#include <string.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>

#include <log.h>

#include <tf/collection.h>
#include <tf/schema.h>
#include <tf/catalog.h>
#include <tf/location.h>
#include <tf/webservices.h>

/**
 * Creates a new project collection with the given name.
 *
 * @param ctx       current database context
 *
 * @return TF_ERROR_SUCCESS or an error code
 */
tf_error tf_create_collection(pgctx *ctx)
{
    tf_service *service = NULL;
    tf_error dberr;

    if (!ctx)
        return TF_ERROR_BAD_PARAMETER;

    if ((dberr = tf_init_pcdb(ctx)) != TF_ERROR_SUCCESS)
        return TF_ERROR_PG_FAILURE;

    log_info("registering project collection services");

    service = tf_new_service(
        TF_SERVICE_LOCATION_ID, 
        TF_SERVICE_LOCATION_TYPE, 
        TF_SERVICE_LOCATION_NAME, 
        TF_CATALOG_TOOL_FRAMEWORK);
    tf_set_service_url(service, TF_LOCATION_SERVICE_PC_ENDPOINT, TF_SERVICE_RELTO_CONTEXT);
    dberr = tf_add_service(ctx, service);
    service = tf_free_service(service);

    if (dberr != TF_ERROR_SUCCESS)
        return TF_ERROR_PG_FAILURE;

    service = tf_new_service(
        TF_SERVICE_LOCATION2_ID, 
        TF_SERVICE_LOCATION_TYPE, 
        TF_SERVICE_LOCATION_NAME, 
        TF_CATALOG_TOOL_FRAMEWORK);
    tf_set_service_url(service, TF_LOCATION_SERVICE_ENDPOINT, TF_SERVICE_RELTO_FULLY_QUALIFIED);
    dberr = tf_add_service(ctx, service);
    service = tf_free_service(service);

    if (dberr != TF_ERROR_SUCCESS)
        return TF_ERROR_PG_FAILURE;

    return TF_ERROR_SUCCESS;
}

/**
 * Attaches a project collection to a Team Foundation instance.
 *
 * @param pcctx     project collection database context
 * @param name      project collection name
 * @param tfctx     Team Foundation configuration database context
 * @param tfhost    Team Foundation host being attach to
 *
 * @return TF_ERROR_SUCCESS or an error code
 */
tf_error tf_attach_collection(pgctx *pcctx, const char *name, pgctx *tfctx, tf_host *tfhost)
{
    char hostname[_POSIX_HOST_NAME_MAX];
    char serveruri[_POSIX_HOST_NAME_MAX * 2];
    char **idarr = NULL;
    tf_access_map *accmap = NULL;
    tf_node **nodearr = NULL;
    tf_node *colnode = NULL;
    tf_host *pchost = NULL;
    tf_error dberr;

    if (!pcctx || !name || !name[0] || !tfctx || !tfhost)
        return TF_ERROR_BAD_PARAMETER;

    log_notice("attaching project collection %s to instance %s", name, tfhost->id);

    gethostname(hostname, _POSIX_HOST_NAME_MAX);
    snprintf(serveruri, _POSIX_HOST_NAME_MAX * 2, "http://%s:8080/tfs", hostname);
    accmap = tf_new_access_map("public", "Public Access Mapping", serveruri);
    accmap->fdefault = 1;
    dberr = tf_add_access_map(pcctx, accmap);
    accmap = tf_free_access_map(accmap);

    if (dberr != TF_ERROR_SUCCESS)
        return TF_ERROR_PG_FAILURE;

    idarr = (char **)calloc(2, sizeof(char *));
    idarr[0] = strdup(tfhost->resource);

    dberr = tf_fetch_resources(tfctx, (const char * const *)idarr, 0, &nodearr);

    free(idarr[0]);
    free(idarr);
    idarr = NULL;

    if (dberr != TF_ERROR_SUCCESS) {
        nodearr = tf_free_node_array(nodearr);
        return TF_ERROR_PG_FAILURE;
    }

    if (!nodearr[0]) {
        log_error("could not find instance catalog node");
        nodearr = tf_free_node_array(nodearr);
        return TF_ERROR_INTERNAL;
    }

    colnode = tf_new_node(nodearr[0], TF_CATALOG_TYPE_TEAM_PRJ_COLLECTION, name, NULL);
    nodearr = tf_free_node_array(nodearr);

    if (!colnode) {
        log_error("failed to create new catalog node for project collection");
        return TF_ERROR_INTERNAL;
    }

    dberr = tf_add_node(tfctx, colnode);

    if (dberr != TF_ERROR_SUCCESS) {
        colnode = tf_free_node(colnode);
        return TF_ERROR_PG_FAILURE;
    }

    pchost = tf_new_host(tfhost, name, pcctx->dsn, &colnode->resource);

    if (!pchost) {
        log_error("failed to create new service host for project collection");
        colnode = tf_free_node(colnode);
        return TF_ERROR_INTERNAL;
    }

    colnode = tf_free_node(colnode);
    dberr = tf_add_host(tfctx, pchost);
    pchost = tf_free_host(pchost);

    if (dberr != TF_ERROR_SUCCESS)
        return TF_ERROR_PG_FAILURE;

    return TF_ERROR_SUCCESS;
}

