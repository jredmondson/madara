
#include "madara/knowledge_engine/containers/Vector.h"
#include "madara/knowledge_engine/containers/Integer_Vector.h"
#include "madara/knowledge_engine/containers/Double_Vector.h"
#include "madara/knowledge_engine/containers/Native_Double_Vector.h"
#include "madara/knowledge_engine/containers/String_Vector.h"
#include "madara/knowledge_engine/containers/Map.h"
#include "madara/knowledge_engine/containers/Flex_Map.h"
#include "madara/knowledge_engine/containers/Integer.h"
#include "madara/knowledge_engine/containers/Double.h"
#include "madara/knowledge_engine/containers/Queue.h"
#include "madara/knowledge_engine/containers/Collection.h"
#include "madara/knowledge_engine/Knowledge_Base.h"
#include <iostream>

namespace engine = Madara::Knowledge_Engine;
namespace containers = engine::Containers;
typedef Madara::Knowledge_Record   Knowledge_Record;

void test_flex_map (void)
{
  std::cout << "************* FLEX MAP: CREATING FLEX MAP*************\n";
  engine::Knowledge_Base knowledge;
  containers::Flex_Map map ("records", knowledge);

  std::cout << "Creating record with name and age\n";
  map["robert"]["name"] = "Robert Jenkins";
  map["robert"]["age"] = 49;

  std::cout << "Creating Flex Map from index operator\n";
  containers::Flex_Map robert_record = map["robert"];

  robert_record["salary"] = 30500.00;
  robert_record["age"] = robert_record["age"].to_integer () + 1;

  containers::Map normal_map;
  robert_record.to_container (normal_map);

  std::cout << "Checking for flex map creation test success:\n";
  std::cout << "  Checking size of normal map creation... ";
  if (normal_map.size () == 3)
  {
    std::cout << "SUCCESS\n";
  }
  else
  {
    std::cout << "FAIL\n";
  }

  std::cout << "  Checking elements of normal map...\n";

  std::cout << "    Retrieving name...";
  if (normal_map["name"] == "Robert Jenkins")
  {
    std::cout << "SUCCESS\n";
  }
  else
  {
    std::cout << "FAIL\n";
  }

  std::cout << "    Retrieving age...";
  if (normal_map["age"] == Knowledge_Record::Integer (50))
  {
    std::cout << "SUCCESS\n";
  }
  else
  {
    std::cout << "FAIL\n";
  }

  std::cout << "    Retrieving salary...";
  if (normal_map["salary"] == 30500.00)
  {
    std::cout << "SUCCESS\n";
  }
  else
  {
    std::cout << "FAIL\n";
  }

  std::cout << "Adding names to the flex map\n";
  map["norman"]["name"] = "Norman Roberts";
  map["greg"]["name"] = "Gregory Shaw";
  map["rupert"]["name"] = "Rupert Glasgow";
  map["cassie"]["name"] = "Cassandra Collins";

  std::cout << "Attempting to_map function for subkeys\n";
  std::vector<std::string> next_keys;
  std::map<std::string, Madara::Knowledge_Record> all_record_vars;
  knowledge.to_map ("records", ".", "", next_keys, all_record_vars);

  std::cout << "  Size check on next keys and all records are a: ";
  if (next_keys.size () == 5 && all_record_vars.size () == 7)
  {
    std::cout << "SUCCESS\n";
  }
  else
  {
    std::cout << "FAIL\n";
  }

  std::vector<std::string> sub_keys;

  std::cout << "Attempting keys function on Flex_Map\n";
  map.keys (sub_keys, true);

  std::cout << "  Size check on flex map's sub keys: ";
  if (sub_keys.size () == 5)
  {
    std::cout << "SUCCESS\n";
  }
  else
  {
    std::cout << "FAIL\n";
  }

  std::cout << "  Checking context.to_map vs flex_map.keys: ";
  if (sub_keys == next_keys)
  {
    std::cout << "SUCCESS\n";
  }
  else
  {
    std::cout << "FAIL\n";
  }

  std::cout << "Adding mixes of string and index keys to a flex map\n";
  map["robert"]["tickets"][0]["offense"] = "Slapping a seal";
  map["robert"]["tickets"][0]["fine"] = 120.00;
  map["robert"]["tickets"][1]["offense"] = "Calling an employee a hobbit";
  map["robert"]["tickets"][1]["fine"] = 5.00;
  map["cassie"]["tickets"][0]["offense"] = "Stabbing Robert for calling her a hobbit";
  map["cassie"]["tickets"][0]["fine"] = 250.00;
  map["cassie"]["tickets"][0]["notes"] = "Victim Robert claimed it was worth it";

  std::cout << "  Checking existence of new indexed items in flex map: ";
  if (robert_record.exists ("tickets.0.offense")
    && robert_record.exists ("tickets.1.offense")
    && map.exists ("cassie.tickets.0.offense")
    && map.exists ("cassie.tickets.0.notes"))
  {
    std::cout << "SUCCESS\n";
  }
  else
  {
    std::cout << "FAIL\n";
  }

  std::cout << "  Checking existence of subkeys in flex map: ";
  if (robert_record.exists ("tickets", true)
    && robert_record.exists ("name", true)
    && map.exists ("cassie", true)
    && map.exists ("greg", true)
    && map.exists ("norman", true))
  {
    std::cout << "SUCCESS\n";
  }
  else
  {
    std::cout << "FAIL\n";
  }

  std::cout << "Changing delimiter of top level map: \n";
  map.set_delimiter (";");

  std::cout << "  Checking subkeys of top level map: ";
  map.keys (sub_keys, true);

  if (sub_keys.size () == 0)
  {
    std::cout << "SUCCESS\n";
  }
  else
  {
    std::cout << "FAIL\n";
  }


  std::cout << "Resetting delimiter of top level map: \n";
  map.set_delimiter (".");

  std::cout << "  Checking subkeys of top level map: ";
  map.keys (sub_keys, true);

  if (sub_keys.size () != 0)
  {
    std::cout << "SUCCESS\n";
  }
  else
  {
    std::cout << "FAIL\n";
  }
}

