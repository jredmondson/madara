
//#define MADARA_NTRACE    0
//#define ACE_NTRACE    0
////#define ACE_NLOGGING  0
//#define ACE_NDEBUG    0

#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <fstream>
#include <set>
#include <assert.h>
#include <stdlib.h>

#include "ace/Log_Msg.h"
#include "ace/Get_Opt.h"
#include "ace/Signal.h"

#include "madara/knowledge_engine/KnowledgeBase.h"
#include "madara/utility/Utility.h"

Madara::KnowledgeRecord::Integer id = 0;
Madara::KnowledgeRecord::Integer left = 0;
Madara::KnowledgeRecord::Integer cars = 1;
Madara::KnowledgeRecord::Integer stop = 10;
Madara::KnowledgeRecord::Integer value = 0;
std::string mapfile = "/configs/maps/default.map";
std::string looppost_logicfile = "/configs/logics/simulator_looppost.klf";
std::string loopmain_logicfile = "/configs/logics/simulator_loopmain.klf";
std::string trafficlight_logicfile = "/configs/logics/traffic_light.klf";
std::string trafficlight_setupfile = "/configs/logics/traffic_light_setup.klf";
std::string settingsfile = "/configs/settings/default_simulation.klf";
std::string host = "localhost";

typedef std::set < std::string >                 Vertices;


typedef std::map < std::string, Madara::KnowledgeRecord::Integer > DistanceMap;
typedef std::map < std::string, DistanceMap >      ConnectivityMap; 

typedef std::vector <std::vector <char> >           LogicalMap;

volatile bool terminated = 0;

typedef std::vector <std::string>                   Path;

class Route
{
public:
  Path path;
  long distance;
};

class RouteComparator
{
  public:
    bool operator () (const Route &s1, const Route &s2) const
    {
      return s1.distance < s2.distance;
    }
};

typedef std::vector <Route>                        RouteVector;
typedef std::map <std::string, Route>              RouteTargetMap;
typedef std::map <std::string, RouteTargetMap>   RouteMap;


// command line arguments
int parse_args (int argc, ACE_TCHAR * argv[]);

// signal handler for someone hitting control+c
extern "C" void terminate (int)
{
  terminated = true;
}

void build_route_map (RouteMap & route_map,
                ConnectivityMap & connectivity_map,
                Vertices & vertices_orig,
                const std::string & source)
{
  // get the available connected vertices from source
  RouteTargetMap & targets = route_map[source];

  // we need our own copy of the vertices because we
  // will be deleting a vertex at each iteration,
  // starting with ourself
  Vertices vertices = vertices_orig;

  // erase our vertex. It's always distance 0.
  vertices.erase (source);

  // from wikipedia:
// 1  function Dijkstra(Graph, source):
// 2      for each vertex v in Graph:           
// 3          dist[v] := infinity ;              
// 4          previous[v] := undefined ;         
// 5      end for ;
// 6      dist[source] := 0 ;                    
// 7      Q := the set of all nodes in Graph ;
//        

// at this point, we are up to line 8 in implementation. route_map[source] is 
// our list of distances, and if it doesn't exist in the map, then we
// effectively have infinite distance
// We force source to be evaluated, so we step over step 8-12. Distance
// to closest neighbors of current source is also already included from
// building the connectivity graph. At this point, what we need to do is
// 

// 8      while Q is not empty:                 // The main loop
// 9          u := vertex in Q with smallest dist[] ;
//10          if dist[u] = infinity:
//11              break ;                        // all remaining vertices are inaccessible from source
//12          fi ;
//13          remove u from Q ;
//14          for each neighbor v of u:         // where v has not yet been removed from Q.
//15              alt := dist[u] + dist_between(u, v) ;
//16              if alt < dist[v]:             // Relax (u,v,a)
//17                  dist[v] := alt ;
//18                  previous[v] := u ;
//19              fi  ;
//20          end for ;
//21      end while ;
//22      return dist[] ;
//23  end Dijkstra.

  std::string current;

  while (vertices.size () > 0)
  {
    RouteVector targets_list;

    // build a list of targets from source since we can't sort a map
    for (RouteTargetMap::iterator i = targets.begin ();
         i != targets.end (); ++i)
    {
      targets_list.push_back (i->second);
    }

    // sort the current targets list from source
    std::sort (targets_list.begin (), targets_list.end (), RouteComparator ());

    current = "";

    // find the vertex with the shortest length that hasn't been visited
    // if the vertex is in vertices, then it hasn't been visited
    for (RouteVector::iterator i = targets_list.begin (); 
         i != targets_list.end (); ++i)
    {
      if (vertices.find (i->path.back ()) != vertices.end ())
      {
        // we've found a vertex that needs expanding. Let's setup the current
        // vertex to be evaluated and remove it from the vertex list
        current = i->path.back ();
        vertices.erase (current);
        break;
      }
    } // end for all sorted targets
    
    // if current is still null, then the vertices that remain are unreachable
    if (current == "")
      break;

    // at this point, we should have a valid current;
    // Let's see what it is connected to

    RouteTargetMap & cur_targets = route_map[current];

    RouteVector cur_targets_list;

    // build a list of targets so we can actually sort them
    // we can't sort a map
    for (RouteTargetMap::iterator i = cur_targets.begin ();
         i != cur_targets.end (); ++i)
    {
      cur_targets_list.push_back (i->second);
    }

    // sort the intermediate targets list from current
    std::sort (cur_targets_list.begin (), cur_targets_list.end (), 
      RouteComparator ());

    for (RouteVector::iterator i = cur_targets_list.begin (); 
         i != cur_targets_list.end (); ++i)
    {
      // the route target map might have been filled for this target,
      // so we are specifically looking for paths from current that
      // are equal to 2 (source to target for path vertex size of 2)
      if (i->path.size () > 2)
        continue;

      std::string last = i->path.back ();

      // we really don't want to create a uturn
      if (last == source)
        continue;

      // what would this new distance be?
      long new_distance = targets[current].distance + i->distance;

      // is this new target already in our map or at a distance greater
      // than our new distance?
      if (targets.find (last) == targets.end () || 
          new_distance < targets[last].distance)
      {
        // so, at this point, we have a valid ith element that can
        // be added to our targets map. We'll already have the path to
        // this point stored in targets[current].path and distance
        // to this point in targets[current].distance
        targets[last].distance = new_distance;

        // our path is going to be the current path plus this new element
        targets[last].path = targets[current].path;
        targets[last].path.push_back (last);
      }
    }
  }

}


