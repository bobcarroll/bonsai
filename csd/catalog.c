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
 * @brief   catalog web service
 *
 * @author  Bob Carroll (bob.carroll@alum.rit.edu)
 */

#include <cs/catalog.h>

#include <gcs/log.h>

#include <tf/catalog.h>
#include <tf/catalogdb.h>
#include <tf/webservices.h>
#include <tf/xml.h>

/**
 * Appends a CatalogResourceType node to the given parent node.
 *
 * @param parent    the parent node to attach to
 * @param type      resource type info for the new node
 */
static void _append_resource_type(xmlNode *parent, tf_catalog_resource_type_t type)
{
    xmlNode *crtnode = xmlNewChild(parent, NULL, "CatalogResourceType", NULL);
    xmlNewProp(crtnode, "Identifier", type.id);
    xmlNewProp(crtnode, "DisplayName", type.name);

    if (type.description != NULL)
        xmlNewChild(crtnode, NULL, "Description", type.description);
}

/**
 * Appends a CatalogServiceReference node to the given parent node.
 *
 * @param parent    the parent node to attach to
 * @param ref       service reference info for the new node
 */
static void _append_service_ref(xmlNode *parent, tf_catalog_service_t ref)
{
    char reltosettingstr[6];
    snprintf(reltosettingstr, 6, "%d", ref.service.reltosetting);

    xmlNode *csrnode = xmlNewChild(parent, NULL, "CatalogServiceReference", NULL);
    xmlNewProp(csrnode, "ResourceIdentifier", ref.id);
    xmlNewProp(csrnode, "AssociationKey", ref.assockey);

    xmlNode *sdnode = xmlNewChild(csrnode, NULL, "ServiceDefinition", NULL);
    xmlNewProp(sdnode, "serviceType", ref.service.type);
    xmlNewProp(sdnode, "identifier", ref.service.id);
    xmlNewProp(sdnode, "displayName", ref.service.name);
    xmlNewProp(sdnode, "relativeToSetting", reltosettingstr);

    if (strcmp(ref.service.relpath, "") != 0)
        xmlNewProp(sdnode, "relativePath", ref.service.relpath);

    xmlNewProp(sdnode, "description", ref.service.description);
    xmlNewProp(sdnode, "toolId", ref.service.tooltype);
    xmlNewChild(sdnode, NULL, "LocationMappings", NULL);
}

/**
 * Appends a CatalogResource node to the given parent node.
 *
 * @param parent    the parent node to attach to
 * @param path      the catalog node full path
 * @param resource  the catalog resource info for the new node
 * @param service   matching catalog service references
 * @param matched
 */
static void _append_resource(xmlNode *parent, const char *path, tf_catalog_resource_t resource,
    tf_catalog_service_array_t services, tf_catalog_property_array_t properties, int matched)
{
    int i;

    xmlNode *crnode = xmlNewChild(parent, NULL, "CatalogResource", NULL);
    xmlNewProp(crnode, "Identifier", resource.id);
    xmlNewProp(crnode, "DisplayName", resource.name);
    xmlNewProp(crnode, "ResourceTypeIdentifier", resource.type.id);
    xmlNewProp(crnode, "TempCorrelationId", resource.id);
    xmlNewProp(crnode, "ctype", "0"); /* TODO */
    xmlNewProp(crnode, "MatchedQuery", matched ? "true" : "false");

    if (resource.description != NULL)
        xmlNewChild(crnode, NULL, "Description", resource.description);

    xmlNode *refsnode = xmlNewChild(crnode, NULL, "CatalogServiceReferences", NULL);

    for (i = 0; i < services.count; i++) {
        tf_catalog_service_t cursvc = services.items[i];

        if (strcmp(resource.id, cursvc.id) != 0)
            continue;

        _append_service_ref(refsnode, cursvc);
    }

    xmlNode *propsnode = xmlNewChild(crnode, NULL, "Properties", NULL);

    for (i = 0; i < properties.count; i++) {
        tf_catalog_property_t curprop = properties.items[i];

        if (resource.propertyid != curprop.artifactid)
            continue;

        xmlNode *kvoss = xmlNewChild(propsnode, NULL, "KeyValueOfStringString", NULL);
        xmlNewChild(kvoss, NULL, "Key", curprop.property);
        xmlNewChild(kvoss, NULL, "Value", curprop.value);
    }

    xmlNode *refpathnode = xmlNewChild(crnode, NULL, "NodeReferencePaths", NULL);
    xmlNewChild(refpathnode, NULL, "string", path);
}

