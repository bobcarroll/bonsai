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
 * @brief   location web service
 *
 * @author  Bob Carroll (bob.carroll@alum.rit.edu)
 */

#include <cs/location.h>

#include <gcs/log.h>

#include <tf/webservices.h>
#include <tf/xml.h>

static void _build_auth_node(xmlNode *parent)
{
    xmlNode *authuser = xmlNewChild(parent, NULL, "AuthenticatedUser", "");
    xmlNewProp(authuser, "DisplayName", "Bob Carroll");
    xmlNewProp(authuser, "IsContainer", "false");
    xmlNewProp(authuser, "IsActive", "true");
    xmlNewProp(authuser, "TeamFoundationId", "d00b4f90-df4a-452f-bd54-1d3d001661f8");
    xmlNewProp(authuser, "UniqueUserId", "0");

    xmlNode *descriptor = xmlNewChild(authuser, NULL, "Descriptor", NULL);
    xmlNewProp(descriptor, "identityType", "System.Security.Principal.WindowsIdentity");
    xmlNewProp(descriptor, "identifier", "S-1-5-21-2230180453-4016108734-2163920374-1106");

    xmlNode *attribs = xmlNewChild(authuser, NULL, "Attributes", NULL);
    xmlNode *kvoss1 = xmlNewChild(attribs, NULL, "KeyValueOfStringString", NULL);
    xmlNode *kvoss1k = xmlNewChild(kvoss1, NULL, "Key", "SchemaClassName");
    xmlNode *kvoss1v = xmlNewChild(kvoss1, NULL, "Value", "User");
    xmlNode *kvoss2 = xmlNewChild(attribs, NULL, "KeyValueOfStringString", NULL);
    xmlNode *kvoss2k = xmlNewChild(kvoss2, NULL, "Key", "Description");
    xmlNode *kvoss2v = xmlNewChild(kvoss2, NULL, "Value", NULL);
    xmlNode *kvoss3 = xmlNewChild(attribs, NULL, "KeyValueOfStringString", NULL);
    xmlNode *kvoss3k = xmlNewChild(kvoss3, NULL, "Key", "Domain");
    xmlNode *kvoss3v = xmlNewChild(kvoss3, NULL, "Value", "REDMOND");
    xmlNode *kvoss4 = xmlNewChild(attribs, NULL, "KeyValueOfStringString", NULL);
    xmlNode *kvoss4k = xmlNewChild(kvoss4, NULL, "Key", "Account");
    xmlNode *kvoss4v = xmlNewChild(kvoss4, NULL, "Value", "bob.carroll");
    xmlNode *kvoss5 = xmlNewChild(attribs, NULL, "KeyValueOfStringString", NULL);
    xmlNode *kvoss5k = xmlNewChild(kvoss5, NULL, "Key", "DN");
    xmlNode *kvoss5v = xmlNewChild(kvoss5, NULL, "Value", "CN=Bob Carroll,CN=Users,DC=redmond,DC=unifieddiff,DC=net");
    xmlNode *kvoss6 = xmlNewChild(attribs, NULL, "KeyValueOfStringString", NULL);
    xmlNode *kvoss6k = xmlNewChild(kvoss6, NULL, "Key", "Mail");
    xmlNode *kvoss6v = xmlNewChild(kvoss6, NULL, "Value", NULL);
    xmlNode *kvoss7 = xmlNewChild(attribs, NULL, "KeyValueOfStringString", NULL);
    xmlNode *kvoss7k = xmlNewChild(kvoss7, NULL, "Key", "SpecialType");
    xmlNode *kvoss7v = xmlNewChild(kvoss7, NULL, "Value", "Generic");

    xmlNode *members = xmlNewChild(authuser, NULL, "Members", NULL);
    xmlNode *memberof = xmlNewChild(authuser, NULL, "MemberOf", NULL);
}

