
#include <string>
#include <vector>
#include <iostream>
#include <algorithm>
#include <sstream>
#include <assert.h>

#include "boost/filesystem.hpp"
#include "boost/crc.hpp"
#include "boost/property_tree/ptree.hpp"
#include "boost/property_tree/json_parser.hpp"

#include "madara/logger/Logger.h"
#include "madara/logger/GlobalLogger.h"
#include "madara/utility/Utility.h"
#include "madara/knowledge/KnowledgeBase.h"

namespace logger = madara::logger;
namespace utility = madara::utility;
namespace knowledge = madara::knowledge;

namespace ptree = boost::property_tree;

// path to what needs to be bundled
std::string source_path (".");
std::string dest_path (".");

std::vector <std::string> contributor_json;

class Contributor
{
public:
  std::string email;
  std::string organization;
  std::string path;
  std::string role;
  std::string title;
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
  int bytes;
  std::string description;
  std::string encoding;
  std::string endTime;
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
  std::string startTime;
  std::string title;
};

template<typename Fun, typename T>
auto for_each_field(Fun &&fun, T &&val) -> madara::enable_if_same_decayed<T, Resource>
{
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
  std::string endTime;
  std::string id;
  std::vector <std::string> keywords;
  std::vector <License> licenses;
  std::string name;
  std::vector <Resource> resources;
  std::vector <Source> sources;
  std::string startTime;
  std::string title;
  std::string version;
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
  

// handle command line arguments
void handle_arguments (int argc, char ** argv)
{
  for (int i = 1; i < argc; ++i)
  {
    std::string arg1 (argv[i]);

    if (arg1 == "-c" || arg1 == "--contributor")
    {
      if (i + 1 < argc)
      {
        Contributor contributor;
        ptree::ptree reader;
        std::stringstream buffer;
        buffer << argv[i + 1];

        ptree::json_parser::read_json (buffer, reader);

        if (reader.count ("email") == 1)
        {
          contributor.email = reader.get<std::string> ("email");
        }
        if (reader.count ("organization") == 1)
        {
          contributor.organization = reader.get<std::string> ("organization");
        }
        if (reader.count ("path") == 1)
        {
          contributor.path = reader.get<std::string> ("path");
        }
        if (reader.count ("role") == 1)
        {
          contributor.role = reader.get<std::string> ("role");
        }
        if (reader.count ("title") == 1)
        {
          contributor.title = reader.get<std::string> ("title");
        }
        // contributor.organization = reader.get<std::string> ("organization");
        // contributor.path = reader.get<std::string> ("path");
        // contributor.role = reader.get<std::string> ("role");
        // contributor.title = reader.get<std::string> ("title");

        manifest.contributors.push_back (contributor);
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
    else if (arg1 == "-s" || arg1 == "--source-path")
    {
      if (i + 1 < argc)
      {
        source_path = argv[i + 1];
      }

      ++i;
    }
    else
    {
      madara_logger_ptr_log (logger::global_logger.get (), logger::LOG_ALWAYS,
        "\nProgram summary for %s [options] [Logic]:\n\n" \
        "Creates digests for MADARA bundles.\n\noptions:\n" \
        "  [-c|--contributor json]  json of contributor info\n" \
        "  [-d|--dest-path path]    directory to generate manifest to\n" \
        "  [-h|--help]              print help menu (i.e., this menu)\n" \
        "  [-s|--source-path path]  directory to read files from\n" \
        "\n",
        argv[0]);
      exit (0);
    }
  }
}

int main (int argc, char ** argv)
{
  handle_arguments (argc, argv);

  knowledge::KnowledgeBase kb;

  kb.set ("datapackage",
    knowledge::KnowledgeRecord (knowledge::Any (manifest)));

  kb.print ();

  std::string datapackage = 
    kb.get ("datapackage").to_any ().to_json ();

  
  madara_logger_ptr_log (logger::global_logger.get (), logger::LOG_ALWAYS,
    "datapackage.json:\n%s\n",
    datapackage.c_str ());

  return 0;
}

