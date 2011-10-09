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
 * @brief   project collection bootstrapping
 *
 * @author  Bob Carroll (bob.carroll@alum.rit.edu)
 */

#include <cs/pcmain.h>
#include <cs/registration.h>
#include <cs/status.h>

#include <gcs/log.h>

#include <tf/catalog.h>
#include <tf/catalogdb.h>
#include <tf/location.h>
#include <tf/servicehost.h>
#include <tf/servicehostdb.h>
#include <tf/webservices.h>
#include <tf/xml.h>

#define MAX_ROUTERS     1024

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
    else if (strcmp(ref->service.type, TF_SERVICE_TYPE_REGISTRATION) == 0)
        registration_service_init(router, prefix, ref);
    else if (strcmp(ref->service.type, TF_SERVICE_TYPE_STATUS) == 0)
        status_service_init(router, prefix, ref);
    else
        gcslog_warn("cannot start unknown service type %s", ref->service.type);
}

/**
 * Project Collection services initialisation.
 *
 * @param prefix    the URI prefix for services.
 * @param instid    team foundation instance ID
 * @param pguser    database connection user ID
 * @param pgpasswd  database connection password
 * @param dbconns   database connection count
 */
void pc_services_init(const char *prefix, const char *instid, const char *pguser, 
    const char *pgpasswd, int dbconns)
{
    tf_host **hostarr = NULL;
    tf_service_ref **refarr = NULL;
    tf_error dberr;
    int i, n;

    if (!prefix || !instid || !pguser || !pgpasswd || dbconns < 1)
        return;

    if (_routers) {
        gcslog_warn("project collection services are already initialised!");
        return;
    }

    dberr = tf_fetch_hosts(instid, &hostarr);

    if (dberr != TF_ERROR_SUCCESS || !hostarr[0]) {
        gcslog_warn("no project collections were found for instance %s", instid);
        hostarr = tf_free_host_array(hostarr);
        return;
    }

    _routers = (SoapRouter **)calloc(MAX_ROUTERS, sizeof(SoapRouter *));

    for (i = 0; hostarr[i]; i++) {
        if (!gcs_pg_connect(hostarr[i]->connstr, pguser, pgpasswd, dbconns, hostarr[i]->id)) {
            gcslog_error("failed to connect to PG");
            continue;
        }

        gcslog_info("initialising project collection services for %s", hostarr[i]->name);

        dberr = tf_fetch_pc_service_refs(hostarr[i]->id, &refarr);

        if (dberr != TF_ERROR_SUCCESS || !refarr[0]) {
            gcslog_warn("failed to retrieve project collection services for %s", hostarr[i]->id);
            refarr = tf_free_service_ref_array(refarr);
            continue;
        }

        for (n = 0; refarr[n]; n++) {
            if (n == MAX_ROUTERS) {
                gcslog_error(
                    "unable to start service because the maximum count was reached (%d)", 
                    MAX_ROUTERS);
                break;
            }

            _start_service(refarr[n], &_routers[n], prefix);
        }

        refarr = tf_free_service_ref_array(refarr);
    }

    hostarr = tf_free_host_array(hostarr);
}

