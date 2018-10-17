/*********************************************************************
 * Copyright (c) 2013-2015 Carnegie Mellon University. All Rights Reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following acknowledgments and disclaimers.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * 3. The names "Carnegie Mellon University," "SEI" and/or
 * "Software Engineering Institute" shall not be used to endorse or promote
 * products derived from this software without prior written permission. For
 * written permission, please contact permission@sei.cmu.edu.
 *
 * 4. Products derived from this software may not be called "SEI" nor may "SEI"
 * appear in their names without prior written permission of
 * permission@sei.cmu.edu.
 *
 * 5. Redistributions of any form whatsoever must retain the following
 * acknowledgment:
 *
 * This material is based upon work funded and supported by the Department of
 * Defense under Contract No. FA8721-05-C-0003 with Carnegie Mellon University
 * for the operation of the Software Engineering Institute, a federally funded
 * research and development center. Any opinions, findings and conclusions or
 * recommendations expressed in this material are those of the author(s) and
 * do not necessarily reflect the views of the United States Department of
 * Defense.
 *
 * NO WARRANTY. THIS CARNEGIE MELLON UNIVERSITY AND SOFTWARE ENGINEERING
 * INSTITUTE MATERIAL IS FURNISHED ON AN "AS-IS" BASIS. CARNEGIE MELLON
 * UNIVERSITY MAKES NO WARRANTIES OF ANY KIND, EITHER EXPRESSED OR IMPLIED,
 * AS TO ANY MATTER INCLUDING, BUT NOT LIMITED TO, WARRANTY OF FITNESS FOR
 * PURPOSE OR MERCHANTABILITY, EXCLUSIVITY, OR RESULTS OBTAINED FROM USE OF THE
 * MATERIAL. CARNEGIE MELLON UNIVERSITY DOES NOT MAKE ANY WARRANTY OF ANY KIND
 * WITH RESPECT TO FREEDOM FROM PATENT, TRADEMARK, OR COPYRIGHT INFRINGEMENT.
 *
 * This material has been approved for public release and unlimited
 * distribution.
 *
 * @author James Edmondson <jedmondson@gmail.com>
 *********************************************************************/
package ai.madara.transport.filters;

import ai.madara.exceptions.MadaraDeadObjectException;
import ai.madara.knowledge.Variables;
import ai.madara.transport.TransportContext;

public class LogAggregate implements AggregateFilter
{
  /**
   * Types of operations
   **/
  public String[] opTypes = {
    "IDLE_OPERATION",
    "SEND_OPERATION",
    "RECEIVE_OPERATION",
    "REBROADCAST_OPERATION"
  };

  /**
   * Logger of all information
   * @param packet  a collection of variable keys and values in the packet
   * @param context  information about current state of transport layer
   * @param variables facade for current knowledge base
 *  @throws MadaraDeadObjectException throws exception if object is already released
   **/
  public void filter(Packet packet, TransportContext context, Variables variables) throws MadaraDeadObjectException
  {
    StringBuffer buffer = new StringBuffer();
    buffer.append("Aggregate Filter args:\n");

    buffer.append("  Operation Type:");

    int index = (int) context.getOperation();
    if(index < opTypes.length)
    {
      buffer.append(opTypes[index]);
    }
    buffer.append("\n");

    buffer.append("  Send bandwidth:");
    buffer.append(context.getSendBandwidth());
    buffer.append(" B/s\n");

    buffer.append("  Recv bandwidth:");
    buffer.append(context.getReceiveBandwidth());
    buffer.append(" B/s\n");

    buffer.append("  Update Time:");
    buffer.append(context.getMessageTime());
    buffer.append("\n");

    buffer.append("  Current Time:");
    buffer.append(context.getCurrentTime());
    buffer.append("\n");

    buffer.append("  Domain:");
    buffer.append(context.getDomain());
    buffer.append("\n");

    buffer.append("  Updates:\n");

    String [] keys = packet.getKeys();

    for(int i = 0; i < keys.length; ++i)
    {
      buffer.append("    ");
      buffer.append(keys[i]);
      buffer.append("=");
      buffer.append(packet.get(keys[i]));
      buffer.append("\n");
    }

    buffer.append("\n");

    System.out.println(buffer.toString());
  }
}