void build_route_map (RouteMap & route_map, 
                ConnectivityMap & connectivity_map)
{
  // build a vertex vector
  Vertices vertices;
  for (RouteMap::iterator vertex = route_map.begin ();
       vertex != route_map.end (); ++vertex)
  {
    vertices.insert (vertex->first);
  }
  

  // use dijkstras on each 
  for (Vertices::iterator i = vertices.begin ();
       i != vertices.end (); ++i)
  {
    build_route_map (route_map, connectivity_map, vertices, *i);
  }
}

void print_logical_map (LogicalMap & logical_map)
{
  ACE_DEBUG ((LM_INFO, "Printing logical map...\n"));

  long max_y = logical_map[0].size ();
  long max_x = logical_map.size ();

  for (long y = 0; y < max_y; ++y)
  {
    for (long x = 0; x < max_x; ++x)
    {
      // we store nulls in the logical map if the position has
      // no street or light, so print a space instead

      char printable = logical_map[x][y] ? logical_map[x][y] : ' ';

      ACE_DEBUG ((LM_INFO, "%c ", printable));
    }
    // print a line when we cross the x boundary
    ACE_DEBUG ((LM_INFO, "\n"));
  }
}

void print_connectivity_map (ConnectivityMap & connectivity_map)
{
  ACE_DEBUG ((LM_INFO, "Printing connectivity map...\n"));

  for (ConnectivityMap::const_iterator i = connectivity_map.begin ();
       i != connectivity_map.end (); ++i)
  {
    ACE_DEBUG ((LM_INFO, "  %s\n", i->first.c_str ()));

    for (DistanceMap::const_iterator j = (*i).second.begin ();
       j != (*i).second.end (); ++j)
    {
      ACE_DEBUG ((LM_INFO, "   -> %s with distance %d\n", 
        j->first.c_str (), j->second));
    }
  }
}

void print_path (const Path & path)
{
  std::stringstream buffer;
  for (uint32_t i = 0; i < path.size (); ++i)
  {
    buffer << path[i];
    if (i != path.size () - 1)
      buffer << "->";
  }
  ACE_DEBUG ((LM_INFO, "%s\n", buffer.str ().c_str ()));
}

