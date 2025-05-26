#include "wl_node.h"
wl_node::wl_node(char k, ptr_node l, ptr_node r, GENERIC d)
{
	key = k;
	left = l;
	right = r;
	data = d;
}

wl_node *wl_node::left_val()
{
	return left;
}

wl_node *wl_node::right_val()
{
	return right;
}

GENERIC wl_node::data_val()
{
	return data;
}


void wl_node::set_left(wl_node *val)
{
	left = val;
}
void wl_node::set_right(wl_node *val)
{
	right = val;
}
void wl_node::set_data(GENERIC val)
{
	data = val;
}

wl_node* wl_node::wl_node::stack_copy(wl_node *n)
{
	wl_node* n2;

	n2 = STACK_ALLOC(wl_node);
	*n2 = *n;
#ifdef TS
	n2->time_stamp = global_time_stamp; /* 9.6 */
#endif
	return n2;

}
wl_node* wl_node::heap_copy(wl_node *n)
{
	wl_node* n2;

	n2 = HEAP_ALLOC(wl_node);
	*n2 = *n;
#ifdef TS
	n2->time_stamp = global_time_stamp; /* 9.6 */
#endif
	return n2;
}

