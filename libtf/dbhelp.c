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
 * @brief	database helper functions
 *
 * @author	Bob Carroll (bob.carroll@alum.rit.edu)
 */

#include <stdio.h>
#include <string.h>

#include <gcs/log.h>

#include <tf/errors.h>

/**
 * Builds comma-seperated list of string from the given list
 * for use in SQL queries.
 *
 * @param list -- a null-terminated array of strings
 * @param buf -- the output character buffer
 * @param buflen -- the size of the output buffer
 * @param curpos -- pointer to the buffer cursor position
 *
 * @return TF_ERROR_SUCCESS or an error code
 */
tf_error_t tf_db_build_list(const char * const *list, char *buf, const int buflen, int *curpos)
{
	int r = buflen - *curpos, i = 0;

	for (i = 0; list[i] != NULL; i++) {
		if (i > 0) {
			if (snprintf(buf + *curpos, r, ", ") >= r)
				return TF_ERROR_PARAM_TOO_LONG;

			*curpos = strlen(buf);
			r = buflen - *curpos;
		}

		if (snprintf(buf + *curpos, r, "'%s'", list[i]) >= r)
			return TF_ERROR_PARAM_TOO_LONG;

		*curpos = strlen(buf);
		r = buflen - *curpos;
	}

	gcslog_trace("SQL list fragment: %s", buf);
	return TF_ERROR_SUCCESS;
}

