#ifndef UTILITY_H
#define UTILITY_H

#include <vector>
#include <string>
#include "../GenerateXML/KATSBonX.h"

namespace KATS
{
  namespace Utility
  {
    /// Convert string to uppercase
    ::std::string & upper (::std::string & input);
    
    /// Convert string to lowercase 
    ::std::string & lower (::std::string & input);

    /// Convert string to lowercase 
    ::std::string & dds_topicify (::std::string & input);

    /// Strip all whitespace
    ::std::string & strip_white_space (::std::string & input);

    /// Strip whitespace from front and end of string and also
    /// condense multiple whitespace into a single space
    ::std::string & strip_extra_white_space (
      ::std::string & input);

    /// Split a string into tokens 
    void tokenizer (const ::std::string & input, 
      const ::std::vector< ::std::string> & splitters,
      ::std::vector< ::std::string> & tokens,
      ::std::vector< ::std::string> & pivot_list);

    /// Return file contents as a string
    std::string file_to_string (const std::string & filename);

    /// Return the path of the provided file or directory
    std::string extract_path (const std::string & name);
    
    /// Return the file name of the provided file or directory
    std::string extract_filename (const std::string & name);

    /// Expand any environment variables in a string
    std::string expand_model_vars (KATS_BON::ProcessBase & current,
      const std::string & source);

    /// Expand any environment variables in a string
    std::string expand_model_vars (KATS_BON::GroupRef & current,
      const std::string & source);

    /// grab an environment variable value (@see expand_envs)
    std::string get_var (const std::string & source, 
      size_t cur, size_t & end);

    std::string clean_dir_name (const std::string & target);
  }
}
#endif
