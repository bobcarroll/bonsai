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
#include <pgcommon.h>

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
 * @param ref       reference to service to start
 * @param router    SOAP router
 * @param prefix    service path prefix
 * @param instid    host instance ID
 */
static void _start_service(tf_service_ref *ref, SoapRouter **router, const char *prefix,
    const char *instid)
{
    if (strcmp(ref->service.type, TF_SERVICE_LOCATION_TYPE) == 0)
        location_service_init(router, prefix, ref->service.relpath, instid);
    else if (strcmp(ref->service.type, TF_SERVICE_CATALOG_TYPE) == 0)
        catalog_service_init(router, prefix, ref->service.relpath, instid);
    else
        log_warn("cannot start unknown service type %s", ref->service.type);
}

/**
 * Team Foundation services initialisation. If the return value
 * is NULL then initialisation has failed.
 *
 * @param prefix    the URI prefix for services.
 *
 * @return true on success, false otherwise
 */
int core_services_init(const char *prefix)
{
    pgctx *ctx;
    tf_node **nodearr = NULL;
    tf_service_ref **refarr = NULL;
    tf_host *host = NULL;
    tf_error dberr;
    int i;

    if (!prefix) {
        log_error("prefix cannot be NULL");
        return 0;
    }

    ctx = pg_context_acquire(NULL);
    if (!ctx) {
        log_critical("failed to obtain PG context!");
        return 0;
    }

    dberr = tf_fetch_single_host(ctx, "TEAM FOUNDATION", 1, &host);

    if (dberr != TF_ERROR_SUCCESS) {
        log_error("no team foundation instances were found!");
        pg_context_release(ctx);
        return 0;
    }

    log_debug("team foundation instance ID is %s", host->id);
    pg_context_retag_default(host->id);

    if (_routers) {
        log_warn("core services are already initialised!");
        host = tf_free_host(host);
        pg_context_release(ctx);
        return 1;
    }

    log_info("initialising team foundation services");

    dberr = tf_query_tree(
        ctx,
        TF_CATALOG_ORGANIZATION_ROOT,
        TF_CATALOG_TYPE_SERVER_INSTANCE,
        &nodearr);

    if (dberr != TF_ERROR_SUCCESS || !nodearr[0]) {
        log_warn("failed to retrieve team foundation catalog nodes");
        nodearr = tf_free_node_array(nodearr);
        host = tf_free_host(host);
        pg_context_release(ctx);
        return 0;
    }

    dberr = tf_fetch_service_refs(ctx, nodearr, &refarr);
    nodearr = tf_free_node_array(nodearr);

    if (dberr != TF_ERROR_SUCCESS || !refarr[0]) {
        log_warn("failed to retrieve team foundation services");
        refarr = tf_free_service_ref_array(refarr);
        host = tf_free_host(host);
        pg_context_release(ctx);
        return 0;
    }

    for (i = 0; refarr[i]; i++)
        ;
    _routers = (SoapRouter **)calloc(i, sizeof(SoapRouter *));

    for (i = 0; refarr[i]; i++)
        _start_service(refarr[i], &_routers[i], prefix, host->id);

    refarr = tf_free_service_ref_array(refarr);
    host = tf_free_host(host);
    pg_context_release(ctx);

    return 1;
}

