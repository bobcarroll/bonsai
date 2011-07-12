
#include <string.h>
#include <stdlib.h>

#include <tf/catalog.h>
#include <tf/catalogdb.h>
#include <tf/location.h>

void tf_catalog_free_node_array(tf_catalog_node_array_t result)
{
	if (result.items != NULL && result.count > 0) {
		int i;
		for (i = 0; i < result.count; i++)
			tf_catalog_free_resource(result.items[i].resource);

		free(result.items);
	}

	result.items = NULL;
}


void tf_catalog_free_resource(tf_catalog_resource_t result)
{
	if (result.description != NULL)
		free(result.description);

	if (result.type.description != NULL)
		free(result.type.description);

	result.description = NULL;
	result.type.description = NULL;
}

void tf_catalog_free_service_array(tf_catalog_service_array_t result)
{
	if (result.items != NULL && result.count > 0) {
		int i;
		for (i = 0; i < result.count; i++)
			tf_location_free_service(result.items[i].service);

		free(result.items);
	}

	result.items = NULL;
}

tf_error_t tf_catalog_query_nodes(const char *path, tf_catalog_node_array_t *result)
{
	char *newpath = NULL;
	int pathlen = strlen(path);
	int depth = TF_CATALOG_NODE_DEPTH_NONE;

	if (path[pathlen - 2] == '*' && path[pathlen - 1] == '*') {
		depth = TF_CATALOG_NODE_DEPTH_FULL;

		newpath = (char *)alloca(sizeof(char) * (pathlen - 1));
		strncpy(newpath, path, pathlen - 2);
		newpath[pathlen - 2] = '\0';
	} else if (path[pathlen - 1] == '*') {
		depth = TF_CATALOG_NODE_DEPTH_SINGLE;

		newpath = (char *)alloca(sizeof(char) * pathlen);
		strncpy(newpath, path, pathlen - 1);
		newpath[pathlen - 1] = '\0';
	} else {
		newpath = (char *)alloca(sizeof(char) * (pathlen + 1));
		strcpy(newpath, path);
	}

	return tf_catalog_fetch_nodes(newpath, depth, result);
}

