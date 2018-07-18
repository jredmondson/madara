
#ifdef  _USE_LZ4_

#include <string.h>
#include "LZ4BufferFilter.h"
#include "lz4.h"

int
madara::filters::LZ4BufferFilter::encode (
  unsigned char * source, int size, int max_size) const
{
  char * source_copy = new char [buffer_size];
  memcpy (source_copy, source, size);

  int new_size = LZ4_compress_default (source_copy, (char *)source, size, max_size);
  delete [] source_copy;

  return new_size;
}

int
madara::filters::LZ4BufferFilter::decode (
  unsigned char * source, int size, int max_size) const
{
  char * source_copy = new char [buffer_size];
  memcpy (source_copy, source, size);

  int new_size = LZ4_decompress_safe (source_copy, (char *)source, size, max_size);
  delete [] source_copy;

  return new_size;
}

#endif // _USE_SSL_
