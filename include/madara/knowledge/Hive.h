#pragma once

#include "KnowledgeBase.h"
#include "madara/transport/SharedMemoryPush.h"
#include "madara/MadaraExport.h"

namespace madara
{

  namespace transport
  {
    /// forward declaration
    class SharedMemoryPush;
  }  // namespace transport

  namespace knowledge
  {
    /**
     * Knowledge bases linked with shared memory to a central hive concept
     **/
    class MADARA_EXPORT Hive
    {
    public:
      /**
       * Default constructor
       **/
      Hive();

      /**
       * Destructor
       **/
      ~Hive();

      /**
       * Clears the hive of knowledge values
       **/
      inline void clear(void)
      {
        for (auto kb : kbs_)
        {
          kb.clear();
        }
      }

      /**
       * Returns a reference to the underlying hive knowledge bases
       * @return linked knowledge bases
       **/
      inline std::vector<knowledge::KnowledgeBase>& get_kbs(void)
      {
        return kbs_;
      }

      /**
       * Resizes the hive to a certain number of shared knowledge bases
       * @param  new_size  the number of linked knowledge bases to create
       **/
      void resize(size_t new_size);

    private:
      /// Knowledge base
      std::vector<madara::knowledge::KnowledgeBase> kbs_;

      /// transports meant for fast memory transport within the controller
      std::vector<madara::transport::SharedMemoryPush*> transports_;
    };
  }  // namespace knowledge
}  // namespace madara