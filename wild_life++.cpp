// wild_life++.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

// #include <iostream>
/* Copyright 1991 Digital Equipment Corporation.
** All Rights Reserved.
*****************************************************************/
/* 	$Id: life.c,v 1.2 1994/12/08 23:27:02 duchier Exp $	 */
#define REV401PLUS
#ifndef lint
static char vcid[] = "$Id: life.c,v 1.2 1994/12/08 23:27:02 duchier Exp $";
#endif /* lint */

#ifdef REV401PLUS
#include "defs.h"
#endif


#ifdef X11
#include "xpred.h"
#endif

#ifdef SOLARIS
#include <stdlib.h>
static unsigned int lifeseed;
#endif

/******** MAIN(argc,argv)
This routine contains the Read-Solve-Prlong loop.
*/

int main(int argc, char *argv[])  // REV401PLUS correct main proto.
// int main(void)  // REV401PLUS correct main proto
{

  // int main()
  // {
  // {
  // next taken from REV233
  ptr_psi_term s;  
  ptr_stack save_undo_stack;
  long sort,exitflag;
  long c; /* 21.12 (prev. char) */ 

  int i;
#ifdef SOLARIS
  for(i=0;i<256;i++)
    rand_array[i]=srand(&lifeseed);
#else
  for(i=0;i<256;i++)
    rand_array[i]=rand();
#endif
  init_globals();
  arg_c=argc;
  if (argc < 10)
    {
      for (i = 0; i < argc;i++)
	{
	  arg_v[i]=argv[i];
	} 
    }
  else
    Errorline("Too many command line arguments\n");
  
  quietflag = GetBoolOption("q");

  str_constants = new str_const();



  init_io();
  wl_mem = new wl_alloc();
  // init_memory();
  exit_if_true(!wl_mem->mem_base_val() || !wl_mem->other_base_val());
  assert(wl_mem->stack_pointer_val() == wl_mem->mem_base_val()); /* 8.10 */
  init_copy();
  assert(wl_mem->stack_pointer_val() == wl_mem->mem_base_val()); /* 8.10 */
  init_print();
//   assert(stack_pointer==mem_base); /* 8.10 */
  assert(wl_mem->stack_pointer_val() == wl_mem->mem_base_val()); /* 8.10 */

  /* Timekeeping initialization */
  _tzset();
  life_start = clock();
  assert(stack_pointer==mem_base); /* 8.10 */
  assert(wl_mem->stack_pointer_val() == wl_mem->mem_base_val()); /* 8.10 */
  init_modules(); /*  RM: Jan  8 1993  */
  
  init_built_in_types();
  assert(stack_pointer==mem_base); /* 8.10 */
#ifdef X11
  x_setup_builtins();
  assert(stack_pointer==mem_base); /* 8.10 */
  assert(wl_mem->stack_pointer_val() == wl_mem->mem_base_val()); /* 8.10 */
#endif
//  init_interrupt();
//  assert(stack_pointer==mem_base); /* 8.10 */
  assert(wl_mem->stack_pointer_val() == wl_mem->mem_base_val()); /* 8.10 */
  title();
  assert(wl_mem->stack_pointer_val() == wl_mem->mem_base_val()); /* 8.10 */
  //assert(stack_pointer==mem_base); /* 8.10 */
  init_trace();
  noisy=FALSE;

 // assert(stack_pointer==mem_base); /* 8.10 */
  assert(wl_mem->stack_pointer_val() == wl_mem->mem_base_val()); /* 8.10 */

  set_current_module(user_module); /*  RM: Jan 27 1993  */
  
  /* Read in the .set_up file */
  init_system();
  
#ifdef ARITY  /*  RM: Mar 29 1993  */
  arity_init();
#endif

  
//   open_input_file("~/life_local/Source/.set_up");
  open_input_file((char*)".set_up");
  push_goal(load,input_state,(ptr_psi_term)file_date,(GENERIC)heap_copy_string("~/life_local/Source/.set_up")); // REV401PLUS casts

  file_date+=2;
  main_prove();


  /* Main loop of interpreter */
  do {
    setjmp(env);
    /* printf("%ld\n",(long)(stack_pointer-mem_base)); */ /* 8.10 */
    init_system(); 
    init_trace();

    begin_terminal_io();
    var_occurred=FALSE;
    save_undo_stack=undo_stack;
    stdin_cleareof();
    c=read_char();
    /* Wait until an EOF or a good character */
    while (c!=EOF && !(c>32 && c!='.' && c!=';')) c=read_char();
    if (c==EOF)
      exitflag=TRUE;
    else {
      put_back_char(c);
      s=stack_copy_psi_term(parse(&sort));
      exitflag=(s->type==eof);
    }
    end_terminal_io();

    if (!exitflag) {
      if (sort==QUERY) {

	/* clear_already_loaded(symbol_table);     RM: Feb  3 1993  */
	
        push_goal(what_next,(ptr_psi_term)TRUE,(ptr_psi_term)var_occurred,(GENERIC)1); // REV401PLUS casts
        ignore_eff=TRUE;
        goal_count=0;
        push_goal(prove,s,(ptr_psi_term)DEFRULES,NULL);  // REV401PLUS cast
        reset_step();
        start_chrono();
        main_prove();
        /* assert(goal_stack==NULL); */
        /* assert(choice_stack==NULL); */
	if (undo_stack) {
	  undo(NULL);
          Errorline("non-NULL undo stack.\n");
	}
        /* assert(undo_stack==NULL); */
      }
      else if (sort==FACT) {
        assert_first=FALSE;
        assert_clause(s);
        undo(save_undo_stack); /* 17.8 */
        var_occurred=FALSE; /* 18.8 */
        encode_types();
        Infoline(assert_ok?(char*)"\n*** Yes\n":(char*)"\n*** No\n"); /* 21.1 */
      }
    }
  } while (!exitflag);

  /* hash_display(x_module->symbol_table); */
  
  exit_life(TRUE);
}

