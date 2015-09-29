#include <ctype.h>
#include <cmath>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <sstream>
#include <fstream>

#include "madara/logger/Global_Logger.h"
#include "madara/utility/Utility.h"
#include "ace/INET_Addr.h"

#include "ace/Default_Constants.h"
#include "ace/Mem_Map.h"
#include "ace/OS_NS_fcntl.h"
#include "ace/OS_NS_unistd.h"
#include "ace/OS_NS_sys_socket.h"
#include "ace/OS_NS_Thread.h"
#include "ace/OS_NS_sys_time.h"
#include "ace/High_Res_Timer.h"

#include "madara/knowledge_engine/Knowledge_Base.h"

std::string
Madara::Utility::get_version (void)
{
#include "madara/Version.h"

  return version + std::string (" compiled on ") +
    __DATE__ + " at " + __TIME__;
}


uint32_t
Madara::Utility::get_uint_version (void)
{
  std::string str_version = get_version ();
  unsigned char version_buffer[4] = {0,0,0,0};
  uint32_t * return_value = (uint32_t *) version_buffer;
  unsigned int major, minor, revision;
  char version_delimiter;

  std::stringstream buffer;

  // copy the stringified version
  buffer << str_version;

  // major version
  buffer >> major;

  buffer >> version_delimiter;

  // minor version
  buffer >> minor;
  
  buffer >> version_delimiter;

  // revision number
  buffer >> revision;

  // copy these into the version_buffer

  version_buffer[1] = (unsigned char)major;
  version_buffer[2] = (unsigned char)minor;
  version_buffer[3] = (unsigned char)revision;

  return *return_value;
}

std::string
Madara::Utility::to_string_version (uint32_t version)
{
  std::stringstream new_version;
  unsigned char * version_ptr = (unsigned char *)&version;
  unsigned int major, minor, revision;

  major = (unsigned int)version_ptr[1];
  minor = (unsigned int)version_ptr[2];
  revision = (unsigned int)version_ptr[3];

  new_version << major;
  new_version << '.';
  new_version << minor;
  new_version << '.';
  new_version << revision;

  return new_version.str ();
}


/// Convert string to uppercase
std::string &
Madara::Utility::upper (std::string &input)
{
  for (std::string::iterator cur = input.begin ();
       cur != input.end (); ++cur)
    *cur = toupper (*cur);

  return input;
}

/// Convert string to lowercase 
std::string & 
Madara::Utility::dds_topicify (std::string & input)
{
  for (std::string::iterator cur = input.begin ();
       cur != input.end (); ++cur)
  {
    // change periods to _
    if (*cur == '.')
      *cur = '_';
  }

  return input;
}

/// Convert string to lowercase 
std::string &
Madara::Utility::lower (std::string &input)
{
  for (std::string::iterator cur = input.begin ();
       cur != input.end (); ++cur)
    *cur = tolower (*cur);

  return input;
}

/// Strip whitespace from front and end of string and also
/// condense multiple whitespace into a single space
std::string &
Madara::Utility::strip_extra_white_space (std::string & input)
{
  std::string::iterator cur = input.begin ();
  char prev = 0;

  for (std::string::iterator eval = cur; 
    eval != input.end (); ++eval)
  {
    // if it isn't whitespace, then copy it over immediately
    if (*eval != ' ' && *eval != '\t' && *eval != '\n' && *eval != '\r')
    {
      prev = *cur = *eval;
      ++cur;
    }
    // if it is whitespace, only insert whitespace if the previous char
    // was non-whitespace
    else if (prev)
    {
      *cur = ' ';
      prev = 0;
      ++cur;
    }
  }

  // if the last char is actually whitespace, then move cur back one spot
  if (cur != input.end ())
  {
    --cur;
    if (*cur != ' ' && *cur != '\t' && *cur != '\n' && *cur != '\r')
      ++cur;
  }

  // erase everything from cur to end of input string
  if (cur != input.end ())
    input.erase (cur, input.end ());

  return input;
}


