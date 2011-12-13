/******************************************************************
*  $Id: soap-env.c,v 1.20 2006/07/09 16:24:19 snowdrop Exp $
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

#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

#ifdef WIN32
#define USE_XMLSTRING
#endif

#ifdef USE_XMLSTRING
#include <libxml/xmlstring.h>
#endif

#include <libcsoap/soap-xml.h>
#include <libcsoap/soap-env.h>

#include <log.h>

/*
Parameters:
1- soap_env_ns
2- soap_env_enc
3- xsi_ns
4- xsd_ns
3- method name
4- uri
5- method name(again)
*/
#define _SOAP_MSG_TEMPLATE_ \
	"<soap:Envelope xmlns:soap=\"%s\" soap:encodingStyle=\"%s\"" \
	" xmlns:xsi=\"%s\"" \
	" xmlns:xsd=\"%s\">" \
	" <soap:Body>"\
	"  <%s xmlns=\"%s\">"\
	"  </%s>" \
	" </soap:Body>"\
	"</soap:Envelope>"


/*
Parameters:
1- soap_env_ns
2- soap_env_enc
3- xsi_ns
4- xsd_ns
3- method name
4- uri
5- method name(again)
*/
#define _SOAP_MSG_TEMPLATE_EMPTY_TARGET_ \
	"<soap:Envelope xmlns:soap=\"%s\" soap:encodingStyle=\"%s\"" \
	" xmlns:xsi=\"%s\"" \
	" xmlns:xsd=\"%s\">" \
	" <soap:Body>"\
	"  <%s xmlns=\"%s\">"\
	"  </%s>" \
	" </soap:Body>"\
	"</soap:Envelope>"


/* ---------------------------------------------------------------------------- */
/*     XML Serializers (implemented at the and of this document)              */
/* ---------------------------------------------------------------------------- */
struct XmlNodeHolder
{
  xmlNodePtr node;
};

static void
xmlbuilder_start_element(const xmlChar * element_name, int attr_count, xmlChar ** keys, xmlChar ** values, void *userData)
{
  struct XmlNodeHolder *holder = (struct XmlNodeHolder *) userData;
  xmlNodePtr parent = NULL;

  if (holder == NULL)
    return;
  parent = holder->node;
  if (parent == NULL)
    return;

  holder->node = xmlNewChild(parent, NULL, element_name, NULL);

  return;
}

static void
xmlbuilder_characters(const xmlChar * element_name, const xmlChar * chars, void *userData)
{
  struct XmlNodeHolder *holder = (struct XmlNodeHolder *) userData;
  xmlNodePtr parent = NULL;

  if (holder == NULL)
    return;
  parent = holder->node;
  if (parent == NULL)
    return;

  xmlNewTextChild(parent, NULL, element_name, chars);

  return;
}

static void
xmlbuilder_end_element(const xmlChar * element_name, void *userData)
{

  struct XmlNodeHolder *holder = (struct XmlNodeHolder *) userData;
  xmlNodePtr parent = NULL;

  if (holder == NULL)
    return;
  parent = holder->node;
  if (parent == NULL)
    return;

  holder->node = parent->parent;

  return;
}

herror_t
soap_env_new_from_doc(xmlDocPtr doc, SoapEnv ** out)
{
  xmlNodePtr node;
  SoapEnv *env;

  if (doc == NULL)
  {
    gcslog_error("Can not create xml document!");
    return herror_new("soap_env_new_from_doc",
                      GENERAL_INVALID_PARAM,
                      "XML Document (xmlDocPtr) is NULL");
  }

  if (!(node = xmlDocGetRootElement(doc)))
  {
    gcslog_error("XML document is empty!");
    return herror_new("soap_env_new_from_doc",
                      XML_ERROR_EMPTY_DOCUMENT, "XML Document is empty!");
  }

  if (!(env = (SoapEnv *) malloc(sizeof(SoapEnv))))
  {
    gcslog_error("malloc failed (%s)", strerror(errno));
    return herror_new("soap_env_from_doc", GENERAL_INVALID_PARAM, "malloc failed");
  }

  env->root = node;
  env->header = soap_env_get_header(env);
  env->body = soap_env_get_body(env);
  env->cur = soap_env_get_method(env);

  *out = env;

  return H_OK;
}


