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

#include <tf/location.h>
#include <tf/errors.h>

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

#define TF_CATALOG_PROPERTY_INSTANCE_ID_ID      10
#define TF_CATALOG_PROPERTY_INSTANCE_ID_VALUE   "InstanceId"

#define TF_CATALOG_TOOL_FRAMEWORK       "Framework"

#define TF_CATALOG_ASSOCIATION_KEY_MAXLEN       257
#define TF_CATALOG_CHILD_ITEM_MAXLEN            25
#define TF_CATALOG_PARENT_PATH_MAXLEN           865
#define TF_CATALOG_PROPERTY_NAME_MAXLEN         401
#define TF_CATALOG_RESOURCE_ID_MAXLEN           37
#define TF_CATALOG_RESOURCE_NAME_MAXLEN         257
#define TF_CATALOG_RESOURCE_TYPE_MAXLEN         37
#define TF_CATALOG_RESOURCE_TYPE_NAME_MAXLEN    257

#define TF_CATALOG_NODE_DEPTH_NONE      0
#define TF_CATALOG_NODE_DEPTH_SINGLE    1
#define TF_CATALOG_NODE_DEPTH_FULL      2

#define TF_CATALOG_QUERY_EXPAND_DEPS    1
#define TF_CATALOG_QUERY_INC_PARENTS    2

static const int _tf_rsrc_tbl_len = 26;
static const int _tf_prop_tbl_len = 1;
static const int _tf_tool_tbl_len = 1;

static const char *_tf_rsrc_type_id[] = {
    TF_CATALOG_TYPE_ANALYSIS_SRVCS_DB,
    TF_CATALOG_TYPE_CONFIG_DB,
    TF_CATALOG_TYPE_DIAG_DATA_ADAPT,
    TF_CATALOG_TYPE_GENERIC_LINK,
    TF_CATALOG_TYPE_INFRASTRUCTURE_ROOT,
    TF_CATALOG_TYPE_MACHINE,
    TF_CATALOG_TYPE_ORGANIZATIONAL_ROOT,
    TF_CATALOG_TYPE_PRJ_COLLECTION_DB,
    TF_CATALOG_TYPE_PRJ_PORTAL,
    TF_CATALOG_TYPE_PRJ_PORTAL_LOCATION,
    TF_CATALOG_TYPE_PROCESS_GUIDANCE,
    TF_CATALOG_TYPE_REL_WAREHOUSE_DB,
    TF_CATALOG_TYPE_REPORT_FOLDER,
    TF_CATALOG_TYPE_REPORTING_CONFIG,
    TF_CATALOG_TYPE_REPORTING_INSTANCE,
    TF_CATALOG_TYPE_RESOURCE_FOLDER,
    TF_CATALOG_TYPE_SERVER_INSTANCE,
    TF_CATALOG_TYPE_SERVER_WEB_APP,
    TF_CATALOG_TYPE_SHAREPOINT_WEB_APP,
    TF_CATALOG_TYPE_SQL_ANALYSIS_INST,
    TF_CATALOG_TYPE_SQL_DB_INSTANCE,
    TF_CATALOG_TYPE_SQL_REPORTING_INST,
    TF_CATALOG_TYPE_TEST_CONTROLLER,
    TF_CATALOG_TYPE_TEST_ENVIRONMENT,
    TF_CATALOG_TYPE_TEAM_PRJ_COLLECITON,
    TF_CATALOG_TYPE_TEAM_PROJECT,
    TF_CATALOG_TYPE_TEAM_WEB_ACCESS
};

