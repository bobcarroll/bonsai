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

#include <tf/catalogtypes.h>
#include <tf/errors.h>

#define TF_CATALOG_NODE_DEPTH_NONE      0
#define TF_CATALOG_NODE_DEPTH_SINGLE    1
#define TF_CATALOG_NODE_DEPTH_FULL      2

tf_error_t tf_catalog_fetch_nodes(tf_catalog_pathspec_array_t, const char * const *, 
    tf_catalog_node_array_t *);
tf_error_t tf_catalog_fetch_resources(const char * const *, int, tf_catalog_node_array_t *);
tf_error_t tf_catalog_fetch_services(tf_catalog_node_array_t, tf_catalog_service_array_t *);
tf_error_t tf_catalog_fetch_properties(tf_catalog_node_array_t, tf_catalog_property_array_t *);