void test_vector (void)
{
  std::cout << "************* VECTOR: CREATING VECTOR*************\n";
  engine::Knowledge_Base knowledge;
  containers::Vector vector ("test_vector", knowledge, 10);
  size_t size = vector.size ();

  vector.set (1, "value.at.1");
  vector.set (7, 7.7);

  knowledge.clear_modifieds ();
  
  std::cout << "Vector contents (modified) after clear\n";
  

  vector.set (2, "value.at.2");
  


  std::cout << "Vector results:\n";

  for (size_t i = 0; i < size; ++i)
  {
    std::cout << "  ";
    std::cout << vector[i];
    std::cout << "\n";
  }
  
  std::cout << "\n";

  if (size != 10)
    std::cout << "FAIL. Vector.size != 10\n";
  else
    std::cout << "SUCCESS. Vector.size == 10\n";

  if (vector[1] == "value.at.1" &&
      vector[7] == 7.7)
    std::cout << "SUCCESS. Vector[1] and [7] were set and retrieved.\n";
  else
    std::cout << "FAIL. Vector[1] and [7] were not set and retrieved.\n";
  
  if (vector.get_name () == "test_vector")
    std::cout << "SUCCESS. vector.name () returned test_vector.\n";
  else
    std::cout << "FAIL. vector.name () did not return test_vector.\n";

  std::cout << "Resizing to 7 elements.\n";

  vector.resize (7);

  
  if (vector[1].to_string () == "value.at.1" &&
      knowledge.get ("test_vector.7") != 7.7 && vector.size () == 7)
    std::cout << "SUCCESS. Vector [7] no longer exists.\n";
  else
    std::cout << "FAIL. Vector [7] still exists after resize.\n";
  
  vector.resize (12);
  
  vector.set (8, "value.at.8");
  vector.set (9, "value.at.9");

  if (vector[1] == "value.at.1" &&
      vector[8] == "value.at.8" &&
      vector[9] == "value.at.9" && vector.size () == 12)
    std::cout << "SUCCESS. Resize to 12 was successful.\n";
  else
    std::cout << "FAIL. Resize to 12 was unsuccessful.\n";
  
  std::cout << "Copying records from vector to a knowledge vector.\n";
  std::cout << "Printing resulting knowledge vector.\n";

  vector.set (11, "value.at.11");

  Madara::Knowledge_Vector records;
  vector.copy_to (records);

  for (size_t i = 0; i < records.size (); ++i)
  {
    std::cout << "  " << i << ": " << records[i] << "\n";
  }
  
  std::cout << "Printing results from knowledge base.\n";
  knowledge.print ();
}

