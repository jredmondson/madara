
#ifndef _MADARA_CONTAINERS_MAP_H_
#define _MADARA_CONTAINERS_MAP_H_

#include <vector>
#include <map>
#include <string>
#include "madara/LockType.h"
#include "madara/knowledge/KnowledgeBase.h"
#include "madara/knowledge/ThreadSafeContext.h"
#include "madara/knowledge/KnowledgeUpdateSettings.h"
#include "BaseContainer.h"

/**
 * @file Map.h
 * @author James Edmondson <jedmondson@gmail.com>
 *
 * This file contains a C++ object that manages interactions for a
 * map of variables
 **/

namespace madara
{
  namespace knowledge
  {
    namespace containers
    {
      /**
       * @class Map
       * @brief This class stores a map of strings to KaRL variables
       */
      class MADARA_EXPORT Map : public BaseContainer
      {
      public:
        /**
         * Default constructor
         * @param  settings  settings to apply by default
         * @param  delimiter  the delimiter for variables in the map
         **/
        Map (const KnowledgeUpdateSettings & settings =
            KnowledgeUpdateSettings (),
          const std::string & delimiter = ".");
      
        /**
         * Constructor
         * @param  name      the name of the map within the variable context
         * @param  knowledge the variable context
         * @param  settings  settings to apply by default
         * @param  delimiter  the delimiter for variables in the map
         **/
        Map (const std::string & name,
          KnowledgeBase & knowledge,
          const KnowledgeUpdateSettings & settings =
            KnowledgeUpdateSettings (),
          const std::string & delimiter = ".");
      
        /**
         * Constructor
         * @param  name      the name of the map within the variable context
         * @param  knowledge the variable context
         * @param  settings  settings to apply by default
         * @param  delimiter  the delimiter for variables in the map
         **/
        Map (const std::string & name,
          Variables & knowledge,
          const KnowledgeUpdateSettings & settings =
          KnowledgeUpdateSettings (),
          const std::string & delimiter = ".");
      
        /**
         * Copy constructor
         **/
        Map (const Map & rhs);

        /**
         * Destructor
         **/
        virtual ~Map ();
        
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
        knowledge::KnowledgeRecord operator[] (const std::string & key);
      
        /**
         * Retrieves a copy of the record from the map. Same functionality
         * as operator[]. This is provided for convenience to match other
         * containers way of converting to a record.
         * @param  key  the name of the variable entry
         * @return the value of the entry. Modifications to this will
         *         not be reflected in the context. This is a local copy.
         **/
        knowledge::KnowledgeRecord to_record (const std::string & key);
      
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
        * Checks for the existence of a prefix in the keys
        * @param  prefix   the first letters in a key
        * @return true if prefix exists in map. False otherwise.
        **/
        bool has_prefix (const std::string & prefix) const;

        /**
         * Sets the variable name that this refers to
         * @param var_name   the name of the variable in the knowledge base
         * @param knowledge  the knowledge base the variable is housed in
         * @param sync       sync the keys to existing vars in the new name
         **/
        void set_name (const std::string & var_name,
          KnowledgeBase & knowledge, bool sync = true);
        
        /**
         * Sets the variable name that this refers to
         * @param var_name  the name of the variable in the knowledge base
         * @param knowledge the knowledge base the variable is housed in
         * @param sync      sync the keys to existing vars in the new name
         **/
        void set_name (const std::string & var_name,
          Variables & knowledge, bool sync = true);

        /**
        * Sets the delimiter for adding and detecting subvariables. By default,
        * MADARA uses a '.' delimiter for names like "ai.madara.Var1". Other
        * conventions can include '/' for ROS-like topic directory structures,
        * e.g., ai/madara/Var1.
        * @param delimiter  the delimiter to use for variable demarcation
        * @param sync       sync the keys to existing vars in the new
        *                   delimiter
        **/
        void set_delimiter (const std::string & delimiter,
          bool sync = true);

        /**
        * Gets the delimiter for adding and detecting subvariables. By default,
        * MADARA uses a '.' delimiter for names like "ai.madara.Var1". Other
        * conventions can include '/' for ROS-like topic directory structures,
        * e.g., ai/madara/Var1.
        * @return   the delimiter to use for variable demarcation
        **/
        std::string get_delimiter (void);

        /**
         * Clears the map
         * @param  clear_knowledge  if true, clear the underlying knowledge
         *                          base as well as the keys in the local map
         **/
        void clear (bool clear_knowledge = true);

