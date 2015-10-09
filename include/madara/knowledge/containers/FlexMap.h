
#ifndef _MADARA_CONTAINERS_FLEX_MAP_H_
#define _MADARA_CONTAINERS_FLEX_MAP_H_

#include <vector>
#include <map>
#include <string>
#include "madara/LockType.h"
#include "madara/knowledge/KnowledgeBase.h"
#include "madara/knowledge/ThreadSafeContext.h"
#include "madara/knowledge/KnowledgeUpdateSettings.h"
#include "String.h"
#include "Integer.h"
#include "Double.h"
#include "Map.h"
#include "StringVector.h"
#include "DoubleVector.h"
#include "IntegerVector.h"
#include "BufferVector.h"
#include "NativeDoubleVector.h"
#include "NativeIntegerVector.h"
#include "BaseContainer.h"

/**
 * @file FlexMap.h
 * @author James Edmondson <jedmondson@gmail.com>
 *
 * This file contains a C++ object that manages interactions for a
 * flexible map of variables.
 **/

namespace madara
{
  namespace knowledge
  {
    namespace containers
    {
      class FlexMap;

      /**
       * @class FlexMap
       * @brief This class stores a flexible map of strings and ints to KaRL
       * variables
       * FlexMap differs from Map in three distinct ways. First, it can use
       * either integers (for easy multi-dimensional arrays) or strings like
       * Map. Second, it allows for the usage of the [] operator to modify
       * values. This can causes a small performance penalty due to the
       * creation of FlexMaps with each [] operator, but is much more
       * intuitive and an increase in quality-of-life. Third, because of its
       * micromanagement of the variable names, it is slower than Map in
       * almost all operations on variables within the map.
       */
      class MADARA_Export FlexMap : public BaseContainer
      {
      public:
        /**
         * Default constructor
         * @param  settings  settings to apply by default
         * @param  delimiter  the delimiter for variables in the map
         **/
        FlexMap (const KnowledgeUpdateSettings & settings =
            KnowledgeUpdateSettings (),
          const std::string & delimiter = ".");
      
        /**
         * Constructor
         * @param  name      the name of the map within the variable context
         * @param  knowledge the variable context
         * @param  settings  settings to apply by default
         * @param  delimiter  the delimiter for variables in the map
         **/
        FlexMap (const std::string & name,
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
        FlexMap (const std::string & name,
          Variables & knowledge,
          const KnowledgeUpdateSettings & settings =
            KnowledgeUpdateSettings (),
          const std::string & delimiter = ".");
      
        /**
         * Copy constructor
         **/
        FlexMap (const FlexMap & rhs);

        /**
         * Destructor
         **/
        ~FlexMap ();
        
        /**
         * Mark the flex map as modified. The map retains the same values
         * but will resend values as if they had been modified.
         **/
        void modify (void);
        
        /**
         * Assignment operator
         * @param  rhs    value to copy
         **/
        void operator= (const FlexMap & rhs);

        /**
         * Exchanges the map at this location with the map at another
         * location
         * @param  other         the other integer to exchange with
         * @param  refresh_keys  force a refresh of the keys in both maps to
         *                       ensure all keys are swapped
         * @param  delete_keys   delete any unused keys
         **/
        void exchange (FlexMap & other,
          bool refresh_keys = true, bool delete_keys = true);

        /**
         * Retrieves a flex map at the keyed location
         * @param  key  the name of the variable entry
         * @return a flex map of the location in the map
         **/
        FlexMap operator[] (const std::string & key);

        /**
        * Retrieves a flex map at the indexed location
        * @param  index  the index of the variable entry
        * @return a flex map of the location in the map
        **/
        FlexMap operator[] (size_t index);

        /**
        * Retrieves a copy of the record from the current location
        * @return the value of the entry. Modifications to this will
        *         not be reflected in the context. This is a local copy.
        **/
        KnowledgeRecord to_record (void) const;

        /**
        * Fills a BufferVector container with all subkeys
        * @param  target  container to be filled
        **/
        void to_container (BufferVector & target) const;

        /**
        * Fills a DoubleVector container with all subkeys
        * @param  target  container to be filled
        **/
        void to_container (DoubleVector & target) const;

        /**
        * Fills a IntegerVector container with all subkeys
        * @param  target  container to be filled
        **/
        void to_container (IntegerVector & target) const;

        /**
        * Fills a NativeDoubleVector container with all subkeys
        * @param  target  container to be filled
        **/
        void to_container (NativeDoubleVector & target) const;

        /**
        * Fills a NativeIntegerVector container with all subkeys
        * @param  target  container to be filled
        **/
        void to_container (NativeIntegerVector & target) const;

        /**
        * Fills a StringVector container with all subkeys
        * @param  target  container to be filled
        **/
        void to_container (StringVector & target) const;

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
        KnowledgeRecord::Integer to_integer (void) const;

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
         * Sets the variable name that this refers to
         * @param var_name  the name of the variable in the knowledge base
         * @param knowledge  the knowledge base the variable is housed in
         **/
        void set_name (const std::string & var_name,
          KnowledgeBase & knowledge);
        
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
        bool is_true (void) const;

        /**
        * Checks if the value in the record is false (0)
        * @return  true if value is false
        **/
        bool is_false (void) const;

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
          const KnowledgeUpdateSettings & settings);
      
