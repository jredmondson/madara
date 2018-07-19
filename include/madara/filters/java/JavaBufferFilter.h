

#ifndef _MADARA_FILTERS_JAVA_BUFFER_H_
#define _MADARA_FILTERS_JAVA_BUFFER_H_

/**
 * @file JavaBufferFilter.h
 * @author James Edmondson <jedmondson@gmail.com>
 *
 * This file contains a filter functor for 256 bit AES encryption
 **/

#ifdef _MADARA_JAVA_
#include <jni.h>
#include "madara_jni.h"

#include <string>

#include "madara/logger/Logger.h"
#include "madara/MadaraExport.h"
#include "madara/utility/ScopedArray.h"
#include "../BufferFilter.h"

namespace madara
{
  namespace filters
  {
    /**
     * @class JavaBufferFilter
     * @brief Filters a buffer in Java
     */
    class MADARA_EXPORT JavaBufferFilter : public BufferFilter
    {
    public:
      /**
      * Constructor
      **/
      JavaBufferFilter (logger::Logger & logger, jobject obj);

      /**
       * Destructor
       **/
      virtual ~JavaBufferFilter ();

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

      /**
       * Gets the id of the filter. This is used in the serialization process
       * for transports and checkpoints to identify which filter is used.
       **/
      virtual std::string get_id (void);

      /**
       * Gets the version of the filter. @see madara::utility::get_uint_version
       * for one way to get this from a string version
       **/
      virtual uint32_t get_version (void);

    private:

      // holds a reference to the Java object
      jobject obj_;

      /// the class of the Java object obj_
      jclass class_;

      /// a logger
      logger::Logger * logger_;
    };
  }
}

#endif // _MADARA_JAVA_

#endif  // _MADARA_FILTERS_JAVA_BUFFER_H_