void test_map (void)
{
  std::cout << "************* MAP: SETTING KEY:VALUE PAIRS*************\n";
  engine::Knowledge_Base knowledge;
  std::vector <std::string> keys;
  containers::Map map ("test_map", knowledge);
  
  map.set ("name", "Rob Roy");
  map.set ("occupation", "Lord of the MacGregors");
  map.set ("age", Madara::Knowledge_Record::Integer (63));
  map.set ("wives", Madara::Knowledge_Record::Integer (1));

  size_t size = map.size ();
  map.keys (keys);
  
  std::cout << "\nMap results:\n";

  for (std::vector <std::string>::iterator i = keys.begin ();
       i != keys.end (); ++i)
    std::cout << "  " << *i << "=" << map[*i] << "\n";
  
  std::cout << "\n";

  if (size != 4)
    std::cout << "FAIL. Map.size != 4\n";
  else
    std::cout << "SUCCESS. Map.size == 4\n";
  
  if (keys.size () != 4)
    std::cout << "FAIL. Map.keys ().size != 4\n";
  else
    std::cout << "SUCCESS. Map.keys ().size == 4\n";
  
  if (map["name"].to_string () == "Rob Roy" &&
      map["occupation"].to_string () == "Lord of the MacGregors" &&
      map["age"].to_integer () == 63 &&
      map["wives"].to_integer () == 1)
    std::cout << "SUCCESS. map values were set and retrieved.\n";
  else
    std::cout << "FAIL. map values were not set and retrieved.\n";
  
  std::cout << "************* MAP: COPYING MAP FROM KB*************\n";
  containers::Map copy ("test_map", knowledge);
  
  size = map.size ();
  map.keys (keys);
  
  if (size != 4)
    std::cout << "FAIL. Map Copy.size != 4\n";
  else
    std::cout << "SUCCESS. Map Copy.size == 4\n";
  
  if (keys.size () != 4)
    std::cout << "FAIL. Map Copy.keys ().size != 4\n";
  else
    std::cout << "SUCCESS. Map Copy.keys ().size == 4\n";
  
  if (copy["name"].to_string () == "Rob Roy" &&
      copy["occupation"].to_string () == "Lord of the MacGregors" &&
      copy["age"].to_integer () == 63 &&
      copy["wives"].to_integer () == 1)
    std::cout << "SUCCESS. map copy values were set and retrieved.\n";
  else
    std::cout << "FAIL. map copy values were not set and retrieved.\n";

  
  std::cout << "************* MAP: SYNCING KEYS*************\n";

  knowledge.set ("test_map.wife", "Mary Helen MacGregor");

  std::vector <std::string> new_keys (map.sync_keys ());

  if (map.exists ("wife") && !copy.exists ("wife") &&
      new_keys.size () == 1 && new_keys[0] == "wife")
    std::cout << "SUCCESS. map.sync_keys () worked.\n";
  else
    std::cout << "FAIL. map.sync_keys () did not work.\n";

  if (map.get_name () == "test_map" && copy.get_name () == "test_map")
    std::cout << "SUCCESS. map.name () returned test_map.\n";
  else
    std::cout << "FAIL. map.name () did not return test_map.\n";

  knowledge.print ();
}

void test_integer (void)
{
  std::cout << "************* INTEGER: GETTING AND SETTING*************\n";
  engine::Knowledge_Base knowledge;
  containers::Integer my_int ("my_int", knowledge);

  std::cout << "Integer value: " << *my_int << "\n";
  
  std::cout << "Changing value...\n";

  my_int = 15;
  
  std::cout << "Integer value: " << *my_int << "\n";
  
  if (my_int.get_name () == "my_int" && *my_int == 15)
    std::cout << "SUCCESS. my_int was the correct name and value.\n";
  else
    std::cout << "FAIL. my_int was not the correct name and value.\n";

  knowledge.print ();
}

