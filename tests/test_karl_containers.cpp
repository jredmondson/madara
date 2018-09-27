
#include "madara/knowledge/containers/Vector.h"
#include "madara/knowledge/containers/IntegerVector.h"
#include "madara/knowledge/containers/DoubleVector.h"
#include "madara/knowledge/containers/NativeDoubleVector.h"
#include "madara/knowledge/containers/StringVector.h"
#include "madara/knowledge/containers/Map.h"
#include "madara/knowledge/containers/FlexMap.h"
#include "madara/knowledge/containers/Integer.h"
#include "madara/knowledge/containers/Double.h"
#include "madara/knowledge/containers/Queue.h"
#include "madara/knowledge/containers/Collection.h"
#include "madara/knowledge/containers/DoubleVectorVector.h"
#include "madara/knowledge/containers/IntegerVectorVector.h"
#include "madara/knowledge/containers/DoubleVector2D.h"
#include "madara/knowledge/containers/DoubleVector3D.h"
#include "madara/knowledge/containers/IntegerVector2D.h"
#include "madara/knowledge/containers/IntegerVector3D.h"
#include "madara/knowledge/containers/CircularBuffer.h"
#include "madara/knowledge/containers/CircularBufferT.h"
#include "madara/knowledge/containers/NativeCircularBufferConsumer.h"
#include "madara/knowledge/containers/CircularBufferConsumer.h"
#include "madara/knowledge/containers/CircularBufferConsumerT.h"
#include "madara/knowledge/KnowledgeBase.h"
#include <iostream>

namespace knowledge = madara::knowledge;
namespace containers = knowledge::containers;
typedef knowledge::KnowledgeRecord   KnowledgeRecord;
typedef knowledge::KnowledgeVector   KnowledgeVector;

int madara_fails (0);

void test_flex_map (void)
{
  std::cerr << "************* FLEX MAP: CREATING FLEX MAP*************\n";
  knowledge::KnowledgeBase knowledge;
  containers::FlexMap map ("records", knowledge);

  std::cerr << "Creating record with name and age\n";
  map["robert"]["name"] = "Robert Jenkins";
  map["robert"]["age"] = 49;

  std::cerr << "Creating Flex Map from index operator\n";
  containers::FlexMap robert_record = map["robert"];

  robert_record["salary"] = 30500.00;
  robert_record["age"] = robert_record["age"].to_integer () + 1;

  containers::Map normal_map;
  robert_record.to_container (normal_map);

  std::cerr << "Checking for flex map creation test success:\n";
  std::cerr << "  Checking size of normal map creation... ";
  if (normal_map.size () == 3)
  {
    std::cerr << "SUCCESS\n";
  }
  else
  {
    std::cerr << "FAIL\n"; ++madara_fails;
  }

  std::cerr << "  Checking elements of normal map...\n";

  std::cerr << "    Retrieving name...";
  if (normal_map["name"] == "Robert Jenkins")
  {
    std::cerr << "SUCCESS\n";
  }
  else
  {
    std::cerr << "FAIL\n"; ++madara_fails;
  }

  std::cerr << "    Retrieving age...";
  if (normal_map["age"] == knowledge::KnowledgeRecord::Integer (50))
  {
    std::cerr << "SUCCESS\n";
  }
  else
  {
    std::cerr << "FAIL\n"; ++madara_fails;
  }

  std::cerr << "    Retrieving salary...";
  if (normal_map["salary"] == 30500.00)
  {
    std::cerr << "SUCCESS\n";
  }
  else
  {
    std::cerr << "FAIL\n"; ++madara_fails;
  }

  std::cerr << "Adding names to the flex map\n";
  map["norman"]["name"] = "Norman Roberts";
  map["greg"]["name"] = "Gregory Shaw";
  map["rupert"]["name"] = "Rupert Glasgow";
  map["cassie"]["name"] = "Cassandra Collins";

  std::cerr << "Attempting to_map function for subkeys\n";
  std::vector<std::string> next_keys;
  std::map<std::string, KnowledgeRecord> all_record_vars;
  knowledge.to_map ("records", ".", "", next_keys, all_record_vars);

  std::cerr << "  Size check on next keys and all records are a: ";
  if (next_keys.size () == 5 && all_record_vars.size () == 7)
  {
    std::cerr << "SUCCESS\n";
  }
  else
  {
    std::cerr << "FAIL\n"; ++madara_fails;
  }

  std::vector<std::string> sub_keys;

  std::cerr << "Attempting keys function on FlexMap\n";
  map.keys (sub_keys, true);

  std::cerr << "  Size check on flex map's sub keys: ";
  if (sub_keys.size () == 5)
  {
    std::cerr << "SUCCESS\n";
  }
  else
  {
    std::cerr << "FAIL\n"; ++madara_fails;
  }

  std::cerr << "  Checking context.to_map vs flex_map.keys: ";
  if (sub_keys == next_keys)
  {
    std::cerr << "SUCCESS\n";
  }
  else
  {
    std::cerr << "FAIL\n"; ++madara_fails;
  }

  std::cerr << "Adding mixes of string and index keys to a flex map\n";
  map["robert"]["tickets"][0]["offense"] = "Slapping a seal";
  map["robert"]["tickets"][0]["fine"] = 120.00;
  map["robert"]["tickets"][1]["offense"] = "Calling an employee a hobbit";
  map["robert"]["tickets"][1]["fine"] = 5.00;
  map["cassie"]["tickets"][0]["offense"] = "Stabbing Robert for calling her a hobbit";
  map["cassie"]["tickets"][0]["fine"] = 250.00;
  map["cassie"]["tickets"][0]["notes"] = "Victim Robert claimed it was worth it";

  std::cerr << "  Checking existence of new indexed items in flex map: ";
  if (robert_record.exists ("tickets.0.offense")
    && robert_record.exists ("tickets.1.offense")
    && map.exists ("cassie.tickets.0.offense")
    && map.exists ("cassie.tickets.0.notes")
    && map["cassie"]["tickets"][0]["notes"].exists ()
    && !map["cassie"]["tickets"][0].exists ())
  {
    std::cerr << "SUCCESS\n";
  }
  else
  {
    std::cerr << "FAIL\n"; ++madara_fails;
  }

  std::cerr << "  Checking existence of subkeys in flex map: ";
  if (robert_record.exists ("tickets", true)
    && robert_record.exists ("name", true)
    && map.exists ("cassie", true)
    && map.exists ("greg", true)
    && map.exists ("norman", true))
  {
    std::cerr << "SUCCESS\n";
  }
  else
  {
    std::cerr << "FAIL\n"; ++madara_fails;
  }

  std::cerr << "Changing delimiter of top level map: \n";
  map.set_delimiter (";");

  std::cerr << "  Checking subkeys of top level map: ";
  map.keys (sub_keys, true);

  if (sub_keys.size () == 0)
  {
    std::cerr << "SUCCESS\n";
  }
  else
  {
    std::cerr << "FAIL\n"; ++madara_fails;
  }


  std::cerr << "Resetting delimiter of top level map: \n";
  map.set_delimiter (".");

  std::cerr << "  Checking subkeys of top level map: ";
  map.keys (sub_keys, true);

  if (sub_keys.size () != 0)
  {
    std::cerr << "SUCCESS\n";
  }
  else
  {
    std::cerr << "FAIL\n"; ++madara_fails;
  }


  std::cerr << "  Checking to_integer defaults: ";
  if (map["unknown"]["int"].to_integer (5) == 5)
  {
    std::cerr << "SUCCESS\n";
  }
  else
  {
    std::cerr << "FAIL\n"; ++madara_fails;
  }

  std::cerr << "  Checking to_string defaults: ";
  if (map["unknown"]["string"].to_string ("nope") == "nope")
  {
    std::cerr << "SUCCESS\n";
  }
  else
  {
    std::cerr << "FAIL\n"; ++madara_fails;
  }

  std::cerr << "  Checking to_double defaults: ";
  if (map["unknown"]["double"].to_double (3.25) == 3.25)
  {
    std::cerr << "SUCCESS\n";
  }
  else
  {
    std::cerr << "FAIL\n"; ++madara_fails;
  }

  std::cerr << "  Checking to_record defaults: ";
  if (map["unknown"]["record"].to_record (knowledge::KnowledgeRecord (1)) == 1)
  {
    std::cerr << "SUCCESS\n";
  }
  else
  {
    std::cerr << "FAIL\n"; ++madara_fails;
  }
}


void test_vector2D (void)
{
  std::cerr << "************* DOUBLEVECTOR2D: CREATING*************\n";
  knowledge::KnowledgeBase knowledge;
  containers::DoubleVector2D double_vector (
    "test_doublevector2D", knowledge, {10,10});
  std::cerr << "  Checking size()\n";
  containers::DoubleVector2D::Dimensions size = double_vector.size ();

  std::cerr << "  Setting indices\n";
  // set the vector entries
  double_vector.set ({1,0}, 10.22);
  double_vector.set ({1,1}, 7.31);
  double_vector.set ({1,2}, 6.179);
  double_vector.set ({7,0}, 8.12);
  double_vector.set ({7,1}, 3.34);
  double_vector.set ({7,2}, 214.555);

  // retrieve the indexes

  std::cerr << "  Retrieving indices\n";
  double array1_0 = double_vector[{1,0}];
  double array1_1 = double_vector[{1,1}];
  double array1_2 = double_vector[{1,2}];
  double array7_0 = double_vector[{7,0}];
  double array7_1 = double_vector[{7,1}];
  double array7_2 = double_vector[{7,2}];

  // check the array size

  std::cerr << "  Checking array size: [" << size.x << "," << size.y <<"]: "<<
    (size.x == 10 && size.y == 10 ? "SUCCESS\n" : "FAIL\n");

  // check the array values
  
  std::cerr << "  Checking all array indices for values: ";
  if (array1_0 == 10.22 && array1_1 == 7.31 && array1_2 == 6.179 &&
      array7_0 == 8.12 && array7_1 == 3.34 && array7_2 == 214.555)
  {
    std::cerr << "SUCCESS\n";
  }
  else
  {
    std::cerr << "FAIL\n"; ++madara_fails;
    std::cerr << "array1_1 = " << array1_0 << "\n";
    std::cerr << "array1_2 = " << array1_1 << "\n";
    std::cerr << "array1_3 = " << array1_2 << "\n";
    std::cerr << "array7_1 = " << array7_0 << "\n";
    std::cerr << "array7_2 = " << array7_1 << "\n";
    std::cerr << "array7_3 = " << array7_2 << "\n";
  }

  std::cerr << "  Checking resize()\n";
  double_vector.resize ({5,2});
  size = double_vector.size ();

  array1_0 = double_vector[{1,0}];
  array1_1 = double_vector[{1,1}];
  array1_2 = double_vector[{1,2}];
  array7_0 = double_vector[{7,0}];
  array7_1 = double_vector[{7,1}];
  array7_2 = double_vector[{7,2}];
  
  std::cerr << "  Checking array size: [" << size.x << "," << size.y <<"]: "<<
    (size.x == 5 && size.y == 2 ? "SUCCESS\n" : "FAIL\n");

  std::cerr << "  Checking all array indices for values: ";
  if (array1_0 == 10.22 && array1_1 == 7.31 && array1_2 == 0 &&
      array7_0 == 0 && array7_1 == 0 && array7_2 == 0)
  {
    std::cerr << "SUCCESS\n";
  }
  else
  {
    std::cerr << "FAIL\n"; ++madara_fails;
    std::cerr << "array1_1 = " << array1_0 << "\n";
    std::cerr << "array1_2 = " << array1_1 << "\n";
    std::cerr << "array1_3 = " << array1_2 << "\n";
    std::cerr << "array7_1 = " << array7_0 << "\n";
    std::cerr << "array7_2 = " << array7_1 << "\n";
    std::cerr << "array7_3 = " << array7_2 << "\n";
  }

  std::cerr << "  Checking resize()\n";
  double_vector.resize ({8,3});
  size = double_vector.size ();

  std::cerr << "  Setting indices\n";
  // set the vector entries
  double_vector.set ({1,0}, 10.22);
  double_vector.set ({1,1}, 7.31);
  double_vector.set ({1,2}, 6.179);
  double_vector.set ({7,0}, 8.12);
  double_vector.set ({7,1}, 3.34);
  double_vector.set ({7,2}, 214.555);

  array1_0 = double_vector[{1,0}];
  array1_1 = double_vector[{1,1}];
  array1_2 = double_vector[{1,2}];
  array7_0 = double_vector[{7,0}];
  array7_1 = double_vector[{7,1}];
  array7_2 = double_vector[{7,2}];
  
  // check the array size

  std::cerr << "  Checking array size: [" << size.x << "," << size.y <<"]: "<<
    (size.x == 8 && size.y == 3 ? "SUCCESS\n" : "FAIL\n");

  // check the array values
  
  std::cerr << "  Checking all array indices for values: ";
  if (array1_0 == 10.22 && array1_1 == 7.31 && array1_2 == 6.179 &&
      array7_0 == 8.12 && array7_1 == 3.34 && array7_2 == 214.555)
  {
    std::cerr << "SUCCESS\n";
  }
  else
  {
    std::cerr << "FAIL\n"; ++madara_fails;
    std::cerr << "array1_1 = " << array1_0 << "\n";
    std::cerr << "array1_2 = " << array1_1 << "\n";
    std::cerr << "array1_3 = " << array1_2 << "\n";
    std::cerr << "array7_1 = " << array7_0 << "\n";
    std::cerr << "array7_2 = " << array7_1 << "\n";
    std::cerr << "array7_3 = " << array7_2 << "\n";
  }

  knowledge.clear_modifieds ();




  std::cerr << "************* INTEGERVECTOR2D: CREATING*************\n";
  containers::IntegerVector2D int_vector (
    "test_integervector2D", knowledge, {10,10});
  std::cerr << "  Checking size()\n";
  containers::IntegerVector2D::Dimensions isize = int_vector.size ();

  std::cerr << "  Setting indices\n";
  // set the vector entries
  int_vector.set ({1,0}, 10);
  int_vector.set ({1,1}, 7);
  int_vector.set ({1,2}, 6);
  int_vector.set ({7,0}, 8);
  int_vector.set ({7,1}, 3);
  int_vector.set ({7,2}, 214);

  // retrieve the indexes

  std::cerr << "  Retrieving indices\n";
  int64_t iarray1_0 = int_vector[{1,0}];
  int64_t iarray1_1 = int_vector[{1,1}];
  int64_t iarray1_2 = int_vector[{1,2}];
  int64_t iarray7_0 = int_vector[{7,0}];
  int64_t iarray7_1 = int_vector[{7,1}];
  int64_t iarray7_2 = int_vector[{7,2}];

  // check the iarray size

  std::cerr << "  Checking iarray size: [" << isize.x << "," << isize.y <<"]: "<<
    (isize.x == 10 && isize.y == 10 ? "SUCCESS\n" : "FAIL\n");

  // check the iarray values
  
  std::cerr << "  Checking all iarray indices for values: ";
  if (iarray1_0 == 10 && iarray1_1 == 7 && iarray1_2 == 6 &&
      iarray7_0 == 8 && iarray7_1 == 3 && iarray7_2 == 214)
  {
    std::cerr << "SUCCESS\n";
  }
  else
  {
    std::cerr << "FAIL\n"; ++madara_fails;
    std::cerr << "iarray1_1 = " << iarray1_0 << "\n";
    std::cerr << "iarray1_2 = " << iarray1_1 << "\n";
    std::cerr << "iarray1_3 = " << iarray1_2 << "\n";
    std::cerr << "iarray7_1 = " << iarray7_0 << "\n";
    std::cerr << "iarray7_2 = " << iarray7_1 << "\n";
    std::cerr << "iarray7_3 = " << iarray7_2 << "\n";
  }

  std::cerr << "  Checking resize()\n";
  int_vector.resize ({5,2});
  isize = int_vector.size ();

  iarray1_0 = int_vector[{1,0}];
  iarray1_1 = int_vector[{1,1}];
  iarray1_2 = int_vector[{1,2}];
  iarray7_0 = int_vector[{7,0}];
  iarray7_1 = int_vector[{7,1}];
  iarray7_2 = int_vector[{7,2}];
  
  std::cerr << "  Checking iarray size: [" << 
     isize.x << "," << isize.y <<"]: "<<
    (isize.x == 5 && isize.y == 2 ? "SUCCESS\n" : "FAIL\n");

  std::cerr << "  Checking all iarray indices for values: ";
  if (iarray1_0 == 10 && iarray1_1 == 7 && iarray1_2 == 0 &&
      iarray7_0 == 0 && iarray7_1 == 0 && iarray7_2 == 0)
  {
    std::cerr << "SUCCESS\n";
  }
  else
  {
    std::cerr << "FAIL\n"; ++madara_fails;
    std::cerr << "iarray1_1 = " << iarray1_0 << "\n";
    std::cerr << "iarray1_2 = " << iarray1_1 << "\n";
    std::cerr << "iarray1_3 = " << iarray1_2 << "\n";
    std::cerr << "iarray7_1 = " << iarray7_0 << "\n";
    std::cerr << "iarray7_2 = " << iarray7_1 << "\n";
    std::cerr << "iarray7_3 = " << iarray7_2 << "\n";
  }

  std::cerr << "  Checking resize()\n";
  int_vector.resize ({8,3});
  isize = int_vector.size ();

  std::cerr << "  Setting indices\n";
  // set the vector entries
  int_vector.set ({1,0}, 10);
  int_vector.set ({1,1}, 7);
  int_vector.set ({1,2}, 6);
  int_vector.set ({7,0}, 8);
  int_vector.set ({7,1}, 3);
  int_vector.set ({7,2}, 214);

  iarray1_0 = int_vector[{1,0}];
  iarray1_1 = int_vector[{1,1}];
  iarray1_2 = int_vector[{1,2}];
  iarray7_0 = int_vector[{7,0}];
  iarray7_1 = int_vector[{7,1}];
  iarray7_2 = int_vector[{7,2}];
  
  // check the iarray size

  std::cerr << "  Checking iarray size: [" << isize.x << "," << isize.y <<"]: "<<
    (isize.x == 8 && isize.y == 3 ? "SUCCESS\n" : "FAIL\n");

  // check the iarray values
  
  std::cerr << "  Checking all iarray indices for values: ";
  if (iarray1_0 == 10 && iarray1_1 == 7 && iarray1_2 == 6 &&
      iarray7_0 == 8 && iarray7_1 == 3 && iarray7_2 == 214)
  {
    std::cerr << "SUCCESS\n";
  }
  else
  {
    std::cerr << "FAIL\n"; ++madara_fails;
    std::cerr << "iarray1_1 = " << iarray1_0 << "\n";
    std::cerr << "iarray1_2 = " << iarray1_1 << "\n";
    std::cerr << "iarray1_3 = " << iarray1_2 << "\n";
    std::cerr << "iarray7_1 = " << iarray7_0 << "\n";
    std::cerr << "iarray7_2 = " << iarray7_1 << "\n";
    std::cerr << "iarray7_3 = " << iarray7_2 << "\n";
  }

  knowledge.clear_modifieds ();
}