void print_route_map (RouteMap & route_map)
{
  ACE_DEBUG ((LM_INFO, "Printing route map...\n"));

  for (RouteMap::const_iterator i = route_map.begin ();
       i != route_map.end (); ++i)
  {
    ACE_DEBUG ((LM_INFO, "  %s\n", i->first.c_str ()));

    for (RouteTargetMap::const_iterator j = (*i).second.begin ();
       j != (*i).second.end (); ++j)
    {
      ACE_DEBUG ((LM_INFO, "   -> %s (%d) : ", 
        j->first.c_str (), j->second.distance));
      print_path (j->second.path);
    }
  }
}

void update_connectivity (ConnectivityMap & connectivity_map,
                          RouteMap & route_map,
                          std::string source, long dest_x, long dest_y)
{
  std::stringstream dest_buffer;

//  ACE_DEBUG ((LM_INFO, "  -> [%d:%d], val=%c\n", 
//    dest_x, dest_y, logical_map[x][y]));
  dest_buffer << dest_x;
  dest_buffer << ".";
  dest_buffer << dest_y;

  std::string dest = dest_buffer.str ();

  // update connectivity map. This may never actually
  // be used as the route map provides the same functionality
  // and a bit more
  connectivity_map [source][dest] = 1;

  // grab a reference to the route_map for source to dest
  Route & route = route_map [source][dest];

  // create a path directly from source to dest
  Path path;
  path.push_back (source);
  path.push_back (dest);

  // distance is 1. I may need this same code
  // some other time for distances that are not
  // just equal to path.size () - 1, so I'm making
  // the structure a bit more robust.
  route.distance = 1;

  // route
  route.path = path;
}


void build_connectivity (LogicalMap & logical_map, ConnectivityMap & connectivity_map,
                         RouteMap & route_map,
                         Madara::KnowledgeRecord::Integer cur_x,
                         Madara::KnowledgeRecord::Integer cur_y,
                         Madara::KnowledgeRecord::Integer max_x,
                         Madara::KnowledgeRecord::Integer max_y)
{
  // look around the current location for connectivity (roads/light)
  // 1 2 3         C 1    1 2     1 2      1 2
  // 4 C 5         2 3    3 C     C 3      3 C
  // 6 7 8                        4 5      4 5
             
  // compute starting x and y, see ascii outline above
  Madara::KnowledgeRecord::Integer x;
  Madara::KnowledgeRecord::Integer y;

  char printable = (char)logical_map[cur_x][cur_y];

  ACE_DEBUG ((LM_INFO, "Looking for connections from [%d:%d], val=%c\n", 
    cur_x, cur_y, printable));

  // build the source string (x:y)
  std::stringstream source_buffer;
  source_buffer << cur_x;
  source_buffer << ".";
  source_buffer << cur_y;
  std::string source = source_buffer.str ();

  // We should really only have to look at the cardinal directions
  // let's check north first
  if (cur_y > 0)
  {
    x = cur_x;
    y = cur_y - 1;
    if (logical_map[x][y] == 'r' || 
        logical_map[x][y] == 't' || 
        logical_map[x][y] == 'h')
    {
      update_connectivity (connectivity_map, route_map, source, x, y);
    }
  }

  // check south
  if (cur_y < max_y)
  {
    x = cur_x;
    y = cur_y + 1;
    if (logical_map[x][y] == 'r' || 
        logical_map[x][y] == 't' || 
        logical_map[x][y] == 'h')
    {
      update_connectivity (connectivity_map, route_map, source, x, y);
    }
  }

  // check west
  if (cur_x > 0)
  {
    x = cur_x - 1;
    y = cur_y;
    if (logical_map[x][y] == 'r' || 
        logical_map[x][y] == 't' || 
        logical_map[x][y] == 'h')
    {
      update_connectivity (connectivity_map, route_map, source, x, y);
    }
  }

  // check east
  if (cur_x < max_x)
  {
    x = cur_x + 1;
    y = cur_y;
    if (logical_map[x][y] == 'r' || 
        logical_map[x][y] == 't' || 
        logical_map[x][y] == 'h')
    {
      update_connectivity (connectivity_map, route_map, source, x, y);
    }
  }
}

