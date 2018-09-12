#ifndef _MADARA_FILTERS_FRAGMENTS_TO_FILES_FILTER_H_
#define _MADARA_FILTERS_FRAGMENTS_TO_FILES_FILTER_H_

#ifndef BOOST_NO_CXX11_SCOPED_ENUMS
  #define BOOST_NO_CXX11_SCOPED_ENUMS
#endif

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
      virtual void filter (knowledge::KnowledgeMap & records,
        const transport::TransportContext &,
        knowledge::Variables &);

      /**
       * Sets a mapping between a variable prefix and a directory
       * @param  variable_prefix   the prefix that must match in a list of
       *                           filtered records
       * @param  directory         the directory to save fragments to
       **/
      inline void set_dir_mapping (const std::string & variable_prefix,
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
      inline std::string get_dir_mapping (const std::string & variable_prefix)
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
      inline void enable_clear_fragments (void)
      {
        clear_fragments_ = true;
      }

      /**
       * Disables the clear fragments flag. Fragments are saved to disk and
       * saved to the local knowledge base
       **/
      inline void disable_clear_fragments (void)
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