void test_vector3D (void)
{
  std::cerr << "************* DOUBLEVECTOR3D: CREATING*************\n";
  knowledge::KnowledgeBase knowledge;
  containers::DoubleVector3D double_vector (
    "test_doublevector2D", knowledge, {10,10,10});
  std::cerr << "  Checking size()\n";
  containers::DoubleVector3D::Dimensions size = double_vector.size ();

  std::cerr << "  Setting indices\n";
  // set the vector entries
  double_vector.set ({1,0,0}, 10.22);
  double_vector.set ({1,1,1}, 7.31);
  double_vector.set ({1,2,2}, 6.179);
  double_vector.set ({7,0,0}, 8.12);
  double_vector.set ({7,1,1}, 3.34);
  double_vector.set ({7,2,2}, 214.555);

  // retrieve the indexes

  std::cerr << "  Retrieving indices\n";
  double array1_0 = double_vector[{1,0,0}];
  double array1_1 = double_vector[{1,1,1}];
  double array1_2 = double_vector[{1,2,2}];
  double array7_0 = double_vector[{7,0,0}];
  double array7_1 = double_vector[{7,1,1}];
  double array7_2 = double_vector[{7,2,2}];

  // check the array size

  std::cerr << "  Checking array size: [" <<
     size.x << "," << size.y << "," << size.z <<"]: "<<
    (size.x == 10 && size.y == 10 && size.z == 10 ? "SUCCESS\n" : "FAIL\n");

  // check the array values
  
  std::cerr << "  Checking all array indices for values: ";
  if (array1_0 == 10.22 && array1_1 == 7.31 && array1_2 == 6.179 &&
      array7_0 == 8.12 && array7_1 == 3.34 && array7_2 == 214.555)
  {
    std::cerr << "SUCCESS\n";
  }
  else
  {
    std::cerr << "FAIL\n"; ++madara_fails;
    std::cerr << "array1_1 = " << array1_0 << "\n";
    std::cerr << "array1_2 = " << array1_1 << "\n";
    std::cerr << "array1_3 = " << array1_2 << "\n";
    std::cerr << "array7_1 = " << array7_0 << "\n";
    std::cerr << "array7_2 = " << array7_1 << "\n";
    std::cerr << "array7_3 = " << array7_2 << "\n";
  }

  std::cerr << "  Checking resize()\n";
  double_vector.resize ({5,2,2});
  size = double_vector.size ();

  array1_0 = double_vector[{1,0,0}];
  array1_1 = double_vector[{1,1,1}];
  array1_2 = double_vector[{1,2,2}];
  array7_0 = double_vector[{7,0,0}];
  array7_1 = double_vector[{7,1,1}];
  array7_2 = double_vector[{7,2,2}];
  
  std::cerr << "  Checking array size: [" <<
     size.x << "," << size.y << "," << size.z <<"]: "<<
    (size.x == 5 && size.y == 2 && size.z == 2 ? "SUCCESS\n" : "FAIL\n");

  std::cerr << "  Checking all array indices for values: ";
  if (array1_0 == 10.22 && array1_1 == 7.31 && array1_2 == 0 &&
      array7_0 == 0 && array7_1 == 0 && array7_2 == 0)
  {
    std::cerr << "SUCCESS\n";
  }
  else
  {
    std::cerr << "FAIL\n"; ++madara_fails;
    std::cerr << "array1_1 = " << array1_0 << "\n";
    std::cerr << "array1_2 = " << array1_1 << "\n";
    std::cerr << "array1_3 = " << array1_2 << "\n";
    std::cerr << "array7_1 = " << array7_0 << "\n";
    std::cerr << "array7_2 = " << array7_1 << "\n";
    std::cerr << "array7_3 = " << array7_2 << "\n";
  }

  std::cerr << "  Checking resize()\n";
  double_vector.resize ({8,3,4});
  size = double_vector.size ();

  std::cerr << "  Setting indices\n";
  // set the vector entries
  double_vector.set ({1,0,0}, 10.22);
  double_vector.set ({1,1,1}, 7.31);
  double_vector.set ({1,2,2}, 6.179);
  double_vector.set ({7,0,0}, 8.12);
  double_vector.set ({7,1,1}, 3.34);
  double_vector.set ({7,2,2}, 214.555);

  //madara::logger::global_logger->set_level (madara::logger::LOG_MINOR);

  array1_0 = double_vector[{1,0,0}];
  array1_1 = double_vector[{1,1,1}];
  array1_2 = double_vector[{1,2,2}];
  array7_0 = double_vector[{7,0,0}];
  array7_1 = double_vector[{7,1,1}];
  array7_2 = double_vector[{7,2,2}];
  
  // check the array size

  std::cerr << "  Checking array size: [" <<
     size.x << "," << size.y << "," << size.z <<"]: "<<
    (size.x == 8 && size.y == 3 && size.z == 4 ? "SUCCESS\n" : "FAIL\n");

  // check the array values
  
  std::cerr << "  Checking all array indices for values: ";
  if (array1_0 == 10.22 && array1_1 == 7.31 && array1_2 == 6.179 &&
      array7_0 == 8.12 && array7_1 == 3.34 && array7_2 == 214.555)
  {
    std::cerr << "SUCCESS\n";
  }
  else
  {
    std::cerr << "FAIL\n"; ++madara_fails;
    std::cerr << "array1_1 = " << array1_0 << "\n";
    std::cerr << "array1_2 = " << array1_1 << "\n";
    std::cerr << "array1_3 = " << array1_2 << "\n";
    std::cerr << "array7_1 = " << array7_0 << "\n";
    std::cerr << "array7_2 = " << array7_1 << "\n";
    std::cerr << "array7_3 = " << array7_2 << "\n";
  }

  knowledge.clear_modifieds ();



  std::cerr << "************* INTEGERVECTOR3D: CREATING*************\n";
  containers::IntegerVector3D int_vector (
    "test_integervector2D", knowledge, {10,10,10});
  std::cerr << "  Checking size()\n";
  containers::IntegerVector3D::Dimensions isize = int_vector.size ();

  std::cerr << "  Setting indices\n";
  // set the vector entries
  int_vector.set ({1,0,0}, 10);
  int_vector.set ({1,1,1}, 7);
  int_vector.set ({1,2,2}, 6);
  int_vector.set ({7,0,0}, 8);
  int_vector.set ({7,1,1}, 3);
  int_vector.set ({7,2,2}, 214);

  // retrieve the indexes

  std::cerr << "  Retrieving indices\n";
  int64_t iarray1_0 = int_vector[{1,0,0}];
  int64_t iarray1_1 = int_vector[{1,1,1}];
  int64_t iarray1_2 = int_vector[{1,2,2}];
  int64_t iarray7_0 = int_vector[{7,0,0}];
  int64_t iarray7_1 = int_vector[{7,1,1}];
  int64_t iarray7_2 = int_vector[{7,2,2}];

  // check the iarray size

  std::cerr << "  Checking iarray size: [" <<
     isize.x << "," << isize.y << "," << isize.z <<"]: "<<
    (isize.x == 10 && isize.y == 10 && isize.z == 10 ? "SUCCESS\n" : "FAIL\n");

  // check the iarray values
  
  std::cerr << "  Checking all iarray indices for values: ";
  if (iarray1_0 == 10 && iarray1_1 == 7 && iarray1_2 == 6 &&
      iarray7_0 == 8 && iarray7_1 == 3 && iarray7_2 == 214)
  {
    std::cerr << "SUCCESS\n";
  }
  else
  {
    std::cerr << "FAIL\n"; ++madara_fails;
    std::cerr << "iarray1_1 = " << iarray1_0 << "\n";
    std::cerr << "iarray1_2 = " << iarray1_1 << "\n";
    std::cerr << "iarray1_3 = " << iarray1_2 << "\n";
    std::cerr << "iarray7_1 = " << iarray7_0 << "\n";
    std::cerr << "iarray7_2 = " << iarray7_1 << "\n";
    std::cerr << "iarray7_3 = " << iarray7_2 << "\n";
  }

  std::cerr << "  Checking resize()\n";
  int_vector.resize ({5,2,2});
  isize = int_vector.size ();

  iarray1_0 = int_vector[{1,0,0}];
  iarray1_1 = int_vector[{1,1,1}];
  iarray1_2 = int_vector[{1,2,2}];
  iarray7_0 = int_vector[{7,0,0}];
  iarray7_1 = int_vector[{7,1,1}];
  iarray7_2 = int_vector[{7,2,2}];
  
  std::cerr << "  Checking iarray size: [" <<
     isize.x << "," << isize.y << "," << isize.z <<"]: "<<
    (isize.x == 5 && isize.y == 2 && isize.z == 2 ? "SUCCESS\n" : "FAIL\n");

  std::cerr << "  Checking all iarray indices for values: ";
  if (iarray1_0 == 10 && iarray1_1 == 7 && iarray1_2 == 0 &&
      iarray7_0 == 0 && iarray7_1 == 0 && iarray7_2 == 0)
  {
    std::cerr << "SUCCESS\n";
  }
  else
  {
    std::cerr << "FAIL\n"; ++madara_fails;
    std::cerr << "iarray1_1 = " << iarray1_0 << "\n";
    std::cerr << "iarray1_2 = " << iarray1_1 << "\n";
    std::cerr << "iarray1_3 = " << iarray1_2 << "\n";
    std::cerr << "iarray7_1 = " << iarray7_0 << "\n";
    std::cerr << "iarray7_2 = " << iarray7_1 << "\n";
    std::cerr << "iarray7_3 = " << iarray7_2 << "\n";
  }

  std::cerr << "  Checking resize()\n";
  int_vector.resize ({8,3,4});
  isize = int_vector.size ();

  std::cerr << "  Setting indices\n";
  // set the vector entries
  int_vector.set ({1,0,0}, 10);
  int_vector.set ({1,1,1}, 7);
  int_vector.set ({1,2,2}, 6);
  int_vector.set ({7,0,0}, 8);
  int_vector.set ({7,1,1}, 3);
  int_vector.set ({7,2,2}, 214);

  //madara::logger::global_logger->set_level (madara::logger::LOG_MINOR);

  iarray1_0 = int_vector[{1,0,0}];
  iarray1_1 = int_vector[{1,1,1}];
  iarray1_2 = int_vector[{1,2,2}];
  iarray7_0 = int_vector[{7,0,0}];
  iarray7_1 = int_vector[{7,1,1}];
  iarray7_2 = int_vector[{7,2,2}];
  
  // check the iarray size

  std::cerr << "  Checking iarray size: [" <<
     isize.x << "," << isize.y << "," << isize.z <<"]: "<<
    (isize.x == 8 && isize.y == 3 && isize.z == 4 ? "SUCCESS\n" : "FAIL\n");

  // check the iarray values
  
  std::cerr << "  Checking all iarray indices for values: ";
  if (iarray1_0 == 10 && iarray1_1 == 7 && iarray1_2 == 6 &&
      iarray7_0 == 8 && iarray7_1 == 3 && iarray7_2 == 214)
  {
    std::cerr << "SUCCESS\n";
  }
  else
  {
    std::cerr << "FAIL\n"; ++madara_fails;
    std::cerr << "iarray1_1 = " << iarray1_0 << "\n";
    std::cerr << "iarray1_2 = " << iarray1_1 << "\n";
    std::cerr << "iarray1_3 = " << iarray1_2 << "\n";
    std::cerr << "iarray7_1 = " << iarray7_0 << "\n";
    std::cerr << "iarray7_2 = " << iarray7_1 << "\n";
    std::cerr << "iarray7_3 = " << iarray7_2 << "\n";
  }

  knowledge.clear_modifieds ();
}

