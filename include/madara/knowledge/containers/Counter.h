
#ifndef _MADARA_CONTAINERS_COUNTER_H_
#define _MADARA_CONTAINERS_COUNTER_H_

#ifndef _MADARA_NO_KARL_

#include <vector>
#include <string>
#include "madara/LockType.h"
#include "madara/knowledge/KnowledgeBase.h"
#include "madara/knowledge/ThreadSafeContext.h"
#include "madara/knowledge/KnowledgeUpdateSettings.h"
#include "BaseContainer.h"

/**
 * @file Counter.h
 * @author James Edmondson <jedmondson@gmail.com>
 *
 * This file contains a distributed counter that may be
 * updated and read by many nodes
 **/

namespace madara
{
  namespace knowledge
  {
    namespace containers
    {
      /**
       * @class Counter
       * @brief This class stores an integer within a variable context
       */
      class MADARA_Export Counter : public BaseContainer
      {
      public:
        /// trait that describes the value type
        typedef KnowledgeRecord::Integer  type;
        
        /**
         * Default constructor
         **/
        Counter (const KnowledgeUpdateSettings & settings =
          KnowledgeUpdateSettings ());
      
        /**
         * Constructor
         * @param  name       name of the integer in the knowledge base
         * @param  knowledge  the knowledge base that will contain the vector
         * @param  settings   settings for evaluating the vector
         **/
        Counter (const std::string & name,
                KnowledgeBase & knowledge,
                const KnowledgeUpdateSettings & settings =
                  KnowledgeUpdateSettings ());
      
        /**
         * Constructor
         * @param  name      the name of the map within the variable context
         * @param  knowledge the variable context
         * @param  settings  settings to apply by default
         **/
        Counter (const std::string & name,
                Variables & knowledge,
                const KnowledgeUpdateSettings & settings =
                  KnowledgeUpdateSettings ());
      
        /**
         * Default constructor
         * @param  name       name of the integer in the knowledge base
         * @param  knowledge  the knowledge base that will contain the vector
         * @param  id         the id of the counter in the counter ring
         * @param  counters   the number of counters in the counter ring
         * @param  value      new value of the variable in the knowledge base
         * @param  settings   settings for evaluating the vector
         **/
        Counter (const std::string & name,
                KnowledgeBase & knowledge,
                int id,
                int counters,
                type value = 0,
                const KnowledgeUpdateSettings & settings =
                  KnowledgeUpdateSettings ());
      
        /**
         * Default constructor
         * @param  name       name of the integer in the knowledge base
         * @param  knowledge  the knowledge base that will contain the vector
         * @param  id         the id of the counter in the counter ring
         * @param  counters  the number of counters in the counter ring
         * @param  value      new value of the variable in the knowledge base
         * @param  settings   settings for evaluating the vector
         **/
        Counter (const std::string & name,
                Variables & knowledge,
                int id,
                int counters,
                type value = 0,
                const KnowledgeUpdateSettings & settings =
                  KnowledgeUpdateSettings ());
      
        /**
         * Copy constructor
         **/
        Counter (const Counter & rhs);

        /**
         * Destructor
         **/
        ~Counter ();
        
        /**
         * Assignment operator
         * @param  rhs    value to copy
         **/
        void operator= (const Counter & rhs);

        /**
         * Returns the id of the counter in the counter ring
         * @return the id of the counter
         **/
        int get_id (void) const;
        
        /**
         * Returns the number of counters in the counter ring
         * @return the number of counters counting
         **/
        int get_counters (void) const;
        
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
         * Mark the value as modified. The Counter retains the same value
         * but will resend its value as if it had been modified.
         **/
        void modify (void);

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
        bool operator== (const Counter & value) const;
        
        /**
         * Checks for inequality
         * @param  value  the value to compare to
         * @return true if inequal, false otherwise
         **/
        bool operator!= (const Counter & value) const;
        
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
         * Increments by a value
         * @param  value  the value to add
         * @return the new value
         **/
        void operator+= (type value);
        
        /**
         * Decrements by a value
         * @param  value  the value to remove
         * @return the new value
         **/
        void operator-= (type value);
        
        /**
         * Increments the value of the variable and returns
         * the result.
         * @return the new value of the variable
         **/
        void operator++ (void);
        
        /**
         * Decrements the value of the variable and returns
         * the result.
         * @return the new value of the variable
         **/
        void operator-- (void);
        
        /**
         * Returns the value as a KnowledgeRecord. This
         * is useful for referencing clock and other record info.
         * @return the value as a KnowledgeRecord
         **/
        KnowledgeRecord to_record (void) const;

        /**
         * Returns the value as a double
         * @return the value as a double
         **/
        double to_double (void) const;
        
        /**
         * Returns the value as an integer (same as *)
         * @return the value as an integer
         **/
        KnowledgeRecord::Integer to_integer (void) const;
        
        /**
         * Returns the value as a string
         * @return the value as a string
         **/
        std::string to_string (void) const;
        
        /**
         * Sets the quality of writing to the counter variables
         *
         * @param quality         quality of writing to this location
         * @param settings        settings for referring to knowledge variables
         **/
        void set_quality (uint32_t quality,
               const KnowledgeReferenceSettings & settings =
                       KnowledgeReferenceSettings (false));
      
        /**
         * Resizes the counter, usually when number of counters change
         * @param id        the id of this counter in the counter ring
         * @param counters the number of counters in counter ring
         **/
        void resize (int id = 0, int counters = 1);

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

        /**
        * Determines if the local count is not zero
        * @return true if the local count is not zero
        **/
        bool is_true (void) const;

        /**
        * Determines if the local count is zero
        * @return true if the local count is zero
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
        * @return info in format <container>: <name>< = value, if appropriate>
        **/
        virtual std::string get_debug_info_ (void);

        /**
         * Builds the aggregate counter logic
         **/
        void build_aggregate_count (void);
        
        /**
         * Builds the variable that is actually incremented
         **/
        void build_var (void);

        /**
         * Initialize the no harm eval settings
         **/
        void init_noharm (void);

        /**
         * Counts all counter variables
         * @return  total count
         **/
        inline type get_count (void) const
        {
          return context_->evaluate (aggregate_count_, no_harm).to_integer ();
        }
        
        /**
         * Counts all counter variables
         * @return  total count
         **/
        inline std::string get_count_string (void) const
        {
          return context_->evaluate (aggregate_count_, no_harm).to_string ();
        }
        
        /**
         * Counts all counter variables
         * @return  total count
         **/
        inline double get_count_double (void) const
        {
          return context_->evaluate (aggregate_count_, no_harm).to_double ();
        }
        
        /**
         * Counts all counter variables
         * @return  total count
         **/
        inline KnowledgeRecord get_count_record (void) const
        {
          return context_->evaluate (aggregate_count_, no_harm);
        }

        /**
         * Variable context that we are modifying
         **/
        mutable ThreadSafeContext * context_;

        /**
         * Variable reference
         **/
        VariableReference variable_;

        /**
         * id of this counter in the counter ring
         **/
        int id_;

        /**
         * the number of counters in the counter ring
         **/
        int counters_;

        /**
         * Expression for aggregating count in one atomic operation
         **/
        CompiledExpression aggregate_count_;

        /**
         * Settings we'll use for all evaluations
         **/
        EvalSettings no_harm;
      };
    }
  }
}

#endif // _MADARA_NO_KARL_

#endif // _MADARA_CONTAINERS_COUNTER_H_