herror_t
soap_env_new_from_buffer(const char *buffer, SoapEnv ** out)
{
  xmlDocPtr doc;
  herror_t err;

  if (buffer == NULL)
    return herror_new("soap_env_new_from_buffer",
                      GENERAL_INVALID_PARAM, "buffer (first param) is NULL");

  if (!(doc = xmlParseDoc(BAD_CAST buffer)))
    return herror_new("soap_env_new_from_buffer",
                      XML_ERROR_PARSE, "Can not parse xml");

  if ((err = soap_env_new_from_doc(doc, out)) != H_OK)
  {
    xmlFreeDoc(doc);
  }
  return err;
}


herror_t
soap_env_new_with_fault(fault_code_t faultcode,
                        const char *faultstring,
                        const char *faultactor, const char *detail,
                        SoapEnv ** out)
{
  xmlDocPtr doc;
  herror_t err;

  doc = soap_fault_build(faultcode, faultstring, faultactor, detail);
  if (doc == NULL)
    return herror_new("soap_env_new_with_fault",
                      XML_ERROR_PARSE, "Can not parse fault xml");

  if ((err = soap_env_new_from_doc(doc, out)) != H_OK)
  {
    xmlFreeDoc(doc);
  }

  return err;
}


herror_t
soap_env_new_with_response(SoapEnv * request, SoapEnv ** out)
{
  char *method, *res_method;
  herror_t ret;
  char *urn;

  if (request == NULL)
  {
    return herror_new("soap_env_new_with_response",
                      GENERAL_INVALID_PARAM, "request (first param) is NULL");
  }

  if (request->root == NULL)
  {
    return herror_new("soap_env_new_with_response",
                      GENERAL_INVALID_PARAM,
                      "request (first param) has no xml structure");
  }

  if (!(method = soap_env_find_methodname(request)))
  {
    return herror_new("soap_env_new_with_response",
                      GENERAL_INVALID_PARAM,
                      "Method name '%s' not found in request",
                      SAVE_STR(method));
  }

  if (!(urn = soap_env_find_urn(request)))
  {

    /* here we have no chance to find out the namespace */
    /* try to continue without namespace (urn) */
    urn = "";
  }

  if (!(res_method = (char *)malloc(strlen(method)+9)))
    return herror_new("soap_env_new_with_response", GENERAL_INVALID_PARAM, "malloc failed");

  sprintf(res_method, "%sResponse", method);

  ret = soap_env_new_with_method(urn, res_method, out);

  free(res_method);

  return ret;
}


herror_t
soap_env_new_with_method(const char *urn, const char *method, SoapEnv ** out)
{
  xmlDocPtr env;
  xmlChar buffer[1054];

  gcslog_debug("URN = '%s'", urn);
  gcslog_debug("Method = '%s'", method);

  if (!strcmp(urn, ""))
  {
#ifdef USE_XMLSTRING
    xmlStrPrintf(buffer, 1054, BAD_CAST _SOAP_MSG_TEMPLATE_EMPTY_TARGET_,
                 soap_env_ns2, soap_env_enc, soap_xsi_ns,
                 soap_xsd_ns, BAD_CAST method, BAD_CAST urn, BAD_CAST method);
#else
    sprintf(buffer, _SOAP_MSG_TEMPLATE_EMPTY_TARGET_,
            soap_env_ns2, soap_env_enc, soap_xsi_ns,
            soap_xsd_ns, method, urn, method);
#endif
  }
  else
  {
#ifdef USE_XMLSTRING
    xmlStrPrintf(buffer, 1054, BAD_CAST _SOAP_MSG_TEMPLATE_,
                 soap_env_ns2, soap_env_enc, soap_xsi_ns,
                 soap_xsd_ns, BAD_CAST method, BAD_CAST urn, BAD_CAST method);
#else
    sprintf(buffer, _SOAP_MSG_TEMPLATE_,
            soap_env_ns2, soap_env_enc, soap_xsi_ns,
            soap_xsd_ns, method, urn, method);
#endif

  }

  if (!(env = xmlParseDoc(buffer)))
    return herror_new("soap_env_new_with_method",
                      XML_ERROR_PARSE, "Can not parse xml");

  return soap_env_new_from_doc(env, out);
}


