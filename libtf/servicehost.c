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

#include <tf/servicehost.h>

/**
 * Frees memory associated with a service host, but not the
 * service host itself.
 *
 * @param result    pointer to a service
 */
void tf_free_host(tf_host *result)
{
    if (result == NULL)
        return;

    if (result->description != NULL)
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
    if (result == NULL)
        return NULL;

    int i;
    for (i = 0; result[i] != NULL; i++) {
        tf_free_host(result[i]);
        free(result[i]);
    }

    free(result);
    return NULL;
}

