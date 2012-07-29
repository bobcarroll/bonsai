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
 * @brief   utility functions
 *
 * @author  Bob Carroll (bob.carroll@alum.rit.edu)
 */

#include <string.h>
#include <stdlib.h>

/**
 * Combines path elements into a single path. The caller must free the result.
 *
 * @param path1     leading path element (may end with a path seperator)
 * @param path2     trailing path element
 *
 * @return the combined path
 */
char *combine(const char *path1, const char *path2)
{
    int len1, len2, lenr, sep = 0;
    char *result = NULL;

    if (!path1 && !path2)
        return NULL;
    else if (path1 && !path2)
        return strdup(path1);
    else if (!path1 && path2)
        return strdup(path2);

    len1 = strlen(path1);
    len2 = strlen(path2);
    lenr = len1 + len2;

    if (path1[len1 - 1] != '/')
        lenr++;
    else
        sep = 1;

    result = (char *)calloc(lenr + 1, sizeof(char));

    strncpy(result, path1, lenr);
    lenr -= len1;

    if (!sep) {
        strncat(result, "/", lenr);
        lenr--;
    }

    strncat(result, path2, lenr);
    return result;
}

