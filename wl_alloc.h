#pragma once
#include "defs.h"
#define EXTERN extern

#ifdef TS
/* Trail if q was last modified before the topmost choice point */
#define TRAIL_CONDITION(Q) (choice_stack && \
                            choice_stack->time_stamp>=Q->time_stamp)
#endif

class wl_alloc
{
private:
	unsigned long delta;
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
public:
	wl_alloc();
	GENERIC heap_alloc(unsigned long s);
	GENERIC stack_alloc(unsigned long s);
	long memory_check();
	long wl_alloc::c_statistics();

};