void test_double (void)
{
  std::cout << "************* DOUBLE: GETTING AND SETTING*************\n";
  engine::Knowledge_Base knowledge;
  containers::Double my_double ("my_double", knowledge);

  std::cout << "Double value: " << *my_double << "\n";
  
  std::cout << "Changing value...\n";

  my_double = 3.3;
  
  std::cout << "Double value: " << *my_double << "\n";
  
  if (my_double.get_name () == "my_double" && *my_double == 3.3)
    std::cout << "SUCCESS. my_double was the correct name and value.\n";
  else
    std::cout << "FAIL. my_double was not the correct name and value.\n";

  knowledge.print ();
}

void test_map_exchanges (void)
{
  std::cout << "************* EXCHANGES: MAPS*************\n";
  engine::Knowledge_Base knowledge;
  std::vector <std::string> keys;
  containers::Map map ("first_map", knowledge);
  containers::Map map2 ("second_map", knowledge);
  
  std::cout << "Creating two maps...\n";
  map.set ("a", "key1 contents");
  map.set ("b", "key2 contents");
  map.set ("c", "key3 contents");

  map2.set ("d", "key4 contents");
  map2.set ("e", "key5 contents");
  map2.set ("f", "key6 contents");
  map2.set ("g", "key7 contents");
  
  std::cout << "Contents of map before exchange...\n";
  knowledge.print ();
  
  std::cout << "Exchanging...\n";
  map.exchange (map2);
  
  if (!map.exists ("a") && !map.exists ("b") && !map.exists ("c")
    && !map2.exists ("d") && !map2.exists ("e") && !map2.exists ("f")
    && !map2.exists ("g")
    && map["d"] == "key4 contents" && map["e"] == "key5 contents"
    && map["f"] == "key6 contents"
    && map2["a"] == "key1 contents" && map2["b"] == "key2 contents"
    && map2["c"] == "key3 contents")
  {
    std::cout << "SUCCESS. maps were exchanged.\n";
  }
  else
  {
    std::cout << "FAIL. maps were not exchanged properly.\n";
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
    std::cout << "SUCCESS. maps were exchanged.\n";
  }
  else
  {
    std::cout << "FAIL. maps were not exchanged properly.\n";
  }
  
  knowledge.print ();
}

void test_vector_exchanges (void)
{
  std::cout << "************* EXCHANGES: VECTORS*************\n";
  engine::Knowledge_Base knowledge;
  containers::Vector vector ("first_vector", knowledge, 12);
  containers::Vector vector2 ("second_vector", knowledge, 7);
  
  std::cout << "Creating two vectors...\n";

  vector.set (1, "vector1.1 value");
  vector.set (5, "vector1.5 value");
  vector.set (10, "vector1.10 value");

  vector2.set (2, "vector2.2 value");
  vector2.set (3, "vector2.3 value");
  
  std::cout << "Exchanging...\n";
  vector.exchange (vector2);
  
  if (vector.size () == 7 && vector2.size () == 12
    && vector[2] == "vector2.2 value"
    && vector[3] == "vector2.3 value"
    && vector2[1] == "vector1.1 value"
    && vector2[5] == "vector1.5 value"
    && vector2[10] == "vector1.10 value")
  {
    std::cout << "SUCCESS. vectors were exchanged.\n";
  }
  else
  {
    std::cout << "FAIL. vectors were not exchanged properly.\n";
  }
  
  std::cout << "Exchanging...\n";
  vector.exchange (vector2);
  
  if (vector2.size () == 7 && vector.size () == 12
    && vector2[2] == "vector2.2 value"
    && vector2[3] == "vector2.3 value"
    && vector[1] == "vector1.1 value"
    && vector[5] == "vector1.5 value"
    && vector[10] == "vector1.10 value")
  {
    std::cout << "SUCCESS. vectors were reverted.\n";
  }
  else
  {
    std::cout << "FAIL. vectors were not reverted properly.\n";
  }
  knowledge.print ();
}

void test_vector_transfer ()
{
  std::cout << "************* TRANSFERS: VECTORS*************\n";
  engine::Knowledge_Base knowledge;
  containers::Vector vector ("first_vector", knowledge, 12);
  containers::Vector vector2 ("second_vector", knowledge, 7);
  
  std::cout << "Creating two vectors...\n";

  vector.set (1, "vector1.1 value");
  vector.set (5, "vector1.5 value");
  vector.set (10, "vector1.10 value");

  vector2.set (2, "vector2.2 value");
  vector2.set (3, "vector2.3 value");
  
  std::cout << "Transferring one vector to another...\n";

  vector.transfer_to (vector2);

  knowledge.print ();
}