void parse_map (const std::string & map, Madara::KnowledgeEngine::KnowledgeBase & knowledge,
                std::vector <std::pair <Madara::KnowledgeRecord::Integer, Madara::KnowledgeRecord::Integer> > & spawn_points,
                std::vector <std::pair <Madara::KnowledgeRecord::Integer, Madara::KnowledgeRecord::Integer> > & hospitals,
                std::vector <std::pair <Madara::KnowledgeRecord::Integer, Madara::KnowledgeRecord::Integer> > & traffic_lights,
                LogicalMap & logical_map, ConnectivityMap & connectivity_map,
                RouteMap & route_map)
{
  Madara::KnowledgeRecord::Integer x = 0, y = 0;
  Madara::KnowledgeRecord::Integer max_x = 0, max_y = 0;

  logical_map.clear ();

  for (uint32_t i = 0; i < map.size (); ++i)
  {
    knowledge.set (".x", x);
    knowledge.set (".y", y);

    // update the x and y coordinates
    if (map[i] == '\n')
    {
      if (x > max_x)
        max_x = x;

      //logical_map[x][y] = 0;
      x = 0;
      ++y;
    }
    else
    {
      if (map[i] == 'T' || map[i] == 't')
      {
        // keep traffic of this traffic light info. We'll use it to build
        // information into the knowledge base about which traffic light
        // is to the north, east, south, and west in the next loop after
        // we are fully informed of all of the traffic lights
        knowledge.evaluate (
"++.light_num;\
.map.{.x}.{.y}.type=.light_type;\
.map.{.x}.{.y}.id=.light_num;\
.light{.light_num}.x=.x;\
.light{.light_num}.y=.y;\
");
      }

      ++x;
    }
  }

  max_y = y - 1;

  // doing a bunch of "pushes" in the first loop was getting annoying.
  // since this is a one time setup, we'll just resize the arrays here
  // and then fill the logical_map with r, t, or 0

  std::vector <char> dummy;
  dummy.reserve (max_y + 2);
  dummy.insert (dummy.end (), max_y + 2, 0);

  logical_map.reserve (max_x + 2);
  logical_map.insert (logical_map.end (), max_x + 2, dummy);

  ACE_DEBUG ((LM_INFO, 
    "After resizing, logical map now has size of %d. max_y = %d\n", 
    logical_map.size (), max_y + 1));

  for (uint32_t i = 0; i < logical_map.size (); ++i)
  {
    ACE_DEBUG ((LM_INFO, 
      " logical_map[%d].size = %d\n", 
      i, logical_map[i].size ()));
  }

  x = 0;
  y = 0;
  // build a logical map so we can reference coordinates in [x][y]
  for (uint32_t i = 0; i < map.size (); ++i)
  {
    ACE_DEBUG ((LM_INFO, "Evaluating %d:%d\n", x, y));

    std::pair <long, long> current (x, y);
    if (map[i] == '\n')
    {
      x = 0;
      ++y;
    }
    else if (map[i] == '|' || map[i] == '_' || 
      map[i] == '-' || map[i] == '/' || map[i] == '\\')
    {
      if (map[i] == '|' && (y == 0 || y == max_y))
      {
        spawn_points.push_back (current);
        ACE_DEBUG ((LM_INFO, "(%P|%t)   Found spawn point ([%d:%d])\n", x, y));
      }
      else if (map[i] == '_' && (x == 0 || x == max_x))
      {
        spawn_points.push_back (current);
        ACE_DEBUG ((LM_INFO, "(%P|%t)   Found spawn point ([%d:%d])\n", x, y));
      }
      logical_map[x][y] = 'r';
    }
    else if (map[i] == 'T' || map[i] == 'T')
    {
      traffic_lights.push_back (current);
      logical_map[x][y] = 't';
    }
    else if (map[i] == 'H' || map[i] == 'h')
    {
      hospitals.push_back (current);
      logical_map[x][y] = 'h';
    }
    else
      logical_map[x][y] = 0;

    ++x;
  }

  knowledge.set (".max_x", max_x);
  knowledge.set (".max_y", max_y);

  x = 0;
  y = 0;

  ACE_DEBUG ((LM_INFO, "(%P|%t)   Found boundaries ([%d:%d])\n", max_x, max_y));

  // now that we've built a logical map, let's use it to figure out hospitals,
  // spawn locations, etc. and also build the connectivity map
  for (y = 0; y <= max_y; ++y)
  {
    for (x = 0; x <= max_x; ++x)
    {
      std::pair <long, long> current (x, y);

      // streets AND traffic lights also need information concerning the next
      // traffic light in either direction. This will help us figure out
      // whether or not we have green lights based on the car pos and allows
      // us to use KaRL to look up light info (even reinforcement values)
      if (logical_map[x][y] == 'r' || logical_map[x][y] == 't')
      {
        Madara::KnowledgeRecord::Integer i = y-1;
        // try to find next light to the north
        for (knowledge.set (".i", i); 
          knowledge.get (".i").to_integer () >= 0; 
           --i, knowledge.set (".i", i))
        {
          knowledge.evaluate (".map.{.x}.{.i}.type == .light_type => \
                   ( .map.{.x}.{.y}.light.north = .map.{.x}.{.i}.id; \
                     .i = -1 \
                   )");
        }

        i = y + 1;
        // try to find next light to the south
        for (knowledge.set (".i", i); 
          knowledge.get (".i").to_integer () < max_y + 1; 
           ++i, knowledge.set (".i", i))
        {
          knowledge.evaluate (".map.{.x}.{.i}.type == .light_type => \
                  ( .map.{.x}.{.y}.light.south = .map.{.x}.{.i}.id; \
                    .i = .max_y + 1 \
                  )");
        }

        i = x + 1;
        // try to find next light to the east
        for (knowledge.set (".i", i); 
          knowledge.get (".i").to_integer () < max_x + 1; 
          ++i, knowledge.set (".i", i))
        {
          knowledge.evaluate (".map.{.i}.{.y}.type == .light_type => \
                  ( .map.{.x}.{.y}.light.east = .map.{.i}.{.y}.id; \
                    .i = .max_x + 1 \
                  )");
        }

        i = x - 1;
        // try to find next light to the west
        for (knowledge.set (".i", i); 
          knowledge.get (".i").to_integer () >= 0; 
             --i, knowledge.set (".i", i))
        {
          knowledge.evaluate (".map.{.i}.{.y}.type == .light_type => \
                 ( .map.{.x}.{.y}.light.west = .map.{.i}.{.y}.id; \
                   .i = -1 \
                 )");
        }

        build_connectivity (logical_map, connectivity_map, route_map, x, y, max_x, max_y);
      } // end if the segment is road or traffic light
    }
  }

  build_route_map (route_map, connectivity_map);

  ACE_DEBUG ((LM_INFO, 
    "(%P|%t) Map contains the following properties:\n"));
  ACE_DEBUG ((LM_INFO, 
    "(%P|%t)   Spawn points:    %d\n", spawn_points.size ()));
  ACE_DEBUG ((LM_INFO, 
    "(%P|%t)   Traffic lights:  %d\n", traffic_lights.size ()));
  ACE_DEBUG ((LM_INFO, 
    "(%P|%t)   Hospitals:       %d\n", hospitals.size ()));

  print_logical_map (logical_map);
  print_connectivity_map (connectivity_map);
  print_route_map (route_map);
}

