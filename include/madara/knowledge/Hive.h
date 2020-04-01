#pragma once
/**
 * Copyright(c) 2020 Galois. All Rights Reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following acknowledgments and disclaimers.
 * 
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 
 * 3. The names "Galois," "Carnegie Mellon University," "SEI" and/or "Software
 *    Engineering Institute" shall not be used to endorse or promote products
 *    derived from this software without prior written permission. For written
 *    permission, please contact permission@sei.cmu.edu.
 * 
 * 4. Products derived from this software may not be called "SEI" nor may "SEI"
 *    appear in their names without prior written permission of
 *    permission@sei.cmu.edu.
 * 
 * 5. Redistributions of any form whatsoever must retain the following
 *    acknowledgment:
 * 
 *      This material is based upon work funded and supported by the Department
 *      of Defense under Contract No. FA8721-05-C-0003 with Carnegie Mellon
 *      University for the operation of the Software Engineering Institute, a
 *      federally funded research and development center. Any opinions,
 *      findings and conclusions or recommendations expressed in this material
 *      are those of the author(s) and do not necessarily reflect the views of
 *      the United States Department of Defense.
 * 
 *      NO WARRANTY. THIS CARNEGIE MELLON UNIVERSITY AND SOFTWARE ENGINEERING
 *      INSTITUTE MATERIAL IS FURNISHED ON AN "AS-IS" BASIS. CARNEGIE MELLON
 *      UNIVERSITY MAKES NO WARRANTIES OF ANY KIND, EITHER EXPRESSED OR
 *      IMPLIED, AS TO ANY MATTER INCLUDING, BUT NOT LIMITED TO, WARRANTY OF
 *      FITNESS FOR PURPOSE OR MERCHANTABILITY, EXCLUSIVITY, OR RESULTS
 *      OBTAINED FROM USE OF THE MATERIAL. CARNEGIE MELLON UNIVERSITY DOES
 *      NOT MAKE ANY WARRANTY OF ANY KIND WITH RESPECT TO FREEDOM FROM PATENT,
 *      TRADEMARK, OR COPYRIGHT INFRINGEMENT.
 * 
 *      This material has been approved for public release and unlimited
 *      distribution.
 **/

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
       * Returns the size of the hive
       **/
      inline size_t get_size(void)
      {
        return kbs_.size();
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