void test_integer_vector (void)
{
  std::cout << "************* VECTOR: CREATING INTEGER_VECTOR*************\n";
  engine::Knowledge_Base knowledge;
  containers::Integer_Vector vector ("test_vector", knowledge, 10);
  size_t size = vector.size ();

  vector.set (1);
  vector.set (2);
  vector.set (7);

  std::cout << "Vector results:\n";

  for (size_t i = 0; i < size; ++i)
  {
    std::cout << "  ";
    std::cout << vector[i];
    std::cout << "\n";
  }
  
  std::cout << "\n";

  if (size != 10)
    std::cout << "FAIL. Vector.size != 10\n";
  else
    std::cout << "SUCCESS. Vector.size == 10\n";

  if (vector[0] == 0 && vector[1] == 1 && vector[2] == 1 &&
      vector[3] == 0 && vector[4] == 0 && vector[7] == 1)
    std::cout << "SUCCESS. Vector[1],[2],[7] were set and retrieved.\n";
  else
    std::cout << "FAIL. Vector[1],[2],[7] were not set and retrieved.\n";
  
  if (vector.get_name () == "test_vector")
    std::cout << "SUCCESS. vector.name () returned test_vector.\n";
  else
    std::cout << "FAIL. vector.name () did not return test_vector.\n";

  std::cout << "Resizing to 7 elements.\n";

  vector.resize (7);

  
  if (vector[0] == 0 && vector[1] == 1 && vector[2] == 1 &&
      vector[3] == 0 && vector[4] == 0 && vector[7] == 0)
    std::cout << "SUCCESS. Vector [7] no longer exists.\n";
  else
    std::cout << "FAIL. Vector [7] still exists after resize.\n";
  
  vector.resize (12);
  
  vector.set (8);
  vector.set (9);

  if (vector[1] == 1 &&
      vector[8] == 1 && vector[10] == 0 &&
      vector[9] == 1 && vector.size () == 12)
    std::cout << "SUCCESS. Resize to 12 was successful.\n";
  else
    std::cout << "FAIL. Resize to 12 was unsuccessful.\n";

  knowledge.print ();
}

void test_double_vector (void)
{
  std::cout << "************* VECTOR: CREATING DOUBLE_VECTOR*************\n";
  engine::Knowledge_Base knowledge;
  containers::Double_Vector vector ("test_vector", knowledge, 10);
  size_t size = vector.size ();

  vector.set (1, 1.1);
  vector.set (2, 2.2);
  vector.set (7, 7.7);

  std::cout << "Vector results:\n";

  for (size_t i = 0; i < size; ++i)
  {
    std::cout << "  ";
    std::cout << vector[i];
    std::cout << "\n";
  }
  
  std::cout << "\n";

  if (size != 10)
    std::cout << "FAIL. Vector.size != 10\n";
  else
    std::cout << "SUCCESS. Vector.size == 10\n";

  if (vector[0] == 0 && vector[1] == 1.1 && vector[2] == 2.2 &&
      vector[3] == 0 && vector[4] == 0 && vector[7] == 7.7)
    std::cout << "SUCCESS. Vector[1],[2],[7] were set and retrieved.\n";
  else
    std::cout << "FAIL. Vector[1],[2],[7] were not set and retrieved.\n";
  
  if (vector.get_name () == "test_vector")
    std::cout << "SUCCESS. vector.name () returned test_vector.\n";
  else
    std::cout << "FAIL. vector.name () did not return test_vector.\n";

  std::cout << "Resizing to 7 elements.\n";

  vector.resize (7);

  
  if (vector[0] == 0 && vector[1] == 1.1 && vector[2] == 2.2 &&
      vector[3] == 0 && vector[4] == 0 && vector[7] == 0)
    std::cout << "SUCCESS. Vector [7] no longer exists.\n";
  else
    std::cout << "FAIL. Vector [7] still exists after resize.\n";
  
  vector.resize (12);
  
  vector.set (8, 8.8);
  vector.set (9, 9.9);

  if (vector[1] == 1.1 &&
      vector[8] == 8.8 && vector[10] == 0 &&
      vector[9] == 9.9 && vector.size () == 12)
    std::cout << "SUCCESS. Resize to 12 was successful.\n";
  else
    std::cout << "FAIL. Resize to 12 was unsuccessful.\n";

  knowledge.print ();
}

