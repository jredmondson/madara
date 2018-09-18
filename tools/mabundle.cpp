
#include <string>
#include <vector>
#include <iostream>
#include <algorithm>
#include <sstream>
#include <assert.h>
#include <ctime>
#include <random>

#include "boost/filesystem.hpp"
#include "boost/crc.hpp"
#include "boost/property_tree/ptree.hpp"
#include "boost/property_tree/json_parser.hpp"

#include "madara/logger/Logger.h"
#include "madara/logger/GlobalLogger.h"
#include "madara/utility/Utility.h"
#include "madara/knowledge/KnowledgeBase.h"
#include "madara/Boost.h"

namespace logger = madara::logger;
namespace utility = madara::utility;
namespace knowledge = madara::knowledge;

namespace filesystem = boost::filesystem;
namespace ptree = boost::property_tree;

// path to what needs to be bundled
std::string source_path (".");
std::string dest_path (".");

std::vector <std::string> sources_json;

knowledge::CheckpointSettings checkpoint_settings;

bool debug = false;

class Contributor
{
public:

  std::string email;
  std::string organization;
  std::string path;
  std::string role;
  std::string title;

  inline void read (const std::string & input)
  {
    ptree::ptree reader;
    std::stringstream buffer;
    buffer << input;

    ptree::json_parser::read_json (buffer, reader);

    if (reader.count ("email") == 1)
    {
      email = reader.get<std::string> ("email");
    }
    if (reader.count ("organization") == 1)
    {
      organization = reader.get<std::string> ("organization");
    }
    if (reader.count ("path") == 1)
    {
      path = reader.get<std::string> ("path");
    }
    if (reader.count ("role") == 1)
    {
      role = reader.get<std::string> ("role");
    }
    if (reader.count ("title") == 1)
    {
      title = reader.get<std::string> ("title");
    }
  }

  inline ptree::ptree write (void)
  {
    ptree::ptree root;

    root.put ("email", email);
    root.put ("organization", organization);
    root.put ("path", path);
    root.put ("role", role);
    root.put ("title", title);

    return root;
  }
};

template<typename Fun, typename T>
auto for_each_field(Fun &&fun, T &&val) -> madara::enable_if_same_decayed<T, Contributor>
{
  fun("email", val.email);
  fun("organization", val.organization);
  fun("path", val.path);
  fun("role", val.role);
  fun("title", val.title);
}

class License 
{
public:
  std::string name;
  std::string path;
  std::string title;

  inline void read (const std::string & input)
  {
    ptree::ptree reader;
    std::stringstream buffer;
    buffer << input;

    ptree::json_parser::read_json (buffer, reader);

    if (reader.count ("name") == 1)
    {
      name = reader.get<std::string> ("name");
    }
    if (reader.count ("path") == 1)
    {
      path = reader.get<std::string> ("path");
    }
    if (reader.count ("title") == 1)
    {
      title = reader.get<std::string> ("title");
    }
  }
};

template<typename Fun, typename T>
auto for_each_field(Fun &&fun, T &&val) -> madara::enable_if_same_decayed<T, License>
{
  fun("name", val.name);
  fun("path", val.path);
  fun("title", val.title);
}

class Source
{
public:
  std::string email;
  std::string path;
  std::string title;

  inline void read (const std::string & input)
  {
    ptree::ptree reader;
    std::stringstream buffer;
    buffer << input;

    ptree::json_parser::read_json (buffer, reader);

    if (reader.count ("email") == 1)
    {
      email = reader.get<std::string> ("email");
    }
    if (reader.count ("path") == 1)
    {
      path = reader.get<std::string> ("path");
    }
    if (reader.count ("title") == 1)
    {
      title = reader.get<std::string> ("title");
    }
  }
};

template<typename Fun, typename T>
auto for_each_field(Fun &&fun, T &&val) -> madara::enable_if_same_decayed<T, Source>
{
  fun("email", val.email);
  fun("path", val.path);
  fun("title", val.title);
}

class Resource
{
public:
  long bytes = 0;
  std::string description;
  std::string encoding;
  long endTime = 0;
  std::string flightName;
  std::string format;
  std::string hash;
  std::vector <License> licenses;
  std::string location;
  std::string mediatype;
  std::string name;
  std::string notes;
  std::string path;
  std::string robotName;
  std::string schema;
  std::vector <Source> sources;
  long startTime = 0;
  std::string title;


