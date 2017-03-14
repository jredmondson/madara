#include "ZMQContext.h"
#include <zmq.h>
#include "madara/logger/GlobalLogger.h"

madara::transport::ZMQContext madara::transport::zmq_context;

madara::transport::ZMQContext::ZMQContext ()
  : context_ (0), references_ (0)
{
  madara_logger_ptr_log (logger::global_logger.get (), logger::LOG_MAJOR,
    "ZMQContext::constr:" \
    " calling constructor: context=%p, references=%d\n",
    context_, references_);
}

void
madara::transport::ZMQContext::set_context (void * context)
{
  madara_logger_ptr_log (logger::global_logger.get (), logger::LOG_MAJOR,
    "ZMQContext::set_context:" \
    " setting the context\n");

  if (context != context_)
  {
    madara_logger_ptr_log (logger::global_logger.get (), logger::LOG_MAJOR,
      "ZMQContext::set_context:" \
      " context is different from new context. Destroying old context.\n");

    destroy_context ();

    madara_logger_ptr_log (logger::global_logger.get (), logger::LOG_MAJOR,
      "ZMQContext::set_context:" \
      " updating context and reference count.\n");

    context_ = context;
    references_ = 1;

    madara_logger_ptr_log (logger::global_logger.get (), logger::LOG_MAJOR,
      "ZMQContext::set_context:" \
      " result: context=%p, references=%d\n",
      context_, references_);
  }
}

void
madara::transport::ZMQContext::add_ref (void)
{
  if (context_ == 0)
  {
    madara_logger_ptr_log (logger::global_logger.get (), logger::LOG_MAJOR,
      "ZMQContext::add_ref:" \
      " context didn't exist. Creating new context.\n",
      context_, references_);

    references_ = 1;
    context_ = zmq_ctx_new ();
  }
  else
  {
    ++references_;
  }

  madara_logger_ptr_log (logger::global_logger.get (), logger::LOG_MAJOR,
    "ZMQContext::add_ref:" \
    " result: context=%p, references=%d\n",
    context_, references_);
}

void
madara::transport::ZMQContext::rem_ref (void)
{
  if (references_ > 0)
  {
    madara_logger_ptr_log (logger::global_logger.get (), logger::LOG_MAJOR,
      "ZMQContext::rem_ref:" \
      " removing reference from context.\n",
      context_, references_);

    --references_;

    if (references_ == 0)
    {
      destroy_context ();
    }
  }

  madara_logger_ptr_log (logger::global_logger.get (), logger::LOG_MAJOR,
    "ZMQContext::rem_ref:" \
    " result: context=%p, references=%d\n",
    context_, references_);
}

void
madara::transport::ZMQContext::destroy_context (void)
{
  if (context_ != 0)
  {
    madara_logger_ptr_log (logger::global_logger.get (), logger::LOG_MAJOR,
      "ZMQContext::destroy_context:" \
      " destroying context.\n",
      context_, references_);

    zmq_ctx_destroy (context_);
    context_ = 0;
    references_ = 0;
  }
}

/**
* Destructor
**/
madara::transport::ZMQContext::~ZMQContext ()
{
  destroy_context ();
}