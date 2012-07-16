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
 * @brief   tfadmin CLI usage information
 *
 * @author  Bob Carroll (bob.carroll@alum.rit.edu)
 */

#include <stdio.h>

int main(int argc, char **argv)
{
    if (argc == 1) {
        printf("USAGE: tfadmin <command> [<args>]\n");
        printf("\n");
        printf("The mostly commonly used TF commands are:\n");
        printf("   help             Display command usage details (this screen)\n");
        printf("   setup            Initialise this Team Foundation server instance\n");
        printf("   tpc-attach       Attach a team project collection to this instance\n");
        printf("   tpc-create       Create a new team project collection\n");
        printf("\n");
        printf("See 'tfadmin help <command>' for more information on a specific command.\n");
    } else
        printf("Sorry, not implemented yet!\n"); /* TODO */

    return 0;
}