  inline void read (const std::string & input)
  {
    ptree::ptree reader;
    std::stringstream buffer;
    buffer << input;

    ptree::json_parser::read_json (buffer, reader);

    if (reader.count ("bytes") == 1)
    {
      bytes = reader.get<long> ("bytes");
    }
    if (reader.count ("description") == 1)
    {
      description = reader.get<std::string> ("description");
    }
    if (reader.count ("encoding") == 1)
    {
      encoding = reader.get<std::string> ("encoding");
    }
    if (reader.count ("endTime") == 1)
    {
      endTime = reader.get<long> ("endTime");
    }
    if (reader.count ("flightName") == 1)
    {
      flightName = reader.get<std::string> ("flightName");
    }
    if (reader.count ("format") == 1)
    {
      format = reader.get<std::string> ("format");
    }
    if (reader.count ("hash") == 1)
    {
      hash = reader.get<std::string> ("hash");
    }
    if (reader.count ("location") == 1)
    {
      location = reader.get<std::string> ("location");
    }
    if (reader.count ("mediatype") == 1)
    {
      mediatype = reader.get<std::string> ("mediatype");
    }
    if (reader.count ("name") == 1)
    {
      name = reader.get<std::string> ("name");
    }
    if (reader.count ("notes") == 1)
    {
      notes = reader.get<std::string> ("notes");
    }
    if (reader.count ("path") == 1)
    {
      path = reader.get<std::string> ("path");
    }
    if (reader.count ("robotName") == 1)
    {
      robotName = reader.get<std::string> ("robotName");
    }
    if (reader.count ("schema") == 1)
    {
      schema = reader.get<std::string> ("schema");
    }
    if (reader.count ("startTime") == 1)
    {
      startTime = reader.get<long> ("startTime");
    }
    if (reader.count ("title") == 1)
    {
      title = reader.get<std::string> ("title");
    }
  }
};

template<typename Fun, typename T>
auto for_each_field(Fun &&fun, T &&val) -> madara::enable_if_same_decayed<T, Resource>
{
  fun("bytes", val.bytes);
  fun("description", val.description);
  fun("encoding", val.encoding);
  fun("endTime", val.endTime);
  fun("flightName", val.flightName);
  fun("format", val.format);
  fun("hash", val.hash);
  fun("licenses", val.licenses);
  fun("mediatype", val.mediatype);
  fun("name", val.name);
  fun("notes", val.notes);
  fun("path", val.path);
  fun("robotName", val.robotName);
  fun("schema", val.schema);
  fun("sources", val.sources);
  fun("startTime", val.startTime);
  fun("title", val.title);
}

// class for holding data package info
class DataPackage
{
public:
  std::vector <Contributor> contributors;
  std::string created;
  std::string description;
  long endTime = 0;
  std::string id;
  std::vector <std::string> keywords;
  std::vector <License> licenses;
  std::string name;
  std::vector <Resource> resources;
  std::vector <Source> sources;
  long startTime = 0;
  std::string title;
  std::string version;

  inline void read (const std::string & input)
  {
    ptree::ptree reader;
    std::stringstream buffer;
    buffer << input;

    ptree::json_parser::read_json (buffer, reader);

    if (reader.count ("created") == 1)
    {
      created = reader.get<std::string> ("created");
    }
    if (reader.count ("description") == 1)
    {
      description = reader.get<std::string> ("description");
    }
    if (reader.count ("endTime") == 1)
    {
      endTime = reader.get<long> ("endTime");
    }
    if (reader.count ("id") == 1)
    {
      id = reader.get<std::string> ("id");
    }
    if (reader.count ("name") == 1)
    {
      name = reader.get<std::string> ("name");
    }
    if (reader.count ("startTime") == 1)
    {
      startTime = reader.get<long> ("startTime");
    }
    if (reader.count ("title") == 1)
    {
      title = reader.get<std::string> ("title");
    }
  }
};

template<typename Fun, typename T>
auto for_each_field(Fun &&fun, T &&val) -> madara::enable_if_same_decayed<T, DataPackage>
{
  fun("contributors", val.contributors);
  fun("created", val.created);
  fun("description", val.description);
  fun("endTime", val.endTime);
  fun("id", val.id);
  fun("keywords", val.keywords);
  fun("licenses", val.licenses);
  fun("name", val.name);
  fun("resources", val.resources);
  fun("sources", val.sources);
  fun("startTime", val.startTime);
  fun("title", val.title);
  fun("version", val.version);
}


DataPackage manifest;

std::vector <Source> sources;
  