static int
_soap_env_xml_io_read(void *ctx, char *buffer, int len)
{
  int readed;

  http_input_stream_t *in = (http_input_stream_t *) ctx;
  if (!http_input_stream_is_ready(in))
    return 0;

  readed = http_input_stream_read(in, buffer, len);
  if (readed == -1)
    return 0;
  return readed;
}

static int
_soap_env_xml_io_close(void *ctx)
{
  /* do nothing */
  return 0;
}


herror_t
soap_env_new_from_stream(http_input_stream_t * in, SoapEnv ** out)
{
  xmlDocPtr doc;

  doc = xmlReadIO(_soap_env_xml_io_read,
                  _soap_env_xml_io_close, in, "", NULL, 0);

  if (in->err != H_OK)
    return in->err;

  if (doc == NULL)
    return herror_new("soap_env_new_from_stream",
                      XML_ERROR_PARSE, "Trying to parse not valid xml");

  return soap_env_new_from_doc(doc, out);
}


xmlNodePtr
soap_env_add_item(SoapEnv * call, const char *type, const char *name, const char *value)
{
  xmlNodePtr newnode;

  newnode = xmlNewTextChild(call->cur, NULL, BAD_CAST name, BAD_CAST value);

  if (newnode == NULL)
  {
    gcslog_error("Can not create new xml node");
    return NULL;
  }

  if (type)
  {
    if (!xmlNewProp(newnode, BAD_CAST "xsi:type", BAD_CAST type))
    {
      gcslog_error("Can not create new xml attribute");
      return NULL;
    }
  }

  return newnode;
}


xmlNodePtr
soap_env_add_itemf(SoapEnv * call, const char *type,
                   const char *name, const char *format, ...)
{

  va_list ap;
  char buffer[1054];


  va_start(ap, format);
  vsprintf(buffer, format, ap);
  va_end(ap);

  return soap_env_add_item(call, type, name, buffer);
}


xmlNodePtr
soap_env_add_attachment(SoapEnv * call, const char *name, const char *href)
{
  xmlNodePtr newnode;

  newnode = xmlNewTextChild(call->cur, NULL, BAD_CAST name, BAD_CAST "");

  if (newnode == NULL)
  {
    gcslog_error("Can not create new xml node");
    return NULL;
  }

  if (href)
  {
    if (!xmlNewProp(newnode, BAD_CAST "href", BAD_CAST href))
    {
      gcslog_error("Can not create new xml attribute");
      return NULL;
    }
  }

  return newnode;
}


void
soap_env_add_custom(SoapEnv * call, void *obj, XmlSerializerCallback cb, const char *type, const char *name)
{
  struct XmlNodeHolder holder;

  holder.node = soap_env_get_method(call);

  cb(obj, BAD_CAST name,
     xmlbuilder_start_element,
     xmlbuilder_characters, xmlbuilder_end_element, &holder);

  return;
}


xmlNodePtr
soap_env_push_item(SoapEnv * call, const char *type, const char *name)
{

  xmlNodePtr node;

  if ((node = soap_env_add_item(call, type, name, "")))
    call->cur = node;

  return node;
}


void
soap_env_pop_item(SoapEnv * call)
{
  call->cur = call->cur->parent;

  return;
}


void
soap_env_free(SoapEnv * env)
{
  if (env)
  {
    if (env->root)
    {
      xmlFreeDoc(env->root->doc);
    }
    free(env);
  }

  return;
}


xmlNodePtr
soap_env_get_body(SoapEnv * env)
{
  xmlNodePtr node;

  if (env == NULL)
  {
    gcslog_error("env object is NULL");
    return NULL;
  }

  if (env->root == NULL)
  {
    gcslog_error("env has no xml");
    return NULL;
  }

  for (node = soap_xml_get_children(env->root); node; node = soap_xml_get_next(node))
  {
    if (!xmlStrcmp(node->name, BAD_CAST "Body")
     && !xmlStrcmp(node->ns->href, BAD_CAST soap_env_ns2))
      return node;

    /* fall back to SOAP 1.1 namespace (thanks MS) */
    if (!xmlStrcmp(node->name, BAD_CAST "Body")
     && !xmlStrcmp(node->ns->href, BAD_CAST soap_env_ns1))
      return node;
  }

  gcslog_error("Body tag not found!");
  return NULL;
}


