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

#pragma once

#include <tf/catalogtypes.h>
#include <tf/errors.h>

#define TF_CATALOG_QUERY_EXPAND_DEPS	1
#define TF_CATALOG_QUERY_INC_PARENTS	2

#define TF_CATALOG_ORGANIZATION_ROOT	"3eYRYkJOok6GHrKam0AcAA=="

void tf_catalog_free_node_array(tf_catalog_node_array_t);
void tf_catalog_free_pathspec_array(tf_catalog_pathspec_array_t);
void tf_catalog_free_resource(tf_catalog_resource_t);
void tf_catalog_free_service_array(tf_catalog_service_array_t);
tf_error_t tf_catalog_query_nodes(const char * const *, const char * const *, 
	tf_catalog_node_array_t *);

