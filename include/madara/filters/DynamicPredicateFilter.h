

#ifndef _MADARA_FILTERS_DYNAMIC_PREDICATE_FILTER_H_
#define _MADARA_FILTERS_DYNAMIC_PREDICATE_FILTER_H_

/**
 * @file DynamicPredicateFilter.h
 * @author James Edmondson <jedmondson@gmail.com>
 *
 * This file contains a filter for including only approved predicates
 **/

#include "AggregateFilter.h"
#include "madara/knowledge/containers/StringVector.h"
#include "madara/utility/Utility.h"

namespace madara
{
  namespace filters
  {
    /**
     * Filter for removing variables with any unwanted predicates
     **/
    class MADARA_EXPORT DynamicPredicateFilter : public AggregateFilter
    {
    public:
      /**
      * Constructor
      * @param string_vector_name   the name of the string vector in the KB
      *                             to use for predicate information
      **/
      DynamicPredicateFilter (
        const std::string & string_vector_name = "predicates.allowed")
      : initialized_ (false), name (string_vector_name), predicates_ ()
      {

      }

      /**
       * Destructor
       **/
      virtual ~DynamicPredicateFilter () = default;

      /**
       * Removes records not in a list of allowed predicates
       * @param   records           the aggregate records vector
       * @param   vars              context for querying current program state
       **/
      inline virtual void filter (knowledge::KnowledgeMap & records,
        const transport::TransportContext &,
        knowledge::Variables & vars)
      {
        // local copy (fast access to predicate vector)
        std::vector <std::string> predicates;

        if (!initialized_)
        {
          // set the initial vector up
          predicates_.set_name (name, vars);
          initialized_ = true;
        }
        else
        {
          // resize for what the current vector is indicating
          predicates_.resize ();
        }

        // copy the knowledge base predicates to a STL vector for speed
        predicates_.copy_to (predicates);

        // by default, the vector is empty and all predicates are accepted
        if (predicates.size () > 0)
        {
          // because of the usage of erase, don't auto inc record in for loop
          for (auto record = records.begin (); record != records.end ();)
          {
            // check for valid predicate
            bool accepted_predicate = false;
            for (auto predicate : predicates)
            {
              if (utility::ends_with (predicate, "*"))
              {
                std::string prefix = predicate;
                prefix.pop_back ();
                if (utility::begins_with (record->first, prefix))
                {
                  accepted_predicate = true;
                }
              }
              else if (record->first == predicate)
              {
                accepted_predicate = true;
              }
            }

            // if not valid, remove the record and update iterator
            if (!accepted_predicate)
            {
              madara_logger_ptr_log (
                madara::logger::global_logger.get (),
                logger::LOG_MAJOR,
                "DynamicPredicateFilter::filter: "
                "removing variable %s\n",
                record->first.c_str ()
              )

              record = records.erase (record); 
            } // end not valid predicate
            else
            {
              // valid predicate so keep the record and proceed to next
              ++record;
            } // end valid predicate
          } // end iteration over records
        } // end if there are predicates to enforce
      } // end filter method

    private:

      /**
       * Tracks if the predicate vector has been initialized
       **/
      bool initialized_;

    public:
      /**
       * The string vector name to bind to inside the knowledge base
       **/
      std::string name;

    protected:

      /**
       * A map of discovered peers
       **/
      knowledge::containers::StringVector predicates_;

    };
  }
}

#endif  // _MADARA_FILTERS_DYNAMIC_PREDICATE_FILTER_H_
