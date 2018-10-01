
#include <string>
#include <vector>
#include <iostream>
#include <algorithm>
#include <sstream>
#include <assert.h>

#include "boost/filesystem.hpp"
#include "boost/crc.hpp"

#include "madara/knowledge/KnowledgeBase.h"
#include "madara/threads/Threader.h"

#include "madara/utility/Utility.h"
#include "madara/filters/GenericFilters.h"
#include "madara/logger/GlobalLogger.h"

#include "madara/knowledge/containers/CircularBufferT.h"
#include "madara/knowledge/containers/CircularBufferConsumerT.h"
#include "madara/knowledge/containers/FlexMap.h"
#include "madara/knowledge/containers/Queue.h"

#include "madara/utility/EpochEnforcer.h"
#include "madara/knowledge/FileFragmenter.h"
#include "madara/knowledge/FileStreamer.h"

#ifdef _USE_SSL_
#include "madara/filters/ssl/AESBufferFilter.h"
#endif

#ifdef _USE_LZ4_
#include "madara/filters/lz4/LZ4BufferFilter.h"
#endif

// convenience namespaces and typedefs
namespace knowledge = madara::knowledge;
namespace containers = knowledge::containers;
namespace transport = madara::transport;
namespace utility = madara::utility;
namespace filters = madara::filters;
namespace logger = madara::logger;
namespace threads = madara::threads;
namespace filesystem = boost::filesystem;

typedef knowledge::KnowledgeRecord KnowledgeRecord;
typedef KnowledgeRecord::Integer Integer;
typedef std::vector<std::string> StringVector;

// default transport settings
std::string host("");
const std::string default_multicast("239.255.0.1:4150");
transport::QoSTransportSettings settings;

#ifdef _USE_SSL_
filters::AESBufferFilter ssl_filter;
#endif

#ifdef _USE_LZ4_
filters::LZ4BufferFilter lz4_filter;
#endif

// time to run for (-1 is forever)
double run_time(-1);

// by default send digests every 1 second
double digest_period(1.0);

// filename to save knowledge base as KaRL to
std::string save_location;

// filename to save knowledge base as JSON to
std::string save_json;

// filename to save knowledge base changes as checkpoint
std::string save_checkpoint;

// filename to save knowledge base as binary to
std::string save_binary;

// filename to save transport settings to
std::string save_transport;
std::string save_transport_prefix;
std::string save_transport_text;
std::string load_transport_prefix;

StringVector initfiles;
StringVector initbinaries;

// checkpoint settings
knowledge::CheckpointSettings load_checkpoint_settings;
knowledge::CheckpointSettings save_checkpoint_settings;

// the prefix to use
std::string prefix("agent.0");

// bandwidth limits
Integer send_bandwidth(-1);
Integer total_bandwidth(-1);

// buffer size
size_t requests_buffer_size(50);

// monitor for shutdown requests
containers::Integer shutdown_request;
containers::Integer current_send_bandwidth;
containers::Integer current_total_bandwidth;

// bandwidth monitor to check send data usage

transport::BandwidthMonitor bandwidth_monitor(2);

// delete the kb entries
bool delete_kb_entries(false);

struct Request
{
  std::string sandbox;
  std::string filename;
  bool all_files;
  Integer last_modified;
  std::string requester;
  std::vector<KnowledgeRecord::Integer> fragments;
};

struct DeleteRequest
{
  std::string sandbox;
  std::string filename;
  bool all_files;
  std::string requester;
};

template<typename Fun, typename T>
auto for_each_field(Fun&& fun, T&& val)
    -> madara::enable_if_same_decayed<T, Request>
{
  fun("sandbox", val.sandbox);
  fun("filename", val.filename);
  fun("all_files", val.all_files);
  fun("last_modified", val.last_modified);
  fun("requester", val.requester);
  fun("fragments", val.fragments);
}

template<typename Fun, typename T>
auto for_each_field(Fun&& fun, T&& val)
    -> madara::enable_if_same_decayed<T, DeleteRequest>
{
  fun("sandbox", val.sandbox);
  fun("filename", val.filename);
  fun("all_files", val.all_files);
  fun("requester", val.requester);
}

bool is_valid_filename(const std::string& filename)
{
  bool result = true;

  if (filename.find("../") != std::string::npos)
  {
    result = false;
    madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_ALWAYS,
        "is_valid_filename: BAD FILENAME: "
        "filename %s has directory changes. LIKELY MALICIOUS ATTACK\n",
        filename.c_str());
  }
  else if (filename.find("//") != std::string::npos)
  {
    result = false;
    madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_ALWAYS,
        "is_valid_filename: BAD FILENAME: "
        "filename %s attempts // directory address. LIKELY MALICIOUS ATTACK\n",
        filename.c_str());
  }
  else if (utility::begins_with(filename, "/"))
  {
    result = false;
    madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_ALWAYS,
        "is_valid_filename: BAD FILENAME: "
        "filename %s starts with /. LIKELY MALICIOUS ATTACK\n",
        filename.c_str());
  }

  return result;
}

