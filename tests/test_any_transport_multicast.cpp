#define BOOST_PP_VARIADICS 1

#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <assert.h>

#include "madara/knowledge/KnowledgeBase.h"
#include "madara/filters/GenericFilters.h"

#include "madara/knowledge/Any.h"
#include "madara/knowledge/KnowledgeRecord.h"
#include "madara/utility/SupportTest.h"
#include "madara/knowledge/CapnAdapt.h"

#include "capnfiles/Geo.capnp.h"


using namespace madara;
using namespace knowledge;

struct Point
  {
    double x = 0, y = 0, z = 0;

    Point() = default;
    Point(double x, double y, double z = 0) : x(x), y(y), z(z) {}

    double &getX() { return x; }
    double &getY() { return y; }
    double &getZ() { return z; }
  };

MADARA_CAPN_MEMBERS(Point, geo_capn::Point,
      (x, X, [](Point &p) -> double & { return p.x; })
      (y, Y, &Point::getY)
      (z, Z)
    )

std::string host ("");
const std::string default_multicast ("239.255.0.1:4150");
madara::transport::QoSTransportSettings settings;

void handle_arguments (int argc, char ** argv)
{
  for (int i = 1; i < argc; ++i)
  {
    std::string arg1 (argv[i]);

    if (arg1 == "-m" || arg1 == "--multicast")
    {
      if (i + 1 < argc)
        settings.hosts[0] = argv[i + 1];

      ++i;
    }
    else if (arg1 == "-o" || arg1 == "--host")
    {
      if (i + 1 < argc)
        host = argv[i + 1];

      ++i;
    }
    else if (arg1 == "-d" || arg1 == "--domain")
    {
      if (i + 1 < argc)
        settings.write_domain = argv[i + 1];

      ++i;
    }
    else if (arg1 == "-i" || arg1 == "--id")
    {
      if (i + 1 < argc)
      {
        std::stringstream buffer (argv[i + 1]);
        buffer >> settings.id;
      }

      ++i;
    }
    else if (arg1 == "-f" || arg1 == "--logfile")
    {
      if (i + 1 < argc)
      {
        madara::logger::global_logger->add_file (argv[i + 1]);
      }

      ++i;
    }
    else if (arg1 == "-l" || arg1 == "--level")
    {
      if (i + 1 < argc)
      {
        std::stringstream buffer (argv[i + 1]);
        int level;
        buffer >> level;
        madara::logger::global_logger->set_level (level);
      }

      ++i;
    }
    else if (arg1 == "-p" || arg1 == "--drop-rate")
    {
      if (i + 1 < argc)
      {
        double drop_rate;
        std::stringstream buffer (argv[i + 1]);
        buffer >> drop_rate;
        
        settings.update_drop_rate (drop_rate,
          madara::transport::PACKET_DROP_DETERMINISTIC);
      }

      ++i;
    }
    else if (arg1 == "-r" || arg1 == "--reduced")
    {
      settings.send_reduced_message_header = true;
    }
    else
    {
      madara_logger_ptr_log (logger::global_logger.get(), logger::LOG_ALWAYS, 
"\nProgram summary for %s:\n\n" \
"  Test sending capnp serialized data using multicast transport. "\
"\n\n" \
" [-o|--host hostname]     the hostname of this process (def:localhost)\n" \
" [-m|--multicast ip:port] the multicast ip to send and listen to\n" \
" [-d|--domain domain]     the knowledge domain to send and listen to\n" \
" [-i|--id id]             the id of this agent (should be non-negative)\n" \
" [-l|--level level]       the logger level (0+, higher is higher detail)\n" \
" [-f|--logfile file]      log to a file\n" \
" [-r|--reduced]           use the reduced message header\n" \
"\n",
        argv[0]);
      exit (0);
    }
  }
}

void register_types()
  {
     //Any::register_type<Point>("Point");
    // Any::register_type<Quaternion>("Quaternion");
    // Any::register_type<Pose>("Pose");
    // Any::register_type<Stamp>("Stamp");
    // Any::register_type<StampedPose>("StampedPose");

    Any::register_schema("Point",
        capnp::Schema::from<geo_capn::Point>());
    Any::register_schema("StampedPoseList",
        capnp::Schema::from<geo_capn::StampedPoseList>());
  }



int main (int argc, char ** argv)
{
  settings.hosts.push_back (default_multicast);
  handle_arguments (argc, argv);
 
 register_types();


#ifndef _MADARA_NO_KARL_
  settings.type = madara::transport::MULTICAST;
  madara::knowledge::KnowledgeBase knowledge (host, settings);
  knowledge.set (".id",
    (madara::knowledge::KnowledgeRecord::Integer) settings.id,
    madara::knowledge::EvalSettings::SEND);

capnp::MallocMessageBuilder buffer;
  auto builder = buffer.initRoot<geo_capn::Point>();
  builder.setX(3);
  builder.setY(6);
  builder.setZ(9);

  GenericCapnObject point("Point", buffer);

  knowledge.set_any("pointOverNetwork", point,
    madara::knowledge::EvalSettings::SEND);

  knowledge.print ();
  
#else
  madara_logger_ptr_log (madara::logger::global_logger.get(), logger::LOG_ALWAYS,
    "This test is disabled due to karl feature being disabled.\n");
#endif
  return 0;
}