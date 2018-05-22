
#include "stdafx.h"
#include <ctype.h>
#include <stdlib.h>
#include "Utility.h"
#include <iostream>
#include <sstream>
#include <Console.h>
#include <fstream>

/// Convert string to uppercase
::std::string &
KATS::Utility::upper (::std::string &input)
{
  for (::std::string::iterator cur = input.begin ();
       cur != input.end (); ++cur)
    *cur = toupper (*cur);

  return input;
}

/// Convert string to lowercase 
::std::string & 
KATS::Utility::dds_topicify (::std::string & input)
{
  for (::std::string::iterator cur = input.begin ();
       cur != input.end (); ++cur)
  {
    // change periods to _
    if (*cur == '.')
      *cur = '_';
  }

  return input;
}

/// Convert string to lowercase 
::std::string &
KATS::Utility::lower (::std::string &input)
{
  for (::std::string::iterator cur = input.begin ();
       cur != input.end (); ++cur)
    *cur = tolower (*cur);

  return input;
}

/// Strip whitespace from front and end of string and also
/// condense multiple whitespace into a single space
::std::string &
KATS::Utility::strip_extra_white_space (::std::string & input)
{
  ::std::string::iterator cur = input.begin ();
  char prev = 0;

  for (::std::string::iterator eval = cur; 
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
::std::string &
KATS::Utility::strip_white_space (::std::string & input)
{
  ::std::string::iterator cur = input.begin ();
  char prev = 0;

  for (::std::string::iterator eval = cur; 
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



/// Split a string into tokens 
void 
KATS::Utility::tokenizer (const ::std::string & input, 
    const ::std::vector< ::std::string> & splitters,
    ::std::vector< ::std::string> & tokens,
    ::std::vector< ::std::string> & pivots)
{
  ::std::string::size_type last = 0;
  ::std::string::size_type cur = 0;
  tokens.clear ();
  pivots.clear ();

  for (; cur < input.size (); ++cur)
  {
    for (::std::string::size_type i = 0; i < splitters.size (); ++i)
    {
      // if the splitter string length is greater than zero
      if (splitters[i].size () > 0)
      {
        // if the first char of the splitter string is equal to the char
        if (input[cur] == splitters[i][0])
        {
          ::std::string::size_type checker = cur;
          ::std::string::size_type j = 1;
          for (++checker; 
            checker < input.size () && j < splitters[i].size () &&
            input[checker] == splitters[i][j];
            ++j, ++checker);

          // we have found a splitter. Tokenize from last to splitter.
          if (j == splitters[i].size ())
          {
            // need to update this to only have as many pivots as tokens - 1
            pivots.push_back (input.substr (cur, j));

            if (cur - last >= j)
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

std::string
KATS::Utility::file_to_string (const std::string & filename)
{
  std::string line;
  std::stringstream buffer;

  std::ifstream file (filename.c_str ());

  // if the file was able to open
  if (file.is_open ())
  {
    // while there is still a line left in the file, read that line
    // into our stringstream buffer
    while (std::getline (file, line))
      buffer << line << "\n";
    file.close ();
  }

  return buffer.str ();
}


std::string
KATS::Utility::extract_path (const std::string & name)
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
KATS::Utility::extract_filename (const std::string & name)
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
KATS::Utility::expand_model_vars (KATS_BON::ProcessBase & current,
                                    const std::string & source)
{
  std::stringstream buffer;

  for (size_t i = 0; i < source.size (); ++i)
  {
    // environment variable must be larger than $()
    if (source[i] == '$' && i + 3 < source.size ())
    {
      size_t saved = i;
      std::string variable = get_var (source, i+2, i);
 
      std::string value = "";

      if      ("MODEL.NAME"  == variable)
      {
        std::stringstream buffer;
        buffer << current->getName ();
        value = buffer.str ();
      }
      else if ("MODEL.ID"    == variable)
      {
        std::stringstream buffer;
        buffer << current->getId ();
        value = buffer.str ();
      }
      else if ("MODEL.PROCESSES"    == variable)
      {
        std::stringstream buffer;
        buffer << current->getProcesses ();
        value = buffer.str ();
      }
      else if ("BARRIER.NAME"    == variable)
      {
        // get the barrier name
        std::set<KATS_BON::BarrierRef> barriers = current->getBarrierRef ();
        std::set<KATS_BON::BarrierRef>::iterator bbegin = barriers.begin();
        size_t barrier_count = 0;
        bool override_processes = false;
        if (bbegin != barriers.end ())
        {
          KATS_BON::BarrierRef bref = *bbegin;
          KATS_BON::Barrier barrier = bref->getBarrier ();

          Util::GenRefCounted * ref_ptr = barrier.getCounted (false);

          // check to see if the user initialized the reference to an
          // actual instance. If not, we use the reference's name as
          // a convenience
          if (ref_ptr)
          {
            std::stringstream buffer;
            buffer << barrier->getName ();
            value = buffer.str ();
          }
        }
      }
      else
      {
        if (i < source.size () && (
          source[i] == ')' || source[i] == '}'))
        {
          value = source.substr (saved, i - saved + 1);
        }
      }

      if (value != "")
        buffer << value;
    }
    else
      buffer << source[i];
  }
  return buffer.str ();
}

/// Expand any environment variables in a string
std::string
KATS::Utility::expand_model_vars (KATS_BON::GroupRef & current,
                                    const std::string & source)
{
  std::stringstream buffer;

  for (size_t i = 0; i < source.size (); ++i)
  {
    // environment variable must be larger than $()
    if (source[i] == '$' && i + 3 < source.size ())
    {
      size_t saved = i;
      std::string variable = get_var (source, i+2, i);
 
      std::string value = "";

      if      ("MODEL.NAME"  == variable)
      {
        std::stringstream buffer;
        buffer << current->getName ();
        value = buffer.str ();
      }
      else if ("MODEL.ID"    == variable)
      {
        std::stringstream buffer;
        buffer << current->getId ();
        value = buffer.str ();
      }
      else if ("MODEL.PROCESSES"    == variable)
      {
        std::stringstream buffer;
        buffer << current->getProcesses ();
        value = buffer.str ();
      }
      else if ("BARRIER.NAME"    == variable)
      {
        std::stringstream buffer;
        buffer << current->getOverrideBarrier ();
        value = buffer.str ();
      }
      else
      {
        if (i < source.size () && (
          source[i] == ')' || source[i] == '}'))
        {
          value = source.substr (saved, i - saved + 1);
        }
      }

      if (value != "")
        buffer << value;
    }
    else
      buffer << source[i];
  }
  return buffer.str ();
}

/// grab an environment variable value (@see expand_envs)
std::string
KATS::Utility::get_var (const std::string & source, 
                          size_t cur, size_t & end)
{
  for (end = cur; end < source.size (); ++end)
  {
    if (source[end] == ')' || source[end] == '}')
    {
      return source.substr (cur, end - cur);
    }
  }

  return source.substr (cur, end - cur);
}

std::string
KATS::Utility::clean_dir_name (const std::string & source)
{
  // define the characters we'll want to replace
#ifdef _WIN32
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