void send_file(knowledge::KnowledgeBase& kb, const std::string& operation,
    const std::string& files_prefix, const std::string& sandbox_path,
    const std::string& filename,
    const std::vector<KnowledgeRecord::Integer>& valid_fragments)
{
  std::string filepath = sandbox_path + "/" + filename;
  std::string contents_key = filename + ".contents";
  std::string crc_key = filename + ".crc";

  madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_ALWAYS,
      "send_file: %s: "
      "requested file %s is being read.\n",
      operation.c_str(), filepath.c_str());

  // break the file into fragments
  // knowledge::FileFragmenter fragmenter (
  //   sandbox_path + "/" + filename);
  knowledge::FileStreamer streamer(files_prefix + "." + filename, filepath, kb);

  size_t num_fragments = streamer.get_size() / 60000;

  if (streamer.get_size() % 60000 > 0)
  {
    num_fragments++;
  }

  madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_ALWAYS,
      "send_file: %s: "
      "requested file %s is being fragmented into %s.\n",
      operation.c_str(), filepath.c_str(), contents_key.c_str());

  // send any lingering modifieds
  kb.send_modifieds();

  madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_ALWAYS,
      "send_file: %s: "
      "total frags: %d, sending: %d.\n",
      operation.c_str(), (int)num_fragments,
      (int)(valid_fragments.size() ? valid_fragments.size() : num_fragments));

  // counter for the current request_frag
  size_t cur_frag = 0;

  // iterate over the fragments and send each one in its own packet
  for (size_t i = 0; i < num_fragments; ++i)
  {
    if (valid_fragments.size() > 0)
    {
      if (cur_frag == valid_fragments.size())
      {
        madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_ALWAYS,
            "send_file: %s: %s:"
            "no more fragments to send. Cleaning up request.\n",
            operation.c_str(), filename.c_str());

        break;
      }

      if (valid_fragments[cur_frag] >= 0 &&
          (size_t)valid_fragments[cur_frag] < num_fragments)
      {
        i = (size_t)valid_fragments[cur_frag];
      }
      else
      {
        madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_ALWAYS,
            "send_file: %s: %s: "
            "frag %d is out of bounds. Cleaning up request.\n",
            operation.c_str(), filename.c_str(),
            (int)valid_fragments[cur_frag]);

        break;
      }
    }

    // if this is a full file request or this is a useful fragment
    // then send the fragment
    // if (valid_fragments.size () == 0 ||
    //     i == (size_t)valid_fragments[cur_frag])
    {
      // modify the fragment, the num fragments, and the CRC
      size_t frag_size = streamer.load(i);
      streamer.modify();

      madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_ALWAYS,
          "send_file: %s: %s: "
          "sending frag %d: %d B.\n",
          operation.c_str(), filename.c_str(), (int)i, (int)frag_size);

      if (send_bandwidth > 0)
      {
        while (
            bandwidth_monitor.get_bytes_per_second() > (uint64_t)send_bandwidth)
        {
          madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_ALWAYS,
              "send_file: %s: %s: "
              "bandwidth violation: %" PRIu64 " / %" PRId64 "."
              " Sleeping for 1s\n",
              operation.c_str(), filename.c_str(),
              bandwidth_monitor.get_bytes_per_second(), send_bandwidth);

          utility::sleep(1.0);
        }  // end managing sleeps
      }    // end bandwidth handling

      kb.send_modifieds();
      bandwidth_monitor.add(frag_size);

      // clear any sent fragments
      streamer.clear_fragments();

      ++cur_frag;
    }
  }  // end fragment loop
}

struct Sandbox
{
  std::string id;
  std::string path;
  containers::String name;
  containers::String description;
  bool recursive;
  bool valid;
  containers::Map files;
  knowledge::KnowledgeBase kb;

  inline void read(containers::FlexMap& map, const std::string sandbox_id,
      knowledge::KnowledgeBase& actual_kb)
  {
    kb = actual_kb;
    id = sandbox_id;
    path = map.to_string();
    recursive = map["recursive"].is_true();
    valid = filesystem::is_directory(path);

    description.set_name(prefix + ".sandbox." + id + ".description", kb);
    name.set_name(prefix + ".sandbox." + id + ".name", kb);
    files.set_name(prefix + ".sandbox." + id + ".file", kb);

    description = map["description"].to_string();
    name = map["name"].to_string();
  }

