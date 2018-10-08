
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
void FragmentsToFilesFilter::filter(knowledge::KnowledgeMap& records,
    const transport::TransportContext&, knowledge::Variables&)
{
  std::string last_file;
  std::string str_crc;
  uint32_t last_crc = 0;
  int64_t last_size = 0;
  std::string last_file_path;

  // because of the usage of erase, don't auto inc record in for loop
  for (auto record = records.begin(); record != records.end();)
  {
    bool is_fragment = false;
    std::string path;
    std::string prefix;
    std::string filename;

    // if the file is malicious, pretend it doesn't exist
    if (utility::filename_has_redirect(record->first))
    {
      madara_logger_ptr_log(madara::logger::global_logger.get(),
          logger::LOG_MINOR,
          "FragmentsToFilesFilter::filter: "
          "WARNING: record %s has abnormal name. Skipping.\n",
          record->first.c_str())

          continue;
    }

    if (record->second.is_binary_file_type())
    {
      // try to find an appropriate mapping for the record
      for (auto map : map_)
      {
        if (utility::begins_with(record->first, map.first))
        {
          prefix = map.first;
          path = map.second;
          std::string number =
              record->first.substr(record->first.find_last_of('.') + 1);
          std::string base_name = record->first.substr(prefix.size() + 1);
          base_name.erase(base_name.find_last_of('.'));
          base_name.erase(base_name.find_last_of('.'));
          std::size_t last_period = record->first.find_last_of('.');

          // 1 thing missing: we need to remove contents from filename
          // utility::string_replace (base_name, ".contents", "");

          filename = path + "/" + base_name + ".";
          filename += number;
          // base_name.erase (
          //   (base_name.find_last_of ('.'),
          //     base_name.find_last_of ('.') - 1));

          if (last_file == "" || last_file.compare(0, last_file.size(),
                                     record->first.substr(0, last_period)) != 0)
          {
            if (last_file_path != "")
            {
              if (utility::file_from_fragments(
                      last_file_path, last_crc, true, false))
              {
                madara_logger_ptr_log(madara::logger::global_logger.get(),
                    logger::LOG_MAJOR,
                    "FragmentsToFilesFilter::filter: "
                    "SUCCESS: file %s is recreated\n",
                    last_file_path.c_str())
              }
              else
              {
                madara_logger_ptr_log(madara::logger::global_logger.get(),
                    logger::LOG_MAJOR,
                    "FragmentsToFilesFilter::filter: "
                    "FAIL: file %s is incomplete\n",
                    last_file_path.c_str())
              }
            }

            // setup the next file
            last_file_path = path + "/" +
                             record->first.substr(prefix.size() + 1,
                                 last_period - prefix.size() - 1 - 9);
            last_file = prefix + "." + base_name;
            auto crc_record = records.find(last_file + ".crc");
            auto size_record = records.find(last_file + ".size");

            if (crc_record != records.end())
            {
              last_crc = (uint32_t)crc_record->second.to_integer();
              str_crc = crc_record->second.to_string();
            }  // end if crc record exists in the incoming records

            if (size_record != records.end())
            {
              last_size = size_record->second.to_integer();
            }  // end if size exists in the incoming records
          }    // if we need to set a new crc and last record

          if (str_crc != "")
          {
            filename += "." + str_crc + ".frag";
            is_fragment = true;

            // create directory that file needs to exist in
            std::string directory = utility::extract_path(filename);

            utility::recursive_mkdir(directory);

            record->second.to_file(filename);

            madara_logger_ptr_log(madara::logger::global_logger.get(),
                logger::LOG_MAJOR,
                "FragmentsToFilesFilter::filter: "
                "found fragment %s:\n"
                "  base_name=%s\n"
                "  last_file=%s\n"
                "  last_file_path=%s\n"
                "  str_crc=%s\n"
                "  last_size=%" PRId64 "\n"
                "  saved to %s\n",
                record->first.c_str(), base_name.c_str(), last_file.c_str(),
                last_file_path.c_str(), str_crc.c_str(), last_size,
                filename.c_str())
          }  // end if str_crc is not null
          else
          {
            madara_logger_ptr_log(madara::logger::global_logger.get(),
                logger::LOG_MAJOR,
                "FragmentsToFilesFilter::filter: "
                "not fragment %s:\n"
                "  base_name=%s\n"
                "  last_file=%s\n"
                "  last_file_path=%s\n"
                "  str_crc=%s\n"
                "  last_size=%" PRId64 "\n"
                "  saved to %s\n",
                record->first.c_str(), base_name.c_str(), last_file.c_str(),
                last_file_path.c_str(), str_crc.c_str(), last_size,
                filename.c_str())
          }
        }  // end if begins with a prefix mapping to directory
        else
        {
          madara_logger_ptr_log(madara::logger::global_logger.get(),
              logger::LOG_MINOR,
              "FragmentsToFilesFilter::filter: "
              "%s is not a fragment of %s\n",
              record->first.c_str(), map.first.c_str())
        }  // end does not begin with a known prefix
      }    // end iteration over directory mappings
    }

    // if this is a fragment, check for delection
    if (is_fragment && clear_fragments_)
    {
      madara_logger_ptr_log(madara::logger::global_logger.get(),
          logger::LOG_MINOR,
          "FragmentsToFilesFilter::filter: "
          "removing variable %s\n",
          record->first.c_str())

          record = records.erase(record);
    }  // end if is fragment and clear fragments enabled
    else
    {
      madara_logger_ptr_log(madara::logger::global_logger.get(),
          logger::LOG_MINOR,
          "FragmentsToFilesFilter::filter: "
          "%s is not being deleted\n",
          record->first.c_str())++ record;
    }  // end no clear fragments needed
  }    // end iteration over incoming records

  if (last_file_path != "")
  {
    if (utility::file_from_fragments(last_file_path, last_crc, true, false))
    {
      madara_logger_ptr_log(madara::logger::global_logger.get(),
          logger::LOG_MAJOR,
          "FragmentsToFilesFilter::filter: "
          "SUCCESS: file %s is recreated\n",
          last_file_path.c_str())
    }
    else
    {
      madara_logger_ptr_log(madara::logger::global_logger.get(),
          logger::LOG_MAJOR,
          "FragmentsToFilesFilter::filter: "
          "FAIL: file %s is incomplete\n",
          last_file_path.c_str())
    }
  }
}  // end filter method

}  // end filters namespace
}  // end madara namespace
