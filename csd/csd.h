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

#include <libcsoap/soap-router.h>

#include <tf/catalog.h>
#include <tf/xml.h>

void catalog_service_init(SoapRouter **, const char *, tf_service_ref *, const char *);

void location_append_service(xmlNode *, tf_service *);
void location_service_init(SoapRouter **, const char *, tf_service_ref *, const char *);

void pc_services_init(const char *, const char *, const char *, const char *, int);

void registration_service_init(SoapRouter **, const char *, tf_service_ref *, const char *);

void status_service_init(SoapRouter **, const char *, tf_service_ref *, const char *);

char *core_services_init(const char *);

