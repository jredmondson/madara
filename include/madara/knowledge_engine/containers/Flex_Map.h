
#ifndef _MADARA_CONTAINERS_FLEX_MAP_H_
#define _MADARA_CONTAINERS_FLEX_MAP_H_

#include <vector>
#include <map>
#include <string>
#include "madara/Lock_Type.h"
#include "madara/knowledge_engine/Knowledge_Base.h"
#include "madara/knowledge_engine/Thread_Safe_Context.h"
#include "madara/knowledge_engine/Knowledge_Update_Settings.h"
#include "String.h"
#include "Integer.h"
#include "Double.h"
#include "Map.h"
#include "String_Vector.h"
#include "Double_Vector.h"
#include "Integer_Vector.h"
#include "Buffer_Vector.h"
#include "Native_Double_Vector.h"
#include "Native_Integer_Vector.h"

/**
 * @file Flex_Map.h
 * @author James Edmondson <jedmondson@gmail.com>
 *
 * This file contains a C++ object that manages interactions for a
 * flexible map of variables.
 **/

namespace Madara
{
  namespace Knowledge_Engine
  {
    namespace Containers
    {
      class Flex_Map;

      /**
       * @class Flex_Map
       * @brief This class stores a flexible map of strings and ints to KaRL
       * variables
       * Flex_Map differs from Map in three distinct ways. First, it can use
       * either integers (for easy multi-dimensional arrays) or strings like
       * Map. Second, it allows for the usage of the [] operator to modify
       * values. This can causes a small performance penalty due to the
       * creation of Flex_Maps with each [] operator, but is much more
       * intuitive and an increase in quality-of-life. Third, because of its
       * micromanagement of the variable names, it is slower than Map in
       * almost all operations on variables within the map.
       */
      class MADARA_Export Flex_Map
      {
      public:
        /**
         * Default constructor
         * @param  settings  settings to apply by default
         * @param  delimiter  the delimiter for variables in the map
         **/
        Flex_Map (const Knowledge_Update_Settings & settings =
            Knowledge_Update_Settings (),
          const std::string & delimiter = ".");
      
        /**
         * Constructor
         * @param  name      the name of the map within the variable context
         * @param  knowledge the variable context
         * @param  settings  settings to apply by default
         * @param  delimiter  the delimiter for variables in the map
         **/
        Flex_Map (const std::string & name,
                Knowledge_Base & knowledge,
                const Knowledge_Update_Settings & settings =
                  Knowledge_Update_Settings (),
                const std::string & delimiter = ".");
      
        /**
         * Constructor
         * @param  name      the name of the map within the variable context
         * @param  knowledge the variable context
         * @param  settings  settings to apply by default
         * @param  delimiter  the delimiter for variables in the map
         **/
        Flex_Map (const std::string & name,
          Variables & knowledge,
          const Knowledge_Update_Settings & settings =
            Knowledge_Update_Settings (),
          const std::string & delimiter = ".");
      
        /**
         * Copy constructor
         **/
        Flex_Map (const Flex_Map & rhs);

        /**
         * Destructor
         **/
        ~Flex_Map ();
        
        /**
         * Mark the flex map as modified. The map retains the same values
         * but will resend values as if they had been modified.
         **/
        void modify (void);
        
        /**
         * Assignment operator
         * @param  rhs    value to copy
         **/
        void operator= (const Flex_Map & rhs);

        /**
         * Exchanges the map at this location with the map at another
         * location
         * @param  other         the other integer to exchange with
         * @param  refresh_keys  force a refresh of the keys in both maps to
         *                       ensure all keys are swapped
         * @param  delete_keys   delete any unused keys
         **/
        void exchange (Flex_Map & other,
          bool refresh_keys = true, bool delete_keys = true);

        /**
         * Retrieves a flex map at the keyed location
         * @param  key  the name of the variable entry
         * @return a flex map of the location in the map
         **/
        Flex_Map operator[] (const std::string & key);

        /**
        * Retrieves a flex map at the indexed location
        * @param  index  the index of the variable entry
        * @return a flex map of the location in the map
        **/
        Flex_Map operator[] (size_t index);

        /**
        * Retrieves a copy of the record from the current location
        * @return the value of the entry. Modifications to this will
        *         not be reflected in the context. This is a local copy.
        **/
        Knowledge_Record to_record (void) const;

        /**
        * Fills a Buffer_Vector container with all subkeys
        * @param  target  container to be filled
        **/
        void to_container (Buffer_Vector & target) const;

        /**
        * Fills a Double_Vector container with all subkeys
        * @param  target  container to be filled
        **/
        void to_container (Double_Vector & target) const;

