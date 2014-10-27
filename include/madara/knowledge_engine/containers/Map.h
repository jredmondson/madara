
#ifndef _MADARA_MAP_H_
#define _MADARA_MAP_H_

#include <vector>
#include <map>
#include <string>
#include "madara/Lock_Type.h"
#include "madara/knowledge_engine/Knowledge_Base.h"
#include "madara/knowledge_engine/Thread_Safe_Context.h"
#include "madara/knowledge_engine/Knowledge_Update_Settings.h"

/**
 * @file Map.h
 * @author James Edmondson <jedmondson@gmail.com>
 *
 * This file contains a C++ object that manages interactions for a
 * map of variables
 **/

namespace Madara
{
  namespace Knowledge_Engine
  {
    namespace Containers
    {
      /**
       * @class Map
       * @brief This class stores a map of strings to KaRL variables
       */
      class MADARA_Export Map
      {
      public:
        /**
         * Default constructor
         **/
        Map (const Knowledge_Update_Settings & settings = Knowledge_Update_Settings ());
      
        /**
         * Constructor
         * @param  name      the name of the map within the variable context
         * @param  knowledge the variable context
         * @param  settings  settings to apply by default
         **/
        Map (const std::string & name,
                Knowledge_Base & knowledge,
                const Knowledge_Update_Settings & settings = Knowledge_Update_Settings ());
      
        /**
         * Constructor
         * @param  name      the name of the map within the variable context
         * @param  knowledge the variable context
         * @param  settings  settings to apply by default
         **/
        Map (const std::string & name,
                Variables & knowledge,
                const Knowledge_Update_Settings & settings = Knowledge_Update_Settings ());
      
        /**
         * Copy constructor
         **/
        Map (const Map & rhs);

        /**
         * Destructor
         **/
        ~Map ();
        
        /**
         * Mark the vector as modified. The map retains the same values
         * but will resend values as if they had been modified.
         **/
        void modify (void);

        /**
         * Mark the value as modified. The map element retains its value
         * but will resend its value as if it had been modified.
         * @param  index  the index to modify
         **/
        void modify (const std::string & index);
    
        
        /**
         * Assignment operator
         * @param  rhs    value to copy
         **/
        void operator= (const Map & rhs);

        /**
         * Exchanges the map at this location with the map at another
         * location.
         * @param  other         the other integer to exchange with
         * @param  refresh_keys  force a refresh of the keys in both maps to
         *                       ensure all keys are swapped
         * @param  delete_keys   delete any unused keys
         **/
        void exchange (Map & other,
          bool refresh_keys = true, bool delete_keys = true);

        /**
         * Retrieves a copy of the record from the map.
         * @param  key  the name of the variable entry
         * @return the value of the entry. Modifications to this will
         *         not be reflected in the context. This is a local copy.
         **/
        Knowledge_Record operator[] (const std::string & key);
      
        /**
         * Retrieves a copy of the record from the map. Same functionality
         * as operator[]. This is provided for convenience to match other
         * containers way of converting to a record.
         * @param  key  the name of the variable entry
         * @return the value of the entry. Modifications to this will
         *         not be reflected in the context. This is a local copy.
         **/
        Knowledge_Record to_record (const std::string & key);
      
        /**
         * Returns the size of the map
         * @return size of the map
         **/
        size_t size (void) const;
      
        /**
         * Syncs the keys from the knowledge base. This can be useful
         * if you expect other knowledge bases to add variables to the map.
         * @return a vector of the keys that were added during the sync
         **/
        std::vector <std::string> sync_keys (void);

        /**
         * Returns the keys within the map
         * @return keys used in the map
         **/
        void keys (std::vector <std::string> & curkeys) const;

        /**
         * Checks for the existence of a key
         * @param key          map key
         * @return true if key exists in map. False otherwise.
         **/
        bool exists (const std::string & key) const;

        /**
         * Returns the name of the map
         * @return name of the map
         **/
        std::string get_name (void) const;
        
        /**
         * Sets the variable name that this refers to
         * @param var_name  the name of the variable in the knowledge base
         * @param knowledge  the knowledge base the variable is housed in
         * @param sync_keys  sync the keys to existing vars in the new name
         **/
        void set_name (const std::string & var_name,
          Knowledge_Base & knowledge, bool sync_keys = true);
        
        /**
         * Sets the variable name that this refers to
         * @param var_name  the name of the variable in the knowledge base
         * @param knowledge  the knowledge base the variable is housed in
         * @param sync_keys  sync the keys to existing vars in the new name
         **/
        void set_name (const std::string & var_name,
          Variables & knowledge, bool sync_keys = true);

        /**
         * Clears the map. This does not change anything within the knowledge
         * base.
         **/
        void clear (void);
        
        /**
         * Read a file into a location in the map
         * @param filename     file to read
         * @param key          key to store the file into
         */
        int read_file (const std::string & key, 
                       const std::string & filename);
      
