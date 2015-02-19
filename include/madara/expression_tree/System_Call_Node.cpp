
#ifndef _MADARA_NO_KARL_

#include "madara/expression_tree/System_Call_Node.h"


Madara::Expression_Tree::System_Call_Node::System_Call_Node (
    Madara::Knowledge_Engine::Thread_Safe_Context & context)
  : Composite_Ternary_Node (),
  context_ (context)
{

}


Madara::Expression_Tree::System_Call_Node::System_Call_Node (
  Madara::Knowledge_Engine::Thread_Safe_Context & context,
  const Component_Nodes & nodes)
  : Composite_Ternary_Node (nodes),
  context_ (context)
{

}

// Dtor
Madara::Expression_Tree::System_Call_Node::~System_Call_Node (void)
{
}

Madara::Knowledge_Record
Madara::Expression_Tree::System_Call_Node::item (void) const
{
  return Madara::Knowledge_Record::Integer (nodes_.size ());
}

// accept a visitor
void 
Madara::Expression_Tree::System_Call_Node::accept (Madara::Expression_Tree::Visitor &visitor) const
{
}

#endif // _MADARA_NO_KARL_
