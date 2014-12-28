#ifndef UTILITY_H
#define UTILITY_H

#include <vector>
#include <string>
#include "ace/SOCK_Dgram.h"
#include "ace/High_Res_Timer.h"
#include "madara/MADARA_export.h"
#include "madara/utility/stdint.h"


namespace Madara
{
  namespace Utility
  {
    /**
     * Gets the MADARA version number
     * @return    the MADARA version number
     **/
    MADARA_Export std::string get_version (void);
    
    /**
     * Gets the MADARA version number
     * @return    the MADARA 4 byte version number
     **/
    MADARA_Export uint32_t get_uint_version (void);
    
    /**
     * Converts a MADARA uint32_t version number to human-readable
     * @param   version     the 4 byte version number
     * @return              the stringified version number
     **/
    MADARA_Export std::string to_string_version (uint32_t version);
    
    /**
     * Replaces an old phrase with a new phrase within a string
     * @param   source     source string to change
     * @param   old_phrase  old phrase in the source string to replace
     * @param   new_phrase  new phrase to replace the old phrase
     * @param   replace_all if true, replace all instances of old phrase.
     *                      if false, replace first instance of old phrase.
     * @return  the number of replacements made
     **/
    MADARA_Export size_t string_replace (std::string & source,
      const std::string & old_phrase,
      const std::string & new_phrase,
      bool replace_all = true);
    
    /**
     * Converts the string to upper. This function will modify and return the
     * input, so make a copy if needed.
     * @param     input         the string to change
     * @return    a reference to the modified input
     **/
    MADARA_Export std::string & upper (std::string & input);
    
    /**
     * Converts the string to lower. This function will modify and return the
     * input, so make a copy if needed.
     * @param     input         the string to change
     * @return    a reference to the modified input
     **/
    MADARA_Export std::string & lower (std::string & input);
    
    /**
     * Changes periods to underscores in compliance with OpenSplice needs.
     * This function will modify and return the input, so make a copy if
     * needed.
     * @param     input         the string to change
     * @return    a reference to the modified input
     **/
    MADARA_Export std::string & dds_topicify (std::string & input);
    
    /**
     * Strips all whitespace characters from a string. This function will
     * modify and return the input, so make a copy if needed.
     * @param     input         the string to change
     * @return    a reference to the modified input
     **/
    MADARA_Export std::string & strip_white_space (std::string & input);
    
    /**
     * Strips all comments (single-line and multi-line). This function will
     * modify and return the input, so make a copy if needed.
     * @param     input         the string to change
     * @return    a reference to the modified input
     **/
    MADARA_Export std::string & strip_comments (std::string & input);
    
    /**
     * Strips an unwanted character. This function will
     * modify and return the input, so make a copy if needed.
     * @param     input         the string to change
     * @param     unwanted      the character value to remove from input
     * @return    a reference to the modified input
     **/
    MADARA_Export std::string & string_remove (std::string & input,
      char unwanted);

    /**
     * Strips whitespace from front and end of string and also
     * condenses multiple whitespace into a single space. This function will
     * modify and return the input, so make a copy if needed.
     * @param     input         the string to change
     * @return    a reference to the modified input
     **/
    MADARA_Export std::string & strip_extra_white_space (
      std::string & input);
    
    /**
     * Splits a key of host:port into a corresponding host and port.
     * @param     key         a string containing a host:port
     * @param     host        the host in the key
     * @param     port        the port in the key
     * @return    1 if there was no ":". 0 if both host and port are set.
     **/
    MADARA_Export int split_hostport_identifier (const std::string & key, 
      std::string & host, std::string & port);
    
    /**
     * Merges a host and port into a host:port key
     * @param     key         a string containing a host:port
     * @param     host        the host in the key
     * @param     port        the port in the key
     * @return    0 if successful (always successful right now)
     **/
    MADARA_Export int merge_hostport_identifier (std::string & key, 
      const std::string & host, const std::string & port);
    
    /**
     * Merges a host and port into a host:port key
     * @param     key         a string containing a host:port
     * @param     host        the host in the key
     * @param     u_port      the port in the key
     * @return    0 if successful (always successful right now)
     **/
    MADARA_Export int merge_hostport_identifier (std::string & key, 
      const std::string & host, unsigned short u_port);
    
