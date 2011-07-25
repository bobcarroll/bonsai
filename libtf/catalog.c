/**
 * GCS - open source group collaboration and application lifecycle management
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
 * @brief	Team Foundation catalog service functions
 *
 * @author	Bob Carroll (bob.carroll@alum.rit.edu)
 */

#include <string.h>
#include <stdlib.h>

#include <tf/catalog.h>
#include <tf/catalogdb.h>
#include <tf/location.h>

/**
 * Frees memory associated with a catalog node array, but not
 * the array itself.
 *
 * @param result
 */
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

/**
 * Frees memory associated with a catalog pathspec array, but not
 * the array itself.
 *
 * @param result
 */
void tf_catalog_free_pathspec_array(tf_catalog_pathspec_array_t result)
{
	if (result.items != NULL && result.count > 0) {
		int i;
		for (i = 0; i < result.count; i++) {
			if (result.items[i].path != NULL)
				free(result.items[i].path);

			result.items[i].path = NULL;
		}

		free(result.items);
	}

	result.items = NULL;
}

/**
 * Frees memory associated with a catalog resource, but not
 * the resource itself.
 *
 * @param result
 */
void tf_catalog_free_resource(tf_catalog_resource_t result)
{
	if (result.description != NULL)
		free(result.description);

	if (result.type.description != NULL)
		free(result.type.description);

	result.description = NULL;
	result.type.description = NULL;
}

/**
 * Frees memory associated with a catalog service array, but not
 * the array itself.
 *
 * @param result
 */
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

/**
 * Queries the catalog for nodes in the given path. Calling functions
 * should call tf_catalog_free_node_array() prior to freeing "result".
 *
 * @param patharr -- a null-terminated array of catalog paths, optionally with depth markers
 * @param types -- a null-terminated array of resource type ID strings to filter by
 * @param result -- pre-allocated output buffer for the results
 *
 * @returns TF_ERROR_SUCCESS or an error code
 */
tf_error_t tf_catalog_query_nodes(const char * const *patharr, const char * const *types, 
	tf_catalog_node_array_t *result)
{
	tf_catalog_pathspec_array_t pathspecs;
	tf_error_t dberr;
	int i;

	for (i = 0; patharr[i] != NULL; i++);
	pathspecs.items = (tf_catalog_pathspec_t *)calloc(i, sizeof(tf_catalog_pathspec_t));
	pathspecs.count = i;

	for (i = 0; i < pathspecs.count; i++) {
		tf_catalog_pathspec_t *ps = &pathspecs.items[i];
		const char *path = patharr[i];
		int pathlen = strlen(path);

		ps->depth = TF_CATALOG_NODE_DEPTH_NONE;

		if (path[pathlen - 2] == '*' && path[pathlen - 1] == '*') {
			ps->depth = TF_CATALOG_NODE_DEPTH_FULL;
			ps->path = (char *)calloc(pathlen - 1, sizeof(char));
			strncpy(ps->path, path, pathlen - 2);
		} else if (path[pathlen - 1] == '*') {
			ps->depth = TF_CATALOG_NODE_DEPTH_SINGLE;
			ps->path = (char *)calloc(pathlen, sizeof(char));
			strncpy(ps->path, path, pathlen - 1);
		} else
			ps->path = strdup(path);
	}

	dberr = tf_catalog_fetch_nodes(pathspecs, types, result);
	tf_catalog_free_pathspec_array(pathspecs);

	return dberr;
}

