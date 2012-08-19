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

    service = tf_new_service(
        TF_SERVICE_REGISTRATION_ID, 
        TF_SERVICE_REGISTRATION_TYPE, 
        TF_SERVICE_REGISTRATION_NAME, 
        TF_CATALOG_TOOL_FRAMEWORK);
    tf_set_service_url(service, TF_REGISTRATION_SERVICE_ENDPOINT, TF_SERVICE_RELTO_CONTEXT);
    dberr = tf_add_service(ctx, service);
    service = tf_free_service(service);

    if (dberr != TF_ERROR_SUCCESS)
        return TF_ERROR_PG_FAILURE;

    service = tf_new_service(
        TF_SERVICE_STATUS_ID, 
        TF_SERVICE_STATUS_TYPE, 
        TF_SERVICE_STATUS_NAME, 
        TF_CATALOG_TOOL_FRAMEWORK);
    tf_set_service_url(service, TF_SERVER_STATUS_SERVICE_ENDPOINT, TF_SERVICE_RELTO_CONTEXT);
    dberr = tf_add_service(ctx, service);
    service = tf_free_service(service);

    if (dberr != TF_ERROR_SUCCESS)
        return TF_ERROR_PG_FAILURE;

    service = tf_new_service(
        TF_SERVICE_AUTHORIZATION3_ID, 
        TF_SERVICE_AUTHORIZATION3_TYPE, 
        TF_SERVICE_AUTHORIZATION3_NAME, 
        TF_CATALOG_TOOL_FRAMEWORK);
    tf_set_service_url(service, TF_AUTHORIZATION3_SERVICE_ENDPOINT, TF_SERVICE_RELTO_CONTEXT);
    dberr = tf_add_service(ctx, service);
    service = tf_free_service(service);

    if (dberr != TF_ERROR_SUCCESS)
        return TF_ERROR_PG_FAILURE;

    service = tf_new_service(
        TF_SERVICE_COMMON_STRUCT_ID, 
        TF_SERVICE_COMMON_STRUCT_TYPE, 
        TF_SERVICE_COMMON_STRUCT_NAME, 
        TF_CATALOG_TOOL_FRAMEWORK);
    tf_set_service_url(service, TF_COMMON_STRUCT_SERVICE_ENDPOINT, TF_SERVICE_RELTO_CONTEXT);
    dberr = tf_add_service(ctx, service);
    service = tf_free_service(service);

    if (dberr != TF_ERROR_SUCCESS)
        return TF_ERROR_PG_FAILURE;

    service = tf_new_service(
        TF_SERVICE_PROCESS_TEMPL_ID, 
        TF_SERVICE_PROCESS_TEMPL_TYPE, 
        TF_SERVICE_PROCESS_TEMPL_NAME, 
        TF_CATALOG_TOOL_FRAMEWORK);
    tf_set_service_url(service, TF_PROCESS_TEMPL_SERVICE_ENDPOINT, TF_SERVICE_RELTO_CONTEXT);
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
 * @param amuri     project collection access mapping URI
 * @param tfctx     Team Foundation configuration database context
 * @param tfhost    Team Foundation host being attach to
 *
 * @return TF_ERROR_SUCCESS or an error code
 */
