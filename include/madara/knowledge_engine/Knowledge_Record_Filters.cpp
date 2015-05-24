#include "Knowledge_Record_Filters.h"
#include "madara/utility/Utility.h"
#include "madara/utility/Log_Macros.h"
#include "madara/filters/Arguments.h"
#include <memory>

#ifdef _MADARA_PYTHON_CALLBACKS_

#include <boost/python/call.hpp> 
#include "madara/python/Acquire_GIL.h"

#endif

#ifdef _MADARA_JAVA_

#include <jni.h>
#include "madara_jni.h"
#include "madara/utility/java/Acquire_VM.h"

#endif

Madara::Knowledge_Engine::Knowledge_Record_Filters::Knowledge_Record_Filters ()
  : context_ (0)
{
}

Madara::Knowledge_Engine::Knowledge_Record_Filters::Knowledge_Record_Filters (
  const Knowledge_Record_Filters & filters)
  : filters_ (filters.filters_), context_ (filters.context_),
    aggregate_filters_ (filters.aggregate_filters_)
{
}

Madara::Knowledge_Engine::Knowledge_Record_Filters::~Knowledge_Record_Filters ()
{
}

void
Madara::Knowledge_Engine::Knowledge_Record_Filters::operator= (
  const Knowledge_Record_Filters & rhs)
{
  if (this != &rhs)
  {
    filters_ = rhs.filters_;
    aggregate_filters_ = rhs.aggregate_filters_;
    context_ = rhs.context_;
  }
}

void
Madara::Knowledge_Engine::Knowledge_Record_Filters::add (uint32_t types,
        Knowledge_Record (*function) (Function_Arguments &, Variables &))
{
  if (function != 0)
  {
    // start with 1st bit, check every bit until types is 0
    for (uint32_t cur = 1; types > 0; cur <<= 1)
    {
      // if current is set in the bitmask
      if (Madara::Utility::bitmask_check (types, cur))
      {
        // remove the filter list from the type cur
        filters_[cur].push_back (Function (function));
      }

      // remove the current flag from the types
      types = Madara::Utility::bitmask_remove (types, cur);
    }
  }
}

void
Madara::Knowledge_Engine::Knowledge_Record_Filters::add (
  void (*function) (
    Knowledge_Map &, const Transport::Transport_Context &, Variables &))
{
  if (function != 0)
  {
    aggregate_filters_.push_back (Aggregate_Filter (function));
  }
}

void
Madara::Knowledge_Engine::Knowledge_Record_Filters::add (
  Filters::Aggregate_Filter * functor)
{
  if (functor != 0)
  {
    aggregate_filters_.push_back (Aggregate_Filter (functor));
  }
}

void
Madara::Knowledge_Engine::Knowledge_Record_Filters::add (
Filters::Buffer_Filter * functor)
{
  if (functor != 0)
  {
    buffer_filters_.push_back (functor);
  }
}

void
Madara::Knowledge_Engine::Knowledge_Record_Filters::add (
  uint32_t types,
  Filters::Record_Filter * functor)
{
  if (functor != 0)
  {
    // start with 1st bit, check every bit until types is 0
    for (uint32_t cur = 1; types > 0; cur <<= 1)
    {
      // if current is set in the bitmask
      if (Madara::Utility::bitmask_check (types, cur))
      {
        // remove the filter list from the type cur
        filters_[cur].push_back (Function (functor));
      }

      // remove the current flag from the types
      types = Madara::Utility::bitmask_remove (types, cur);
    }
  }
}

#ifdef _MADARA_JAVA_

void
Madara::Knowledge_Engine::Knowledge_Record_Filters::add (uint32_t types,
                                                         jobject & callable)
{
  if (callable != NULL)
  {
    // start with 1st bit, check every bit until types is 0
    for (uint32_t cur = 1; types > 0; cur <<= 1)
    {
      // if current is set in the bitmask
      if (Madara::Utility::bitmask_check (types, cur))
      {
        // remove the filter list from the type cur
        filters_[cur].push_back (Function (callable));
      }
      
      // remove the current flag from the types
      types = Madara::Utility::bitmask_remove (types, cur);
    }
  }
}

void
Madara::Knowledge_Engine::Knowledge_Record_Filters::add (
                                                         jobject & callable)
{
  if (callable != NULL)
  {
    aggregate_filters_.push_back (Aggregate_Filter (callable));
  }
}

#endif

#ifdef _MADARA_PYTHON_CALLBACKS_
      
