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

/**
 * @brief   XML utility functions
 *
 * @author  Bob Carroll (bob.carroll@alum.rit.edu)
 */

#include <tf/xml.h>

/**
 * Finds all XML nodes with the given XPath.
 *
 * @param parent    the top-most node where the search begins
 * @param nsname    the namespace name
 * @param nshref    the namespace URI
 * @param expr      the XPath expression to use
 *
 * @return an XML nodeset or NULL
 */
xmlXPathObject *tf_xml_find_all(xmlNode *parent, xmlChar *nsname, xmlChar *nshref, xmlChar *expr)
{
    xmlXPathContextPtr xpctx;
    xmlXPathObjectPtr xpres;

    if (!parent || !parent->doc || !expr)
        return NULL;

    xpctx = xmlXPathNewContext(parent->doc);
    if (nsname)
        xmlXPathRegisterNs(xpctx, nsname, nshref);

    xpres = xmlXPathEvalExpression(expr, xpctx);
    xmlXPathFreeContext(xpctx);

    return xpres;
}

/**
 * Finds the first XML node with the given XPath.
 *
 * @param parent    the top-most node where the search begins
 * @param nsname    the namespace name
 * @param nshref    the namespace URI
 * @param expr      the XPath expression to use
 *
 * @return the first matching node found or NULL
 */
xmlNode *tf_xml_find_first(xmlNode *parent, xmlChar *nsname, xmlChar *nshref, xmlChar *expr)
{
    xmlXPathObject *xpres = tf_xml_find_all(parent, nsname, nshref, expr);
    xmlNode *result = NULL;

    if (!xpres)
        return NULL;

    if (!xmlXPathNodeSetIsEmpty(xpres->nodesetval))
        result = xpres->nodesetval->nodeTab[0];

    xmlXPathFreeObject(xpres);
    return result;
}

