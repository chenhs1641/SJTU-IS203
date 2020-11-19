//
// See copyright.h for copyright notice and limitation of liability
// and disclaimer of warranty provisions.
//
#include "copyright.h"

extern int curr_lineno;
extern bool flag_nx;
///////////////////////////////////////////////////////////////////////////
//
// file: tree.cc
//
// This file defines the basic class of tree node
//
///////////////////////////////////////////////////////////////////////////

#include "tree.h"

/* line number to assign to the current node being constructed */
int node_lineno = 1;

///////////////////////////////////////////////////////////////////////////
//
// tree_node::tree_node
//
// constructor of tree node
//
///////////////////////////////////////////////////////////////////////////
tree_node::tree_node()
{
    if (flag_nx) node_lineno = curr_lineno;
    line_number = node_lineno;
}

///////////////////////////////////////////////////////////////////////////
//
// tree_node::get_line_number
//
///////////////////////////////////////////////////////////////////////////
int tree_node::get_line_number()
{	
	return line_number;
}

//
// Set up common area from existing node
//
tree_node *tree_node::set(tree_node *t) {
   line_number = t->line_number;
   return this;
}