void
Madara::Knowledge_Engine::Knowledge_Record_Filters::add (uint32_t types,
  boost::python::object & callable)
{
  if (!callable.is_none ())
  {
    // start with 1st bit, check every bit until types is 0
    for (uint32_t cur = 1; types > 0; cur <<= 1)
    {
      // if current is set in the bitmask
      if (Madara::Utility::bitmask_check (types, cur))
      {
        // remove the filter list from the type cur
        filters_[cur].push_back (Function (callable));
      }

      // remove the current flag from the types
      types = Madara::Utility::bitmask_remove (types, cur);
    }
  }
}

void
Madara::Knowledge_Engine::Knowledge_Record_Filters::add (
  boost::python::object & callable)
{
  if (!callable.is_none ())
  {
    aggregate_filters_.push_back (Aggregate_Filter (callable));
  }
}

#endif

void
Madara::Knowledge_Engine::Knowledge_Record_Filters::attach (
  Thread_Safe_Context * context)
{
  context_ = context;
}

void
Madara::Knowledge_Engine::Knowledge_Record_Filters::clear (uint32_t types)
{
  // start with 1st bit, check every bit until types is 0
  for (uint32_t cur = 1; types > 0; cur <<= 1)
  {
    // if current is set in the bitmask
    if (Madara::Utility::bitmask_check (types, cur))
    {
      // remove the filter list from the type cur
      filters_.erase (cur);
    }

    // remove the current flag from the types
    types = Madara::Utility::bitmask_remove (types, cur);
  }
}

void
Madara::Knowledge_Engine::Knowledge_Record_Filters::clear_aggregate_filters (
  void)
{
  aggregate_filters_.clear ();
}

void
Madara::Knowledge_Engine::Knowledge_Record_Filters::clear_buffer_filters (
void)
{
  buffer_filters_.clear ();
}

void
Madara::Knowledge_Engine::Knowledge_Record_Filters::print_num_filters (
  void) const
{
  MADARA_DEBUG (0, (LM_DEBUG,
    "Printing Knowledge Record Filter Chains by Type...\n"));

  for (Filter_Map::const_iterator i = filters_.begin ();
       i != filters_.end (); ++i)
  {
    MADARA_DEBUG (0, (LM_DEBUG,
      "%d = %d chained filters\n", i->first, i->second.size ()));
  }
  
  MADARA_DEBUG (0, (LM_DEBUG,
    "%d chained aggregate filters\n", aggregate_filters_.size ()));

  MADARA_DEBUG (0, (LM_DEBUG,
    "%d chained buffer filters\n", buffer_filters_.size ()));
}


