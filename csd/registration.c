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
 * @brief   registration service
 *
 * @author  Bob Carroll (bob.carroll@alum.rit.edu)
 */

#include <gcs/log.h>

#include <tf/webservices.h>
#include <tf/xml.h>

#include <csd.h>

/**
 * Registration SOAP service handler for GetRegistrationEntries.
 *
 * @param req   SOAP request context
 * @param res   SOAP response context
 *
 * @return H_OK on success
 */
static herror_t _get_registration_entries(SoapCtx *req, SoapCtx *res)
{
    xmlNode *cmd;

    cmd = soap_env_get_method(req->env);
    soap_env_new_with_method(cmd->ns->href, "GetRegistrationEntriesResponse", &res->env);

    xmlNode *arg = tf_xml_find_first(req->env->body, "m", TF_REGISTRATION_NAMESPACE, "//m:toolId/text()");
    int vstfs = (arg && strcmp(arg->content, "vstfs") == 0);

    xmlNode *result = xmlNewChild(res->env->body->children->next, NULL, "GetRegistrationEntriesResult", NULL);
    xmlNode *entry = xmlNewChild(result, NULL, "RegistrationEntry", NULL);

    if (vstfs)
        xmlNewChild(entry, NULL, "Type", "vstfs");
    else
        xmlNewChild(entry, NULL, "Type", "Framework");

    xmlNode *si = xmlNewChild(entry, NULL, "ServiceInterfaces", NULL);

    if (vstfs) {
        xmlNode *si1 = xmlNewChild(si, NULL, "ServiceInterface", NULL);
        xmlNode *si1n = xmlNewChild(si1, NULL, "Name", "RegistrationService");
        xmlNode *si1v = xmlNewChild(si1, NULL, "Value", "/Services/v1.0/Registration.asmx");

        xmlNode *si2 = xmlNewChild(si, NULL, "ServiceInterface", NULL);
        xmlNode *si2n = xmlNewChild(si2, NULL, "Name", "ServerStatus");
        xmlNode *si2v = xmlNewChild(si2, NULL, "Value", "/Services/v1.0/ServerStatus.asmx");

        xmlNode *si3 = xmlNewChild(si, NULL, "ServiceInterface", NULL);
        xmlNode *si3n = xmlNewChild(si3, NULL, "Name", "Eventing");
        xmlNode *si3v = xmlNewChild(si3, NULL, "Value", "/Services/v1.0/EventService.asmx");
    } else {
        xmlNode *si1 = xmlNewChild(si, NULL, "ServiceInterface", NULL);
        xmlNode *si1n = xmlNewChild(si1, NULL, "Name", "LocationService");
        xmlNode *si1v = xmlNewChild(si1, NULL, "Value", "/Services/v3.0/LocationService.asmx");
    }

    xmlNode *db = xmlNewChild(entry, NULL, "Databases", NULL);

    xmlNode *db1 = xmlNewChild(db, NULL, "Database", NULL);
    xmlNode *db1n = xmlNewChild(db1, NULL, "Name", "BIS DB");
    xmlNode *db1d = xmlNewChild(db1, NULL, "DatabaseName", "tfsconfig");
    xmlNode *db1s = xmlNewChild(db1, NULL, "SQLServerName", "GANYMEDE");
    xmlNode *db1c = xmlNewChild(db1, NULL, "ConnectionString", "Data Source=POT;Initial Catalog=Tfs_Foobie;Integrated Security=True");
    xmlNode *db1b = xmlNewChild(db1, NULL, "ExcludeFromBackup", "false");

    xmlNewChild(entry, NULL, "EventTypes", NULL);
    xmlNewChild(entry, NULL, "ArtifactTypes", NULL);

    xmlNode *reas = xmlNewChild(entry, NULL, "RegistrationExtendedAttributes", NULL);

    xmlNode *rea1 = xmlNewChild(reas, NULL, "RegistrationExtendedAttribute", NULL);
    xmlNode *rea1n = xmlNewChild(rea1, NULL, "Name", "InstalledUICulture");
    xmlNode *rea1v = xmlNewChild(rea1, NULL, "Value", "1033");

    xmlNode *rea2 = xmlNewChild(reas, NULL, "RegistrationExtendedAttribute", NULL);
    xmlNode *rea2n = xmlNewChild(rea2, NULL, "Name", "InstanceId");
    xmlNode *rea2v = xmlNewChild(rea2, NULL, "Value", "75ce4f73-3c70-4770-8e95-a4413d2d6a78");

    xmlNode *rea3 = xmlNewChild(reas, NULL, "RegistrationExtendedAttribute", NULL);
    xmlNode *rea3n = xmlNewChild(rea3, NULL, "Name", "ATMachineName");
    xmlNode *rea3v = xmlNewChild(rea3, NULL, "Value", "ZIM");

    xmlNode *rea4 = xmlNewChild(reas, NULL, "RegistrationExtendedAttribute", NULL);
    xmlNode *rea4n = xmlNewChild(rea4, NULL, "Name", "ATNetBIOSName");
    xmlNode *rea4v = xmlNewChild(rea4, NULL, "Value", "ZIM");

    return H_OK;
}

/**
 * Registration service initialisation.
 *
 * @param router    output buffer for the SOAP router
 * @param prefix    the URI prefix for this service
 * @param ref       service reference info
 * @param instid    host instance ID
 */
void registration_service_init(SoapRouter **router, const char *prefix, tf_service_ref *ref,
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
        _get_registration_entries,
        "GetRegistrationEntries",
        TF_REGISTRATION_NAMESPACE);

    gcslog_info("registered registration service %s for host %s", url, instid);
}

