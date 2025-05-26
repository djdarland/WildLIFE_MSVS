#pragma once

#include "defs.h"
class wl_node
	/* Binary tree node. */
	/* KEY can be either an integer (a pointer) or a pointer to a string. */
	/* DATA is the information accessed under the KEY, in most cases a pointer */
	/* to a PSI-TERM.  */

{
private:
	char* key;
	wl_node *left;
	wl_node *right;
	GENERIC data;
public:
	wl_node(char *key, wl_node *left, wl_node *right, GENERIC data);
	char* key_val();
	wl_node *left_val();
	wl_node *right_val();
	GENERIC data_val();
	void set_left(wl_node *val);
	void set_right(wl_node *val);
	void set_data(GENERIC val);
	wl_node* stack_copy(wl_node* n);
	wl_node* heap_copy(wl_node* n);
};

