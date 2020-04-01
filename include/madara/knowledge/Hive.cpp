/**
 * Copyright(c) 2020 Galois. All Rights Reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following acknowledgments and disclaimers.
 * 
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 
 * 3. The names "Galois," "Carnegie Mellon University," "SEI" and/or "Software
 *    Engineering Institute" shall not be used to endorse or promote products
 *    derived from this software without prior written permission. For written
 *    permission, please contact permission@sei.cmu.edu.
 * 
 * 4. Products derived from this software may not be called "SEI" nor may "SEI"
 *    appear in their names without prior written permission of
 *    permission@sei.cmu.edu.
 * 
 * 5. Redistributions of any form whatsoever must retain the following
 *    acknowledgment:
 * 
 *      This material is based upon work funded and supported by the Department
 *      of Defense under Contract No. FA8721-05-C-0003 with Carnegie Mellon
 *      University for the operation of the Software Engineering Institute, a
 *      federally funded research and development center. Any opinions,
 *      findings and conclusions or recommendations expressed in this material
 *      are those of the author(s) and do not necessarily reflect the views of
 *      the United States Department of Defense.
 * 
 *      NO WARRANTY. THIS CARNEGIE MELLON UNIVERSITY AND SOFTWARE ENGINEERING
 *      INSTITUTE MATERIAL IS FURNISHED ON AN "AS-IS" BASIS. CARNEGIE MELLON
 *      UNIVERSITY MAKES NO WARRANTIES OF ANY KIND, EITHER EXPRESSED OR
 *      IMPLIED, AS TO ANY MATTER INCLUDING, BUT NOT LIMITED TO, WARRANTY OF
 *      FITNESS FOR PURPOSE OR MERCHANTABILITY, EXCLUSIVITY, OR RESULTS
 *      OBTAINED FROM USE OF THE MATERIAL. CARNEGIE MELLON UNIVERSITY DOES
 *      NOT MAKE ANY WARRANTY OF ANY KIND WITH RESPECT TO FREEDOM FROM PATENT,
 *      TRADEMARK, OR COPYRIGHT INFRINGEMENT.
 * 
 *      This material has been approved for public release and unlimited
 *      distribution.
 **/


#include "Hive.h"
#include "madara/transport/QoSTransportSettings.h"
#include "madara/transport/SharedMemoryPush.h"


madara::knowledge::Hive::Hive()
{
}

madara::knowledge::Hive::~Hive()
{
  for (size_t i = 0; i < transports_.size(); ++i)
  {
    delete transports_[i];
  }
}

void
madara::knowledge::Hive::resize(size_t new_size)
{
  size_t old_size = kbs_.size();

  if (new_size != old_size)
  {
    kbs_.resize(new_size);

    for (auto transport : transports_)
    {
      if (transport != 0)
      {
        transport->set(kbs_);
      }
    }

    transports_.resize(new_size);

    if (old_size < new_size)
    {
      transport::QoSTransportSettings settings;

      for (size_t i = old_size; i < new_size; ++i)
      {
        transports_[i] = new madara::transport::SharedMemoryPush(
          kbs_[i].get_id(), settings, kbs_[i]);

        transports_[i]->set(kbs_);

        kbs_[i].attach_transport(transports_[i]);
      } // for old -> new
    } // if old < new
  } // if new != old
}
