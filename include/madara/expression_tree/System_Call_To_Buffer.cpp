
#include "madara/utility/Log_Macros.h"
#include "madara/expression_tree/Leaf_Node.h"
#include "madara/expression_tree/System_Call_To_Buffer.h"
#include "madara/knowledge_engine/Thread_Safe_Context.h"
#include "madara/expression_tree/Visitor.h"


Madara::Expression_Tree::System_Call_To_Buffer::System_Call_To_Buffer (
  Madara::Knowledge_Engine::Thread_Safe_Context & context,
  const Component_Nodes & nodes)
  : System_Call_Node (context, nodes)
{

}

// Dtor
Madara::Expression_Tree::System_Call_To_Buffer::~System_Call_To_Buffer (void)
{
}

Madara::Knowledge_Record
Madara::Expression_Tree::System_Call_To_Buffer::item (void) const
{
  return Madara::Knowledge_Record::Integer (nodes_.size ());
}

/// Prune the tree of unnecessary nodes. 
/// Returns evaluation of the node and sets can_change appropriately.
/// if this node can be changed, that means it shouldn't be pruned.
Madara::Knowledge_Record
Madara::Expression_Tree::System_Call_To_Buffer::prune (bool & can_change)
{
  // user can always change a function, and we have no control over
  // what it does. Consequently, a function node cannot be pruned out
  // under any situation
  can_change = true;
  
  Madara::Knowledge_Record result;

  if (nodes_.size () > 0)
  {
    bool arg_can_change = false;
    result = nodes_[0]->prune (arg_can_change);
    
    if (!arg_can_change && dynamic_cast <Leaf_Node *> (nodes_[0]) == 0)
    {
      delete nodes_[0];
      nodes_[0] = new Leaf_Node (result);
    }
  }

  return result;
}

/// Evaluates the node and its children. This does not prune any of
/// the expression tree, and is much faster than the prune function
Madara::Knowledge_Record 
Madara::Expression_Tree::System_Call_To_Buffer::evaluate (
const Madara::Knowledge_Engine::Knowledge_Update_Settings & settings)
{
  Knowledge_Record return_value;

  if (nodes_.size () > 0)
  {
    MADARA_DEBUG (MADARA_LOG_MINOR_EVENT, (LM_DEBUG, 
      "System call to_buffer is converting an argument to an "
      "unmanaged buffer.\n"));

    size_t size;
    unsigned char * buffer =
      nodes_[0]->evaluate (settings).to_unmanaged_buffer (size);

    return_value.set_file (buffer, size);
  }
  else
  {
    MADARA_DEBUG (MADARA_LOG_MINOR_EVENT, (LM_DEBUG, 
      "System call to_buffer is converting 0 to a buffer.\n"));
    
    size_t size;
    unsigned char * buffer =
      return_value.to_unmanaged_buffer (size);

    return_value.set_file (buffer, size);
  }

  return return_value;
}

// accept a visitor
void 
Madara::Expression_Tree::System_Call_To_Buffer::accept (
  Madara::Expression_Tree::Visitor &visitor) const
{
  visitor.visit (*this);
}