/// Strip all whitespace
std::string &
Madara::Utility::strip_white_space (std::string & input)
{
  std::string::iterator cur = input.begin ();
  char prev = 0;

  for (std::string::iterator eval = cur; 
    eval != input.end (); ++eval)
  {
    // if it isn't whitespace, then copy it over immediately
    if (*eval != ' ' && *eval != '\t' && *eval != '\n' && *eval != '\r')
    {
      prev = *cur = *eval;
      ++cur;
    }
  }

  // erase everything from cur to end of input string
  if (cur != input.end ())
    input.erase (cur, input.end ());

  return input;
}


/**
  * Strips an unwanted character. This function will
  * modify and return the input, so make a copy if needed.
  **/
std::string &
Madara::Utility::string_remove (std::string & input, char unwanted)
{
  std::string::iterator cur = input.begin ();
  char prev = 0;

  for (std::string::iterator eval = cur; 
    eval != input.end (); ++eval)
  {
    // if it isn't whitespace, then copy it over immediately
    if (*eval != unwanted)
    {
      prev = *cur = *eval;
      ++cur;
    }
  }

  // erase everything from cur to end of input string
  if (cur != input.end ())
    input.erase (cur, input.end ());

  return input;
}

size_t
Madara::Utility::string_replace (std::string & source,
  const std::string & old_phrase,
  const std::string & new_phrase,
  bool replace_all)
{
  // return value
  size_t replacements = 0;

  for (std::string::size_type i = source.find (old_phrase, 0);
    i != source.npos; i = source.find (old_phrase, i))
  {
    source.replace (i, old_phrase.size (), new_phrase);
    i += new_phrase.size ();

    ++replacements;

    if (!replace_all)
      break;
  }

  return replacements;
}

std::string &
Madara::Utility::strip_comments (std::string & input)
{
  std::stringstream source, dest;
  std::string cur;
  std::vector <std::string> splitters;
  splitters.resize (2);

  splitters[0] = "//";

  // place the input in the string stream
  source << input;
  
  while (std::getline (source, cur))
  {
    std::vector <std::string> tokens;
    std::vector <std::string> pivots;
    tokenizer (cur, splitters, tokens, pivots);

    if (tokens.size ())
    {
      dest << tokens[0];
      dest << "\n";
    }
  }

  input = dest.str ();
  return input;
}


/// Split a string into tokens 
void 
Madara::Utility::tokenizer (const std::string & input, 
    const ::std::vector< std::string> & splitters,
    ::std::vector< std::string> & tokens,
    ::std::vector< std::string> & pivots)
{
  std::string::size_type last = 0;
  std::string::size_type cur = 0;
  tokens.clear ();
  pivots.clear ();

  for (; cur < input.size (); ++cur)
  {
    for (std::string::size_type i = 0; i < splitters.size (); ++i)
    {
      // if the splitter string length is greater than zero
      if (splitters[i].size () > 0)
      {
        // if the first char of the splitter string is equal to the char
        if (input[cur] == splitters[i][0])
        {
          std::string::size_type checker = cur;
          std::string::size_type j = 1;
          for (++checker; 
            checker < input.size () && j < splitters[i].size () &&
            input[checker] == splitters[i][j];
            ++j, ++checker);

          // we have found a splitter. Tokenize from last to splitter.
          if (j == splitters[i].size ())
          {
            // need to update this to only have as many pivots as tokens - 1
            pivots.push_back (input.substr (cur, j));

            if (cur - last >= splitters[i].size () - 1)
              tokens.push_back (input.substr (last, cur - last));
            else
              tokens.push_back ("");

            // we want last to point to the last valid token begin
            cur = checker - 1;
            last = checker;
          } // if found a splitter
        } // if first char == splitter first char
      } // if splitter length >= 1
    } // for splitters

  } // for chars

  if (last != cur)
  {
    tokens.push_back (input.substr (last, cur - last));
  }
}

