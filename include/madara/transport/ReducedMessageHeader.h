#ifndef  _MADARA_REDUCED_MESSAGE_HEADER_H_
#define  _MADARA_REDUCED_MESSAGE_HEADER_H_

/**
* @file MessageHeader.h
* @author James Edmondson <jedmondson@gmail.com>
*
* This file contains the message header used by default in some KaRL
* network transports (e.g. IP multicast and broadcast)
**/

#include <string.h>
#include "madara/utility/StdInt.h"
#include "madara/MadaraExport.h"
#include "madara/transport/MessageHeader.h"

namespace madara
{
  namespace transport
  {

    #define REDUCED_MADARA_ID    "karl1.3"

    /**
    * @class ReducedMessageHeader
    * @brief Defines a simple, smaller message header of 29 bytes that
    *        supports less QoS.
    *        
    *        Format:
    *
    *        [0] [64 bit unsigned size]<br />
    *        [8] [8 byte string transport id]<br />
    *        [16] [4 byte unsigned num updates]<br />
    *        [20] [64 bit unsigned clock]<br />
    *        [28] [8 bit unsigned ttl--for rebroadcasts]<br />
    *        [29] [knowledge updates start here]
    */

    class MADARA_EXPORT ReducedMessageHeader : public MessageHeader
    {
    public:

#define REDUCED_ORIGINATOR_SIZE    24

      /**
       * Constructor
       **/
      ReducedMessageHeader ();

      /**
       * Destructor
       **/
      virtual ~ReducedMessageHeader ();

      /**
      * Returns the size of the encoded MessageHeader class, which may be
      * different from sizeof (MessageHeader) because of compiler
      * optimizations for word boundaries
      **/
      virtual uint32_t encoded_size (void) const;

      /**
      * Reads a MessageHeader instance from a buffer and updates
      * the amount of buffer room remaining.
      * @param     buffer     the readable buffer where data is stored
      * @param     buffer_remaining  the count of bytes remaining in the
      *                              buffer to read
      * @return    current buffer position for next read
      **/
      virtual const char * read (const char * buffer,
        int64_t & buffer_remaining);

      /**
      * Writes a MessageHeader instance to a buffer and updates
      * the amount of buffer room remaining.
      * @param     buffer     the readable buffer where data is stored
      * @param     buffer_remaining  the count of bytes remaining in the
      *                              buffer to read
      * @return    current buffer position for next write
      **/
      virtual char * write (char * buffer, int64_t & buffer_remaining);

      /**
      * Converts the relevant fields to a printable string
      * @return  the printable string of fields in the header
      **/
      virtual std::string to_string (void);

      /**
      * Compares the fields of this instance to another instance
      * @param     other      the other instance to compare against
      * @return    true if equal, false otherwise
      **/
      virtual bool equals (const MessageHeader & other);

      /**
       * Tests the buffer for a reduced message identifier
       * @return   true if identifier indicates reduced message header
       **/
      static inline bool reduced_message_header_test (const char * buffer)
      {
        return strncmp (&(buffer[8]), REDUCED_MADARA_ID, 7) == 0; 
      }
    };
  }
}

#endif   // _MADARA_REDUCED_MESSAGE_HEADER_H_
