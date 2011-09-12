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
 * @brief   Team Foundation catalog service functions
 *
 * @author  Bob Carroll (bob.carroll@alum.rit.edu)
 */

#include <string.h>
#include <stdlib.h>

#include <tf/catalog.h>
#include <tf/catalogdb.h>
#include <tf/location.h>

/**
 * Frees memory associated with a catalog node array.
 *
 * @param result    a null-terminated catalog node array
 */
void *tf_free_node_array(tf_node **result)
{
    if (result == NULL)
        return NULL;

    int i;
    for (i = 0; result[i] != NULL; i++) {
        tf_free_resource(result[i]->resource);
        free(result[i]);
    }

    free(result);
    return NULL;
}

/**
 * Frees memory associated with a catalog path spec array.
 *
 * @param result    a null-terminated path spec array
 *
 * @return NULL
 */
void *tf_free_path_spec_array(tf_path_spec **result)
{
    if (result == NULL)
        return NULL;

    int i;
    for (i = 0; result[i] != NULL; i++) {
        if (result[i]->path != NULL)
            free(result[i]->path);
    }

    free(result);
    return NULL;
}

/**
 * Frees memory associated with a catalog property, but not
 * the property itself.
 *
 * @param result    pointer to a catalog property
 */
void tf_free_property(tf_property *result)
{
    if (result == NULL)
        return;

    if (result->value != NULL)
        free(result->value);

    result->value = NULL;
}

/**
 * Frees memory associated with a catalog property array.
 *
 * @param result    a null-terminated property array
 *
 * @return NULL
 */
void *tf_free_property_array(tf_property **result)
{
    if (result == NULL)
        return NULL;

    int i;
    for (i = 0; result[i] != NULL; i++) {
        tf_free_property(result[i]);
        free(result[i]);
    }

    free(result);
    return NULL;
}

/**
 * Frees memory associated with a catalog resource, but not
 * the resource itself.
 *
 * @param result    a catalog resource
 */
void tf_free_resource(tf_resource result)
{
    if (result.description != NULL)
        free(result.description);

    if (result.type.description != NULL)
        free(result.type.description);

    result.description = NULL;
    result.type.description = NULL;
}

/**
 * Frees memory associated with a service reference array.
 *
 * @param result    a null-terminated service reference array
 *
 * @return NULL
 */
void *tf_free_service_ref_array(tf_service_ref **result)
{
    if (result == NULL)
        return NULL;

    int i;
    for (i = 0; result[i] != NULL; i++) {
        tf_free_service(&result[i]->service);
        free(result[i]);
    }

    free(result);
    return NULL;
}

/**
 * Queries the catalog for nodes in the given path. Calling functions
 * should call tf_free_node_array() to free "result".
 *
 * @param patharr   a null-terminated array of catalog paths, optionally with depth markers
 * @param types     a null-terminated array of resource type ID strings to filter by
 * @param result    pointer to an output buffer for the results
 *
 * @return TF_ERROR_SUCCESS or an error code
 */
tf_error tf_query_nodes(const char * const *patharr, const char * const *types, tf_node ***result)
{
    tf_path_spec **pathspecs = NULL;
    tf_error dberr;
    int i;

    for (i = 0; patharr[i] != NULL; i++);
    pathspecs = (tf_path_spec **)calloc(i, sizeof(tf_path_spec *));

    for (i = 0; patharr[i] != NULL; i++) {
        const char *path = patharr[i];
        int pathlen = strlen(path);

        pathspecs[i] = (tf_path_spec *)malloc(sizeof(tf_path_spec));
        bzero(pathspecs[i], sizeof(tf_path_spec));

        pathspecs[i]->depth = TF_CATALOG_NODE_DEPTH_NONE;

        if (path[pathlen - 2] == '*' && path[pathlen - 1] == '*') {
            pathspecs[i]->depth = TF_CATALOG_NODE_DEPTH_FULL;
            pathspecs[i]->path = (char *)calloc(pathlen - 1, sizeof(char));
            strncpy(pathspecs[i]->path, path, pathlen - 2);
        } else if (path[pathlen - 1] == '*') {
            pathspecs[i]->depth = TF_CATALOG_NODE_DEPTH_SINGLE;
            pathspecs[i]->path = (char *)calloc(pathlen, sizeof(char));
            strncpy(pathspecs[i]->path, path, pathlen - 1);
        } else
            pathspecs[i]->path = strdup(path);
    }

    dberr = tf_fetch_nodes(pathspecs, types, result);
    pathspecs = tf_free_path_spec_array(pathspecs);

    return dberr;
}