/**
 * Appends a CatalogNode node to the given parent node.
 *
 * @param parent    the parent node to attach to
 * @param path      the catalog node full path
 * @param node      catalog node info for the new node
 * @param matched
 */
static void _append_node(xmlNode *parent, const char *path, tf_catalog_node_t node, int matched)
{
    xmlNode *cnnode = xmlNewChild(parent, NULL, "CatalogNode", NULL);
    xmlNewProp(cnnode, "FullPath", path);
    xmlNewProp(cnnode, "default", node.fdefault ? "true" : "false");
    xmlNewProp(cnnode, "ResourceIdentifier", node.resource.id);
    xmlNewProp(cnnode, "ParentPath", node.parent);
    xmlNewProp(cnnode, "ChildItem", node.child);
    xmlNewProp(cnnode, "NodeDependenciesIncluded", "false"); /* TODO */
    xmlNewProp(cnnode, "ctype", "0"); /* TODO */
    xmlNewProp(cnnode, "MatchedQuery", matched ? "true" : "false");
    xmlNewChild(cnnode, NULL, "NodeDependencies", NULL); /* TODO */
}

/**
 * Catalog SOAP service handler for QueryResources.
 *
 * @param req   SOAP request context
 * @param res   SOAP response context
 *
 * @return H_OK on success
 */
static herror_t _query_resources(SoapCtx *req, SoapCtx *res)
{
    xmlNode *body = req->env->body;
    xmlXPathObject *xpres;
    char **idarr = NULL;
    tf_catalog_node_array_t nodearr;
    tf_catalog_service_array_t svcarr;
    tf_catalog_property_array_t proparr;
    tf_error_t dberr;
    int ftypes = 0, i;

    xpres = tf_xml_find_all(body, "m", TF_DEFAULT_NAMESPACE, "//m:resourceIdentifiers/m:guid/text()");
    if (xpres != NULL && xpres->nodesetval != NULL) {
        idarr = (char **)calloc(xpres->nodesetval->nodeNr + 1, sizeof(char *));

        for (i = 0; i < xpres->nodesetval->nodeNr; i++)
            idarr[i] = strdup(xpres->nodesetval->nodeTab[i]->content);

        xmlXPathFreeObject(xpres);
    } else if (xpres != NULL)
        xmlXPathFreeObject(xpres);

    xpres = tf_xml_find_all(body, "m", TF_DEFAULT_NAMESPACE, "//m:resourceTypeIdentifiers/m:guid/text()");
    if (xpres != NULL && xpres->nodesetval != NULL && idarr == NULL) {
        idarr = (char **)calloc(xpres->nodesetval->nodeNr + 1, sizeof(char *));
        ftypes = 1;

        for (i = 0; i < xpres->nodesetval->nodeNr; i++)
            idarr[i] = strdup(xpres->nodesetval->nodeTab[i]->content);

        xmlXPathFreeObject(xpres);
    } else if (xpres != NULL) {
        if (xpres->nodesetval != NULL)
            gcslog_warn("skipping resourceTypeIdentifiers because resourceIdentifiers was found");
        xmlXPathFreeObject(xpres);
    }

    if (idarr == NULL) { 
        tf_fault_env(
            Fault_Client, 
            "No resourceIdentifiers or resourceTypeIdentifiers were found in the message", 
            dberr, 
            &res->env);
        return H_OK;
    }

    /* TODO property filters */
    /* TODO query options */

    dberr = tf_catalog_fetch_resources((const char * const *)idarr, ftypes, &nodearr);

    for (i = 0; idarr[i] != NULL; i++)
        free(idarr[i]);
    free(idarr);

    if (dberr != TF_ERROR_SUCCESS) {
        tf_fault_env(
            Fault_Server, 
            "Failed to retrieve catalog resources from the database", 
            dberr, 
            &res->env);
        return H_OK;
    }

    dberr = tf_catalog_fetch_services(nodearr, &svcarr);
    if (dberr != TF_ERROR_SUCCESS) {
        tf_catalog_free_node_array(nodearr);
        tf_fault_env(
            Fault_Server, 
            "Failed to retrieve service definitions from the database", 
            dberr, 
            &res->env);
        return H_OK;
    }

    dberr = tf_catalog_fetch_properties(nodearr, &proparr);
    if (dberr != TF_ERROR_SUCCESS) {
        tf_catalog_free_node_array(nodearr);
        tf_catalog_free_service_array(svcarr);
        tf_fault_env(
            Fault_Server, 
            "Failed to retrieve resource properties from the database", 
            dberr, 
            &res->env);
        return H_OK;
    }

    xmlNode *cmd = soap_env_get_method(req->env);
    soap_env_new_with_method(cmd->ns->href, "QueryResourcesResponse", &res->env);

    xmlNode *result = xmlNewChild(res->env->body->children->next, NULL, "QueryResourcesResult", NULL);
    xmlNode *restypenode = xmlNewChild(result, NULL, "CatalogResourceTypes", NULL);
    xmlNode *resnode = xmlNewChild(result, NULL, "CatalogResources", NULL);
    xmlNode *nodesnode = xmlNewChild(result, NULL, "CatalogNodes", NULL);
    xmlNewChild(result, NULL, "DeletedResources", NULL);
    xmlNewChild(result, NULL, "DeletedNodeResources", NULL);
    xmlNewChild(result, NULL, "DeletedNodes", NULL);
    xmlNewChild(result, NULL, "LocationServiceLastChangeId", "2565"); /* TODO */

    for (i = 0; i < nodearr.count; i++) {
        tf_catalog_node_t curnode = nodearr.items[i];

        char *noderefpath = (char *)alloca(sizeof(char) * 
            (strlen(curnode.parent) + strlen(curnode.child) + 1));
        sprintf(noderefpath, "%s%s", curnode.parent, curnode.child);

        _append_resource_type(restypenode, curnode.resource.type);
        _append_resource(resnode, noderefpath, curnode.resource, svcarr, proparr, 1);
        _append_node(nodesnode, noderefpath, curnode, 0);
    }

    tf_catalog_free_node_array(nodearr);
    tf_catalog_free_service_array(svcarr);
    tf_catalog_free_property_array(proparr);

    return H_OK;
}