std::string get_mediatype (const std::string & format)
{
  if (format == "csv")
  {
    return "text/csv";
  }
  else if (format == "gif")
  {
    return "image/gif";
  }
  else if (format == "html")
  {
    return "text/html";
  }
  else if (format == "jpeg")
  {
    return "image/jpeg";
  }
  else if (format == "mp4")
  {
    return "application/mp4";
  }
  else if (format == "ogg")
  {
    return "audio/ogg";
  }
  else if (format == "pdf")
  {
    return "application/pdf";
  }
  else if (format == "png")
  {
    return "image/png";
  }
  else if (format == "rtf")
  {
    return "doc/rtf";
  }
  else if (format == "xml")
  {
    return "application/xml";
  }
  else if (format == "zip")
  {
    return "application/zip";
  }
  else
  {
    return "application/octet-stream";
  }
}

// handle command line arguments
void handle_arguments (int argc, char ** argv)
{
  for (int i = 1; i < argc; ++i)
  {
    std::string arg1 (argv[i]);

    if (arg1 == "-ac" || arg1 == "--contributor")
    {
      if (i + 1 < argc)
      {
        // we add contributor to sources and contributors
        Contributor contributor;
        Source new_source;
        new_source.read (argv[i + 1]);

        contributor.read (argv[i + 1]);

        sources.push_back (new_source);
        manifest.contributors.push_back (contributor);
      }

      ++i;
    }
    else if (arg1 == "-as" || arg1 == "--source")
    {
      if (i + 1 < argc)
      {
        Source new_source;
        new_source.read (argv[i + 1]);
        
        sources.push_back (new_source);
      }

      ++i;
    }
    else if (arg1 == "-d" || arg1 == "--dest-path")
    {
      if (i + 1 < argc)
      {
        dest_path = argv[i + 1];
      }

      ++i;
    }
    else if (arg1 == "--debug")
    {
      debug = true;
    }
    else if (arg1 == "-e" || arg1 == "--description")
    {
      if (i + 1 < argc)
      {
        manifest.description = argv[i + 1];
      }

      ++i;
    }
    else if (arg1 == "-k" || arg1 == "--keyword")
    {
      if (i + 1 < argc)
      {
        manifest.keywords.push_back (argv[i + 1]);
      }

      ++i;
    }
    else if (arg1 == "-l" || arg1 == "--license")
    {
      if (i + 1 < argc)
      {
        License license;
        license.read (argv[i + 1]);
        
        manifest.licenses.push_back (license);
      }

      ++i;
    }
    else if (arg1 == "-ls" || arg1 == "--load-size")
    {
      if (i + 1 < argc)
      {
        std::stringstream buffer (argv[i + 1]);
        buffer >> checkpoint_settings.buffer_size;
      }

      ++i;
    }
    else if (arg1 == "-n" || arg1 == "--name")
    {
      if (i + 1 < argc)
      {
        manifest.name = argv[i + 1];
      }

      ++i;
    }
    else if (arg1 == "-s" || arg1 == "--source-path")
    {
      if (i + 1 < argc)
      {
        source_path = argv[i + 1];
      }

      ++i;
    }
    else if (arg1 == "-t" || arg1 == "--title")
    {
      if (i + 1 < argc)
      {
        manifest.title = argv[i + 1];
      }

      ++i;
    }
    else if (arg1 == "-v" || arg1 == "--version")
    {
      if (i + 1 < argc)
      {
        manifest.version = argv[i + 1];
      }

      ++i;
    }
    else
    {
      madara_logger_ptr_log (logger::global_logger.get (), logger::LOG_ALWAYS,
        "\nInvalid arg %s...\n" \
        "\nProgram summary for %s [options] [Logic]:\n\n" \
        "Creates digests for MADARA bundles.\n\noptions:\n" \
        "  [-ac|--contributor json] json of contributor info\n" \
        "  [-as|--source json]      json of source of resources\n" \
        "  [-d|--dest-path path]    directory to generate manifest to\n" \
        "  [-e|--description text]  description of bundle contents\n" \
        "  [-h|--help]              print help menu (i.e., this menu)\n" \
        "  [-k|--keyword word]      a keyword to associate with the bundle\n" \
        "  [-l|--license json]      json of a license\n" \
        "  [-ls|--load-size bytes]  size of buffer needed for file load\n" \
        "  [-s|--source-path path]  directory to read files from\n" \
        "  [--debug]                print datapackage.json\n" \
        "  [-t|--title title]       the manifest title\n" \
        "  [-v|--version MAJ.MIN.REV] sets version to MAJOR.MINOR.REV\n" \
        "\n",
        arg1.c_str (), argv[0]);
      exit (0);
    }
  }
}

