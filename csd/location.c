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

#include <tf/catalog.h>
#include <tf/location.h>
#include <tf/locationdb.h>
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

/**
 * Appends a LocationServiceData node to the given parent node.
 *
 * @param parent        the parent node to attach to
 * @param svcarr        service definitions array
 * @param accmaparr     access mapping array
 * @param inclall       flag to include all service definitions
 */
static void _append_location_data(xmlNode *parent, tf_service **svcarr, tf_access_map **accmaparr, int inclall)
{
    int i;

    char *defmoniker = tf_find_default_moniker(accmaparr);
    if (defmoniker) {
        xmlNewProp(parent, "DefaultAccessMappingMoniker", defmoniker);
        free(defmoniker);
    }

    xmlNewProp(parent, "LastChangeId", "2565"); /* TODO */
    xmlNewProp(parent, "ClientCacheFresh", "true"); /* TODO */
    xmlNewProp(parent, "AccessPointsDoNotIncludeWebAppRelativeDirectory", "false"); /* TODO */

    xmlNode *svcdefs = xmlNewChild(parent, NULL, "ServiceDefinitions", NULL);

    if (inclall) {
        for (i = 0; svcarr[i] != NULL; i++ )
            location_append_service(svcdefs, svcarr[i]);
    }

    xmlNode *accessmappings = xmlNewChild(parent, NULL, "AccessMappings", NULL);

    for (i = 0; accmaparr[i] != NULL; i++) {
        xmlNode *am = xmlNewChild(accessmappings, NULL, "AccessMapping", NULL);
        xmlNewProp(am, "DisplayName", accmaparr[i]->name);
        xmlNewProp(am, "Moniker", accmaparr[i]->moniker);
        xmlNewProp(am, "AccessPoint", accmaparr[i]->apuri);
    }
}

/**
 * Appends a ServiceDefinition node to the given parent node.
 *
 * @param parent    the parent node to attach to
 * @param ref       service info for the new node
 */
void location_append_service(xmlNode *parent, tf_service *svcdef)
{
    char reltosettingstr[6];
    snprintf(reltosettingstr, 6, "%d", svcdef->reltosetting);

    xmlNode *sdnode = xmlNewChild(parent, NULL, "ServiceDefinition", NULL);
    xmlNewProp(sdnode, "serviceType", svcdef->type);
    xmlNewProp(sdnode, "identifier", svcdef->id);
    xmlNewProp(sdnode, "displayName", svcdef->name);
    xmlNewProp(sdnode, "relativeToSetting", reltosettingstr);

    if (strcmp(svcdef->relpath, "") != 0)
        xmlNewProp(sdnode, "relativePath", svcdef->relpath);

    xmlNewProp(sdnode, "description", svcdef->description);
    xmlNewProp(sdnode, "toolId", svcdef->tooltype);

    /* TODO */
    xmlNewChild(sdnode, NULL, "LocationMappings", NULL);
}

/**
 * Builds an array of service filters from XML. Calling functions
 * should call tf_free_service_filter_array() to free the result.
 *
 * @param xpres     the XPath result for the XML filter nodes
 *
 * @return a null-terminated array filters (return value will never be NULL)
 */
