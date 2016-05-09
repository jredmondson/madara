#include "NativeDoubleVectorStaged.h"
#include "madara/knowledge/ContextGuard.h"
#include "madara/logger/GlobalLogger.h"

madara::knowledge::containers::NativeDoubleVectorStaged::NativeDoubleVectorStaged (
  const KnowledgeUpdateSettings & settings)
: BaseContainer ("", settings), context_ (0), has_changed_ (false)
{
  madara_logger_ptr_log (logger::global_logger.get (),
    logger::LOG_MAJOR,
    "NativeDoubleVectorStaged::constructor: new object\n");
}
   
madara::knowledge::containers::NativeDoubleVectorStaged::NativeDoubleVectorStaged (
  const std::string & name,
  KnowledgeBase & knowledge,
  int size,
  const KnowledgeUpdateSettings & settings)
  : BaseContainer (name, settings), context_ (&(knowledge.get_context ())),
    has_changed_ (false)
{
  madara_logger_ptr_log (logger::global_logger.get (),
    logger::LOG_MAJOR,
    "NativeDoubleVectorStaged::constructor called for %s[%d]\n",
    name.c_str (), size);

  vector_ = knowledge.get_ref (name, settings_);
  if (size >= 0)
  {
    resize (size);
  }
}
  
madara::knowledge::containers::NativeDoubleVectorStaged::NativeDoubleVectorStaged (
  const std::string & name,
  Variables & knowledge,
  int size,
  const KnowledgeUpdateSettings & settings)
: context_ (knowledge.get_context ()), has_changed_ (false)
{
  madara_logger_ptr_log (logger::global_logger.get (),
    logger::LOG_MAJOR,
    "NativeDoubleVectorStaged::constructor called for %s[%d]\n",
    name.c_str (), size);

  vector_ = knowledge.get_ref (name, settings);
  if (size >= 0)
  {
    resize (size);
  }
}

madara::knowledge::containers::NativeDoubleVectorStaged::NativeDoubleVectorStaged (
  const NativeDoubleVectorStaged & rhs)
: BaseContainer (rhs), context_ (rhs.context_),
  vector_ (rhs.vector_)
{
  madara_logger_ptr_log (logger::global_logger.get (),
    logger::LOG_MAJOR,
    "NativeDoubleVectorStaged::copy constructor called on %s\n",
    rhs.name_.c_str ());
}


madara::knowledge::containers::NativeDoubleVectorStaged::~NativeDoubleVectorStaged ()
{
  madara_logger_ptr_log (logger::global_logger.get (),
    logger::LOG_MAJOR,
    "NativeDoubleVectorStaged::destructor called on %s\n",
    this->name_.c_str ());

  write ();
}

madara::knowledge::containers::BaseContainer *
madara::knowledge::containers::NativeDoubleVectorStaged::clone (void) const
{
  madara_logger_ptr_log (logger::global_logger.get (),
    logger::LOG_MAJOR,
    "NativeDoubleVectorStaged::clone: cloning %s\n",
    this->name_.c_str ());

  return new NativeDoubleVectorStaged (*this);
}

bool
madara::knowledge::containers::NativeDoubleVectorStaged::is_false_ (void) const
{
  return is_false ();
}

bool
madara::knowledge::containers::NativeDoubleVectorStaged::is_true_ (void) const
{
  return is_true ();
}

void
madara::knowledge::containers::NativeDoubleVectorStaged::modify_ (void)
{
  modify ();
}