        /**
        * Fills a Integer_Vector container with all subkeys
        * @param  target  container to be filled
        **/
        void to_container (Integer_Vector & target) const;

        /**
        * Fills a Native_Double_Vector container with all subkeys
        * @param  target  container to be filled
        **/
        void to_container (Native_Double_Vector & target) const;

        /**
        * Fills a Native_Integer_Vector container with all subkeys
        * @param  target  container to be filled
        **/
        void to_container (Native_Integer_Vector & target) const;

        /**
        * Fills a String_Vector container with all subkeys
        * @param  target  container to be filled
        **/
        void to_container (String_Vector & target) const;

        /**
        * Fills a Double container with the current location
        * @param  target  container to be filled
        **/
        void to_container (Double & target) const;

        /**
        * Fills an Integer container with the current location
        * @param  target  container to be filled
        **/
        void to_container (Integer & target) const;

        /**
        * Fills a String container with the current location
        * @param  target  container to be filled
        **/
        void to_container (String & target) const;

        /**
         * Fills a Map container with all subkeys
         * @param  target  container to be filled
         **/
        void to_container (Map & target) const;

        /**
        * Returns the value at the location as an integer
        * @return  the value at the location
        **/
        Knowledge_Record::Integer to_integer (void) const;

        /**
        * Returns the value at the location as a double
        * @return  the value at the location
        **/
        double to_double (void) const;

        /**
        * Returns the value at the location as a string
        * @return  the value at the location
        **/
        std::string to_string (void) const;

        /**
         * Returns the size of the map
         * @param first_level_keys_only  if true, only use first level
         *                               keys
         * @return size of the map
         **/
        size_t size (bool first_level_keys_only = true) const;
      
        /**
         * Returns the keys within the map. This can be an O(n)
         * operation that checks for any key that has name{delimiter}key.
         * @param curkeys the results of the operation
         * @param first_level_keys_only  if true, only generate first level
         *                               keys
         **/
        void keys (std::vector <std::string> & curkeys,
          bool first_level_keys_only = true) const;

        /**
        * Checks if a key is in the map. By default, this will check for an
        * exact match (i.e., the exact string matches to an existing record).
        * If you want to just check if the key exists in the next level,
        * set first_level_key to true.
        * @param  key  the map key to check for
        * @param  first_level_key    if true, checks for existence of key in
        *                            the next level of map hierarchy. This is
        *                            a more expensive version of exists.
        *                            O (log N) vs. O (N)
        **/
        bool exists (
          const std::string & key, bool first_level_key = false) const;

        /**
        * Checks for the existence of the current location in the context
        * @return true if location exists in map. False otherwise.
        **/
        bool exists (void) const;

        /**
         * Returns the name of the map
         * @return name of the map
         **/
        std::string get_name (void) const;
        
        /**
         * Sets the variable name that this refers to
         * @param var_name  the name of the variable in the knowledge base
         * @param knowledge  the knowledge base the variable is housed in
         **/
        void set_name (const std::string & var_name,
          Knowledge_Base & knowledge);
        
        /**
         * Sets the variable name that this refers to
         * @param var_name  the name of the variable in the knowledge base
         * @param knowledge  the knowledge base the variable is housed in
         **/
        void set_name (const std::string & var_name,
          Variables & knowledge);

        /**
        * Sets the delimiter for adding and detecting subvariables. By default,
        * MADARA uses a '.' delimiter for names like "com.madara.Var1". Other
        * conventions can include '/' for ROS-like topic directory structures,
        * e.g., com/madara/Var1.
        * @param delimiter  the delimiter to use for variable demarcation
        **/
        void set_delimiter (const std::string & delimiter);

        /**
        * Gets the delimiter for adding and detecting subvariables. By default,
        * MADARA uses a '.' delimiter for names like "com.madara.Var1". Other
        * conventions can include '/' for ROS-like topic directory structures,
        * e.g., com/madara/Var1.
        * @return   the delimiter to use for variable demarcation
        **/
        std::string get_delimiter (void);

        /**
         * Clears the map. This deletes all keys in the Flex Map from the context.
         **/
        void clear (void);

        /**
        * Erases a variable from the map
        * @param  key  the variable to delete from the map
        * @param  delete_subkeys  if true, delete all keys with the key prefix
        **/
        void erase (const std::string & key, bool delete_subkeys = true);

        /**
        * Checks if the value in the record is not false (0)
        * @return  true if value is not false
        **/
        bool is_true (void);

        /**
        * Checks if the value in the record is false (0)
        * @return  true if value is false
        **/
        bool is_false (void);

