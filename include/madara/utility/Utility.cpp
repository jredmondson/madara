#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include <ios>
#include <iostream>
#include <sstream>
#include <fstream>
#include <thread>

#include "madara/logger/GlobalLogger.h"
#include "madara/utility/Utility.h"
#include "madara/utility/EpochEnforcer.h"

#include "madara/knowledge/KnowledgeBase.h"

#include "Utility.h"
#include "Timer.h"

namespace madara
{
namespace utility
{
std::string get_version(void)
{
#include "madara/Version.h"

  return version + std::string(" compiled on ") + __DATE__ + " at " + __TIME__;
}

uint32_t get_uint_version(const std::string& str_version)
{
  unsigned char version_buffer[4] = {0, 0, 0, 0};
  uint32_t* return_value = (uint32_t*)version_buffer;
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

std::string to_string_version(uint32_t version)
{
  std::stringstream new_version;
  unsigned char* version_ptr = (unsigned char*)&version;
  unsigned int major, minor, revision;

  major = (unsigned int)version_ptr[1];
  minor = (unsigned int)version_ptr[2];
  revision = (unsigned int)version_ptr[3];

  new_version << major;
  new_version << '.';
  new_version << minor;
  new_version << '.';
  new_version << revision;

  return new_version.str();
}

/// Strip whitespace from front and end of string and also
/// condense multiple whitespace into a single space
std::string& strip_extra_white_space(std::string& input)
{
  std::string::iterator cur = input.begin();
  char prev = 0;

  for (std::string::iterator eval = cur; eval != input.end(); ++eval)
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
  if (cur != input.end())
  {
    --cur;
    if (*cur != ' ' && *cur != '\t' && *cur != '\n' && *cur != '\r')
      ++cur;
  }

  // erase everything from cur to end of input string
  if (cur != input.end())
    input.erase(cur, input.end());

  return input;
}

/// Strip all whitespace
std::string& strip_white_space(std::string& input)
{
  std::string::iterator cur = input.begin();

  for (std::string::iterator eval = cur; eval != input.end(); ++eval)
  {
    // if it isn't whitespace, then copy it over immediately
    if (*eval != ' ' && *eval != '\t' && *eval != '\n' && *eval != '\r')
    {
      *cur = *eval;
      ++cur;
    }
  }

  // erase everything from cur to end of input string
  if (cur != input.end())
    input.erase(cur, input.end());

  return input;
}

/**
 * Strips an unwanted character. This function will
 * modify and return the input, so make a copy if needed.
 **/
std::string& string_remove(std::string& input, char unwanted)
{
  std::string::iterator cur = input.begin();

  for (std::string::iterator eval = cur; eval != input.end(); ++eval)
  {
    // if it isn't whitespace, then copy it over immediately
    if (*eval != unwanted)
    {
      *cur = *eval;
      ++cur;
    }
  }

  // erase everything from cur to end of input string
  if (cur != input.end())
    input.erase(cur, input.end());

  return input;
}

size_t string_replace(std::string& source, const std::string& old_phrase,
    const std::string& new_phrase, bool replace_all)
{
  // return value
  size_t replacements = 0;

  if (old_phrase != "")
  {
    for (std::string::size_type i = source.find(old_phrase, 0);
         i != source.npos; i = source.find(old_phrase, i))
    {
      source.replace(i, old_phrase.size(), new_phrase);
      i += new_phrase.size();

      ++replacements;

      if (!replace_all)
        break;
    }
  }

  return replacements;
}

std::string& strip_comments(std::string& input)
{
  std::stringstream source, dest;
  std::string cur;
  std::vector<std::string> splitters;
  splitters.resize(2);

  splitters[0] = "//";

  // place the input in the string stream
  source << input;

  while (std::getline(source, cur))
  {
    std::vector<std::string> tokens;
    std::vector<std::string> pivots;
    tokenizer(cur, splitters, tokens, pivots);

    if (tokens.size())
    {
      dest << tokens[0];
      dest << "\n";
    }
  }

  input = dest.str();
  return input;
}

/// Split a string into tokens
void tokenizer(const std::string& input,
    const ::std::vector<std::string>& splitters,
    ::std::vector<std::string>& tokens, ::std::vector<std::string>& pivots)
{
  std::string::size_type last = 0;
  std::string::size_type cur = 0;
  tokens.clear();
  pivots.clear();

  for (; cur < input.size(); ++cur)
  {
    for (std::string::size_type i = 0; i < splitters.size(); ++i)
    {
      // if the splitter string length is greater than zero
      if (splitters[i].size() > 0)
      {
        // if the first char of the splitter string is equal to the char
        if (input[cur] == splitters[i][0])
        {
          std::string::size_type checker = cur;
          std::string::size_type j = 1;
          for (++checker; checker < input.size() && j < splitters[i].size() &&
                          input[checker] == splitters[i][j];
               ++j, ++checker)
            ;

          // we have found a splitter. Tokenize from last to splitter.
          if (j == splitters[i].size())
          {
            // need to update this to only have as many pivots as tokens - 1
            pivots.push_back(input.substr(cur, j));

            if (cur - last >= splitters[i].size() - 1)
              tokens.push_back(input.substr(last, cur - last));
            else
              tokens.push_back("");

            // we want last to point to the last valid token begin
            cur = checker - 1;
            last = checker;
          }  // if found a splitter
        }    // if first char == splitter first char
      }      // if splitter length >= 1
    }        // for splitters

  }  // for chars

  if (last != cur)
  {
    tokens.push_back(input.substr(last, cur - last));
  }
}

// split a key into a corresponding host and port
int split_hostport_identifier(
    const std::string& key, std::string& host, std::string& port)
{
  // delimiter is either a : or an @
  std::string::size_type delim = key.rfind(':');
  delim = delim == key.npos ? key.rfind('@') : delim;

  // no delimiter found
  if (delim == key.npos)
  {
    host = key;
    port = "";

    return 1;
  }

  // otherwise, set the host and port appropriately
  host = key.substr(0, delim);
  port = key.substr(delim + 1, key.size() - delim);

  return 0;
}

// merge a host and port into a key
int merge_hostport_identifier(
    std::string& key, const std::string& host, const std::string& port)
{
  if ((const std::string*)&key != &host)
    key = host;

  key += ':';
  key += port;

  return 0;
}

// merge a host and ushort port into a key
int merge_hostport_identifier(
    std::string& key, const std::string& host, unsigned short u_port)
{
  std::stringstream port_stream;
  port_stream << u_port;

  return merge_hostport_identifier(key, host, port_stream.str());
}

std::string file_to_string(const std::string& filename)
{
  std::string line;
  std::stringstream buffer;

  std::ifstream file(filename.c_str());

  // if the file was able to open
  if (file.is_open())
  {
    std::getline(file, line);

    if (line != "")
      buffer << line;

    // while there is still a line left in the file, read that line
    // into our stringstream buffer
    while (std::getline(file, line))
      buffer << "\n" << line;
    file.close();
  }
  else
  {
    madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_MAJOR,
        "utility::file_to_string:"
        " failed to open file: %s\n",
        filename.c_str());
  }

  return buffer.str();
}

/// Expand any environment variables in a string
std::string expand_envs(const std::string& source)
{
  std::stringstream buffer;

  for (size_t i = 0; i < source.size(); ++i)
  {
    // environment variable must be larger than $()
    if (source[i] == '$' && i + 3 < source.size())
    {
      char* value = get_var(source, i + 2, i);
      if (value)
        buffer << value;
    }
    else
      buffer << source[i];
  }
  return buffer.str();
}

/// grab an environment variable value (@see expand_envs)
char* get_var(const std::string& source, size_t cur, size_t& end)
{
  for (end = cur; end < source.size(); ++end)
  {
    if (source[end] == ')' || source[end] == '}')
    {
      return getenv(source.substr(cur, end - cur).c_str());
    }
  }

  return getenv(source.substr(cur, source.size() - cur).c_str());
}

std::string clean_dir_name(const std::string& source)
{
// define the characters we'll want to replace
#ifdef _WIN32
#define REPLACE_THIS '/'
#define REPLACE_WITH '\\'
#else
#define REPLACE_THIS '\\'
#define REPLACE_WITH '/'
#endif

  std::string target(source);

  for (std::string::iterator i = target.begin(); i != target.end(); ++i)
  {
    if (*i == REPLACE_THIS)
      *i = REPLACE_WITH;
  }

  return target;
}

int read_file(const std::string& filename, void*& buffer, size_t& size,
    bool add_zero_char)
{
  int ret_value = 0;
  size = 0;

  if (filename != "")
  {
    try
    {
      std::ifstream file(filename, std::ifstream::binary);

      if (file)
      {
        file.seekg(0, file.end);
        size = (size_t)file.tellg();
        file.seekg(0, file.beg);

        madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_MAJOR,
            "utility::read_file:"
            " reading %d bytes from %s\n",
            (int)size, filename.c_str());

        if (add_zero_char)
        {
          ++size;
        }

        buffer = new unsigned char[size];

        if (size > 0)
        {
          file.read((char*)buffer, size);

          madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_MAJOR,
              "utility::read_file:"
              " successfully read %d bytes from %s\n",
              (int)size, filename.c_str());

          if (add_zero_char)
          {
            unsigned char* zeroed = (unsigned char*)buffer;
            zeroed[size - 1] = 0;
          }  // end if adding a zero char

          ret_value = 0;
        }

        file.close();
      }  // end if file is open
      else
        ret_value = -1;
    }  // end try
    catch (const std::exception& e)
    {
      madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_ALWAYS,
          "utility::read_file:"
          " exception: %s\n",
          e.what());