xmlNodePtr
soap_env_get_header(SoapEnv *env)
{
  xmlNodePtr node;

  if (!env)
  {
    gcslog_error("SoapEnv is NULL");
    return NULL;
  }

  if (!env->root)
  {
    gcslog_error("SoapEnv contains no document");
    return NULL;
  }

  for (node = soap_xml_get_children(env->root); node; node = soap_xml_get_next(node))
  {
    if (!xmlStrcmp(node->name, BAD_CAST "Header")
     && !xmlStrcmp(node->ns->href, BAD_CAST soap_env_ns2))
      return node;

    /* fall back to SOAP 1.1 namespace (thanks MS) */
    if (!xmlStrcmp(node->name, BAD_CAST "Header")
     && !xmlStrcmp(node->ns->href, BAD_CAST soap_env_ns1))
      return node;
  }

  return NULL;
}


xmlNodePtr
soap_env_get_fault(SoapEnv * env)
{
  xmlNodePtr node;

  node = soap_env_get_body(env);

  if (!node)
    return NULL;

  while (node != NULL)
  {
    if (!xmlStrcmp(node->name, BAD_CAST "Fault"))
      return node;
    node = soap_xml_get_next(node);
  }

/*  gcslog_warn ("Node Fault tag found!");*/
  return NULL;
}


xmlNodePtr
soap_env_get_method(SoapEnv * env)
{
  xmlNodePtr body;

  if (!(body = soap_env_get_body(env)))
  {
    gcslog_debug("SoapEnv contains no Body");
    return NULL;
  }

  /* method is the first child */
  return soap_xml_get_children(body);
}


/* XXX: unused function? */
xmlNodePtr
_soap_env_get_body(SoapEnv * env)
{
  xmlNodePtr body;
  xmlNodeSetPtr nodeset;
  xmlXPathObjectPtr xpathobj;

  if (env == NULL)
  {
    gcslog_error("SoapEnv is NULL");
    return NULL;
  }

  if (env->root == NULL)
  {
    gcslog_error("SoapEnv contains no XML document");
    return NULL;
  }

  /* 
     find <Body> tag find out namespace xpath: //Envelope/Body/ */
  xpathobj = soap_xpath_eval(env->root->doc, "//Envelope/Body");

  if (!xpathobj)
  {
    gcslog_error("No Body (xpathobj)!");
    return NULL;
  }

  if (!(nodeset = xpathobj->nodesetval))
  {
    gcslog_error("No Body (nodeset)!");
    xmlXPathFreeObject(xpathobj);
    return NULL;
  }

  if (nodeset->nodeNr < 1)
  {
    gcslog_error("No Body (nodeNr)!");
    xmlXPathFreeObject(xpathobj);
    return NULL;
  }

  body = nodeset->nodeTab[0];   /* body is <Body> */
  xmlXPathFreeObject(xpathobj);
  return body;

}


char *
soap_env_find_urn(SoapEnv * env)
{
  xmlNsPtr ns;
  xmlNodePtr body, node;

  if (!(body = soap_env_get_body(env)))
  {
    gcslog_debug("body is NULL");
    return 0;
  }

  /* node is the first child */
  if (!(node = soap_xml_get_children(body)))
  {
    gcslog_error("No namespace found");
    return 0;
  }

  /* if (node->ns && node->ns->prefix) MRC 1/25/2006 */
  if (node->ns)
  {
    ns = xmlSearchNs(body->doc, node, node->ns->prefix);
    if (ns != NULL)
    {
      return((char *) ns->href); /* namespace found! */
    }
  }
  else
  {
    static char *empty = "";
    gcslog_warn("No namespace found");
    return(empty);
  }

  return 0;
}


char *
soap_env_find_methodname(SoapEnv * env)
{
  xmlNodePtr body, node;

  body = soap_env_get_body(env);
  if (body == NULL)
    return 0;

  node = soap_xml_get_children(body);   /* node is the first child */

  if (node == NULL)
  {
    gcslog_error("No method found");
    return 0;
  }

  if (node->name == NULL)
  {
    gcslog_error("No methodname found");
    return 0;

  }

  return((char *) node->name);
}


