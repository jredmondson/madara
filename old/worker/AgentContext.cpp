
#include <iostream>
#include <fstream>

#include "MadaraCommon.h"
#include "AgentContext.h"

const int Madara::NUM_CLIENT_LATENCIES = 5;

Madara::AgentContext::AgentContext()
: num_latencies_ (Madara::NUM_CLIENT_LATENCIES)
{
}

Madara::AgentContext::AgentContext(const std::string& host, 
                                     const std::string& port)
: host_ (host), port_ (port), num_latencies_ (Madara::NUM_CLIENT_LATENCIES)
{
}

void
Madara::AgentContext::read (const std::string& filename)
{
  ContextGuard guard (mutex_);
  std::ifstream input (filename.c_str ());

  if (input.is_open ())
    {
      while (input)
        {
          std::string key;
          int value;

          input >> key;
          input >> value;

          if (input)
            addLatency (key, value);
        }
      input.close ();
    }
}

void
Madara::AgentContext::write (const std::string& filename)
{
  ContextGuard guard (mutex_);
  std::ofstream output (filename.c_str ());

  if (output.is_open ())
    {
      for (LatencyMap::iterator i = map_.begin (); i != map_.end (); ++i)
        {
          for (size_t j = 0; j < i->second.latencies.size (); ++j)
            output << i->first << " " << i->second.latencies[j] << "\n";
        }
      output.close ();
    }
}

void
Madara::AgentContext::write (std::ostream& output)
{
    for (LatencyMap::iterator i = map_.begin (); i != map_.end (); ++i)
      {
        for (size_t j = 0; j < i->second.latencies.size (); ++j)
          output << i->first << " " << i->second.latencies[j] << "\n";
      }
}

void 
Madara::AgentContext::addLatency (const std::string& key, int latency)
{
  ContextGuard guard (mutex_);
  createEntry (key);
  LatencyTable& table = map_[key];

  // add the latency and update cur
  table.latencies[table.cur] = latency;
  table.cur = table.cur == num_latencies_-1 ? 0 : table.cur+1;

  // update the total recorded
  ++table.total_recorded;
}


int 
Madara::AgentContext::getAverageLatency (const std::string& key)
{
  ContextGuard guard (mutex_);
  createEntry (key);

  // grab the latency table
  Madara::LatencyTable& table = map_[key];

  // calculate the sum
  int sum = 0;
  int num_latencies = min(Madara::NUM_CLIENT_LATENCIES, table.total_recorded);

  for (::size_t i = 0; i < num_latencies; ++i)
    {
      if (table.latencies[i] != -1)
        sum += table.latencies[i];
    }

  // return the avg
  return sum / num_latencies;
}

void 
Madara::AgentContext::createEntry (const std::string& key)
{
  ContextGuard guard (mutex_);
  if (map_.find (key) == map_.end ())
    {
      LatencyTable table;
      table.latencies.resize (num_latencies_);

      for (int i = 0; i < num_latencies_; ++i)
        table.latencies[i] = 0 - 1;

      table.cur = 0;
      table.total_recorded = 0;
      map_[key] = table;
    }
}


unsigned int  
Madara::AgentContext::getNumKeys ()
{
  return map_.size ();
}

std::string  
Madara::AgentContext::getKey (unsigned int pos)
{
  std::string unfound = "";

  if (pos < map_.size ())
    {
      LatencyMap::iterator i = map_.begin ();
      for(unsigned int cur = 0; cur < pos; ++cur, ++i);

      return i->first;
    }
  return unfound;
}

Madara::LatencyTable
Madara::AgentContext::getLatencyTable (unsigned int pos)
{
  return map_[getKey (pos)];
}

Madara::LatencyTable
Madara::AgentContext::getLatencyTable (const std::string & key)
{
  ContextGuard guard (mutex_);
  return map_[key];
}

const std::string&  
Madara::AgentContext::getHost ()
{
  ContextGuard guard (mutex_);
  return host_;
}

void 
Madara::AgentContext::setHost (const std::string& host)
{
  ContextGuard guard (mutex_);
  host_ = host;
}

const std::string&  
Madara::AgentContext::getPort ()
{
  ContextGuard guard (mutex_);
  return port_;
}

void 
Madara::AgentContext::setPort (const std::string& port)
{
  ContextGuard guard (mutex_);
  port_ = port;
}


bool
Madara::AgentContext::peerExists (const std::string& key)
{
  return map_.find (key) != map_.end ();
}

bool 
Madara::AgentContext::peerExists (const std::string& host, u_short u_port)
{
  char port_buf[16];
  std::string key;
  std::string port;

  // convert the server_port into string form for our context
  itoa (u_port, port_buf, 10);

  port = port_buf;

  Madara::merge_key (key, host, port);

  return peerExists (key);
}

bool 
Madara::AgentContext::peerExists (const std::string& host, const std::string& port)
{
  std::string key;
  Madara::merge_key (key, host, port);

  return peerExists (key);
}


bool 
Madara::AgentContext::selfCheck (const std::string& host, const std::string& port)
{
  return host == host_ && port == port_;
}


bool 
Madara::AgentContext::selfCheck (const std::string& host, u_short u_port)
{
  char port_buf[16];
  std::string port;

  // convert the server_port into string form for our context
  itoa (u_port, port_buf, 10);
  port = port_buf;

  return selfCheck (host, port);
}