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
 * @brief   team foundation bootstrapping
 *
 * @author  Bob Carroll (bob.carroll@alum.rit.edu)
 */

#include <log.h>

#include <tf/catalog.h>
#include <tf/location.h>
#include <tf/servicehost.h>
#include <tf/webservices.h>
#include <tf/xml.h>

#include <csd.h>

static SoapRouter **_routers = NULL;

/**
 * Start the given SOAP service.
 *
 * @param service   service to start
 * @param router    SOAP router
 * @param prefix    service path prefix
 * @param instid    host instance ID
 */
static void _start_service(tf_service_ref *ref, SoapRouter **router, const char *prefix,
    const char *instid)
{
    if (strcmp(ref->service.type, TF_SERVICE_TYPE_LOCATION) == 0)
        location_service_init(router, prefix, ref, instid);
    else if (strcmp(ref->service.type, TF_SERVICE_TYPE_CATALOG) == 0)
        catalog_service_init(router, prefix, ref, instid);
    else
        gcslog_warn("cannot start unknown service type %s", ref->service.type);
}

/**
 * Team Foundation services initialisation. If the return value
 * is NULL then initialisation has failed.
 *
 * @param prefix    the URI prefix for services.
 *
 * @return the team foundation instance ID
 */
char *core_services_init(const char *prefix)
{
    pgctx *ctx;
    tf_node **nodearr = NULL;
    tf_service_ref **refarr = NULL;
    tf_host **hostarr = NULL;
    tf_error dberr;
    char *result = NULL;
    int i;

    if (!prefix) {
        gcslog_error("prefix cannot be NULL");
        return NULL;
    }

    ctx = gcs_pgctx_acquire(NULL);
    dberr = tf_fetch_hosts(ctx, NULL, &hostarr);

    if (dberr != TF_ERROR_SUCCESS || !hostarr[0]) {
        gcslog_error("no team foundation instances were found!");
        hostarr = tf_free_host_array(hostarr);
        gcs_pgctx_release(ctx);
        return NULL;
    }

    result = strdup(hostarr[0]->id); /* TODO support more than one host */
    gcslog_debug("team foundation instance ID is %s", result);
    hostarr = tf_free_host_array(hostarr);

    if (_routers) {
        gcslog_warn("core services are already initialised!");
        gcs_pgctx_release(ctx);
        return result;
    }

    gcslog_info("initialising team foundation services");

    dberr = tf_query_single_node(
        ctx,
        TF_CATALOG_ORGANIZATION_ROOT,
        TF_CATALOG_TYPE_SERVER_INSTANCE,
        &nodearr);

    if (dberr != TF_ERROR_SUCCESS || !nodearr[0]) {
        gcslog_warn("failed to retrieve team foundation catalog nodes");
        nodearr = tf_free_node_array(nodearr);
        gcs_pgctx_release(ctx);
        free(result);
        return NULL;
    }

    dberr = tf_fetch_service_refs(ctx, nodearr, &refarr);
    nodearr = tf_free_node_array(nodearr);

    if (dberr != TF_ERROR_SUCCESS || !refarr[0]) {
        gcslog_warn("failed to retrieve team foundation services");
        refarr = tf_free_service_ref_array(refarr);
        gcs_pgctx_release(ctx);
        free(result);
        return NULL;
    }

    for (i = 0; refarr[i]; i++);
    _routers = (SoapRouter **)calloc(i, sizeof(SoapRouter *));

    for (i = 0; refarr[i]; i++)
        _start_service(refarr[i], &_routers[i], prefix, result);

    refarr = tf_free_service_ref_array(refarr);
    gcs_pgctx_release(ctx);

    return result;
}