        /**
         * Read a file into a location in the map
         * @param filename     file to read
         * @param key          key to store the file into
         * @param settings     settings to use when evaluating/updating
         */
        int read_file (const std::string & key, 
                       const std::string & filename, 
          const Knowledge_Update_Settings & settings);
      
        /**
         * Sets a location within the map to the specified value
         *
         * @param key             location within the map
         * @param value           value to set at location
         * @return                0 if successful, -1 if key is null, and
         *                        -2 if quality isn't high enough
         **/
        int set (const std::string & key,
          Madara::Knowledge_Record::Integer value = 
            Madara::Knowledge_Record::MODIFIED);

        /**
         * Sets a location within the map to the specified value
         *
         * @param key             location within the map
         * @param value           value to set at location
         * @param settings        settings for applying the update
         * @return                0 if successful, -1 if key is null, and
         *                        -2 if quality isn't high enough
         **/
        int set (const std::string & key,
          Madara::Knowledge_Record::Integer value, 
          const Knowledge_Update_Settings & settings);
        
        /**
         * Sets an index within an array to a specified value
         *
         * @param key             location within the map
         * @param index           index of the location in the array
         * @param value           value to set at location
         * @return                0 if successful, -1 if key is null, and
         *                        -2 if quality isn't high enough
         **/
        int set_index (const std::string & key,
          size_t index,
          Madara::Knowledge_Record::Integer value);

        /**
         * Sets an index within an array to a specified value
         *
         * @param key             location within the map
         * @param index           index of the location in the array
         * @param value           value to set at location
         * @param settings        settings for applying the update
         * @return                0 if successful, -1 if key is null, and
         *                        -2 if quality isn't high enough
         **/
        int set_index (const std::string & key,
          size_t index,
          Madara::Knowledge_Record::Integer value,
          const Knowledge_Update_Settings & settings);
        
        /**
         * Sets a location within the map to the specified value
         *
         * @param key             location within the map
         * @param value           array of integers to set at the location
         * @param size            number of elements in the array
         * @return                0 if successful, -1 if key is null, and
         *                        -2 if quality isn't high enough
         **/
        int set (const std::string & key,
          const Madara::Knowledge_Record::Integer * value,
          uint32_t size);
       
        /**
         * Sets a location within the map to the specified value
         *
         * @param key             location within the map
         * @param value           array of integers to set at the location
         * @param size            number of elements in the array
         * @param settings        settings for applying the update
         * @return                0 if successful, -1 if key is null, and
         *                        -2 if quality isn't high enough
         **/
        int set (const std::string & key,
          const Madara::Knowledge_Record::Integer * value,
          uint32_t size,
          const Knowledge_Update_Settings & settings);
       
        /**
         * Sets a location within the map to the specified value
         *
         * @param key             location within the map
         * @param value           array of integers to set at the location
         * @return                0 if successful, -1 if key is null, and
         *                        -2 if quality isn't high enough
         **/
        int set (const std::string & key,
          const std::vector <Knowledge_Record::Integer> & value);
       
        /**
         * Sets a location within the map to the specified value
         *
         * @param key             location within the map
         * @param value           array of integers to set at the location
         * @param settings        settings for applying the update
         * @return                0 if successful, -1 if key is null, and
         *                        -2 if quality isn't high enough
         **/
        int set (const std::string & key,
          const std::vector <Knowledge_Record::Integer> & value,
          const Knowledge_Update_Settings & settings);
       
        /**
         * Sets a location within the map to the specified value
         *
         * @param key             location within the map
         * @param value           value to set at location
         * @return                0 if successful, -1 if key is null, and
         *                        -2 if quality isn't high enough
         **/
        int set (const std::string & key, double value);

        /**
         * Sets a location within the map to the specified value
         *
         * @param key             location within the map
         * @param value           value to set at location
         * @param settings        settings for applying the update
         * @return                0 if successful, -1 if key is null, and
         *                        -2 if quality isn't high enough
         **/
        int set (const std::string & key, double value, 
          const Knowledge_Update_Settings & settings);
        
        /**
         * Sets an index within a map to a specified value
         *
         * @param key             location within the map
         * @param index           index of the location in the array
         * @param value           value to set at location
         * @return                0 if successful, -1 if key is null, and
         *                        -2 if quality isn't high enough
         **/
        int set_index (const std::string & key,
          size_t index,
          double value);

        /**
         * Sets an index within a map to a specified value
         *
         * @param key             location within the map
         * @param index           index of the location in the array
         * @param value           value to set at location
         * @param settings        settings for applying the update
         * @return                0 if successful, -1 if key is null, and
         *                        -2 if quality isn't high enough
         **/
        int set_index (const std::string & key,
          size_t index,
          double value,
          const Knowledge_Update_Settings & settings);
        
