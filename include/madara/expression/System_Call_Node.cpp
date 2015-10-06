
#ifndef _MADARA_NO_KARL_

#include "madara/expression/System_Call_Node.h"
#include "madara/knowledge/Thread_Safe_Context.h"


madara::expression::System_Call_Node::System_Call_Node (
    madara::knowledge::Thread_Safe_Context & context)
  : Composite_Ternary_Node (context.get_logger ()),
  context_ (context)
{

}


madara::expression::System_Call_Node::System_Call_Node (
  madara::knowledge::Thread_Safe_Context & context,
  const Component_Nodes & nodes)
: Composite_Ternary_Node (context.get_logger (), nodes),
  context_ (context)
{

}

// Dtor
madara::expression::System_Call_Node::~System_Call_Node (void)
{
}

madara::Knowledge_Record
madara::expression::System_Call_Node::item (void) const
{
  return madara::Knowledge_Record::Integer (nodes_.size ());
}

// accept a visitor
void 
madara::expression::System_Call_Node::accept (madara::expression::Visitor &visitor) const
{
}

#endif // _MADARA_NO_KARL_
