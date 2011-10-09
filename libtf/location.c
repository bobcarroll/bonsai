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
 * Frees memory associated with an access mapping, but not the
 * mapping itself.
 *
 * @param result    pointer to an access mapping
 */
void tf_free_access_map(tf_access_map *result)
{
    if (!result)
        return;

    if (result->apuri)
        free(result->apuri);

    result->apuri = NULL;
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
    for (i = 0; result[i]; i++) {
        tf_free_access_map(result[i]);
        free(result[i]);
    }

    free(result);
    return NULL;
}

/**
 * Frees memory associated with a service, but not the
 * service itself.
 *
 * @param result    pointer to a service
 */
void tf_free_service(tf_service *result)
{
    if (!result)
        return;

    if (result->description)
        free(result->description);

    result->description = NULL;
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
    for (i = 0; result[i]; i++) {
        tf_free_service(result[i]);
        free(result[i]);
    }

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

        free(result[i]);
    }

    free(result);
    return NULL;
}

