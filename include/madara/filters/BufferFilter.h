

#ifndef _MADARA_FILTERS_BUFFER_FILTER_H_
#define _MADARA_FILTERS_BUFFER_FILTER_H_

/**
 * @file BufferFilter.h
 * @author James Edmondson <jedmondson@gmail.com>
 *
 * This file contains a filter functor for buffers
 **/

#include <list>

namespace madara
{
  namespace filters
  {
    /**
     * Abstract base class for implementing buffer filters via
     * a functor interface. When subclassing this class, create a new
     * instance with the new operator, and the pointer will be managed
     * by the underlying MADARA infrastructure.
     **/
    class BufferFilter
    {
    public:
      /**
       * Destructor
       **/
      virtual ~BufferFilter ()
      {
      }

      /**
      * Encodes the buffer in place
      * @param   source           the source and destination buffer
      * @param   size             the amount of data in the buffer in bytes
      * @param   max_size         the amount of bytes the buffer can hold
      * @return  the new size after encoding
      **/
      virtual int encode (unsigned char * source, int size, int max_size) const = 0;

      /**
      * Decodes the buffer in place
      * @param   source           the source and destination buffer
      * @param   size             the amount of data in the buffer in bytes
      * @param   max_size         the amount of bytes the buffer can hold
      * @return  the new size after decoding
      **/
      virtual int decode (unsigned char * source, int size, int max_size) const = 0;

    };

    typedef  std::list <BufferFilter *>   BufferFilters;
  }
}

#endif  // _MADARA_FILTERS_AGGREGATE_FILTER_H_
