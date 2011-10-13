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

#include <gcs/pgctxpool.h>

#include <tf/locationtypes.h>
#include <tf/errors.h>

#define TF_LOCATION_FILTER_SERVICE_ID       "567713db-d56d-4bb0-8f35-604e0e116174"
#define TF_LOCATION_FILTER_SERVICE_TYPE     "*"

tf_error tf_fetch_access_map(pgctx *, tf_access_map ***);
tf_error tf_fetch_services(pgctx *, tf_service_filter **, tf_service ***);