        /**
         * Sets the location within the map to the specified value
         *
         * @param value           value to set at location
         * @return                0 if successful, -1 if key is null, and
         *                        -2 if quality isn't high enough
         **/
        int set (
          KnowledgeRecord::Integer value = KnowledgeRecord::MODIFIED);

        /**
        * Sets the location to the specified Integer
        *
        * @param value           value to set at location
        **/
        void operator= (
          KnowledgeRecord::Integer value);

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
        void operator= (const std::vector <KnowledgeRecord::Integer> & value);

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
        int set (KnowledgeRecord::Integer value, 
          const KnowledgeUpdateSettings & settings);
        
        /**
         * Sets an index within an array to a specified value
         *
         * @param index           index of the location in the array
         * @param value           value to set at location
         * @return                0 if successful, -1 if key is null, and
         *                        -2 if quality isn't high enough
         **/
        int set_index (size_t index,
          KnowledgeRecord::Integer value);

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
          KnowledgeRecord::Integer value,
          const KnowledgeUpdateSettings & settings);
        
        /**
         * Sets the location within the map to the specified value
         *
         * @param value           array of integers to set at the location
         * @param size            number of elements in the array
         * @return                0 if successful, -1 if key is null, and
         *                        -2 if quality isn't high enough
         **/
        int set (const KnowledgeRecord::Integer * value,
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
        int set (const KnowledgeRecord::Integer * value,
          uint32_t size,
          const KnowledgeUpdateSettings & settings);
       
        /**
         * Sets the location within the map to the specified value
         *
         * @param value           array of integers to set at the location
         * @return                0 if successful, -1 if key is null, and
         *                        -2 if quality isn't high enough
         **/
        int set (const std::vector <KnowledgeRecord::Integer> & value);
       
        /**
         * Sets the location within the map to the specified value
         *
         * @param value           array of integers to set at the location
         * @param settings        settings for applying the update
         * @return                0 if successful, -1 if key is null, and
         *                        -2 if quality isn't high enough
         **/
        int set (const std::vector <KnowledgeRecord::Integer> & value,
          const KnowledgeUpdateSettings & settings);
       
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
          const KnowledgeUpdateSettings & settings);
        
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
          const KnowledgeUpdateSettings & settings);
        
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
          const KnowledgeUpdateSettings & settings);
       
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
          const KnowledgeUpdateSettings & settings);
        
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
          const KnowledgeUpdateSettings & settings);
        
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
          const KnowledgeUpdateSettings & settings);
      
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
          const KnowledgeUpdateSettings & settings);
      
        /**
         * Sets the quality of writing to a certain variable from this entity
         *
         * @param quality         quality of writing to this location
         * @param settings        settings for referring to knowledge variables
         **/
        void set_quality (uint32_t quality,
               const KnowledgeReferenceSettings & settings =
                       KnowledgeReferenceSettings (false));

        /**
        * Returns the type of the container along with name and any other
        * useful information. The provided information should be useful
        * for developers wishing to debug container operations, especially
        * as it pertains to pending network operations (i.e., when used
        * in conjunction with modify)
        *
        * @return info in format <container>: <name>< = value, if appropriate>
        **/
        std::string get_debug_info (void);

        /**
        * Clones this container
        * @return  a deep copy of the container that must be managed
        *          by the user (i.e., you have to delete the return value)
        **/
        virtual BaseContainer * clone (void) const;

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
        * @return info in format <container>: <name>< = value, if appropriate>
        **/
        virtual std::string get_debug_info_ (void);

        /// Updates the variable reference if necessary
        void update_variable (void) const;

        /// internal map of variable references
        typedef std::map <std::string, VariableReference>  InternalFlexMap;

        /**
         * Variable context that we are modifying
         **/
        ThreadSafeContext * context_;

        /**
         * The current location variable
         **/
        mutable VariableReference variable_;

        /**
        * Delimiter for the prefix to subvars
        **/
        std::string delimiter_;
      };
    }
  }
}

#endif // _MADARA_CONTAINERS_FLEX_MAP_H_
