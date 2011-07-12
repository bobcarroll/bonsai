
#pragma once

#include <tf/locationtypes.h>

#define TF_CATALOG_ASSOCIATION_KEY_MAXLEN	257
#define TF_CATALOG_CHILD_ITEM_MAXLEN		25
#define TF_CATALOG_PARENT_PATH_MAXLEN		865
#define TF_CATALOG_RESOURCE_ID_MAXLEN		37
#define TF_CATALOG_RESOURCE_NAME_MAXLEN		257
#define TF_CATALOG_RESOURCE_TYPE_MAXLEN		37
#define TF_CATALOG_RESOURCE_TYPE_NAME_MAXLEN	257

#define TF_CATALOG_ORGANIZATION_ROOT	"3eYRYkJOok6GHrKam0AcAA=="

typedef struct {
	char id[TF_CATALOG_RESOURCE_TYPE_MAXLEN];
	char name[TF_CATALOG_RESOURCE_TYPE_NAME_MAXLEN];
	char *description;
} tf_catalog_resource_type_t;

typedef struct {
	char id[TF_CATALOG_RESOURCE_ID_MAXLEN];
	tf_catalog_resource_type_t type;
	char name[TF_CATALOG_RESOURCE_NAME_MAXLEN];
	char *description;
} tf_catalog_resource_t;

typedef struct {
	char parent[TF_CATALOG_PARENT_PATH_MAXLEN];
	char child[TF_CATALOG_CHILD_ITEM_MAXLEN];
	tf_catalog_resource_t resource;
	int fdefault;
} tf_catalog_node_t;

typedef struct {
	tf_catalog_node_t *items;
	int count;
} tf_catalog_node_array_t;

typedef struct {
	char id[TF_CATALOG_RESOURCE_ID_MAXLEN];
	char assockey[TF_CATALOG_ASSOCIATION_KEY_MAXLEN];
	tf_location_service_t service;
} tf_catalog_service_t;

typedef struct {
	tf_catalog_service_t *items;
	int count;
} tf_catalog_service_array_t;