        /**
         * Read a file into a location in the map
         * @param filename     file to read
         */
        int read_file (const std::string & filename);
      
        /**
         * Read a file into a location in the map
         * @param filename     file to read
         * @param settings     settings to use when evaluating/updating
         */
        int read_file (
          const std::string & filename, 
          const Knowledge_Update_Settings & settings);
      
        /**
         * Sets the location within the map to the specified value
         *
         * @param value           value to set at location
         * @return                0 if successful, -1 if key is null, and
         *                        -2 if quality isn't high enough
         **/
        int set (
          Knowledge_Record::Integer value = Knowledge_Record::MODIFIED);

        /**
        * Sets the location to the specified Integer
        *
        * @param value           value to set at location
        **/
        void operator= (
          Knowledge_Record::Integer value);

        /**
        * Sets the location to the specified int
        *
        * @param value           value to set at location
        **/
        void operator= (int value);

        /**
        * Sets the location to the specified double
        *
        * @param value           value to set at location
        **/
        void operator= (
          double value);

        /**
        * Sets the location to the specified STL string
        *
        * @param value           value to set at location
        **/
        void operator= (
          const std::string value);

        /**
        * Sets the location to native integer vector of the provided values
        *
        * @param value           array of integers to set at the location
        * @return                0 if successful, -1 if key is null, and
        *                        -2 if quality isn't high enough
        **/
        void operator= (const std::vector <Knowledge_Record::Integer> & value);

        /**
        * Sets the location to native double vector of the provided values
        *
        * @param value           array of doubles to set at the location
        * @return                0 if successful, -1 if key is null, and
        *                        -2 if quality isn't high enough
        **/
        void operator= (const std::vector <double> & value);

        /**
         * Sets the location within the map to the specified value
         *
         * @param value           value to set at location
         * @param settings        settings for applying the update
         * @return                0 if successful, -1 if key is null, and
         *                        -2 if quality isn't high enough
         **/
        int set (Knowledge_Record::Integer value, 
          const Knowledge_Update_Settings & settings);
        
        /**
         * Sets an index within an array to a specified value
         *
         * @param index           index of the location in the array
         * @param value           value to set at location
         * @return                0 if successful, -1 if key is null, and
         *                        -2 if quality isn't high enough
         **/
        int set_index (size_t index,
          Knowledge_Record::Integer value);

        /**
         * Sets an index within an array to a specified value
         *
         * @param index           index of the location in the array
         * @param value           value to set at location
         * @param settings        settings for applying the update
         * @return                0 if successful, -1 if key is null, and
         *                        -2 if quality isn't high enough
         **/
        int set_index (size_t index,
          Knowledge_Record::Integer value,
          const Knowledge_Update_Settings & settings);
        
        /**
         * Sets the location within the map to the specified value
         *
         * @param value           array of integers to set at the location
         * @param size            number of elements in the array
         * @return                0 if successful, -1 if key is null, and
         *                        -2 if quality isn't high enough
         **/
        int set (const Knowledge_Record::Integer * value,
          uint32_t size);
       
        /**
         * Sets the location within the map to the specified value
         *
         * @param value           array of integers to set at the location
         * @param size            number of elements in the array
         * @param settings        settings for applying the update
         * @return                0 if successful, -1 if key is null, and
         *                        -2 if quality isn't high enough
         **/
        int set (const Knowledge_Record::Integer * value,
          uint32_t size,
          const Knowledge_Update_Settings & settings);
       
        /**
         * Sets the location within the map to the specified value
         *
         * @param value           array of integers to set at the location
         * @return                0 if successful, -1 if key is null, and
         *                        -2 if quality isn't high enough
         **/
        int set (const std::vector <Knowledge_Record::Integer> & value);
       
        /**
         * Sets the location within the map to the specified value
         *
         * @param value           array of integers to set at the location
         * @param settings        settings for applying the update
         * @return                0 if successful, -1 if key is null, and
         *                        -2 if quality isn't high enough
         **/
        int set (const std::vector <Knowledge_Record::Integer> & value,
          const Knowledge_Update_Settings & settings);
       
        /**
         * Sets the location within the map to the specified value
         *
         * @param value           value to set at location
         * @return                0 if successful, -1 if key is null, and
         *                        -2 if quality isn't high enough
         **/
        int set (double value);

        /**
         * Sets the location within the map to the specified value
         *
         * @param value           value to set at location
         * @param settings        settings for applying the update
         * @return                0 if successful, -1 if key is null, and
         *                        -2 if quality isn't high enough
         **/
        int set (double value, 
          const Knowledge_Update_Settings & settings);
        
