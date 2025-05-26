/* Copyright 1991 Digital Equipment Corporation.
** All Rights Reserved.
*****************************************************************/
/* 	$Id: info.c,v 1.4 1995/01/30 21:03:55 duchier Exp $	 */
#define REV401PLUS
#ifndef lint
static char vcid[] = "$Id: info.c,v 1.4 1995/01/30 21:03:55 duchier Exp $";
#endif /* lint */
#define EXTERN extern
#ifdef REV401PLUS
#include "defs.h"
#endif
#include <stdio.h>

/******** PNF(s,n)
  This prints one line of info cleanly. The string S is the name of the
  structure, N the number of bytes it uses
*/
static void pnf(char* s, long n)
// char *s;
// long n;
{
	long i;

	printf("        %s ", s);
	i = strlen(s);
	while (++i < 40) printf(".");
	printf(" %3ld bytes.\n", n);  // REV401 -> %3ld
}



/******** TITLE()
  This routine marks the start of Life. It prints info about the compile DATE,
  some blurb about the author etc... Who cares anyway??
  [I do! -hak]
*/
void title()
{
	//    char** it;
	long len;
	long sign;
	long val;
	char* it2 = (char*)"5234";
	char** it = &it2;

	if (quietflag)
		return; /*  RM: Feb 17 1993  */

	printf("Wild_Life Interpreter Version 5.10 Sun May 18 03:20:48 PM CDT 2025\n");
	printf("Copyright (C) 1991-93 DEC Paris Research Laboratory\n");
	printf("Extensions, Copyright (C) 1994-1995 Intelligent Software Group, SFU\n");
	printf("Ported to Linux and Cygwin December 2022 after many efforts since 1995\n");
	printf("By Dennis J. Darland dennis.darland@hotmail.com\n");

	val = is_int(it, &len, &sign);

	printf("it = %s len = %ld, sign = %ld val = %ld\n", *it, len, sign, val);

	val = is_int(&str_constants->num_tst, &len, &sign);

	printf("num_tat = %s len = %ld, sign = %ld val = %ld\n", str_constants->num_tst, len, sign, val);

	

#ifndef X11
	printf("X interface not installed.\n");
#endif
//	exit(0); // TEMP for DEBUGGING DJD
#if 0
	printf("\n- Main data-structure sizes:\n");
	pnf("rule", sizeof(struct pair_list));
	pnf("psi_term", sizeof(struct psi_term));
	pnf("binary tree node", sizeof(struct node));
	pnf("stacked goal", sizeof(struct goal));
	pnf("stacked choice-point", sizeof(struct choice_point));
	pnf("backtracking action", sizeof(struct ptr_stack));
	pnf("symbol definition", sizeof(struct definition));
	pnf("code node", sizeof(struct int_list));
	pnf("list node", sizeof(struct list));
	pnf("real number", sizeof(REAL));

	printf("\n- Size of C built-in types:\n");
	pnf("REAL", sizeof(REAL));
	pnf("long", sizeof(long));
	pnf("int", sizeof(unsigned long));
	pnf("pointer", sizeof(char*));

	printf("\n- System constants:\n");
	pnf("Maximum string or line length:", STRLEN);
	pnf("Parser stack depth:", PARSER_STACK_SIZE);
	pnf("Size of real numbers:", sizeof(REAL));
	printf("\n\n");
#endif
}
