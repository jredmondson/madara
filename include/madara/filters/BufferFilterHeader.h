#ifndef  _MADARA_FILTERS_BUFFERFILTERHEADER_H_
#define  _MADARA_FILTERS_BUFFERFILTERHEADER_H_

/**
 * @file BufferFilterHeader.h
 * @author James Edmondson <jedmondson@gmail.com>
 *
 * This file contains the buffer filter header used in
 * checkpointing and messaging
 **/

#include "madara/utility/StdInt.h"
#include "madara/MadaraExport.h"
#include "BufferFilter.h"
#include <string.h>

namespace madara
{
  namespace filters
  {
    /**
    * @class BufferFilterHeader
    * @brief Defines a buffer filter header
    *        
    *        Format:
    *
    *       [00] [8 byte unsigned size] <br />
    *       [08] [8 byte id string] <br />
    *       [16] [4 byte version] <br />
    */

    class MADARA_EXPORT BufferFilterHeader
    {
    public:
      
      /**
       * Constructor
       **/
      BufferFilterHeader ()
      : size (20), id (""), version (0) 
      {
      }

      /**
       * Returns the size of the encoded BufferFilterHeader class
       **/
      static uint64_t encoded_size (void)
      {
        return 20;
      }

      /**
       * Checks compatability between the header and the filter
       * @param filter  the filter to check
       * @return  true if the filter matches the header
       **/
      bool check_filter (filters::BufferFilter * filter);

      /**
       * Reads relevant fields from a filter
       * @param filter  the filter to refer to
       **/
      void read (filters::BufferFilter * filter);

      /**
       * Reads a BufferFiltefrHeader instance from a buffer and updates
       * the amount of buffer room remaining.
       * @param     buffer     the readable buffer where data is stored
       * @param     buffer_remaining  the count of bytes remaining in the
       *                              buffer to read
       * @return    current buffer position for next read
       * @throw exceptions::MemoryException  not enough buffer to encode
       **/
      const char * read (const char * buffer,
        int64_t & buffer_remaining);
      
      /**
       * Writes a BufferFilterHeader instance to a buffer and updates
       * the amount of buffer room remaining.
       * @param     buffer     the readable buffer where data is stored
       * @param     buffer_remaining  the count of bytes remaining in the
       *                              buffer to read
       * @return    current buffer position for next write
       * @throw exceptions::MemoryException  not enough buffer to encode
       **/
      char * write (char * buffer, int64_t & buffer_remaining);
 
      /**
       * the size of this header plus the updates
       **/
      uint64_t          size;

      
      /**
       * filter id
       **/
      char              id[8];

      /**
       * filter version
       **/
      uint32_t          version;
    };
  }
}

#include "BufferFilterHeader.inl"

#endif   // _MADARA_FILTERS_BUFFERFILTERHEADER_H_