      ret_value = -1;
    }
  }
  else
    ret_value = -1;

  return ret_value;
}

ssize_t write_file(const std::string& filename, void* buffer, size_t size)
{
  // error is -1
  ssize_t actual = -1;

  std::ofstream file;

  try
  {
    file.open(filename, std::ios::out | std::ios::binary);
    if (file.write((char*)buffer, size))
    {
      actual = size;
    }
    file.close();

    madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_MAJOR,
        "utility::write_file:"
        " wrote %d bytes to %s\n",
        (int)actual, filename.c_str());
  }
  catch (const std::exception& e)
  {
    madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_ALWAYS,
        "utility::write_file:"
        " exception: %s\n",
        e.what());
  }

  // return the actual bytes written. -1 if error
  return actual;
}

double rand_double(double floor, double ceiling, bool set_seed_to_time)
{
  // check if the user has specified setting through srand
  if (set_seed_to_time)
  {
    srand((unsigned int)get_time());
  }

  // Get a double number between 0 and 1.
  double position_in_range = ((double)rand()) / ((double)RAND_MAX);

  if (floor < ceiling)
    return (position_in_range * (ceiling - floor)) + floor;
  else
    return (position_in_range * (floor - ceiling)) + ceiling;
}

int64_t rand_int(int64_t floor, int64_t ceiling, bool set_seed_to_time)
{
  double random = rand_double((double)floor, (double)ceiling, set_seed_to_time);
  return nearest_int(random);
}

