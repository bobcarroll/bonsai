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
 * @brief   Team Foundation service host functions
 *
 * @author  Bob Carroll (bob.carroll@alum.rit.edu)
 */

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <uuid/uuid.h>

#include <tf/servicehost.h>

/**
 * Frees memory associated with a service host, but not the
 * service host itself.
 *
 * @param result    pointer to a service
 */
void tf_free_host(tf_host *result)
{
    if (!result)
        return;

    if (result->description)
        free(result->description);

    result->description = NULL;
}

/**
 * Frees memory associated with a service host array.
 *
 * @param result    a null-terminated service host array
 *
 * @return NULL
 */
void *tf_free_host_array(tf_host **result)
{
    if (!result)
        return NULL;

    int i;
    for (i = 0; result[i]; i++) {
        tf_free_host(result[i]);
        free(result[i]);
    }

    free(result);
    return NULL;
}

/**
 * Creates a new service host. The caller is responsible for
 * freeing the result using tf_free_host().
 *
 * @param parent    optional parent service host
 * @param name      name of the new service host
 * @param connstr   database connection string
 * @param rsrc      catalog resource for the host
 *
 * @return a new service host or NULL on error
 */
tf_host *tf_new_host(tf_host *parent, const char *name, const char *connstr, tf_resource *rsrc)
{
    tf_host *result;
    uuid_t newid;
    char newid_s[1024];

    if (!name || !name[0] || !connstr || !connstr[0] || !rsrc || !rsrc->id[0])
        return NULL;

    result = (tf_host *)malloc(sizeof(tf_host));
    bzero(result, sizeof(tf_host));

    if (parent)
        strncpy(result->parent, parent->id, TF_SERVICE_HOST_ID_MAXLEN);

    uuid_generate(newid);
    uuid_unparse_lower(newid, newid_s);
    strncpy(result->id, newid_s, TF_SERVICE_HOST_ID_MAXLEN);

    strncpy(result->name, name, TF_SERVICE_HOST_NAME_MAXLEN);
    strncpy(result->connstr, connstr, TF_SERVICE_HOST_CONN_STR_MAXLEN);
    strncpy(result->resource, rsrc->id, TF_CATALOG_RESOURCE_ID_MAXLEN);

    return result;
}

