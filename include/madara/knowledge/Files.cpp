#ifndef _MADARA_KNOWLEDGE_BASE_FILES_CPP_
#define _MADARA_KNOWLEDGE_BASE_FILES_CPP_

#include "Files.h"
#include <sstream>
#include "ace/OS_NS_fcntl.h"

const std::string 
  madara::knowledge::Files::files_folder_ = "files";
const std::string 
  madara::knowledge::Files::policies_folder_ = "policies";

/// Constructor
madara::knowledge::Files::Files (ThreadSafeContext & map) :
 map_(map)
{
}

/// Shape a file according to a shaping policy
int 
madara::knowledge::Files::shape_file (
  const std::string &, const std::string &,
  const std::string &)
{


  return 0;
}

/// Read a file and update a buffer and size
int 
madara::knowledge::Files::read_file (const char * filename, 
                     ACE_Mem_Map & mem_map)
{
  // load the source file into a mapped file of the OS's 
  // virtual memory system
  ACE_Mem_Map mapped_file (filename);
  return mem_map.map (filename);
}

/// Read a file into the knowledge base
int 
madara::knowledge::Files::read_file (
  const std::string & knowledge_key, const std::string & filename)
{
  // return value for function
  int ret = 0;

  // Stringstreams for building the name of the files
  std::stringstream source_filename;
  std::stringstream dest_filename;

  // build source file name
  source_filename << filename;

  // build dest file name
  dest_filename << files_folder_;
  dest_filename << "/";
  dest_filename << knowledge_key;

  madara_logger_log (map_.get_logger (), logger::LOG_DETAILED,
    "Files::read_file : %s->%s\n",
    source_filename.str ().c_str (), dest_filename.str ().c_str ());

  // load the source file into a mapped file of the OS's 
  // virtual memory system
  ACE_Mem_Map mapped_file;
  read_file (source_filename.str ().c_str (), mapped_file);
  void * file_contents = mapped_file.addr ();
  size_t size = mapped_file.size ();

  madara_logger_log (map_.get_logger (), logger::LOG_DETAILED,
    "Files::read_file : file size is %d, loaded at 0x%x\n",
    size, file_contents);

  if (size > 0)
  {
    // obtain the address of the mapped file and write the mapped file
    // to the destination
    size_t actual = write_file (dest_filename.str ().c_str (), 
                                file_contents, size);

    // if we actually wrote something to the file, update the specified
    // knowledge key
    if (actual > 0)
    {
      map_.set (knowledge_key, (madara::KnowledgeRecord::Integer) actual);

      madara_logger_log (map_.get_logger (), logger::LOG_DETAILED,
        "Files::read_file : file imported with %" PRIu64 " bytes\n", actual);
    }
    else ret = -2;
  }
  else ret = -1;

  // unmap the memory mapped file
  mapped_file.close_filemapping_handle ();
  mapped_file.close ();

  if (ret < 0)
  {
    madara_logger_log (map_.get_logger (), logger::LOG_EMERGENCY,
      "Files::read_file : read file failed with %d\n", ret);
  }

  return ret;
}

