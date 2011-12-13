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

#include <log.h>
#include <authz.h>

#include <tf/catalog.h>
#include <tf/location.h>
#include <tf/servicehost.h>
#include <tf/webservices.h>
#include <tf/xml.h>

#include <csd.h>

/**
 * Appends a KeyValueOfStringString node to the given node.
 *
 * @param parent    the Attributes node to attach to
 * @param key       attribute key
 * @param value     attribute value
 */
static void _append_attr_kvoss(xmlNode *parent, const char *key, const char *value)
{
    xmlNode *kvoss = xmlNewChild(parent, NULL, "KeyValueOfStringString", NULL);
    xmlNewChild(kvoss, NULL, "Key", key);
    xmlNewChild(kvoss, NULL, "Value", value);
}

/**
 * Appends authentication/authorisation user data to the given node.
 *
 * @param parent    the AuthenticatedUser or AuthorizedUser node to attach to
 * @param ui        session user info
 */
static void _append_auth_user(xmlNode *parent, userinfo_t *ui)
{
    xmlNewProp(parent, "DisplayName", ui->display_name);
    xmlNewProp(parent, "IsContainer", "false"); /* TODO */
    xmlNewProp(parent, "IsActive", "true"); /* TODO */
    xmlNewProp(parent, "TeamFoundationId", "d00b4f90-df4a-452f-bd54-1d3d001661f8"); /* TODO */
    xmlNewProp(parent, "UniqueUserId", "0"); /* TODO */

    xmlNode *descriptor = xmlNewChild(parent, NULL, "Descriptor", NULL);
    xmlNewProp(descriptor, "identityType", "System.Security.Principal.WindowsIdentity");
    xmlNewProp(descriptor, "identifier", ui->sid);

    xmlNode *attrs = xmlNewChild(parent, NULL, "Attributes", NULL);
    _append_attr_kvoss(attrs, "SchemaClassName", "User"); /* TODO */
    _append_attr_kvoss(attrs, "Description", NULL); /* TODO */
    _append_attr_kvoss(attrs, "Domain", ui->domain);
    _append_attr_kvoss(attrs, "Account", ui->logon_name);
    _append_attr_kvoss(attrs, "DN", NULL); /* TODO */
    _append_attr_kvoss(attrs, "Mail", NULL); /* TODO */
    _append_attr_kvoss(attrs, "SpecialType", "Generic"); /* TODO */

    xmlNewChild(parent, NULL, "Members", NULL); /* TODO */
    xmlNewChild(parent, NULL, "MemberOf", NULL); /* TODO */
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
    xmlNewProp(parent, "ClientCacheFresh", inclall ? "false" : "true"); /* TODO */
    xmlNewProp(parent, "AccessPointsDoNotIncludeWebAppRelativeDirectory", "false"); /* TODO */

    if (inclall) {
        xmlNode *svcdefs = xmlNewChild(parent, NULL, "ServiceDefinitions", NULL);

        for (i = 0; svcarr[i]; i++)
            location_append_service(svcdefs, svcarr[i]);

        xmlNode *accessmappings = xmlNewChild(parent, NULL, "AccessMappings", NULL);

        for (i = 0; accmaparr[i]; i++) {
            xmlNode *am = xmlNewChild(accessmappings, NULL, "AccessMapping", NULL);
            xmlNewProp(am, "DisplayName", accmaparr[i]->name);
            xmlNewProp(am, "Moniker", accmaparr[i]->moniker);
            xmlNewProp(am, "AccessPoint", accmaparr[i]->apuri);
        }
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
    pgctx *ctx;
    tf_service_filter **filters = NULL;
    tf_service **svcarr = NULL;
    tf_access_map **accmaparr = NULL;
    tf_node **nodearr = NULL;
    tf_host *host = NULL;
    tf_error dberr;
    userinfo_t *ui = NULL;
    const char *hostid = req->tag;
    int inclservices = 0;
    int lastchgid = -1;

    if (!hostid) {
        tf_fault_env(
            Fault_Server, 
            "service host instance ID was NULL!", 
            TF_ERROR_INTERNAL, 
            &res->env);
        return H_OK;
    }

    if (!req->userid) {
        tf_fault_env(
                Fault_Server, 
                "Anonymous access is not permitted!", 
                TF_ERROR_ACCESS_DENIED, 
                &res->env);
        return H_OK;
    }

    ui = authz_lookup_user(req->userid);

    if (!ui) {
        tf_fault_env(
                Fault_Server, 
                "Failed to retrieve user information from account database", 
                TF_ERROR_INTERNAL, 
                &res->env);
        return H_OK;
    }

    arg = tf_xml_find_first(body, "m", TF_DEFAULT_NAMESPACE, "//m:connectOptions/text()");
    if (arg)
        inclservices = atoi(arg->content);

    arg = tf_xml_find_first(body, "m", TF_DEFAULT_NAMESPACE, "//m:lastChangeId/text()");
    if (arg)
        lastchgid = atoi(arg->content);

    xpres = tf_xml_find_all(body, "m", TF_DEFAULT_NAMESPACE, "//m:serviceTypeFilters/m:ServiceTypeFilter");
    if (xpres && xpres->nodesetval) {
        filters = _build_service_filters(xpres);
        xmlXPathFreeObject(xpres);
    } else if (xpres) {
        filters = (tf_service_filter **)calloc(1, sizeof(tf_service_filter));
        xmlXPathFreeObject(xpres);
    }

    ctx = pg_context_acquire(NULL);
    dberr = tf_fetch_single_host(ctx, hostid, &host);

    if (dberr != TF_ERROR_SUCCESS || !host) {
        authz_free_buffer(ui);
        pg_context_release(ctx);
        tf_fault_env(
                Fault_Server, 
                "Failed to retrieve the host instance from the database", 
                dberr, 
                &res->env);
        return H_OK;
    }

    char **idarr = (char **)calloc(2, sizeof(char *));
    idarr[0] = strdup(host->resource);

    dberr = tf_fetch_resources(ctx, (const char * const *)idarr, 0, &nodearr);

    free(idarr[0]);
    free(idarr);

    if (dberr != TF_ERROR_SUCCESS || !nodearr[0]) {
        authz_free_buffer(ui);

        tf_free_host(host);
        free(host);

        pg_context_release(ctx);

        tf_fault_env(
                Fault_Server, 
                "Failed to retrieve catalog resource for host", 
                dberr, 
                &res->env);
        return H_OK;
    }

    if (host->parent) {
        pg_context_release(ctx);
        ctx = pg_context_acquire(hostid);
    }

    if (inclservices) {
        /* TODO check last changed ID */
        dberr = tf_fetch_services(ctx, filters, &svcarr);
        filters = tf_free_service_filter_array(filters);

        if (dberr != TF_ERROR_SUCCESS) {
            authz_free_buffer(ui);

            tf_free_host(host);
            free(host);

            nodearr = tf_free_node_array(nodearr);
            pg_context_release(ctx);

            tf_fault_env(
                    Fault_Server, 
                    "Failed to retrieve service definitions from the database", 
                    dberr, 
                    &res->env);
            return H_OK;
        }
    } else
        filters = tf_free_service_filter_array(filters);

    dberr = tf_fetch_access_map(ctx, &accmaparr);
    if (dberr != TF_ERROR_SUCCESS) {
        authz_free_buffer(ui);

        tf_free_host(host);
        free(host);

        nodearr = tf_free_node_array(nodearr);
        svcarr = tf_free_service_array(svcarr);
        pg_context_release(ctx);

        tf_fault_env(
                Fault_Server, 
                "Failed to retrieve service definitions from the database", 
                dberr, 
                &res->env);
        return H_OK;
    }

    xmlNode *cmd = soap_env_get_method(req->env);
    soap_env_new_with_method(cmd->ns->href, "ConnectResponse", &res->env);

    xmlNode *connresult = xmlNewChild(res->env->body->children->next, NULL, "ConnectResult", NULL);
    xmlNewProp(connresult, "InstanceId", hostid);
    xmlNewProp(connresult, "CatalogResourceId", nodearr[0]->resource.id);

    if (host->vdir)
        xmlNewProp(connresult, "WebApplicationRelativeDirectory", host->vdir);

    xmlNode *authuser = xmlNewChild(connresult, NULL, "AuthenticatedUser", NULL);
    _append_auth_user(authuser, ui);

    xmlNode *authzuser = xmlNewChild(connresult, NULL, "AuthorizedUser", NULL);
    _append_auth_user(authzuser, ui);

    xmlNode *locdata = xmlNewChild(connresult, NULL, "LocationServiceData", NULL);
    _append_location_data(locdata, svcarr, accmaparr, inclservices);

    svcarr = tf_free_service_array(svcarr);
    accmaparr = tf_free_access_map_array(accmaparr);
    nodearr = tf_free_node_array(nodearr);

    authz_free_buffer(ui);

    tf_free_host(host);
    free(host);

    pg_context_release(ctx);

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
    pgctx *ctx;
    tf_service_filter **filters = NULL;
    tf_service **svcarr = NULL;
    tf_access_map **accmaparr = NULL;
    tf_error dberr;
    const char *hostid = req->tag;
    int lastchgid = -1;

    xmlNode *arg = tf_xml_find_first(body, "m", TF_DEFAULT_NAMESPACE, "//m:lastChangeId/text()");
    if (arg)
        lastchgid = atoi(arg->content);

    xpres = tf_xml_find_all(body, "m", TF_DEFAULT_NAMESPACE, "//m:serviceTypeFilters/m:ServiceTypeFilter");
    if (xpres && xpres->nodesetval) {
        filters = _build_service_filters(xpres);
        xmlXPathFreeObject(xpres);
    } else if (xpres) {
        filters = (tf_service_filter **)calloc(1, sizeof(tf_service_filter));
        xmlXPathFreeObject(xpres);
    }

    ctx = pg_context_acquire(hostid);

    /* TODO check last changed ID */
    dberr = tf_fetch_services(ctx, filters, &svcarr);
    filters = tf_free_service_filter_array(filters);

    if (dberr != TF_ERROR_SUCCESS) {
        pg_context_release(ctx);
        tf_fault_env(
                Fault_Server, 
                "Failed to retrieve service definitions from the database", 
                dberr, 
                &res->env);
        return H_OK;
    }

    dberr = tf_fetch_access_map(ctx, &accmaparr);
    if (dberr != TF_ERROR_SUCCESS) {
        svcarr = tf_free_service_array(svcarr);
        pg_context_release(ctx);
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

    pg_context_release(ctx);

    return H_OK;
}

/**
 * Location service initialisation.
 *
 * @param router    output buffer for the SOAP router
 * @param prefix    the URI prefix for this service
 * @param ref       service reference info
 * @param instid    host instance ID
 */
void location_service_init(SoapRouter **router, const char *prefix, tf_service_ref *ref,
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
        _connect,
        "Connect",
        TF_DEFAULT_NAMESPACE);
    soap_router_register_service(
        *router,
        _query_services,
        "QueryServices",
        TF_DEFAULT_NAMESPACE);

    log_info("registered location service %s for host %s", url, instid);
}

