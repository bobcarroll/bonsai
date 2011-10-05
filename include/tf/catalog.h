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

#define TF_CATALOG_QUERY_EXPAND_DEPS    1
#define TF_CATALOG_QUERY_INC_PARENTS    2

#define TF_CATALOG_INFRASTRUCTURE_ROOT  "Vc1S6XwnTEe/isOiPfhmxw=="
#define TF_CATALOG_ORGANIZATION_ROOT    "3eYRYkJOok6GHrKam0AcAA=="

#define TF_CATALOG_TYPE_ANALYSIS_SRVCS_DB   "64C0C64F-7199-4C0A-A1F7-6D979292E86E"
#define TF_CATALOG_TYPE_CONFIG_DB           "526301DE-F821-48C8-ABBD-3430DC7946D3"
#define TF_CATALOG_TYPE_DIAG_DATA_ADAPT     "B1A784AD-4C46-4574-B18D-2AA07AA2BDDB"
#define TF_CATALOG_TYPE_GENERIC_LINK        "53D857F7-0197-45FA-BB58-EDF76AD7BFB2"
#define TF_CATALOG_TYPE_INFRASTRUCTURE_ROOT "14F04669-6779-42D5-8975-184B93650C83"
#define TF_CATALOG_TYPE_MACHINE             "0584A4A2-475B-460E-A7AC-10C28951518F"
#define TF_CATALOG_TYPE_ORGANIZATIONAL_ROOT "69A51C5E-C093-447E-A177-A09E47A60974"
#define TF_CATALOG_TYPE_PRJ_COLLECTION_DB   "1B6B5931-69F6-4C53-90A0-220B177353B7"
#define TF_CATALOG_TYPE_PRJ_PORTAL          "450901B6-B528-4863-9876-5BD3927DF467"
#define TF_CATALOG_TYPE_PRJ_PORTAL_LOCATION "9FB288AE-9D94-40CB-B5E7-0EFC3FE3599F"
#define TF_CATALOG_TYPE_PROCESS_GUIDANCE    "15DA1594-45F5-47D4-AE52-78F16E67EB1E"
#define TF_CATALOG_TYPE_REL_WAREHOUSE_DB    "CE318CD9-F797-45DC-ACC7-792C3428E39D"
#define TF_CATALOG_TYPE_REPORT_FOLDER       "41C8B6DB-39EC-49DB-9DB8-0760E836BFBE"
#define TF_CATALOG_TYPE_REPORTING_CONFIG    "143B22C5-D1B9-494F-B124-68D098ABA598"
#define TF_CATALOG_TYPE_REPORTING_INSTANCE  "065977D6-00EA-4A77-81EC-1CD011644AAC"
#define TF_CATALOG_TYPE_RESOURCE_FOLDER     "55F97194-EC42-4DFC-B596-7DECC43CDE1E"
#define TF_CATALOG_TYPE_SERVER_INSTANCE     "B36F1BDA-DF2D-482B-993A-F194A31A1FA2"
#define TF_CATALOG_TYPE_SERVER_WEB_APP      "FFAF34BB-ADED-4507-9E52-FCA85E91BA63"
#define TF_CATALOG_TYPE_SHAREPOINT_WEB_APP  "3DADD190-40E6-4FC1-A306-4906713C87CE"
#define TF_CATALOG_TYPE_SQL_ANALYSIS_INST   "D22D57DA-355D-4A3C-82DE-62B3E157D0B3"
#define TF_CATALOG_TYPE_SQL_DB_INSTANCE     "EB1E0B3B-FAA1-49D2-931A-FDC373682BA5"
#define TF_CATALOG_TYPE_SQL_REPORTING_INST  "F756975E-3593-448B-A6B8-E34010908621"
#define TF_CATALOG_TYPE_TEST_CONTROLLER     "3C856555-8737-48B6-8B61-4B24DB7FEB15"
#define TF_CATALOG_TYPE_TEST_ENVIRONMENT    "D457AA94-F00E-4342-92E8-FFE81535E74B"
#define TF_CATALOG_TYPE_TEAM_PRJ_COLLECITON "26338D9E-D437-44AA-91F2-55880A328B54"
#define TF_CATALOG_TYPE_TEAM_PROJECT        "48577A4A-801E-412C-B8AE-CF7EF3529616"
#define TF_CATALOG_TYPE_TEAM_WEB_ACCESS     "47FA57A4-8157-4FB5-9A64-A7A4954BD284"

void *tf_free_node_array(tf_node **);
void *tf_free_path_spec_array(tf_path_spec **);
void tf_free_property(tf_property *);
void *tf_free_property_array(tf_property **);
void tf_free_resource(tf_resource);
void *tf_free_service_ref_array(tf_service_ref **);
tf_error tf_query_nodes(const char * const *, const char * const *, tf_node ***);
tf_error tf_query_single_node(const char *, const char *, tf_node ***);

