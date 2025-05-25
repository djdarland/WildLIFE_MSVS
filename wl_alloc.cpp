#include "wl_alloc.h"

/****************************************************************************

  MEMORY ALLOCATION ROUTINES.

*/


/******** HEAP_ALLOC(s)
  This returns a pointer to S bytes of memory in the heap.
  Alignment is taken into account in the following manner:
  the macro ALIGN is supposed to be a power of 2 and the pointer returned
  is a multiple of ALIGN.
*/
GENERIC wl_alloc::heap_alloc(unsigned long s)
// long s;
{
	if (s & (ALIGN - 1))
		s = s - (s & (ALIGN - 1)) + ALIGN;
	/* assert(s % sizeof(*heap_pointer) == 0); */
	s /= sizeof(*heap_pointer);

	heap_pointer -= s;

	if (stack_pointer > heap_pointer)
		Errorline("the heap overflowed into the stack.\n");

	return heap_pointer;
}



/******** STACK_ALLOC(s)
  This returns a pointer to S bytes of memory in the stack.
  Alignment is taken into account in the following manner:
  the macro ALIGN is supposed to be a power of 2 and the pointer returned
  is a multiple of ALIGN.
*/
GENERIC wl_alloc::stack_alloc(unsigned long s)
// long s;
{
	GENERIC r;

	r = stack_pointer;

	if (s & (ALIGN - 1))
		s = s - (s & (ALIGN - 1)) + ALIGN;
	/* assert(s % sizeof(*stack_pointer) == 0); */
	s /= sizeof(*stack_pointer);

	stack_pointer += s;

	if (stack_pointer > heap_pointer)
		Errorline("the stack overflowed into the heap.\n");

	return r;
}



/******** INIT_MEMORY()
  Get two big blocks of memory to work in.
  The second is only used for the half-space garbage collector.
  The start and end addresses of the blocks are re-aligned correctly.
  to allocate.
*/


wl_alloc::wl_alloc()
{
	printf("ALLOC_WORDS = %ld\n", ALLOC_WORDS);
	//   alloc_words=GetIntOption("memory",ALLOC_WORDS);
	alloc_words = ALLOC_WORDS;
	mem_size = alloc_words * sizeof(long);

	mem_base = (GENERIC)malloc(mem_size);
	other_base = (GENERIC)malloc(mem_size);
	printf("mem_base = %p other_base = %p \n", (void*)mem_base, (void*)other_base);
	if (mem_base && other_base) {
		/* Rewrote some rather poor code... RM: Mar  1 1994  */
		// ALIGNUP(mem_base);
		stack_pointer = mem_base;
		printf("stack_pointer = %p\n", (void*)stack_pointer);
		mem_limit = mem_base + alloc_words - 2;
		// ALIGNUP(mem_limit);
		heap_pointer = mem_limit;
		printf("heap_pointer = %p\n", (void*)heap_pointer);
		// ALIGNUP(other_base);
		other_pointer = other_base;
		printf("other_pointer=%p\n", (void*)other_pointer);
		other_limit = other_base + alloc_words - 2;
		printf("other_limit = %p\n", (void*)other_limit);
		// ALIGNUP(other_limit);

		delta = other_base - mem_base;
		printf("delta =%ld\n", delta);
		buffer = (GENERIC)malloc(PRINT_BUFFER); /* The printing buffer */

		/*  RM: Oct 22 1993  */
		/* Fill the memory with rubbish data */
		/*
		{
		  int i;

		  for(i=0;i<alloc_words;i++) {
		mem_base[i]= -1234;
		other_base[i]= -1234;
		  }
		}
		*/
	}
	else
		Errorline("Wild_life could not allocate sufficient memory to run.\n\n");
}



/******** MEMORY_CHECK()
  This function tests to see whether enough memory is available to allow
  execution to continue.  It causes a garbage collection if not, and if that
  fails to release enough memory it returns FALSE.
*/
long wl_alloc::memory_check()
{
	long success = TRUE;

	if (heap_pointer - stack_pointer < GC_THRESHOLD) {
		if (verbose) fprintf(stderr, "\n"); /*  RM: Feb  1 1993  */
		//     garbage();
			/* Abort if didn't recover at least GC_THRESHOLD/10 of memory */
		if (heap_pointer - stack_pointer < GC_THRESHOLD + GC_THRESHOLD / 10) {
			fprintf(stderr, "*********************\n");
			fprintf(stderr, "*** OUT OF MEMORY ***\n");
			fprintf(stderr, "*********************\n");
			//    fail_all();
			success = FALSE;
		}
	}
	return success;
}
long wl_alloc::c_statistics()
{
	ptr_psi_term t;
	long success = TRUE;
	long t1, t2, t3;

	t = aim->aaaa_1;
	deref_args(t, set_empty);

	t1 = sizeof(mem_base) * (stack_pointer - mem_base);
	t2 = sizeof(mem_base) * (mem_limit - heap_pointer);
	t3 = sizeof(mem_base) * (mem_limit - mem_base);

	printf("\n");
	/* printf("************** SYSTEM< INFORMATION **************\n"); */
	printf("Stack size  : %8ld bytes (%5ldK) (%ld%%)\n", t1, t1 / 1024, 100 * t1 / t3);
	printf("Heap size   : %8ld bytes (%5ldK) (%ld%%)\n", t2, t2 / 1024, 100 * t2 / t3);
	printf("Total memory: %8ld bytes (%5ldK)\n", t3, t3 / 1024);

#ifdef X11
	printf("X predicates are installed.\n");
#else
	printf("X predicates are not installed.\n");
#endif

	/* printf("\n"); */
	/* printf("************************************************\n"); */
	return success;
}