        /**
         * Sets a location within the map to the specified value
         *
         * @param key             location within the map
         * @param value           array of doubles to set at the location
         * @param size            number of elements in the array
         * @return                0 if successful, -1 if key is null, and
         *                        -2 if quality isn't high enough
         **/
        int set (const std::string & key,
          const double * value,
          uint32_t size);
       
        /**
         * Sets a location within the map to the specified value
         *
         * @param key             location within the map
         * @param value           array of doubles to set at the location
         * @param size            number of elements in the array
         * @param settings        settings for applying the update
         * @return                0 if successful, -1 if key is null, and
         *                        -2 if quality isn't high enough
         **/
        int set (const std::string & key,
          const double * value,
          uint32_t size,
          const Knowledge_Update_Settings & settings);
       
        /**
         * Sets a location within the map to the specified value
         *
         * @param key             location within the map
         * @param value           array of doubles to set at the location
         * @return                0 if successful, -1 if key is null, and
         *                        -2 if quality isn't high enough
         **/
        int set (const std::string & key,
          const std::vector <double> & value);
        
        /**
         * Sets a location within the map to the specified value
         *
         * @param key             location within the map
         * @param value           array of doubles to set at the location
         * @param settings        settings for applying the update
         * @return                0 if successful, -1 if key is null, and
         *                        -2 if quality isn't high enough
         **/
        int set (const std::string & key,
          const std::vector <double> & value,
          const Knowledge_Update_Settings & settings);
        
        /**
         * Sets a location within the map to the specified value
         *
         * @param key             location within the map
         * @param value           value to set at location
         * @return                0 if successful, -1 if key is null, and
         *                        -2 if quality isn't high enough
         **/
        int set (const std::string & key, const std::string & value);

        /**
         * Sets a location within the map to the specified value
         *
         * @param key             location within the map
         * @param value           value to set at location
         * @param settings        settings for applying the update
         * @return                0 if successful, -1 if key is null, and
         *                        -2 if quality isn't high enough
         **/
        int set (const std::string & key, const std::string & value, 
          const Knowledge_Update_Settings & settings);
        
        /**
         * Atomically sets the value of an index to an arbitrary string.
         * @param   key       key to store the file into
         * @param   value     new value of the variable
         * @param   size      indicates the size of the value buffer
         * @return   0 if the value was set. -1 if null key
         **/
        int set_file (const std::string & key,
          const unsigned char * value, size_t size);
      
        /**
         * Atomically sets the value of an index to an arbitrary string.
         * @param   key       key to store the file into
         * @param   value     new value of the variable
         * @param   size      indicates the size of the value buffer
         * @param   settings  settings for applying the update
         * @return   0 if the value was set. -1 if null key
         **/
        int set_file (const std::string & key,
          const unsigned char * value, size_t size, 
          const Knowledge_Update_Settings & settings);
      
        /**
         * Atomically sets the value of an index to a JPEG image
         * @param   key       name of a variable
         * @param   value     new value of the variable
         * @param   size      indicates the size of the value buffer
         * @return   0 if the value was set. -1 if null key
         **/
        int set_jpeg (const std::string & key,
          const unsigned char * value, size_t size);
      
        /**
         * Atomically sets the value of an index to a JPEG image
         * @param   key       name of a variable
         * @param   value     new value of the variable
         * @param   size      indicates the size of the value buffer
         * @param   settings  settings for applying the update
         * @return   0 if the value was set. -1 if null key
         **/
        int set_jpeg (const std::string & key,
          const unsigned char * value, size_t size, 
          const Knowledge_Update_Settings & settings);
      
        /**
         * Sets the quality of writing to a certain variable from this entity
         *
         * @param key             location within the map
         * @param quality         quality of writing to this location
         * @param settings        settings for referring to knowledge variables
         **/
        void set_quality (const std::string & key, uint32_t quality,
               const Knowledge_Reference_Settings & settings =
                       Knowledge_Reference_Settings (false));
      
        /**
         * Sets the update settings for the variable
         * @param  settings  the new settings to use
         * @return the old update settings
         **/
        Knowledge_Update_Settings set_settings (
          const Knowledge_Update_Settings & settings);

      private:

        /// guard for access and changes
        typedef ACE_Guard<MADARA_LOCK_TYPE> Guard;
      
        /// internal map of variable references
        typedef std::map <std::string, Variable_Reference>  Internal_Map;

        /**
         * Mutex for local changes
         **/
        mutable MADARA_LOCK_TYPE mutex_;

        /**
         * Variable context that we are modifying
         **/
        Thread_Safe_Context * context_;

        /**
         * Prefix of variable
         **/
        std::string name_;

        /**
         * Map of variables to values
         **/
        Internal_Map map_;

        /**
         * Settings for modifications
         **/
        Knowledge_Update_Settings settings_;
      };
    }
  }
}




#endif // _MADARA_MAP_H_