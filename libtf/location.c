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
 * @param array     access mapping array
 *
 * @return the default access mapping moniker, or NULL if none is found
 */
char *tf_location_find_default_accmap(tf_location_accmap_array_t array)
{
    int i;
    for (i = 0; i < array.count; i++) {
        tf_location_accmap_t accmap = array.items[i];

        if (accmap.fdefault)
            return strdup(accmap.moniker);
    }

    return NULL;
}

/**
 * Frees memory associated with an access mapping, but not the
 * service itself.
 *
 * @param result
 */
void tf_location_free_accmap(tf_location_accmap_t result)
{
    if (result.apuri != NULL)
        free(result.apuri);

    result.apuri = NULL;
}

/**
 * Frees memory associated with a location service array, but not
 * the array itself.
 *
 * @param result
 */
void tf_location_free_accmap_array(tf_location_accmap_array_t result)
{
    if (result.items != NULL && result.count > 0) {
        int i;
        for (i = 0; i < result.count; i++)
            tf_location_free_accmap(result.items[i]);

        free(result.items);
    }

    result.items = NULL;
}

/**
 * Frees memory associated with a service, but not the
 * service itself.
 *
 * @param result
 */
void tf_location_free_service(tf_location_service_t result)
{
    if (result.description != NULL)
        free(result.description);

    result.description = NULL;
}

/**
 * Frees memory associated with a location service array, but not
 * the array itself.
 *
 * @param result
 */
void tf_location_free_service_array(tf_location_service_array_t result)
{
    if (result.items != NULL && result.count > 0) {
        int i;
        for (i = 0; i < result.count; i++)
            tf_location_free_service(result.items[i]);

        free(result.items);
    }

    result.items = NULL;
}