void init_globals()
{
  first_definition=NULL;
  interrupted=TRUE;
  warningflag=TRUE;
  quietflag=FALSE;  // 21.1
  trace=FALSE;
  verbose=FALSE; // 21.1 

  module_table=NULL;        /* The table of modules */
  current_module=NULL;    /* The current module for the tokenizer */
  
  no_module=NULL;
  bi_module=NULL;
  syntax_module=NULL;
  sys_module=NULL;
  print_depth=PRINT_DEPTH;
  indent=FALSE;
  const_quote=TRUE;
  write_resids=FALSE;
  write_canon=FALSE;
  write_stderr=FALSE;
  write_corefs=TRUE;
  
  page_width=PAGE_WIDTH;
  
  display_persistent=FALSE;
  
  no_name=(char*)"pointer";
  name=(char*)"symbol";
  
  display_modules=TRUE;   /* Should really default to FALSE */
  
  numbers[0] = (char*)"1";
  numbers[1] = (char*)"2";
  numbers[2] = (char*)"3";
  numbers[3] = (char*)"4";
  numbers[4] = (char*)"5";
  numbers[5] = (char*)"6";
  numbers[6] = (char*)"7";
  numbers[7] = (char*)"8";
  numbers[8] = (char*)"9";
  numbers[9] = (char*)"10";
  numbers[10] = (char*)"11";
  numbers[11] = (char*)"12";
  numbers[12] = (char*)"13";
  numbers[13] = (char*)"14";
  numbers[14] = (char*)"15";
  numbers[15] = (char*)"16";
  numbers[16] = (char*)"17";
  numbers[17] = (char*)"18";
  numbers[18] = (char*)"19";
  numbers[19] = (char*)"20";
  numbers[20] = NULL;

  set_extra_args[0] = set_empty;
  set_extra_args[1] = set_1;
  set_extra_args[2] = set_2;
  set_extra_args[3] = set_1_2;
  set_extra_args[4] = set_1_2_3;
  set_extra_args[5] = set_1_2_3_4;

  old_state=NULL; /*  RM: Feb 17 1993  */
  trace_input=FALSE;

#ifdef X11
  xevent_existing = NULL;
  xevent_list = NULL;
  x_window_creation = FALSE;
#endif
  
}
//    std::cout << "Hello World!\n";
// }

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