static const char *_tf_rsrc_type_name[] = {
    "Analysis Services Database",
    "Team Foundation Configuration Database",
    "Diagnostic Data Adapter",
    "Generic Link",
    "Infrastructure Root",
    "Machine",
    "Organizational Root",
    "Team Foundation Project Collection Database",
    "Project Portal",
    "Default location for team project portals",
    "Process Guidance",
    "Relational Warehouse Database",
    "Report Folder",
    "Reporting Configuration",
    "Reporting Instance",
    "Resource Folder",
    "Team Foundation Server Instance",
    "Team Foundation Server Web Application",
    "SharePoint Web Application",
    "SQL Analysis Instance",
    "SQL Database Instance",
    "SQL Server Reporting Services",
    "Test Controller",
    "Test Environment",
    "Team Project Collection",
    "Team Project",
    "Team Web Access"
};

static const char *_tf_rsrc_type_desc[] = {
    "The cube for Visual Studio Team System.",
    "The database that houses the information for a Team Foundation Application Instance.",
    "Contains properties for a diagnostic data adapter used in test environments.",
    "A link to a generic resource used in the TFS deployment.",
    "The root of the catalog tree that describes the physical makeup of the TFS deployment.",
    "A machine that exists as part of the TFS deployment.",
    "The root of the catalog tree that describes the organizational makeup of the TFS deployment.",
    "The database that houses the information for a Team Foundation Project Collection.",
    "A Web site or SharePoint site that provides a portal for a team project.",
    "By default, each team project Web site will be a sub-site of this location.",
    "A Web site or SharePoint library that provides process guidance for a team project.",
    "A relational database used as a data warehouse for reporting.",
    "The name of the folder on the server that is running SQL Server Reporting Services. This folder is where reports are stored.",
    "Configuration information for reports, such as the data warehouse, the Visual Studio Team System cube, and information about the location of the server that is running SQL Server Reporting Services.",
    "The instance that is running SQL Server Reporting Services and is configured for use with Team Foundation Server.",
    "A folder that is used to group catalog resources in a logical way.",
    "A deployed instance of Team Foundation Server.",
    "The web application that hosts a Team Foundation Server",
    "A SharePoint Web application that has been configured to support team project portals.",
    "SQL Analysis Instance Description",
    "An instance of a SQL database that exists in the TFS deployment.",
    "The server that is running SQL Server Reporting Services and is configured for use with Team Foundation Server.",
    "This is a test controller type in Catalog Service.",
    "This is a test environment type in Catalog Service.",
    "A Team Project Collection that exists within the TFS deployment.",
    "A Team Project that exists within the TFS deployment.",
    "Team Web Access Location"
};

static const int _tf_property_id[] = {
    TF_CATALOG_PROPERTY_INSTANCE_ID_ID
};

static const char *_tf_property_value[] = {
    TF_CATALOG_PROPERTY_INSTANCE_ID_VALUE
};

static const char *_tf_tool_types[] = {
    TF_CATALOG_TOOL_FRAMEWORK
};

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

void *tf_free_node(tf_node *);
void *tf_free_node_array(tf_node **);
void *tf_free_property(tf_property *);
void *tf_free_property_array(tf_property **);
void *tf_free_service_ref(tf_service_ref *);
void *tf_free_service_ref_array(tf_service_ref **);

tf_error tf_query_nodes(pgctx *, const char * const *, const char * const *, tf_node ***);
tf_error tf_query_single_node(pgctx *, const char *, const char *, tf_node ***);

tf_node *tf_new_node(tf_node *, const char *, const char *, const char *);
tf_service_ref *tf_new_service_ref(tf_resource *, tf_service *, const char *);

tf_error tf_fetch_nodes(pgctx *, tf_path_spec **, const char * const *, tf_node ***);
tf_error tf_fetch_resources(pgctx *, const char * const *, int, tf_node ***);
tf_error tf_fetch_pc_service_refs(pgctx *, const char *, tf_service_ref ***);
tf_error tf_fetch_service_refs(pgctx *, tf_node **, tf_service_ref ***);
tf_error tf_fetch_properties(pgctx *, tf_node **, tf_property ***);

tf_error tf_add_node(pgctx *, tf_node *);
tf_error tf_add_service_ref(pgctx *, tf_service_ref *);