inline void process_car (
  Madara::KnowledgeEngine::KnowledgeBase & knowledge, RouteMap & route_map,
  const std::string & logic)
{
  // we're going to cheat a bit since KaRL does not support
  // for loops yet. Loopmain is going to setup a few variables
  // for us: 
  // .finished = .car{.i}.finish_time;
  //
  // if !.finished, then we set up the following
  // .cur_x = .car{.i}.cur.x;
  // .cur_y = .car{.i}.cur.y;
  // .dest_x = .car{.i}.dest.x;
  // .dest_y = .car{.i}.dest.y;
  // .velocity = .car{.i}.velocity;

  knowledge.evaluate ("\
.cur_x = .car{.i}.cur.x;\
.cur_y = .car{.i}.cur.y;\
.dest_x = .car{.i}.dest.x;\
.dest_y = .car{.i}.dest.y;\
.velocity = .car{.i}.velocity;");

  Madara::KnowledgeRecord cur_x = knowledge.get (".cur_x");
  Madara::KnowledgeRecord cur_y = knowledge.get (".cur_y");

  Madara::KnowledgeRecord dest_x = knowledge.get (".dest_x");
  Madara::KnowledgeRecord dest_y = knowledge.get (".dest_y");


  std::stringstream source_stream;
  std::stringstream dest_stream;

  source_stream << cur_x;
  source_stream << ".";
  source_stream << cur_y;

  dest_stream << dest_x;
  dest_stream << ".";
  dest_stream << dest_y;

  std::string source = source_stream.str ();
  std::string dest = dest_stream.str ();

  Route route = route_map [source][dest];
  Path path = route.path;

  Madara::KnowledgeRecord max_speed = knowledge.get ("max_speed");
  Madara::KnowledgeRecord::Integer j;

  for (j = 0; j < path.size () && j <= max_speed.to_integer (); ++j)
  {
    // j is the path point on the way to destination
    knowledge.set (".j", j);
    Madara::KnowledgeRecord::Integer x;
    Madara::KnowledgeRecord::Integer y;
    char separator;

    // buffer splits from a coord in the form of x.y
    std::stringstream buffer (path[j]);
    buffer >> x;
    buffer >> separator;
    buffer >> y;

    // set a temporary variable so we can go 
    knowledge.set (".x", x);
    knowledge.set (".y", y);

    // setup the path entry for this car. 
    knowledge.evaluate (".path.{.j}.x = .x; .path.{.j}.y = .y");
  }

  // fill up the path entries up to max_speed, to simplify the
  // job of the KaRL logic
  for (; j <= max_speed.to_integer (); ++j)
  {
    // j is the path point on the way to destination
    knowledge.set (".j", j);

    // setup the path entry for this car. 
    knowledge.evaluate (".path.{.j}.x = .x; .path.{.j}.y = .y");
  }
  
  knowledge.evaluate (logic);

  // .y_abs_diff = .y_diff = .cur_y - .dest_y;
  // .y_diff < 0 => .y_abs_diff = -.y_diff;

  // .x_abs_diff = .x_diff = .cur_x - .dest_x;
  // .x_diff < 0 => .x_abs_diff = -.x_diff;

  // .cur.vel.{0}.x = 
  
}

