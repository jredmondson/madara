#include "Context_Guard.h"

madara::knowledge::Context_Guard::Context_Guard (
  Knowledge_Base & knowledge)
  : context_ (knowledge.get_context ())
{
  context_.lock ();
}

madara::knowledge::Context_Guard::Context_Guard (
  Thread_Safe_Context & context)
  : context_ (context)
{
  context_.lock ();
}
      
madara::knowledge::Context_Guard::~Context_Guard ()
{
  
  context_.unlock ();
}
