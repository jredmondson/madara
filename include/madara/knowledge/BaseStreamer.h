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

class MADARA_EXPORT BaseStreamer
{
public:
  virtual ~BaseStreamer() = default;

  virtual void enqueue(std::string name, KnowledgeRecord record) = 0;
};

} } // namespace madara::knowledge

#endif // MADARA_KNOWLEDGE_BASE_STREAMER_H_
