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
 * @brief   Team Foundation location service functions
 *
 * @author  Bob Carroll (bob.carroll@alum.rit.edu)
 */

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include <tf/location.h>

/**
 * Finds the default access mapping in the given array. Calling functions
 * should free the return value;
 *
 * @param accmaparr     a null-terminated access mapping array
 *
 * @return the default access mapping moniker, or NULL if none is found
 */
char *tf_find_default_moniker(tf_access_map **accmaparr)
{
    if (!accmaparr || !accmaparr[0])
        return NULL;

    int i;
    for (i = 0; accmaparr[i]; i++) {
        if (accmaparr[i]->fdefault)
            return strdup(accmaparr[i]->moniker);
    }

    return NULL;
}

/**
 * Frees memory associated with an access mapping.
 *
 * @param result    pointer to an access mapping
 *
 * @return NULL
 */
void *tf_free_access_map(tf_access_map *result)
{
    if (result) {
        if (result->apuri)
            free(result->apuri);

        result->apuri = NULL;
        free(result);
    }

    return NULL;
}

/**
 * Frees memory associated with an access mapping array.
 *
 * @param result    a null-terminated access mapping array
 *
 * @return NULL
 */
void *tf_free_access_map_array(tf_access_map **result)
{
    if (!result)
        return NULL;

    int i;
    for (i = 0; result[i]; i++)
        tf_free_access_map(result[i]);

    free(result);
    return NULL;
}

/**
 * Frees memory associated with a service.
 *
 * @param result    pointer to a service
 *
 * @return NULL
 */
void *tf_free_service(tf_service *result)
{
    if (result) {
        if (result->description)
            free(result->description);

        result->description = NULL;
        free(result);
    }

    return NULL;
}

/**
 * Frees memory associated with a service array.
 *
 * @param result    a null-terminated service array
 *
 * @return NULL
 */
void *tf_free_service_array(tf_service **result)
{
    if (!result)
        return NULL;

    int i;
    for (i = 0; result[i]; i++)
        tf_free_service(result[i]);

    free(result);
    return NULL;
}

/**
 * Frees memory associated with a service filter array.
 *
 * @param result    a null-terminated service filter array
 *
 * @return NULL
 */
void *tf_free_service_filter_array(tf_service_filter **result)
{
    if (!result)
        return NULL;

    int i;
    for (i = 0; result[i]; i++) {
        if (result[i]->id)
            free(result[i]->id);

        if (result[i]->type)
            free(result[i]->type);

        result[i]->id = NULL;
        result[i]->type = NULL;

        free(result[i]);
    }

    free(result);
    return NULL;
}

/**
 * Creates a new access mapping. The caller is responsible for
 * freeing the result using tf_free_access_map().
 *
 * @param moniker   access mapping name
 * @param name      display name
 * @param uri       access point URI
 *
 * @return a new access mapping or NULL on error
 */
tf_access_map *tf_new_access_map(const char *moniker, const char *name, const char *uri)
{
    if (!moniker || !moniker[0] || !name || !name[0] || !uri || !uri[0])
        return NULL;

    tf_access_map *result = (tf_access_map *)malloc(sizeof(tf_access_map));
    bzero(result, sizeof(tf_access_map));

    strncpy(result->moniker, moniker, TF_LOCATION_ACCMAP_MONIKER_MAXLEN);
    strncpy(result->name, name, TF_LOCATION_ACCMAP_DISPLNAME_MAXLEN);

    result->apuri = strdup(uri);

    return result;
}

/**
 * Creates a new service definition. The caller is responsible for
 * freeing the result using tf_free_service().
 *
 * @param id    service ID
 * @param type  service type name
 * @param name  new service name
 * @param tool  service tool type
 *
 * @return a new service definition or NULL on error
 */
tf_service *tf_new_service(const char *id, const char *type, const char *name, const char *tool)
{
    if (!id || !id[0] || !type || !type[0] || !name || !name[0] || !tool || !tool[0])
        return NULL;

    tf_service *result = (tf_service *)malloc(sizeof(tf_service));
    bzero(result, sizeof(tf_service));

    strncpy(result->id, id, TF_LOCATION_SERVICE_ID_MAXLEN);
    strncpy(result->type, type, TF_LOCATION_SERVICE_TYPE_MAXLEN);
    strncpy(result->name, name, TF_LOCATION_SERVICE_NAME_MAXLEN);
    strncpy(result->tooltype, tool, TF_LOCATION_SERVICE_TOOL_TYPE_MAXLEN);

    return result;
}

/**
 * Sets the service relative path to the given URL.
 *
 * @param service   service structure to modify
 * @param url       new service url
 * @param relto     relative to path setting
 *
 * @return 1 on success, 0 on error
 */
int tf_set_service_url(tf_service *service, const char *url, int relto)
{
    if (!service || !url || strlen(url) >= TF_LOCATION_SERVICE_REL_PATH_MAXLEN)
        return 0;

    strncpy(service->relpath, url, TF_LOCATION_SERVICE_REL_PATH_MAXLEN);
    service->reltosetting = relto;

    return 1;
}