void test_vector_vectors (void)
{
  std::cerr << "************* DOUBLEVECTORVECTOR: CREATING*************\n";
  knowledge::KnowledgeBase knowledge;
  containers::DoubleVectorVector double_vector (
    "test_vector_vector", knowledge, 10);
  size_t size = double_vector.size ();

  // set the vector entries
  double_vector.set (1, {10.22, 7.31, 6.179});
  double_vector.set (7, {8.12, 3.34, 214.555});

  // retrieve the indexes

  double array1_0 = double_vector[{1,0}];
  double array1_1 = double_vector[{1,1}];
  double array1_2 = double_vector[{1,2}];
  double array7_0 = double_vector[{7,0}];
  double array7_1 = double_vector[{7,1}];
  double array7_2 = double_vector[{7,2}];

  // check the array size

  std::cerr << "  Checking array size: " << size << ": " <<
    (size == 10 ? "SUCCESS\n" : "FAIL\n");

  // check the array values
  
  std::cerr << "  Checking all array indices for values: ";
  if (array1_0 == 10.22 && array1_1 == 7.31 && array1_2 == 6.179 &&
      array7_0 == 8.12 && array7_1 == 3.34 && array7_2 == 214.555)
  {
    std::cerr << "SUCCESS\n";
  }
  else
  {
    std::cerr << "FAIL\n"; ++madara_fails;
    std::cerr << "array1_1 = " << array1_0 << "\n";
    std::cerr << "array1_2 = " << array1_1 << "\n";
    std::cerr << "array1_3 = " << array1_2 << "\n";
    std::cerr << "array7_1 = " << array7_0 << "\n";
    std::cerr << "array7_2 = " << array7_1 << "\n";
    std::cerr << "array7_3 = " << array7_2 << "\n";
  }

  knowledge.clear_modifieds ();


  std::cerr << "************* INTEGERVECTORVECTOR: CREATING*************\n";
  containers::IntegerVectorVector integer_vector (
    "test_vector_vector", knowledge, 10);
  size = integer_vector.size ();

  // set the vector entries
  integer_vector.set (1, {10, 7, 6});
  integer_vector.set (7, {8, 3, 214});

  // retrieve the indexes

  int64_t int_array1_0 = integer_vector[{1,0}];
  int64_t int_array1_1 = integer_vector[{1,1}];
  int64_t int_array1_2 = integer_vector[{1,2}];
  int64_t int_array7_0 = integer_vector[{7,0}];
  int64_t int_array7_1 = integer_vector[{7,1}];
  int64_t int_array7_2 = integer_vector[{7,2}];

  // check the array size

  std::cerr << "  Checking array size: " << size << ": " <<
    (size == 10 ? "SUCCESS\n" : "FAIL\n");

  // check the array values
  
  std::cerr << "  Checking all array indices for values: ";
  if (int_array1_0 == 10 && int_array1_1 == 7 && int_array1_2 == 6 &&
      int_array7_0 == 8 && int_array7_1 == 3 && int_array7_2 == 214)
  {
    std::cerr << "SUCCESS\n";
  }
  else
  {
    std::cerr << "FAIL\n"; ++madara_fails;
    std::cerr << "array1_1 = " << int_array1_0 << "\n";
    std::cerr << "array1_2 = " << int_array1_1 << "\n";
    std::cerr << "array1_3 = " << int_array1_2 << "\n";
    std::cerr << "array7_1 = " << int_array7_0 << "\n";
    std::cerr << "array7_2 = " << int_array7_1 << "\n";
    std::cerr << "array7_3 = " << int_array7_2 << "\n";
  }

  knowledge.clear_modifieds ();
  
}

void test_vector (void)
{
  std::cerr << "************* VECTOR: CREATING VECTOR*************\n";
  knowledge::KnowledgeBase knowledge;
  containers::Vector vector ("test_vector", knowledge, 10);
  size_t size = vector.size ();

  vector.set (1, "value.at.1");
  vector.set (7, 7.7);

  knowledge.clear_modifieds ();
  
  std::cerr << "Vector contents (modified) after clear\n";
  

  vector.set (2, "value.at.2");
  


  std::cerr << "Vector results:\n";

  for (size_t i = 0; i < size; ++i)
  {
    std::cerr << "  ";
    std::cerr << vector[i];
    std::cerr << "\n";
  }
  
  std::cerr << "\n";

  if (size != 10)
    std::cerr << "FAIL. Vector.size != 10\n";
  else
    std::cerr << "SUCCESS. Vector.size == 10\n";

  if (vector[1] == "value.at.1" &&
      vector[7] == 7.7)
    std::cerr << "SUCCESS. Vector[1] and [7] were set and retrieved.\n";
  else
    std::cerr << "FAIL. Vector[1] and [7] were not set and retrieved.\n";
  
  if (vector.get_name () == "test_vector")
    std::cerr << "SUCCESS. vector.name () returned test_vector.\n";
  else
    std::cerr << "FAIL. vector.name () did not return test_vector.\n";

  std::cerr << "Resizing to 7 elements.\n";

  vector.resize (7);

  
  if (vector[1].to_string () == "value.at.1" &&
      knowledge.get ("test_vector.7").to_double () != 7.7 && vector.size () == 7)
    std::cerr << "SUCCESS. Vector [7] no longer exists.\n";
  else
    std::cerr << "FAIL. Vector [7] still exists after resize.\n";
  
  vector.resize (12);
  
  vector.set (8, "value.at.8");
  vector.set (9, "value.at.9");

  if (vector[1] == "value.at.1" &&
      vector[8] == "value.at.8" &&
      vector[9] == "value.at.9" && vector.size () == 12)
    std::cerr << "SUCCESS. Resize to 12 was successful.\n";
  else
    std::cerr << "FAIL. Resize to 12 was unsuccessful.\n";
  
  std::cerr << "Copying records from vector to a knowledge vector.\n";

  vector.set (11, "value.at.11");

  std::cerr << "Printing results from knowledge base.\n";
  knowledge.print ();

  KnowledgeVector records;
  vector.copy_to (records);

  std::cerr << "Printing resulting knowledge vector.\n";
  for (size_t i = 0; i < records.size (); ++i)
  {
    std::cerr << "  " << i << ": " << records[i] << "\n";
  }
  
  std::cerr << "Printing results from knowledge base.\n";
  knowledge.print ();

  std::cerr << "Resizing vector to zero elements.\n";
  vector.resize (0);

  std::cerr << "Printing results from knowledge base.\n";
  knowledge.print ();
}

void test_map (void)
{
  std::cerr << "************* MAP: SETTING KEY:VALUE PAIRS*************\n";
  knowledge::KnowledgeBase knowledge;
  std::vector <std::string> keys;
  containers::Map map ("test_map", knowledge);
  
  map.set ("name", "Rob Roy");
  map.set ("occupation", "Lord of the MacGregors");
  map.set ("age", madara::knowledge::KnowledgeRecord::Integer (63));
  map.set ("wives", madara::knowledge::KnowledgeRecord::Integer (1));

  size_t size = map.size ();
  map.keys (keys);
  
  std::cerr << "\nMap results:\n";

  for (std::vector <std::string>::iterator i = keys.begin ();
       i != keys.end (); ++i)
    std::cerr << "  " << *i << "=" << map[*i] << "\n";
  
  std::cerr << "\n";

  if (size != 4)
    std::cerr << "FAIL. Map.size != 4\n";
  else
    std::cerr << "SUCCESS. Map.size == 4\n";
  
  if (keys.size () != 4)
    std::cerr << "FAIL. Map.keys ().size != 4\n";
  else
    std::cerr << "SUCCESS. Map.keys ().size == 4\n";
  
  if (map["name"].to_string () == "Rob Roy" &&
      map["occupation"].to_string () == "Lord of the MacGregors" &&
      map["age"].to_integer () == 63 &&
      map["wives"].to_integer () == 1)
    std::cerr << "SUCCESS. map values were set and retrieved.\n";
  else
    std::cerr << "FAIL. map values were not set and retrieved.\n";
  
  std::cerr << "************* MAP: COPYING MAP FROM KB*************\n";
  containers::Map copy ("test_map", knowledge);
  
  size = map.size ();
  map.keys (keys);
  
  if (size != 4)
    std::cerr << "FAIL. Map Copy.size != 4\n";
  else
    std::cerr << "SUCCESS. Map Copy.size == 4\n";
  
  if (keys.size () != 4)
    std::cerr << "FAIL. Map Copy.keys ().size != 4\n";
  else
    std::cerr << "SUCCESS. Map Copy.keys ().size == 4\n";
  
  if (copy["name"].to_string () == "Rob Roy" &&
      copy["occupation"].to_string () == "Lord of the MacGregors" &&
      copy["age"].to_integer () == 63 &&
      copy["wives"].to_integer () == 1)
    std::cerr << "SUCCESS. map copy values were set and retrieved.\n";
  else
    std::cerr << "FAIL. map copy values were not set and retrieved.\n";

  
  std::cerr << "************* MAP: SYNCING KEYS*************\n";

  knowledge.set ("test_map.wife", "Mary Helen MacGregor");

  std::vector <std::string> new_keys (map.sync_keys ());

  if (map.exists ("wife") && !copy.exists ("wife") &&
      new_keys.size () == 1 && new_keys[0] == "wife")
    std::cerr << "SUCCESS. map.sync_keys () worked.\n";
  else
  {
    std::cerr << "FAIL. map.sync_keys () did not work.\n";
    std::cerr << "  new_keys[0] = " << new_keys[0] << ", should be 'wife'\n";
    std::cerr << "  new_keys.size() = " << new_keys.size () <<
      ", should be 1\n";
  }

  if (map.get_name () == "test_map" && copy.get_name () == "test_map")
    std::cerr << "SUCCESS. map.name () returned test_map.\n";
  else
    std::cerr << "FAIL. map.name () did not return test_map.\n";

  knowledge.print ();

  // test deleting map entries


  std::cerr << "************* MAP: CLEARING KEYS*************\n";

  knowledge::VariableReferences before_delete = knowledge.save_modifieds ();

  map.clear (true);

  knowledge::VariableReferences after_delete = knowledge.save_modifieds ();

  int size_diff = int (before_delete.size () - after_delete.size ());

  if (before_delete.size () > after_delete.size ())
  {
    std::cerr << "SUCCESS. map.clear () removed " << size_diff <<
      " elements from modifieds list.\n";
  }
  else
  {
    std::cerr << "FAIL. map.clear () has a non-positive size difference of "
      << size_diff << ".\n";
  }
}

void test_integer (void)
{
  std::cerr << "************* INTEGER: GETTING AND SETTING*************\n";
  knowledge::KnowledgeBase knowledge;
  containers::Integer my_int ("my_int", knowledge);

  std::cerr << "Integer value: " << *my_int << "\n";
  
  std::cerr << "Changing value...\n";

  my_int = 15;
  
  std::cerr << "Integer value: " << *my_int << "\n";
  
  if (my_int.get_name () == "my_int" && *my_int == 15)
    std::cerr << "SUCCESS. my_int was the correct name and value.\n";
  else
    std::cerr << "FAIL. my_int was not the correct name and value.\n";

  knowledge.print ();
}

void test_double (void)
{
  std::cerr << "************* DOUBLE: GETTING AND SETTING*************\n";
  knowledge::KnowledgeBase knowledge;
  containers::Double my_double ("my_double", knowledge);

  std::cerr << "Double value: " << *my_double << "\n";
  
  std::cerr << "Changing value...\n";

  my_double = 3.3;
  
  std::cerr << "Double value: " << *my_double << "\n";
  
  if (my_double.get_name () == "my_double" && *my_double == 3.3)
    std::cerr << "SUCCESS. my_double was the correct name and value.\n";
  else
    std::cerr << "FAIL. my_double was not the correct name and value.\n";

  knowledge.print ();
}

void test_map_exchanges (void)
{
  std::cerr << "************* EXCHANGES: MAPS*************\n";
  knowledge::KnowledgeBase knowledge;
  std::vector <std::string> keys;
  containers::Map map ("first_map", knowledge);
  containers::Map map2 ("second_map", knowledge);
  
  std::cerr << "Creating two maps...\n";
  map.set ("a", "key1 contents");
  map.set ("b", "key2 contents");
  map.set ("c", "key3 contents");

  map2.set ("d", "key4 contents");
  map2.set ("e", "key5 contents");
  map2.set ("f", "key6 contents");
  map2.set ("g", "key7 contents");
  
  std::cerr << "Contents of map before exchange...\n";
  knowledge.print ();
  
  std::cerr << "Exchanging...\n";
  map.exchange (map2);
  
  if (!map.exists ("a") && !map.exists ("b") && !map.exists ("c")
    && !map2.exists ("d") && !map2.exists ("e") && !map2.exists ("f")
    && !map2.exists ("g")
    && map["d"] == "key4 contents" && map["e"] == "key5 contents"
    && map["f"] == "key6 contents"
    && map2["a"] == "key1 contents" && map2["b"] == "key2 contents"
    && map2["c"] == "key3 contents")
  {
    std::cerr << "SUCCESS. maps were exchanged.\n";
  }
  else
  {
    std::cerr << "FAIL. maps were not exchanged properly.\n";
  }

  knowledge.print ();

  map.exchange (map2, false, false);

  if (map2.exists ("a") && map2.exists ("b") && map2.exists ("c")
    && map.exists ("d") && map.exists ("e") && map.exists ("f")
    && map.exists ("g")
    && map2["d"] == "key4 contents" && map2["e"] == "key5 contents"
    && map2["f"] == "key6 contents"
    && map["a"] == "key1 contents" && map["b"] == "key2 contents"
    && map["c"] == "key3 contents")
  {
    std::cerr << "SUCCESS. maps were exchanged.\n";
  }
  else
  {
    std::cerr << "FAIL. maps were not exchanged properly.\n";
  }
  
  knowledge.print ();
}

void test_vector_exchanges (void)
{
  std::cerr << "************* EXCHANGES: VECTORS*************\n";
  knowledge::KnowledgeBase knowledge;
  containers::Vector vector ("first_vector", knowledge, 12);
  containers::Vector vector2 ("second_vector", knowledge, 7);
  
  std::cerr << "Creating two vectors...\n";

  vector.set (1, "vector1.1 value");
  vector.set (5, "vector1.5 value");
  vector.set (10, "vector1.10 value");

  vector2.set (2, "vector2.2 value");
  vector2.set (3, "vector2.3 value");
  
  knowledge.print ();
  std::cerr << "Exchanging...\n";
  vector.exchange (vector2);
  
  if (vector.size () == 7 && vector2.size () == 12
    && vector[2] == "vector2.2 value"
    && vector[3] == "vector2.3 value"
    && vector2[1] == "vector1.1 value"
    && vector2[5] == "vector1.5 value"
    && vector2[10] == "vector1.10 value")
  {
    std::cerr << "SUCCESS. vectors were exchanged.\n";
  }
  else
  {
    std::cerr << "FAIL. vectors were not exchanged properly.\n";
  }
  
  std::cerr << "Exchanging...\n";
  vector.exchange (vector2);
  
  if (vector2.size () == 7 && vector.size () == 12
    && vector2[2] == "vector2.2 value"
    && vector2[3] == "vector2.3 value"
    && vector[1] == "vector1.1 value"
    && vector[5] == "vector1.5 value"
    && vector[10] == "vector1.10 value")
  {
    std::cerr << "SUCCESS. vectors were reverted.\n";
  }
  else
  {
    std::cerr << "FAIL. vectors were not reverted properly.\n";
  }
  knowledge.print ();
}

void test_vector_transfer ()
{
  std::cerr << "************* TRANSFERS: VECTORS*************\n";
  knowledge::KnowledgeBase knowledge;
  containers::Vector vector ("first_vector", knowledge, 12);
  containers::Vector vector2 ("second_vector", knowledge, 7);
  
  std::cerr << "Creating two vectors...\n";

  vector.set (1, "vector1.1 value");
  vector.set (5, "vector1.5 value");
  vector.set (10, "vector1.10 value");

  vector2.set (2, "vector2.2 value");
  vector2.set (3, "vector2.3 value");
  
  std::cerr << "Transferring one vector to another...\n";

  vector.transfer_to (vector2);

  knowledge.print ();
}

