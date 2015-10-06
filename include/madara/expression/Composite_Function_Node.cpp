/* -*- C++ -*- */
#ifndef _FUNCTION_NODE_CPP_
#define _FUNCTION_NODE_CPP_

#ifndef _MADARA_NO_KARL_

#include <iostream>
#include <sstream>

#include "madara/expression/Component_Node.h"
#include "madara/expression/Composite_Unary_Node.h"
#include "madara/expression/Visitor.h"
#include "madara/expression/Composite_Function_Node.h"
#include "madara/expression/Leaf_Node.h"


#include "madara/knowledge/Functions.h"
#include "madara/knowledge/Extern_Function_Variables.h"

#ifdef _MADARA_PYTHON_CALLBACKS_

  #include <boost/python/call.hpp> 

#endif

#ifdef _MADARA_JAVA_

#include <jni.h>
#include "madara_jni.h"
#include "madara/utility/java/Acquire_VM.h"

#endif

// Ctor

madara::expression::Composite_Function_Node::Composite_Function_Node (
        const std::string & name, 
        madara::knowledge::Thread_Safe_Context & context,
        const Component_Nodes & nodes)
: Composite_Ternary_Node (context.get_logger (), nodes),
  context_ (context), name_ (name), 
  function_ (context.retrieve_function (name))
{
  
}

// Dtor
madara::expression::Composite_Function_Node::~Composite_Function_Node (void)
{
}

madara::Knowledge_Record
madara::expression::Composite_Function_Node::item (void) const
{
  madara::Knowledge_Record record;
  record.set_value (name_ + "()");
  return record;
}

/// Prune the tree of unnecessary nodes. 
/// Returns evaluation of the node and sets can_change appropriately.
/// if this node can be changed, that means it shouldn't be pruned.
madara::Knowledge_Record
madara::expression::Composite_Function_Node::prune (bool & can_change)
{
  // user can always change a function, and we have no control over
  // what it does. Consequently, a function node cannot be pruned out
  // under any situation
  can_change = true;
  
  madara::Knowledge_Record result;

  // setup array of record pointers that point to .1, .2, .3, etc.
  if (nodes_.size () > 0)
    compiled_args_.resize (nodes_.size ());
  

  for (Component_Nodes::size_type i = 0; i < nodes_.size (); ++i)
  {
    bool arg_can_change = false;
    result = nodes_[i]->prune (arg_can_change);
    
    if (!arg_can_change && dynamic_cast <Leaf_Node *> (nodes_[i]) == 0)
    {
      delete nodes_[i];
      nodes_[i] = new Leaf_Node (*(this->logger_), result);
    }

    {
      // setup the corresponding compiled arg
      std::stringstream buffer;
      buffer << ".";
      buffer << i;
      compiled_args_[i] = context_.get_record (buffer.str ());
    }
  }

  return result;
}

/// Evaluates the node and its children. This does not prune any of
/// the expression tree, and is much faster than the prune function
madara::Knowledge_Record 
madara::expression::Composite_Function_Node::evaluate (
const madara::knowledge::Knowledge_Update_Settings & settings)
{
  madara::knowledge::Function_Arguments args;
  madara::Knowledge_Record result;

  args.resize (nodes_.size ());

  int j = 0;

  for (Component_Nodes::iterator i = nodes_.begin (); i != nodes_.end ();
       ++i, ++j)
  {
    args[j] = (*i)->evaluate (settings);
    *(compiled_args_[j]) = args[j];
  }


  madara::knowledge::Variables variables;
  variables.context_ = &context_;

  madara_logger_ptr_log (logger_, logger::LOG_DETAILED,
    "Function %s is being called with %d args.\n",
    this->name_.c_str (), args.size ());
  
  // if the user has defined a named function, return that
  if (function_->is_extern_named ())
    result = function_->extern_named (name_.c_str (), args, variables);

  // if the user has defined an unnamed function, return that
  else if (function_->is_extern_unnamed ())
    result = function_->extern_unnamed (args, variables);

#ifdef _MADARA_JAVA_
  else if (function_->is_java_callable())
  {
    madara::utility::Java::Acquire_VM jvm;
    JNIEnv * env = jvm.env;

    /**
      * Create the variables java object
      **/

    jclass jvarClass = madara::utility::Java::find_class (
      env, "com/madara/Variables");
    jclass jlistClass = madara::utility::Java::find_class (
      env, "com/madara/KnowledgeList");
        
    jmethodID fromPointerCall = env->GetStaticMethodID (jvarClass,
      "fromPointer", "(J)Lcom/madara/Variables;");
    jobject jvariables = env->CallStaticObjectMethod (jvarClass,
      fromPointerCall, (jlong) &variables);
        
    // prep to create the KnowledgeList
    jmethodID listConstructor = env->GetMethodID(jlistClass,
      "<init>", "([J)V");
        
    jlongArray ret = env->NewLongArray((jsize)args.size());
    jlong * tmp = new jlong [(jsize)args.size()];

    for (unsigned int x = 0; x < args.size(); x++)
    {
      tmp[x] = (jlong) args[x].clone ();
    }

    env->SetLongArrayRegion(ret, 0, (jsize)args.size(), tmp);
    delete [] tmp;
        
    // create the KnowledgeList
    jobject jlist = env->NewObject (jlistClass, listConstructor, ret);

    // get the filter's class
    jclass filterClass = env->GetObjectClass(function_->java_object);
        
    // get the filter method
    jmethodID filterMethod = env->GetMethodID (filterClass,
      "filter",
      "(Lcom/madara/KnowledgeList;Lcom/madara/Variables;)Lcom/madara/KnowledgeRecord;");
        
    // call the filter and hold the result
    jobject jresult = env->CallObjectMethod (function_->java_object,
      filterMethod, jlist, jvariables);

    jmethodID getPtrMethod = env->GetMethodID (
      env->GetObjectClass(jresult), "getCPtr", "()J");
    jlong cptr = env->CallLongMethod (jresult, getPtrMethod);
    
    bool do_delete = true;
    //We need to see if they returned an arg we sent them, or a new value     
    for (unsigned int x = 0; x < args.size(); x++)
    {
      if (cptr == (jlong)&(args[x]))
      {
        do_delete = false;
        break;
      }
    }

    result.deep_copy(*(madara::Knowledge_Record *)cptr);

    jvm.env->DeleteLocalRef (jresult);
    jvm.env->DeleteLocalRef (filterClass);
    jvm.env->DeleteLocalRef (jlist);
    jvm.env->DeleteLocalRef (ret);
    jvm.env->DeleteLocalRef (jvariables);
    jvm.env->DeleteWeakGlobalRef (jlistClass);
    jvm.env->DeleteWeakGlobalRef (jvarClass);

    //if (do_delete)
    //  delete (Knowledge_Record*)cptr;
  }
#endif
  
#ifdef _MADARA_PYTHON_CALLBACKS_
  else if (function_->is_python_callable ())
    return boost::python::call <madara::Knowledge_Record> (
          function_->python_function.ptr (),
          boost::ref (args), boost::ref (variables));
#endif

  // otherwise, assume it is a MADARA function
  else
  {
    result = function_->function_contents.evaluate ();
  }

  return result;
}

// accept a visitor
void 
madara::expression::Composite_Function_Node::accept (Visitor &visitor) const
{
  visitor.visit (*this);
}

#endif // _MADARA_NO_KARL_

#endif /* _FUNCTION_NODE_CPP_ */
