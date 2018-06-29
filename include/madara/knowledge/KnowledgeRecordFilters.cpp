#include "KnowledgeRecordFilters.h"
#include "madara/utility/Utility.h"

#include "madara/filters/Arguments.h"
#include <memory>
#include "madara/logger/GlobalLogger.h"

#ifdef _MADARA_PYTHON_CALLBACKS_

#include <boost/python/call.hpp> 
#include "madara/python/Acquire_GIL.h"

#endif

#ifdef _MADARA_JAVA_

#include <jni.h>
#include "madara_jni.h"
#include "madara/utility/java/Acquire_VM.h"

#endif

madara::knowledge::KnowledgeRecordFilters::KnowledgeRecordFilters ()
  : context_ (0)
{
}

madara::knowledge::KnowledgeRecordFilters::KnowledgeRecordFilters (
  const knowledge::KnowledgeRecordFilters & filters)
  : filters_ (filters.filters_),
    aggregate_filters_ (filters.aggregate_filters_),
    context_ (filters.context_)
{
}

madara::knowledge::KnowledgeRecordFilters::~KnowledgeRecordFilters ()
{
}

void
madara::knowledge::KnowledgeRecordFilters::operator= (
  const knowledge::KnowledgeRecordFilters & rhs)
{
  if (this != &rhs)
  {
    filters_ = rhs.filters_;
    aggregate_filters_ = rhs.aggregate_filters_;
    context_ = rhs.context_;
  }
}

void
madara::knowledge::KnowledgeRecordFilters::add (uint32_t types,
        knowledge::KnowledgeRecord (*function) (FunctionArguments &, Variables &))
{
  if (function != 0)
  {
    madara_logger_cond_log (context_,
      context_->get_logger (), logger::global_logger.get (),
      logger::LOG_MAJOR,
      "KnowledgeRecordFilters::add: "
      "Adding C record filter\n");

    // start with 1st bit, check every bit until types is 0
    for (uint32_t cur = 1; types > 0; cur <<= 1)
    {
      // if current is set in the bitmask
      if (madara::utility::bitmask_check (types, cur))
      {
        // remove the filter list from the type cur
        filters_[cur].push_back (Function (function));
      }

      // remove the current flag from the types
      types = madara::utility::bitmask_remove (types, cur);
    }
  }
}

void
madara::knowledge::KnowledgeRecordFilters::add (
  void (*function) (
    KnowledgeMap &, const transport::TransportContext &, Variables &))
{
  if (function != 0)
  {
    madara_logger_cond_log (context_,
      context_->get_logger (), logger::global_logger.get (),
      logger::LOG_MAJOR,
      "KnowledgeRecordFilters::add: "
      "Adding C aggregate filter\n");

    aggregate_filters_.push_back (AggregateFilter (function));
  }
}

void
madara::knowledge::KnowledgeRecordFilters::add (
  filters::AggregateFilter * functor)
{
  if (functor != 0)
  {
    madara_logger_cond_log (context_,
      context_->get_logger (), logger::global_logger.get (),
      logger::LOG_MAJOR,
      "KnowledgeRecordFilters::add: "
      "Adding aggregate functor filter\n");

    aggregate_filters_.push_back (AggregateFilter (functor));
  }
}

void
madara::knowledge::KnowledgeRecordFilters::add (
filters::BufferFilter * functor)
{
  if (functor != 0)
  {
    madara_logger_cond_log (context_,
      context_->get_logger (), logger::global_logger.get (),
      logger::LOG_MAJOR,
      "KnowledgeRecordFilters::add: "
      "Adding buffer functor filter\n");

    buffer_filters_.push_back (functor);
  }
}

void
madara::knowledge::KnowledgeRecordFilters::add (
  uint32_t types,
  filters::RecordFilter * functor)
{
  if (functor != 0)
  {
    madara_logger_cond_log (context_,
      context_->get_logger (), logger::global_logger.get (),
      logger::LOG_MAJOR,
      "KnowledgeRecordFilters::add: "
      "Adding record function filter to types\n");

    // start with 1st bit, check every bit until types is 0
    for (uint32_t cur = 1; types > 0; cur <<= 1)
    {
      // if current is set in the bitmask
      if (madara::utility::bitmask_check (types, cur))
      {
        // remove the filter list from the type cur
        filters_[cur].push_back (Function (functor));
      }

      // remove the current flag from the types
      types = madara::utility::bitmask_remove (types, cur);
    }
  }
}

