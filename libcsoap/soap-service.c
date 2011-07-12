/******************************************************************
*  $Id: soap-service.c,v 1.8 2006/07/09 16:24:19 snowdrop Exp $
*
* CSOAP Project:  A SOAP client/server library in C
* Copyright (C) 2003  Ferhat Ayaz
*
* This library is free software; you can redistribute it and/or
* modify it under the terms of the GNU Library General Public
* License as published by the Free Software Foundation; either
* version 2 of the License, or (at your option) any later version.
*
* This library is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
* Library General Public License for more details.
*
* You should have received a copy of the GNU Library General Public
* License along with this library; if not, write to the
* Free Software Foundation, Inc., 59 Temple Place - Suite 330,
* Boston, MA  02111-1307, USA.
* 
* Email: ayaz@jprogrammer.net
******************************************************************/

#include <string.h>

#include <nanohttp/nanohttp-common.h>

#include <libcsoap/soap-service.h>

#include <gcs/log.h>

SoapServiceNode *
soap_service_node_new(SoapService * service, SoapServiceNode * next)
{
  SoapServiceNode *node;

  node = (SoapServiceNode *) malloc(sizeof(SoapServiceNode));
  node->service = service;
  node->next = next;

  return node;
}




SoapService *
soap_service_new(const char *urn, const char *method, SoapServiceFunc f)
{
  SoapService *service;


  service = (SoapService *) malloc(sizeof(SoapService));
  service->func = f;

  if (urn != NULL)
  {
    service->urn = (char *) malloc(strlen(urn) + 1);
    strcpy(service->urn, urn);
  }
  else
  {
    gcslog_warn("urn is NULL");
    service->urn = "";
  }

  if (method != NULL)
  {
    service->method = (char *) malloc(strlen(method) + 1);
    strcpy(service->method, method);
  }
  else
  {
    gcslog_warn("method is NULL");
    service->method = "";
  }

  return service;
}



void
soap_service_free(SoapService * service)
{
  gcslog_debug("enter: service=%p", service);

  if (service == NULL)
    return;

  if (strcmp(service->urn, ""))
    free(service->urn);

  if (strcmp(service->method, ""))
    free(service->method);

  free(service);
  gcslog_debug("leave with success");
}
