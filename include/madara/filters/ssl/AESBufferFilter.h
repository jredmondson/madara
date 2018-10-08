

#ifndef _MADARA_FILTERS_SSL_AES_H_
#define _MADARA_FILTERS_SSL_AES_H_

/**
 * @file AESBufferFilter.h
 * @author James Edmondson <jedmondson@gmail.com>
 *
 * This file contains a filter functor for 256 bit AES encryption
 **/

#ifdef _USE_SSL_

#include <string>

#include "madara/MadaraExport.h"
#include "madara/utility/ScopedArray.h"
#include "../BufferFilter.h"

namespace madara
{
namespace filters
{
/**
 * @class AESBufferFilter
 * @brief Encrypts a buffer with 256 bit AES via OpenSSL
 */
class MADARA_EXPORT AESBufferFilter : public BufferFilter
{
public:
  /**
   * Constructor
   **/
  AESBufferFilter();

  /**
   * Copy constructor
   * @param  input   the buffer filter to copy
   **/
  AESBufferFilter(const AESBufferFilter& input);

  /**
   * 256 bit key constructor
   * @param  key         the key to use for encryption
   * @param  key_length  the length of the key
   **/
  AESBufferFilter(unsigned char* key, int key_length);

  /**
   * Destructor
   **/
  virtual ~AESBufferFilter();

  /**
   * Generates a 256 bit key from a password
   * @param  password   a password to seed the key with
   * @return  0 on success, -1 on error
   **/
  int generate_key(const std::string& password);

  /**
   * Encodes the buffer in place using AES encryption
   * @param   source           the source and destination buffer
   * @param   size             the amount of data in the buffer in bytes
   * @param   max_size         the amount of bytes the buffer can hold
   * @return  the new size after encoding
   **/
  virtual int encode(char* source, int size, int max_size) const;

  /**
   * Decodes the buffer in place using AES encryption
   * @param   source           the source and destination buffer
   * @param   size             the amount of data in the buffer in bytes
   * @param   max_size         the amount of bytes the buffer can hold
   * @return  the new size after decoding
   **/
  virtual int decode(char* source, int size, int max_size) const;

  /**
   * Gets the id of the filter. This is used in the serialization process
   * for transports and checkpoints to identify which filter is used.
   **/
  virtual std::string get_id(void);

  /**
   * Gets the version of the filter. @see madara::utility::get_uint_version
   * for one way to get this from a string version
   **/
  virtual uint32_t get_version(void);

private:
  /// the user's cypher key
  utility::ScopedArray<unsigned char> key_;

  /// initialization vector
  utility::ScopedArray<unsigned char> iv_;
};
}
}

#endif  // _USE_SSL_

#endif  // _MADARA_FILTERS_SSL_AES_H_