  template<typename T>
  inline std::vector<std::string> build_file_vector(void)
  {
    T end, p;
    std::vector<std::string> result;

    for (p = T(path); p != end; ++p)
    {
      auto file_path = p->path();

      madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_ALWAYS,
          "build_file_vector: checking file %s\n", file_path.string().c_str());

      if (filesystem::is_regular_file(file_path))
      {
        madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_ALWAYS,
            "build_file_vector: adding file %s\n", file_path.string().c_str());

        // grab the filename
        result.push_back(file_path.string());
      }
    }

    return result;
  }

  template<typename T>
  inline void refresh_iterate(void)
  {
    T end, p;

    for (p = T(path); p != end; ++p)
    {
      auto file_path = p->path();
      if (filesystem::is_regular_file(file_path))
      {
        // grab the filename
        std::string full_path = file_path.string();

        // remove the path (we can't just use the filename () if recursive)
        std::string filename = full_path.erase(0, path.size() + 1);

        // get the size and set this in the map
        Integer size = filesystem::file_size(file_path);
        Integer last_modified = filesystem::last_write_time(file_path);
        files.set(filename + ".size", size);
        files.set(filename + ".last_modified", last_modified);
      }
    }
  }

  template<typename T>
  inline void delete_iterate(void)
  {
    T end, p;

    madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_ALWAYS,
        "delete_iterate: deleting all files in %s\n", path.c_str());

    try
    {
      for (p = T(path); p != end; ++p)
      {
        auto file_path = p->path();

        madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_ALWAYS,
            "delete_iterate: checking file %s\n", file_path.string().c_str());

        if (filesystem::is_regular_file(file_path))
        {
          madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_ALWAYS,
              "delete_iterate: removing file %s\n", file_path.string().c_str());

          try
          {
            filesystem::remove(file_path);
          }
          catch (boost::filesystem::filesystem_error& e)
          {
            madara_logger_ptr_log(logger::global_logger.get(),
                logger::LOG_ALWAYS,
                "delete_iterate: caught exception during remove %s: %s\n",
                file_path.string().c_str(), e.what());
          }
        }
        madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_ALWAYS,
            "delete_iterate: removed file %s\n", file_path.string().c_str());
      }
    }
    catch (...)
    {
      madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_ALWAYS,
          "delete_iterate: caught exception outside of a file remove %s\n");
    }
  }

  template<typename T>
  inline void send_iterate(void)
  {
    T end, p;

    for (p = T(path); p != end; ++p)
    {
      auto file_path = p->path();

      madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_ALWAYS,
          "send_iterate: checking file %s\n", file_path.string().c_str());

      if (filesystem::is_regular_file(file_path))
      {
        madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_ALWAYS,
            "send_iterate: sending file %s\n", file_path.string().c_str());

        // grab the filename
        std::string full_path = file_path.string();

        // remove the path (we can't just use the filename () if recursive)
        std::string filename = full_path.erase(0, path.size() + 1);

        send_file(kb, "TRANSFER", files.get_name(), path, filename, {});
      }
    }
  }

  inline void modify(void)
  {
    name.modify();
    description.modify();
    refresh_files();
  }

  inline void refresh_files(void)
  {
    if (valid)
    {
      if (recursive)
      {
        refresh_iterate<filesystem::recursive_directory_iterator>();
      }
      else
      {
        refresh_iterate<filesystem::directory_iterator>();
      }
    }
  }

  inline void send_all_files(void)
  {
    if (valid)
    {
      if (recursive)
      {
        send_iterate<filesystem::recursive_directory_iterator>();
      }
      else
      {
        send_iterate<filesystem::directory_iterator>();
      }
    }
  }

  inline void delete_all_files(void)
  {
    if (valid)
    {
      std::vector<std::string> filenames;
      if (recursive)
      {
        filenames =
            build_file_vector<filesystem::recursive_directory_iterator>();
      }
      else
      {
        filenames = build_file_vector<filesystem::directory_iterator>();
      }

      for (auto filename : filenames)
      {
        madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_ALWAYS,
            "delete_all_files: deleting file %s\n", filename.c_str());

        std::string temp = prefix + ".sandbox." + this->id + ".file.";
        temp += filename.substr(this->path.size() + 1);
        filesystem::remove(filename);

        madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_ALWAYS,
            "delete_all_files: deleting keys with %s prefix in KB\n",
            temp.c_str());

        {
          knowledge::ContextGuard guard(kb);

          // delete every key that starts with temp
          knowledge::KnowledgeMap keys = kb.to_map(temp);

          madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_ALWAYS,
              "process_requests: deleting keys with %s prefix in KB\n",
              temp.c_str());

          for (auto entry : keys)
          {
            madara_logger_ptr_log(logger::global_logger.get(),
                logger::LOG_ALWAYS, "process_requests: deleting key %s\n",
                entry.first.c_str());

            kb.clear(entry.first);
          }
        }
      }
    }
  }
};

/**
 * Filter for receiving and handling requests from remote users
 **/

class HandleRequests : public filters::AggregateFilter
{
public:
  virtual ~HandleRequests() = default;

  /// requests from remote users
  containers::CircularBufferT<Request> requests;
  containers::CircularBufferT<DeleteRequest> delete_requests;

  /**
   * Handle requests from other remote users
   **/
  virtual void filter(knowledge::KnowledgeMap& records,
      const transport::TransportContext& transport_context,
      knowledge::Variables&)
  {
    madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_ALWAYS,
        "HandleRequests: Processing packet with %d records\n",
        (int)records.size());

    current_send_bandwidth = (Integer)transport_context.get_send_bandwidth();

    current_total_bandwidth =
        (Integer)transport_context.get_receive_bandwidth();

    std::string sync_sandbox_prefix = prefix + ".sync.sandbox.";
    std::string delete_sandbox_prefix = prefix + ".delete.sandbox.";

    knowledge::KnowledgeMap::iterator record;

    Request request;
    DeleteRequest delete_request;

    request.requester = transport_context.get_originator();
    delete_request.requester = request.requester;

    // record = records.lower_bound (prefix);
    record = records.begin();

    madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_ALWAYS,
        "HandleRequests: Iterating from record %s from requester %s\n",
        record->first.c_str(), request.requester.c_str());

    for (; record != records.end(); ++record)
    {
      bool bad_record = false;
      if (utility::begins_with(record->first, sync_sandbox_prefix))
      {
        if (utility::ends_with(record->first, "all_files"))
        {
          size_t sandbox_end =
              record->first.find(".all_files", sync_sandbox_prefix.size());

          request.sandbox = record->first.substr(sync_sandbox_prefix.size(),
              sandbox_end - sync_sandbox_prefix.size());

          madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_ALWAYS,
              "HandleRequests: Request: sandbox=%s "
              "all_files=1\n",
              request.sandbox.c_str());

          request.all_files = true;

          requests.add(request);
        }     // end all_files
        else  // has sandbox prefix but is not all_files
        {
          // try to split the sandbox name and the file name from the rest
          size_t sandbox_end =
              record->first.find(".file.", sync_sandbox_prefix.size());

          if (sandbox_end != std::string::npos)
          {
            request.sandbox = record->first.substr(sync_sandbox_prefix.size(),
                sandbox_end - sync_sandbox_prefix.size());
            request.filename = record->first.substr(sandbox_end + 6);

            if (!is_valid_filename(request.filename))
            {
              madara_logger_ptr_log(logger::global_logger.get(),
                  logger::LOG_ALWAYS,
                  "HandleRequests: DROPPING REQUEST: sandbox=%s "
                  "file=%s\n",
                  request.sandbox.c_str(), request.filename.c_str());
              bad_record = true;
              break;
            }

            request.all_files = false;

            // if the type is an integer array, then these are fragments to try
            // to send.
            if (record->second.is_array_type())
            {
              request.fragments = record->second.to_integers();

              // sort the list to put it in a format we can use
              std::sort(request.fragments.begin(), request.fragments.end());
              request.last_modified = 0;
            }
            else
            {
              request.last_modified = record->second.to_integer();
            }

            madara_logger_ptr_log(logger::global_logger.get(),
                logger::LOG_ALWAYS,
                "HandleRequests: Request: sandbox=%s "
                "file=%s, last_mod=%d\n",
                request.sandbox.c_str(), request.filename.c_str(),
                (int)request.last_modified);

          }  // end if sandbox .file was located
          else
          {
            bad_record = true;

            madara_logger_ptr_log(logger::global_logger.get(),
                logger::LOG_ERROR,
                "HandleRequests: ERROR: parameter %s was neither all files "
                "or a specific file. Check for malformed request.\n",
                record->first.c_str());
          }  // end else of sandbox end check

          if (!bad_record)
          {
            madara_logger_ptr_log(logger::global_logger.get(),
                logger::LOG_ERROR, "HandleRequests: Enqueueing request.\n");

            requests.add(request);
          }  // end if !bad_record
        }    // end has sandbox prefix for loop
      }      // end if begins with sync prefix
      else if (utility::begins_with(record->first, delete_sandbox_prefix))
      {
        madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_ALWAYS,
            "HandleRequests: DeleteRequest: sandbox=%s, "
            "match=%s\n",
            record->first.c_str(), delete_sandbox_prefix.c_str());

        if (utility::ends_with(record->first, ".all_files"))
        {
          size_t sandbox_end =
              record->first.find(".all_files", delete_sandbox_prefix.size());

          delete_request.sandbox =
              record->first.substr(delete_sandbox_prefix.size(),
                  sandbox_end - delete_sandbox_prefix.size());

          madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_ALWAYS,
              "HandleRequests: DeleteRequest: sandbox=%s, "
              "all_files=1\n",
              delete_request.sandbox.c_str());

          delete_request.all_files = true;

          delete_requests.add(delete_request);
        }     // end all_files
        else  // has sandbox prefix but is not all_files
        {
          // try to split the sandbox name and the file name from the rest
          size_t sandbox_end =
              record->first.find(".file.", delete_sandbox_prefix.size());

          if (sandbox_end != std::string::npos)
          {
            delete_request.sandbox =
                record->first.substr(delete_sandbox_prefix.size(),
                    sandbox_end - delete_sandbox_prefix.size());
            delete_request.filename = record->first.substr(sandbox_end + 6);

            if (!is_valid_filename(delete_request.filename))
            {
              madara_logger_ptr_log(logger::global_logger.get(),
                  logger::LOG_ALWAYS,
                  "HandleRequests: DROPPING DELETE REQUEST: sandbox=%s "
                  "file=%s\n",
                  delete_request.sandbox.c_str(),
                  delete_request.filename.c_str());

              bad_record = true;
              break;
            }

            delete_request.all_files = false;

            madara_logger_ptr_log(logger::global_logger.get(),
                logger::LOG_ALWAYS,
                "HandleRequests: DeleteRequest: sandbox=%s "
                "file=%s\n",
                delete_request.sandbox.c_str(),
                delete_request.filename.c_str());

          }  // end if sandbox .file was located
          else
          {
            bad_record = true;

            madara_logger_ptr_log(logger::global_logger.get(),
                logger::LOG_ERROR,
                "HandleRequests: ERROR: parameter %s was neither all files "
                "or a specific file. Check for malformed delete request.\n",
                record->first.c_str());
          }  // end else of sandbox end check

          if (!bad_record)
          {
            madara_logger_ptr_log(logger::global_logger.get(),
                logger::LOG_ALWAYS,
                "HandleRequests: Enqueueing delete request.\n");

            delete_requests.add(delete_request);
          }  // end if !bad_record
        }    // end has sandbox prefix for loop
      }
    }  // end for loop over records with our prefix

    records.clear();
  }  // end filter method
};   // end HandleRequests class

/**
 * Process requests from the HandleRequests filter
 **/