#ifdef _MADARA_JAVA_

void
madara::knowledge::KnowledgeRecordFilters::add (uint32_t types,
                                                         jobject & callable)
{
  if (callable != NULL)
  {
    madara_logger_cond_log (context_,
      context_->get_logger (), logger::global_logger.get (),
      logger::LOG_MAJOR,
      "KnowledgeRecordFilters::add: "
      "Adding Java record filter\n");

    // start with 1st bit, check every bit until types is 0
    for (uint32_t cur = 1; types > 0; cur <<= 1)
    {
      // if current is set in the bitmask
      if (madara::utility::bitmask_check (types, cur))
      {
        // remove the filter list from the type cur
        filters_[cur].push_back (Function (callable));
      }
      
      // remove the current flag from the types
      types = madara::utility::bitmask_remove (types, cur);
    }
  }
}

void
madara::knowledge::KnowledgeRecordFilters::add (
                                                         jobject & callable)
{
  if (callable != NULL)
  {
    madara_logger_cond_log (context_,
      context_->get_logger (), logger::global_logger.get (),
      logger::LOG_MAJOR,
      "KnowledgeRecordFilters::add: "
      "Adding Java aggregate filter\n");

    aggregate_filters_.push_back (AggregateFilter (callable));
  }
}

#endif

#ifdef _MADARA_PYTHON_CALLBACKS_
      
void
madara::knowledge::KnowledgeRecordFilters::add (uint32_t types,
  boost::python::object & callable)
{
  if (!callable.is_none ())
  {
    // start with 1st bit, check every bit until types is 0
    for (uint32_t cur = 1; types > 0; cur <<= 1)
    {
      // if current is set in the bitmask
      if (madara::utility::bitmask_check (types, cur))
      {
        // remove the filter list from the type cur
        filters_[cur].push_back (Function (callable));
      }

      // remove the current flag from the types
      types = madara::utility::bitmask_remove (types, cur);
    }
  }
}

void
madara::knowledge::KnowledgeRecordFilters::add (
  boost::python::object & callable)
{
  if (!callable.is_none ())
  {
    aggregate_filters_.push_back (AggregateFilter (callable));
  }
}

#endif

void
madara::knowledge::KnowledgeRecordFilters::attach (
  ThreadSafeContext * context)
{
  context_ = context;
}

void
madara::knowledge::KnowledgeRecordFilters::clear (uint32_t types)
{
  // start with 1st bit, check every bit until types is 0
  for (uint32_t cur = 1; types > 0; cur <<= 1)
  {
    // if current is set in the bitmask
    if (madara::utility::bitmask_check (types, cur))
    {
      // remove the filter list from the type cur
      filters_.erase (cur);
    }

    // remove the current flag from the types
    types = madara::utility::bitmask_remove (types, cur);
  }
}

void
madara::knowledge::KnowledgeRecordFilters::clear_aggregate_filters (
  void)
{
  aggregate_filters_.clear ();
}

void
madara::knowledge::KnowledgeRecordFilters::clear_buffer_filters (
void)
{
  buffer_filters_.clear ();
}

void
madara::knowledge::KnowledgeRecordFilters::print_num_filters (
  void) const
{
  if (context_)
  {
    madara_logger_cond_log (context_,
      context_->get_logger (), logger::global_logger.get (),
      logger::LOG_ALWAYS,
      "Printing Knowledge Record Filter Chains by Type...\n");

    for (FilterMap::const_iterator i = filters_.begin ();
      i != filters_.end (); ++i)
    {
      madara_logger_cond_log (context_,
        context_->get_logger (), logger::global_logger.get (),
        logger::LOG_ALWAYS,
        "%d = %d chained filters\n", i->first, i->second.size ());
    }

    madara_logger_cond_log (context_,
      context_->get_logger (), logger::global_logger.get (),
      logger::LOG_ALWAYS,
      "%d chained aggregate filters\n", aggregate_filters_.size ());

    madara_logger_cond_log (context_,
      context_->get_logger (), logger::global_logger.get (),
      logger::LOG_ALWAYS,
      "%d chained buffer filters\n", buffer_filters_.size ());
  }
}


