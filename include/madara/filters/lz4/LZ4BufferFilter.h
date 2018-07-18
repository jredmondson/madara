
#ifndef _MADARA_FILTERS_LZ4_BUFFER_FILTER_H_
#define _MADARA_FILTERS_LZ4_BUFFER_FILTER_H_

/**
 * @file LZ4BufferFilter.h
 * @author James Edmondson <jedmondson@gmail.com>
 *
 * This file contains a filter functor for 256 bit AES encryption
 **/

#ifdef _USE_LZ4_

#include <string>

#include "madara/MadaraExport.h"
#include "../BufferFilter.h"

namespace madara
{
  namespace filters
  {
    /**
     * @class LZ4BufferFilter
     * @brief Compresses with LZ4
     */
    class MADARA_EXPORT LZ4BufferFilter : public BufferFilter
    {
    public:
      /**
       * Destructor
       **/
      virtual ~LZ4BufferFilter () = default;

      /**
       * Encodes the buffer in place using LZ4 compression
       * @param   source           the source and destination buffer
       * @param   size             the amount of data in the buffer in bytes
       * @param   max_size         the amount of bytes the buffer can hold
       * @return  the new size after encoding
       **/
      virtual int encode (unsigned char * source, int size, int max_size) const;

      /**
       * Decodes the buffer in place using LZ4 decompression
       * @param   source           the source and destination buffer
       * @param   size             the amount of data in the buffer in bytes
       * @param   max_size         the amount of bytes the buffer can hold
       * @return  the new size after decoding
       **/
      virtual int decode (unsigned char * source, int size, int max_size) const;

      /// the size of the decompress buffer (max size)
      int buffer_size = 10000000;
    };
  }
}

#endif // _USE_LZ4_

#endif  // _MADARA_FILTERS_LZ4_BUFFER_FILTER_H_