        /**
         * Sets an index within a map to a specified value
         *
         * @param index           index of the location in the array
         * @param value           value to set at location
         * @return                0 if successful, -1 if key is null, and
         *                        -2 if quality isn't high enough
         **/
        int set_index (size_t index, double value);

        /**
         * Sets an index within a map to a specified value
         *
         * @param index           index of the location in the array
         * @param value           value to set at location
         * @param settings        settings for applying the update
         * @return                0 if successful, -1 if key is null, and
         *                        -2 if quality isn't high enough
         **/
        int set_index (size_t index, double value,
          const Knowledge_Update_Settings & settings);
        
        /**
         * Sets the location within the map to the specified value
         *
         * @param value           array of doubles to set at the location
         * @param size            number of elements in the array
         * @return                0 if successful, -1 if key is null, and
         *                        -2 if quality isn't high enough
         **/
        int set (const double * value,
          uint32_t size);
       
        /**
         * Sets the location within the map to the specified value
         *
         * @param value           array of doubles to set at the location
         * @param size            number of elements in the array
         * @param settings        settings for applying the update
         * @return                0 if successful, -1 if key is null, and
         *                        -2 if quality isn't high enough
         **/
        int set (const double * value,
          uint32_t size,
          const Knowledge_Update_Settings & settings);
       
        /**
         * Sets the location within the map to the specified value
         *
         * @param value           array of doubles to set at the location
         * @return                0 if successful, -1 if key is null, and
         *                        -2 if quality isn't high enough
         **/
        int set (const std::vector <double> & value);
        
        /**
         * Sets the location within the map to the specified value
         *
         * @param value           array of doubles to set at the location
         * @param settings        settings for applying the update
         * @return                0 if successful, -1 if key is null, and
         *                        -2 if quality isn't high enough
         **/
        int set (const std::vector <double> & value,
          const Knowledge_Update_Settings & settings);
        
        /**
         * Sets the location within the map to the specified value
         *
         * @param value           value to set at location
         * @return                0 if successful, -1 if key is null, and
         *                        -2 if quality isn't high enough
         **/
        int set (const std::string & value);

        /**
         * Sets the location within the map to the specified value
         *
         * @param value           value to set at location
         * @param settings        settings for applying the update
         * @return                0 if successful, -1 if key is null, and
         *                        -2 if quality isn't high enough
         **/
        int set (const std::string & value, 
          const Knowledge_Update_Settings & settings);
        
        /**
         * Atomically sets the value of an index to an arbitrary string.
         * @param   value     new value of the variable
         * @param   size      indicates the size of the value buffer
         * @return   0 if the value was set. -1 if null key
         **/
        int set_file (const unsigned char * value, size_t size);
      
        /**
         * Atomically sets the value of an index to an arbitrary string.
         * @param   value     new value of the variable
         * @param   size      indicates the size of the value buffer
         * @param   settings  settings for applying the update
         * @return   0 if the value was set. -1 if null key
         **/
        int set_file (const unsigned char * value, size_t size, 
          const Knowledge_Update_Settings & settings);
      
        /**
         * Atomically sets the value of an index to a JPEG image
         * @param   value     new value of the variable
         * @param   size      indicates the size of the value buffer
         * @return   0 if the value was set. -1 if null key
         **/
        int set_jpeg (const unsigned char * value, size_t size);
      
        /**
         * Atomically sets the value of an index to a JPEG image
         * @param   value     new value of the variable
         * @param   size      indicates the size of the value buffer
         * @param   settings  settings for applying the update
         * @return   0 if the value was set. -1 if null key
         **/
        int set_jpeg (const unsigned char * value, size_t size, 
          const Knowledge_Update_Settings & settings);
      
        /**
         * Sets the quality of writing to a certain variable from this entity
         *
         * @param quality         quality of writing to this location
         * @param settings        settings for referring to knowledge variables
         **/
        void set_quality (uint32_t quality,
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

        /// Updates the variable reference if necessary
        void update_variable (void) const;

        /// guard for access and changes
        typedef ACE_Guard<MADARA_LOCK_TYPE> Guard;
      
        /// internal map of variable references
        typedef std::map <std::string, Variable_Reference>  Internal_Flex_Map;

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
         * The current location variable
         **/
        mutable Variable_Reference variable_;

        /**
         * Settings for modifications
         **/
        Knowledge_Update_Settings settings_;

        /**
        * Delimiter for the prefix to subvars
        **/
        std::string delimiter_;
      };
    }
  }
}




#endif // _MADARA_CONTAINERS_FLEX_MAP_H_