
#ifdef  _USE_LZ4_

#include <string.h>
#include "LZ4BufferFilter.h"
#include "lz4.h"
#include "madara/utility/Utility.h"
#include "madara/logger/GlobalLogger.h"

int
madara::filters::LZ4BufferFilter::encode (
  unsigned char * source, int size, int max_size) const
{
  char * source_copy = new char [buffer_size];
  memcpy (source_copy, source, size);

  madara_logger_ptr_log (logger::global_logger.get_ptr (), logger::LOG_MINOR,
    "LZ4BufferFilter::encode: calling LZ4_compress_default(%d,%d).\n",
    size, max_size);

  int new_size = LZ4_compress_default (source_copy, (char *)source, size, max_size);
  delete [] source_copy;

  madara_logger_ptr_log (logger::global_logger.get_ptr (), logger::LOG_MINOR,
    "LZ4BufferFilter::encode: new_size=%d.\n",
    new_size);

  // error codes are negative and appear to point to where in the buffer
  // bad things happened
  return new_size > 0 ? new_size : 0;
}

int
madara::filters::LZ4BufferFilter::decode (
  unsigned char * source, int size, int max_size) const
{
  char * source_copy = new char [buffer_size];
  memcpy (source_copy, source, size);

  madara_logger_ptr_log (logger::global_logger.get_ptr (), logger::LOG_MINOR,
    "LZ4BufferFilter::decode: calling LZ4_decompress_safe(%d,%d).\n",
    size, max_size);

  int new_size = LZ4_decompress_safe (source_copy, (char *)source, size, max_size);
  delete [] source_copy;

  madara_logger_ptr_log (logger::global_logger.get_ptr (), logger::LOG_MINOR,
    "LZ4BufferFilter::decode: new_size=%d.\n",
    new_size);

  // error codes are negative and appear to point to where in the buffer
  // bad things happened
  return new_size > 0 ? new_size : 0;
}

std::string
madara::filters::LZ4BufferFilter::get_id (void)
{
  return "lz4";
}

uint32_t
madara::filters::LZ4BufferFilter::get_version (void)
{
  return madara::utility::get_uint_version ("1.0.0");
}

#endif // _USE_SSL_
