
#include <iostream>
#include <fstream>

#include "Madara_Common.h"
#include "Agent_Context.h"

Madara::Agent_Context::Agent_Context()
: num_latencies_ (5)
{
}

Madara::Agent_Context::Agent_Context(const std::string& host, const std::string& port)
: host_ (host), port_ (port), num_latencies_ (5)
{
}

void
Madara::Agent_Context::read (const std::string& filename)
{
  Context_Guard guard (mutex_);
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
Madara::Agent_Context::write (const std::string& filename)
{
  Context_Guard guard (mutex_);
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
Madara::Agent_Context::write (std::ostream& output)
{
    for (LatencyMap::iterator i = map_.begin (); i != map_.end (); ++i)
      {
        for (size_t j = 0; j < i->second.latencies.size (); ++j)
          output << i->first << " " << i->second.latencies[j] << "\n";
      }
}

void 
Madara::Agent_Context::addLatency (const std::string& key, int latency)
{
  Context_Guard guard (mutex_);
  createEntry (key);
  LatencyTable& table = map_[key];

  table.latencies[table.cur] = latency;
  table.cur = table.cur == num_latencies_-1 ? 0 : table.cur+1;
}


void 
Madara::Agent_Context::createEntry (const std::string& key)
{
  Context_Guard guard (mutex_);
  if (map_.find (key) == map_.end ())
    {
      LatencyTable table;
      table.latencies.resize (num_latencies_);

      for (int i = 0; i < num_latencies_; ++i)
        table.latencies[i] = 0 - 1;

      table.cur = 0;
      map_[key] = table;
    }
}


unsigned int  
Madara::Agent_Context::getNumKeys ()
{
  return map_.size ();
}

std::string  
Madara::Agent_Context::getKey (unsigned int pos)
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
Madara::Agent_Context::getLatencyTable (unsigned int pos)
{
  return map_[getKey (pos)];
}

Madara::LatencyTable
Madara::Agent_Context::getLatencyTable (const std::string & key)
{
  Context_Guard guard (mutex_);
  return map_[key];
}

const std::string&  
Madara::Agent_Context::getHost ()
{
  Context_Guard guard (mutex_);
  return host_;
}

void 
Madara::Agent_Context::setHost (const std::string& host)
{
  Context_Guard guard (mutex_);
  host_ = host;
}

const std::string&  
Madara::Agent_Context::getPort ()
{
  Context_Guard guard (mutex_);
  return port_;
}

void 
Madara::Agent_Context::setPort (const std::string& port)
{
  Context_Guard guard (mutex_);
  port_ = port;
}


bool
Madara::Agent_Context::peerExists (const std::string& key)
{
  return map_.find (key) != map_.end ();
}

bool 
Madara::Agent_Context::peerExists (const std::string& host, u_short u_port)
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
Madara::Agent_Context::peerExists (const std::string& host, const std::string& port)
{
  std::string key;
  Madara::merge_key (key, host, port);

  return peerExists (key);
}


bool 
Madara::Agent_Context::selfCheck (const std::string& host, const std::string& port)
{
  return host == host_ && port == port_;
}


bool 
Madara::Agent_Context::selfCheck (const std::string& host, u_short u_port)
{
  char port_buf[16];
  std::string port;

  // convert the server_port into string form for our context
  itoa (u_port, port_buf, 10);
  port = port_buf;

  return selfCheck (host, port);
}