    /**
     * Splits an input string into tokens.
     * @param     input       a string to be split by splitters
     * @param     splitters   a vector of strings to look for in input
     * @param     tokens      the sections in between splitters
     * @param     pivot_list  the actual splitters that were found between
     *                        the tokens
     **/
    MADARA_Export void tokenizer (const std::string & input, 
      const ::std::vector< std::string> & splitters,
      ::std::vector< std::string> & tokens,
      ::std::vector< std::string> & pivot_list);
    
    /**
     * Binds to an ephemeral port
     * @param     socket      socket to bind to unique ephemeral
     * @param     host        the host name of this device or computer
     * @param     port        starting port to try to bind to
     * @param     increase_until_bound   if true, increment port until success
     * @return    0 if successful bind. -1 if unsuccessful bind.
     **/
    MADARA_Export int bind_to_ephemeral_port (ACE_SOCK_Dgram & socket,
      std::string & host, unsigned short & port,
      bool increase_until_bound = true);
    
    /**
     * Reads a file into a string
     * @param     filename    name of the file to read
     * @return    contents of the file. Null string if unsuccessful.
     **/
    MADARA_Export std::string file_to_string (const std::string & filename);

    /**
     * Extracts the path of a filename
     * @param     name        name of the file to extract path of
     * @return    the directory path of the file
     **/
    MADARA_Export std::string extract_path (const std::string & name);
    
    /**
     * Extracts the file name of an absolute or relative path
     * @param     name        name of the file to extract path of
     * @return    the file name
     **/
    MADARA_Export std::string extract_filename (const std::string & name);

    /**
     * Expands environment variables referenced in the string. The environment
     * variables must be specified as $(var) and not $var.
     * @param     source      the string to expand
     * @return    a string with environment variables expanded
     **/
    MADARA_Export std::string expand_envs (
      const std::string & source);

    /**
     * Helper function for Madara::Utility::expand_envs which retrieves an
     * environment variable.
     * @param     source      the string to expand
     * @param     cur         the current focus position of expand_vars
     * @param     end         the end of the variable
     * @return    the value of the environment variable
     **/
    char * get_var (const std::string & source, size_t cur, size_t & end);
    
    /**
     * Substitutes the appropriate directory delimiter, which may help
     * with portability between operating systems.
     * @param     target      the string to modify
     * @return    a more compliant directory path string
     **/
    MADARA_Export std::string clean_dir_name (const std::string & target);
    
    /**
     * Converts a host format uint64_t into big endian. Can
     * also be used to convert back into host form.
     * @param     value      the value to convert
     * @return    the converted value
     **/
    MADARA_Export uint64_t endian_swap (uint64_t value);
    
    /**
     * Converts a host format signed Madara::Knowledge_Record::Integer
     * into big endian.  Can also be used to convert back into host form.
     * @param     value      the value to convert
     * @return    the converted value
     **/
    MADARA_Export int64_t endian_swap (int64_t value);
    
    /**
     * Converts a host format uint64_t into big endian. Can
     * also be used to convert back into host form.
     * @param     value      the value to convert
     * @return    the converted value
     **/
    MADARA_Export uint32_t endian_swap (uint32_t value);
    
    /**
     * Converts a host format signed Madara::Knowledge_Record::Integer
     * into big endian.  Can also be used to convert back into host form.
     * @param     value      the value to convert
     * @return    the converted value
     **/
    MADARA_Export int32_t endian_swap (int32_t value);
    
    /**
     * Converts a host format double precision into big endian. Can
     * also be used to convert back into host form.
     * @param     value      the value to convert
     * @return    the converted value
     **/
    MADARA_Export double endian_swap (double value);
    
    /**
     * Reads a file into a provided void pointer. The void pointer will point
     * to an allocated buffer that the user will need to delete.
     * @param    filename   the name of the file
     * @param    buffer     a buffer that contains the contents of the file
     * @param    size       the size of the allocated buffer (will change)
     * @param    add_zero_char  add a zero char to the end of the buffer
     * @return              zero if successful
     **/
    MADARA_Export int  read_file (const std::string & filename,
      void *& buffer, size_t & size, bool add_zero_char = false);
    
    /**
     * Writes a file with provided contents.
     * @param    filename   the name of the file
     * @param    buffer     a buffer that contains the contents of the file
     * @param    size       the size of the allocated buffer
     * @return              amount of bytes written (-1 if unsuccessful)
     **/
    MADARA_Export ssize_t write_file (const std::string & filename,
      void * buffer, size_t size);

    /**
     * Returns a time of day in nanoseconds
     * @return  time of day in nanoseconds
     **/
    MADARA_Export int64_t get_time (void);

