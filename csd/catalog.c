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
#include <cs/location.h>

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
static void _append_resource_type(xmlNode *parent, tf_resource_type type)
{
    xmlNode *crtnode = xmlNewChild(parent, NULL, "CatalogResourceType", NULL);
    xmlNewProp(crtnode, "Identifier", type.id);
    xmlNewProp(crtnode, "DisplayName", type.name);

    if (type.description)
        xmlNewChild(crtnode, NULL, "Description", type.description);
}

/**
 * Appends a CatalogServiceReference node to the given parent node.
 *
 * @param parent    the parent node to attach to
 * @param ref       service reference info for the new node
 */
static void _append_service_ref(xmlNode *parent, tf_service_ref *ref)
{
    xmlNode *csrnode = xmlNewChild(parent, NULL, "CatalogServiceReference", NULL);
    xmlNewProp(csrnode, "ResourceIdentifier", ref->id);
    xmlNewProp(csrnode, "AssociationKey", ref->assockey);

    location_append_service(csrnode, &ref->service);
}

/**
 * Appends a CatalogResource node to the given parent node.
 *
 * @param parent        the parent node to attach to
 * @param path          the catalog node full path
 * @param resource      the catalog resource info for the new node
 * @param services      a null-terminated array of matching catalog service references
 * @param properties    property info array
 * @param matched
 */