static void _build_authz_node(xmlNode *parent)
{
    xmlNode *authzuser = xmlNewChild(parent, NULL, "AuthorizedUser", NULL);
    xmlNewProp(authzuser, "DisplayName", "Bob Carroll");
    xmlNewProp(authzuser, "IsContainer", "false");
    xmlNewProp(authzuser, "IsActive", "true");
    xmlNewProp(authzuser, "TeamFoundationId", "d00b4f90-df4a-452f-bd54-1d3d001661f8");
    xmlNewProp(authzuser, "UniqueUserId", "0");

    xmlNode *autzdesc = xmlNewChild(authzuser, NULL, "Descriptor", NULL);
    xmlNewProp(autzdesc, "identityType", "System.Security.Principal.WindowsIdentity");
    xmlNewProp(autzdesc, "identifier", "S-1-5-21-2230180453-4016108734-2163920374-1106");

    xmlNode *authzattrs = xmlNewChild(authzuser, NULL, "Attributes", NULL);
    xmlNode *authzkvoss1 = xmlNewChild(authzattrs, NULL, "KeyValueOfStringString", NULL);
    xmlNode *authzkvoss1k = xmlNewChild(authzkvoss1, NULL, "Key", "SchemaClassName");
    xmlNode *authzkvoss1v = xmlNewChild(authzkvoss1, NULL, "Value", "User");
    xmlNode *authzkvoss2 = xmlNewChild(authzattrs, NULL, "KeyValueOfStringString", NULL);
    xmlNode *authzkvoss2k = xmlNewChild(authzkvoss2, NULL, "Key", "Description");
    xmlNode *authzkvoss2v = xmlNewChild(authzkvoss2, NULL, "Value", NULL);
    xmlNode *authzkvoss3 = xmlNewChild(authzattrs, NULL, "KeyValueOfStringString", NULL);
    xmlNode *authzkvoss3k = xmlNewChild(authzkvoss3, NULL, "Key", "Domain");
    xmlNode *authzkvoss3v = xmlNewChild(authzkvoss3, NULL, "Value", "REDMOND");
    xmlNode *authzkvoss4 = xmlNewChild(authzattrs, NULL, "KeyValueOfStringString", NULL);
    xmlNode *authzkvoss4k = xmlNewChild(authzkvoss4, NULL, "Key", "Account");
    xmlNode *authzkvoss4v = xmlNewChild(authzkvoss4, NULL, "Value", "bob.carroll");
    xmlNode *authzkvoss5 = xmlNewChild(authzattrs, NULL, "KeyValueOfStringString", NULL);
    xmlNode *authzkvoss5k = xmlNewChild(authzkvoss5, NULL, "Key", "DN");
    xmlNode *authzkvoss5v = xmlNewChild(authzkvoss5, NULL, "Value", "CN=Bob Carroll,CN=Users,DC=redmond,DC=unifieddiff,DC=net");
    xmlNode *authzkvoss6 = xmlNewChild(authzattrs, NULL, "KeyValueOfStringString", NULL);
    xmlNode *authzkvoss6k = xmlNewChild(authzkvoss6, NULL, "Key", "Mail");
    xmlNode *authzkvoss6v = xmlNewChild(authzkvoss6, NULL, "Value", NULL);
    xmlNode *authzkvoss7 = xmlNewChild(authzattrs, NULL, "KeyValueOfStringString", NULL);
    xmlNode *authzkvoss7k = xmlNewChild(authzkvoss7, NULL, "Key", "SpecialType");
    xmlNode *authzkvoss7v = xmlNewChild(authzkvoss7, NULL, "Value", "Generic");

    xmlNode *authzmembers = xmlNewChild(authzuser, NULL, "Members", NULL);
    xmlNode *authzmemberof = xmlNewChild(authzuser, NULL, "MemberOf", NULL);
}