int64_t nearest_int(double input)
{
  int change = input >= 0 ? 1 : -1;
  int64_t left = (int64_t)input;
  int64_t right = (int64_t)input + change;

  if (input - left < -input + right)
    return left;
  else
    return right;
}

double sleep(double sleep_time)
{
  SecondsDuration period = seconds_to_seconds_duration(sleep_time);
  period = sleep(period);

  return period.count();
}

SecondsDuration sleep(const SecondsDuration& sleep_time)
{
#ifdef MADARA_FEATURE_SIMTIME
  static const SecondsDuration max_sleep{0.01};
#endif

  using TVal = std::chrono::time_point<Clock, SecondsDuration>;

  TVal start = get_time_value();
  TVal target = start + sleep_time;
  TVal current;

  while ((current = get_time_value()) < target)
  {
#ifndef MADARA_FEATURE_SIMTIME
    std::this_thread::sleep_until(target);
#else
    double rate = utility::SimTime::rate();
    SecondsDuration actual_dur = ((target - current) / rate);
#if 0
    std::cerr << (target - current).count() << " "  << rate << " " <<
      actual_dur.count() << std::endl;
#endif
    TVal actual_current = Clock::now();
    TVal actual_target = actual_current + actual_dur;
    TVal max_target = actual_current + max_sleep;
#if 0
    std::cerr <<
      start.time_since_epoch().count() << " " <<
      target.time_since_epoch().count() << " " <<
      current.time_since_epoch().count() << " " <<
      SimTime::time() << " " <<
      actual_target.time_since_epoch().count() << " " <<
      max_target.time_since_epoch().count() << std::endl;
#endif
    if (actual_target < max_target)
    {
      std::this_thread::sleep_until(actual_target);
    }
    else
    {
      std::this_thread::sleep_until(max_target);
    }
#endif
  }

  return current - start;
}

Duration sleep_until(TimeValue wake)
{
#ifdef MADARA_FEATURE_SIMTIME
  static const Duration max_sleep{10000000};
#endif

  TimeValue start = get_time_value();;
  TimeValue current;

  while ((current = get_time_value()) < wake)
  {
#ifndef MADARA_FEATURE_SIMTIME
    std::this_thread::sleep_until(wake);
#else
#if 0
    std::cerr << (target - current).count() << " "  << rate << " " <<
      actual_dur.count() << std::endl;
#endif
    TimeValue actual_target =
      TimeValue(Duration(SimTime::realtime(wake.time_since_epoch().count())));

    TimeValue actual_current = Clock::now();
    TimeValue max_target = actual_current + max_sleep;
#if 0
    std::cerr <<
      start.time_since_epoch().count() << " " <<
      target.time_since_epoch().count() << " " <<
      current.time_since_epoch().count() << " " <<
      SimTime::time() << " " <<
      actual_target.time_since_epoch().count() << " " <<
      max_target.time_since_epoch().count() << std::endl;
#endif
    if (actual_target < max_target)
    {
      std::this_thread::sleep_until(actual_target);
    }
    else
    {
      std::this_thread::sleep_until(max_target);
    }
#endif
  }

  return current - start;
}

