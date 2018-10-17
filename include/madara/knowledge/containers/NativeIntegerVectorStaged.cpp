#include "NativeIntegerVectorStaged.h"
#include "madara/knowledge/ContextGuard.h"

madara::knowledge::containers::NativeIntegerVectorStaged::
    NativeIntegerVectorStaged(const KnowledgeUpdateSettings& settings)
  : BaseContainer("", settings), context_(0), has_changed_(false)
{
  madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_MAJOR,
      "NativeIntegerVectorStaged::constructor: new object\n");
}

madara::knowledge::containers::NativeIntegerVectorStaged::
    NativeIntegerVectorStaged(const std::string& name, KnowledgeBase& knowledge,
        int size, const KnowledgeUpdateSettings& settings)
  : BaseContainer(name, settings),
    context_(&(knowledge.get_context())),
    has_changed_(false)
{
  madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_MAJOR,
      "NativeIntegerVectorStaged::constructor called for %s[%d]\n",
      name.c_str(), size);

  vector_ = knowledge.get_ref(name, KnowledgeUpdateSettings(true));
  if(size >= 0)
  {
    resize(size);
  }
}

madara::knowledge::containers::NativeIntegerVectorStaged::
    NativeIntegerVectorStaged(const std::string& name, Variables& knowledge,
        int size, const KnowledgeUpdateSettings& settings)
  : BaseContainer(name, settings),
    context_(knowledge.get_context()),
    has_changed_(false)
{
  madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_MAJOR,
      "NativeIntegerVectorStaged::constructor called for %s[%d]\n",
      name.c_str(), size);

  vector_ = knowledge.get_ref(name, settings);
  if(size >= 0)
  {
    resize(size);
  }
}

madara::knowledge::containers::NativeIntegerVectorStaged::
    NativeIntegerVectorStaged(const NativeIntegerVectorStaged& rhs)
  : BaseContainer(rhs),
    context_(rhs.context_),
    vector_(rhs.vector_),
    has_changed_(false)
{
  madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_MAJOR,
      "NativeIntegerVectorStaged::copy constructor called on %s\n",
      rhs.name_.c_str());
}

madara::knowledge::containers::NativeIntegerVectorStaged::
    ~NativeIntegerVectorStaged()
{
  madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_MAJOR,
      "NativeIntegerVectorStaged::destructor called on %s\n",
      this->name_.c_str());

  write();
}

madara::knowledge::containers::BaseContainer*
madara::knowledge::containers::NativeIntegerVectorStaged::clone(void) const
{
  return new NativeIntegerVectorStaged(*this);
}

bool madara::knowledge::containers::NativeIntegerVectorStaged::is_false_(
    void) const
{
  return is_false();
}

bool madara::knowledge::containers::NativeIntegerVectorStaged::is_true_(
    void) const
{
  return is_true();
}

inline void madara::knowledge::containers::NativeIntegerVectorStaged::modify_(
    void)
{
  modify();
}