static void _build_service_list(xmlNode *parent, int inclservices)
{
    xmlNode *locdata = xmlNewChild(parent, NULL, "LocationServiceData", NULL);
    xmlNewProp(locdata, "DefaultAccessMappingMoniker", "PublicAccessMapping");
    xmlNewProp(locdata, "LastChangeId", "2565");
    xmlNewProp(locdata, "ClientCacheFresh", "true");
    xmlNewProp(locdata, "AccessPointsDoNotIncludeWebAppRelativeDirectory", "false");

    if (inclservices) {
        xmlNode *servicedefs = xmlNewChild(locdata, NULL, "ServiceDefinitions", NULL);

        xmlNode *sd1 = xmlNewChild(servicedefs, NULL, "ServiceDefinition", NULL);
        xmlNewProp(sd1, "serviceType", "CatalogService");
        xmlNewProp(sd1, "identifier", "c2f9106f-127a-45b7-b0a3-e0ad8239a2a7");
        xmlNewProp(sd1, "displayName", "Catalog Service");
        xmlNewProp(sd1, "relativeToSetting", "0");
        xmlNewProp(sd1, "relativePath", "/TeamFoundation/Administration/v3.0/CatalogService.asmx");
        xmlNewProp(sd1, "description", "Catalog Service for Visual Studio Team Foundation Server.");
        xmlNewProp(sd1, "toolId", "Framework");

        xmlNode *sd2 = xmlNewChild(servicedefs, NULL, "ServiceDefinition", NULL);
        xmlNewProp(sd2, "serviceType", "LocationService");
        xmlNewProp(sd2, "identifier", "bf9cf1d0-24ac-4d35-aeca-6cd18c69c1fe");
        xmlNewProp(sd2, "displayName", "Location Service");
        xmlNewProp(sd2, "relativeToSetting", "0");
        xmlNewProp(sd2, "relativePath", "/TeamFoundation/Administration/v3.0/LocationService.asmx");
        xmlNewProp(sd2, "description", "Location Service for Visual Studio Team Foundation Server.");
        xmlNewProp(sd2, "toolId", "Framework");

        xmlNode *sd3 = xmlNewChild(servicedefs, NULL, "ServiceDefinition", NULL);
        xmlNewProp(sd3, "serviceType", "LocationService");
        xmlNewProp(sd3, "identifier", "75ce4f73-3c70-4770-8e95-a4413d2d6a78");
        xmlNewProp(sd3, "displayName", "Location Service");
        xmlNewProp(sd3, "relativeToSetting", "0");
        xmlNewProp(sd3, "relativePath", "/Foobie/Services/v3.0/LocationService.asmx");
        xmlNewProp(sd3, "description", "Location Service for Visual Studio Team Foundation Server.");
        xmlNewProp(sd3, "toolId", "Framework");

        xmlNode *accessmappings = xmlNewChild(locdata, NULL, "AccessMappings", NULL);

        xmlNode *am1 = xmlNewChild(accessmappings, NULL, "AccessMapping", NULL);
        xmlNewProp(am1, "DisplayName", "Public Access Mapping");
        xmlNewProp(am1, "Moniker", "PublicAccessMapping");
        xmlNewProp(am1, "AccessPoint", "http://zim.unifieddiff.net:8080/tfs");

        xmlNode *am2 = xmlNewChild(accessmappings, NULL, "AccessMapping", NULL);
        xmlNewProp(am2, "DisplayName", "Server Access Mapping");
        xmlNewProp(am2, "Moniker", "ServerAccessMapping");
        xmlNewProp(am2, "AccessPoint", "http://localhost:8080/tfs");
    }
}

/**
 * Location SOAP service handler for Connect.
 *
 * @param req   SOAP request context
 * @param res   SOAP response context
 *
 * @return H_OK on success
 */
static herror_t _connect(SoapCtx *req, SoapCtx *res)
{
    xmlNode *cmd;
    xmlNode *arg;
    xmlNode *body;
    xmlNode *connresult;
    int inclservices;
    char *lastchgid;

    cmd = soap_env_get_method(req->env);
    soap_env_new_with_method(cmd->ns->href, "ConnectResponse", &res->env);
    body = req->env->body;

    arg = tf_xml_find_first(body, "m", TF_DEFAULT_NAMESPACE, "//m:Connect/m:connectOptions");
    if (arg != NULL && arg->children != NULL)
        inclservices = (strcmp(arg->children->content, "1") == 0);

    arg = tf_xml_find_first(body, "m", TF_DEFAULT_NAMESPACE, "//m:Connect/lastChangeId");
    if (arg != NULL && arg->children != NULL)
        lastchgid = arg->children->content;

    connresult = xmlNewChild(res->env->body->children->next, NULL, "ConnectResult", NULL);
    xmlNewProp(connresult, "InstanceId", "bed36b22-8b2a-464b-8702-8df2dbc413fe");
    xmlNewProp(connresult, "CatalogResourceId", "e06e112f-0c57-4611-b88d-49a4eff6f52a");

    _build_auth_node(connresult);
    _build_authz_node(connresult);
    _build_service_list(connresult, inclservices);

    return H_OK;
}

static int _auth_ntlm(SoapEnv *env, const char *user, const char *passwd)
{
    return 0;
}

/**
 * Location service initialisation.
 *
 * @param router    output buffer for the SOAP router
 * @param prefix    the URI prefix for this service
 */
void location_service_init(SoapRouter **router, const char *prefix)
{
    char url[1024];

    (*router) = soap_router_new();
    soap_router_register_security(*router, (httpd_auth)_auth_ntlm);

    sprintf(url, "%s%s", prefix != NULL ? prefix : "", TF_LOCATION_SERVICE_ENDPOINT);
    soap_server_register_router(*router, url);

    soap_router_register_service(
        *router,
        _connect,
        "Connect",
        TF_DEFAULT_NAMESPACE);

    gcslog_info("registered location services");
}

