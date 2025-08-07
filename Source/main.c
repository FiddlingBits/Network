/****************************************************************************************************
 * Include
 ****************************************************************************************************/

#include "list.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/****************************************************************************************************
 * Function Definition (Public)
 ****************************************************************************************************/

/*** Main ***/
int main(int argc, char *argv[])
{
    /*** Main ***/
    /* Handle Arguments */
    if((argc == 2) && (strcmp(argv[1], "list") == 0))
    {
        /* List Network Interface Information */
        list();
    }
    else
    {
        /* Usage */
        (void)printf("usage: %s [OPTION]\n", argv[0]);
        (void)printf("  list, List Network Interface Information\n");
    }
    
    /* Exit */
    exit(EXIT_SUCCESS);
}
