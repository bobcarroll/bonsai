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
 * @brief   server status service
 *
 * @author  Bob Carroll (bob.carroll@alum.rit.edu)
 */

#include <gcs/log.h>

#include <tf/webservices.h>
#include <tf/xml.h>

#include <csd.h>

/**
 * Server Status SOAP service handler for CheckAuthentication.
 *
 * @param req   SOAP request context
 * @param res   SOAP response context
 *
 * @return H_OK on success
 */
static herror_t _check_authentication(SoapCtx *req, SoapCtx *res)
{
    xmlNode *cmd;

    cmd = soap_env_get_method(req->env);
    soap_env_new_with_method(cmd->ns->href, "CheckAuthenticationResponse", &res->env);

    /* TODO */
    xmlNode *result = xmlNewChild(res->env->body->children->next, NULL, "CheckAuthenticationResult", "REDMOND\\bob");

    return H_OK;
}

/**
 * Server Status SOAP service handler for GetServerStatus.
 *
 * @param req   SOAP request context
 * @param res   SOAP response context
 *
 * @return H_OK on success
 */
static herror_t _get_server_status(SoapCtx *req, SoapCtx *res)
{
    xmlNode *cmd;

    cmd = soap_env_get_method(req->env);
    soap_env_new_with_method(cmd->ns->href, "GetServerStatusResponse", &res->env);

    /* TODO */
    xmlNode *result = xmlNewChild(res->env->body->children->next, NULL, "GetServerStatusResult", NULL);

    return H_OK;
}

/**
 * Server Status service initialisation.
 *
 * @param router    output buffer for the SOAP router
 * @param prefix    the URI prefix for this service
 * @param ref       service reference info
 * @param instid    host instance ID
 */
void status_service_init(SoapRouter **router, const char *prefix, tf_service_ref *ref,
    const char *instid)
{
    char url[1024];

    (*router) = soap_router_new();
    soap_router_register_security(*router, NTLM_SPNEGO);
    soap_router_set_tag(*router, instid);

    sprintf(url, "%s%s", prefix ? prefix : "", ref->service.relpath);
    soap_server_register_router(*router, url);

    soap_router_register_service(
        *router,
        _check_authentication,
        "CheckAuthentication",
        TF_SERVER_STATUS_NAMESPACE);
    soap_router_register_service(
        *router,
        _get_server_status,
        "GetServerStatus",
        TF_SERVER_STATUS_NAMESPACE);

    gcslog_info("registered server status service %s for host %s", url, instid);
}

