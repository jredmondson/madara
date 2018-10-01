#ifndef _MADARA_TRANSPORT_SETTINGS_INL_
#define _MADARA_TRANSPORT_SETTINGS_INL_

/**
 * @file TransportSettings.inl
 * @author James Edmondson <jedmondson@gmail.com>
 *
 * This file contains inline functions from the TransportSettings class
 **/

#include "TransportSettings.h"

inline void madara::transport::TransportSettings::add_read_domain(
    const std::string domain)
{
  read_domains_[domain] = 1;
}

inline void madara::transport::TransportSettings::clear_read_domains(void)
{
  read_domains_.clear();
}

inline void madara::transport::TransportSettings::debug_to_kb(
    const std::string& prefix)
{
  debug_to_kb_prefix = prefix;
}

inline void madara::transport::TransportSettings::get_read_domains(
    std::vector<std::string>& domains) const
{
  domains.clear();
  for (std::map<std::string, int>::const_iterator i = read_domains_.begin();
       i != read_domains_.end(); ++i) {
    domains.push_back(i->first);
  }
}

inline bool madara::transport::TransportSettings::is_reading_domain(
    const std::string domain) const
{
  return read_domains_.find(domain) != read_domains_.end();
}

inline size_t madara::transport::TransportSettings::num_read_domains(void) const
{
  return read_domains_.size();
}

#endif  // _MADARA_TRANSPORT_SETTINGS_INL_
