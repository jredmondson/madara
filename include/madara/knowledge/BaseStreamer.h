#ifndef MADARA_KNOWLEDGE_BASE_STREAMER_H_
#define MADARA_KNOWLEDGE_BASE_STREAMER_H_

#include "madara/MadaraExport.h"

/**
 * @file BaseStreamer.h
 * @author David Kyle <david.kyle@shield.ai>
 *
 * This file contains the BaseStream base interface class for knowledge
 * streamers.
 **/

namespace madara { namespace knowledge {

/**
 * Interface for knowledge update streaming. Objects which implement this
 * interface can be attachecd to a KnowledgeBase with the attach_streamer
 * method to receive copies of every modification of knowledge.
 **/
class MADARA_EXPORT BaseStreamer
{
public:
  virtual ~BaseStreamer() = default;

  /**
   * When attached to a KnowledgeBase, this will be called every time a
   * KnowledgeRecord held within is modified.
   *
   * @param name the variable name of the KnowledgeRecord
   * @param record a copy of the new value
   **/
  virtual void enqueue(std::string name, KnowledgeRecord record) = 0;
};

} } // namespace madara::knowledge

#endif // MADARA_KNOWLEDGE_BASE_STREAMER_H_