void process_cars (
  Madara::KnowledgeEngine::KnowledgeBase & knowledge, RouteMap & route_map,
  const std::string & logic)
{
  knowledge.set (".i", Madara::KnowledgeRecord::Integer (0));
  // run the main logic on the cars that currently exist
  for (knowledge.set (".i", Madara::KnowledgeRecord::Integer (0)); 
       (knowledge.get (".i") < knowledge.get (".num_cars")).is_true (); 
       knowledge.evaluate ("++.i"))
  {
    process_car (knowledge, route_map, logic);
  }
}

int ACE_TMAIN (int argc, ACE_TCHAR * argv[])
{
  int retcode = parse_args (argc, argv);

  if (retcode < 0)
    return retcode;

  ACE_LOG_MSG->priority_mask (LM_INFO, ACE_Log_Msg::PROCESS);

  ACE_TRACE (ACE_TEXT ("main"));

  // signal handler for clean exit
  ACE_Sig_Action sa ((ACE_SignalHandler) terminate, SIGINT);

  Madara::KnowledgeEngine::KnowledgeBase knowledge(
    host, Madara::Transport::SPLICE);

  ACE_DEBUG ((LM_INFO, "(%P|%t) Starting traffic simulation...\n" \
    "  logic: %s\n  map: %s\n  settings: %s\n",
    loopmain_logicfile.c_str (), mapfile.c_str (), settingsfile.c_str ()));

  // set my id
  knowledge.set (".id", id);
  knowledge.set (".hospital_type", Madara::KnowledgeRecord::Integer (20));
  knowledge.set (".light_type", Madara::KnowledgeRecord::Integer (10));
  knowledge.set (".northsouth_road_type");
  knowledge.set (".eastwest_road_type", Madara::KnowledgeRecord::Integer (2));

  // time related knowledge values
  // simulation_time = current time step in the simulation

  knowledge.set ("simulation_time", Madara::KnowledgeRecord::Integer (0));   // 3 minutes (180 seconds)

  std::stringstream main_logic;

  std::string filename = getenv ("MADARA_ROOT");
  filename += mapfile;

  std::string map = Madara::Utility::file_to_string (filename);

  filename = getenv ("MADARA_ROOT");
  filename += loopmain_logicfile;

  std::string loopmain_logic = Madara::Utility::file_to_string (filename);

  filename = getenv ("MADARA_ROOT");
  filename += looppost_logicfile;

  std::string looppost_logic = Madara::Utility::file_to_string (filename);

  filename = getenv ("MADARA_ROOT");
  filename += settingsfile;

  std::string settings = Madara::Utility::file_to_string (filename);

  filename = getenv ("MADARA_ROOT");
  filename += trafficlight_logicfile;

  std::string light_logic = Madara::Utility::file_to_string (filename);

  filename = getenv ("MADARA_ROOT");
  filename += trafficlight_setupfile;

  std::string light_setup = Madara::Utility::file_to_string (filename);

  std::ofstream main_logic_file;

  main_logic_file.open ("ts_main_logic.klf");

  if (main_logic_file.is_open ())
  {
    main_logic_file << loopmain_logic;
    main_logic_file.close ();
  }

  ACE_DEBUG ((LM_INFO, "(%P|%t) Main simulator logic is the following:\n%s\n\n",
    loopmain_logic.c_str ()));

  ACE_DEBUG ((LM_INFO, "(%P|%t) Traffic light logic is the following:\n%s\n\n",
    light_logic.c_str ()));

  ACE_DEBUG ((LM_INFO, "(%P|%t) Map is the following:\n%s\n\n",
    map.c_str ()));

  ACE_DEBUG ((LM_INFO, "(%P|%t) Simulation settings are the following:\n%s\n\n",
    settings.c_str ()));

  knowledge.evaluate (settings);

  ACE_DEBUG ((LM_DEBUG, "(%P|%t) Starting Knowledge\n"));
  knowledge.print_knowledge ();

  // this doesn't change from settings
  Madara::KnowledgeRecord spawn_rate = knowledge.get ("spawn_rate");
  Madara::KnowledgeRecord max_speed = knowledge.get ("max_speed");

  // can't have a max_speed that is zero or we'll have
  // floating point exceptions
  if (max_speed.to_integer () == 0)
    max_speed.set_value (Madara::KnowledgeRecord::Integer (1));

  long num_cars = 0;

  LogicalMap logical_map;
  ConnectivityMap connectivity_map;
  RouteMap route_map;

  std::vector <std::pair <Madara::KnowledgeRecord::Integer, Madara::KnowledgeRecord::Integer> > spawn_points;
  std::vector <std::pair <Madara::KnowledgeRecord::Integer, Madara::KnowledgeRecord::Integer> > hospitals;
  std::vector <std::pair <Madara::KnowledgeRecord::Integer, Madara::KnowledgeRecord::Integer> > traffic_lights;
  
  std::vector <std::vector <Madara::KnowledgeRecord::Integer> >     map_lookup;

  parse_map (map, knowledge, spawn_points, hospitals, 
    traffic_lights, logical_map, connectivity_map, route_map);

  //return 0;

  // setup traffic light defaults (like braking time)
  // note that these setup statements should be setup so that
  // users may override these setup values in the default_simulation.klf file
  for (knowledge.set (".id"); 
    (knowledge.get (".id") <= knowledge.get (".light_num")).is_true (); 
       knowledge.evaluate ("++.id"))
  {
    knowledge.evaluate (light_setup);
  }

  // termination is done via signalling from the user (Control+C)
  while (!terminated)
  {
    // run the traffic light logic next

    for (knowledge.set (".id"); 
         (knowledge.get (".id") <= knowledge.get (".light_num")).is_true (); 
         knowledge.evaluate ("++.id"))
    {
      knowledge.evaluate (light_logic);
    }

    // process cars that are in play
    process_cars (knowledge, route_map, loopmain_logic);

    // spawn new cars
    for (Madara::KnowledgeRecord::Integer i = 0;
         i < spawn_rate.to_integer (); ++i, ++num_cars)
    {
      Madara::KnowledgeRecord::Integer destination = rand () % spawn_points.size ();
      Madara::KnowledgeRecord::Integer start = rand () % spawn_points.size ();
      Madara::KnowledgeRecord::Integer estimated_time = 0;

      // try to handle case where map has just one spawn point gracefully
      // generate a start location until we find a start location that
      // is different from its destination (e.g. it has to move)
      if (spawn_points.size () > 1)
        while (start == destination)
          start = rand () % spawn_points.size ();

      knowledge.set (".cur_start.x", spawn_points[start].first);
      knowledge.set (".cur_start.y", spawn_points[start].second);
      knowledge.set (".cur_destination.x", spawn_points[destination].first);
      knowledge.set (".cur_destination.y", spawn_points[destination].second);

      estimated_time += abs (spawn_points[start].first - spawn_points[destination].first);
      estimated_time += abs (spawn_points[start].second - spawn_points[destination].second);

      estimated_time /= max_speed.to_integer ();

      if (estimated_time % max_speed.to_integer () > 0)
        ++estimated_time;

      knowledge.set (".estimated_time", estimated_time);

      // spawned car setup expression
      std::string expression = "\
 .car{.num_cars}.start.x=.cur_start.x; \
 .car{.num_cars}.start.y=.cur_start.y; \
 .car{.num_cars}.cur.x=.cur_start.x; \
 .car{.num_cars}.cur.y=.cur_start.y; \
 .car{.num_cars}.dest.x=.cur_destination.x; \
 .car{.num_cars}.dest.y=.cur_destination.y; \
 .car{.num_cars}.est_time=.estimated_time; \
 car{.num_cars}.start_time=simulation_time; \
 .car{.num_cars}.velocity=max_speed; \
 ++.num_cars \
";
      knowledge.evaluate (expression);
    }

    // update 
    knowledge.evaluate (looppost_logic);
    knowledge.print("time: {simulation_time} of {simulation_cutoff}\n");

    // cutoff the simulation if we have passed the cutoff
    if ((knowledge.get ("simulation_time") >= knowledge.get ("simulation_cutoff")).is_true ())
    {
      terminated = true;
      knowledge.set ("simulation_finished");
    }

    ACE_DEBUG ((LM_DEBUG, "(%P|%t) Knowledge at time=%d\n", 
      knowledge.get ("simulation_time")));
    knowledge.print_knowledge ();
    madara::utility::sleep (1);
  }

  ACE_DEBUG ((LM_DEBUG, "(%P|%t) Final Knowledge\n"));
  knowledge.print_knowledge ();

  ACE_DEBUG ((LM_DEBUG, "(%P|%t) Exiting\n"));
  return 0;
}