void test_string_vector (void)
{
  std::cout << "************* VECTOR: CREATING STRING_VECTOR*************\n";
  engine::Knowledge_Base knowledge;
  containers::String_Vector vector ("test_vector", knowledge, 10);
  size_t size = vector.size ();

  vector.set (1, "1.1");
  vector.set (2, "2.2");
  vector.set (7, "7.7");

  std::cout << "Vector results:\n";

  for (size_t i = 0; i < size; ++i)
  {
    std::cout << "  ";
    std::cout << vector[i];
    std::cout << "\n";
  }
  
  std::cout << "\n";

  if (size != 10)
    std::cout << "FAIL. Vector.size != 10\n";
  else
    std::cout << "SUCCESS. Vector.size == 10\n";

  if (vector[0] == "0" && vector[1] == "1.1" && vector[2] == "2.2" &&
      vector[3] == "0" && vector[4] == "0" && vector[7] == "7.7")
    std::cout << "SUCCESS. Vector[1],[2],[7] were set and retrieved.\n";
  else
    std::cout << "FAIL. Vector[1],[2],[7] were not set and retrieved.\n";
  
  if (vector.get_name () == "test_vector")
    std::cout << "SUCCESS. vector.name () returned test_vector.\n";
  else
    std::cout << "FAIL. vector.name () did not return test_vector.\n";

  std::cout << "Resizing to 7 elements.\n";

  vector.resize (7);

  
  if (vector[0] == "0" && vector[1] == "1.1" && vector[2] == "2.2" &&
      vector[3] == "0" && vector[4] == "0" && vector[7] == "0")
    std::cout << "SUCCESS. Vector [7] no longer exists.\n";
  else
    std::cout << "FAIL. Vector [7] still exists after resize.\n";
  
  vector.resize (12);
  
  vector.set (8, "8.8");
  vector.set (9, "9.9");

  if (vector[1] == "1.1" &&
      vector[8] == "8.8" && vector[10] == "0" &&
      vector[9] == "9.9" && vector.size () == 12)
    std::cout << "SUCCESS. Resize to 12 was successful.\n";
  else
    std::cout << "FAIL. Resize to 12 was unsuccessful.\n";

  knowledge.print ();
}

void test_native_vectors (void)
{
  std::cout <<
    "************* VECTOR: CREATING NATIVE DOUBLE VECTOR*************\n";
  Madara::Knowledge_Engine::Knowledge_Base knowledge;
  containers::Native_Double_Vector vector ("test_vector", knowledge, 10);
  size_t size = vector.size ();

  vector.set (1, 1.1);
  vector.set (2, 2.2);
  vector.set (7, 7.7);

  std::cout << "Vector results:\n";

  for (size_t i = 0; i < size; ++i)
  {
    std::cout << "  ";
    std::cout << vector[i];
    std::cout << "\n";
  }
  
  std::cout << "\n";

  if (size != 10)
    std::cout << "FAIL. Vector.size != 10\n";
  else
    std::cout << "SUCCESS. Vector.size == 10\n";

  if (vector[0] == 0 && vector[1] == 1.1 && vector[2] == 2.2 &&
      vector[3] == 0 && vector[4] == 0 && vector[7] == 7.7)
    std::cout << "SUCCESS. Vector[1],[2],[7] were set and retrieved.\n";
  else
    std::cout << "FAIL. Vector[1],[2],[7] were not set and retrieved.\n";
  
  if (vector.get_name () == "test_vector")
    std::cout << "SUCCESS. vector.name () returned test_vector.\n";
  else
    std::cout << "FAIL. vector.name () did not return test_vector.\n";

  std::cout << "Resizing to 7 elements.\n";

  vector.resize (7);

  
  if (vector[0] == 0 && vector[1] == 1.1 && vector[2] == 2.2 &&
      vector[3] == 0 && vector[4] == 0 && vector[7] == 0)
    std::cout << "SUCCESS. Vector [7] no longer exists.\n";
  else
    std::cout << "FAIL. Vector [7] still exists after resize.\n";
  
  vector.resize (12);
  
  vector.set (8, 8.8);
  vector.set (9, 9.9);

  if (vector[1] == 1.1 &&
      vector[8] == 8.8 && vector[10] == 0 &&
      vector[9] == 9.9 && vector.size () == 12)
    std::cout << "SUCCESS. Resize to 12 was successful.\n";
  else
    std::cout << "FAIL. Resize to 12 was unsuccessful.\n";

  knowledge.print ();
}

