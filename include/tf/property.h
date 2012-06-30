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

#include <pgctxpool.h>

#include <tf/errors.h>

#define TF_PROPERTY_INSTANCE_ID_ID      10
#define TF_PROPERTY_INSTANCE_ID_VALUE   "InstanceId"

#define TF_PROPERTY_NAME_MAXLEN         401

static const int _tf_prop_tbl_len = 1;

static const int _tf_property_id[] = {
    TF_PROPERTY_INSTANCE_ID_ID
};

static const char *_tf_property_value[] = {
    TF_PROPERTY_INSTANCE_ID_VALUE
};

typedef struct {
    int artifactid;
    int version;
    int propertyid;
    char property[TF_PROPERTY_NAME_MAXLEN];
    int kindid;
    char *value;
} tf_property;

void *tf_free_property(tf_property *);
void *tf_free_property_array(tf_property **);

tf_property *tf_new_property(int, int, char *);

tf_error tf_gen_artifact_id(pgctx *, int *);
tf_error tf_add_property(pgctx *, tf_property *);