Duration sleep_until(uint64_t wake)
{
  return sleep_until(TimeValue(Duration(wake)));
}

bool wait_true(knowledge::KnowledgeBase& knowledge, const std::string& variable,
    const knowledge::WaitSettings& settings)
{
  // use the EpochEnforcer utility to keep track of sleeps
  EpochEnforcer<std::chrono::steady_clock> enforcer(
      settings.poll_frequency, settings.max_wait_time);

  knowledge::VariableReference ref = knowledge.get_ref(variable);

  // print the post statement at highest log level (cannot be masked)
  if (settings.pre_print_statement != "")
    knowledge.print(settings.pre_print_statement, logger::LOG_ALWAYS);

  knowledge::KnowledgeRecord last_value = knowledge.get(ref, settings);

  madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_MAJOR,
      "utility::wait_true:"
      " variable returned %s\n",
      last_value.to_string().c_str());

  // wait for expression to be true
  while (!last_value.is_true() &&
         (settings.max_wait_time < 0 || !enforcer.is_done()))
  {
    madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_DETAILED,
        "utility::wait_true:"
        " last value didn't result in success\n");

    // Unlike the other wait statements, we allow for a time based wait.
    // To do this, we allow a user to specify a
    if (settings.poll_frequency > 0)
    {
      enforcer.sleep_until_next();
    }
    else
    {
      knowledge.wait_for_change();
    }

    madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_DETAILED,
        "utility::wait_true:"
        " waiting on %s\n",
        variable.c_str());

    last_value = knowledge.get(ref, settings);

    madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_DETAILED,
        "utility::wait_true:"
        " completed eval to get %s\n",
        last_value.to_string().c_str());
  }  // end while (!last)

  if (enforcer.is_done())
  {
    madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_DETAILED,
        "utility::wait_true:"
        " Evaluate did not succeed. Timeout occurred\n",
        last_value.to_string().c_str());
  }

  // print the post statement at highest log level (cannot be masked)
  if (settings.post_print_statement != "")
    knowledge.print(settings.post_print_statement, logger::LOG_ALWAYS);

  return last_value.is_true();
}

bool wait_false(knowledge::KnowledgeBase& knowledge,
    const std::string& variable, const knowledge::WaitSettings& settings)
{
  // use the EpochEnforcer utility to keep track of sleeps
  EpochEnforcer<std::chrono::steady_clock> enforcer(
      settings.poll_frequency, settings.max_wait_time);

  knowledge::VariableReference ref = knowledge.get_ref(variable);

  // print the post statement at highest log level (cannot be masked)
  if (settings.pre_print_statement != "")
    knowledge.print(settings.pre_print_statement, logger::LOG_ALWAYS);

  knowledge::KnowledgeRecord last_value(!knowledge.get(ref, settings));

  madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_DETAILED,
      "utility::wait_false:"
      " variable returned %s\n",
      last_value.to_string().c_str());

  // wait for expression to be true
  while (last_value.is_true() &&
         (settings.max_wait_time < 0 || !enforcer.is_done()))
  {
    madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_DETAILED,
        "utility::wait_false:"
        " last value didn't result in success\n");

    // Unlike the other wait statements, we allow for a time based wait.
    // To do this, we allow a user to specify a
    if (settings.poll_frequency > 0)
    {
      enforcer.sleep_until_next();
    }
    else
    {
      knowledge.wait_for_change();
    }

    madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_MAJOR,
        "utility::wait_false:"
        " waiting on %s\n",
        variable.c_str());

    last_value = knowledge::KnowledgeRecord(!knowledge.get(ref, settings));

    madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_DETAILED,
        "utility::wait_false:"
        " completed eval to get %s\n",
        last_value.to_string().c_str());
  }  // end while (!last)

  if (enforcer.is_done())
  {
    madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_MAJOR,
        "utility::wait_false:"
        " Evaluate did not succeed. Timeout occurred\n",
        last_value.to_string().c_str());
  }

  // print the post statement at highest log level (cannot be masked)
  if (settings.post_print_statement != "")
    knowledge.print(settings.post_print_statement, logger::LOG_EMERGENCY);

  return last_value.is_true();
}

std::pair<std::string, uint16_t> parse_address(std::string addr)
{
  size_t colon_pos = addr.find(':');
  if (colon_pos == std::string::npos || colon_pos >= addr.size() - 1)
  {
    return {addr, 0};
  }

  int16_t port = std::atoi(addr.c_str() + colon_pos + 1);
  addr.resize(colon_pos);
  return {std::move(addr), port};
}
}
}