madara::knowledge::KnowledgeRecord
madara::knowledge::KnowledgeRecordFilters::filter (
  const knowledge::KnowledgeRecord & input,
  const std::string & name,
  transport::TransportContext & transport_context) const
{
  // grab the filter chain entry for the type
  uint32_t type = input.type ();
  FilterMap::const_iterator type_match = filters_.find (type);
  knowledge::KnowledgeRecord result (input);

  // if there are filters for this type
  if (type_match != filters_.end ())
  {
    madara_logger_cond_log (context_,
      context_->get_logger (), logger::global_logger.get (),
      logger::LOG_MAJOR,
      "KnowledgeRecordFilters::filter: "
      "Entering record filter logic\n");

    const FilterChain & chain = type_match->second;
    FunctionArguments arguments;
    
    // JVMs appear to do strange things with the stack on jni_attach
    std::unique_ptr <Variables> heap_variables (
      new Variables ());

    heap_variables->context_ = context_;
    
    for (FilterChain::const_iterator i = chain.begin ();
         i != chain.end (); ++i)
    {
      madara_logger_cond_log (context_,
        context_->get_logger (), logger::global_logger.get (),
        logger::LOG_MAJOR,
        "KnowledgeRecordFilters::filter: "
        "Preparing args for filter\n");

      /**
       * arguments vector is modifiable by filter, so we have to
       * resize every filter call to make sure we have adequate space
       **/
      
      arguments.resize (madara::filters::TOTAL_ARGUMENTS);

      if (name != "")
      {
        // second argument is the variable name, if applicable
        arguments[1].set_value (name);
      }

      // third argument is the operation being performed
      arguments[2].set_value (KnowledgeRecord::Integer (
        transport_context.get_operation ()));
      
      // fourth argument is the send/rebroadcast bandwidth utilization
      arguments[3].set_value (KnowledgeRecord::Integer (
        transport_context.get_send_bandwidth ()));
      
      // fifth argument is the send/rebroadcast bandwidth utilization
      arguments[4].set_value (KnowledgeRecord::Integer (
        transport_context.get_receive_bandwidth ()));

      // sixth argument is the message timestamp
      arguments[5].set_value (KnowledgeRecord::Integer (
        transport_context.get_message_time ()));
      
      // sixth argument is the message timestamp
      arguments[6].set_value (KnowledgeRecord::Integer (
        transport_context.get_current_time ()));

      // seventh argument is the networking domain
      arguments[7].set_value (transport_context.get_domain ());

      // eighth argument is the update originator
      arguments[8].set_value (transport_context.get_originator ());

      // setup arguments to the function
      arguments[0] = result;

      madara_logger_cond_log (context_,
        context_->get_logger (), logger::global_logger.get (),
        logger::LOG_MAJOR,
        "KnowledgeRecordFilters::filter: "
        "Checking filter type\n");

      // optimize selection for functors, the preferred filter impl
      if (i->is_functor ())
      {
        madara_logger_cond_log (context_,
          context_->get_logger (), logger::global_logger.get (),
          logger::LOG_MAJOR,
          "KnowledgeRecordFilters::filter: "
          "Calling functor filter\n");

        result = i->functor->filter (arguments, *heap_variables.get ());
      }
#ifdef _MADARA_JAVA_
      else if (i->is_java_callable ())
      {
        madara_logger_cond_log (context_,
          context_->get_logger (), logger::global_logger.get (),
          logger::LOG_MAJOR,
          "KnowledgeRecordFilters::filter: "
          "Calling Java filter\n");

        madara::utility::java::Acquire_VM jvm;

        /**
        * Create the variables java object
        **/

        jclass jvarClass = madara::utility::java::find_class (
          jvm.env, "ai/madara/knowledge/Variables");
        jclass jlistClass = madara::utility::java::find_class (
          jvm.env, "ai/madara/knowledge/KnowledgeList");

        jmethodID fromPointerCall = jvm.env->GetStaticMethodID (jvarClass,
          "fromPointer", "(J)Lai/madara/knowledge/Variables;");
        jobject jvariables = jvm.env->CallStaticObjectMethod (jvarClass,
          fromPointerCall, (jlong)heap_variables.get ());

        // prep to create the KnowledgeList
        jmethodID listConstructor = jvm.env->GetMethodID (jlistClass,
          "<init>", "([J)V");

        jlongArray ret = jvm.env->NewLongArray ((jsize)arguments.size ());
        jlong * tmp = new jlong[(jsize)arguments.size ()];

        for (unsigned int x = 0; x < arguments.size (); x++)
        {
          tmp[x] = (jlong)arguments[x].clone ();
        }

        jvm.env->SetLongArrayRegion (ret, 0, (jsize)arguments.size (), tmp);
        delete[] tmp;

        // create the KnowledgeList
        jobject jlist = jvm.env->NewObject (jlistClass, listConstructor, ret);

        // get the filter's class
        jclass filterClass = jvm.env->GetObjectClass (i->java_object);

        // get the filter method
        jmethodID filterMethod = jvm.env->GetMethodID (filterClass,
          "filter",
          "(Lai/madara/knowledge/KnowledgeList;"
          "Lai/madara/knowledge/Variables;)Lai/madara/knowledge/KnowledgeRecord;");

        madara_logger_cond_log (context_,
          context_->get_logger (), logger::global_logger.get (),
          logger::LOG_MAJOR,
          "KnowledgeRecordFilters::filter: "
          "Calling Java method\n");

        // call the filter and hold the result
        jobject jresult = jvm.env->CallObjectMethod (i->java_object,
          filterMethod, jlist, jvariables);

        madara_logger_cond_log (context_,
          context_->get_logger (), logger::global_logger.get (),
          logger::LOG_MINOR,
          "KnowledgeRecordFilters::filter: "
          "Obtaining pointer from Knowledge Record result\n");

        jmethodID getPtrMethod = jvm.env->GetMethodID (
          jvm.env->GetObjectClass (jresult), "getCPtr", "()J");
        jlong cptr = jvm.env->CallLongMethod (jresult, getPtrMethod);

        madara_logger_cond_log (context_,
          context_->get_logger (), logger::global_logger.get (),
          logger::LOG_MINOR,
          "KnowledgeRecordFilters::filter: "
          "Checking return for origination outside of filter\n");

        bool do_delete = true;
        //We need to see if they returned an arg we sent them, or a new value     
        for (unsigned int x = 0; x < arguments.size (); x++)
        {
          if (cptr == (jlong)&(arguments[x]))
          {
            do_delete = false;
            break;
          }
        }

        if (cptr != 0)
        {
          madara_logger_cond_log (context_,
            context_->get_logger (), logger::global_logger.get (),
            logger::LOG_MINOR,
            "KnowledgeRecordFilters::filter: "
            "Doing a deep copy of the return value\n");

          result = *(madara::knowledge::KnowledgeRecord *)cptr;
        }

        if (do_delete)
        {
          madara_logger_cond_log (context_,
            context_->get_logger (), logger::global_logger.get (),
            logger::LOG_MINOR,
            "KnowledgeRecordFilters::filter: "
            "Deleting the original pointer\n");

          delete (KnowledgeRecord*)cptr;
        }

        jvm.env->DeleteWeakGlobalRef (jvarClass);
        jvm.env->DeleteLocalRef (jvariables);
        jvm.env->DeleteLocalRef (jresult);
        jvm.env->DeleteLocalRef (jlist);
        jvm.env->DeleteWeakGlobalRef (jlistClass);
        jvm.env->DeleteLocalRef (ret);
      }
#endif

#ifdef _MADARA_PYTHON_CALLBACKS_

      else if (i->is_python_callable ())
      {
        madara_logger_cond_log (context_,
          context_->get_logger (), logger::global_logger.get (),
          logger::LOG_MAJOR,
          "KnowledgeRecordFilters::filter: "
          "Calling Python filter\n");

        // acquire the interpreter lock to use the python function
        madara::python::Acquire_GIL acquire_gil;

        // some guides have stated that we should let python handle exceptions
        result = boost::python::call <madara::knowledge::KnowledgeRecord> (
          i->python_function.ptr (), boost::ref (arguments),
          boost::ref (*heap_variables.get ()));
      }

#endif

      // if the function is not zero
      else if (i->is_extern_unnamed ())
      {
        madara_logger_cond_log (context_,
          context_->get_logger (), logger::global_logger.get (),
          logger::LOG_MAJOR,
          "KnowledgeRecordFilters::filter: "
          "Calling unnamed C filter\n");

        result = i->extern_unnamed (arguments, *heap_variables.get ());
      }
      
      // did the filter add records to be sent?
      if (arguments.size () > madara::filters::TOTAL_ARGUMENTS)
      {
        for (unsigned int j = madara::filters::TOTAL_ARGUMENTS;
          j + 1 < arguments.size (); j += 2)
        {
          if (arguments[j].is_string_type ())
          {
            madara_logger_cond_log (context_,
              context_->get_logger (), logger::global_logger.get (),
              logger::LOG_MAJOR,
              "KnowledgeRecordFilters::filter: Adding %s "
              "to transport context.\n", arguments[j].to_string ().c_str ());

            transport_context.add_record (
              arguments[j].to_string (), arguments[j + 1]);
          }
          else
          {
            madara_logger_cond_log (context_,
              context_->get_logger (), logger::global_logger.get (),
              logger::LOG_ALWAYS,
              "KnowledgeRecordFilters::filter: ERROR. Filter attempted to"
              " add records to transport context, but args[%d] was not"
              " a string value.\n", j);

            break;
          }
        }
      }
    }
  }

  return result;
}