void test_integer_vector (void)
{
  std::cerr << "************* VECTOR: CREATING INTEGER_VECTOR*************\n";
  knowledge::KnowledgeBase knowledge;
  containers::IntegerVector vector ("test_vector", knowledge, 10);
  size_t size = vector.size ();

  vector.set (1);
  vector.set (2);
  vector.set (7);

  std::cerr << "Vector results:\n";

  for (size_t i = 0; i < size; ++i)
  {
    std::cerr << "  ";
    std::cerr << vector[i];
    std::cerr << "\n";
  }
  
  std::cerr << "\n";

  if (size != 10)
    std::cerr << "FAIL. Vector.size != 10\n";
  else
    std::cerr << "SUCCESS. Vector.size == 10\n";

  if (vector[0] == 0 && vector[1] == 1 && vector[2] == 1 &&
      vector[3] == 0 && vector[4] == 0 && vector[7] == 1)
    std::cerr << "SUCCESS. Vector[1],[2],[7] were set and retrieved.\n";
  else
    std::cerr << "FAIL. Vector[1],[2],[7] were not set and retrieved.\n";
  
  if (vector.get_name () == "test_vector")
    std::cerr << "SUCCESS. vector.name () returned test_vector.\n";
  else
    std::cerr << "FAIL. vector.name () did not return test_vector.\n";

  std::cerr << "Resizing to 7 elements.\n";

  vector.resize (7);

  
  if (vector[0] == 0 && vector[1] == 1 && vector[2] == 1 &&
      vector[3] == 0 && vector[4] == 0 && vector[7] == 0)
    std::cerr << "SUCCESS. Vector [7] no longer exists.\n";
  else
    std::cerr << "FAIL. Vector [7] still exists after resize.\n";
  
  vector.resize (12);
  
  vector.set (8);
  vector.set (9);

  if (vector[1] == 1 &&
      vector[8] == 1 && vector[10] == 0 &&
      vector[9] == 1 && vector.size () == 12)
    std::cerr << "SUCCESS. Resize to 12 was successful.\n";
  else
    std::cerr << "FAIL. Resize to 12 was unsuccessful.\n";

  knowledge.print ();
}

void test_double_vector (void)
{
  std::cerr << "************* VECTOR: CREATING DOUBLE_VECTOR*************\n";
  knowledge::KnowledgeBase knowledge;
  containers::DoubleVector vector ("test_vector", knowledge, 10);
  size_t size = vector.size ();

  vector.set (1, 1.1);
  vector.set (2, 2.2);
  vector.set (7, 7.7);

  std::cerr << "Vector results:\n";

  for (size_t i = 0; i < size; ++i)
  {
    std::cerr << "  ";
    std::cerr << vector[i];
    std::cerr << "\n";
  }
  
  std::cerr << "\n";

  if (size != 10)
    std::cerr << "FAIL. Vector.size != 10\n";
  else
    std::cerr << "SUCCESS. Vector.size == 10\n";

  if (vector[0] == 0 && vector[1] == 1.1 && vector[2] == 2.2 &&
      vector[3] == 0 && vector[4] == 0 && vector[7] == 7.7)
    std::cerr << "SUCCESS. Vector[1],[2],[7] were set and retrieved.\n";
  else
    std::cerr << "FAIL. Vector[1],[2],[7] were not set and retrieved.\n";
  
  if (vector.get_name () == "test_vector")
    std::cerr << "SUCCESS. vector.name () returned test_vector.\n";
  else
    std::cerr << "FAIL. vector.name () did not return test_vector.\n";

  std::cerr << "Resizing to 7 elements.\n";

  vector.resize (7);

  
  if (vector[0] == 0 && vector[1] == 1.1 && vector[2] == 2.2 &&
      vector[3] == 0 && vector[4] == 0 && vector[7] == 0)
    std::cerr << "SUCCESS. Vector [7] no longer exists.\n";
  else
    std::cerr << "FAIL. Vector [7] still exists after resize.\n";
  
  vector.resize (12);
  
  vector.set (8, 8.8);
  vector.set (9, 9.9);

  if (vector[1] == 1.1 &&
      vector[8] == 8.8 && vector[10] == 0 &&
      vector[9] == 9.9 && vector.size () == 12)
    std::cerr << "SUCCESS. Resize to 12 was successful.\n";
  else
    std::cerr << "FAIL. Resize to 12 was unsuccessful.\n";

  knowledge.print ();
}

void test_string_vector (void)
{
  std::cerr << "************* VECTOR: CREATING STRING_VECTOR*************\n";
  knowledge::KnowledgeBase knowledge;
  containers::StringVector vector ("test_vector", knowledge, 10);
  size_t size = vector.size ();

  vector.set (1, "1.1");
  vector.set (2, "2.2");
  vector.set (7, "7.7");

  std::cerr << "Vector results:\n";

  for (size_t i = 0; i < size; ++i)
  {
    std::cerr << "  ";
    std::cerr << vector[i];
    std::cerr << "\n";
  }
  
  std::cerr << "\n";

  if (size != 10)
    std::cerr << "FAIL. Vector.size != 10\n";
  else
    std::cerr << "SUCCESS. Vector.size == 10\n";

  if (vector[0] == "0" && vector[1] == "1.1" && vector[2] == "2.2" &&
      vector[3] == "0" && vector[4] == "0" && vector[7] == "7.7")
    std::cerr << "SUCCESS. Vector[1],[2],[7] were set and retrieved.\n";
  else
    std::cerr << "FAIL. Vector[1],[2],[7] were not set and retrieved.\n";
  
  if (vector.get_name () == "test_vector")
    std::cerr << "SUCCESS. vector.name () returned test_vector.\n";
  else
    std::cerr << "FAIL. vector.name () did not return test_vector.\n";

  std::cerr << "Resizing to 7 elements.\n";

  vector.resize (7);

  
  if (vector[0] == "0" && vector[1] == "1.1" && vector[2] == "2.2" &&
      vector[3] == "0" && vector[4] == "0" && vector[7] == "0")
    std::cerr << "SUCCESS. Vector [7] no longer exists.\n";
  else
    std::cerr << "FAIL. Vector [7] still exists after resize.\n";
  
  vector.resize (12);
  
  vector.set (8, "8.8");
  vector.set (9, "9.9");

  if (vector[1] == "1.1" &&
      vector[8] == "8.8" && vector[10] == "0" &&
      vector[9] == "9.9" && vector.size () == 12)
    std::cerr << "SUCCESS. Resize to 12 was successful.\n";
  else
    std::cerr << "FAIL. Resize to 12 was unsuccessful.\n";

  knowledge.print ();
}

void test_native_vectors (void)
{
  std::cerr <<
    "************* VECTOR: CREATING NATIVE DOUBLE VECTOR*************\n";
  madara::knowledge::KnowledgeBase knowledge;
  containers::NativeDoubleVector vector ("test_vector", knowledge, 10);
  size_t size = vector.size ();

  vector.set (1, 1.1);
  vector.set (2, 2.2);
  vector.set (7, 7.7);

  std::cerr << "Vector results:\n";

  for (size_t i = 0; i < size; ++i)
  {
    std::cerr << "  ";
    std::cerr << vector[i];
    std::cerr << "\n";
  }
  
  std::cerr << "\n";

  if (size != 10) {
    std::cerr << "FAIL. Vector.size != 10\n"; ++madara_fails;
  } else
    std::cerr << "SUCCESS. Vector.size == 10\n";

  if (vector[0] == 0 && vector[1] == 1.1 && vector[2] == 2.2 &&
      vector[3] == 0 && vector[4] == 0 && vector[7] == 7.7)
    std::cerr << "SUCCESS. Vector[1],[2],[7] were set and retrieved.\n";
  else {
    std::cerr << "FAIL. Vector[1],[2],[7] were not set and retrieved.\n";
    ++madara_fails;
  }
  
  if (vector.get_name () == "test_vector")
    std::cerr << "SUCCESS. vector.name () returned test_vector.\n";
  else {
    std::cerr << "FAIL. vector.name () did not return test_vector.\n";
    ++madara_fails;
  }

  std::cerr << "Resizing to 7 elements.\n";

  vector.resize (7);

  
  if (vector[0] == 0 && vector[1] == 1.1 && vector[2] == 2.2 &&
      vector[3] == 0 && vector[4] == 0 && vector[7] == 0)
    std::cerr << "SUCCESS. Vector [7] no longer exists.\n";
  else {
    std::cerr << "FAIL. Vector [7] still exists after resize.\n";
    ++madara_fails;
  }
  
  vector.resize (12);
  
  vector.set (8, 8.8);
  vector.set (9, 9.9);

  if (vector[1] == 1.1 &&
      vector[8] == 8.8 && vector[10] == 0 &&
      vector[9] == 9.9 && vector.size () == 12)
    std::cerr << "SUCCESS. Resize to 12 was successful.\n";
  else {
    std::cerr << "FAIL. Resize to 12 was unsuccessful.\n";
    ++madara_fails;
  }

  std::vector<double> vals = {4, 8, 15, 16, 23, 42};
  vector.set (vals.data(), (uint32_t) vals.size());

  if (vector.size () == 6 && vector[0] == 4 &&
      vector[1] == 8 && vector[2] == 15 &&
      vector[3] == 16 && vector[4] == 23 && vector[5] == 42)
    std::cerr << "SUCCESS. Set from C array succeeded.\n";
  else {
    std::cerr << "FAIL. Set from C array failed.\n";
    ++madara_fails;
  }

  knowledge.print ();
}

void test_queue (void)
{
  std::cerr <<
    "************* QUEUES: Testing multithreaded queues*************\n";
  knowledge::KnowledgeBase knowledge;
  containers::Queue messages ("queue", knowledge, 7);
  madara::knowledge::KnowledgeRecord record;
  bool check;

  messages.emplace ("first string");
  messages.emplace ("second string");
  messages.emplace ("third string");
  messages.emplace ("fourth string");
  messages.emplace ("fifth string");
  messages.emplace ("sixth string");
  messages.emplace ("seventh string");
  check = messages.emplace ("eighth string");

  if (!check)
    std::cerr << "  SUCCESS: enqueue size check.\n";
  else
    std::cerr << "  FAIL: enqueue size check.\n";

  record = messages.dequeue ();
  if (record == "first string")
    std::cerr << "  SUCCESS: first dequeue.\n";
  else
    std::cerr << "  FAIL: first dequeue.\n";
  
  record = messages.dequeue ();
  if (record == "second string")
    std::cerr << "  SUCCESS: second dequeue.\n";
  else
    std::cerr << "  FAIL: second dequeue.\n";

  messages.emplace ("eighth string");
  messages.emplace ("ninth string");
  check = messages.emplace ("tenth string");
  
  if (!check)
    std::cerr << "  SUCCESS: 2nd enqueue size check.\n";
  else
    std::cerr << "  FAIL: 2nd enqueue size check.\n";

  record = messages.dequeue ();
  if (record == "third string")
    std::cerr << "  SUCCESS: third dequeue.\n";
  else
    std::cerr << "  FAIL: third dequeue.\n";
  
  record = messages.dequeue ();
  if (record == "fourth string")
    std::cerr << "  SUCCESS: fourth dequeue.\n";
  else
    std::cerr << "  FAIL: fourth dequeue.\n";
  
  record = messages.dequeue ();
  if (record == "fifth string")
    std::cerr << "  SUCCESS: fifth dequeue.\n";
  else
    std::cerr << "  FAIL: fifth dequeue.\n";
  
  record = messages.dequeue ();
  if (record == "sixth string")
    std::cerr << "  SUCCESS: sixth dequeue.\n";
  else
    std::cerr << "  FAIL: sixth dequeue.\n";
  
  record = messages.dequeue ();
  if (record == "seventh string")
    std::cerr << "  SUCCESS: seventh dequeue.\n";
  else
    std::cerr << "  FAIL: seventh dequeue.\n";
  
  record = messages.dequeue ();
  if (record == "eighth string")
    std::cerr << "  SUCCESS: eighth dequeue.\n";
  else
    std::cerr << "  FAIL: eighth dequeue.\n";
  
  record = messages.dequeue ();
  if (record == "ninth string")
    std::cerr << "  SUCCESS: ninth dequeue.\n";
  else
    std::cerr << "  FAIL: ninth dequeue.\n";
  
  messages.emplace ("first cleared");
  messages.emplace ("second cleared");

  if (messages.inspect (0) == "first cleared" &&
      messages.inspect (1) == "second cleared")
  {
    std::cerr << "  SUCCESS: inspect on multiple records.\n";
  }
  else
  {
    std::cerr << "  FAIL: inspect on multiple records.\n";
  }

  messages.clear ();

  if (messages.count () == 0 && messages.size () == 7)
    std::cerr << "  SUCCESS: clear.\n";
  else
    std::cerr << "  FAIL: clear.\n";
  
  
  if (!messages.dequeue (false).exists ())
  {
    std::cerr << "  SUCCESS: asynchronous dequeue empty.\n";
  }
  else
  {
    std::cerr << "  FAIL: asynchronous dequeue empty.\n";
  }

  messages.emplace ("first string");
  messages.emplace ("second string");
  messages.emplace ("third string");
  messages.emplace ("fourth string");
  messages.emplace ("fifth string");
  messages.emplace ("sixth string");
  messages.emplace ("seventh string");

  messages.resize (5);

  if (messages.count () == 5 && 
    messages.count () == messages.size ())
  {
    std::cerr << "  SUCCESS: resize.\n";
  }
  else
  {
    std::cerr << "  FAIL: resize.\n";
  }

  knowledge.print ();
}

