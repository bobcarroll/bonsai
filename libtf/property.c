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
 * @brief   Team Foundation property service functions
 *
 * @author  Bob Carroll (bob.carroll@alum.rit.edu)
 */

#include <string.h>
#include <stdlib.h>

#include <log.h>

#include <tf/property.h>

/**
 * Frees memory associated with a property.
 *
 * @param result    pointer to a property
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
 * Frees memory associated with a property array.
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
 * Creates a new property.
 *
 * @param id            property ID
 * @param artifactid    unique artifact ID
 * @param value         new property value
 *
 * @return the new property structure
 */
tf_property *tf_new_property(int id, int artifactid, char *value)
{
    if (id < 1 || artifactid < 1 || !value) {
        log_debug("bad parameter: id=%d, artifactid=%d, value=%s", id, artifactid, value);
        return NULL;
    }

    int i;
    for (i = 0; i < _tf_prop_tbl_len && _tf_property_id[i] != id; i++)
        ;

    if (i == _tf_prop_tbl_len) {
        log_debug("property %d not found in ID table", id);
        return NULL;
    }

    tf_property *result = (tf_property *)malloc(sizeof(tf_property));
    bzero(result, sizeof(tf_property));

    result->artifactid = artifactid;
    result->propertyid = id;
    result->kindid = 2; /* TODO figure out what this is used for */
    result->value = strdup(value);

    strncpy(result->property, _tf_property_value[i], TF_PROPERTY_NAME_MAXLEN);

    return result;
}

