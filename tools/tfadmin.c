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
 * @brief   Team Foundation administrative tools wrapper
 *
 * @author  Bob Carroll (bob.carroll@alum.rit.edu)
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

int main(int argc, char **argv)
{
    char newcmd[1024];
    char **newargs = NULL;

    bzero(&newcmd, 1024);

    if (argc == 1)
        snprintf(newcmd, 1024, "tfadmin-help");
    else
        snprintf(newcmd, 1024, "tfadmin-%s", argv[1]);

    newargs = (char **)calloc(argc, sizeof(char *));
    newargs[0] = newcmd;

    int i;
    for (i = 2; i < argc; i++)
        newargs[i - 1] = argv[i];

    execv(newcmd, newargs);

    if (errno == ENOENT)
        fprintf(stderr, "tfadmin: '%s' is not a tfadmin command. See 'tfs help'.\n", argv[1]);
    else
        fprintf(stderr, "tfadmin: Could not execute '%s' command! (Sorry)\n", newcmd);

    return 0;
}