void test_queue (void)
{
  std::cout <<
    "************* QUEUES: Testing multithreaded queues*************\n";
  engine::Knowledge_Base knowledge;
  containers::Queue messages ("queue", knowledge, 7);
  Madara::Knowledge_Record record;
  bool check;

  messages.enqueue ("first string");
  messages.enqueue ("second string");
  messages.enqueue ("third string");
  messages.enqueue ("fourth string");
  messages.enqueue ("fifth string");
  messages.enqueue ("sixth string");
  messages.enqueue ("seventh string");
  check = messages.enqueue ("eighth string");

  if (!check)
    std::cout << "  SUCCESS: enqueue size check.\n";
  else
    std::cout << "  FAIL: enqueue size check.\n";

  record = messages.dequeue ();
  if (record == "first string")
    std::cout << "  SUCCESS: first dequeue.\n";
  else
    std::cout << "  FAIL: first dequeue.\n";
  
  record = messages.dequeue ();
  if (record == "second string")
    std::cout << "  SUCCESS: second dequeue.\n";
  else
    std::cout << "  FAIL: second dequeue.\n";

  messages.enqueue ("eighth string");
  messages.enqueue ("ninth string");
  check = messages.enqueue ("tenth string");
  
  if (!check)
    std::cout << "  SUCCESS: 2nd enqueue size check.\n";
  else
    std::cout << "  FAIL: 2nd enqueue size check.\n";

  record = messages.dequeue ();
  if (record == "third string")
    std::cout << "  SUCCESS: third dequeue.\n";
  else
    std::cout << "  FAIL: third dequeue.\n";
  
  record = messages.dequeue ();
  if (record == "fourth string")
    std::cout << "  SUCCESS: fourth dequeue.\n";
  else
    std::cout << "  FAIL: fourth dequeue.\n";
  
  record = messages.dequeue ();
  if (record == "fifth string")
    std::cout << "  SUCCESS: fifth dequeue.\n";
  else
    std::cout << "  FAIL: fifth dequeue.\n";
  
  record = messages.dequeue ();
  if (record == "sixth string")
    std::cout << "  SUCCESS: sixth dequeue.\n";
  else
    std::cout << "  FAIL: sixth dequeue.\n";
  
  record = messages.dequeue ();
  if (record == "seventh string")
    std::cout << "  SUCCESS: seventh dequeue.\n";
  else
    std::cout << "  FAIL: seventh dequeue.\n";
  
  record = messages.dequeue ();
  if (record == "eighth string")
    std::cout << "  SUCCESS: eighth dequeue.\n";
  else
    std::cout << "  FAIL: eighth dequeue.\n";
  
  record = messages.dequeue ();
  if (record == "ninth string")
    std::cout << "  SUCCESS: ninth dequeue.\n";
  else
    std::cout << "  FAIL: ninth dequeue.\n";
  
  messages.enqueue ("first cleared");
  messages.enqueue ("second cleared");

  if (messages.inspect (0) == "first cleared" &&
      messages.inspect (1) == "second cleared")
  {
    std::cout << "  SUCCESS: inspect on multiple records.\n";
  }
  else
  {
    std::cout << "  FAIL: inspect on multiple records.\n";
  }

  messages.clear ();

  if (messages.count () == 0 && messages.size () == 7)
    std::cout << "  SUCCESS: clear.\n";
  else
    std::cout << "  FAIL: clear.\n";
  
  
  if (!messages.dequeue (false).is_valid ())
  {
    std::cout << "  SUCCESS: asynchronous dequeue empty.\n";
  }
  else
  {
    std::cout << "  FAIL: asynchronous dequeue empty.\n";
  }

  messages.enqueue ("first string");
  messages.enqueue ("second string");
  messages.enqueue ("third string");
  messages.enqueue ("fourth string");
  messages.enqueue ("fifth string");
  messages.enqueue ("sixth string");
  messages.enqueue ("seventh string");

  messages.resize (5);

  if (messages.count () == 5 && 
    messages.count () == messages.size ())
  {
    std::cout << "  SUCCESS: resize.\n";
  }
  else
  {
    std::cout << "  FAIL: resize.\n";
  }

  knowledge.print ();
}

