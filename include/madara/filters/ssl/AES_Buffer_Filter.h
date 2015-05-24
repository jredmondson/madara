

#ifndef _MADARA_FILTERS_SSL_AES_H_
#define _MADARA_FILTERS_SSL_AES_H_

/**
 * @file AES_Buffer_Filter.h
 * @author James Edmondson <jedmondson@gmail.com>
 *
 * This file contains a filter functor for aggregate records
 **/

#include <string>

#include "madara/MADARA_Export.h"
#include "madara/utility/Scoped_Array.h"
#include "../Buffer_Filter.h"

namespace Madara
{
  namespace Filters
  {
    /**
     * Filter for encrypting with the AES 256 bit cypher
     **/
    class MADARA_Export AES_Buffer_Filter : public Buffer_Filter
    {
    public:
      /**
      * Constructor
      **/
      AES_Buffer_Filter ();

      /**
      * 256 bit key constructor
      * @param  key         the key to use for encryption
      * @param  key_length  the length of the key
      **/
      AES_Buffer_Filter (unsigned char * key, int key_length);

      /**
       * Destructor
       **/
      virtual ~AES_Buffer_Filter ();

      /**
      * Generates a 256 bit key from a password
      * @param  password   a password to seed the key with
      * @return  0 on success, -1 on error
      **/
      int generate_key (const std::string & password);

      /**
       * Encodes the buffer in place using AES encryption
       * @param   source           the source and destination buffer
       * @param   size             the amount of data in the buffer in bytes
       * @param   max_size         the amount of bytes the buffer can hold
       * @return  the new size after encoding
       **/
      virtual int encode (unsigned char * source, int size, int max_size) const;

      /**
       * Decodes the buffer in place using AES encryption
       * @param   source           the source and destination buffer
       * @param   size             the amount of data in the buffer in bytes
       * @param   max_size         the amount of bytes the buffer can hold
       * @return  the new size after decoding
       **/
      virtual int decode (unsigned char * source, int size, int max_size) const;

    private:

      /// the user's cypher key
      Utility::Scoped_Array <unsigned char> key_;

      /// initialization vector
      Utility::Scoped_Array <unsigned char> iv_;
    };
  }
}

#endif  // _MADARA_FILTERS_SSL_AES_H_
