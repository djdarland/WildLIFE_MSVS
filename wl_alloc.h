#pragma once
#include "defs.h"
#define EXTERN extern
#define LONELY 1
#ifdef TS
/* Trail if q was last modified before the topmost choice point */
#define TRAIL_CONDITION(Q) (choice_stack && \
                            choice_stack->time_stamp>=Q->time_stamp)
#endif

class wl_alloc
{
private:
	unsigned long delta;
	long pass;
	float gc_time, life_time;
	clock_t last_garbage_time;

#ifdef TS
	void push_psi_ptr_value(); /* 9.6 */
	unsigned long global_time_stamp; /* 9.6 */
	/*! \var alloc_words
  \brief number of words to allocate - from either command lind or ALLOC_WORDS define

*/

	unsigned long alloc_words;

	/*! \var mem_size
	  \brief number of words from memory = alloc_words * sizeof word

	*/

	unsigned long mem_size;

	/*! \var mem_base
	  \brief mem_size memory allocated in init_memory by malloc

	*/

	GENERIC mem_base;
	GENERIC other_base;

	/*! \var heap_pointer
	  \brief used to allocate from heap - size allocated subtracted - adj for alignment

	*/

	GENERIC heap_pointer;

	/*! mem_limit
	  \brief starting point of heap - mem_base aligned

	*/

	GENERIC mem_limit;

	/*! \var stack_pointer
	  \brief used to allocate from stack - size allocated added - adj for alignment

	*/

	GENERIC stack_pointer;
#endif

void pchoices();

/****************************************************************************/

/* GC sanity checks */

/* Keep for release versions, unless the garbage collector is very robust */
/* #define GCTEST */

/* Remove for release versions */
/* #define GCVERBOSE */

#ifndef GCTEST
#undef assert
#define assert(N)
#endif

void print_undo_stack();

long bounds_undo_stack();
void fail_all();

 void compress();

#define UNCHECKED(P) (! *((GENERIC)(P)+delta))

long unchecked (GENERIC *p, long len);



 void check_string (GENERIC *s);
 void check_bytedata(GENERIC *s);
 void check_code(ptr_int_list *c);
 void check_pair_list(ptr_pair_list *p);
 void check_triple_list(ptr_triple_list *p);
 void check_kids(ptr_int_list *c);
 void check_operator_data(ptr_operator_data *op);
void check_hash_table();          /*  RM: Feb  3 1993  */
 void check_module_list(ptr_int_list *c);    /*  RM: Jan 12 1993  */
 void check_module_tree(ptr_node *n);    /*  RM: Jan 13 1993  */

 void check_module(ptr_module *m);        /*  RM: Jan 12 1993  */
     
void check_hash_table(ptr_hash_table table); /*  RM: Feb  3 1993  */
     
 void check_keyword(ptr_keyword *k);      /*  RM: Jan 12 1993  */

void check_definition_list();   /*  RM: Feb 15 1993  */
 void check_def_code(ptr_definition *d);
 void check_def_rest(ptr_definition *d);
 void check_symbol(ptr_node *n);
 void check_type_disj(ptr_int_list *p);
 void check_goal_stack(ptr_goal *g);
 void check_resid(ptr_residuation *r);
void check_resid_block(ptr_resid_block *rb);
void check_psi_term(ptr_psi_term *t);
void check_attr(ptr_node *n);
void check_gamma_code();
 void check_gamma_rest();
 void check_undo_stack(ptr_stack *s);
 void check_choice_structs(ptr_choice_point *c);
 void check_choice(ptr_choice_point *c);
 void check_special_addresses();
 void check_psi_list(ptr_int_list *l);
 void check_resid_list(ptr_resid_list *l);
 void check_var(ptr_node *n);
 void check();
void print_gc_info(long timeflag);



public:
	wl_alloc();
	GENERIC heap_alloc(unsigned long s);
	GENERIC stack_alloc(unsigned long s);
	long memory_check();
	long c_statistics();
	GENERIC heap_pointer_val();
	GENERIC stack_pointer_val();
	unsigned long alloc_words_val();
	GENERIC mem_base_val();
	GENERIC mem_limit_val();
	GENERIC other_base_val();
	void set_stack_pointer(GENERIC val);
	void set_heap_pointer(GENERIC val);
	void garbage();
	void check_definition(ptr_definition* d);
};

