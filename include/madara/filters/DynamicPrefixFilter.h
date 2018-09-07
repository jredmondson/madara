

#ifndef _MADARA_FILTERS_DYNAMIC_PREFIX_FILTER_H_
#define _MADARA_FILTERS_DYNAMIC_PREFIX_FILTER_H_

/**
 * @file DynamicPrefixFilter.h
 * @author James Edmondson <jedmondson@gmail.com>
 *
 * This file contains a filter for including only approved prefixes
 **/

#include "AggregateFilter.h"
#include "madara/knowledge/containers/StringVector.h"
#include "madara/utility/Utility.h"

namespace madara
{
  namespace filters
  {
    /**
     * Filter for removing variables with any unwanted prefixes
     **/
    class MADARA_EXPORT DynamicPrefixFilter : public AggregateFilter
    {
    public:
      /**
      * Constructor
      * @param string_vector_name   the name of the string vector in the KB
      *                             to use for prefix information
      **/
      DynamicPrefixFilter (
        const std::string & string_vector_name = "prefixes.allowed")
      : initialized_ (false), name (string_vector_name), prefixes_ ()
      {

      }

      /**
       * Destructor
       **/
      virtual ~DynamicPrefixFilter () = default;

      /**
       * Removes records not in a list of allowed prefixes
       * @param   records           the aggregate records vector
       * @param   vars              context for querying current program state
       **/
      inline virtual void filter (knowledge::KnowledgeMap & records,
        const transport::TransportContext &,
        knowledge::Variables & vars)
      {
        // local copy (fast access to prefix vector)
        std::vector <std::string> prefixes;

        if (!initialized_)
        {
          // set the initial vector up
          prefixes_.set_name (name, vars);
          initialized_ = true;
        }
        else
        {
          // resize for what the current vector is indicating
          prefixes_.resize ();
        }

        // copy the knowledge base prefixes to a STL vector for speed
        prefixes_.copy_to (prefixes);

        // by default, the vector is empty and all prefixes are accepted
        if (prefixes.size () > 0)
        {
          // because of the usage of erase, don't auto inc record in for loop
          for (auto record = records.begin (); record != records.end ();)
          {
            // check for valid prefix
            bool accepted_prefix = false;
            for (auto prefix : prefixes)
            {
              if (utility::begins_with (record->first, prefix))
              {
                accepted_prefix = true;
              }
            }

            // if not valid, remove the record and update iterator
            if (!accepted_prefix)
            {
              madara_logger_ptr_log (
                madara::logger::global_logger.get (),
                logger::LOG_MAJOR,
                "DynamicPrefixFilter::filter: "
                "removing variable %s\n",
                record->first.c_str ()
              )

              record = records.erase (record); 
            } // end not valid prefix
            else
            {
              // valid prefix so keep the record and proceed to next
              ++record;
            } // end valid prefix
          } // end iteration over records
        } // end if there are prefixes to enforce
      } // end filter method

    private:

      /**
       * Tracks if the prefix vector has been initialized
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
      knowledge::containers::StringVector prefixes_;

    };
  }
}

#endif  // _MADARA_FILTERS_DYNAMIC_PREFIX_FILTER_H_