        /**
        * Erases a variable from the map
        * @param  key  the variable to delete from the map
        **/
        void erase (const std::string & key);

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
          const KnowledgeUpdateSettings & settings);
      
        /**
         * Sets a location within the map to the specified value
         *
         * @param key             location within the map
         * @param value           value to set at location
         * @return                0 if successful, -1 if key is null, and
         *                        -2 if quality isn't high enough
         **/
        int set (const std::string & key,
          madara::knowledge::KnowledgeRecord::Integer value = 
            madara::knowledge::KnowledgeRecord::MODIFIED);

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
          madara::knowledge::KnowledgeRecord::Integer value, 
          const KnowledgeUpdateSettings & settings);
        
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
          madara::knowledge::KnowledgeRecord::Integer value);

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
          madara::knowledge::KnowledgeRecord::Integer value,
          const KnowledgeUpdateSettings & settings);
        
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
          const madara::knowledge::KnowledgeRecord::Integer * value,
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
          const madara::knowledge::KnowledgeRecord::Integer * value,
          uint32_t size,
          const KnowledgeUpdateSettings & settings);
       
        /**
         * Sets a location within the map to the specified value
         *
         * @param key             location within the map
         * @param value           array of integers to set at the location
         * @return                0 if successful, -1 if key is null, and
         *                        -2 if quality isn't high enough
         **/
        int set (const std::string & key,
          const std::vector <KnowledgeRecord::Integer> & value);
       
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
          const std::vector <KnowledgeRecord::Integer> & value,
          const KnowledgeUpdateSettings & settings);
       
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
          const KnowledgeUpdateSettings & settings);
        
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
          const KnowledgeUpdateSettings & settings);
        
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
          const KnowledgeUpdateSettings & settings);
       
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
          const KnowledgeUpdateSettings & settings);
        
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
          const KnowledgeUpdateSettings & settings);
        
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
          const KnowledgeUpdateSettings & settings);
      
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
          const KnowledgeUpdateSettings & settings);
      
        /**
         * Sets the quality of writing to a certain variable from this entity
         *
         * @param key             location within the map
         * @param quality         quality of writing to this location
         * @param settings        settings for referring to knowledge variables
         **/
        void set_quality (const std::string & key, uint32_t quality,
               const KnowledgeReferenceSettings & settings =
                       KnowledgeReferenceSettings (false));

        /**
        * Returns the type of the container along with name and any other
        * useful information. The provided information should be useful
        * for developers wishing to debug container operations, especially
        * as it pertains to pending network operations (i.e., when used
        * in conjunction with modify)
        *
        * @return info in format {container}: {name}{ = value, if appropriate}
        **/
        std::string get_debug_info (void);

        /**
        * Clones this container
        * @return  a deep copy of the container that must be managed
        *          by the user (i.e., you have to delete the return value)
        **/
        virtual BaseContainer * clone (void) const;

        /**
        * Determines if all values in the map are true
        * @return true if all values are true
        **/
        bool is_true (void) const;

        /**
        * Determines if the value of the map is false
        * @return true if at least one value is false
        **/
        bool is_false (void) const;

      private:

        /**
        * Polymorphic is true method which can be used to determine if
        * all values in the container are true
        **/
        virtual bool is_true_ (void) const;

        /**
        * Polymorphic is false method which can be used to determine if
        * at least one value in the container is false
        **/
        virtual bool is_false_ (void) const;

        /**
        * Polymorphic modify method used by collection containers. This
        * method calls the modify method for this class. We separate the
        * faster version (modify) from this version (modify_) to allow
        * users the opportunity to have a fastery version that does not
        * use polymorphic functions (generally virtual functions are half
        * as efficient as normal function calls)
        **/
        virtual void modify_ (void);

        /**
        * Returns the type of the container along with name and any other
        * useful information. The provided information should be useful
        * for developers wishing to debug container operations, especially
        * as it pertains to pending network operations (i.e., when used
        * in conjunction with modify)
        *
        * @return info in format {container}: {name}{ = value, if appropriate}
        **/
        virtual std::string get_debug_info_ (void);

        /// internal map of variable references
        typedef std::map <std::string, VariableReference>  InternalMap;

        /**
         * Variable context that we are modifying
         **/
        ThreadSafeContext * context_;

        /**
         * Map of variables to values
         **/
        InternalMap map_;

        /**
         * Delimiter for the prefix to subvars
         **/
        std::string delimiter_;
      };
    }
  }
}




#endif // _MADARA_CONTAINERS_MAP_H_