void test_collection (void)
{
  std::cerr <<
    "************* COLLECTION: Testing container collections*************\n";

  knowledge::KnowledgeBase knowledge;
  knowledge::ThreadSafeContext & context (knowledge.get_context ());

  unsigned char buffer1[1024];
  unsigned char buffer2[2048];

  std::cerr << "  Creating containers...\n";

  containers::Integer age ("age", knowledge);
  containers::String name ("name", knowledge);
  containers::Double salary ("salary", knowledge);
  containers::NativeDoubleVector gps_location ("location.gps", knowledge, 3);
  containers::NativeIntegerVector years_employed ("employment.years", knowledge);
  containers::Map jobs ("jobs", knowledge);
  containers::BufferVector images ("images", knowledge);
  containers::StringVector movies ("movies.favorite", knowledge);
  containers::IntegerVector years ("years.favorite", knowledge);
  containers::IntegerVector sensors_ready ("sensors_ready", knowledge, 3);
  containers::DoubleVector coolfactor ("coolfactor.by.year", knowledge, 35);

  std::cerr << "  Testing modifieds.size == 3 after container creation... ";

  if (context.get_modifieds ().size () != 3)
  {
    std::cerr << "FAIL\n"; ++madara_fails;
    std::cerr << "    Printing modified elements in context\n\n";
    std::cerr << context.debug_modifieds () << "\n";
  }
  else
  {
    std::cerr << "SUCCESS\n";
  }

  std::cerr << "  Assigning values to containers...\n";

  age = 40;
  name = "Jack Franklin";
  salary = 45000.0;
  gps_location.set (2, 1000.0);
  gps_location.set (0, 72.0);
  gps_location.set (1, 40.0);
  years_employed.set (0, 1998);
  years_employed.push_back (1999);
  years_employed.push_back (2005);
  years_employed.push_back (2007);
  years_employed.push_back (2010);
  years_employed.push_back (2011);
  years_employed.push_back (2012);
  years_employed.push_back (2013);
  years_employed.push_back (2014);
  years_employed.push_back (2015);
  jobs.set ("Albert's", "Courtesy Clerk");
  jobs.set ("Nursery House", "Plant Care Technician");
  jobs.set ("Johnson's", "Landscaping Expert");
  images.push_back (buffer1, sizeof (buffer1));
  images.push_back (buffer2, sizeof (buffer2));
  movies.push_back ("Edge of Tomorrow");
  movies.push_back ("Fight Club");
  movies.push_back ("Seven");
  movies.push_back ("Serenity");
  years.push_back (2000);
  years.push_back (2012);
  coolfactor.set (0, 10.0);
  coolfactor.set (1, 12.0);
  coolfactor.set (2, 10.5);
  coolfactor.set (3, 9);
  coolfactor.set (4, 8);
  coolfactor.set (5, 8.5);
  coolfactor.set (6, 8.5);
  coolfactor.set (7, 8.5);
  coolfactor.set (8, 8);
  coolfactor.set (9, 9);
  coolfactor.set (10, 10);
  coolfactor.set (11, 10);
  coolfactor.set (12, 11);
  coolfactor.set (13, 11);
  coolfactor.set (14, 6);
  coolfactor.set (15, 7);
  coolfactor.set (16, 20);
  coolfactor.set (17, 30);
  coolfactor.set (18, 35);
  coolfactor.set (19, 25);
  coolfactor.set (20, 20);
  coolfactor.set (21, 35);
  coolfactor.set (22, 30);
  coolfactor.set (23, 22);
  coolfactor.set (24, 18);
  coolfactor.set (25, 14);
  coolfactor.set (26, 11);
  coolfactor.set (27, 10);
  coolfactor.set (28, 9);
  coolfactor.set (29, 9);
  coolfactor.set (30, 5);
  coolfactor.set (31, 5);
  coolfactor.set (32, 4);
  coolfactor.set (33, 3);
  coolfactor.set (34, 3);
  sensors_ready.set (0, 1);
  sensors_ready.set (2, 1);

  containers::Collection collection;

  std::cerr << "\n  Adding 10 containers to collection container\n";

  collection.add (age);
  collection.add (name);
  collection.add (salary);
  collection.add (gps_location);
  collection.add (years_employed);
  collection.add (jobs);
  collection.add (images);
  collection.add (movies);
  collection.add (years);
  collection.add (coolfactor);

  std::cerr << "  Testing collection.size == 10 after adding containers... ";

  if (collection.size () != 10)
  {
    std::cerr << "FAIL. Size returned " << collection.size () << "\n";
  }
  else
  {
    std::cerr << "SUCCESS\n";
  }

  std::cerr << "\n  Printing Collection contents\n\n";
  std::cerr << collection.get_debug_info () << "\n";

  std::cerr << "  Printing modified elements in context\n\n";
  std::cerr << context.debug_modifieds () << "\n";

  std::cerr << "  Clearing modified elements in context\n\n";
  knowledge.clear_modifieds ();

  std::cerr << "  Testing modifieds.size == 0 after clearing modified... ";

  if (context.get_modifieds ().size () != 0)
  {
    std::cerr << "FAIL\n"; ++madara_fails;
    std::cerr << "    Printing modified elements in context\n\n";
    std::cerr << context.debug_modifieds () << "\n";
  }
  else
  {
    std::cerr << "SUCCESS\n";
  }

  std::cerr << "  Calling modify on collection\n";
  collection.modify ();

  std::cerr << "  Testing modifieds.size == 55 after modifying containers... ";

  if (context.get_modifieds ().size () != 55)
  {
    std::cerr << "FAIL\n"; ++madara_fails;
  }
  else
  {
    std::cerr << "SUCCESS\n";
  }

  std::cerr << "  Printing modified elements in context\n\n";
  std::cerr << context.debug_modifieds () << "\n";

  std::cerr << "  Clearing modified elements in context\n\n";
  knowledge.clear_modifieds ();

  std::cerr << "  Clearing collection\n\n";
  collection.clear ();

  std::cerr << "  Testing collection.size == 0 after clearing containers... ";

  if (collection.size () != 0)
  {
    std::cerr << "FAIL. Size returned " << collection.size () << "\n";
  }
  else
  {
    std::cerr << "SUCCESS\n";
  }

  std::cerr << "\nAdding 3 containers to collection container\n";

  collection.add (age);
  collection.add (name);
  collection.add (salary);

  std::cerr << "  Testing collection.size == 3 after adding containers... ";

  if (collection.size () != 3)
  {
    std::cerr << "FAIL. Size returned " << collection.size () << "\n";
  }
  else
  {
    std::cerr << "SUCCESS\n";
  }

  std::cerr << "\nPrinting Collection contents\n\n";
  std::cerr << collection.get_debug_info () << "\n";

  std::cerr << "  Calling modify on collection\n";
  collection.modify ();

  std::cerr << "  Testing modifieds.size == 3... ";

  if (context.get_modifieds ().size () != 3)
  {
    std::cerr << "FAIL\n"; ++madara_fails;
  }
  else
  {
    std::cerr << "SUCCESS\n";
  }

  std::cerr << "  Printing modified elements in context\n\n";
  std::cerr << context.debug_modifieds () << "\n";

  std::cerr << "  Clearing modified elements in context\n\n";
  knowledge.clear_modifieds ();


  std::cerr << "  Calling modify_if_true on collection\n";
  collection.modify_if_true (sensors_ready);

  std::cerr << "  Testing modifieds.size == 0... ";

  if (context.get_modifieds ().size () != 0)
  {
    std::cerr << "FAIL\n"; ++madara_fails;
  }
  else
  {
    std::cerr << "SUCCESS\n";
  }

  std::cerr << "  Updating final sensors ready to true\n";
  sensors_ready.set (1, 1);

  std::cerr << "  Calling modify_if_true on collection\n";
  collection.modify_if_true (sensors_ready);

  std::cerr << "  Testing modifieds.size == 4... ";

  if (context.get_modifieds ().size () != 4)
  {
    std::cerr << "FAIL\n"; ++madara_fails;
    std::cerr << "    Printing modified elements in context\n\n";
    std::cerr << context.debug_modifieds () << "\n";
  }
  else
  {
    std::cerr << "SUCCESS\n";
  }

}

void test_circular (void)
{
  std::cerr <<
    "************* CIRCULARBUFFER: Testing CircularBuffer*************\n";

  knowledge::KnowledgeBase kb;

  containers::CircularBuffer buffer1 ("buffer1", kb);
  containers::CircularBuffer buffer2 ("buffer2", kb, 3);

  buffer1.resize (3);

  buffer1.add (KnowledgeRecord (1));
  buffer1.add (KnowledgeRecord (2.0));

  std::cerr << "  Testing count (2) vs size (3)... ";

  if (buffer1.size () == 3 && buffer1.count () == 2)
  {
    std::cerr << "SUCCESS\n";
  }
  else
  {
    std::cerr << "FAIL\n";
    ++madara_fails;

    std::cerr << "    count () == " << buffer1.count () << ", ";
    std::cerr << "size () == " << buffer1.size () << "\n";
  }  


  buffer1.add (KnowledgeRecord ("value3"));

  buffer2.add (KnowledgeRecord ("value1"));
  buffer2.add (KnowledgeRecord (2));
  buffer2.add (KnowledgeRecord (3.0));

  std::vector <KnowledgeRecord> buffer1_contents = buffer1.get_latest (3);
  std::vector <KnowledgeRecord> buffer1_earliest = buffer1.get_earliest (3);
  std::vector <KnowledgeRecord> buffer2_contents = buffer2.get_latest (3);


  std::cerr << "  Testing addx3 on buffer1... ";

  if (buffer1_contents.size () == 3 &&
    buffer1_contents[2] == 1 &&
    buffer1_contents[1] == 2.0 &&
    buffer1_contents[0] == "value3")
  {
    std::cerr << "SUCCESS\n";
  }
  else
  {
    std::cerr << "FAIL\n";
    ++madara_fails;

    std::cerr << "  Contents of std::vector<KnowledgeRecord> were:\n";
    for (auto record: buffer1_contents)
    {
      std::cerr << "    " << record << "\n";
    }
  }  

  std::cerr << "  Testing addx3 on buffer2... ";

  if (buffer2_contents.size () == 3 &&
    buffer2_contents[2] == "value1" &&
    buffer2_contents[1] == 2 &&
    buffer2_contents[0] == 3.0)
  {
    std::cerr << "SUCCESS\n";
  }
  else
  {
    std::cerr << "FAIL\n";
    ++madara_fails;

    std::cerr << "  Contents of std::vector<KnowledgeRecord> were:\n";
    for (auto record: buffer2_contents)
    {
      std::cerr << "    " << record << "\n";
    }
  }

  std::cerr << "  Testing get_earliest ()...";

  if (buffer1_earliest.size () == 3 &&
    buffer1_earliest[0] == 1 &&
    buffer1_earliest[1] == 2.0 &&
    buffer1_earliest[2] == "value3")
  {
    std::cerr << "SUCCESS\n";
  }
  else
  {
    std::cerr << "FAIL\n";
    ++madara_fails;

    std::cerr << "Contents of earliest std::vector<KnowledgeRecord> were:\n";
    for (auto record: buffer1_earliest)
    {
      std::cerr << "    " << record << "\n";
    }
  }

  std::cerr << "  Attempting to get more elements than exist in buffer1... ";

  buffer1_contents = buffer1.get_latest (5);

  if (buffer1_contents.size () == 3 &&
    buffer1_contents[2] == 1 &&
    buffer1_contents[1] == 2.0 &&
    buffer1_contents[0] == "value3")
  {
    std::cerr << "SUCCESS\n";
  }
  else
  {
    std::cerr << "FAIL\n";
    ++madara_fails;

    std::cerr << "  Contents of std::vector<KnowledgeRecord> were:\n";
    for (auto record: buffer1_contents)
    {
      std::cerr << "    " << record << "\n";
    }
  }  

  std::cerr << "  Testing adding past size boundary... ";

  buffer1.add (KnowledgeRecord (5));
  buffer1.add (KnowledgeRecord (5.5));

  buffer1_contents = buffer1.get_latest (4);

  if (buffer1_contents.size () == 3 &&
    buffer1_contents[0] == 5.5 &&
    buffer1_contents[1] == 5 &&
    buffer1_contents[2] == "value3")
  {
    std::cerr << "SUCCESS\n";
  }
  else
  {
    std::cerr << "FAIL\n";
    ++madara_fails;

    std::cerr << "  Contents of std::vector<KnowledgeRecord> were:\n";
    for (auto record: buffer1_contents)
    {
      std::cerr << "    " << record << "\n";
    }
  }

  std::cerr << "  Testing get_earliest()... ";

  buffer1_earliest = buffer1.get_earliest (2);

  if (buffer1_earliest.size () == 2 &&
    buffer1_earliest[1] == 5 &&
    buffer1_earliest[0] == "value3")
  {
    std::cerr << "SUCCESS\n";
  }
  else
  {
    std::cerr << "FAIL\n";
    ++madara_fails;

    std::cerr << "  Contents of std::vector<KnowledgeRecord> were:\n";
    for (auto record: buffer1_earliest)
    {
      std::cerr << "    " << record << "\n";
    }
  }

  std::cerr << "  Testing count (3) vs size (3)... ";

  if (buffer1.size () == 3 && buffer1.count () == 3)
  {
    std::cerr << "SUCCESS\n";
  }
  else
  {
    std::cerr << "FAIL\n";
    ++madara_fails;

    std::cerr << "    count () == " << buffer1.count () << ", ";
    std::cerr << "size () == " << buffer1.size () << "\n";
  }

  buffer1.resize (5);
  buffer1.set_index (-1);

  std::cerr << "  Testing count (0) vs size (5)... ";

  if (buffer1.size () == 5 && buffer1.count () == 0)
  {
    std::cerr << "SUCCESS\n";
  }
  else
  {
    std::cerr << "FAIL\n";
    ++madara_fails;

    std::cerr << "    count () == " << buffer1.count () << ", ";
    std::cerr << "size () == " << buffer1.size () << "\n";
  }

  buffer1.add (KnowledgeRecord (1));
  buffer1.add (KnowledgeRecord (2));
  buffer1.add (KnowledgeRecord (3));
  buffer1.add (KnowledgeRecord (4));
  buffer1.add (KnowledgeRecord (5));

  std::cerr << "  Testing get()... ";

  if (buffer1.get () == 5)
  {
    std::cerr << "SUCCESS\n";
  }
  else
  {
    std::cerr << "FAIL\n";
    ++madara_fails;

    std::cerr << "    get () == " << buffer1.get () << "\n";
  }


  buffer1.add (KnowledgeRecord (1));
  buffer1.add (KnowledgeRecord (2));

  std::cerr << "  Testing get_latest (3)... ";

  buffer1_contents = buffer1.get_latest (3);

  if (buffer1_contents.size () == 3 &&
    buffer1_contents[0] == 2 &&
    buffer1_contents[1] == 1 &&
    buffer1_contents[2] == 5)
  {
    std::cerr << "SUCCESS\n";
  }
  else
  {
    std::cerr << "FAIL\n";
    ++madara_fails;

    std::cerr << "  Contents of std::vector<KnowledgeRecord> were:\n";
    for (auto record: buffer1_contents)
    {
      std::cerr << "    " << record << "\n";
    }
  }

  std::cerr << "  Testing get_latest (4)... ";

  buffer1_contents = buffer1.get_latest (4);

  if (buffer1_contents.size () == 4 &&
    buffer1_contents[0] == 2 &&
    buffer1_contents[1] == 1 &&
    buffer1_contents[2] == 5 &&
    buffer1_contents[3] == 4)
  {
    std::cerr << "SUCCESS\n";
  }
  else
  {
    std::cerr << "FAIL\n";
    ++madara_fails;

    std::cerr << "  Contents of std::vector<KnowledgeRecord> were:\n";
    for (auto record: buffer1_contents)
    {
      std::cerr << "    " << record << "\n";
    }
  }

  std::cerr << "  Testing clear ()... ";

  buffer1.clear ();

  if (buffer1.size () == 0 && buffer1.count () == 0)
  {
    std::cerr << "SUCCESS\n";
  }
  else
  {
    std::cerr << "FAIL\n";
    ++madara_fails;

    std::cerr << "    count () == " << buffer1.count () << ", ";
    std::cerr << "size () == " << buffer1.size () << "\n";
  }

  std::cerr << "  Testing resize(5)... ";
  buffer1.resize (5);

  std::cerr << "  Testing add(vector)... ";

  // add contents back but in a reverse order of the 4 elements
  buffer1.add (buffer1_contents);

  std::cerr << "  Testing get_latest(5)... ";

  buffer1_contents = buffer1.get_latest (5);

  if (buffer1_contents.size () == 4 &&
    buffer1_contents[3] == 2 &&
    buffer1_contents[2] == 1 &&
    buffer1_contents[1] == 5 &&
    buffer1_contents[0] == 4)
  {
    std::cerr << "SUCCESS\n";
  }
  else
  {
    std::cerr << "FAIL\n";
    ++madara_fails;

    std::cerr << "  Contents of std::vector<KnowledgeRecord> were:\n";
    for (auto record: buffer1_contents)
    {
      std::cerr << "    " << record << "\n";
    }
  }

  std::cerr << "  Testing inspect... ";

  if (buffer1.inspect (0) == 4 &&
      buffer1.inspect (-1) == 5 &&
      buffer1.inspect (-2) == 1 &&
      buffer1.inspect (-3) == 2)
  {
    std::cerr << "SUCCESS\n";
  }
  else
  {
    std::cerr << "FAIL\n";
    ++madara_fails;

    std::cerr << "  Contents of std::vector<KnowledgeRecord> were:\n";
    for (KnowledgeRecord::Integer i = 0; i < 4; ++i)
    {
      std::cerr << "    " << buffer1.inspect(-i) << "\n";
    }
  }


  std::cerr << "  Testing inspect(2,5)...";

  std::vector <KnowledgeRecord> records = buffer1.inspect (-3, 4);

  if (records.size () == 4 &&
      records[0] == 2 &&
      records[1] == 1 &&
      records[2] == 5 &&
      records[3] == 4)
  {
    std::cerr << "SUCCESS\n";
  }
  else
  {
    std::cerr << "FAIL\n";
    ++madara_fails;

    std::cerr << "    size=" << records.size () << "\n";
    for (auto record : records)
    {
      std::cerr << "      " << record << "\n";
    }
  }

  std::cerr << "  Testing exception in inspect...";
  
  try
  {
    buffer1.inspect (1);
    std::cerr << "FAIL\n";
    ++madara_fails;
  }
  catch (madara::exceptions::IndexException &)
  {
    std::cerr << "SUCCESS\n";
  }

  std::cerr << "  Testing exception in constructors...";
  
  try
  {
    containers::CircularBuffer buffer3 ("", kb);
    std::cerr << "FAIL\n";
    ++madara_fails;
  }
  catch (madara::exceptions::NameException &)
  {
    std::cerr << "SUCCESS\n";
  }

  std::cerr << "  Testing const CircularBuffer creation...";

  // for testing a pure consumer of the CircularBuffer
  const containers::CircularBuffer const_buffer1 ("buffer1", kb);

  buffer1_contents = const_buffer1.get_latest (5);

  if (buffer1_contents.size () == 4 &&
    buffer1_contents[3] == 2 &&
    buffer1_contents[2] == 1 &&
    buffer1_contents[1] == 5 &&
    buffer1_contents[0] == 4)
  {
    std::cerr << "SUCCESS\n";
  }
  else
  {
    std::cerr << "FAIL\n";
    ++madara_fails;

    std::cerr << "  Contents of std::vector<KnowledgeRecord> were:\n";
    for (auto record: buffer1_contents)
    {
      std::cerr << "    " << record << "\n";
    }
  }
}

