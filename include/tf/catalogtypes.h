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

#include <tf/locationtypes.h>

#define TF_CATALOG_ASSOCIATION_KEY_MAXLEN       257
#define TF_CATALOG_CHILD_ITEM_MAXLEN            25
#define TF_CATALOG_PARENT_PATH_MAXLEN           865
#define TF_CATALOG_PROPERTY_NAME_MAXLEN         401
#define TF_CATALOG_RESOURCE_ID_MAXLEN           37
#define TF_CATALOG_RESOURCE_NAME_MAXLEN         257
#define TF_CATALOG_RESOURCE_TYPE_MAXLEN         37
#define TF_CATALOG_RESOURCE_TYPE_NAME_MAXLEN    257

typedef struct {
    char id[TF_CATALOG_RESOURCE_TYPE_MAXLEN];
    char name[TF_CATALOG_RESOURCE_TYPE_NAME_MAXLEN];
    char *description;
} tf_resource_type;

typedef struct {
    char id[TF_CATALOG_RESOURCE_ID_MAXLEN];
    tf_resource_type type;
    char name[TF_CATALOG_RESOURCE_NAME_MAXLEN];
    char *description;
    int propertyid;
} tf_resource;

typedef struct {
    char parent[TF_CATALOG_PARENT_PATH_MAXLEN];
    char child[TF_CATALOG_CHILD_ITEM_MAXLEN];
    tf_resource resource;
    int fdefault;
} tf_node;

typedef struct {
    char id[TF_CATALOG_RESOURCE_ID_MAXLEN];
    char assockey[TF_CATALOG_ASSOCIATION_KEY_MAXLEN];
    tf_service service;
} tf_service_ref;

typedef struct {
    char *path;
    int depth;
} tf_path_spec;

typedef struct {
    int artifactid;
    int version;
    char property[TF_CATALOG_PROPERTY_NAME_MAXLEN];
    int type;
    int kindid;
    char *value;
} tf_property;