/// Write a file from the knowledge base to a specified location
ssize_t 
madara::knowledge::Files::write_file (
  const std::string & knowledge_key, const std::string & filename)
{
  // return value for function
  ssize_t ret = 0;

  // Stringstreams for building the name of the files
  std::stringstream source_filename;
  std::stringstream dest_filename;

  // build source file name
  source_filename << files_folder_;
  source_filename << "/";
  source_filename << knowledge_key;

  // build dest file name
  dest_filename << filename;

  madara_logger_log (map_.get_logger (), logger::LOG_DETAILED,
    "Files::write_file : %s->%s\n",
    source_filename.str ().c_str (), dest_filename.str ().c_str ());

  // load the source file into a mapped file of the OS's 
  // virtual memory system
  ACE_Mem_Map mapped_file;
  read_file (source_filename.str ().c_str (), mapped_file);
  void * file_contents = mapped_file.addr ();
  size_t size = mapped_file.size ();

  madara_logger_log (map_.get_logger (), logger::LOG_DETAILED,
    "Files::write_file : file size is %d, loaded at 0x%x\n",
    size, file_contents);

  if (size > 0)
  {
    // obtain the address of the mapped file and write the mapped file
    // to the destination
    size_t actual = write_file (dest_filename.str ().c_str (), 
                                file_contents, size);

    // if we actually wrote something to the file, update the specified
    // knowledge key
    if (actual > 0)
    {
      map_.set (knowledge_key, (madara::KnowledgeRecord::Integer) actual);

      madara_logger_log (map_.get_logger (), logger::LOG_DETAILED,
        "Files::write_file : file saved with %" PRIu64 " bytes\n", actual);
    }
    else ret = -2;
  }
  else ret = -1;

  // unmap the memory mapped file
  mapped_file.close_filemapping_handle ();
  mapped_file.close ();

  if (ret < 0)
  {
    madara_logger_log (map_.get_logger (), logger::LOG_ERROR,
      "Files::write_file : write file failed with %d\n", ret);
  }

  return ret;
}

/// Write a memory buffer to file
ssize_t 
madara::knowledge::Files::write_file (const char * filename, 
                     void *& buffer, size_t & size)
{
  // error is -1
  ssize_t actual = -1;

  // using ACE for writing to the destination file
  ACE_HANDLE file_handle = ACE_OS::open (filename,
    O_RDWR | O_CREAT | O_TRUNC,
    ACE_DEFAULT_FILE_PERMS);

  madara_logger_log (map_.get_logger (), logger::LOG_DETAILED,
    "Files::write_file : beginning write of %" PRIu64 " bytes\n", size);

  if (file_handle  != ACE_INVALID_HANDLE)
  {
    // write to the file, save actual bytes read, and close the file handle
    actual = ACE_OS::write (file_handle, buffer, size);
    ACE_OS::close (file_handle);
  }

  // return the actual bytes written. -1 if error
  return actual;
}


/// Read a policy into the knowledge base
int 
madara::knowledge::Files::read_policy (
  const std::string & policy_key, const std::string & policy_file)
{

  // return value for function
  int ret = 0;

  // Stringstreams for building the name of the files
  std::stringstream source_filename;
  std::stringstream dest_filename;

  // build source file name
  source_filename << policy_file;

  // build dest file name
  dest_filename << policies_folder_;
  dest_filename << "/";
  dest_filename << policy_key;

  madara_logger_log (map_.get_logger (), logger::LOG_DETAILED,
    "Files::read_policy : %s->%s\n",
    source_filename.str ().c_str (), dest_filename.str ().c_str ());

  // load the source file into a mapped file of the OS's 
  // virtual memory system
  ACE_Mem_Map mapped_file;
  read_file (source_filename.str ().c_str (), mapped_file);
  void * file_contents = mapped_file.addr ();
  size_t size = mapped_file.size ();

  madara_logger_log (map_.get_logger (), logger::LOG_DETAILED,
    "Files::read_policy : file size is %d, loaded at 0x%x\n",
    size, file_contents);

  if (size > 0)
  {
    // obtain the address of the mapped file and write the mapped file
    // to the destination
    size_t actual = write_file (dest_filename.str ().c_str (), 
                                file_contents, size);

    // if we actually wrote something to the file, update the specified
    // knowledge key
    if (actual > 0)
    {
      map_.set (policy_key, (madara::KnowledgeRecord::Integer) actual);

      madara_logger_log (map_.get_logger (), logger::LOG_DETAILED,
        "Files::read_policy : file imported with %zu bytes\n", actual);
    }
    else ret = -2;
  }
  else ret = -1;

  // unmap the memory mapped file
  mapped_file.close_filemapping_handle ();
  mapped_file.close ();

  if (ret < 0)
  {
    madara_logger_log (map_.get_logger (), logger::LOG_ERROR,
      "Files::read_policy : read file failed with %d\n", ret);
  }

  return ret;
}

#endif