void test_circular_consumer (void)
{
  std::cerr <<
    "************* CIRCULARBUFFER: Testing CircularBufferConsumer*************\n";

  knowledge::KnowledgeBase kb;
  containers::CircularBuffer producer ("buffer", kb, 100);
  containers::CircularBufferConsumer consumer ("buffer", kb);

  for (KnowledgeRecord::Integer i = 0; i < 100; ++i)
  {
    producer.add (KnowledgeRecord (i));
  }


  std::cerr << "  Testing get_earliest(50)...";

  std::vector <KnowledgeRecord> records = consumer.consume_earliest (50);

  bool has_failed = records.size () != 50;

  for (KnowledgeRecord::Integer i = 0;
       !has_failed && i < (KnowledgeRecord::Integer)records.size (); ++i)
  {
    if (records[i] != i)
    {
      has_failed = true;
      std::cerr << " Fail: records[" << i << "]=" << records[i] << "...";
    }
  }

  if (!has_failed)
  {
    std::cerr << "SUCCESS\n";
  }
  else
  {
    std::cerr << "FAIL\n";
    ++madara_fails;
    
    std::cerr << "    size=" << records.size () << "\n";
    for (auto record : records)
    {
      std::cerr << "      " << record << "\n";
    }
  }


  std::cerr << "  Testing remaining()...";

  if (consumer.remaining () == 50)
  {
    std::cerr << "SUCCESS\n";
  }
  else
  {
    std::cerr << "FAIL\n";
    ++madara_fails;
    std::cerr << "      remaining() == " << consumer.remaining () << "\n";
  }

  std::cerr << "  Testing get_earliest(25)...";

  records = consumer.consume_earliest (25);

  has_failed = records.size () != 25;

  for (KnowledgeRecord::Integer i = 0;
       !has_failed && i < (KnowledgeRecord::Integer)records.size (); ++i)
  {
    if (records[i] != i + 50)
    {
      has_failed = true;
      std::cerr << " Fail: records[" << i << "]=" << records[i] << "...";
    }
  }

  if (!has_failed)
  {
    std::cerr << "SUCCESS\n";
  }
  else
  {
    std::cerr << "FAIL\n";
    ++madara_fails;
    
    std::cerr << "    size=" << records.size () << "\n";
    for (auto record : records)
    {
      std::cerr << "      " << record << "\n";
    }
  }

  std::cerr << "  Testing remaining()...";

  if (consumer.remaining () == 25)
  {
    std::cerr << "SUCCESS\n";
  }
  else
  {
    std::cerr << "FAIL\n";
    ++madara_fails;
    std::cerr << "      remaining() == " << consumer.remaining () << "\n";
  }

  std::cerr << "  Testing get_earliest(25)...";

  records = consumer.consume_earliest (25);

  has_failed = records.size () != 25;

  for (KnowledgeRecord::Integer i = 0;
       !has_failed && i < (KnowledgeRecord::Integer)records.size (); ++i)
  {
    if (records[i] != i + 75)
    {
      has_failed = true;
      std::cerr << " Fail: records[" << i << "]=" << records[i] << "...";
    }
  }

  if (!has_failed)
  {
    std::cerr << "SUCCESS\n";
  }
  else
  {
    std::cerr << "FAIL\n";
    ++madara_fails;
    
    std::cerr << "    size=" << records.size () << "\n";
    for (auto record : records)
    {
      std::cerr << "      " << record << "\n";
    }
  }

  std::cerr << "  Testing remaining()...";

  if (consumer.remaining () == 0)
  {
    std::cerr << "SUCCESS\n";
  }
  else
  {
    std::cerr << "FAIL\n";
    ++madara_fails;
    std::cerr << "      remaining() == " << consumer.remaining () << "\n";
  }

  std::cerr << "  Testing count()...";

  if (consumer.count () == 100)
  {
    std::cerr << "SUCCESS\n";
  }
  else
  {
    std::cerr << "FAIL\n";
    ++madara_fails;
    std::cerr << "      count() == " << consumer.count () << "\n";
  }

  std::cerr << "  Testing size()...";

  if (consumer.size () == 100)
  {
    std::cerr << "SUCCESS\n";
  }
  else
  {
    std::cerr << "FAIL\n";
    ++madara_fails;
    std::cerr << "      size() == " << consumer.size () << "\n";
  }

  std::cerr << "  Adding 25 more elements...\n";

  for (KnowledgeRecord::Integer i = 0; i < 25; ++i)
  {
    producer.add (KnowledgeRecord (i + 100));
  }

  std::cerr << "  Testing remaining()...";

  if (consumer.remaining () == 25)
  {
    std::cerr << "SUCCESS\n";
  }
  else
  {
    std::cerr << "FAIL\n";
    ++madara_fails;
    std::cerr << "      remaining() == " << consumer.remaining () << "\n";
  }

  std::cerr << "  Testing count()...";

  if (consumer.count () == 100)
  {
    std::cerr << "SUCCESS\n";
  }
  else
  {
    std::cerr << "FAIL\n";
    ++madara_fails;
    std::cerr << "      count() == " << consumer.count () << "\n";
  }

  std::cerr << "  Testing size()...";

  if (consumer.size () == 100)
  {
    std::cerr << "SUCCESS\n";
  }
  else
  {
    std::cerr << "FAIL\n";
    ++madara_fails;
    std::cerr << "      size() == " << consumer.size () << "\n";
  }


  std::cerr << "  Testing get_earliest(5)...";

  records = consumer.consume_earliest (5);

  has_failed = records.size () != 5;

  for (KnowledgeRecord::Integer i = 0;
       !has_failed && i < (KnowledgeRecord::Integer)records.size (); ++i)
  {
    if (records[i] != i + 100)
    {
      has_failed = true;
      std::cerr << " Fail: records[" << i << "]=" << records[i] << "...";
    }
  }

  if (!has_failed)
  {
    std::cerr << "SUCCESS\n";
  }
  else
  {
    std::cerr << "FAIL\n";
    ++madara_fails;
    
    std::cerr << "    size=" << records.size () << "\n";
    for (auto record : records)
    {
      std::cerr << "      " << record << "\n";
    }
  }

  std::cerr << "  Testing remaining()...";

  if (consumer.remaining () == 20)
  {
    std::cerr << "SUCCESS\n";
  }
  else
  {
    std::cerr << "FAIL\n";
    ++madara_fails;
    std::cerr << "      remaining() == " << consumer.remaining () << "\n";
  }

  std::cerr << "  Testing inspect(0)...";

  if (consumer.inspect (0) == 104)
  {
    std::cerr << "SUCCESS\n";
  }
  else
  {
    std::cerr << "FAIL\n";
    ++madara_fails;
    std::cerr << "      inspect(0) == " << consumer.inspect (0) << "\n";
  }

  std::cerr << "  Testing inspect(-1)...";

  if (consumer.inspect (-1) == 103)
  {
    std::cerr << "SUCCESS\n";
  }
  else
  {
    std::cerr << "FAIL\n";
    ++madara_fails;
    std::cerr << "      inspect(-1) == " << consumer.inspect (-1) << "\n";
  }

  std::cerr << "  Testing inspect(-2)...";

  if (consumer.inspect (-2) == 102)
  {
    std::cerr << "SUCCESS\n";
  }
  else
  {
    std::cerr << "FAIL\n";
    ++madara_fails;
    std::cerr << "      inspect(-2) == " << consumer.inspect (-2) << "\n";
  }

  for (KnowledgeRecord::Integer i = 0; i < 5; ++i)
  {
    producer.add (KnowledgeRecord (i + 105));
  }

  std::cerr << "  Testing inspect(1)...";

  if (consumer.inspect (1) == 105)
  {
    std::cerr << "SUCCESS\n";
  }
  else
  {
    std::cerr << "FAIL\n";
    ++madara_fails;
    std::cerr << "      inspect(1) == " << consumer.inspect (-1) << "\n";
  }

  std::cerr << "  Testing inspect(2)...";

  if (consumer.inspect (2) == 106)
  {
    std::cerr << "SUCCESS\n";
  }
  else
  {
    std::cerr << "FAIL\n";
    ++madara_fails;
    std::cerr << "      inspect(2) == " << consumer.inspect (2) << "\n";
  }

  std::cerr << "  Testing inspect(-2, 5)...";

  records = consumer.inspect (-2, 5);

  if (records.size () == 5 &&
      records[0] == 102 &&
      records[1] == 103 &&
      records[2] == 104 &&
      records[3] == 105 &&
      records[4] == 106)
  {
    std::cerr << "SUCCESS\n";
  }
  else
  {
    std::cerr << "FAIL\n";
    ++madara_fails;

    std::cerr << "    size=" << records.size () << "\n";
    for (auto record : records)
    {
      std::cerr << "      " << record << "\n";
    }
  }

  std::cerr << "  Testing get_latest(5)...";

  records = consumer.consume_latest (5);

  has_failed = records.size () != 5;

  for (KnowledgeRecord::Integer i = 0;
       !has_failed && i < (KnowledgeRecord::Integer)records.size (); ++i)
  {
    if (records[i] != 109 - i)
    {
      has_failed = true;
      std::cerr << " Fail: records[" << i << "]=" << records[i] << "...";
    }
  }

  if (!has_failed)
  {
    std::cerr << "SUCCESS\n";
  }
  else
  {
    std::cerr << "FAIL\n";
    ++madara_fails;
    
    std::cerr << "    size=" << records.size () << "\n";
    for (auto record : records)
    {
      std::cerr << "      " << record << "\n";
    }
  }

  std::cerr << "  Testing remaining()...";

  if (consumer.remaining () == 0)
  {
    std::cerr << "SUCCESS\n";
  }
  else
  {
    std::cerr << "FAIL\n";
    ++madara_fails;
    std::cerr << "      remaining() == " << consumer.remaining () << "\n";
  }

  std::cerr << "  producer.resize(41), consumer.resize()...\n";

  producer.resize (41);
  consumer.resize ();

  std::cerr << "  Testing producer.size()...";

  if (producer.size () == 41)
  {
    std::cerr << "SUCCESS\n";
  }
  else
  {
    std::cerr << "FAIL\n";
    ++madara_fails;
    std::cerr << "      size() == " << producer.size () << "\n";
  }

  std::cerr << "  Testing consumer.size()...";

  if (consumer.size () == 41)
  {
    std::cerr << "SUCCESS\n";
  }
  else
  {
    std::cerr << "FAIL\n";
    ++madara_fails;
    std::cerr << "      size() == " << consumer.size () << "\n";
  }

  std::cerr << "  producer.add() x 95 times...\n";

  for (KnowledgeRecord::Integer i = 0; i < 95; ++i)
  {
    producer.add (KnowledgeRecord (i));
  }

  std::cerr << "  consumer.get() x 5...\n";

  std::cerr << "  Testing get()...";

  KnowledgeRecord result = consumer.consume ();

  if (result == 54)
  {
    std::cerr << "SUCCESS\n";
  }
  else
  {
    std::cerr << "FAIL\n";
    ++madara_fails;
    std::cerr << "      get() == " << result << "\n";
  }

  std::cerr << "  Testing get()...";

  result = consumer.consume ();

  if (result == 55)
  {
    std::cerr << "SUCCESS\n";
  }
  else
  {
    std::cerr << "FAIL\n";
    ++madara_fails;
    std::cerr << "      get() == " << result << "\n";
  }

  std::cerr << "  Testing get()...";

  result = consumer.consume ();

  if (result == 56)
  {
    std::cerr << "SUCCESS\n";
  }
  else
  {
    std::cerr << "FAIL\n";
    ++madara_fails;
    std::cerr << "      get() == " << result << "\n";
  }

  std::cerr << "  Testing get()...";

  result = consumer.consume ();

  if (result == 57)
  {
    std::cerr << "SUCCESS\n";
  }
  else
  {
    std::cerr << "FAIL\n";
    ++madara_fails;
    std::cerr << "      get() == " << result << "\n";
  }

  std::cerr << "  Testing get()...";

  result = consumer.consume ();

  if (result == 58)
  {
    std::cerr << "SUCCESS\n";
  }
  else
  {
    std::cerr << "FAIL\n";
    ++madara_fails;
    std::cerr << "      get() == " << result << "\n";
  }

  std::cerr << "  Testing get()...";

  result = consumer.consume ();

  if (result == 59)
  {
    std::cerr << "SUCCESS\n";
  }
  else
  {
    std::cerr << "FAIL\n";
    ++madara_fails;
    std::cerr << "      get() == " << result << "\n";
  }

  std::cerr << "  Testing peek_latest()...";

  result = consumer.peek_latest ();

  if (result == 94)
  {
    std::cerr << "SUCCESS\n";
  }
  else
  {
    std::cerr << "FAIL\n";
    ++madara_fails;
    std::cerr << "      peek_latest() == " << result << "\n";
  }

  std::cerr << "  Testing peek_latest(5)...";

  records = consumer.peek_latest (5);

  if (records.size () == 5 &&
      records[0] == 94 &&
      records[1] == 93 &&
      records[2] == 92 &&
      records[3] == 91 &&
      records[4] == 90)
  {
    std::cerr << "SUCCESS\n";
  }
  else
  {
    std::cerr << "FAIL\n";
    ++madara_fails;

    std::cerr << "    size=" << records.size () << "\n";
    for (auto record : records)
    {
      std::cerr << "      " << record << "\n";
    }
  }
}

