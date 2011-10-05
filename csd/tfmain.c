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

#include <cs/tfmain.h>
#include <cs/catalog.h>
#include <cs/location.h>

#include <gcs/log.h>

#include <tf/catalog.h>
#include <tf/catalogdb.h>
#include <tf/location.h>
#include <tf/webservices.h>
#include <tf/xml.h>

static SoapRouter **_routers = NULL;

/**
 * Start the given SOAP service.
 *
 * @param service   service to start
 * @param router    SOAP router
 * @param prefix    service path prefix
 */
static void _start_service(tf_service_ref *ref, SoapRouter **router, const char *prefix)
{
    if (strcmp(ref->service.type, TF_SERVICE_TYPE_LOCATION) == 0)
        location_service_init(router, prefix, ref);
    else if (strcmp(ref->service.type, TF_SERVICE_TYPE_CATALOG) == 0)
        catalog_service_init(router, prefix, ref);
    else
        gcslog_warn("cannot start unknown service type %s", ref->service.type);
}

/**
 * Team Foundation services initialisation.
 *
 * @param prefix    the URI prefix for services.
 */
void core_services_init(const char *prefix)
{
    tf_node **nodearr = NULL;
    tf_service_ref **refarr = NULL;
    tf_error dberr;
    int i;

    gcslog_info("initialising team foundation services");

    dberr = tf_query_single_node(
        TF_CATALOG_ORGANIZATION_ROOT,
        TF_CATALOG_TYPE_SERVER_INSTANCE,
        &nodearr);

    if (dberr != TF_ERROR_SUCCESS || nodearr[0] == NULL) {
        gcslog_warn("failed to retrieve team foundation catalog nodes");
        nodearr = tf_free_node_array(nodearr);
        return;
    }

    dberr = tf_fetch_service_refs(nodearr, &refarr);
    nodearr = tf_free_node_array(nodearr);

    if (dberr != TF_ERROR_SUCCESS || refarr[0] == NULL) {
        gcslog_warn("failed to retrieve team foundation services");
        refarr = tf_free_service_ref_array(refarr);
        return;
    }

    for (i = 0; refarr[i] != NULL; i++);
    _routers = (SoapRouter **)calloc(i, sizeof(SoapRouter *));

    for (i = 0; refarr[i] != NULL; i++)
        _start_service(refarr[i], &_routers[i], prefix);

    refarr = tf_free_service_ref_array(refarr);
}