static tf_service_filter **_build_service_filters(xmlXPathObject *xpres)
{
    tf_service_filter **filters = 
        (tf_service_filter **)calloc(xpres->nodesetval->nodeNr + 1, sizeof(tf_service_filter));

    int i;
    for (i = 0; i < xpres->nodesetval->nodeNr; i++) {
        filters[i] = (tf_service_filter *)malloc(sizeof(tf_service_filter));
        bzero(filters[i], sizeof(tf_service_filter));

        xmlNode *stf = xpres->nodesetval->nodeTab[i];
        xmlChar *type = xmlGetProp(stf, "ServiceType");
        xmlChar *id = xmlGetProp(stf, "Identifier");

        filters[i]->type = strdup(type);
        filters[i]->id = strdup(id);

        xmlFree(type);
        xmlFree(id);
    }

    return filters;
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
    xmlNode *body = req->env->body;
    xmlXPathObject *xpres;
    xmlNode *arg;
    tf_service_filter **filters = NULL;
    tf_service **svcarr = NULL;
    tf_access_map **accmaparr = NULL;
    tf_node **nodearr = NULL;
    tf_error dberr;
    int inclservices = 0;
    int lastchgid = -1;

    arg = tf_xml_find_first(body, "m", TF_DEFAULT_NAMESPACE, "//m:connectOptions/text()");
    if (arg != NULL)
        inclservices = atoi(arg->content);

    arg = tf_xml_find_first(body, "m", TF_DEFAULT_NAMESPACE, "//m:lastChangeId/text()");
    if (arg != NULL)
        lastchgid = atoi(arg->content);

    xpres = tf_xml_find_all(body, "m", TF_DEFAULT_NAMESPACE, "//m:serviceTypeFilters/m:ServiceTypeFilter");
    if (xpres != NULL && xpres->nodesetval != NULL) {
        filters = _build_service_filters(xpres);
        xmlXPathFreeObject(xpres);
    } else if (xpres != NULL) {
        filters = (tf_service_filter **)calloc(1, sizeof(tf_service_filter));
        xmlXPathFreeObject(xpres);
    }

    if (inclservices) {
        /* TODO check last changed ID */
        dberr = tf_fetch_services(filters, &svcarr);
        filters = tf_free_service_filter_array(filters);

        if (dberr != TF_ERROR_SUCCESS) {
            tf_fault_env(
                    Fault_Server, 
                    "Failed to retrieve service definitions from the database", 
                    dberr, 
                    &res->env);
            return H_OK;
        }
    } else
        filters = tf_free_service_filter_array(filters);

    dberr = tf_fetch_access_map(&accmaparr);
    if (dberr != TF_ERROR_SUCCESS) {
        svcarr = tf_free_service_array(svcarr);
        tf_fault_env(
                Fault_Server, 
                "Failed to retrieve service definitions from the database", 
                dberr, 
                &res->env);
        return H_OK;
    }

    char **pathspec = (char **)calloc(2, sizeof(char *));
    pathspec[0] = (char *)alloca(sizeof(char) * 27);
    snprintf(pathspec[0], 27, "%s**", TF_CATALOG_ORGANIZATION_ROOT);

    char **typearr = (char **)calloc(2, sizeof(char *));
    typearr[0] = TF_CATALOG_TYPE_SERVER_INSTANCE;

    dberr = tf_query_nodes(
        (const char * const *)pathspec,
        (const char * const *)typearr,
        &nodearr);

    free(pathspec);
    free(typearr);

    if (dberr != TF_ERROR_SUCCESS || nodearr[0] == NULL) {
        svcarr = tf_free_service_array(svcarr);
        accmaparr = tf_free_access_map_array(accmaparr);
        nodearr = tf_free_node_array(nodearr);
        tf_fault_env(
                Fault_Server, 
                "Failed to retrieve the server instance ID from the database", 
                dberr, 
                &res->env);
        return H_OK;
    }

    xmlNode *cmd = soap_env_get_method(req->env);
    soap_env_new_with_method(cmd->ns->href, "ConnectResponse", &res->env);

    xmlNode *connresult = xmlNewChild(res->env->body->children->next, NULL, "ConnectResult", NULL);
    xmlNewProp(connresult, "InstanceId", "bed36b22-8b2a-464b-8702-8df2dbc413fe"); /* TODO */
    xmlNewProp(connresult, "CatalogResourceId", nodearr[0]->resource.id);

    _build_auth_node(connresult); /* TODO */
    _build_authz_node(connresult); /* TODO */

    xmlNode *locdata = xmlNewChild(connresult, NULL, "LocationServiceData", NULL);
    _append_location_data(locdata, svcarr, accmaparr, inclservices);

    svcarr = tf_free_service_array(svcarr);
    accmaparr = tf_free_access_map_array(accmaparr);
    nodearr = tf_free_node_array(nodearr);

    return H_OK;
}

/**
 * Location SOAP service handler for QueryServices.
 *
 * @param req   SOAP request context
 * @param res   SOAP response context
 *
 * @return H_OK on success
 */
static herror_t _query_services(SoapCtx *req, SoapCtx *res)
{
    xmlNode *body = req->env->body;
    xmlXPathObject *xpres;
    tf_service_filter **filters = NULL;
    tf_service **svcarr = NULL;
    tf_access_map **accmaparr = NULL;
    tf_error dberr;
    int lastchgid = -1;

    xmlNode *arg = tf_xml_find_first(body, "m", TF_DEFAULT_NAMESPACE, "//m:lastChangeId/text()");
    if (arg != NULL)
        lastchgid = atoi(arg->content);

    xpres = tf_xml_find_all(body, "m", TF_DEFAULT_NAMESPACE, "//m:serviceTypeFilters/m:ServiceTypeFilter");
    if (xpres != NULL && xpres->nodesetval != NULL) {
        filters = _build_service_filters(xpres);
        xmlXPathFreeObject(xpres);
    } else if (xpres != NULL) {
        filters = (tf_service_filter **)calloc(1, sizeof(tf_service_filter));
        xmlXPathFreeObject(xpres);
    }

    /* TODO check last changed ID */
    dberr = tf_fetch_services(filters, &svcarr);
    filters = tf_free_service_filter_array(filters);

    if (dberr != TF_ERROR_SUCCESS) {
        tf_fault_env(
                Fault_Server, 
                "Failed to retrieve service definitions from the database", 
                dberr, 
                &res->env);
        return H_OK;
    }

    dberr = tf_fetch_access_map(&accmaparr);
    if (dberr != TF_ERROR_SUCCESS) {
        svcarr = tf_free_service_array(svcarr);
        tf_fault_env(
                Fault_Server, 
                "Failed to retrieve service definitions from the database", 
                dberr, 
                &res->env);
        return H_OK;
    }

    xmlNode *cmd = soap_env_get_method(req->env);
    soap_env_new_with_method(cmd->ns->href, "QueryServicesResponse", &res->env);

    xmlNode *result = xmlNewChild(res->env->body->children->next, NULL, "QueryServicesResult", NULL);
    _append_location_data(result, svcarr, accmaparr, 1);

    svcarr = tf_free_service_array(svcarr);
    accmaparr = tf_free_access_map_array(accmaparr);

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
    soap_router_register_service(
        *router,
        _query_services,
        "QueryServices",
        TF_DEFAULT_NAMESPACE);

    gcslog_info("registered location services");
}