// split a key into a corresponding host and port
int 
Madara::Utility::split_hostport_identifier (const std::string & key, 
    std::string & host, std::string & port)
{
  // delimiter is either a : or an @
  std::string::size_type delim = key.rfind (':');
  delim = delim == key.npos ? key.rfind ('@') : delim;

  // no delimiter found
  if (delim == key.npos)
    {
      host = key;
      port = "";
      
      return 1;
    }

  // otherwise, set the host and port appropriately
  host = key.substr (0, delim);
  port = key.substr (delim + 1, key.size () - delim);

  return 0;
}

// merge a host and port into a key
int 
Madara::Utility::merge_hostport_identifier (std::string & key, 
  const std::string & host, const std::string & port)
{
  if ((const std::string *)&key != &host)
    key = host;

  key += ':';
  key += port;

  return 0;
}

// merge a host and ushort port into a key
int 
Madara::Utility::merge_hostport_identifier (std::string & key, 
  const std::string & host, unsigned short u_port)
{
  std::stringstream port_stream;
  port_stream << u_port;

  return merge_hostport_identifier (key, host, port_stream.str ());
}

/// Bind to an ephemeral port
int 
Madara::Utility::bind_to_ephemeral_port (ACE_SOCK_Dgram & socket,
   std::string & host, unsigned short & port, bool increase_until_bound)
{
  // start with the initial port provided
  // increase port each time we don't properly bind

  madara_logger_ptr_log (Logger::global_logger.get(), Logger::LOG_DETAILED,
    "Utility::bind_to_ephemeral_port:" \
    " creating ACE_INET_Addr\n");

  ACE_INET_Addr addr (port);
  char hostname[HOST_NAME_MAX + 1] = "";

  madara_logger_ptr_log (Logger::global_logger.get(), Logger::LOG_MAJOR,
    "Utility::bind_to_ephemeral_port:" \
    " getting hostname from ACE_INET_Addr\n");

  addr.get_host_name (hostname, HOST_NAME_MAX);
  host = hostname;

  madara_logger_ptr_log (Logger::global_logger.get(), Logger::LOG_MINOR,
    "Utility::bind_to_ephemeral_port:" \
    " hostname equals %s\n", hostname);

  madara_logger_ptr_log (Logger::global_logger.get(), Logger::LOG_DETAILED,
    "Utility::bind_to_ephemeral_port:" \
    " calling ACE_OS::socket_init\n");

  ACE_OS::socket_init (2, 2);

  for ( ; port < 65535; ++port, addr.set (port))
  {

    madara_logger_ptr_log (Logger::global_logger.get(), Logger::LOG_MINOR,
      "Utility::bind_to_ephemeral_port:" \
      " attempting open on socket %d\n", port);

    if (socket.open (addr, 2, 0, 0) != -1)
      return 0;

    // failed to get port
    if (!increase_until_bound)
      break;
  }

  madara_logger_ptr_log (Logger::global_logger.get(), Logger::LOG_MAJOR,
    "Utility::bind_to_ephemeral_port:" \
    " unable to bind to any ephemeral port. Check firewall\n");

  return -1;
}

std::string
Madara::Utility::file_to_string (const std::string & filename)
{
  std::string line;
  std::stringstream buffer;

  std::ifstream file (filename.c_str ());

  // if the file was able to open
  if (file.is_open ())
  {
    std::getline (file, line);

    if (line != "")
      buffer << line;

    // while there is still a line left in the file, read that line
    // into our stringstream buffer
    while (std::getline (file, line))
      buffer << "\n" << line;
    file.close ();
  }
  else
  {
    madara_logger_ptr_log (Logger::global_logger.get(), Logger::LOG_MAJOR,
      "Utility::file_to_string:" \
      " failed to open file: %s\n");
  }

  return buffer.str ();
}


std::string
Madara::Utility::extract_path (const std::string & name)
{
  std::string::size_type start = 0;
  for (std::string::size_type i = 0; i < name.size (); ++i)
  {
    // check for directory delimiters and update start
    if (name[i] == '/' || name[i] == '\\')
    {
      // if they have supplied a directory with an
      // ending slash, then use the previous start
      if (i != name.size () - 1)
        start = i + 1;
    }
  }

  // return the substring from 0 with start number of elements
  return name.substr (0, start);
}