Madara::Knowledge_Record
Madara::Knowledge_Engine::Knowledge_Record_Filters::filter (
  const Knowledge_Record & input,
  const std::string & name,
  Transport::Transport_Context & transport_context) const
{
  // grab the filter chain entry for the type
  uint32_t type = input.type ();
  Filter_Map::const_iterator type_match = filters_.find (type);
  Knowledge_Record result (input);

  // if there are filters for this type
  if (type_match != filters_.end ())
  {
    const Filter_Chain & chain = type_match->second;
    Function_Arguments arguments;
    
    // JVMs appear to do strange things with the stack on jni_attach
    std::auto_ptr <Variables> heap_variables (
      new Variables ());

    heap_variables->context_ = context_;
    
    for (Filter_Chain::const_iterator i = chain.begin ();
         i != chain.end (); ++i)
    {
      /**
       * arguments vector is modifiable by filter, so we have to
       * resize every filter call to make sure we have adequate space
       **/
      
      arguments.resize (Madara::Filters::TOTAL_ARGUMENTS);

      if (name != "")
      {
        // second argument is the variable name, if applicable
        arguments[1].set_value (name);
      }

      // third argument is the operation being performed
      arguments[2].set_value (Knowledge_Record::Integer (
        transport_context.get_operation ()));
      
      // fourth argument is the send/rebroadcast bandwidth utilization
      arguments[3].set_value (Knowledge_Record::Integer (
        transport_context.get_send_bandwidth ()));
      
      // fifth argument is the send/rebroadcast bandwidth utilization
      arguments[4].set_value (Knowledge_Record::Integer (
        transport_context.get_receive_bandwidth ()));

      // sixth argument is the message timestamp
      arguments[5].set_value (Knowledge_Record::Integer (
        transport_context.get_message_time ()));
      
      // sixth argument is the message timestamp
      arguments[6].set_value (Knowledge_Record::Integer (
        transport_context.get_current_time ()));

      // seventh argument is the networking domain
      arguments[7].set_value (transport_context.get_domain ());

      // eighth argument is the update originator
      arguments[8].set_value (transport_context.get_originator ());

      // setup arguments to the function
      arguments[0] = result;

      // optimize selection for functors, the preferred filter impl
      if (i->is_functor ())
      {
        result = i->functor->filter (arguments, *heap_variables.get ());
      }
      // if the function is not zero
      else if (i->is_extern_unnamed ())
      {
        result = i->extern_unnamed (arguments, *heap_variables.get ());
      }
      
#ifdef _MADARA_JAVA_
      else if (i->is_java_callable())
      {
        Madara::Utility::Java::Acquire_VM jvm;
        
        /**
         * Create the variables java object
         **/

        jclass jvarClass = jvm.env->FindClass ("com/madara/Variables");
        jclass jlistClass = jvm.env->FindClass ("com/madara/KnowledgeList");
        
        jmethodID fromPointerCall = jvm.env->GetStaticMethodID (jvarClass,
          "fromPointer", "(J)Lcom/madara/Variables;");
        jobject jvariables = jvm.env->CallStaticObjectMethod (jvarClass,
          fromPointerCall, (jlong)heap_variables.get ());
        
        // prep to create the KnowledgeList
        jmethodID listConstructor = jvm.env->GetMethodID(jlistClass,
          "<init>", "([J)V");
        
        jlongArray ret = jvm.env->NewLongArray((jsize)arguments.size());
        jlong * tmp = new jlong [(jsize)arguments.size()];

        for (unsigned int x = 0; x < arguments.size(); x++)
        {
          tmp[x] = (jlong) arguments[x].clone ();
        }

        jvm.env->SetLongArrayRegion(ret, 0, (jsize)arguments.size(), tmp);
        delete [] tmp;
        
        // create the KnowledgeList
        jobject jlist = jvm.env->NewObject (jlistClass, listConstructor, ret);

        // get the filter's class
        jclass filterClass = jvm.env->GetObjectClass(i->java_object);
        
        // get the filter method
        jmethodID filterMethod = jvm.env->GetMethodID (filterClass,
          "filter",
          "(Lcom/madara/KnowledgeList;Lcom/madara/Variables;)Lcom/madara/KnowledgeRecord;");
        
        // call the filter and hold the result
        jobject jresult = jvm.env->CallObjectMethod (i->java_object,
          filterMethod, jlist, jvariables);

        jmethodID getPtrMethod = jvm.env->GetMethodID (
          jvm.env->GetObjectClass(jresult), "getCPtr", "()J");
        jlong cptr = jvm.env->CallLongMethod (jresult, getPtrMethod);
        
        bool do_delete = true;
        //We need to see if they returned an arg we sent them, or a new value     
        for (unsigned int x = 0; x < arguments.size(); x++)
        {
          if (cptr == (jlong)&(arguments[x]))
          {
            do_delete = false;
            break;
          }
        }

        result.deep_copy (*(Madara::Knowledge_Record *)cptr);

        if (do_delete)
          delete (Knowledge_Record*)cptr;
      }
#endif
      
#ifdef _MADARA_PYTHON_CALLBACKS_

      else if (i->is_python_callable ())
      {
        // acquire the interpreter lock to use the python function
        Python::Acquire_GIL acquire_gil;

        // some guides have stated that we should let python handle exceptions
        result = boost::python::call <Madara::Knowledge_Record> (
          i->python_function.ptr (), boost::ref (arguments),
          boost::ref (*heap_variables.get ()));
      }

#endif

      // did the filter add records to be sent?
      if (arguments.size () > Madara::Filters::TOTAL_ARGUMENTS)
      {
        for (unsigned int i = Madara::Filters::TOTAL_ARGUMENTS;
          i + 1 < arguments.size (); i += 2)
        {
          if (arguments[i].is_string_type ())
          {
            MADARA_DEBUG (MADARA_LOG_MAJOR_EVENT, (LM_DEBUG,
              "Knowledge_Record_Filters::filter: Adding %s "
              "to transport context.\n", arguments[i].to_string ().c_str ()));
            transport_context.add_record (
              arguments[i].to_string (), arguments[i + 1]);
          }
          else
          {
            MADARA_DEBUG (MADARA_LOG_EMERGENCY, (LM_DEBUG,
              "Knowledge_Record_Filters::filter: ERROR. Filter attempted to"
              " add records to transport context, but args[%d] was not"
              " a string value.\n", i));
            break;
          }
        }
      }
    }
  }

  return result;
}