void process_requests(std::vector<Sandbox>& sandboxes,
    knowledge::KnowledgeBase& kb,
    containers::CircularBufferConsumerT<Request>& requests,
    containers::CircularBufferConsumerT<DeleteRequest>& delete_requests)
{
  knowledge::ContextGuard guard(kb);

  delete_requests.resize();
  requests.resize();

  madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_ALWAYS,
      "process_requests: Requests queue size: %d, "
      "DeleteRequests queue size: %d\n",
      (int)requests.remaining(), (int)delete_requests.remaining());

  while (requests.remaining() > 0)
  {
    // grab a request from the front of the queue
    Request request;
    requests.consume(request);

    madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_ALWAYS,
        "process_requests: Request: sandbox=%s "
        "file=%s, last_mod=%d\n",
        request.sandbox.c_str(), request.filename.c_str(),
        (int)request.last_modified)

        for (auto sandbox : sandboxes)
    {
      // if this is the sandbox
      if (sandbox.id == request.sandbox)
      {
        // check for last_modified time
        KnowledgeRecord last_modified =
            sandbox.files[request.filename + ".last_modified"];

        std::string filename = sandbox.path + "/" + request.filename;

        if (!request.all_files)
        {
          if (!filesystem::is_regular_file(filename))
          {
            madara_logger_ptr_log(logger::global_logger.get(),
                logger::LOG_ERROR,
                "process_requests: ERROR: "
                "requested file %s does not exists.\n",
                filename.c_str());

            break;
          }  // end not a regular file

          std::string operation;

          // is user requesting fragments of a file?
          if (request.fragments.size() > 0)
          {
            operation = "TRANSFER";
            madara_logger_ptr_log(logger::global_logger.get(),
                logger::LOG_ALWAYS,
                "process_requests: %s: "
                "handling request transfer of %d fragments of file %s.\n",
                operation.c_str(), (int)request.fragments.size(),
                filename.c_str());
          }
          else if (last_modified.is_valid() &&
                   last_modified > request.last_modified)
          {
            operation = "TRANSFER";
            madara_logger_ptr_log(logger::global_logger.get(),
                logger::LOG_ALWAYS,
                "process_requests: %s: "
                "handling request for full transfer of file %s "
                "(last_mod=%d).\n",
                operation.c_str(), filename.c_str(),
                (int)request.last_modified);
          }
          else
          {
            operation = "DROP";
            madara_logger_ptr_log(logger::global_logger.get(),
                logger::LOG_ALWAYS,
                "process_requests: %s: "
                "file %s (last_mod=%" PRId64 ") is not newer than request "
                "(last_mod=%" PRId64 ".\n",
                operation.c_str(), filename.c_str(), last_modified.to_integer(),
                request.last_modified);

            break;
          }

          // send the file or portions of the file
          send_file(kb, operation, sandbox.files.get_name(), sandbox.path,
              request.filename, request.fragments);
        }  // end if not request all files
        else
        {
          madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_ERROR,
              "process_requests: WARN: user requested sending all files in "
              "sandbox %s for dir %s. Could be a lot of data.\n",
              sandbox.id.c_str(), sandbox.path.c_str());

          sandbox.send_all_files();
        }  // end send all files in sandbox
      }    // if sandbox is a match
    }      // foreach sandbox
  }        // while requests.remaining () > 0

  while (delete_requests.remaining() > 0)
  {
    // grab a request from the front of the queue
    DeleteRequest delete_request;
    delete_requests.consume(delete_request);

    madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_ERROR,
        "process_requests: DeleteRequest: sandbox=%s file=%s\n",
        delete_request.sandbox.c_str(), delete_request.filename.c_str());

    for (auto sandbox : sandboxes)
    {
      // if this is the sandbox
      if (sandbox.id == delete_request.sandbox)
      {
        std::string filename = sandbox.path + "/" + delete_request.filename;

        if (!delete_request.all_files)
        {
          madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_ALWAYS,
              "process_requests: handling delete request for %s\n",
              filename.c_str());

          // check if the filename has ../ or // which is a malicious request

          if (filesystem::is_regular_file(filename))
          {
            std::string temp = prefix + ".sandbox." + sandbox.id + ".file.";
            temp += delete_request.filename;

            filesystem::remove(sandbox.path + "/" + delete_request.filename);

            {
              knowledge::ContextGuard guard(kb);

              // delete every key that starts with temp
              knowledge::KnowledgeMap keys = kb.to_map(temp);

              madara_logger_ptr_log(logger::global_logger.get(),
                  logger::LOG_ALWAYS,
                  "process_requests: deleting keys with %s prefix in KB\n",
                  temp.c_str());

              for (auto entry : keys)
              {
                madara_logger_ptr_log(logger::global_logger.get(),
                    logger::LOG_ALWAYS, "process_requests: deleting key %s\n",
                    entry.first.c_str());

                kb.clear(entry.first);
              }
            }
          }  // end if a regular file
          else if (filesystem::is_directory(filename))
          {
            filesystem::remove_all(
                sandbox.path + "/" + delete_request.filename);
          }  // end if a regular file
          else
          {
            madara_logger_ptr_log(logger::global_logger.get(),
                logger::LOG_ERROR,
                "process_requests: ERROR: "
                "requested file %s no longer exists.\n",
                filename.c_str());
          }  // end not a regular file
        }    // end if not request all files
        else
        {
          madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_ERROR,
              "process_requests: WARN: user requested deleting all files in "
              "sandbox %s for dir %s.\n",
              sandbox.id.c_str(), sandbox.path.c_str());

          sandbox.delete_all_files();
        }  // end send all files in sandbox
      }    // if sandbox is a match
    }      // foreach sandbox
  }        // while requests.remaining () > 0
}

