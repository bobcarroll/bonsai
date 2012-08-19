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

#define TF_SERVICE_AUTHORIZATION_ID     "6373ee32-aad4-4bf9-9ec8-72201ab1c45c"
#define TF_SERVICE_AUTHORIZATION3_ID    "da728b84-3c54-46bb-a423-8a5fb526a722"
#define TF_SERVICE_CATALOG_ID           "c2f9106f-127a-45b7-b0a3-e0ad8239a2a7"
#define TF_SERVICE_COMMON_STRUCT_ID     "d9c3f8ff-8938-4193-919b-7588e81cb730"
#define TF_SERVICE_COMMON_STRUCT3_ID    "02ea5fcc-1e40-4d94-a8e5-ed62c15cb676"
#define TF_SERVICE_LOCATION_ID          "bf9cf1d0-24ac-4d35-aeca-6cd18c69c1fe"
#define TF_SERVICE_PROCESS_TEMPL_ID     "75ab998e-7f09-479e-9559-b86b5b06f688"
#define TF_SERVICE_REGISTRATION_ID      "b8f97328-80d2-412d-9810-67c5a3f4190f"
#define TF_SERVICE_STATUS_ID            "d395630a-d784-45b9-b8d1-f4b82042a8d0"
#define TF_SERVICE_VERSION_CONTR_ID     "b2b178f5-bef9-460d-a5cf-35bcc0281cc4"
#define TF_SERVICE_VERSION_CONTR3_ID    "ec9b0153-ee54-450e-b6e0-664ecb033c99"

/* When used in a project collection database, this points to the location
   service hosted by the project collection daemon. */
#define TF_SERVICE_LOCATION2_ID        "8d299418-9467-402b-a171-9165e2f703e2"

#define TF_SERVICE_AUTHORIZATION_TYPE   "Authorization"
#define TF_SERVICE_AUTHORIZATION3_TYPE  "Authorization3"
#define TF_SERVICE_CATALOG_TYPE         "CatalogService"
#define TF_SERVICE_COMMON_STRUCT_TYPE   "CommonStructure"
#define TF_SERVICE_COMMON_STRUCT3_TYPE  "CommonStructure3"
#define TF_SERVICE_LOCATION_TYPE        "LocationService"
#define TF_SERVICE_PROCESS_TEMPL_TYPE   "ProcessTemplate"
#define TF_SERVICE_REGISTRATION_TYPE    "RegistrationService"
#define TF_SERVICE_STATUS_TYPE          "StatusService"
#define TF_SERVICE_VERSION_CONTR_TYPE   "ISCCProvider"
#define TF_SERVICE_VERSION_CONTR3_TYPE  "ISCCProvider3"

#define TF_SERVICE_AUTHORIZATION_NAME   "Authorization Service"
#define TF_SERVICE_AUTHORIZATION3_NAME  "Authorization Service 3"
#define TF_SERVICE_CATALOG_NAME         "Catalog Service"
#define TF_SERVICE_COMMON_STRUCT_NAME   "Common Structure Service"
#define TF_SERVICE_COMMON_STRUCT3_NAME  "Common Structure Service 3"
#define TF_SERVICE_LOCATION_NAME        "Location Service"
#define TF_SERVICE_PROCESS_TEMPL_NAME   "Process Template Service"
#define TF_SERVICE_REGISTRATION_NAME    "Registration Service"
#define TF_SERVICE_STATUS_NAME          "Status Service"
#define TF_SERVICE_VERSION_CONTR_NAME   "Version Control Service"
#define TF_SERVICE_VERSION_CONTR3_NAME  "Version Control Service 3"

#define TF_SERVICE_RELTO_CONTEXT            0
#define TF_SERVICE_RELTO_WEB_APPLICATION    1
#define TF_SERVICE_RELTO_FULLY_QUALIFIED    2

#define TF_ACCESSMAP_PUBLIC_MONIKER     "public"
#define TF_ACCESSMAP_PUBLIC_DISPLNAME   "Team Foundation front-end"

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

tf_error tf_set_default_access_map(pgctx *, tf_access_map *);

