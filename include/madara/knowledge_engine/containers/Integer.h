
#ifndef _MADARA_INTEGER_H_
#define _MADARA_INTEGER_H_

#include <vector>
#include <string>
#include "madara/Lock_Type.h"
#include "madara/knowledge_engine/Knowledge_Base.h"
#include "madara/knowledge_engine/Thread_Safe_Context.h"
#include "madara/knowledge_engine/Knowledge_Update_Settings.h"

/**
 * @file Integer.h
 * @author James Edmondson <jedmondson@gmail.com>
 *
 * This file contains a C++ object that manages interactions for an
 * array of variables
 **/

namespace Madara
{
  namespace Knowledge_Engine
  {
    namespace Containers
    {
      /**
       * @class Integer
       * @brief This class stores an integer within a variable context
       */
      class MADARA_Export Integer
      {
      public:
        /// trait that describes the value type
        typedef Knowledge_Record::Integer  type;
        
        /**
         * Default constructor
         **/
        Integer (const Knowledge_Update_Settings & settings =
          Knowledge_Update_Settings ());
      
        /**
         * Constructor
         * @param  name       name of the integer in the knowledge base
         * @param  knowledge  the knowledge base that will contain the vector
         * @param  settings   settings for evaluating the vector
         **/
        Integer (const std::string & name,
                Knowledge_Base & knowledge,
                const Knowledge_Update_Settings & settings =
                  Knowledge_Update_Settings ());
      
        /**
         * Constructor
         * @param  name      the name of the map within the variable context
         * @param  knowledge the variable context
         * @param  settings  settings to apply by default
         **/
        Integer (const std::string & name,
                Variables & knowledge,
                const Knowledge_Update_Settings & settings =
                  Knowledge_Update_Settings ());
      
        /**
         * Default constructor
         * @param  name       name of the integer in the knowledge base
         * @param  knowledge  the knowledge base that will contain the vector
         * @param  value      new value of the variable in the knowledge base
         * @param  settings   settings for evaluating the vector
         **/
        Integer (const std::string & name,
                Knowledge_Base & knowledge,
                type value, 
                const Knowledge_Update_Settings & settings =
                  Knowledge_Update_Settings ());
      
        /**
         * Default constructor
         * @param  name       name of the integer in the knowledge base
         * @param  knowledge  the knowledge base that will contain the vector
         * @param  value      new value of the variable in the knowledge base
         * @param  settings   settings for evaluating the vector
         **/
        Integer (const std::string & name,
                Variables & knowledge,
                type value, 
                const Knowledge_Update_Settings & settings =
                  Knowledge_Update_Settings ());
      
        /**
         * Copy constructor
         **/
        Integer (const Integer & rhs);

        /**
         * Destructor
         **/
        ~Integer ();
        
        /**
         * Mark the value as modified. The Integer retains the same value
         * but will resend its value as if it had been modified.
         **/
        void modify (void);

        /**
         * Assignment operator
         * @param  rhs    value to copy
         **/
        void operator= (const Integer & rhs);

        /**
         * Returns the name of the variable
         * @return name of the variable
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
         * Sets the variable name that this refers to
         * @param var_name  the name of the variable in the knowledge base
         * @param knowledge  the knowledge base the variable is housed in
         **/
        void set_name (const std::string & var_name,
          Thread_Safe_Context & knowledge);

        /**
         * Exchanges the integer at this location with the integer at another
         * location.
         * @param  other   the other integer to exchange with
         **/
        void exchange (Containers::Integer & other);

        /**
         * Sets the value of the variable
         * @param  value  the new value of the variable
         * @return the updated value (should be same as value param)
         **/
        type operator= (type value);
        
        /**
         * Checks for equality
         * @param  value  the value to compare to
         * @return true if equal, false otherwise
         **/
        bool operator== (const Integer & value) const;
        
        /**
         * Checks for inequality
         * @param  value  the value to compare to
         * @return true if inequal, false otherwise
         **/
        bool operator!= (const Integer & value) const;
        
        /**
         * Checks for equality
         * @param  value  the value to compare to
         * @return true if equal, false otherwise
         **/
        bool operator== (type value) const;
        
        /**
         * Checks for inequality
         * @param  value  the value to compare to
         * @return true if inequal, false otherwise
         **/
        bool operator!= (type value) const;
        
        /**
         * Checks for less than relationship
         * @param  value  the value to compare to
         * @return true if less than
         **/
        bool operator< (type value) const;
        
        /**
         * Checks for less than or equal to relationship
         * @param  value  the value to compare to
         * @return true if less than or equal to
         **/
        bool operator<= (type value) const;
        
        /**
         * Checks for greater than relationship
         * @param  value  the value to compare to
         * @return true if greater than
         **/
        bool operator> (type value) const;
        
        /**
         * Checks for greater than or equal to relationship
         * @param  value  the value to compare to
         * @return true if greater than or equal to
         **/
        bool operator>= (type value) const;
        
        /**
         * Returns the value of the variable
         * @return the value of the variable
         **/
        type operator* (void) const;
      
        /**
         * Checks to see if the variable has ever been assigned a value
         * @return true if the record has been set to a value. False if
         *         uninitialized
         **/
        bool exists (void) const;
      
        /**
         * Increments by a value
         * @param  value  the value to add
         * @return the new value
         **/
        type operator+= (type value);
        
        /**
         * Decrements by a value
         * @param  value  the value to remove
         * @return the new value
         **/
        type operator-= (type value);
        
        /**
         * Increments the value of the variable and returns
         * the result.
         * @return the new value of the variable
         **/
        type operator++ (void);
        
        /**
         * Decrements the value of the variable and returns
         * the result.
         * @return the new value of the variable
         **/
        type operator-- (void);
        
        /**
         * Returns the value as a Knowledge_Record. This
         * is useful for referencing clock and other record info.
         * @return the value as a Knowledge_Record
         **/
        Knowledge_Record to_record (void) const;

        /**
         * Returns the value as a double
         * @return the value as a double
         **/
        double to_double (void) const;
        
        /**
         * Returns the value as an integer (same as *)
         * @return the value as an integer
         **/
        Knowledge_Record::Integer to_integer (void) const;
        
        /**
         * Returns the value as a string
         * @return the value as a string
         **/
        std::string to_string (void) const;
        
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
        Thread_Safe_Context * context_;

        /**
         * Prefix of variable
         **/
        std::string name_;

        /**
         * Variable reference
         **/
        Variable_Reference variable_;

        /**
         * Settings for modifications
         **/
        Knowledge_Update_Settings settings_;
      };
    }
  }
}




#endif // _MADARA_INTEGER_H_