std::string
Madara::Utility::extract_filename (const std::string & name)
{
  std::string::size_type start = 0;
  for (std::string::size_type i = 0; i < name.size (); ++i)
  {
    // check for directory delimiters and update start
    if (name[i] == '/' || name[i] == '\\')
    {
      // if they have supplied a directory with an
      // ending slash, then use the previous start
      if (i != name.size () - 1)
        start = i + 1;
    }
  }

  // return the substring from start to the end of the filename
  return name.substr (start, name.size () - start);
}

/// Expand any environment variables in a string
std::string
Madara::Utility::expand_envs (const std::string & source)
{
  std::stringstream buffer;

  for (size_t i = 0; i < source.size (); ++i)
  {
    // environment variable must be larger than $()
    if (source[i] == '$' && i + 3 < source.size ())
    {
      char * value = get_var (source, i+2, i);
      if (value)
        buffer << value;
    }
    else
      buffer << source[i];
  }
  return buffer.str ();
}

/// grab an environment variable value (@see expand_envs)
char * 
Madara::Utility::get_var (const std::string & source, 
                          size_t cur, size_t & end)
{
  for (end = cur; end < source.size (); ++end)
  {
    if (source[end] == ')' || source[end] == '}')
    {
      return getenv (source.substr (cur, end - cur).c_str ());
    }
  }

  return getenv (source.substr (cur,source.size () - cur).c_str ());
}

std::string
Madara::Utility::clean_dir_name (const std::string & source)
{
  // define the characters we'll want to replace
#ifdef ACE_WIN32
  #define REPLACE_THIS    '/'
  #define REPLACE_WITH    '\\'
#else
  #define REPLACE_THIS    '\\'
  #define REPLACE_WITH    '/'
#endif

  std::string target (source);

  for (std::string::iterator i = target.begin (); i != target.end(); ++i)
  {
    if (*i == REPLACE_THIS)
      *i = REPLACE_WITH;
  }

  return target;
}


// helper class for htonll
class EndiannessChecker
{
public:
  EndiannessChecker ()
  {
    x.us = 1;

    if (x.c[sizeof (unsigned short) - 1] == 1)
      is_little = false;
    else
      is_little = true;
  }

  bool is_little;

private:
  // by using a union, we can reference the bytes of the short
  union {
        unsigned short us;
        unsigned char c[sizeof (unsigned short)];
    } x;
};

EndiannessChecker endianness;

/**
  * Converts a host format uint64_t into big endian
  **/
uint64_t
Madara::Utility::endian_swap (uint64_t value)
{
  // if host is little endian, then we have work to do
  if (::endianness.is_little)
  {
    value = ((value << 8) & 0xFF00FF00FF00FF00ULL )
          | ((value >> 8) & 0x00FF00FF00FF00FFULL );
    value = ((value << 16) & 0xFFFF0000FFFF0000ULL )
          | ((value >> 16) & 0x0000FFFF0000FFFFULL );
    return (value << 32) | (value >> 32);
  }

  return value;
}

/**
* Converts a host format signed Madara::Knowledge_Record::Integer into big endian
**/
int64_t
Madara::Utility::endian_swap (int64_t value)
{
  // if host is little endian, then we have work to do
  if (::endianness.is_little)
  {
    value = ((value << 8) & 0xFF00FF00FF00FF00ULL )
          | ((value >> 8) & 0x00FF00FF00FF00FFULL );
    value = ((value << 16) & 0xFFFF0000FFFF0000ULL )
          | ((value >> 16) & 0x0000FFFF0000FFFFULL );
    return (value << 32) | ((value >> 32) & 0xFFFFFFFFULL);
  }

  return value;
}


/**
  * Converts a host format uint64_t into big endian
  **/
uint32_t
Madara::Utility::endian_swap (uint32_t value)
{
  // if host is little endian, then we have work to do
  if (::endianness.is_little)
  {
    value = ((value << 8) & 0xFF00FF00 ) |
            ((value >> 8) & 0xFF00FF ); 
    return (value << 16) | (value >> 16);
  }

  return value;
}

