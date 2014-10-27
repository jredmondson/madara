
#ifndef _MADARA_CONTAINERS_RELIABLE_RECORD_H_
#define _MADARA_CONTAINERS_RELIABLE_RECORD_H_

#include <vector>
#include <string>
#include "madara/Lock_Type.h"
#include "madara/knowledge_engine/containers/Integer_Vector.h"
#include "madara/knowledge_engine/containers/Buffer_Vector.h"
#include "madara/knowledge_engine/Knowledge_Base.h"
#include "madara/knowledge_engine/Thread_Safe_Context.h"
#include "madara/knowledge_engine/Knowledge_Update_Settings.h"

/**
 * @file Reliable_Record.h
 * @author James Edmondson <jedmondson@gmail.com>
 *
 * This file contains a C++ object that manages acknowledging
 * the receipt of a record
 **/

namespace Madara
{
  namespace Knowledge_Engine
  {
    namespace Containers
    {
      /**
       * @class Reliable_Record
       * @brief This class manages acknowledging the receipt of a record
       */
      class MADARA_Export Reliable_Record
      {
      public:
        /// trait that describes the value type
        typedef std::string  type;
        
        /**
         * Default constructor
         **/
        Reliable_Record (const Knowledge_Update_Settings & settings =
          Knowledge_Update_Settings ());
      
        /**
         * Default constructor
         * @param  name       name of the variable in the knowledge base
         * @param  knowledge  the knowledge base that will contain the vector
         * @param  settings   settings for evaluating the vector
         **/
        Reliable_Record (const std::string & name,
                Knowledge_Base & knowledge,
                const Knowledge_Update_Settings & settings =
                  Knowledge_Update_Settings ());
      
        /**
         * Copy constructor
         **/
        Reliable_Record (const Reliable_Record & rhs);

        /**
         * Destructor
         **/
        ~Reliable_Record ();
        
        /**
         * Assignment operator
         * @param  rhs    value to copy
         **/
        void operator= (const Reliable_Record & rhs);

        /**
         * Gets the id of this process, for the purposes of acks
         * @return the id of this process
         **/
        size_t get_id (void);
        
        /**
         * Gets the number of processes participating in the ack
         * @return the id of this process
         **/
        size_t get_processes (void);

        /**
         * Returns true if all acks have been received
         * @return true if all acks have been received
         **/
        bool is_done (void) const;
        
        /**
         * Sends out next unacknowledged fragment of record
         **/
        void next (void);

        /**
         * Returns the name of the source record
         * @return name of the record
         **/
        std::string get_name (void) const;
        
        /**
         * Sets the source variable name that this refers to. The source is
         * the record that will be fragmented and acknowledged by others
         * @param var_name  the source variable in the knowledge base
         * @param knowledge  the knowledge base the variable is housed in
         **/
        void set_name (const std::string & var_name,
          Knowledge_Base & knowledge);
        
        /**
         * Checks for equality
         * @param  value  the value to compare to
         * @return true if equal, false otherwise
         **/
        bool operator== (const Reliable_Record & value) const;
        
        /**
         * Checks for inequality
         * @param  value  the value to compare to
         * @return true if inequal, false otherwise
         **/
        bool operator!= (const Reliable_Record & value) const;
        
        /**
         * Sets the update settings for the variable
         * @param  settings  the new settings to use
         * @return the old update settings
         **/
        Knowledge_Update_Settings set_settings (
          const Knowledge_Update_Settings & settings);

        /**
         * Sets the quality of writing to the variable
         *
         * @param quality         quality of writing to this location
         * @param settings        settings for referring to knowledge variables
         **/
        void set_quality (uint32_t quality,
               const Knowledge_Reference_Settings & settings =
                       Knowledge_Reference_Settings (false));
      
        /**
         * Syncs with the source
         **/
        void sync (void);
        
        /**
         * Merges fragments into the source location
         * @return  true if the merge was successful, false otherwise
         **/
        bool merge (void);
        
        /**
         * Resizes the number of ack participants in the reliability ring,
         * and the current participant's id
         * @param id        the id of this process in the reliability ring
         * @param participants the number of participants in reliability ring
         **/
        void resize (size_t id = 0, size_t participants = 1);

      private:

        /// guard for access and changes
        typedef ACE_Guard<MADARA_LOCK_TYPE> Guard;
      
        /**
         * Mutex for local changes
         **/
        mutable MADARA_LOCK_TYPE mutex_;

        /**
         * Variable context that we are modifying
         **/
        mutable Thread_Safe_Context * context_;

        /**
         * Prefix of variable
         **/
        std::string name_;
        
        /**
         * id of this process
         **/
        size_t id_;

        /**
         * number of processes responding with acks
         **/
        size_t processes_;

        /**
         * current fragment waiting for acknowledgement 
         **/
        mutable size_t current_fragment_;
        
        /**
         * current participant without acknowledgement
         **/
        mutable size_t current_ack_;

        /**
         * the source variable to split into fragments
         **/
        Variable_Reference source_;

        /**
         * Variable reference
         **/
        Buffer_Array fragments_;

        /**
         * Variable reference
         **/
        std::vector <Integer_Array> acks_;

        /**
         * Settings for modifications
         **/
        Knowledge_Update_Settings settings_;
      };
    }
  }
}

#endif // _MADARA_CONTAINERS_RELIABLE_RECORD_H_