int parse_args (int argc, ACE_TCHAR * argv[])
{
  // options string which defines all short args
  ACE_TCHAR options [] = ACE_TEXT ("i:m:s:l:o:h");

  // create an instance of the command line args
  ACE_Get_Opt cmd_opts (argc, argv, options);

  // set up an alias for '-n' to be '--name'
  cmd_opts.long_option (ACE_TEXT ("id"), 'i', ACE_Get_Opt::ARG_REQUIRED);
  cmd_opts.long_option (ACE_TEXT ("map"), 'm', ACE_Get_Opt::ARG_REQUIRED);
  cmd_opts.long_option (ACE_TEXT ("settings"), 's', ACE_Get_Opt::ARG_REQUIRED);
  cmd_opts.long_option (ACE_TEXT ("logic"), 'l', ACE_Get_Opt::ARG_REQUIRED);
  cmd_opts.long_option (ACE_TEXT ("help"), 'h', ACE_Get_Opt::NO_ARG);
  cmd_opts.long_option (ACE_TEXT ("host"), 'o', ACE_Get_Opt::ARG_REQUIRED);
 
  // temp for current switched option
  int option;
//  ACE_TCHAR * arg;

  // iterate through the options until done
  while ((option = cmd_opts ()) != EOF)
  {
    //arg = cmd_opts.opt_arg ();
    switch (option)
    {
    case 'i':
      // thread number
      id = atoi (cmd_opts.opt_arg ());
      break;
    case 'm':
      // thread number
      mapfile = cmd_opts.opt_arg ();
      break;
    case 'l':
      // thread number
      loopmain_logicfile = cmd_opts.opt_arg ();
      break;
    case 's':
      // thread number
      settingsfile = cmd_opts.opt_arg ();
      break;
    case 'o':
      host = cmd_opts.opt_arg ();
      ACE_DEBUG ((LM_DEBUG, "(%P|%t) host set to %s\n", host.c_str ()));
      break;
    case ':':
      ACE_ERROR_RETURN ((LM_ERROR, 
        ACE_TEXT ("ERROR: -%c requires an argument"), 
           cmd_opts.opt_opt ()), -2); 
    case 'h':
    default:
      ACE_DEBUG ((LM_DEBUG, "Program Options:      \n\
      -i (--id)        set process id (0 default)  \n\
      -m (--map)       map file name for traffic lights/hospitals/streets \n\
                       note that this path will start from $MADARA_ROOT \n\
      -l (--logic)     logic file name from $MADARA_ROOT \n\
      -s (--settings)  configuration for simulation from $MADARA_ROOT\n\
      -o (--host)      this host ip/name (localhost default) \n\
      -h (--help)      print this menu             \n"));
      ACE_ERROR_RETURN ((LM_ERROR, 
        ACE_TEXT ("Returning from Help Menu")), -1); 
      break;
    }
  }

  return 0;
}
