/* Copyright 1991 Digital Equipment Corporation.
 * All Rights Reserved.
*****************************************************************/
/* 	$Id: templates.c,v 1.2 1994/12/08 23:33:53 duchier Exp $	 */
#define REV401PLUS
#ifndef lint
static char vcid[] = "$Id: templates.c,v 1.2 1994/12/08 23:33:53 duchier Exp $";
#endif /* lint */
#define EXTERN extern
#ifdef REV401PLUS
#include "defs.h"
#endif

/******** GET_ARG

  assign the argument "number" of the goal "g" in "arg".
  return FALSE if bad argument.
*/

long get_arg (ptr_psi_term g, ptr_psi_term *arg, char *number)
// ptr_psi_term g, *arg;
// char *number;
{
    ptr_node n;


    if (n = find (FEATCMP, number, g->attr_list))
        return (*arg = (ptr_psi_term) n->data) ? TRUE: FALSE;
    else
        return FALSE;
}



