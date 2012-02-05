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

#pragma once

#define TF_ERROR_SUCCESS            0       /* no error */
#define TF_ERROR_BAD_PARAMETER      1       /* a function encountered an invalid argument */
#define TF_ERROR_PARAM_TOO_LONG     2       /* an argument or parameter was longer than expected */
#define TF_ERROR_PG_FAILURE         3       /* a generic error for failed database queries */
#define TF_ERROR_NOT_FOUND          4       /* the requested resources was not found in the database */
#define TF_ERROR_INTERNAL           5       /* a generic internal error */
#define TF_ERROR_ACCESS_DENIED      6       /* user does not have permission to access the requested resource */

typedef int tf_error;

