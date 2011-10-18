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

#define TF_SERVICE_HOST_CONN_STR_MAXLEN         521
#define TF_SERVICE_HOST_ID_MAXLEN               37
#define TF_SERVICE_HOST_NAME_MAXLEN             129
#define TF_SERVICE_HOST_PATH_MAXLEN             261
#define TF_SERVICE_HOST_STATUS_REASON_MAXLEN    2049

typedef struct {
    char id[TF_SERVICE_HOST_ID_MAXLEN];
    char parent[TF_SERVICE_HOST_ID_MAXLEN];
    char name[TF_SERVICE_HOST_NAME_MAXLEN];
    char *description;
    char vdir[TF_SERVICE_HOST_PATH_MAXLEN];
    char rsrcdir[TF_SERVICE_HOST_PATH_MAXLEN];
    char connstr[TF_SERVICE_HOST_CONN_STR_MAXLEN];
    int status;
    char reason[TF_SERVICE_HOST_STATUS_REASON_MAXLEN];
    int features;
    char resource[TF_CATALOG_RESOURCE_ID_MAXLEN];
} tf_host;