void test_collection (void)
{
  std::cout <<
    "************* COLLECTION: Testing container collections*************\n";

  engine::Knowledge_Base knowledge;
  engine::Thread_Safe_Context & context (knowledge.get_context ());

  unsigned char buffer1[1024];
  unsigned char buffer2[2048];

  std::cout << "  Creating containers...\n";

  containers::Integer age ("age", knowledge);
  containers::String name ("name", knowledge);
  containers::Double salary ("salary", knowledge);
  containers::Native_Double_Vector gps_location ("location.gps", knowledge, 3);
  containers::Native_Integer_Vector years_employed ("employment.years", knowledge);
  containers::Map jobs ("jobs", knowledge);
  containers::Buffer_Vector images ("images", knowledge);
  containers::String_Vector movies ("movies.favorite", knowledge);
  containers::Integer_Vector years ("years.favorite", knowledge);
  containers::Double_Vector coolfactor ("coolfactor.by.year", knowledge, 35);

  std::cout << "  Testing modifieds.size == 0 after container creation... ";

  if (context.get_modifieds ().size () != 0)
  {
    std::cout << "FAIL\n";
    std::cout << "    Printing modified elements in context\n\n";
    std::cout << context.debug_modifieds () << "\n";
  }
  else
  {
    std::cout << "SUCCESS\n";
  }

  std::cout << "  Assigning values to containers...\n";

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

  containers::Collection collection;

  std::cout << "\n  Adding 9 containers to collection container\n";

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

  std::cout << "  Testing collection.size == 9 after adding containers... ";

  if (collection.size () != 9)
  {
    std::cout << "FAIL. Size returned " << collection.size () << "\n";
  }
  else
  {
    std::cout << "SUCCESS\n";
  }

  std::cout << "\n  Printing Collection contents\n\n";
  std::cout << collection.get_debug_info () << "\n";

  std::cout << "  Printing modified elements in context\n\n";
  std::cout << context.debug_modifieds () << "\n";

  std::cout << "  Clearing modified elements in context\n\n";
  knowledge.clear_modifieds ();

  std::cout << "  Testing modifieds.size == 0 after clearing modified... ";

  if (context.get_modifieds ().size () != 0)
  {
    std::cout << "FAIL\n";
    std::cout << "    Printing modified elements in context\n\n";
    std::cout << context.debug_modifieds () << "\n";
  }
  else
  {
    std::cout << "SUCCESS\n";
  }

  std::cout << "  Calling modify on collection\n";
  collection.modify ();

  std::cout << "  Testing modifieds.size == 55 after modifying containers... ";

  if (context.get_modifieds ().size () != 55)
  {
    std::cout << "FAIL\n";
  }
  else
  {
    std::cout << "SUCCESS\n";
  }

  std::cout << "  Printing modified elements in context\n\n";
  std::cout << context.debug_modifieds () << "\n";

  std::cout << "  Clearing modified elements in context\n\n";
  knowledge.clear_modifieds ();

  std::cout << "  Clearing collection\n\n";
  collection.clear ();

  std::cout << "  Testing collection.size == 0 after clearing containers... ";

  if (collection.size () != 0)
  {
    std::cout << "FAIL. Size returned " << collection.size () << "\n";
  }
  else
  {
    std::cout << "SUCCESS\n";
  }

  std::cout << "\nAdding 3 containers to collection container\n";

  collection.add (age);
  collection.add (name);
  collection.add (salary);

  std::cout << "  Testing collection.size == 3 after adding containers... ";

  if (collection.size () != 3)
  {
    std::cout << "FAIL. Size returned " << collection.size () << "\n";
  }
  else
  {
    std::cout << "SUCCESS\n";
  }

  std::cout << "\nPrinting Collection contents\n\n";
  std::cout << collection.get_debug_info () << "\n";

  std::cout << "  Calling modify on collection\n";
  collection.modify ();

  std::cout << "  Testing modifieds.size == 3... ";

  if (context.get_modifieds ().size () != 3)
  {
    std::cout << "FAIL\n";
  }
  else
  {
    std::cout << "SUCCESS\n";
  }

  std::cout << "  Printing modified elements in context\n\n";
  std::cout << context.debug_modifieds () << "\n";
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

  return 0;
}