// handle command line arguments
void handle_arguments(int argc, char** argv)
{
  for (int i = 1; i < argc; ++i)
  {
    std::string arg1(argv[i]);

    if (arg1 == "-b" || arg1 == "--broadcast")
    {
      if (i + 1 < argc)
      {
        settings.hosts.push_back(argv[i + 1]);
        settings.type = transport::BROADCAST;
      }
      ++i;
    }
    else if (arg1 == "-d" || arg1 == "--domain")
    {
      if (i + 1 < argc)
        settings.write_domain = argv[i + 1];

      ++i;
    }
    else if (arg1 == "-dp" || arg1 == "--digest-period")
    {
      if (i + 1 < argc)
      {
        std::stringstream buffer(argv[i + 1]);
        buffer >> digest_period;
      }

      ++i;
    }
    else if (arg1 == "-esb" || arg1 == "--send-bandwidth")
    {
      if (i + 1 < argc)
      {
        std::stringstream buffer(argv[i + 1]);
        buffer >> send_bandwidth;

        madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_ALWAYS,
            "Setting send bandwidth to %d b/s over 10s window\n",
            (int)send_bandwidth);
      }

      ++i;
    }
    else if (arg1 == "-etb" || arg1 == "--total-bandwidth")
    {
      if (i + 1 < argc)
      {
        std::stringstream buffer(argv[i + 1]);
        buffer >> total_bandwidth;

        // madara_logger_ptr_log (logger::global_logger.get (),
        //   logger::LOG_ALWAYS,
        //   "Setting total bandwidth to %d b/s over 10s window\n",
        //   (int)send_bandwidth);
      }

      ++i;
    }
    else if (arg1 == "-f" || arg1 == "--logfile")
    {
      if (i + 1 < argc)
      {
        logger::global_logger->add_file(argv[i + 1]);
      }

      ++i;
    }
    else if (arg1 == "-l" || arg1 == "--level")
    {
      if (i + 1 < argc)
      {
        int level;
        std::stringstream buffer(argv[i + 1]);
        buffer >> level;
        logger::global_logger->set_level(level);
      }

      ++i;
    }
    else if (arg1 == "-lt" || arg1 == "--load-transport")
    {
      if (i + 1 < argc)
      {
        if (load_transport_prefix == "")
          settings.load(argv[i + 1]);
        else
          settings.load(argv[i + 1], load_transport_prefix);
      }

      ++i;
    }
    else if (arg1 == "-ltp" || arg1 == "--load-transport-prefix")
    {
      if (i + 1 < argc)
      {
        load_transport_prefix = argv[i + 1];
      }

      ++i;
    }
    else if (arg1 == "-ltt" || arg1 == "--load-transport-text")
    {
      if (i + 1 < argc)
      {
        if (load_transport_prefix == "")
          settings.load_text(argv[i + 1]);
        else
          settings.load_text(argv[i + 1], load_transport_prefix);
      }

      ++i;
    }
    else if (arg1 == "-lz4" || arg1 == "--lz4")
    {
#ifdef _USE_LZ4_
      settings.add_filter(&lz4_filter);
#else
      madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_ERROR,
          "ERROR: parameter (-lz4|--lz4) requires feature lz4 to be "
          "compiled\n");
#endif
    }
    else if (arg1 == "-m" || arg1 == "--multicast")
    {
      if (i + 1 < argc)
      {
        settings.hosts.push_back(argv[i + 1]);
        settings.type = transport::MULTICAST;
      }
      ++i;
    }
    else if (arg1 == "-o" || arg1 == "--host")
    {
      if (i + 1 < argc)
        host = argv[i + 1];

      ++i;
    }
    else if (arg1 == "-p" || arg1 == "--prefix")
    {
      if (i + 1 < argc)
        prefix = argv[i + 1];

      ++i;
    }
    else if (arg1 == "-q" || arg1 == "--queue-length")
    {
      if (i + 1 < argc)
      {
        std::stringstream buffer(argv[i + 1]);
        buffer >> settings.queue_length;
      }

      ++i;
    }
    else if (arg1 == "-r" || arg1 == "--reduced")
    {
      settings.send_reduced_message_header = true;
    }
    else if (arg1 == "-rhz" || arg1 == "--read-hz")
    {
      if (i + 1 < argc)
      {
        std::stringstream buffer(argv[i + 1]);
        buffer >> settings.read_thread_hertz;
      }

      ++i;
    }
    else if (arg1 == "-rq" || arg1 == "--requests")
    {
      if (i + 1 < argc)
      {
        std::stringstream buffer(argv[i + 1]);
        buffer >> requests_buffer_size;
      }

      ++i;
    }
    else if (arg1 == "-s" || arg1 == "--save")
    {
      if (i + 1 < argc)
        save_location = argv[i + 1];

      ++i;
    }
    else if (arg1 == "-sj" || arg1 == "--save-json")
    {
      if (i + 1 < argc)
        save_json = argv[i + 1];

      ++i;
    }
    else if (arg1 == "-sb" || arg1 == "--save-binary")
    {
      if (i + 1 < argc)
        save_binary = argv[i + 1];

      ++i;
    }
    else if (arg1 == "-sc" || arg1 == "--save-checkpoint")
    {
      if (i + 1 < argc)
        save_checkpoint = argv[i + 1];

      ++i;
    }
    else if (arg1 == "-scp" || arg1 == "--save-checkpoint-prefix")
    {
      if (i + 1 < argc)
      {
        save_checkpoint_settings.prefixes.push_back(argv[i + 1]);
      }

      ++i;
    }
    else if (arg1 == "-ss" || arg1 == "--save-size")
    {
      if (i + 1 < argc)
      {
        std::stringstream buffer(argv[i + 1]);
        buffer >> save_checkpoint_settings.buffer_size;
      }

      ++i;
    }
    else if (arg1 == "-ssl" || arg1 == "--ssl")
    {
#ifdef _USE_SSL_
      if (i + 1 < argc)
      {
        ssl_filter.generate_key(argv[i + 1]);
        load_checkpoint_settings.buffer_filters.push_back(&ssl_filter);
        ++i;
      }
      else
      {
        madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_ERROR,
            "ERROR: parameter (-ssl|--ssl) requires password\n");
      }
#else
      madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_ERROR,
          "ERROR: parameter (-ssl|--ssl) requires feature ssl be compiled\n");
      ++i;