void test_native_circular_consumer (void)
{
  std::cerr <<
    "************* CIRCULARBUFFER: Testing NativeCircularBufferConsumer*************\n";

  knowledge::KnowledgeBase kb;
  containers::CircularBuffer producer ("buffer", kb, 100);
  containers::NativeCircularBufferConsumer consumer ("buffer", kb);

  for (KnowledgeRecord::Integer i = 0; i < 100; ++i)
  {
    producer.add (KnowledgeRecord (i));
  }


  std::cerr << "  Testing get_earliest(50)...";

  std::vector <KnowledgeRecord> records = consumer.consume_many (50);

  bool has_failed = records.size () != 50;

  for (KnowledgeRecord::Integer i = 0;
       !has_failed && i < (KnowledgeRecord::Integer)records.size (); ++i)
  {
    if (records[i] != i)
    {
      has_failed = true;
      std::cerr << " Fail: records[" << i << "]=" << records[i] << "...";
    }
  }

  if (!has_failed)
  {
    std::cerr << "SUCCESS\n";
  }
  else
  {
    std::cerr << "FAIL\n";
    ++madara_fails;
    
    std::cerr << "    size=" << records.size () << "\n";
    for (auto record : records)
    {
      std::cerr << "      " << record << "\n";
    }
  }


  std::cerr << "  Testing remaining()...";

  if (consumer.remaining () == 50)
  {
    std::cerr << "SUCCESS\n";
  }
  else
  {
    std::cerr << "FAIL\n";
    ++madara_fails;
    std::cerr << "      remaining() == " << consumer.remaining () << "\n";
  }

  std::cerr << "  Testing get_earliest(25)...";

  records = consumer.consume_many (25);

  has_failed = records.size () != 25;

  for (KnowledgeRecord::Integer i = 0;
       !has_failed && i < (KnowledgeRecord::Integer)records.size (); ++i)
  {
    if (records[i] != i + 50)
    {
      has_failed = true;
      std::cerr << " Fail: records[" << i << "]=" << records[i] << "...";
    }
  }

  if (!has_failed)
  {
    std::cerr << "SUCCESS\n";
  }
  else
  {
    std::cerr << "FAIL\n";
    ++madara_fails;
    
    std::cerr << "    size=" << records.size () << "\n";
    for (auto record : records)
    {
      std::cerr << "      " << record << "\n";
    }
  }

  std::cerr << "  Testing remaining()...";

  if (consumer.remaining () == 25)
  {
    std::cerr << "SUCCESS\n";
  }
  else
  {
    std::cerr << "FAIL\n";
    ++madara_fails;
    std::cerr << "      remaining() == " << consumer.remaining () << "\n";
  }

  std::cerr << "  Testing get_earliest(25)...";

  records = consumer.consume_many (25);

  has_failed = records.size () != 25;

  for (KnowledgeRecord::Integer i = 0;
       !has_failed && i < (KnowledgeRecord::Integer)records.size (); ++i)
  {
    if (records[i] != i + 75)
    {
      has_failed = true;
      std::cerr << " Fail: records[" << i << "]=" << records[i] << "...";
    }
  }

  if (!has_failed)
  {
    std::cerr << "SUCCESS\n";
  }
  else
  {
    std::cerr << "FAIL\n";
    ++madara_fails;
    
    std::cerr << "    size=" << records.size () << "\n";
    for (auto record : records)
    {
      std::cerr << "      " << record << "\n";
    }
  }

  std::cerr << "  Testing remaining()...";

  if (consumer.remaining () == 0)
  {
    std::cerr << "SUCCESS\n";
  }
  else
  {
    std::cerr << "FAIL\n";
    ++madara_fails;
    std::cerr << "      remaining() == " << consumer.remaining () << "\n";
  }

  std::cerr << "  Testing count()...";

  if (consumer.count () == 100)
  {
    std::cerr << "SUCCESS\n";
  }
  else
  {
    std::cerr << "FAIL\n";
    ++madara_fails;
    std::cerr << "      count() == " << consumer.count () << "\n";
  }

  std::cerr << "  Testing size()...";

  if (consumer.size () == 100)
  {
    std::cerr << "SUCCESS\n";
  }
  else
  {
    std::cerr << "FAIL\n";
    ++madara_fails;
    std::cerr << "      size() == " << consumer.size () << "\n";
  }

  std::cerr << "  Adding 25 more elements...\n";

  for (KnowledgeRecord::Integer i = 0; i < 25; ++i)
  {
    producer.add (KnowledgeRecord (i + 100));
  }

  std::cerr << "  Testing remaining()...";

  if (consumer.remaining () == 25)
  {
    std::cerr << "SUCCESS\n";
  }
  else
  {
    std::cerr << "FAIL\n";
    ++madara_fails;
    std::cerr << "      remaining() == " << consumer.remaining () << "\n";
  }

  std::cerr << "  Testing count()...";

  if (consumer.count () == 100)
  {
    std::cerr << "SUCCESS\n";
  }
  else
  {
    std::cerr << "FAIL\n";
    ++madara_fails;
    std::cerr << "      count() == " << consumer.count () << "\n";
  }

  std::cerr << "  Testing size()...";

  if (consumer.size () == 100)
  {
    std::cerr << "SUCCESS\n";
  }
  else
  {
    std::cerr << "FAIL\n";
    ++madara_fails;
    std::cerr << "      size() == " << consumer.size () << "\n";
  }


  std::cerr << "  Testing get_earliest(5)...";

  records = consumer.consume_many (5);

  has_failed = records.size () != 5;

  for (KnowledgeRecord::Integer i = 0;
       !has_failed && i < (KnowledgeRecord::Integer)records.size (); ++i)
  {
    if (records[i] != i + 100)
    {
      has_failed = true;
      std::cerr << " Fail: records[" << i << "]=" << records[i] << "...";
    }
  }

  if (!has_failed)
  {
    std::cerr << "SUCCESS\n";
  }
  else
  {
    std::cerr << "FAIL\n";
    ++madara_fails;
    
    std::cerr << "    size=" << records.size () << "\n";
    for (auto record : records)
    {
      std::cerr << "      " << record << "\n";
    }
  }

  std::cerr << "  Testing remaining()...";

  if (consumer.remaining () == 20)
  {
    std::cerr << "SUCCESS\n";
  }
  else
  {
    std::cerr << "FAIL\n";
    ++madara_fails;
    std::cerr << "      remaining() == " << consumer.remaining () << "\n";
  }

  std::cerr << "  Testing inspect(0)...";

  if (consumer.inspect (0) == 104)
  {
    std::cerr << "SUCCESS\n";
  }
  else
  {
    std::cerr << "FAIL\n";
    ++madara_fails;
    std::cerr << "      inspect(0) == " << consumer.inspect (0) << "\n";
  }

  std::cerr << "  Testing inspect(-1)...";

  if (consumer.inspect (-1) == 103)
  {
    std::cerr << "SUCCESS\n";
  }
  else
  {
    std::cerr << "FAIL\n";
    ++madara_fails;
    std::cerr << "      inspect(-1) == " << consumer.inspect (-1) << "\n";
  }

  std::cerr << "  Testing inspect(-2)...";

  if (consumer.inspect (-2) == 102)
  {
    std::cerr << "SUCCESS\n";
  }
  else
  {
    std::cerr << "FAIL\n";
    ++madara_fails;
    std::cerr << "      inspect(-2) == " << consumer.inspect (-2) << "\n";
  }

  for (KnowledgeRecord::Integer i = 0; i < 5; ++i)
  {
    producer.add (KnowledgeRecord (i + 105));
  }

  std::cerr << "  Testing inspect(1)...";

  if (consumer.inspect (1) == 105)
  {
    std::cerr << "SUCCESS\n";
  }
  else
  {
    std::cerr << "FAIL\n";
    ++madara_fails;
    std::cerr << "      inspect(1) == " << consumer.inspect (-1) << "\n";
  }

  std::cerr << "  Testing inspect(2)...";

  if (consumer.inspect (2) == 106)
  {
    std::cerr << "SUCCESS\n";
  }
  else
  {
    std::cerr << "FAIL\n";
    ++madara_fails;
    std::cerr << "      inspect(2) == " << consumer.inspect (2) << "\n";
  }

  std::cerr << "  Testing inspect(-2, 5)...";

  records = consumer.inspect (-2, 5);

  if (records.size () == 5 &&
      records[0] == 102 &&
      records[1] == 103 &&
      records[2] == 104 &&
      records[3] == 105 &&
      records[4] == 106)
  {
    std::cerr << "SUCCESS\n";
  }
  else
  {
    std::cerr << "FAIL\n";
    ++madara_fails;

    std::cerr << "    size=" << records.size () << "\n";
    for (auto record : records)
    {
      std::cerr << "      " << record << "\n";
    }
  }

  std::cerr << "  Testing get_latest(5)...";

  records = consumer.consume_latest (5);

  has_failed = records.size () != 5;

  for (KnowledgeRecord::Integer i = 0;
       !has_failed && i < (KnowledgeRecord::Integer)records.size (); ++i)
  {
    if (records[i] != 109 - i)
    {
      has_failed = true;
      std::cerr << " Fail: records[" << i << "]=" << records[i] << "...";
    }
  }

  if (!has_failed)
  {
    std::cerr << "SUCCESS\n";
  }
  else
  {
    std::cerr << "FAIL\n";
    ++madara_fails;
    
    std::cerr << "    size=" << records.size () << "\n";
    for (auto record : records)
    {
      std::cerr << "      " << record << "\n";
    }
  }

  std::cerr << "  Testing remaining()...";

  if (consumer.remaining () == 0)
  {
    std::cerr << "SUCCESS\n";
  }
  else
  {
    std::cerr << "FAIL\n";
    ++madara_fails;
    std::cerr << "      remaining() == " << consumer.remaining () << "\n";
  }

  std::cerr << "  producer.add() x 95 times...\n";

  for (KnowledgeRecord::Integer i = 0; i < 95; ++i)
  {
    producer.add (KnowledgeRecord (i));
  }

  std::cerr << "  consumer.get() x 5...\n";

  std::cerr << "  Testing get()...";

  KnowledgeRecord result = consumer.consume ();

  if (result == 54)
  {
    std::cerr << "SUCCESS\n";
  }
  else
  {
    std::cerr << "FAIL\n";
    ++madara_fails;
    std::cerr << "      get() == " << result << "\n";
  }

  std::cerr << "  Testing get()...";

  result = consumer.consume ();

  if (result == 55)
  {
    std::cerr << "SUCCESS\n";
  }
  else
  {
    std::cerr << "FAIL\n";
    ++madara_fails;
    std::cerr << "      get() == " << result << "\n";
  }

  std::cerr << "  Testing get()...";

  result = consumer.consume ();

  if (result == 56)
  {
    std::cerr << "SUCCESS\n";
  }
  else
  {
    std::cerr << "FAIL\n";
    ++madara_fails;
    std::cerr << "      get() == " << result << "\n";
  }

  std::cerr << "  Testing get()...";

  result = consumer.consume ();

  if (result == 57)
  {
    std::cerr << "SUCCESS\n";
  }
  else
  {
    std::cerr << "FAIL\n";
    ++madara_fails;
    std::cerr << "      get() == " << result << "\n";
  }

  std::cerr << "  Testing get()...";

  result = consumer.consume ();

  if (result == 58)
  {
    std::cerr << "SUCCESS\n";
  }
  else
  {
    std::cerr << "FAIL\n";
    ++madara_fails;
    std::cerr << "      get() == " << result << "\n";
  }

  std::cerr << "  Testing get()...";

  result = consumer.consume ();

  if (result == 59)
  {
    std::cerr << "SUCCESS\n";
  }
  else
  {
    std::cerr << "FAIL\n";
    ++madara_fails;
    std::cerr << "      get() == " << result << "\n";
  }

  std::cerr << "  Testing peek_latest()...";

  result = consumer.peek_latest ();

  if (result == 94)
  {
    std::cerr << "SUCCESS\n";
  }
  else
  {
    std::cerr << "FAIL\n";
    ++madara_fails;
    std::cerr << "      peek_latest() == " << result << "\n";
  }

  std::cerr << "  Testing peek_latest(5)...";

  records = consumer.peek_latest (5);

  if (records.size () == 5 &&
      records[0] == 94 &&
      records[1] == 93 &&
      records[2] == 92 &&
      records[3] == 91 &&
      records[4] == 90)
  {
    std::cerr << "SUCCESS\n";
  }
  else
  {
    std::cerr << "FAIL\n";
    ++madara_fails;

    std::cerr << "    size=" << records.size () << "\n";
    for (auto record : records)
    {
      std::cerr << "      " << record << "\n";
    }
  }

  result = consumer.consume_latest ();
  if ( result == 94 )
  {
    std::cerr << "SUCCESS\n";
  }
  else
  {
    std::cerr << "FAIL\n";
    ++madara_fails;
    std::cerr << "      consume_latest() == " << result << "\n";
  }

}

class SpecialClass
{
  public:
  int x;
  int y;
};


std::ostream & operator<< (std::ostream & output, const SpecialClass & value)
{
  output << "class.x=" << value.x << "," "class.y=" << value.y << "\n";
  return output;
}

template<typename Fun, typename T>
auto for_each_field(Fun &&fun, T &&val) -> madara::enable_if_same_decayed<T, SpecialClass>
{
  fun("x", val.x);
  fun("y", val.y);
}

void test_circular_any (void)
{
  std::cerr <<
    "************* CB: Testing CircularBuffer Any*************\n";

  SpecialClass sample;
  sample.x = 1;
  sample.y = 2;

  knowledge::KnowledgeBase kb;
  containers::CircularBuffer producer ("buffer", kb, 10);
  std::vector <KnowledgeRecord> records;
  std::vector <SpecialClass> classes;
  SpecialClass class_result;
  KnowledgeRecord record_result;
  bool has_failed;

  std::cerr << "  Testing add<T> and get<T>...";

  producer.add (sample);

  producer.get (class_result);

  if (class_result.x == 1 && class_result.y == 2)
  {
    std::cerr << "SUCCESS\n";
  }
  else
  {
    std::cerr << "FAIL\n";
    ++madara_fails;
    std::cerr << "    class_result.x==" << class_result.x << ", ";
    std::cerr << "class_result.y==" << class_result.y << "\n";
  }

  for (int i = 0; i < 10; ++i)
  {
    sample.x = i;
    sample.y = i;
    producer.add (sample);
  }

  std::cerr << "  Testing get_earliest(10)...";

  classes.clear ();
  producer.get_earliest (10, classes);

  has_failed = classes.size () != 10;

  for (int i = 0;
       !has_failed && i < (int)classes.size (); ++i)
  {
    if (classes[i].x != i && classes[i].y != i)
    {
      has_failed = true;
      std::cerr << " Fail: records[" << i << "]=" << records[i] << "...";
    }
  }

  if (!has_failed)
  {
    std::cerr << "SUCCESS\n";
  }
  else
  {
    std::cerr << "FAIL\n";
    ++madara_fails;
    
    std::cerr << "    size=" << classes.size () << "\n";
    for (auto record : classes)
    {
      std::cerr << "      " << record << "\n";
    }
  }

  std::cerr << "  Testing get_latest(10)...";

  classes.clear ();
  producer.get_latest (10, classes);

  has_failed = classes.size () != 10;

  for (int i = 0;
       !has_failed && i < (int)classes.size (); ++i)
  {
    if (classes[i].x != 9 - i && classes[i].y != 9 - i)
    {
      has_failed = true;
      std::cerr << " Fail: records[" << i << "]=" << records[i] << "...";
    }
  }

  if (!has_failed)
  {
    std::cerr << "SUCCESS\n";
  }
  else
  {
    std::cerr << "FAIL\n";
    ++madara_fails;
    
    std::cerr << "    size=" << classes.size () << "\n";
    for (auto record : classes)
    {
      std::cerr << "      " << record << "\n";
    }
  }

  std::cerr << "  Testing inspect(1)...";

  producer.inspect (1, class_result);

  if (class_result.x == 0 && class_result.y == 0)
  {
    std::cerr << "SUCCESS\n";
  }
  else
  {
    std::cerr << "FAIL\n";
    ++madara_fails;
    std::cerr << "    class_result.x==" << class_result.x << ", ";
    std::cerr << "class_result.y==" << class_result.y << "\n";
  }

  for (int i = 0; i < 10; ++i)
  {
    sample.x = i;
    sample.y = i;
    producer.add (sample);
  }

  std::cerr << "  Testing inspect(10)...";

  classes.clear ();
  producer.inspect (1, 10, classes);

  has_failed = classes.size () != 10;

  for (int i = 0;
       !has_failed && i < (int)classes.size (); ++i)
  {
    if (classes[i].x != i && classes[i].y != i)
    {
      has_failed = true;
      std::cerr << " Fail: records[" << i << "]=" << records[i] << "...";
    }
  }

  if (!has_failed)
  {
    std::cerr << "SUCCESS\n";
  }
  else
  {
    std::cerr << "FAIL\n";
    ++madara_fails;
    
    std::cerr << "    size=" << classes.size () << "\n";
    for (auto record : classes)
    {
      std::cerr << "      " << record << "\n";
    }
  }

}

