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

#define TF_LOCATION_ACCMAP_MONIKER_MAXLEN       129
#define TF_LOCATION_ACCMAP_DISPLNAME_MAXLEN     257

#define TF_LOCATION_SERVICE_NAME_MAXLEN         257
#define TF_LOCATION_SERVICE_ID_MAXLEN           37
#define TF_LOCATION_SERVICE_REL_PATH_MAXLEN     257
#define TF_LOCATION_SERVICE_TOOL_TYPE_MAXLEN    257
#define TF_LOCATION_SERVICE_TYPE_MAXLEN         257

#define TF_LOCATION_FILTER_SERVICE_ID       "567713db-d56d-4bb0-8f35-604e0e116174"
#define TF_LOCATION_FILTER_SERVICE_TYPE     "*"

#define TF_SERVICE_CATALOG_ID           "c2f9106f-127a-45b7-b0a3-e0ad8239a2a7"
#define TF_SERVICE_LOCATION_ID          "bf9cf1d0-24ac-4d35-aeca-6cd18c69c1fe"

/* When used in a project collection database, this points to the location
   service on a Team Foundation service host. */
#define TF_SERVICE_LOCATION2_ID        "8d299418-9467-402b-a171-9165e2f703e2"

#define TF_SERVICE_CATALOG_TYPE         "CatalogService"
#define TF_SERVICE_LOCATION_TYPE        "LocationService"
#define TF_SERVICE_REGISTRATION_TYPE    "RegistrationService"
#define TF_SERVICE_STATUS_TYPE          "StatusService"

#define TF_SERVICE_CATALOG_NAME         "Catalog Service"
#define TF_SERVICE_LOCATION_NAME        "Location Service"

#define TF_SERVICE_RELTO_CONTEXT            0
#define TF_SERVICE_RELTO_WEB_APPLICATION    1
#define TF_SERVICE_RELTO_FULLY_QUALIFIED    2

typedef struct {
    char moniker[TF_LOCATION_ACCMAP_MONIKER_MAXLEN];
    char name[TF_LOCATION_ACCMAP_DISPLNAME_MAXLEN];
    char *apuri;
    int fdefault;
} tf_access_map;

typedef struct {
    char id[TF_LOCATION_SERVICE_ID_MAXLEN];
    char type[TF_LOCATION_SERVICE_TYPE_MAXLEN];
    char name[TF_LOCATION_SERVICE_NAME_MAXLEN];
    int reltosetting;
    char relpath[TF_LOCATION_SERVICE_REL_PATH_MAXLEN];
    int singleton;
    char *description;
    char tooltype[TF_LOCATION_SERVICE_TOOL_TYPE_MAXLEN];
} tf_service;

typedef struct {
    char *id;
    char *type;
} tf_service_filter;

char *tf_find_default_moniker(tf_access_map **);

void *tf_free_access_map(tf_access_map *);
void *tf_free_access_map_array(tf_access_map **);
void *tf_free_service(tf_service *);
void *tf_free_service_array(tf_service **);
void *tf_free_service_filter_array(tf_service_filter **);

tf_access_map *tf_new_access_map(const char *, const char *, const char *);
tf_service *tf_new_service(const char *, const char *, const char *, const char *);
int tf_set_service_url(tf_service *, const char *, int relto);

tf_error tf_fetch_access_map(pgctx *, tf_access_map ***);
tf_error tf_fetch_services(pgctx *, tf_service_filter **, tf_service ***);

tf_error tf_add_access_map(pgctx *, tf_access_map *);
tf_error tf_add_service(pgctx *, tf_service *);