void
madara::knowledge::KnowledgeRecordFilters::filter (
  KnowledgeMap & records,
  const transport::TransportContext & transport_context) const
{
  // if there are aggregate filters
  if (aggregate_filters_.size () > 0)
  {
    madara_logger_cond_log (context_,
      context_->get_logger (), logger::global_logger.get (),
      logger::LOG_MAJOR,
      "KnowledgeRecordFilters::filter: "
      "Entering aggregate filter method\n");

    // JVMs appear to do strange things with the stack on jni_attach
    std::unique_ptr <Variables> heap_variables (
      new Variables ());

    heap_variables->context_ = context_;
    
    for (AggregateFilters::const_iterator i = aggregate_filters_.begin ();
         i != aggregate_filters_.end (); ++i)
    {
      // optimize selection for functors, the preferred filter impl
      if (i->is_functor ())
      {
        madara_logger_cond_log (context_,
          context_->get_logger (), logger::global_logger.get (),
          logger::LOG_MINOR,
          "KnowledgeRecordFilters::filter: "
          "Checking vars for null\n");

        Variables * vars = heap_variables.get ();
        if (vars)
        {
          madara_logger_cond_log (context_,
            context_->get_logger (), logger::global_logger.get (),
            logger::LOG_MAJOR,
            "KnowledgeRecordFilters::filter: "
            "Calling C++ filter\n");

          i->functor->filter (records, transport_context,
            *vars);

          madara_logger_cond_log (context_,
            context_->get_logger (), logger::global_logger.get (),
            logger::LOG_MAJOR,
            "KnowledgeRecordFilters::filter: "
            "Finished calling C++ filter\n");
        }
        else
        {
          madara_logger_cond_log (context_,
            context_->get_logger (), logger::global_logger.get (),
            logger::LOG_ERROR,
            "KnowledgeRecordFilters::filter: "
            "Memory issues caused vars to be null before filtering\n");
        }
      }
#ifdef _MADARA_JAVA_
      else if (i->is_java_callable ())
      {
        // manage VM attachment
        madara::utility::java::Acquire_VM jvm;

        // JVMs appear to do strange things with the stack on jni_attach
        std::unique_ptr <KnowledgeMap> heap_records (new KnowledgeMap (records));
        std::unique_ptr <transport::TransportContext> heap_context (
          new transport::TransportContext (transport_context));

        /**
        * Create the variables java object
        **/
        jclass jvarClass = madara::utility::java::find_class (jvm.env,
          "ai/madara/knowledge/Variables");
        jclass jpacketClass = madara::utility::java::find_class (jvm.env,
          "ai/madara/transport/filters/Packet");
        jclass jcontextClass = madara::utility::java::find_class (jvm.env,
          "ai/madara/transport/TransportContext");

        jmethodID varfromPointerCall = jvm.env->GetStaticMethodID (
          jvarClass,
          "fromPointer", "(J)Lai/madara/knowledge/Variables;");
        jobject jvariables = jvm.env->CallStaticObjectMethod (
          jvarClass,
          varfromPointerCall, (jlong)heap_variables.get ());

        jmethodID packetfromPointerCall = jvm.env->GetStaticMethodID (
          jpacketClass,
          "fromPointer", "(J)Lai/madara/transport/filters/Packet;");
        jobject jpacket = jvm.env->CallStaticObjectMethod (jpacketClass,
          packetfromPointerCall, (jlong)heap_records.get ());

        jmethodID contextfromPointerCall = jvm.env->GetStaticMethodID (
          jcontextClass,
          "fromPointer", "(J)Lai/madara/transport/TransportContext;");
        jobject jcontext = jvm.env->CallStaticObjectMethod (jcontextClass,
          contextfromPointerCall, (jlong)heap_context.get ());

        // get the filter's class and method
        jclass filterClass = jvm.env->GetObjectClass (i->java_object);
        jmethodID filterMethod = jvm.env->GetMethodID (filterClass,
          "filter",
          "(Lai/madara/transport/filters/Packet;"
          "Lai/madara/transport/TransportContext;Lai/madara/knowledge/Variables;)V");


        madara_logger_cond_log (context_,
          context_->get_logger (), logger::global_logger.get (),
          logger::LOG_MAJOR,
          "KnowledgeRecordFilters::filter: "
          "Calling Java method\n");

        jvm.env->CallVoidMethod (i->java_object, filterMethod,
          jpacket, jcontext, jvariables);

        madara_logger_cond_log (context_,
          context_->get_logger (), logger::global_logger.get (),
          logger::LOG_MAJOR,
          "KnowledgeRecordFilters::filter: "
          "Overwriting resulting records from heap records\n");

        // overwrite the old records
        //records = *heap_records.get ();

        for (KnowledgeMap::const_iterator i = heap_records->begin ();
          i != heap_records->end (); ++i)
        {
          records[i->first] = i->second;
        }

        jvm.env->DeleteLocalRef (filterClass);
        jvm.env->DeleteLocalRef (jcontext);
        jvm.env->DeleteLocalRef (jpacket);
        jvm.env->DeleteWeakGlobalRef (jvarClass);
        jvm.env->DeleteWeakGlobalRef (jpacketClass);
        jvm.env->DeleteWeakGlobalRef (jcontextClass);
        // the auto_ptr should clear up the heap-allocated records
      }
#endif

#ifdef _MADARA_PYTHON_CALLBACKS_

      else if (i->is_python_callable ())
      {
        // acquire the interpreter lock to use the python function
        madara::python::Acquire_GIL acquire_gil;

        // some guides have stated that we should let python handle exceptions
        boost::python::call <madara::knowledge::KnowledgeRecord> (
          i->python_function.ptr (),
          boost::ref (records), boost::ref (transport_context),
          boost::ref (*heap_variables.get ()));
      }

#endif

      // if the function is not zero
      else if (i->is_extern_unnamed ())
      {
        i->unnamed_filter (records, transport_context,
          *heap_variables.get ());
      }

    }
  }
}

void madara::knowledge::KnowledgeRecordFilters::filter_decode (
  unsigned char * source, int size, int max_size) const
{
  // decode from back to front
  for (filters::BufferFilters::const_reverse_iterator i = buffer_filters_.rbegin ();
    i != buffer_filters_.rend (); ++i)
  {
    (*i)->decode (source, size, max_size);
  }
}

void madara::knowledge::KnowledgeRecordFilters::filter_encode (
  unsigned char * source, int size, int max_size) const
{
  // encode from front to back
  for (filters::BufferFilters::const_iterator i = buffer_filters_.begin ();
    i != buffer_filters_.end (); ++i)
  {
    (*i)->encode (source, size, max_size);
  }
}

size_t
madara::knowledge::KnowledgeRecordFilters::get_number_of_filtered_types (
  void) const
{
  return filters_.size ();
}

size_t
madara::knowledge::KnowledgeRecordFilters::get_number_of_aggregate_filters (
  void) const
{
  return aggregate_filters_.size ();
}

size_t
madara::knowledge::KnowledgeRecordFilters::get_number_of_buffer_filters (
void) const
{
  return buffer_filters_.size ();
}
