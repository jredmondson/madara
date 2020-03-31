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
