#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <sstream>
#include <fstream>

#include "madara/logger/GlobalLogger.h"
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

#include "madara/knowledge/KnowledgeBase.h"

#include "Utility.h"

namespace madara { namespace utility {

std::string
get_version (void)
{
#include "madara/Version.h"

  return version + std::string (" compiled on ") +
    __DATE__ + " at " + __TIME__;
}


uint32_t
get_uint_version (void)
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
to_string_version (uint32_t version)
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

/// Strip whitespace from front and end of string and also
/// condense multiple whitespace into a single space
std::string &
strip_extra_white_space (std::string & input)
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
strip_white_space (std::string & input)
{
  std::string::iterator cur = input.begin ();

  for (std::string::iterator eval = cur; 
    eval != input.end (); ++eval)
  {
    // if it isn't whitespace, then copy it over immediately
    if (*eval != ' ' && *eval != '\t' && *eval != '\n' && *eval != '\r')
    {
      *cur = *eval;
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
string_remove (std::string & input, char unwanted)
{
  std::string::iterator cur = input.begin ();

  for (std::string::iterator eval = cur; 
    eval != input.end (); ++eval)
  {
    // if it isn't whitespace, then copy it over immediately
    if (*eval != unwanted)
    {
      *cur = *eval;
      ++cur;
    }
  }

  // erase everything from cur to end of input string
  if (cur != input.end ())
    input.erase (cur, input.end ());

  return input;
}

size_t
string_replace (std::string & source,
  const std::string & old_phrase,
  const std::string & new_phrase,
  bool replace_all)
{
  // return value
  size_t replacements = 0;

  if (old_phrase != "")
  {
    for (std::string::size_type i = source.find (old_phrase, 0);
      i != source.npos; i = source.find (old_phrase, i))
    {
      source.replace (i, old_phrase.size (), new_phrase);
      i += new_phrase.size ();

      ++replacements;

      if (!replace_all)
        break;
    }
  }

  return replacements;
}

std::string &
strip_comments (std::string & input)
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
tokenizer (const std::string & input, 
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
split_hostport_identifier (const std::string & key, 
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
merge_hostport_identifier (std::string & key, 
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
merge_hostport_identifier (std::string & key, 
  const std::string & host, unsigned short u_port)
{
  std::stringstream port_stream;
  port_stream << u_port;

  return merge_hostport_identifier (key, host, port_stream.str ());
}

/// Bind to an ephemeral port
int 
bind_to_ephemeral_port (ACE_SOCK_Dgram & socket,
   std::string & host, unsigned short & port, bool increase_until_bound)
{
  // start with the initial port provided
  // increase port each time we don't properly bind

  madara_logger_ptr_log (logger::global_logger.get(), logger::LOG_DETAILED,
    "utility::bind_to_ephemeral_port:" \
    " creating ACE_INET_Addr\n");

  ACE_INET_Addr addr (port);
  char hostname[HOST_NAME_MAX + 1] = "";

  madara_logger_ptr_log (logger::global_logger.get(), logger::LOG_MAJOR,
    "utility::bind_to_ephemeral_port:" \
    " getting hostname from ACE_INET_Addr\n");

  addr.get_host_name (hostname, HOST_NAME_MAX);
  host = hostname;

  madara_logger_ptr_log (logger::global_logger.get(), logger::LOG_MINOR,
    "utility::bind_to_ephemeral_port:" \
    " hostname equals %s\n", hostname);

  madara_logger_ptr_log (logger::global_logger.get(), logger::LOG_DETAILED,
    "utility::bind_to_ephemeral_port:" \
    " calling ACE_OS::socket_init\n");

  ACE_OS::socket_init (2, 2);

  for ( ; port < 65535; ++port, addr.set (port))
  {

    madara_logger_ptr_log (logger::global_logger.get(), logger::LOG_MINOR,
      "utility::bind_to_ephemeral_port:" \
      " attempting open on socket %d\n", port);

    if (socket.open (addr, 2, 0, 0) != -1)
      return 0;

    // failed to get port
    if (!increase_until_bound)
      break;
  }

  madara_logger_ptr_log (logger::global_logger.get(), logger::LOG_MAJOR,
    "utility::bind_to_ephemeral_port:" \
    " unable to bind to any ephemeral port. Check firewall\n");

  return -1;
}

std::string
file_to_string (const std::string & filename)
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
    madara_logger_ptr_log (logger::global_logger.get(), logger::LOG_MAJOR,
                           "utility::file_to_string:"           \
                           " failed to open file: %s\n", filename.c_str());
  }

  return buffer.str ();
}


std::string
extract_path (const std::string & name)
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
extract_filename (const std::string & name)
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
expand_envs (const std::string & source)
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
get_var (const std::string & source, 
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
clean_dir_name (const std::string & source)
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

int
  read_file (const std::string & filename,
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
  write_file (const std::string & filename,
  void * buffer, size_t size)
{
  // error is -1
  ssize_t actual = -1;

  // using ACE for writing to the destination file
  ACE_HANDLE file_handle = ACE_OS::open (filename.c_str (),
    O_RDWR | O_CREAT | O_TRUNC,
    ACE_DEFAULT_FILE_PERMS);

  madara_logger_ptr_log (logger::global_logger.get(), logger::LOG_MAJOR,
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

double
rand_double (double floor, double ceiling, bool set_seed_to_time)
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
rand_int (int64_t floor, int64_t ceiling,
                           bool set_seed_to_time)
{
  double random = rand_double (floor, ceiling, set_seed_to_time);
  return nearest_int (random);
}

int64_t
nearest_int (double input)
{
  int change = input >= 0 ? 1 : -1;
  int64_t left = input;
  int64_t right = input + change;

  if (input - left < -input + right)
    return left;
  else
    return right;
}


double sleep (double sleep_time)
{
  ACE_Time_Value actual_time;
  actual_time.set (sleep_time);

  ACE_Time_Value duration = sleep (actual_time);

  double time_taken = (double) duration.sec ();
  time_taken += ((double) duration.usec ()) / micro_per;

  return time_taken;
}

ACE_Time_Value sleep (const ACE_Time_Value & sleep_time)
{
  ACE_Time_Value start = get_ace_time ();
  ACE_Time_Value current = start;
  ACE_Time_Value earliest = current + sleep_time;

  while (current < earliest)
  {
    ACE_Time_Value duration = earliest - current;
#ifdef MADARA_FEATURE_SIMTIME
    double rate = SimTime::rate ();
    duration *= (1 / rate);
    static const int64_t min_sec = simtime_min_sleep / nano_per;
    static const int64_t min_usec = (simtime_min_sleep % nano_per) /
                                    (nano_per / micro_per);
    if (duration.sec () < min_sec ||
        (duration.sec() == min_sec && duration.usec () < min_usec)) {
      duration = ACE_Time_Value (min_sec, min_usec);
    }
#endif
    ACE_OS::sleep (duration);
    current = get_ace_time ();
  }

  return get_ace_time () - start;
}

bool
wait_true (
  knowledge::KnowledgeBase & knowledge,
  const std::string & variable,
  const knowledge::WaitSettings & settings)
{  
  // get current time of day
  ACE_Time_Value current = get_ace_time ();  
  ACE_Time_Value max_wait, sleep_time, next_epoch;
  ACE_Time_Value poll_frequency, last = current;

  knowledge::VariableReference ref = knowledge.get_ref (variable);

  if (settings.poll_frequency >= 0)
  {
    max_wait.set (settings.max_wait_time);
    max_wait = current + max_wait;
    
    poll_frequency.set (settings.poll_frequency);
    next_epoch = current + poll_frequency;
  }

  // print the post statement at highest log level (cannot be masked)
  if (settings.pre_print_statement != "")
    knowledge.print (settings.pre_print_statement, logger::LOG_ALWAYS);

  knowledge::KnowledgeRecord last_value = knowledge.get (ref, settings);

  madara_logger_ptr_log (logger::global_logger.get(), logger::LOG_MAJOR,
    "utility::wait_true:" \
    " variable returned %s\n",
    last_value.to_string ().c_str ());
  
  current = get_ace_time ();

  // wait for expression to be true
  while (!last_value.to_integer () &&
    (settings.max_wait_time < 0 || current < max_wait))
  {
    madara_logger_ptr_log (logger::global_logger.get(), logger::LOG_DETAILED,
      "utility::wait_true:" \
      " current is %" PRIu64 ".%" PRIu64 " and max is %" PRIu64 ".%" PRIu64 " (poll freq is %f)\n",
      current.sec (), current.usec (), max_wait.sec (), max_wait.usec (),
      settings.poll_frequency);

    madara_logger_ptr_log (logger::global_logger.get(), logger::LOG_DETAILED,
      "utility::wait_true:" \
      " last value didn't result in success\n");

    // Unlike the other wait statements, we allow for a time based wait.
    // To do this, we allow a user to specify a 
    if (settings.poll_frequency > 0)
    {
      if (current < next_epoch)
      {
        sleep_time = next_epoch - current;
        sleep (sleep_time);
      }

      next_epoch = next_epoch + poll_frequency;
    }
    else
      knowledge.wait_for_change ();

    madara_logger_ptr_log (logger::global_logger.get(), logger::LOG_DETAILED,
      "utility::wait_true:" \
      " waiting on %s\n", variable.c_str ());

    last_value = knowledge.get (ref, settings);

    madara_logger_ptr_log (logger::global_logger.get(), logger::LOG_DETAILED,
      "utility::wait_true:" \
      " completed eval to get %s\n",
      last_value.to_string ().c_str ());
  
    // get current time
    current = get_ace_time ();

  } // end while (!last)
  
  if (current >= max_wait)
  {
    madara_logger_ptr_log (logger::global_logger.get(), logger::LOG_DETAILED,
      "utility::wait_true:" \
      " Evaluate did not succeed. Timeout occurred\n",
      last_value.to_string ().c_str ());
  }

  // print the post statement at highest log level (cannot be masked)
  if (settings.post_print_statement != "")
    knowledge.print (settings.post_print_statement, logger::LOG_ALWAYS);

  return last_value.is_true ();
}

bool wait_false (
  knowledge::KnowledgeBase & knowledge,
  const std::string & variable,
  const knowledge::WaitSettings & settings)
{
  // get current time of day
  ACE_Time_Value current = get_ace_time ();  
  ACE_Time_Value max_wait, sleep_time, next_epoch;
  ACE_Time_Value poll_frequency, last = current;

  knowledge::VariableReference ref = knowledge.get_ref (variable);

  if (settings.poll_frequency >= 0)
  {
    max_wait.set (settings.max_wait_time);
    max_wait = current + max_wait;
    
    poll_frequency.set (settings.poll_frequency);
    next_epoch = current + poll_frequency;
  }

  // print the post statement at highest log level (cannot be masked)
  if (settings.pre_print_statement != "")
    knowledge.print (settings.pre_print_statement, logger::LOG_ALWAYS);

  knowledge::KnowledgeRecord last_value (!knowledge.get (ref, settings));

  madara_logger_ptr_log (logger::global_logger.get(), logger::LOG_DETAILED,
    "utility::wait_false:" \
    " variable returned %s\n",
    last_value.to_string ().c_str ());
  
  current = get_ace_time ();

  // wait for expression to be true
  while (!last_value.to_integer () &&
    (settings.max_wait_time < 0 || current < max_wait))
  {
    madara_logger_ptr_log (logger::global_logger.get(), logger::LOG_DETAILED,
      "utility::wait_false:" \
      " current is %d.%d and max is %d.%d (poll freq is %f)\n",
      current.sec (), current.usec (), max_wait.sec (), max_wait.usec (),
      settings.poll_frequency);

    madara_logger_ptr_log (logger::global_logger.get(), logger::LOG_DETAILED,
      "utility::wait_false:"
      " last value didn't result in success\n");

    // Unlike the other wait statements, we allow for a time based wait.
    // To do this, we allow a user to specify a 
    if (settings.poll_frequency > 0)
    {
      if (current < next_epoch)
      {
        sleep_time = next_epoch - current;
        sleep (sleep_time);
      }

      next_epoch = next_epoch + poll_frequency;
    }
    else
      knowledge.wait_for_change ();

    madara_logger_ptr_log (logger::global_logger.get(), logger::LOG_MAJOR,
      "utility::wait_false:"
      " waiting on %s\n", variable.c_str ());

    last_value = knowledge::KnowledgeRecord (!knowledge.get (ref, settings));

    madara_logger_ptr_log (logger::global_logger.get(), logger::LOG_DETAILED,
      "utility::wait_false:"
      " completed eval to get %s\n",
      last_value.to_string ().c_str ());
  
    // get current time
    current = get_ace_time ();

  } // end while (!last)
  
  if (current >= max_wait)
  {
    madara_logger_ptr_log (logger::global_logger.get(), logger::LOG_MAJOR,
      "utility::wait_false:"
      " Evaluate did not succeed. Timeout occurred\n",
      last_value.to_string ().c_str ());
  }

  // print the post statement at highest log level (cannot be masked)
  if (settings.post_print_statement != "")
    knowledge.print (settings.post_print_statement, logger::LOG_EMERGENCY);

  return last_value.is_true ();
}

std::pair<std::string, uint16_t> parse_address (std::string addr)
{
  size_t colon_pos = addr.find(':');
  if (colon_pos == std::string::npos || colon_pos >= addr.size() - 1) {
    return {addr, 0};
  }

  int16_t port = std::atoi(addr.c_str() + colon_pos + 1);
  addr.resize(colon_pos);
  return {std::move(addr), port};
}

} }
