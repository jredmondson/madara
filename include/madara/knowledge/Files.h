#ifndef _MADARA_KNOWLEDGE_BASE_FILES_H_
#define _MADARA_KNOWLEDGE_BASE_FILES_H_

/**
 * @file Files.h
 * @author James Edmondson <jedmondson@gmail.com>
 *
 * This file contains the KnowledgeBase Files class
 **/

#include <string>
#include "ace/Mem_Map.h"
#include "madara/knowledge/ThreadSafeContext.h"
#include "madara/MADARA_export.h"

namespace madara
{
  namespace knowledge
  {
    /**
     * @class Files
     * @brief This class provides file repo capabilities to the knowledge base
     **/
    class Files
    {
    public:
      /**
       * Constructor
       * @param map                knowledge context
       **/
      Files (ThreadSafeContext & map);

      /**
       * Read a file into the knowledge base
       * @param filename           file to read
       * @param knowledge_key       key to store the file into
       **/
      MADARA_Export int read_file (const std::string & knowledge_key,
                     const std::string & filename);
      
      /**
       * Read a file into an ACE_Mem_Map
       * @param filename           file to read
       * @param mem_map             memory map of file
       * @return                    
       **/
      MADARA_Export int read_file (const char * filename, ACE_Mem_Map & mem_map);
      
      /**
       * Write a file from the knowledge base to a specified location
       * @param filename           file to write to
       * @param knowledge_key       key to read the file from
       **/
      MADARA_Export ssize_t write_file (const std::string & knowledge_key,
                      const std::string & filename);

      /**
       * Write a memory buffer to file
       * @param filename           file to write to
       * @param buffer              pointer to buffer
       * @param size                size of the buffer
       * @return                    amount written to file (-1 if error)
       **/
      MADARA_Export ssize_t write_file (const char * filename, 
                     void *& buffer, size_t & size);
      
      
      /**
       * Read a policy into the knowledge base
       * @param policy_key          key to read the policy into
       * @param policy_file         file to read the policy from
       **/
      MADARA_Export int read_policy (const std::string & policy_key, 
                       const std::string & policy_file);

      /**
       * Shape a file according to a shaping policy
       * @param source_key          file that needs to be shaped
       * @param target_key          target key to save the shaped file into
       * @param policy_key          policy to use for shaping the file
       **/
      MADARA_Export int shape_file (const std::string & source_key, 
                      const std::string & target_key, 
                      const std::string & policy_key);

    private:
      // folder to store files into
      const static std::string files_folder_;

      // folder to store policies into
      const static std::string policies_folder_;

      // handle to the knowledge base
      ThreadSafeContext & map_;
    };
  }
}

#endif
