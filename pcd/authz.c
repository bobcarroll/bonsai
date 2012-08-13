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
 * @brief   authorization service
 *
 * @author  Bob Carroll (bob.carroll@alum.rit.edu)
 */

#include <log.h>

#include <tf/webservices.h>
#include <tf/xml.h>

#include <pcd.h>

/**
 * Authorization SOAP service handler for CheckPermission
 *
 * @param req   SOAP request context
 * @param res   SOAP response context
 *
 * @return H_OK on success
 */
static herror_t _check_permission(SoapCtx *req, SoapCtx *res)
{
    xmlNode *cmd;

    cmd = soap_env_get_method(req->env);
    soap_env_new_with_method(cmd->ns->href, "CheckPermissionResponse", &res->env);

    /* TODO actually check permissions */

    return H_OK;
}

/**
 * Authorization service initialisation.
 *
 * @param router    output buffer for the SOAP router
 * @param prefix    the URI prefix for this service
 * @param relpath   relative path of the service
 * @param instid    host instance ID
 * @param version   authorization service version
 */
void authz_service_init(SoapRouter **router, const char *prefix, const char *relpath,
    const char *instid, int version)
{
    char url[1024];

    (*router) = soap_router_new();
    soap_router_register_security(*router, NTLM_SPNEGO);
    soap_router_set_tag(*router, instid);

    sprintf(url, "%s%s", prefix ? prefix : "", relpath);
    soap_server_register_router(*router, url);

    soap_router_register_service(
        *router,
        _check_permission,
        "CheckPermission",
        TF_AUTHORIZATION3_NAMESPACE);

    log_info("registered authorization service %s for host %s", url, instid);
}

