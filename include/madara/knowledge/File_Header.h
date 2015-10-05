#ifndef  _FILE_HEADER_H_
#define  _FILE_HEADER_H_

/**
 * @file File_Header.h
 * @author James Edmondson <jedmondson@gmail.com>
 *
 * This file contains the filer header used by default in KaRL checkpointing
 **/

#include "madara/utility/stdint.h"
#include "madara/MADARA_export.h"

namespace madara
{
  namespace knowledge
  {
    /**
    * @class File_Header
    * @brief Defines a file header which is the default for
    *        KaRL checkpointing.
    *        
    *        Format:
    *
    *       [00] [64 bit file size] <br />
    *       [08] [64 bit states] <br />
    *       [16] [64 bit timestamp] <br />
    *       [24] [64 bit last_timestamp] <br />
    *       [32] [8 byte file_type = "KaRL"] <br />
    *       [40] [32 bit karl_version = version (0.255.255.255)] <br />
    *       [44] [64 byte id = originator] <br />
    *       [108] [Updates]
    */

    class MADARA_Export File_Header
    {
    public:
      
      /**
       * Constructor
       **/
      File_Header ();

      /**
       * Destructor
       **/
      virtual ~File_Header ();

      /**
       * Returns the size of the encoded File_Header class, which may be
       * different from sizeof (File_Header) because of compiler
       * optimizations for word boundaries
       **/
      static uint32_t encoded_size (void);

      /**
       * Reads a File_Header instance from a buffer and updates
       * the amount of buffer room remaining.
       * @param     buffer     the readable buffer where data is stored
       * @param     buffer_remaining  the count of bytes remaining in the
       *                              buffer to read
       * @return    current buffer position for next read
       **/
      virtual const char * read (const char * buffer,
        int64_t & buffer_remaining);
      
      /**
       * Writes a File_Header instance to a buffer and updates
       * the amount of buffer room remaining.
       * @param     buffer     the readable buffer where data is stored
       * @param     buffer_remaining  the count of bytes remaining in the
       *                              buffer to read
       * @return    current buffer position for next write
       **/
      virtual char * write (char * buffer, int64_t & buffer_remaining);
 
      /**
       * Compares the fields of this instance to another instance
       * @param     other      the other instance to compare against
       * @return    true if equal, false otherwise
       **/
      virtual bool equals (const File_Header & other);
      
      /**
       * Tests the buffer for a normal message identifier
       * @return   true if identifier indicates reduced message header
       **/
      static inline bool file_header_test (const char * buffer)
      {
        return strncmp (&(buffer[32]), "KaRL", 4) == 0; 
      }

      /**
       * the size of this header plus the updates
       **/
      uint64_t          size;
      
      /**
       * the number of states checkpointed in the file stream
       **/
      uint64_t          states;
      
      /**
       * the timestamp for the initial checkpointing
       **/
      uint64_t          initial_timestamp;
      
      /**
       * the timestamp for the initial checkpointing
       **/
      uint64_t          last_timestamp;
      
      /**
       * file type identifier ("KaRL")
       **/
      char              file_type[8];

      /**
       * Version of KaRL installed when file was created
       **/
      uint32_t          karl_version;

      /**
       * the originator of the message (host:port)
       **/
      char              originator[64];
    };
  }
}


#endif   // _FILE_HEADER_H_