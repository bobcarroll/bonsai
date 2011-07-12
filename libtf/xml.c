
#include <tf/xml.h>

xmlNode *tf_xml_find_first(xmlNode *parent, xmlChar *nsname, xmlChar *nshref, xmlChar *expr)
{
	xmlXPathContextPtr xpctx;
	xmlXPathObjectPtr xpres;
	xmlNode *result = NULL;

	if (parent == NULL || parent->doc == NULL || expr == NULL)
		return NULL;

	xpctx = xmlXPathNewContext(parent->doc);
	if (nsname != NULL)
		xmlXPathRegisterNs(xpctx, nsname, nshref);

	xpres = xmlXPathEvalExpression(expr, xpctx);
	xmlXPathFreeContext(xpctx);

	if (!xpres)
		return NULL;

	if (!xmlXPathNodeSetIsEmpty(xpres->nodesetval))
		result = xpres->nodesetval->nodeTab[0];

	xmlXPathFreeObject(xpres);
	return result;
}