/**
* Converts a host format signed Madara::Knowledge_Record::Integer into big endian
**/
int32_t
Madara::Utility::endian_swap (int32_t value)
{
  // if host is little endian, then we have work to do
  if (::endianness.is_little)
  {
    value = ((value << 8) & 0xFF00FF00) |
            ((value >> 8) & 0xFF00FF ); 
    return (value << 16) | ((value >> 16) & 0xFFFF);
  }

  return value;
}

/**
  * Converts a host format double into big endian
  **/
double
Madara::Utility::endian_swap (double orig)
{
  uint64_t * value = (uint64_t *)&orig;
  double result = orig;
  // if host is little endian, then we have work to do
  if (::endianness.is_little)
  {
    *value = ((*value << 8) & 0xFF00FF00FF00FF00ULL )
          | ((*value >> 8) & 0x00FF00FF00FF00FFULL );
    *value = ((*value << 16) & 0xFFFF0000FFFF0000ULL )
          | ((*value >> 16) & 0x0000FFFF0000FFFFULL );
    *value = (*value << 32) | (*value >> 32);
    memcpy (&result, value, 8);
  }

  return result;
}

int
  Madara::Utility::read_file (const std::string & filename,
  void *& buffer, size_t & size, bool add_zero_char)
{
  int ret_value = 0;
  if (filename != "")
  {
    // load the source file into a mapped file of the OS's 
    // virtual memory system
    ACE_Mem_Map mapped_file;
    void * temp_buffer;
    ret_value = mapped_file.map (filename.c_str ());
    temp_buffer = mapped_file.addr ();
    size = mapped_file.size ();

    if (add_zero_char)
      ++size;

    buffer = new unsigned char [size];
    memcpy (buffer, temp_buffer, size);

    if (add_zero_char)
    {
      unsigned char * zeroed = (unsigned char *)buffer;
      zeroed[size - 1] = 0;
    }
  }
  else ret_value = -1;

  return ret_value;
}

