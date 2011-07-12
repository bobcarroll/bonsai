
#pragma once

#include <tf/catalogtypes.h>
#include <tf/errors.h>

#define TF_CATALOG_NODE_DEPTH_NONE	0
#define TF_CATALOG_NODE_DEPTH_SINGLE	1
#define TF_CATALOG_NODE_DEPTH_FULL	2

tf_error_t tf_catalog_fetch_nodes(const char *, int, tf_catalog_node_array_t *);
tf_error_t tf_catalog_fetch_resources(const char *, tf_catalog_node_array_t *);
tf_error_t tf_catalog_fetch_services(tf_catalog_node_array_t, tf_catalog_service_array_t *);

