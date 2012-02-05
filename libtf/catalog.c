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
#include <stdio.h>
#include <uuid/uuid.h>

#include <base64.h>

#include <tf/catalog.h>
#include <tf/location.h>

/**
 * Frees memory associated with a catalog node.
 *
 * @param result    pointer to a catalog node
 */
void *tf_free_node(tf_node *result)
{
    if (result) {
        if (result->resource.description)
            free(result->resource.description);

        if (result->resource.type.description)
            free(result->resource.type.description);

        result->resource.description = NULL;
        result->resource.type.description = NULL;

        free(result);
    }

    return NULL;
}

/**
 * Frees memory associated with a catalog node array.
 *
 * @param result    a null-terminated catalog node array
 */
void *tf_free_node_array(tf_node **result)
{
    if (!result)
        return NULL;

    int i;
    for (i = 0; result[i]; i++)
        tf_free_node(result[i]);

    free(result);
    return NULL;
}

/**
 * Frees memory associated with a catalog property.
 *
 * @param result    pointer to a catalog property
 *
 * @return NULL
 */
void *tf_free_property(tf_property *result)
{
    if (result) {
        if (result->value)
            free(result->value);

        result->value = NULL;
        free(result);
    }

    return NULL;
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
    if (!result)
        return NULL;

    int i;
    for (i = 0; result[i]; i++)
        tf_free_property(result[i]);

    free(result);
    return NULL;
}

/**
 * Frees memory associated with a service reference.
 *
 * @param result    a service reference structure
 *
 * @return NULL
 */
void *tf_free_service_ref(tf_service_ref *result)
{
    if (result) {
        if (result->service.description)
            free(result->service.description);

        result->service.description = NULL;
        free(result);
    }

    return NULL;
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
    if (!result)
        return NULL;

    int i;
    for (i = 0; result[i]; i++)
        tf_free_service_ref(result[i]);

    free(result);
    return NULL;
}

/**
 * Queries the catalog for nodes in the given path. Calling functions
 * should call tf_free_node_array() to free "result".
 *
 * @param ctx       current database context
 * @param patharr   a null-terminated array of catalog paths, optionally with depth markers
 * @param types     a null-terminated array of resource type ID strings to filter by
 * @param result    pointer to an output buffer for the results
 *
 * @return TF_ERROR_SUCCESS or an error code
 */
tf_error tf_query_nodes(pgctx *ctx, const char * const *patharr, const char * const *types, tf_node ***result)
{
    tf_path_spec **pathspecs = NULL;
    tf_error dberr;
    int i;

    if (!ctx || !patharr || !types || !result)
        return TF_ERROR_BAD_PARAMETER;

    for (i = 0; patharr[i]; i++)
        ;
    pathspecs = (tf_path_spec **)calloc(i + 1, sizeof(tf_path_spec *));

    for (i = 0; patharr[i]; i++) {
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

    dberr = tf_fetch_nodes(ctx, pathspecs, types, result);

    for (i = 0; pathspecs[i]; i++) {
        if (pathspecs[i]->path)
            free(pathspecs[i]->path);

        free(pathspecs[i]);
    }

    free(pathspecs);
    pathspecs = NULL;

    return dberr;
}

/**
 * Queries the catalog for nodes of a single type in the given path. Calling functions
 * should call tf_free_node_array() to free "result".
 *
 * @param ctx       current database context
 * @param path      a catalog path, optionally with a depth marker
 * @param type      a resource type ID strings to filter by
 * @param result    pointer to an output buffer for the results
 *
 * @return TF_ERROR_SUCCESS or an error code
 */
tf_error tf_query_tree(pgctx *ctx, const char *path, const char *type, tf_node ***result)
{
    tf_error dberr;

    if (!ctx || !path || !type || !result)
        return TF_ERROR_BAD_PARAMETER;

    char **pathspec = (char **)calloc(2, sizeof(char *));
    pathspec[0] = (char *)alloca(sizeof(char) * 27);
    snprintf(pathspec[0], 27, "%s**", path);

    char **typearr = (char **)calloc(2, sizeof(char *));
    typearr[0] = strdup(type);

    dberr = tf_query_nodes(
            ctx,
            (const char * const *)pathspec,
            (const char * const *)typearr,
            result);

    free(pathspec);
    free(typearr[0]);
    free(typearr);

    return dberr;
}

/**
 * Creates a new catalog node structure. The caller is responsible for freeing
 * the result using tf_free_node().
 *
 * @param parent    optional parent catalog node
 * @param type      resource type ID
 * @param name      new resource name
 * @param desc      optional resource description
 *
 * @return a new catalog node or NULL on error
 */
tf_node *tf_new_node(tf_node *parent, const char *type, const char *name, const char *desc)
{
    tf_node *result;
    uuid_t newid;
    char newid_s[1024];
    int n, i;

    if (!type || !type[0] || !name || !name[0])
        return NULL;

    for (i = 0; i < _tf_rsrc_tbl_len && strcmp(_tf_rsrc_type_id[i], type) == 0; i++)
        ;
    if (i > _tf_rsrc_tbl_len)
        return NULL;
    
    result = (tf_node *)malloc(sizeof(tf_node));
    bzero(result, sizeof(tf_node));

    if (parent) {
        strncpy(result->parent, parent->parent, TF_CATALOG_PARENT_PATH_MAXLEN);
        n = TF_CATALOG_PARENT_PATH_MAXLEN - strlen(result->parent) - 1;
        strncat(result->parent, parent->child, n);
    }

    uuid_generate(newid);
    base64_ntop(newid, sizeof(uuid_t), result->child, TF_CATALOG_CHILD_ITEM_MAXLEN);

    strncpy(result->resource.type.id, type, TF_CATALOG_RESOURCE_TYPE_MAXLEN);
    strncpy(result->resource.type.name, _tf_rsrc_type_name[i], TF_CATALOG_RESOURCE_TYPE_NAME_MAXLEN);
    result->resource.type.description = strdup(_tf_rsrc_type_desc[i]);

    uuid_generate(newid);
    uuid_unparse_lower(newid, newid_s);
    strncpy(result->resource.id, newid_s, TF_CATALOG_RESOURCE_ID_MAXLEN);

    strncpy(result->resource.name, name, TF_CATALOG_RESOURCE_NAME_MAXLEN);

    if (desc)
        result->resource.description = strdup(desc);

    return result;
}

/**
 * Creates a new catalog service reference. The caller is responsible for freeing
 * the result using tf_free_service_ref().
 *
 * @param rsrc      catalog node resource to associate
 * @param service   service definition structure
 * @param assockey  reference association key
 *
 * @return a new catalog service reference or NULL on error
 */
tf_service_ref *tf_new_service_ref(tf_resource *rsrc, tf_service *service, const char *assockey)
{
    if (!rsrc || !service || !assockey || !assockey[0])
        return NULL;

    tf_service_ref *result = (tf_service_ref *)malloc(sizeof(tf_service_ref));
    bzero(result, sizeof(tf_service_ref));

    strncpy(result->id, rsrc->id, TF_CATALOG_RESOURCE_ID_MAXLEN);
    strncpy(result->assockey, assockey, TF_CATALOG_ASSOCIATION_KEY_MAXLEN);

    memcpy(&result->service, service, sizeof(tf_service));

    return result;
}