void
Madara::Knowledge_Engine::Knowledge_Record_Filters::filter (
  Knowledge_Map & records,
  const Transport::Transport_Context & transport_context) const
{
  // if there are aggregate filters
  if (aggregate_filters_.size () > 0)
  {
    // JVMs appear to do strange things with the stack on jni_attach
    std::auto_ptr <Variables> heap_variables (
      new Variables ());

    heap_variables->context_ = context_;
    
    for (Aggregate_Filters::const_iterator i = aggregate_filters_.begin ();
         i != aggregate_filters_.end (); ++i)
    {
      // optimize selection for functors, the preferred filter impl
      if (i->is_functor ())
      {
        i->functor->filter (records, transport_context,
          *heap_variables.get ());
      }
      // if the function is not zero
      else if (i->is_extern_unnamed ())
      {
        i->unnamed_filter (records, transport_context,
          *heap_variables.get ());
      }

#ifdef _MADARA_JAVA_
      else if (i->is_java_callable())
      {
        // manage VM attachment
        Madara::Utility::Java::Acquire_VM jvm;

        // JVMs appear to do strange things with the stack on jni_attach
        std::auto_ptr <Knowledge_Map> heap_records (new Knowledge_Map (records));
        std::auto_ptr <Transport::Transport_Context> heap_context (
          new Transport::Transport_Context (transport_context));

        //result = i->call_java_filter(arguments, variables);
        JNIEnv * env = jni_attach();
        
        /**
         * Create the variables java object
         **/
        jclass jvarClass = jvm.env->FindClass ("com/madara/Variables");
        jclass jpacketClass = jvm.env->FindClass (
          "com/madara/transport/filters/Packet");
        jclass jcontextClass = jvm.env->FindClass (
          "com/madara/transport/TransportContext");
        
        jmethodID varfromPointerCall = jvm.env->GetStaticMethodID (
          jvarClass,
          "fromPointer", "(J)Lcom/madara/Variables;");
        jobject jvariables = jvm.env->CallStaticObjectMethod (
          jvarClass,
          varfromPointerCall, (jlong) heap_variables.get ());
        
        jmethodID packetfromPointerCall = jvm.env->GetStaticMethodID (
          jpacketClass,
          "fromPointer", "(J)Lcom/madara/transport/filters/Packet;");
        jobject jpacket = jvm.env->CallStaticObjectMethod (jpacketClass,
          packetfromPointerCall, (jlong)heap_records.get ());
        
        jmethodID contextfromPointerCall = jvm.env->GetStaticMethodID (
          jcontextClass,
          "fromPointer", "(J)Lcom/madara/transport/TransportContext;");
        jobject jcontext = jvm.env->CallStaticObjectMethod (jcontextClass,
          contextfromPointerCall, (jlong)heap_context.get ());

        // get the filter's class and method
        jclass filterClass = jvm.env->GetObjectClass(i->java_object);
        jmethodID filterMethod = jvm.env->GetMethodID (filterClass,
          "filter",
          "(Lcom/madara/transport/filters/Packet;Lcom/madara/transport/TransportContext;Lcom/madara/Variables;)V");
        jvm.env->CallVoidMethod (i->java_object, filterMethod,
          jpacket, jcontext, jvariables);

        // overwrite the old records
        records = *heap_records.get ();

        // the auto_ptr should clear up the heap-allocated records
      }
#endif
      
#ifdef _MADARA_PYTHON_CALLBACKS_

      else if (i->is_python_callable ())
      {
        // acquire the interpreter lock to use the python function
        Python::Acquire_GIL acquire_gil;

        // some guides have stated that we should let python handle exceptions
        boost::python::call <Madara::Knowledge_Record> (
          i->python_function.ptr (),
          boost::ref (records), boost::ref (transport_context),
          boost::ref (*heap_variables.get ()));
      }

#endif

    }
  }
}

void Madara::Knowledge_Engine::Knowledge_Record_Filters::filter_decode (
  unsigned char * source, int size, int max_size) const
{
  // decode from back to front
  for (Filters::Buffer_Filters::const_reverse_iterator i = buffer_filters_.crbegin ();
    i != buffer_filters_.crend (); ++i)
  {
    (*i)->decode (source, size, max_size);
  }
}

void Madara::Knowledge_Engine::Knowledge_Record_Filters::filter_encode (
  unsigned char * source, int size, int max_size) const
{
  // encode from front to back
  for (Filters::Buffer_Filters::const_iterator i = buffer_filters_.begin ();
    i != buffer_filters_.end (); ++i)
  {
    (*i)->encode (source, size, max_size);
  }
}

size_t
Madara::Knowledge_Engine::Knowledge_Record_Filters::get_number_of_filtered_types (
  void) const
{
  return filters_.size ();
}

size_t
Madara::Knowledge_Engine::Knowledge_Record_Filters::get_number_of_aggregate_filters (
  void) const
{
  return aggregate_filters_.size ();
}

size_t
Madara::Knowledge_Engine::Knowledge_Record_Filters::get_number_of_buffer_filters (
void) const
{
  return buffer_filters_.size ();
}
