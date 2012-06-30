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
 * @brief   tests querying resource types
 *
 * @author  Bob Carroll (bob.carroll@alum.rit.edu)
 */

#include <stdio.h>
#include <string.h>

#include <log.h>

#include <tf/catalog.h>

int main(int argc, char **argv)
{
    if (!log_open(NULL, LOG_TRACE, 1, 0)) {
        fprintf(stderr, "%s: failed to open log file!\n", argv[0]);
        return 1;
    }

    tf_resource_type **typelst = NULL;
    tf_error dberr = tf_query_resource_types(&typelst);

    if (dberr != TF_ERROR_SUCCESS)
        return 1;

    int i;
    for (i = 0; typelst[i]; i++)
        ;

    if (i != _tf_rsrc_tbl_len) {
        log_error("expected count (%d) did not match actual count (%d)", _tf_rsrc_tbl_len, i);
        return 1;
    }

    log_info("count: %d", i);

    for (i = 0; typelst[i]; i++) {
        log_info("item %d ID: %s", i, typelst[i]->id);
        log_info("item %d name: %s", i, typelst[i]->name);
        log_info("item %d description: %s", i, typelst[i]->description);
    }

    typelst = tf_free_resource_type_array(typelst);

    return 0;
}

