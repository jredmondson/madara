/* -*- C++ -*- */
#ifndef _MADARA_EXPRESSION_VARIABLEEXPANDER_H_
#define _MADARA_EXPRESSION_VARIABLEEXPANDER_H_

#ifndef _MADARA_NO_KARL_

#include <string>
#include <vector>
#include <stdexcept>

#include "madara/logger/Logger.h"
#include "madara/knowledge/VariableReference.h"
#include "madara/utility/Utility.h"

namespace madara
{
  namespace expression
  {
    /**
     * A convenience class for 
     **/
    class MADARA_EXPORT VariableExpander
    {
    public:
      /**
       * Expands a key into either a VariableReference or tokens
       * @throw exceptions::KarlException  occurs when variable expansion is done
       *                       improperly, e.g., when too few expander
       *                       braces ({}) are used.
       **/
      inline madara::knowledge::VariableReference expand (const std::string & key,
        const std::string & print_prefix, 
        madara::knowledge::ThreadSafeContext & context,
        madara::logger::Logger * cur_logger,
        bool & key_expansion_necessary,
        std::vector< std::string> & splitters,
        std::vector< std::string> & tokens,
        std::vector< std::string> & pivot_list)
      {
        knowledge::VariableReference ref;

        // this key requires expansion. We do the compilation and error checking here
        // as the key shouldn't change, and this allows us to only have to do this
        // once
        if (key.find ("{") != key.npos)
        {
          madara_logger_ptr_log (cur_logger, logger::LOG_DETAILED,
            "%s: Variable %s requires variable expansion.\n",
            print_prefix.c_str (), key.c_str ());

          key_expansion_necessary = true;
          splitters.push_back ("{");
          splitters.push_back ("}");

          utility::tokenizer (key, splitters, tokens, pivot_list);

          if (pivot_list.size () % 2 != 0)
          {
            std::stringstream buffer;
            buffer << print_prefix;
            buffer << ": KARL COMPILE ERROR: matching braces not found in ";
            buffer << key << "\n";
            madara_logger_ptr_log (cur_logger, logger::LOG_ERROR,
              buffer.str ().c_str ());

            throw exceptions::KarlException (buffer.str ());
          }

          // check for braces that are not properly closed
          std::vector<std::string>::const_iterator pivot = pivot_list.begin ();
          unsigned int num_opens = 0;
          unsigned int num_closes = 0;

          for (; pivot != pivot_list.end (); ++pivot)
          {
            if (*pivot == "{")
            {
              ++num_opens;
            }
            else if (*pivot == "}")
            {
              ++num_closes;
            }
          }

          if (num_opens > num_closes)
          {
            std::stringstream buffer;
            buffer << print_prefix;
            buffer << ": KARL COMPILE ERROR: more opening braces than closing in ";
            buffer << key << "\n";
            madara_logger_ptr_log (cur_logger, logger::LOG_ERROR,
              buffer.str ().c_str ());

            throw exceptions::KarlException (buffer.str ());
          }
          else if (num_closes > num_opens)
          {
            std::stringstream buffer;
            buffer << print_prefix;
            buffer << ": KARL COMPILE ERROR: more closing braces than opening in ";
            buffer << key << "\n";
            madara_logger_ptr_log (cur_logger, logger::LOG_ERROR,
              buffer.str ().c_str ());

            throw exceptions::KarlException (buffer.str ());
          }
        }
        // no variable expansion necessary. Create a hard link to the record_->
        // this will save us lots of clock cycles each variable access or
        // mutation.
        else
        {
          ref = context.get_ref (key);
        }

        return ref;
      }
    };
  }
}

#endif // _MADARA_NO_KARL_

#endif /* _MADARA_EXPRESSION_VARIABLEEXPANDER_H_ */