ssize_t
  Madara::Utility::write_file (const std::string & filename,
  void * buffer, size_t size)
{
  // error is -1
  ssize_t actual = -1;

  // using ACE for writing to the destination file
  ACE_HANDLE file_handle = ACE_OS::open (filename.c_str (),
    O_RDWR | O_CREAT | O_TRUNC,
    ACE_DEFAULT_FILE_PERMS);

  madara_logger_ptr_log (Logger::global_logger.get(), Logger::LOG_MAJOR,
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

int64_t
Madara::Utility::get_time (void)
{
  ACE_Time_Value tv = ACE_High_Res_Timer::gettimeofday ();
  int64_t timeofday (tv.sec () * 1000000000);
  timeofday += tv.usec () * 1000;

  return timeofday;
}

double
Madara::Utility::rand_double (double floor, double ceiling,
                              bool set_seed_to_time)
{
  // check if the user has specified setting through srand
  if (set_seed_to_time)
  {
    unsigned int new_seed (get_time ());
    srand (new_seed);
  }

  // Get a double number between 0 and 1.
  double position_in_range = ((double)rand()) / ((double)RAND_MAX);

  if (floor < ceiling)
      return (position_in_range * (ceiling - floor)) + floor;
  else
      return (position_in_range * (floor - ceiling)) + ceiling;
}

int64_t
Madara::Utility::rand_int (int64_t floor, int64_t ceiling,
                           bool set_seed_to_time)
{
  double random = rand_double (floor, ceiling, set_seed_to_time);
  return nearest_int (random);
}

int64_t
Madara::Utility::nearest_int (double input)
{
  int change = input >= 0 ? 1 : -1;
  int64_t left = input;
  int64_t right = input + change;

  if (input - left < -input + right)
    return left;
  else
    return right;
}


double Madara::Utility::sleep (double sleep_time)
{
  ACE_Time_Value current = ACE_High_Res_Timer::gettimeofday (); 
  ACE_Time_Value actual_time;
  actual_time.set (sleep_time);

  sleep (actual_time);

  ACE_Time_Value end = ACE_High_Res_Timer::gettimeofday ();
  end = end - current;
  double time_taken = (double) end.sec ();
  time_taken += (double) end.usec () / 1000000;

  return time_taken;
}
    

ACE_Time_Value Madara::Utility::sleep (const ACE_Time_Value & sleep_time)
{
  ACE_Time_Value current = ACE_High_Res_Timer::gettimeofday (); 
  ACE_Time_Value earliest = current + sleep_time;

  while (current < earliest)
  {
    ACE_OS::sleep (earliest - current);
    current = ACE_High_Res_Timer::gettimeofday ();
  }

  return ACE_High_Res_Timer::gettimeofday () - current;
}

bool Madara::Utility::approx_equal (
  double value1, double value2, double epsilon)
{
  return std::abs (value1 - value2) < epsilon;
}

bool
Madara::Utility::file_exists (const std::string & filename)
{
  if (FILE * file = fopen (filename.c_str(), "r"))
  {
    fclose(file);
    return true;
  }
  else
  {
    return false;
  }   
}

unsigned int
Madara::Utility::file_size (const std::string & filename)
{
  unsigned int size = 0;
  if (FILE * file = fopen (filename.c_str(), "r"))
  {
    fseek (file, 0L, SEEK_END);
    size = ftell (file);
    fclose (file);
  }

  return size;
}

bool
Madara::Utility::wait_true (
  Knowledge_Engine::Knowledge_Base & knowledge,
  const std::string & variable,
  const Knowledge_Engine::Wait_Settings & settings)
{  
  // get current time of day
  ACE_Time_Value current = ACE_High_Res_Timer::gettimeofday ();  
  ACE_Time_Value max_wait, sleep_time, next_epoch;
  ACE_Time_Value poll_frequency, last = current;

  Knowledge_Engine::Variable_Reference ref = knowledge.get_ref (variable);

  if (settings.poll_frequency >= 0)
  {
    max_wait.set (settings.max_wait_time);
    max_wait = current + max_wait;
    
    poll_frequency.set (settings.poll_frequency);
    next_epoch = current + poll_frequency;
  }

  // print the post statement at highest log level (cannot be masked)
  if (settings.pre_print_statement != "")
    knowledge.print (settings.pre_print_statement, Logger::LOG_ALWAYS);

  Madara::Knowledge_Record last_value = knowledge.get (ref, settings);

  madara_logger_ptr_log (Logger::global_logger.get(), Logger::LOG_MAJOR,
    "Utility::wait_true:" \
    " variable returned %s\n",
    last_value.to_string ().c_str ());
  
  current = ACE_High_Res_Timer::gettimeofday ();

  // wait for expression to be true
  while (!last_value.to_integer () &&
    (settings.max_wait_time < 0 || current < max_wait))
  {
    madara_logger_ptr_log (Logger::global_logger.get(), Logger::LOG_DETAILED,
      "Utility::wait_true:" \
      " current is %" PRIu64 ".%" PRIu64 " and max is %" PRIu64 ".%" PRIu64 " (poll freq is %f)\n",
      current.sec (), current.usec (), max_wait.sec (), max_wait.usec (),
      settings.poll_frequency);

    madara_logger_ptr_log (Logger::global_logger.get(), Logger::LOG_DETAILED,
      "Utility::wait_true:" \
      " last value didn't result in success\n");

    // Unlike the other wait statements, we allow for a time based wait.
    // To do this, we allow a user to specify a 
    if (settings.poll_frequency > 0)
    {
      if (current < next_epoch)
      {
        sleep_time = next_epoch - current;
        Madara::Utility::sleep (sleep_time);
      }

      next_epoch = next_epoch + poll_frequency;
    }
    else
      knowledge.wait_for_change ();

    madara_logger_ptr_log (Logger::global_logger.get(), Logger::LOG_DETAILED,
      "Utility::wait_true:" \
      " waiting on %s\n", variable.c_str ());

    last_value = knowledge.get (ref, settings);

    madara_logger_ptr_log (Logger::global_logger.get(), Logger::LOG_DETAILED,
      "Utility::wait_true:" \
      " completed eval to get %s\n",
      last_value.to_string ().c_str ());
  
    // get current time
    current = ACE_High_Res_Timer::gettimeofday ();

  } // end while (!last)
  
  if (current >= max_wait)
  {
    madara_logger_ptr_log (Logger::global_logger.get(), Logger::LOG_DETAILED,
      "Utility::wait_true:" \
      " Evaluate did not succeed. Timeout occurred\n",
      last_value.to_string ().c_str ());
  }

  // print the post statement at highest log level (cannot be masked)
  if (settings.post_print_statement != "")
    knowledge.print (settings.post_print_statement, Logger::LOG_ALWAYS);

  return last_value.is_true ();
}

bool Madara::Utility::wait_false (
  Knowledge_Engine::Knowledge_Base & knowledge,
  const std::string & variable,
  const Knowledge_Engine::Wait_Settings & settings)
{
  // get current time of day
  ACE_Time_Value current = ACE_High_Res_Timer::gettimeofday ();  
  ACE_Time_Value max_wait, sleep_time, next_epoch;
  ACE_Time_Value poll_frequency, last = current;

  Knowledge_Engine::Variable_Reference ref = knowledge.get_ref (variable);

  if (settings.poll_frequency >= 0)
  {
    max_wait.set (settings.max_wait_time);
    max_wait = current + max_wait;
    
    poll_frequency.set (settings.poll_frequency);
    next_epoch = current + poll_frequency;
  }

  // print the post statement at highest log level (cannot be masked)
  if (settings.pre_print_statement != "")
    knowledge.print (settings.pre_print_statement, Logger::LOG_ALWAYS);

  Madara::Knowledge_Record last_value = !knowledge.get (ref, settings);

  madara_logger_ptr_log (Logger::global_logger.get(), Logger::LOG_DETAILED,
    "Utility::wait_false:" \
    " variable returned %s\n",
    last_value.to_string ().c_str ());
  
  current = ACE_High_Res_Timer::gettimeofday ();

  // wait for expression to be true
  while (!last_value.to_integer () &&
    (settings.max_wait_time < 0 || current < max_wait))
  {
    madara_logger_ptr_log (Logger::global_logger.get(), Logger::LOG_DETAILED,
      "Utility::wait_false:" \
      " current is %d.%d and max is %d.%d (poll freq is %f)\n",
      current.sec (), current.usec (), max_wait.sec (), max_wait.usec (),
      settings.poll_frequency);

    madara_logger_ptr_log (Logger::global_logger.get(), Logger::LOG_DETAILED,
      "Utility::wait_false:"
      " last value didn't result in success\n");

    // Unlike the other wait statements, we allow for a time based wait.
    // To do this, we allow a user to specify a 
    if (settings.poll_frequency > 0)
    {
      if (current < next_epoch)
      {
        sleep_time = next_epoch - current;
        Madara::Utility::sleep (sleep_time);
      }

      next_epoch = next_epoch + poll_frequency;
    }
    else
      knowledge.wait_for_change ();

    madara_logger_ptr_log (Logger::global_logger.get(), Logger::LOG_MAJOR,
      "Utility::wait_false:"
      " waiting on %s\n", variable.c_str ());

    last_value = !knowledge.get (ref, settings);

    madara_logger_ptr_log (Logger::global_logger.get(), Logger::LOG_DETAILED,
      "Utility::wait_false:"
      " completed eval to get %s\n",
      last_value.to_string ().c_str ());
  
    // get current time
    current = ACE_High_Res_Timer::gettimeofday ();

  } // end while (!last)
  
  if (current >= max_wait)
  {
    madara_logger_ptr_log (Logger::global_logger.get(), Logger::LOG_MAJOR,
      "Utility::wait_false:"
      " Evaluate did not succeed. Timeout occurred\n",
      last_value.to_string ().c_str ());
  }

  // print the post statement at highest log level (cannot be masked)
  if (settings.post_print_statement != "")
    knowledge.print (settings.post_print_statement, Logger::LOG_EMERGENCY);

  return last_value.is_true ();
}