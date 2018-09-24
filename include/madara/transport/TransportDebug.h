#ifndef _MADARA_TRANSPORT_TRANSPORT_DEBUG_H_
#define _MADARA_TRANSPORT_TRANSPORT_DEBUG_H_

#include "madara/knowledge/VariableReference.h"

// begin madara and transport namespaces
namespace madara { namespace transport {

/**
 * Encapsulates Transport Debug Information
 **/
struct TransportDebug
{
  /// sent packets
  knowledge::VariableReference sent_packets;

  /// sent data
  knowledge::VariableReference sent_data;

  /// max data sent
  knowledge::VariableReference sent_data_max;

  /// min data sent
  knowledge::VariableReference sent_data_min;

  /// sent packets
  knowledge::VariableReference received_packets;

  /// sent data
  knowledge::VariableReference received_data;

  /// max data sent
  knowledge::VariableReference received_data_max;

  /// min data sent
  knowledge::VariableReference received_data_min;
};

}} // end madara and transport namespaces


#endif