static void _append_resource(xmlNode *parent, const char *path, tf_resource resource,
    tf_service_ref **services, tf_property **properties, int matched)
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

    for (i = 0; services[i]; i++) {
        if (strcmp(resource.id, services[i]->id) != 0)
            continue;

        _append_service_ref(refsnode, services[i]);
    }

    xmlNode *propsnode = xmlNewChild(crnode, NULL, "Properties", NULL);

    for (i = 0; properties[i]; i++) {
        if (resource.propertyid != properties[i]->artifactid)
            continue;

        xmlNode *kvoss = xmlNewChild(propsnode, NULL, "KeyValueOfStringString", NULL);
        xmlNewChild(kvoss, NULL, "Key", properties[i]->property);
        xmlNewChild(kvoss, NULL, "Value", properties[i]->value);
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
static void _append_node(xmlNode *parent, const char *path, tf_node *node, int matched)
{
    xmlNode *cnnode = xmlNewChild(parent, NULL, "CatalogNode", NULL);
    xmlNewProp(cnnode, "FullPath", path);
    xmlNewProp(cnnode, "default", node->fdefault ? "true" : "false");
    xmlNewProp(cnnode, "ResourceIdentifier", node->resource.id);
    xmlNewProp(cnnode, "ParentPath", node->parent);
    xmlNewProp(cnnode, "ChildItem", node->child);
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
    pgctx *ctx;
    char **idarr = NULL;
    tf_node **nodearr = NULL;
    tf_service_ref **svcarr = NULL;
    tf_property **proparr = NULL;
    tf_error dberr;
    int ftypes = 0, i;

    xpres = tf_xml_find_all(body, "m", TF_DEFAULT_NAMESPACE, "//m:resourceIdentifiers/m:guid/text()");
    if (xpres && xpres->nodesetval) {
        idarr = (char **)calloc(xpres->nodesetval->nodeNr + 1, sizeof(char *));

        for (i = 0; i < xpres->nodesetval->nodeNr; i++)
            idarr[i] = strdup(xpres->nodesetval->nodeTab[i]->content);

        xmlXPathFreeObject(xpres);
    } else if (xpres)
        xmlXPathFreeObject(xpres);

    xpres = tf_xml_find_all(body, "m", TF_DEFAULT_NAMESPACE, "//m:resourceTypeIdentifiers/m:guid/text()");
    if (xpres && xpres->nodesetval && !idarr) {
        idarr = (char **)calloc(xpres->nodesetval->nodeNr + 1, sizeof(char *));
        ftypes = 1;

        for (i = 0; i < xpres->nodesetval->nodeNr; i++)
            idarr[i] = strdup(xpres->nodesetval->nodeTab[i]->content);

        xmlXPathFreeObject(xpres);
    } else if (xpres) {
        if (xpres->nodesetval)
            gcslog_warn("skipping resourceTypeIdentifiers because resourceIdentifiers was found");
        xmlXPathFreeObject(xpres);
    }

    if (!idarr) { 
        tf_fault_env(
            Fault_Client, 
            "No resourceIdentifiers or resourceTypeIdentifiers were found in the message", 
            dberr, 
            &res->env);
        return H_OK;
    }

    ctx = gcs_pgctx_acquire(NULL);

    /* TODO property filters */
    /* TODO query options */

    dberr = tf_fetch_resources(ctx, (const char * const *)idarr, ftypes, &nodearr);

    for (i = 0; idarr[i]; i++)
        free(idarr[i]);
    free(idarr);

    if (dberr != TF_ERROR_SUCCESS) {
        gcs_pgctx_release(ctx);
        tf_fault_env(
            Fault_Server, 
            "Failed to retrieve catalog resources from the database", 
            dberr, 
            &res->env);
        return H_OK;
    }

    dberr = tf_fetch_service_refs(ctx, nodearr, &svcarr);
    if (dberr != TF_ERROR_SUCCESS) {
        nodearr = tf_free_node_array(nodearr);
        gcs_pgctx_release(ctx);
        tf_fault_env(
            Fault_Server, 
            "Failed to retrieve service definitions from the database", 
            dberr, 
            &res->env);
        return H_OK;
    }

    dberr = tf_fetch_properties(ctx, nodearr, &proparr);
    if (dberr != TF_ERROR_SUCCESS) {
        nodearr = tf_free_node_array(nodearr);
        svcarr = tf_free_service_ref_array(svcarr);
        gcs_pgctx_release(ctx);
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

    for (i = 0; nodearr[i]; i++) {
        char *noderefpath = (char *)alloca(sizeof(char) * 
            (strlen(nodearr[i]->parent) + strlen(nodearr[i]->child) + 1));
        sprintf(noderefpath, "%s%s", nodearr[i]->parent, nodearr[i]->child);

        _append_resource_type(restypenode, nodearr[i]->resource.type);
        _append_resource(resnode, noderefpath, nodearr[i]->resource, svcarr, proparr, 1);
        _append_node(nodesnode, noderefpath, nodearr[i], 0);
    }

    nodearr = tf_free_node_array(nodearr);
    svcarr = tf_free_service_ref_array(svcarr);
    proparr = tf_free_property_array(proparr);

    gcs_pgctx_release(ctx);

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
    pgctx *ctx;
    char **pathspec = NULL;
    char **typefilter = NULL;
    tf_node **nodearr = NULL;
    tf_service_ref **svcarr = NULL;
    tf_property **proparr = NULL;
    tf_error dberr;
    int i;

    xpres = tf_xml_find_all(body, "m", TF_DEFAULT_NAMESPACE, "//m:pathSpecs/m:string/text()");
    if (xpres && xpres->nodesetval) {
        pathspec = (char **)calloc(xpres->nodesetval->nodeNr + 1, sizeof(char *));

        for (i = 0; i < xpres->nodesetval->nodeNr; i++)
            pathspec[i] = strdup(xpres->nodesetval->nodeTab[i]->content);

        xmlXPathFreeObject(xpres);
    } else if (xpres) {
        pathspec = (char **)calloc(1, sizeof(char *));
        xmlXPathFreeObject(xpres);
    }

    xpres = tf_xml_find_all(body, "m", TF_DEFAULT_NAMESPACE, "//m:resourceTypeFilters/m:guid/text()");
    if (xpres && xpres->nodesetval) {
        typefilter = (char **)calloc(xpres->nodesetval->nodeNr + 1, sizeof(char *));

        for (i = 0; i < xpres->nodesetval->nodeNr; i++)
            typefilter[i] = strdup(xpres->nodesetval->nodeTab[i]->content);

        xmlXPathFreeObject(xpres);
    } else if (xpres) {
        typefilter = (char **)calloc(1, sizeof(char *));
        xmlXPathFreeObject(xpres);
    }

    ctx = gcs_pgctx_acquire(NULL);

    /* TODO property filter */
    /* TODO query options */

    dberr = tf_query_nodes(
        ctx,
        (const char * const *)pathspec, 
        (const char * const *)typefilter, 
        &nodearr);

    for (i = 0; pathspec[i]; i++)
        free(pathspec[i]);
    free(pathspec);

    for (i = 0; typefilter[i]; i++)
        free(typefilter[i]);
    free(typefilter);

    if (dberr != TF_ERROR_SUCCESS) {
        gcs_pgctx_release(ctx);
        tf_fault_env(
            Fault_Server, 
            "Failed to retrieve catalog nodes from the database", 
            dberr, 
            &res->env);
        return H_OK;
    }

    if (nodearr[0]) {
        dberr = tf_fetch_service_refs(ctx, nodearr, &svcarr);
        if (dberr != TF_ERROR_SUCCESS) {
            nodearr = tf_free_node_array(nodearr);
            gcs_pgctx_release(ctx);
            tf_fault_env(
                    Fault_Server, 
                    "Failed to retrieve service definitions from the database", 
                    dberr, 
                    &res->env);
            return H_OK;
        }

        dberr = tf_fetch_properties(ctx, nodearr, &proparr);
        if (dberr != TF_ERROR_SUCCESS) {
            nodearr = tf_free_node_array(nodearr);
            svcarr = tf_free_service_ref_array(svcarr);
            gcs_pgctx_release(ctx);
            tf_fault_env(
                    Fault_Server, 
                    "Failed to retrieve resource properties from the database", 
                    dberr, 
                    &res->env);
            return H_OK;
        }
    } else {
        svcarr = (tf_service_ref **)calloc(1, sizeof(tf_service_ref *));
        proparr = (tf_property **)calloc(1, sizeof(tf_property *));
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

    for (i = 0; nodearr[i]; i++) {
        char *noderefpath = (char *)alloca(sizeof(char) * 
            (strlen(nodearr[i]->parent) + strlen(nodearr[i]->child) + 1));
        sprintf(noderefpath, "%s%s", nodearr[i]->parent, nodearr[i]->child);

        _append_resource_type(restypenode, nodearr[i]->resource.type);
        _append_resource(resnode, noderefpath, nodearr[i]->resource, svcarr, proparr, 1);
        _append_node(nodesnode, noderefpath, nodearr[i], 1);
    }

    nodearr = tf_free_node_array(nodearr);
    svcarr = tf_free_service_ref_array(svcarr);
    proparr = tf_free_property_array(proparr);

    gcs_pgctx_release(ctx);

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
 * @param ref       service reference info
 * @param instid    host instance ID
 */
void catalog_service_init(SoapRouter **router, const char *prefix, tf_service_ref *ref,
    const char *instid)
{
    char url[1024];

    (*router) = soap_router_new();
    soap_router_register_security(*router, (httpd_auth)_auth_ntlm);
    soap_router_set_tag(*router, instid);

    sprintf(url, "%s%s", prefix ? prefix : "", ref->service.relpath);
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

    gcslog_info("registered catalog service %s for host %s", url, instid);
}