void test_circulart_any (void)
{
  std::cerr <<
    "************* CBT: Testing CircularBufferT Any*************\n";

  SpecialClass sample;
  sample.x = 1;
  sample.y = 2;

  knowledge::KnowledgeBase kb;
  containers::CircularBufferT <SpecialClass> producer ("buffer", kb, 10);
  std::vector <KnowledgeRecord> records;
  std::vector <SpecialClass> classes;
  SpecialClass class_result;
  KnowledgeRecord record_result;
  bool has_failed;

  std::cerr << "  Testing add<T> and get<T>...";

  producer.add (sample);

  producer.get (class_result);

  if (class_result.x == 1 && class_result.y == 2)
  {
    std::cerr << "SUCCESS\n";
  }
  else
  {
    std::cerr << "FAIL\n";
    ++madara_fails;
    std::cerr << "    class_result.x==" << class_result.x << ", ";
    std::cerr << "class_result.y==" << class_result.y << "\n";
  }

  for (int i = 0; i < 10; ++i)
  {
    sample.x = i;
    sample.y = i;
    producer.add (sample);
  }

  std::cerr << "  Testing get_earliest(10)...";

  classes.clear ();
  producer.get_earliest (10, classes);

  has_failed = classes.size () != 10;

  for (int i = 0;
       !has_failed && i < (int)classes.size (); ++i)
  {
    if (classes[i].x != i && classes[i].y != i)
    {
      has_failed = true;
      std::cerr << " Fail: records[" << i << "]=" << records[i] << "...";
    }
  }

  if (!has_failed)
  {
    std::cerr << "SUCCESS\n";
  }
  else
  {
    std::cerr << "FAIL\n";
    ++madara_fails;
    
    std::cerr << "    size=" << classes.size () << "\n";
    for (auto record : classes)
    {
      std::cerr << "      " << record << "\n";
    }
  }

  std::cerr << "  Testing get_latest(10)...";

  classes.clear ();
  producer.get_latest (10, classes);

  has_failed = classes.size () != 10;

  for (int i = 0;
       !has_failed && i < (int)classes.size (); ++i)
  {
    if (classes[i].x != 9 - i && classes[i].y != 9 - i)
    {
      has_failed = true;
      std::cerr << " Fail: records[" << i << "]=" << records[i] << "...";
    }
  }

  if (!has_failed)
  {
    std::cerr << "SUCCESS\n";
  }
  else
  {
    std::cerr << "FAIL\n";
    ++madara_fails;
    
    std::cerr << "    size=" << classes.size () << "\n";
    for (auto record : classes)
    {
      std::cerr << "      " << record << "\n";
    }
  }

  std::cerr << "  Testing inspect(1)...";

  producer.inspect (1, class_result);

  if (class_result.x == 0 && class_result.y == 0)
  {
    std::cerr << "SUCCESS\n";
  }
  else
  {
    std::cerr << "FAIL\n";
    ++madara_fails;
    std::cerr << "    class_result.x==" << class_result.x << ", ";
    std::cerr << "class_result.y==" << class_result.y << "\n";
  }

  for (int i = 0; i < 10; ++i)
  {
    sample.x = i;
    sample.y = i;
    producer.add (sample);
  }

  std::cerr << "  Testing inspect(10)...";

  classes.clear ();
  producer.inspect (1, 10, classes);

  has_failed = classes.size () != 10;

  for (int i = 0;
       !has_failed && i < (int)classes.size (); ++i)
  {
    if (classes[i].x != i && classes[i].y != i)
    {
      has_failed = true;
      std::cerr << " Fail: records[" << i << "]=" << records[i] << "...";
    }
  }

  if (!has_failed)
  {
    std::cerr << "SUCCESS\n";
  }
  else
  {
    std::cerr << "FAIL\n";
    ++madara_fails;
    
    std::cerr << "    size=" << classes.size () << "\n";
    for (auto record : classes)
    {
      std::cerr << "      " << record << "\n";
    }
  }

}

void test_circular_consumer_any (void)
{
  std::cerr <<
    "********** CBC: Testing CircularBufferConsumer Any**********\n";

  SpecialClass sample;
  sample.x = 1;
  sample.y = 2;

  knowledge::KnowledgeBase kb;
  containers::CircularBuffer producer ("buffer", kb, 10);
  containers::CircularBufferConsumer consumer ("buffer", kb);
  std::vector <KnowledgeRecord> records;
  std::vector <SpecialClass> classes;
  std::vector <SpecialClass> classes_return;
  SpecialClass class_result;
  KnowledgeRecord record_result;
  bool has_failed;

  std::cerr << "  Testing add<T> and consume<T>...";

  producer.add (sample);

  consumer.consume (class_result);

  if (class_result.x == 1 && class_result.y == 2)
  {
    std::cerr << "SUCCESS\n";
  }
  else
  {
    std::cerr << "FAIL\n";
    ++madara_fails;
    std::cerr << "    class_result.x==" << class_result.x << ", ";
    std::cerr << "class_result.y==" << class_result.y << "\n";
  }

  std::cerr << "  Testing peek_latest()...";

  consumer.peek_latest (class_result);

  if (class_result.x == 1 && class_result.y == 2)
  {
    std::cerr << "SUCCESS\n";
  }
  else
  {
    std::cerr << "FAIL\n";
    ++madara_fails;
    std::cerr << "    class_result.x==" << class_result.x << ", ";
    std::cerr << "class_result.y==" << class_result.y << "\n";
  }

  for (int i = 0; i < 10; ++i)
  {
    sample.x = i;
    sample.y = i;
    producer.add (sample);
  }

  std::cerr << "  Testing get_earliest(10)...";

  classes.clear ();
  consumer.consume_earliest (10, classes);

  has_failed = classes.size () != 10;

  for (int i = 0;
       !has_failed && i < (int)classes.size (); ++i)
  {
    if (classes[i].x != i && classes[i].y != i)
    {
      has_failed = true;
      std::cerr << " Fail: records[" << i << "]=" << records[i] << "...";
    }
  }

  if (!has_failed)
  {
    std::cerr << "SUCCESS\n";
  }
  else
  {
    std::cerr << "FAIL\n";
    ++madara_fails;
    
    std::cerr << "    size=" << classes.size () << "\n";
    for (auto record : classes)
    {
      std::cerr << "      " << record << "\n";
    }
  }

  std::cerr << "  Testing get_latest(10)...";

  producer.add (classes);
  classes.clear ();
  consumer.consume_latest (10, classes);

  has_failed = classes.size () != 10;

  for (int i = 0;
       !has_failed && i < (int)classes.size (); ++i)
  {
    if (classes[i].x != 9 - i && classes[i].y != 9 - i)
    {
      has_failed = true;
      std::cerr << " Fail: records[" << i << "]=" << records[i] << "...";
    }
  }

  if (!has_failed)
  {
    std::cerr << "SUCCESS\n";
  }
  else
  {
    std::cerr << "FAIL\n";
    ++madara_fails;
    
    std::cerr << "    size=" << classes.size () << "\n";
    for (auto record : classes)
    {
      std::cerr << "      " << record << "\n";
    }
  }

  producer.add(classes);

  std::cerr << "  Testing inspect(1)...";

  consumer.inspect (1, class_result);

  if (class_result.x == 9 && class_result.y == 9)
  {
    std::cerr << "SUCCESS\n";
  }
  else
  {
    std::cerr << "FAIL\n";
    ++madara_fails;
    std::cerr << "    class_result.x==" << class_result.x << ", ";
    std::cerr << "class_result.y==" << class_result.y << "\n";
  }

  for (int i = 0; i < 10; ++i)
  {
    sample.x = i;
    sample.y = i;
    producer.add (sample);
  }

  std::cerr << "  Testing inspect(10)...";

  classes.clear ();
  consumer.inspect (1, 10, classes);

  has_failed = classes.size () != 10;

  for (int i = 0;
       !has_failed && i < (int)classes.size (); ++i)
  {
    if (classes[i].x != i && classes[i].y != i)
    {
      has_failed = true;
      std::cerr << " Fail: records[" << i << "]=" << records[i] << "...";
    }
  }

  if (!has_failed)
  {
    std::cerr << "SUCCESS\n";
  }
  else
  {
    std::cerr << "FAIL\n";
    ++madara_fails;
    
    std::cerr << "    size=" << classes.size () << "\n";
    for (auto record : classes)
    {
      std::cerr << "      " << record << "\n";
    }
  }

  std::cerr << "  Testing peek_latest(10)...";

  classes.clear ();
  consumer.peek_latest (10, classes);

  has_failed = classes.size () != 10;

  for (int i = 0;
       !has_failed && i < (int)classes.size (); ++i)
  {
    if (classes[i].x != 9 - i && classes[i].y != 9 - i)
    {
      has_failed = true;
      std::cerr << " Fail: records[" << i << "]=" << records[i] << "...";
    }
  }

  if (!has_failed)
  {
    std::cerr << "SUCCESS\n";
  }
  else
  {
    std::cerr << "FAIL\n";
    ++madara_fails;
    
    std::cerr << "    size=" << classes.size () << "\n";
    for (auto record : classes)
    {
      std::cerr << "      " << record << "\n";
    }
  }

  std::cerr << "  Testing dropped...";

  producer.resize (5);
  consumer.resize ();
  consumer.consume_latest (1, classes_return);
  producer.add (classes);
  size_t dropped = 0;

  consumer.consume_earliest ((size_t)1, classes_return, dropped);

  if (dropped == 5)
  {
    std::cerr << "SUCCESS\n";
  }
  else
  {
    std::cerr << "FAIL\n";
    ++madara_fails;
    
    std::cerr << "    dropped=" << dropped << "\n";
  }
}


void test_circular_consumert_any (void)
{
  std::cerr <<
  "********** CBCT: Testing CircularBufferConsumerT Any**********\n";

  SpecialClass sample;
  sample.x = 1;
  sample.y = 2;

  knowledge::KnowledgeBase kb;
  containers::CircularBuffer producer ("buffer", kb, 10);
  containers::CircularBufferConsumerT<SpecialClass> consumer ("buffer", kb);
  std::vector <KnowledgeRecord> records;
  std::vector <SpecialClass> classes;
  std::vector <SpecialClass> classes_return;
  SpecialClass class_result;
  bool has_failed;

  std::cerr << "  Testing add<T> and consume<T>...";

  producer.add (sample);

  consumer.consume (class_result);

  if (class_result.x == 1 && class_result.y == 2)
  {
    std::cerr << "SUCCESS\n";
  }
  else
  {
    std::cerr << "FAIL\n";
    ++madara_fails;
    std::cerr << "    class_result.x==" << class_result.x << ", ";
    std::cerr << "class_result.y==" << class_result.y << "\n";
  }

  std::cerr << "  Testing peek_latest()...";

  consumer.peek_latest (class_result);

  if (class_result.x == 1 && class_result.y == 2)
  {
    std::cerr << "SUCCESS\n";
  }
  else
  {
    std::cerr << "FAIL\n";
    ++madara_fails;
    std::cerr << "    class_result.x==" << class_result.x << ", ";
    std::cerr << "class_result.y==" << class_result.y << "\n";
  }

  for (int i = 0; i < 10; ++i)
  {
    sample.x = i;
    sample.y = i;
    producer.add (sample);
  }

  std::cerr << "  Testing get_earliest(10)...";

  classes.clear ();
  consumer.consume_earliest (10, classes);

  has_failed = classes.size () != 10;

  for (int i = 0;
       !has_failed && i < (int)classes.size (); ++i)
  {
    if (classes[i].x != i && classes[i].y != i)
    {
      has_failed = true;
      std::cerr << " Fail: records[" << i << "]=" << records[i] << "...";
    }
  }

  if (!has_failed)
  {
    std::cerr << "SUCCESS\n";
  }
  else
  {
    std::cerr << "FAIL\n";
    ++madara_fails;
    
    std::cerr << "    size=" << classes.size () << "\n";
    for (auto record : classes)
    {
      std::cerr << "      " << record << "\n";
    }
  }

  std::cerr << "  Testing get_latest(10)...";

  producer.add (classes);
  classes.clear ();
  consumer.consume_latest (10, classes);

  has_failed = classes.size () != 10;

  for (int i = 0;
       !has_failed && i < (int)classes.size (); ++i)
  {
    if (classes[i].x != 9 - i && classes[i].y != 9 - i)
    {
      has_failed = true;
      std::cerr << " Fail: records[" << i << "]=" << records[i] << "...";
    }
  }

  if (!has_failed)
  {
    std::cerr << "SUCCESS\n";
  }
  else
  {
    std::cerr << "FAIL\n";
    ++madara_fails;
    
    std::cerr << "    size=" << classes.size () << "\n";
    for (auto record : classes)
    {
      std::cerr << "      " << record << "\n";
    }
  }

  std::cerr << "  Testing inspect(1)...";

  producer.add (classes);

  consumer.inspect (1, class_result);

  if (class_result.x == 9 && class_result.y == 9)
  {
    std::cerr << "SUCCESS\n";
  }
  else
  {
    std::cerr << "FAIL\n";
    ++madara_fails;
    std::cerr << "    class_result.x==" << class_result.x << ", ";
    std::cerr << "class_result.y==" << class_result.y << "\n";
  }

  for (int i = 1; i < 10; ++i)
  {
    sample.x = i;
    sample.y = i;
    producer.add (sample);
  }

  std::cerr << "  Testing inspect(10)...";

  classes.clear ();
  consumer.inspect (1, 10, classes);

  has_failed = classes.size () != 10;

  for (int i = 0;
       !has_failed && i < (int)classes.size (); ++i)
  {
    if (classes[i].x != i && classes[i].y != i)
    {
      has_failed = true;
      std::cerr << " Fail: records[" << i << "]=" << records[i] << "...";
    }
  }

  if (!has_failed)
  {
    std::cerr << "SUCCESS\n";
  }
  else
  {
    std::cerr << "FAIL\n";
    ++madara_fails;
    
    std::cerr << "    size=" << classes.size () << "\n";
    for (auto record : classes)
    {
      std::cerr << "      " << record << "\n";
    }
  }

  std::cerr << "  Testing peek_latest(10)...";

  classes.clear ();
  consumer.peek_latest (10, classes);

  has_failed = classes.size () != 10;

  for (int i = 0;
       !has_failed && i < (int)classes.size (); ++i)
  {
    if (classes[i].x != 9 - i && classes[i].y != 9 - i)
    {
      has_failed = true;
      std::cerr << " Fail: records[" << i << "]=" << records[i] << "...";
    }
  }

  if (!has_failed)
  {
    std::cerr << "SUCCESS\n";
  }
  else
  {
    std::cerr << "FAIL\n";
    ++madara_fails;
    
    std::cerr << "    size=" << classes.size () << "\n";
    for (auto record : classes)
    {
      std::cerr << "      " << record << "\n";
    }
  }

  std::cerr << "  Testing dropped...";

  producer.resize (5);
  consumer.resize ();
  consumer.consume_latest (1, classes_return);
  producer.add (classes);
  size_t dropped = 0;

  consumer.consume_earliest ((size_t)1, classes_return, dropped);

  if (dropped == 5)
  {
    std::cerr << "SUCCESS\n";
  }
  else
  {
    std::cerr << "FAIL\n";
    ++madara_fails;
    
    std::cerr << "    dropped=" << dropped << "\n";
  }
}


int main (int , char **)
{
  test_vector ();
  test_map ();
  test_integer_vector ();
  test_double_vector ();
  test_string_vector ();
  test_integer ();
  test_double ();
  test_map_exchanges ();
  test_vector_exchanges ();
  test_native_vectors ();
  test_queue ();
  
  test_vector_transfer ();
  test_flex_map ();

  test_collection ();
  test_vector_vectors ();

  test_vector2D ();
  test_vector3D ();

  test_circular ();
  test_circular_consumer ();
  test_circular_any ();
  test_circulart_any ();
  test_circular_consumer_any ();
  test_circular_consumert_any ();


  if (madara_fails > 0)
  {
    std::cerr << "OVERALL: FAIL. " << madara_fails << " tests failed.\n";
  }
  else
  {
    std::cerr << "OVERALL: SUCCESS.\n";
  }

  return madara_fails;
}
