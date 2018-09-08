#ifndef _MADARA_FILTERS_FRAGMENTS_TO_FILES_FILTER_H_
#define _MADARA_FILTERS_FRAGMENTS_TO_FILES_FILTER_H_

#include <string>
#include <vector>
#include "madara/knowledge/KnowledgeUpdateSettings.h"
#include "madara/knowledge/KnowledgeRecord.h"
#include "madara/utility/Utility.h"
#include "madara/utility/ScopedArray.h"
#include "madara/knowledge/containers/Vector.h"
#include "madara/logger/GlobalLogger.h"
#include "madara/filters/AggregateFilter.h"

namespace madara
{
  namespace filters
  {
    /**
     * @class FragmentsToFilesFilter
     * @brief Receives fragments and saves them to files. This filter is
     *        intended to be paired with the FileFragmenter class, e.g.,
     *        with the Madara File Service (mfs)
     */
    class FragmentsToFilesFilter : public AggregateFilter
    {
    public:
      /**
       * Constructor
       **/
      FragmentsToFilesFilter ()
      : clear_fragments_ (true)
      {
      }

      /**
       * Destructor
       **/
      virtual ~FragmentsToFilesFilter () = default;

      /**
       * Filters the fragments and converts them to files
       * @param   records           the aggregate records vector
       **/
      inline virtual void filter (knowledge::KnowledgeMap & records,
        const transport::TransportContext &,
        knowledge::Variables &)
      {
          // because of the usage of erase, don't auto inc record in for loop
          for (auto record = records.begin (); record != records.end ();)
          {
            bool is_fragment = false;
            std::string path;
            std::string prefix;
            std::string filename;

            // try to find an appropriate mapping for the record
            for (auto map : map_)
            {
              if (utility::begins_with (record->first, map.first))
              {
                prefix = map.first;
                path = map.second;
                filename = path + "/" + record->first.substr (prefix.size () + 1);
                is_fragment = true;

                madara_logger_ptr_log (
                  madara::logger::global_logger.get (),
                  logger::LOG_ALWAYS,
                  "FragmentsToFilesFilter::filter: "
                  "found fragment. Saving %s to %s\n",
                  record->first.c_str (),
                  filename.c_str ()
                )
              }
              else
              {
                madara_logger_ptr_log (
                  madara::logger::global_logger.get (),
                  logger::LOG_ALWAYS,
                  "FragmentsToFilesFilter::filter: "
                  "%s is not a fragment of %s\n",
                  record->first.c_str (),
                  map.first.c_str ()
                )
              }
            }

            if (is_fragment && clear_fragments_)
            {
              madara_logger_ptr_log (
                madara::logger::global_logger.get (),
                logger::LOG_ALWAYS,
                "FragmentsToFilesFilter::filter: "
                "removing variable %s\n",
                record->first.c_str ()
              )

              record = records.erase (record); 
            }
            else
            {
              madara_logger_ptr_log (
                madara::logger::global_logger.get (),
                logger::LOG_ALWAYS,
                "FragmentsToFilesFilter::filter: "
                "%s is not being deleted\n",
                record->first.c_str ()
              )
              ++record;
            }
          }
      }

      /**
       * Sets a mapping between a variable prefix and a directory
       * @param  variable_prefix   the prefix that must match in a list of
       *                           filtered records
       * @param  directory         the directory to save fragments to
       **/
      void set_dir_mapping (const std::string & variable_prefix,
        const std::string & directory)
      {
        madara_logger_ptr_log (
          madara::logger::global_logger.get (),
          logger::LOG_MAJOR,
          "FragmentsToFilesFilter::set_dir_mapping: "
          "attempting to map %s -> %s\n",
          variable_prefix.c_str (), directory.c_str ()
        )

        if (variable_prefix != "" && directory != "")
        {
          map_[variable_prefix] = directory;
        }
      }

      /**
       * Gets the directory associated with the variable prefix
       * @param  variable_prefix   the prefix that must match in a list of
       *                           filtered records
       **/
      std::string get_dir_mapping (const std::string & variable_prefix)
      {
        auto found = map_.find (variable_prefix);

        if (found != map_.end ())
        {
          return found->second;
        }
        else
        {
          return "";
        }
      }

      /**
       * Enables the clear fragments flag. Whenever a fragment is saved to
       * disk, it will not also be applied to the local knowledge base
       **/
      void enable_clear_fragments (void)
      {
        clear_fragments_ = true;
      }

      /**
       * Disables the clear fragments flag. Fragments are saved to disk and
       * saved to the local knowledge base
       **/
      void disable_clear_fragments (void)
      {
        clear_fragments_ = false;
      }

      /// if true, clear fragments after sent to file
      bool clear_fragments_;

      /// map of variable prefixes to directories
      std::map <std::string, std::string> map_;
    };
  }
}

#endif // _MADARA_FILTERS_FRAGMENTS_TO_FILES_FILTER_H_
