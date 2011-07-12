
#pragma once

#include <tf/catalogtypes.h>
#include <tf/errors.h>

void tf_catalog_free_node_array(tf_catalog_node_array_t);
void tf_catalog_free_resource(tf_catalog_resource_t);
void tf_catalog_free_service_array(tf_catalog_service_array_t);
tf_error_t tf_catalog_query_nodes(const char *, tf_catalog_node_array_t *);