tf_error tf_attach_collection(pgctx *pcctx, const char *name, const char *amuri, pgctx *tfctx, tf_host *tfhost)
{
    char path[1024];
    tf_access_map *accmap = NULL;
    tf_node *instnode = NULL;
    tf_node *colnode = NULL;
    tf_host *pchost = NULL;
    tf_service *service = NULL;
    tf_service_ref *ref = NULL;
    tf_property *instprop = NULL;
    tf_error dberr;

    if (!pcctx || !name || !name[0] || !amuri || !amuri[0] || !tfctx || !tfhost)
        return TF_ERROR_BAD_PARAMETER;

    log_notice("attaching project collection %s to instance %s", name, tfhost->id);

    accmap = tf_new_access_map(TF_ACCESSMAP_PUBLIC_MONIKER, TF_ACCESSMAP_PUBLIC_DISPLNAME, amuri);
    dberr = tf_add_access_map(pcctx, accmap);

    if (dberr != TF_ERROR_SUCCESS)
        return TF_ERROR_PG_FAILURE;

    dberr = tf_set_default_access_map(pcctx, accmap);
    accmap = tf_free_access_map(accmap);

    if (dberr != TF_ERROR_SUCCESS)
        return TF_ERROR_PG_FAILURE;

    dberr = tf_fetch_instance_node(tfctx, tfhost->id, &instnode);

    if (dberr == TF_ERROR_NOT_FOUND) {
        log_error("could not find instance catalog node");
        return TF_ERROR_INTERNAL;
    } else if (dberr != TF_ERROR_SUCCESS) {
        instnode = tf_free_node(instnode);
        return TF_ERROR_PG_FAILURE;
    }

    colnode = tf_new_node(instnode, TF_CATALOG_TYPE_TEAM_PRJ_COLLECTION, name, NULL);
    instnode = tf_free_node(instnode);

    if (!colnode) {
        log_error("failed to create new catalog node for project collection");
        return TF_ERROR_INTERNAL;
    }

    dberr = tf_add_node(tfctx, colnode);

    if (dberr != TF_ERROR_SUCCESS) {
        colnode = tf_free_node(colnode);
        return TF_ERROR_PG_FAILURE;
    }

    pchost = tf_new_host(name, pcctx->dsn);
    tf_set_host_vdir(pchost, name);

    if (!pchost) {
        log_error("failed to create new service host for project collection");
        colnode = tf_free_node(colnode);
        return TF_ERROR_INTERNAL;
    }

    dberr = tf_add_host(tfctx, pchost);

    if (dberr != TF_ERROR_SUCCESS) {
        colnode = tf_free_node(colnode);
        pchost = tf_free_host(pchost);
        return TF_ERROR_PG_FAILURE;
    }

    instprop = tf_new_property(
        TF_PROPERTY_INSTANCE_ID_ID,
        colnode->resource.propertyid,
        pchost->id);

    if (!instprop) {
        log_error("failed to create instance property for collection node");
        colnode = tf_free_node(colnode);
        pchost = tf_free_host(pchost);
        return TF_ERROR_INTERNAL;
    }

    dberr = tf_add_property(tfctx, instprop);
    instprop = tf_free_property(instprop);

    if (dberr != TF_ERROR_SUCCESS) {
        colnode = tf_free_node(colnode);
        pchost = tf_free_host(pchost);
        return TF_ERROR_PG_FAILURE;
    }

    service = tf_new_service(
        pchost->id, 
        TF_SERVICE_LOCATION_TYPE, 
        TF_SERVICE_LOCATION_NAME, 
        TF_CATALOG_TOOL_FRAMEWORK);
    pchost = tf_free_host(pchost);

    if (!service) {
        log_error("failed to create location service definition for collection");
        colnode = tf_free_node(colnode);
        return TF_ERROR_INTERNAL;
    }

    snprintf(path, 1024, "/%s%s", name, TF_LOCATION_SERVICE_PC_ENDPOINT);
    tf_set_service_url(service, path, TF_SERVICE_RELTO_CONTEXT);
    dberr = tf_add_service(tfctx, service);

    if (dberr != TF_ERROR_SUCCESS) {
        colnode = tf_free_node(colnode);
        service = tf_free_service(service);
        return TF_ERROR_PG_FAILURE;
    }

    ref = tf_new_service_ref(&colnode->resource, service, "Location");
    colnode = tf_free_node(colnode);
    service = tf_free_service(service);

    if (!ref) {
        log_error("failed to create Location catalog service reference for collection");
        return TF_ERROR_INTERNAL;
    }

    dberr = tf_add_service_ref(tfctx, ref);
    ref = tf_free_service_ref(ref);

    if (dberr != TF_ERROR_SUCCESS)
        return TF_ERROR_PG_FAILURE;

    return TF_ERROR_SUCCESS;
}