    /**
     * Returns a random double between floor and ceiling
     * @param  floor      lowest possible double
     * @param  ceiling    highest possible double
     * @param  set_seed_to_time  if true, seed the random generator with time
     * @return a double between floor and ceiling (inclusive)
     **/
    MADARA_Export double rand_double (double floor = 0.0,
      double ceiling = 1.0, bool set_seed_to_time = true);
     
    /**
     * Returns a random integer between a floor and ceiling
     * @param  floor      lowest possible integer
     * @param  ceiling    highest possible integer
     * @param  set_seed_to_time  if true, seed the random generator with time
     * @return a integer between floor and ceiling (inclusive)
     **/
    MADARA_Export int64_t rand_int (int64_t floor = 0,
      int64_t ceiling = RAND_MAX, bool set_seed_to_time = true);

    /**
     * Rounds a double to the nearest integer
     * @param  input      real number to round to integer
     * @return the nearest integer to the input
     **/
    MADARA_Export int64_t nearest_int (double input);

    /**
     * Check if input contains prefix at the beginning
     * @param  input     string to check for prefix
     * @param  prefix    string to match
     * @return true if input begins with prefix. False otherwise.
     **/
    MADARA_Export bool begins_with (const std::string & input,
      const std::string & prefix);
    
    /**
     * Check if input contains a pattern at the end
     * @param  input     string to check for prefix
     * @param  ending    string to match
     * @return true if input ends with ending. False otherwise.
     **/
    MADARA_Export bool ends_with (const std::string & input,
      const std::string & ending);
    
    /**
     * Sleeps for a certain amount of time. Unlike other sleep timers
     * this will sleep for the amount of time regardless of OS interrupts
     * @param  sleep_time  time in seconds to sleep for
     **/
    MADARA_Export double sleep (double sleep_time);
    
    /**
     * Sleeps for a certain amount of time. Unlike other sleep timers
     * this will sleep for the amount of time regardless of OS interrupts
     * @param  sleep_time  time to sleep for
     **/
    MADARA_Export ACE_Time_Value sleep (const ACE_Time_Value & sleep_time);

    /**
     * Checks if a file exists
     * @param filename   path and name of the file to open
     * @return true if file exists. false otherwise.
     **/
    MADARA_Export bool file_exists (const std::string & filename);

    /**
     * Returns the size of a file
     * @param filename   path and name of the file to open
     * @return size of the file
     **/
    MADARA_Export unsigned int file_size (const std::string & filename);

    /**
     * Returns true if left < right
     **/
    template <typename T>
    bool less_compare (const T & left, const T & right);

    /**
     * Returns true if right < left
     **/
    template <typename T>
    bool greater_compare (const T & left, const T & right);

    /**
     * Sifts elements down a heap according to a comparator
     * @param  input       the array to heapify
     * @param  start       index to start from
     * @param  end         index that signifies end of range to sift
     * @param  comparator  a function for comparing two elements
     **/
    template <typename T>
    void sift_down (T * input, int start, int end,
      bool (*comparator) (const T & left, const T & right) =
        greater_compare<T>);
    
    /**
     * Builds a heap out of an array of elements
     * @param  input       the array to heapify
     * @param  size        the size of the array
     * @param  comparator  a function for comparing two elements
     **/
    template <typename T>
    void heapify (T * input, int size,
     bool (*comparator) (const T & left, const T & right) =
        greater_compare<T>);

    /**
     * Sorts an array with heap sort
     * @param  input       the array to heapify
     * @param  size        the size of the array
     * @param  comparator  a function for comparing two elements
     **/
    template <typename T>
    void heap_sort (T * input, int size,
     bool (*comparator) (const T & left, const T & right) =
        greater_compare<T>);

    /**
     * Adds values to a bit mask
     * @param   mask    the target bit mask
     * @param   values  values to bitwise add to mask
     * @return  resulting mask
     **/
    template <typename T>
    T bitmask_add (T mask, T values);
    
    /**
     * Returns true if mask contains values
     * @param   mask    the bit mask to check
     * @param   values  values to check for inclusion in mask
     * @return  true if values in exist in mask, false otherwise
     **/
    template <typename T>
    bool bitmask_check (T mask, T values);
    
    /**
     * Removes values from a bit mask
     * @param   mask    the target bit mask
     * @param   values  values to bitwise remove from mask
     * @return  resulting mask
     **/
    template <typename T>
    T bitmask_remove (T mask, T values);
  }
}

#include "Utility.inl"

#endif
