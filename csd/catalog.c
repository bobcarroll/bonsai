
#include <cs/catalog.h>

#include <gcs/log.h>

#include <tf/catalogdb.h>
#include <tf/webservices.h>
#include <tf/xml.h>

static void _catalog_append_resource_type(xmlNode *parent, tf_catalog_resource_type_t type)
{
	xmlNode *crtnode = xmlNewChild(parent, NULL, "CatalogResourceType", NULL);
	xmlNewProp(crtnode, "Identifier", type.id);
	xmlNewProp(crtnode, "DisplayName", type.name);

	if (type.description != NULL)
		xmlNewChild(crtnode, NULL, "Description", type.description);
}

static void _catalog_append_service_ref(xmlNode *parent, tf_catalog_service_t ref)
{
	char reltosettingstr[5];
	sprintf(reltosettingstr, "%d", ref.service.reltosetting);

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

static void _catalog_append_resource(xmlNode *parent, const char *path, tf_catalog_resource_t resource,
	tf_catalog_service_array_t services, int matched)
{
	int i;

	xmlNode *crnode = xmlNewChild(parent, NULL, "CatalogResource", NULL);
	xmlNewProp(crnode, "Identifier", resource.id);
	xmlNewProp(crnode, "DisplayName", resource.name);
	xmlNewProp(crnode, "ResourceTypeIdentifier", resource.type.id);
	xmlNewProp(crnode, "TempCorrelationId", resource.id);
	xmlNewProp(crnode, "ctype", "0");
	xmlNewProp(crnode, "MatchedQuery", matched ? "true" : "false");

	if (resource.description != NULL)
		xmlNewChild(crnode, NULL, "Description", resource.description);

	xmlNode *refsnode = xmlNewChild(crnode, NULL, "CatalogServiceReferences", NULL);

	for (i = 0; i < services.count; i++) {
		tf_catalog_service_t cursvc = services.items[i];

		if (strcmp(resource.id, cursvc.id) != 0)
			continue;

		_catalog_append_service_ref(refsnode, cursvc);
	}

	xmlNode *propsnode = xmlNewChild(crnode, NULL, "Properties", NULL);
	xmlNode *kvoss = xmlNewChild(propsnode, NULL, "KeyValueOfStringString", NULL);
	xmlNewChild(kvoss, NULL, "Key", "InstanceId");
	xmlNewChild(kvoss, NULL, "Value", "75ce4f73-3c70-4770-8e95-a4413d2d6a78");

	xmlNode *refpathnode = xmlNewChild(crnode, NULL, "NodeReferencePaths", NULL);
	xmlNewChild(refpathnode, NULL, "string", path);
}

static void _catalog_append_node(xmlNode *parent, const char *path, tf_catalog_node_t node, int matched)
{
	xmlNode *cnnode = xmlNewChild(parent, NULL, "CatalogNode", NULL);
	xmlNewProp(cnnode, "FullPath", path);
	xmlNewProp(cnnode, "default", node.fdefault ? "true" : "false");
	xmlNewProp(cnnode, "ResourceIdentifier", node.resource.id);
	xmlNewProp(cnnode, "ParentPath", node.parent);
	xmlNewProp(cnnode, "ChildItem", node.child);
	xmlNewProp(cnnode, "NodeDependenciesIncluded", "false");
	xmlNewProp(cnnode, "ctype", "0");
	xmlNewProp(cnnode, "MatchedQuery", matched ? "true" : "false");
	xmlNewChild(cnnode, NULL, "NodeDependencies", NULL);
}

static herror_t _catalog_query_resources(SoapCtx *req, SoapCtx *res)
{
	char *guid = NULL;
	tf_catalog_node_array_t nodearr;
	tf_catalog_service_array_t svcarr;
	tf_error_t dberr;
	int i;
	int f = 0;

	xmlNode *cmd = soap_env_get_method(req->env);
	soap_env_new_with_method(cmd->ns->href, "QueryResourcesResponse", &res->env);
	xmlNode *body = req->env->body;

	xmlNode *arg = tf_xml_find_first(body, "m", TF_DEFAULT_NAMESPACE, "//m:resourceIdentifiers//m:guid");
	if (arg != NULL && arg->children != NULL)
		guid = arg->children->content;

	arg = tf_xml_find_first(body, "m", TF_DEFAULT_NAMESPACE, "//m:resourceTypeIdentifiers//m:guid");
	if (arg != NULL && arg->children != NULL) {
		guid = arg->children->content;
		f = 1;
	}

	xmlNode *result = xmlNewChild(res->env->body->children->next, NULL, "QueryResourcesResult", NULL);
	xmlNode *restypenode = xmlNewChild(result, NULL, "CatalogResourceTypes", NULL);
	xmlNode *resnode = xmlNewChild(result, NULL, "CatalogResources", NULL);
	xmlNode *nodesnode = xmlNewChild(result, NULL, "CatalogNodes", NULL);
	xmlNewChild(result, NULL, "DeletedResources", NULL);
	xmlNewChild(result, NULL, "DeletedNodeResources", NULL);
	xmlNewChild(result, NULL, "DeletedNodes", NULL);
	xmlNewChild(result, NULL, "LocationServiceLastChangeId", "2565");

	dberr = tf_catalog_fetch_resources(guid, &nodearr);
	dberr = tf_catalog_fetch_services(nodearr, &svcarr);

	for (i = 0; i < nodearr.count; i++) {
		tf_catalog_node_t curnode = nodearr.items[i];

		if (f && strcmp(curnode.resource.type.id, guid) != 0)
			continue;
		else if (!f && strcmp(curnode.resource.id, guid) != 0)
			continue;

		char *noderefpath = (char *)alloca(sizeof(char) * (strlen(curnode.parent) + strlen(curnode.child) + 1));
		sprintf(noderefpath, "%s%s", curnode.parent, curnode.child);

		_catalog_append_resource_type(restypenode, curnode.resource.type);
		_catalog_append_resource(resnode, noderefpath, curnode.resource, svcarr, 1);
		_catalog_append_node(nodesnode, noderefpath, curnode, 0);
	}

	tf_catalog_free_node_array(nodearr);
	tf_catalog_free_service_array(svcarr);

	return H_OK;
}

static herror_t _catalog_query_nodes(SoapCtx *req, SoapCtx *res)
{
	char *pathspec = NULL;
	char *typefilter = NULL;
	tf_catalog_node_array_t nodearr;
	tf_catalog_service_array_t svcarr;
	tf_error_t dberr;
	int i;

	xmlNode *cmd = soap_env_get_method(req->env);
	soap_env_new_with_method(cmd->ns->href, "QueryNodesResponse", &res->env);
	xmlNode *body = req->env->body;

	/* TODO */
	xmlNode *arg = tf_xml_find_first(body, "m", TF_DEFAULT_NAMESPACE, "//m:QueryNodes/m:pathSpecs/m:string");
	if (arg != NULL && arg->children != NULL) {
		pathspec = arg->children->content;
	}

	/* TODO */
	arg = tf_xml_find_first(body, "m", TF_DEFAULT_NAMESPACE, "//m:QueryNodes/m:resourceTypeFilters/m:guid");
	if (arg != NULL && arg->children != NULL) {
		typefilter = arg->children->content;
	}

	/* TODO property filter */

	xmlNode *result = xmlNewChild(res->env->body->children->next, NULL, "QueryNodesResult", NULL);
	xmlNode *restypenode = xmlNewChild(result, NULL, "CatalogResourceTypes", NULL);
	xmlNode *resnode = xmlNewChild(result, NULL, "CatalogResources", NULL);
	xmlNode *nodesnode = xmlNewChild(result, NULL, "CatalogNodes", NULL);
	xmlNewChild(result, NULL, "DeletedResources", NULL);
	xmlNewChild(result, NULL, "DeletedNodeResources", NULL);
	xmlNewChild(result, NULL, "DeletedNodes", NULL);
	xmlNewChild(result, NULL, "LocationServiceLastChangeId", "2565");

	dberr = tf_catalog_query_nodes(pathspec, &nodearr);
	dberr = tf_catalog_fetch_services(nodearr, &svcarr);

	for (i = 0; i < nodearr.count; i++) {
		tf_catalog_node_t curnode = nodearr.items[i];

		if (typefilter != NULL && strcmp(curnode.resource.type.id, typefilter) != 0)
			continue;

		char *noderefpath = (char *)alloca(sizeof(char) * (strlen(curnode.parent) + strlen(curnode.child) + 1));
		sprintf(noderefpath, "%s%s", curnode.parent, curnode.child);

		_catalog_append_resource_type(restypenode, curnode.resource.type);
		_catalog_append_resource(resnode, noderefpath, curnode.resource, svcarr, 1);
		_catalog_append_node(nodesnode, noderefpath, curnode, 1);
	}

	tf_catalog_free_node_array(nodearr);
	tf_catalog_free_service_array(svcarr);

	return H_OK;
}

static int _catalog_auth_ntlm(SoapEnv *env, const char *user, const char *passwd)
{
	return 0;
}

void catalog_service_init(SoapRouter **router, const char *prefix)
{
	char url[1024];

	(*router) = soap_router_new();
	soap_router_register_security(*router, (httpd_auth)_catalog_auth_ntlm);

	sprintf(url, "%s%s", prefix != NULL ? prefix : "", TF_CATALOG_SERVICE_ENDPOINT);
	soap_server_register_router(*router, url);

	soap_router_register_service(
		*router,
		_catalog_query_resources,
		"QueryResources",
		TF_DEFAULT_NAMESPACE);
	soap_router_register_service(
		*router,
		_catalog_query_nodes,
		"QueryNodes",
		TF_DEFAULT_NAMESPACE);

	gcslog_info("registered catalog services");
}