#endif
    }
    else if (arg1 == "-st" || arg1 == "--save-transport")
    {
      if (i + 1 < argc)
      {
        save_transport = argv[i + 1];
      }

      ++i;
    }
    else if (arg1 == "-stp" || arg1 == "--save-transport-prefix")
    {
      if (i + 1 < argc)
      {
        save_transport_prefix = argv[i + 1];
      }

      ++i;
    }
    else if (arg1 == "-stt" || arg1 == "--save-transport-text")
    {
      if (i + 1 < argc)
      {
        save_transport_text = argv[i + 1];
      }

      ++i;
    }
    else if (arg1 == "-sz" || arg1 == "--send-hz")
    {
      if (i + 1 < argc)
      {
        std::stringstream buffer(argv[i + 1]);
        buffer >> settings.max_send_hertz;
      }

      ++i;
    }
    else if (arg1 == "-u" || arg1 == "--udp")
    {
      if (i + 1 < argc)
      {
        settings.hosts.push_back(argv[i + 1]);
        settings.type = transport::UDP;
      }
      ++i;
    }
    else if (arg1 == "-v" || arg1 == "--version")
    {
      madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_ALWAYS,
          "MADARA version: %s\n", utility::get_version().c_str());
    }
    else if (arg1 == "-t" || arg1 == "--time")
    {
      if (i + 1 < argc)
      {
        std::stringstream buffer(argv[i + 1]);
        buffer >> run_time;
      }
      ++i;
    }
    else if (arg1 == "-tdp" || arg1 == "--transport-debug-prefix")
    {
      if (i + 1 < argc)
      {
        settings.debug_to_kb(argv[i + 1]);
      }

      ++i;
    }
    else if (arg1 == "--zmq" || arg1 == "--0mq")
    {
      if (i + 1 < argc)
      {
        settings.hosts.push_back(argv[i + 1]);
        settings.type = transport::ZMQ;
      }
      ++i;
    }
    else if (arg1 == "-0f" || arg1 == "--init-file")
    {
      if (i + 1 < argc)
      {
        initfiles.push_back(argv[i + 1]);
      }

      ++i;
    }
    else if (arg1 == "-0b" || arg1 == "--init-bin")
    {
      if (i + 1 < argc)
      {
        initbinaries.push_back(argv[i + 1]);
      }

      ++i;
    }
    else
    {
      madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_ALWAYS,
          "\nProgram summary for %s [options] [Logic]:\n\n"
          "Serves files up to clients within MFS-defined "
          "sandboxes.\n\noptions:\n"
          "  [-b|--broadcast ip:port] the broadcast ip to send and listen to\n"
          "  [-d|--domain domain]     the knowledge domain to send and listen "
          "to\n"
          "  [-esb|--send-bandwidth limit] bandwidth limit to enforce over a "
          "10s\n"
          "                           window. If the limit is violated, then "
          "the\n"
          "                           mfs agent will not send again until the "
          "send\n"
          "                           bandwidth is within an acceptable level "
          "again\n"
          "  [-etb|--total-bandwidth limit] bandwidth limit to enforce over a "
          "10s\n"
          "                           window. If the limit is violated, then "
          "the\n"
          "                           mfs agent will not send again until the "
          "total\n"
          "                           bandwidth is within an acceptable level "
          "again\n"
          "  [-f|--logfile file]      log to a file\n"
          "  [-h|--help]              print help menu (i.e., this menu)\n"
          "  [-l|--level level]       the logger level (0+, higher is higher "
          "detail)\n"
          "  [-lt|--load-transport file] a file to load transport settings "
          "from\n"
          "  [-ltp|--load-transport-prefix prfx] prefix of saved settings\n"
          "  [-ltt|--load-transport-text file] text file to load transport "
          "settings\n"
          "  [-lz4|--lz4]             add lz4 compression filter\n"
          "  [-m|--multicast ip:port] the multicast ip to send and listen to\n"
          "  [-o|--host hostname]     the hostname of this process "
          "(def:localhost)\n"
          "  [-p|--prefix prefix]     prefix of this agent / service (e.g. "
          "agent.0)\n"
          "  [-q|--queue-length size] size of network buffers in bytes\n"
          "  [-r|--reduced]           use the reduced message header\n"
          "  [-rhz|--read-hz hz]      hertz rate of read threads\n"
          "  [-rq|--requests size]    size of the requests queue (def 50)\n"
          "  [-s|--save file]         save the resulting knowledge base as "
          "karl\n"
          "  [-sb|--save-binary file] save the resulting knowledge base as a\n"
          "                           binary checkpoint\n"
          "  [-sc|--save-checkpoint file] save any changes by logics since "
          "initial\n"
          "                           loads as a checkpoint diff to the "
          "specified\n"
          "                           file in an appended layer\n"
          "  [-scp|--save-checkpoint-prefix prfx]\n"
          "                           prefix of knowledge to save in "
          "checkpoint\n"
          "  [-sj|--save-json file]   save the resulting knowledge base as "
          "JSON\n"
          "  [-ss|--save-size bytes]  size of buffer needed for file saves\n"
          "  [-ssl|--ssl pass]        add an ssl filter with a password\n"
          "  [-st|--save-transsport file] a file to save transport settings "
          "to\n"
          "  [-stp|--save-transport-prefix prfx] prefix to save settings at\n"
          "  [-stt|--save-transport-text file] text file to save settings to\n"
          "  [-sz|--send-hz hz]       maximum messages per second (inf by "
          "default)\n"
          "  [-t|--time time]         time to run this service (-1 is "
          "forever).\n"
          "  [-tdp|--transport-debug-prefix pfx] prefix in the knowledge base\n"
          "                           to save transport debug info\n"
          "  [-u|--udp ip:port]       the udp ips to send to (first is self to "
          "bind\n"
          "                           to) only runs once. If zero, hertz is "
          "infinite.\n"
          "                           If positive, hertz is that hertz rate.\n"
          "  [--zmq|--0mq proto://ip:port] a ZeroMQ endpoint to connect to.\n"
          "                           examples include tcp://127.0.0.1:30000\n"
          "                           or any of the other endpoint types like\n"
          "                           pgm://. For tcp, remember that the "
          "first\n"
          "                           endpoint defined must be your own, the\n"
          "                           one you are binding to, and all other\n"
          "                           agent endpoints must also be defined or\n"
          "                           no messages will ever be sent to them.\n"
          "                           Similarly, all agents will have to have\n"
          "                           this endpoint added to their list or\n"
          "                           this karl agent will not see them.\n"
          "  [-0|--init-logic logic]  logic with initial variables (only ran "
          "once)\n"
          "  [-0f|--init-file file]   file with initial variables (only ran "
          "once)\n"
          "  [-0b|--init-bin file]    file with binary knowledge base, the "
          "result\n"
          "                           of save_context (only ran once)\n"
          "  [--meta-prefix prefix]   store checkpoint meta data at knowledge "
          "prefix\n"
          "  [--use-id]               use the id of the checkpointed binary "
          "load\n"
          "\n",
          argv[0]);
      exit(0);
    }
  }
}