/**
 * Catalog SOAP service handler for QueryNodes.
 *
 * @param req   SOAP request context
 * @param res   SOAP response context
 *
 * @return H_OK on success
 */
static herror_t _query_nodes(SoapCtx *req, SoapCtx *res)
{
    xmlNode *body = req->env->body;
    xmlXPathObject *xpres;
    char **pathspec = NULL;
    char **typefilter = NULL;
    tf_catalog_node_array_t nodearr;
    tf_catalog_service_array_t svcarr;
    tf_catalog_property_array_t proparr;
    tf_error_t dberr;
    int i;

    xpres = tf_xml_find_all(body, "m", TF_DEFAULT_NAMESPACE, "//m:pathSpecs/m:string/text()");
    if (xpres != NULL && xpres->nodesetval != NULL) {
        pathspec = (char **)calloc(xpres->nodesetval->nodeNr + 1, sizeof(char *));

        for (i = 0; i < xpres->nodesetval->nodeNr; i++)
            pathspec[i] = strdup(xpres->nodesetval->nodeTab[i]->content);

        xmlXPathFreeObject(xpres);
    } else if (xpres != NULL) {
        pathspec = (char **)calloc(1, sizeof(char *));
        xmlXPathFreeObject(xpres);
    }

    xpres = tf_xml_find_all(body, "m", TF_DEFAULT_NAMESPACE, "//m:resourceTypeFilters/m:guid/text()");
    if (xpres != NULL && xpres->nodesetval != NULL) {
        typefilter = (char **)calloc(xpres->nodesetval->nodeNr + 1, sizeof(char *));

        for (i = 0; i < xpres->nodesetval->nodeNr; i++)
            typefilter[i] = strdup(xpres->nodesetval->nodeTab[i]->content);

        xmlXPathFreeObject(xpres);
    } else if (xpres != NULL) {
        typefilter = (char **)calloc(1, sizeof(char *));
        xmlXPathFreeObject(xpres);
    }

    /* TODO property filter */
    /* TODO query options */

    dberr = tf_catalog_query_nodes(
        (const char * const *)pathspec, 
        (const char * const *)typefilter, 
        &nodearr);

    for (i = 0; pathspec[i] != NULL; i++)
        free(pathspec[i]);
    free(pathspec);

    for (i = 0; typefilter[i] != NULL; i++)
        free(typefilter[i]);
    free(typefilter);

    if (dberr != TF_ERROR_SUCCESS) {
        tf_fault_env(
            Fault_Server, 
            "Failed to retrieve catalog nodes from the database", 
            dberr, 
            &res->env);
        return H_OK;
    }

    dberr = tf_catalog_fetch_services(nodearr, &svcarr);
    if (dberr != TF_ERROR_SUCCESS) {
        tf_catalog_free_node_array(nodearr);
        tf_fault_env(
            Fault_Server, 
            "Failed to retrieve service definitions from the database", 
            dberr, 
            &res->env);
        return H_OK;
    }

    dberr = tf_catalog_fetch_properties(nodearr, &proparr);
    if (dberr != TF_ERROR_SUCCESS) {
        tf_catalog_free_node_array(nodearr);
        tf_catalog_free_service_array(svcarr);
        tf_fault_env(
            Fault_Server, 
            "Failed to retrieve resource properties from the database", 
            dberr, 
            &res->env);
        return H_OK;
    }

    xmlNode *cmd = soap_env_get_method(req->env);
    soap_env_new_with_method(cmd->ns->href, "QueryNodesResponse", &res->env);

    xmlNode *result = xmlNewChild(res->env->body->children->next, NULL, "QueryNodesResult", NULL);
    xmlNode *restypenode = xmlNewChild(result, NULL, "CatalogResourceTypes", NULL);
    xmlNode *resnode = xmlNewChild(result, NULL, "CatalogResources", NULL);
    xmlNode *nodesnode = xmlNewChild(result, NULL, "CatalogNodes", NULL);
    xmlNewChild(result, NULL, "DeletedResources", NULL);
    xmlNewChild(result, NULL, "DeletedNodeResources", NULL);
    xmlNewChild(result, NULL, "DeletedNodes", NULL);
    xmlNewChild(result, NULL, "LocationServiceLastChangeId", "2565");

    for (i = 0; i < nodearr.count; i++) {
        tf_catalog_node_t curnode = nodearr.items[i];

        char *noderefpath = (char *)alloca(sizeof(char) * 
            (strlen(curnode.parent) + strlen(curnode.child) + 1));
        sprintf(noderefpath, "%s%s", curnode.parent, curnode.child);

        _append_resource_type(restypenode, curnode.resource.type);
        _append_resource(resnode, noderefpath, curnode.resource, svcarr, proparr, 1);
        _append_node(nodesnode, noderefpath, curnode, 1);
    }

    tf_catalog_free_node_array(nodearr);
    tf_catalog_free_service_array(svcarr);
    tf_catalog_free_property_array(proparr);

    return H_OK;
}

static int _auth_ntlm(SoapEnv *env, const char *user, const char *passwd)
{
    return 0;
}

/**
 * Catalog service initialisation.
 *
 * @param router    output buffer for the SOAP router
 * @param prefix    the URI prefix for this service
 */
void catalog_service_init(SoapRouter **router, const char *prefix)
{
    char url[1024];

    (*router) = soap_router_new();
    soap_router_register_security(*router, (httpd_auth)_auth_ntlm);

    sprintf(url, "%s%s", prefix != NULL ? prefix : "", TF_CATALOG_SERVICE_ENDPOINT);
    soap_server_register_router(*router, url);

    soap_router_register_service(
        *router,
        _query_resources,
        "QueryResources",
        TF_DEFAULT_NAMESPACE);
    soap_router_register_service(
        *router,
        _query_nodes,
        "QueryNodes",
        TF_DEFAULT_NAMESPACE);

    gcslog_info("registered catalog services");
}

