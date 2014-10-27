
#ifndef _MADARA_CONTAINERS_BARRIER_H_
#define _MADARA_CONTAINERS_BARRIER_H_

#include <vector>
#include <string>
#include "madara/Lock_Type.h"
#include "madara/knowledge_engine/Knowledge_Base.h"
#include "madara/knowledge_engine/Thread_Safe_Context.h"
#include "madara/knowledge_engine/Knowledge_Update_Settings.h"

/**
 * @file Barrier.h
 * @author James Edmondson <jedmondson@gmail.com>
 *
 * This file contains a distributed barrier that may be
 * updated and read by many nodes
 **/

namespace Madara
{
  namespace Knowledge_Engine
  {
    namespace Containers
    {
      /**
       * @class Barrier
       * @brief This class stores an integer within a variable context
       */
      class MADARA_Export Barrier
      {
      public:
        /// trait that describes the value type
        typedef Knowledge_Record::Integer  type;
        
        /**
         * Default constructor
         **/
        Barrier (const Knowledge_Update_Settings & settings =
          Knowledge_Update_Settings ());
      
        /**
         * Constructor
         * @param  name       name of the integer in the knowledge base
         * @param  knowledge  the knowledge base that will contain the vector
         * @param  settings   settings for evaluating the vector
         **/
        Barrier (const std::string & name,
                Knowledge_Base & knowledge,
                const Knowledge_Update_Settings & settings =
                  Knowledge_Update_Settings ());
      
        /**
         * Constructor
         * @param  name      the name of the map within the variable context
         * @param  knowledge the variable context
         * @param  settings  settings to apply by default
         **/
        Barrier (const std::string & name,
                Variables & knowledge,
                const Knowledge_Update_Settings & settings =
                  Knowledge_Update_Settings ());
      
        /**
         * Default constructor
         * @param  name       name of the integer in the knowledge base
         * @param  knowledge  the knowledge base that will contain the vector
         * @param  id         the id of the barrier in the barrier ring
         * @param  participants   the number of participants in the barrier ring
         * @param  settings   settings for evaluating the vector
         **/
        Barrier (const std::string & name,
                Knowledge_Base & knowledge,
                int id,
                int participants,
                const Knowledge_Update_Settings & settings =
                  Knowledge_Update_Settings ());
      
        /**
         * Default constructor
         * @param  name       name of the integer in the knowledge base
         * @param  knowledge  the knowledge base that will contain the vector
         * @param  id         the id of the barrier in the barrier ring
         * @param  participants  the number of participants in the barrier ring
         * @param  settings   settings for evaluating the vector
         **/
        Barrier (const std::string & name,
                Variables & knowledge,
                int id,
                int participants,
                const Knowledge_Update_Settings & settings =
                  Knowledge_Update_Settings ());
      
        /**
         * Copy constructor
         **/
        Barrier (const Barrier & rhs);

        /**
         * Destructor
         **/
        ~Barrier ();
        
        /**
         * Assignment operator
         * @param  rhs    value to copy
         **/
        void operator= (const Barrier & rhs);

        /**
         * Returns the name of the variable
         * @return name of the variable
         **/
        std::string get_name (void) const;
        
        /**
         * Returns the id of the barrier in the barrier ring
         * @return the id of the barrier
         **/
        size_t get_id (void) const;
        
        /**
         * Returns the number of participants in the barrier ring
         * @return the number of participants barriering
         **/
        size_t get_participants (void) const;
        
        /**
         * Returns the current barrier round
         * @return the current barrier round number
         **/
        inline type get_round (void) const
        {
          return to_integer ();
        }

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
         * Sets the value of the variable
         * @param  value  the new value of the variable
         * @return the updated value (should be same as value param)
         **/
        type operator= (type value);
        
        /**
         * Goes to the next barrier round
         **/
        void next (void);
        
        /**
         * Wait for all other participants to reach your barrier round
         * @return true if barrier round is finished. False otherwise.
         **/
        bool is_done (void);
        
        /**
         * Mark the value as modified. The barrier retains the same value
         * but will resend its value as if it had been modified.
         **/
        void modify (void);

        /**
         * Returns the barrier round number as a Knowledge_Record. This
         * is useful for referencing clock and other record info.
         * @return the value as a Knowledge_Record
         **/
        Knowledge_Record to_record (void) const;

        /**
         * Returns the barrier round number as a double
         * @return the value as a double
         **/
        double to_double (void) const;
        
        /**
         * Returns the barrier round number as an integer (same as *)
         * @return the value as an integer
         **/
        Knowledge_Record::Integer to_integer (void) const;
        
        /**
         * Returns the barrier round number as a string
         * @return the value as a string
         **/
        std::string to_string (void) const;
        
        /**
         * Checks for equality
         * @param  value  the value to compare to
         * @return true if equal, false otherwise
         **/
        bool operator== (const Barrier & value) const;
        
        /**
         * Checks for inequality
         * @param  value  the value to compare to
         * @return true if inequal, false otherwise
         **/
        bool operator!= (const Barrier & value) const;
        
        /**
         * Sets the update settings for the variable
         * @param  settings  the new settings to use
         * @return the old update settings
         **/
        Knowledge_Update_Settings set_settings (
          const Knowledge_Update_Settings & settings);

        /**
         * Sets the quality of writing to the barrier variables
         *
         * @param quality         quality of writing to this location
         * @param settings        settings for referring to knowledge variables
         **/
        void set_quality (uint32_t quality,
               const Knowledge_Reference_Settings & settings =
                       Knowledge_Reference_Settings (false));
      
        /**
         * Resizes the barrier, usually when number of participants change
         * @param id        the id of this barrier in the barrier ring
         * @param participants the number of participants in barrier ring
         **/
        void resize (size_t id = 0, size_t participants = 1);

      private:
        /**
         * Builds the aggregate barrier logic
         **/
        void build_aggregate_barrier (void);
        
        /**
         * Checks if current barrier is successful
         * @return  0 if unsuccessful, otherwise it is successful
         **/
        inline type barrier_result (void)
        {
          return context_->evaluate (aggregate_barrier_, no_harm).to_integer ();
        }

        /**
         * Builds the variable that is actually incremented
         **/
        void build_var (void);

        /**
         * Initialize the no harm eval settings
         **/
        void init_noharm (void);

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
         * Variable reference
         **/
        Variable_Reference variable_;

        /**
         * id of this barrier in the barrier ring
         **/
        size_t id_;

        /**
         * the number of participants in the barrier ring
         **/
        size_t participants_;

        /**
         * Settings for modifications
         **/
        Knowledge_Update_Settings settings_;

        /**
         * Expression for aggregating barrier in one atomic operation
         **/
        Compiled_Expression aggregate_barrier_;
        
        /**
         * Settings we'll use for all evaluations
         **/
        Eval_Settings no_harm;

        /**
         * Holder for variable name to quickly refresh modified status
         **/
        std::string variable_name_;
      };
    }
  }
}




#endif // _MADARA_CONTAINERS_BARRIER_H_