int main(int argc, char** argv)
{
  // handle all user arguments
  handle_arguments(argc, argv);

  // default transport is multicast
  if (settings.hosts.size() == 0)
  {
    settings.type = transport::MULTICAST;
    settings.hosts.push_back(default_multicast);
  }

  HandleRequests receive_filter;

  settings.add_receive_filter(&receive_filter);

  // save transport always happens after all possible transport chagnes
  if (save_transport != "")
  {
    if (save_transport_prefix == "")
      settings.save(save_transport);
    else
      settings.save(save_transport, save_transport_prefix);
  }

  // save transport always happens after all possible transport chagnes
  if (save_transport_text != "")
  {
    if (save_transport_prefix == "")
      settings.save_text(save_transport_text);
    else
      settings.save_text(save_transport_text, save_transport_prefix);
  }

  // create a knowledge base and setup our id
  knowledge::KnowledgeBase kb;

  receive_filter.requests.set_name(".requests", kb);
  receive_filter.requests.resize(requests_buffer_size);

  receive_filter.delete_requests.set_name(".delete_requests", kb);
  receive_filter.delete_requests.resize(requests_buffer_size);

  madara::knowledge::EvalSettings silent(true, true, true, true, true);
  shutdown_request.set_name(prefix + ".shutdown", kb);

  // map that is accessible to all filters and threads
  containers::FlexMap sandbox_map;

  // construct bandwidth probes
  current_send_bandwidth.set_name(".current_send_bandwidth", kb);
  current_total_bandwidth.set_name(".current_total_bandwidth", kb);

  // queue for requests for sandbox files
  containers::CircularBufferConsumerT<Request> request_consumer(
      ".requests", kb);
  containers::CircularBufferConsumerT<DeleteRequest> delete_consumer(
      ".delete_requests", kb);
  request_consumer.resize();
  delete_consumer.resize();

  // load any binary settings
  for (auto& file : initbinaries)
  {
    madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_ALWAYS,
        "Reading binary checkpoint from file %s:\n", file.c_str());

    load_checkpoint_settings.filename = file;
    load_checkpoint_settings.clear_knowledge = false;
    kb.load_context(load_checkpoint_settings, silent);
  }

  load_checkpoint_settings.ignore_header_check = true;

  // allow user-readable text files to overwrite binary settings
  for (auto& file : initfiles)
  {
    madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_ALWAYS,
        "Reading karl logic from file %s:\n", file.c_str());

    load_checkpoint_settings.filename = file;
    load_checkpoint_settings.clear_knowledge = false;
    kb.evaluate_file(load_checkpoint_settings, silent);
  }

  // construct sandbox map out of the loaded files
  sandbox_map.set_name(".sandbox", kb);
  knowledge::KnowledgeRules keys;
  sandbox_map.keys(keys, true);

  kb.attach_transport(host, settings);

  if (keys.size() == 0)
  {
    sandbox_map["default"].set(filesystem::current_path().string());
    sandbox_map.keys(keys, true);
    madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_ALWAYS,
        "Using default sandbox mapping of default => '.'\n");
  }

  std::vector<Sandbox> sandboxes(keys.size());

  for (size_t i = 0; i < keys.size(); ++i)
  {
    containers::FlexMap sandbox = sandbox_map[keys[i]];
    sandboxes[i].read(sandbox, keys[i], kb);
    sandboxes[i].refresh_files();
  }

  if (run_time > 0)
  {
    utility::EpochEnforcer<utility::Clock> enforcer(digest_period, run_time);

    while (shutdown_request.is_false() && !enforcer.is_done())
    {
      for (auto sandbox : sandboxes)
      {
        sandbox.modify();
      }

      process_requests(sandboxes, kb, request_consumer, delete_consumer);

      kb.send_modifieds();

      kb.print();
      enforcer.sleep_until_next();
    }
  }
  else
  {
    utility::EpochEnforcer<utility::Clock> enforcer(digest_period, run_time);

    while (shutdown_request.is_false())
    {
      for (auto sandbox : sandboxes)
      {
        sandbox.modify();
      }

      process_requests(sandboxes, kb, request_consumer, delete_consumer);

      kb.send_modifieds();

      kb.print();
      enforcer.sleep_until_next();
    }
  }

  kb.print();

  // save as checkpoint of changes by logics and input files
  if (save_checkpoint.size() > 0)
  {
    save_checkpoint_settings.filename = save_checkpoint;
    kb.save_checkpoint(save_checkpoint_settings);
  }

  // save as karl if requested
  if (save_location.size() > 0)
  {
    save_checkpoint_settings.filename = save_location;
    kb.save_as_karl(save_checkpoint_settings);
  }

  // save as karl if requested
  if (save_json.size() > 0)
  {
    save_checkpoint_settings.filename = save_json;
    kb.save_as_json(save_checkpoint_settings);
  }

  // save as binary if requested
  if (save_binary.size() > 0)
  {
    save_checkpoint_settings.filename = save_binary;
    kb.save_context(save_checkpoint_settings);
  }

  return 0;
}
