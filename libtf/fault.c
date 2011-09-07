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
 * @brief   SOAP fault helper functions
 *
 * @author  Bob Carroll (bob.carroll@alum.rit.edu)
 */

#include <libxml/tree.h>
#include <libxml/xpath.h>

#include <gcs/log.h>

#include <tf/fault.h>
#include <tf/xml.h>

/**
 * Generates a fault message in a SOAP envelope.
 *
 * @param code  the fault code
 * @param msg   the fault string
 * @param tferr the TF error code
 * @param out   the SOAP envelope output buffer
 *
 * @return the fault detail node
 */
xmlNode *tf_fault_env(int code, const char *msg, int tferr, SoapEnv **out)
{
    xmlNode *detailnode = NULL;
    char tferrstr[10];

    gcslog_info("SOAP Fault %d: %s", tferr, msg);

    if (*out)
        soap_env_free(*out);

    soap_env_new_with_fault(code, msg, "", "", out);

    if ((detailnode = tf_xml_find_first((*out)->body, NULL, NULL, "//detail"))) {
        snprintf(tferrstr, 10, "%d", tferr);
        xmlNewChild(detailnode, NULL, "tferror", tferrstr);
    }

    return detailnode;
}

