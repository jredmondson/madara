#ifndef _MADARA_KNOWLEDGE_BASE_INL_
#define _MADARA_KNOWLEDGE_BASE_INL_

#include <fstream>

#include "ace/streams.h"

/**
 * @file KnowledgeBase.inl
 * @author James Edmondson <jedmondson@gmail.com>
 *
 * This file contains the inline functions for the KnowledgeBase class
 */

inline int
madara::knowledge::KnowledgeBase::modify (const EvalSettings & settings)
{
  return apply_modified (settings);
}


#endif   // _MADARA_KNOWLEDGE_BASE_INL_
