#include "boost/filesystem.hpp"
#include "FragmentsToFilesFilter.h"

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

    void
    FragmentsToFilesFilter::filter (knowledge::KnowledgeMap & records,
        const transport::TransportContext &,
        knowledge::Variables &)
    {
      std::string last_file;
      std::string last_crc;
      int64_t last_size = 0;
      std::string last_path;

      // because of the usage of erase, don't auto inc record in for loop
      for (auto record = records.begin (); record != records.end ();)
      {
        bool is_fragment = false;
        std::string path;
        std::string prefix;
        std::string filename;

        // if the file is malicious, pretend it doesn't exist
        if (utility::filename_has_redirect (record->first))
        {
          madara_logger_ptr_log (
            madara::logger::global_logger.get (),
            logger::LOG_ALWAYS,
            "FragmentsToFilesFilter::filter: "
            "WARNING: record %s has abnormal name. Skipping.\n",
            record->first.c_str ()
          )

          continue;
        }

        // try to find an appropriate mapping for the record
        for (auto map : map_)
        {
          if (utility::begins_with (record->first, map.first) &&
              record->second.is_binary_file_type ())
          {
            prefix = map.first;
            path = map.second;
            filename = path + "/" +
              record->first.substr (prefix.size () + 1);
            std::size_t last_period = record->first.find_last_of ('.');

            if (last_file == "" || 
                last_file.compare (0, last_file.size (), record->first) != 0)
            {
              if (last_file != "")
              {
                // attempt to assemble files
                int i = 0;
              }

              // setup the next file
              last_path = path;
              last_file = record->first.substr (0, last_period);
              auto crc_record = records.find (last_file + ".crc");
              auto size_record = records.find (last_file + ".size");

              if (crc_record != records.end ())
              {
                last_crc = crc_record->second.to_string ();
                last_size = size_record->second.to_integer ();
              } // end if crc record exists in the incoming records
            } // if we need to set a new crc and last record

            if (last_crc != "")
            {
              filename += "." + last_crc + ".frag";
              is_fragment = true;

              // create directory that file needs to exist in
              std::string directory = utility::extract_path (filename);
              if (!boost::filesystem::is_directory (directory))
              {
                madara_logger_ptr_log (
                  madara::logger::global_logger.get (),
                  logger::LOG_ALWAYS,
                  "FragmentsToFilesFilter::filter: "
                  "recursively creating directory %s\n",
                  directory.c_str ()
                )

                boost::filesystem::create_directories (directory);
              }

              record->second.to_file (filename);

              madara_logger_ptr_log (
                madara::logger::global_logger.get (),
                logger::LOG_ALWAYS,
                "FragmentsToFilesFilter::filter: "
                "found fragment %s:\n"
                "  last_file=%s\n"
                "  last_crc=%s\n"
                "  last_size=%" PRId64 "\n"
                "  saved to %s\n",
                record->first.c_str (),
                last_file.c_str (),
                last_crc.c_str (),
                last_size,
                filename.c_str ()
              )
            } // end if last_crc is not null
          } // end if begins with a prefix mapping to directory
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
          } // end does not begin with a known prefix
        } // end iteration over directory mappings

        // if this is a fragment, check for delection
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
        } // end if is fragment and clear fragments enabled
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
        } // end no clear fragments needed
      } // end iteration over incoming records
    } // end filter method

  } // end filters namespace
} // end madara namespace