template <typename T>
void read_resources (const std::string & path)
{
  T end, p;

  if (filesystem::is_directory (path))
  {
    for (p = T (path); p != end; ++p)
    {
      auto file_path = p->path ();
      if (filesystem::is_regular_file (file_path) &&
          !filesystem::is_directory (file_path))
      {
        Resource resource;

        // grab the filename
        std::string full_path = file_path.string ();

        // remove the path (we can't just use the filename () if recursive)
        std::string filename = full_path.erase (0, source_path.size () + 1);
        //resource_path = resource_path.erase (0, source_path.size () + 1);
        std::string only_filename = madara::utility::extract_filename (filename);
        std::string resource_path = full_path.substr (
          0, full_path.size () - only_filename.size () - 1);

        void * temp_buffer;
        size_t file_size;
        resource.name = file_path.stem ().string ();
        resource.format = file_path.extension ().string ();

        if (utility::read_file (filename, temp_buffer, file_size) == 0)
        {
          utility::ScopedArray<char> file_contents = (char *)temp_buffer;
          std::stringstream buffer;

          resource.path = resource_path;
          resource.bytes = (int)file_size;
          resource.mediatype = get_mediatype (resource.format);
          resource.sources = sources;
          boost::crc_32_type crc_32_hash;
          crc_32_hash.process_bytes (
            file_contents.get (), file_size);

          buffer << crc_32_hash.checksum ();
          buffer >> resource.hash;

          if (resource.format == ".stk" || resource.format == ".kb")
          {
            madara_logger_ptr_log (logger::global_logger.get (),
              logger::LOG_ALWAYS,
              "%s is a knowledge base. Attempting to read timestamps.\n",
              only_filename.c_str ());

            checkpoint_settings.filename = full_path;

            knowledge::KnowledgeBase kb;
            kb.load_context (checkpoint_settings);

            resource.startTime = (long)checkpoint_settings.initial_timestamp;
            resource.endTime = (long)checkpoint_settings.last_timestamp;

            if (manifest.startTime == 0 || resource.startTime < manifest.startTime)
            {
              manifest.startTime = resource.startTime;
            }

            if (resource.endTime > manifest.endTime)
            {
              manifest.endTime = resource.endTime;
            }
          }
        
        } // end if read file is successful

        manifest.resources.push_back (resource);
      } // end if is regular file
    } // end iterate through directory
  } // end if is directory
  else
  {
    madara_logger_ptr_log (logger::global_logger.get (),
      logger::LOG_ALWAYS,
      "directory check: %s doesn't exist\n",
      path.c_str ());
  }
}

int main (int argc, char ** argv)
{
  handle_arguments (argc, argv);

  manifest.sources = sources;

  knowledge::KnowledgeBase kb;

  auto now = std::chrono::system_clock::now ();
  auto in_time_t = std::chrono::system_clock::to_time_t (now);

  std::stringstream ss;
  ss << std::put_time(std::localtime (&in_time_t), "%Y-%M-%dT%H:%M:%S");
  manifest.created = ss.str();

  boost::mt19937 ran (std::random_device{} ());
  auto uuid = boost::uuids::basic_random_generator<boost::mt19937> (ran) ();
  auto uuid_str = boost::uuids::to_string (uuid);
  uuid_str.erase(std::remove (
    uuid_str.begin (), uuid_str.end (), '-'), uuid_str.end ());
  
  manifest.id = uuid_str;

  read_resources <filesystem::recursive_directory_iterator> (
    source_path + "/data/input");
  read_resources <filesystem::recursive_directory_iterator> (
    source_path + "/data/output");
  read_resources <filesystem::recursive_directory_iterator> (
    source_path + "/data/processed");
  read_resources <filesystem::recursive_directory_iterator> (
    source_path + "/bin");
  read_resources <filesystem::recursive_directory_iterator> (
    source_path + "/doc");
  read_resources <filesystem::recursive_directory_iterator> (
    source_path + "/src");
  read_resources <filesystem::recursive_directory_iterator> (
    source_path + "/lib");
  read_resources <filesystem::recursive_directory_iterator> (
    source_path + "/scripts");

  kb.set ("datapackage",
    knowledge::KnowledgeRecord (knowledge::Any (manifest)));

  std::string datapackage = 
    kb.get ("datapackage").to_any ().to_json ();

  datapackage.erase (0, 30);
  datapackage.pop_back ();

  utility::string_replace (datapackage, "\n    ", "\n", true);

  if (debug)
  {
    madara_logger_ptr_log (logger::global_logger.get (), logger::LOG_ALWAYS,
      "datapackage.json:\n%s\n",
      datapackage.c_str ());
  }

  madara_logger_ptr_log (logger::global_logger.get (), logger::LOG_ALWAYS,
    "saving %s/databackage.json\n",
    dest_path.c_str ());

  utility::write_file (dest_path + "/datapackage.json",
    (void *)datapackage.c_str (), datapackage.length ());

  return 0;
}

