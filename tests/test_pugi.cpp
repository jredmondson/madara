
#include <pugi/pugixml.hpp>
#include <vector>
#include <iostream>
#include <sstream>
#include <stdlib.h>

void handle_arguments (int argc, char ** argv)
{
  for (int i = 1; i < argc; ++i)
  {
    std::string arg1 (argv[i]);
    
    if (argc > 1)
    {
      std::cout <<
        "\nProgram Summary for " << argv[0] << ":\n\n" \
        "  This stand-alone application tests functions from pugi\n"
        "  xml functions.\n"
        "\n";
      exit (0);
    }
  }
}

struct simple_walker : pugi::xml_tree_walker
{
  virtual bool for_each (pugi::xml_node& node)
  {
    // calculate indentation and save it
    std::string indent (depth () * 2, ' ');

    // print the indentation and node name
    std::cout << indent << node.name () << "\n";

    if (strcmp (node.value (), "") != 0)
    {
      std::cout << " = " << node.value () << "\n";
    }

    for (pugi::xml_attribute attr : node.attributes ())
    {
      std::cout << indent << "  " << attr.name () << "=" << attr.value () << "\n";
    }

    return true; // continue traversal
  }
};

void karlify (std::ostream & output, const std::string & prefix, const pugi::xml_node & node)
{
  std::string nodename;

  pugi::xml_attribute prefix_override = node.attribute ("prefix");

  if (!prefix_override.empty ())
  {
    nodename = prefix_override.value ();
  }
  else if (prefix != "")
  {
    if (strcmp (node.name (), "") != 0)
    {
      nodename = prefix + "." + node.name ();
    }
    else
    {
      nodename = prefix;
    }
  }
  else if (strcmp (node.name (), "") != 0)
  {
    nodename = node.name ();
  }

  if (strcmp (node.value (), "") != 0)
  {
    output << nodename << " = \"" << node.value () << "\";\n";
  }

  for (pugi::xml_attribute attr : node.attributes ())
  {
    output << nodename << "." << attr.name () << "=\"" << attr.value () << "\";\n";
  }

  for (pugi::xml_node child : node.children ())
  {
    karlify (output, nodename, child);
  }
}

void test_simple_xml (void)
{
  std::string directory (getenv ("MADARA_ROOT"));
  directory += "/tests/xml";

  std::string filename;
  pugi::xml_document doc;
  pugi::xml_parse_result result;
  simple_walker walker;

  filename = directory + "/agentinfo.xml";
  result = doc.load_file (filename.c_str ());
  std::cout << "\n" << filename << "\n";
  std::cout << "Load result: " << result.description () << "\n";
  std::cout << "status: " << result.status << "\n";

  doc.traverse (walker);
  std::cout << "karlify:\n";
  karlify (std::cout, "", doc.root ());

  doc.reset ();

  filename = directory + "/character.xml";
  result = doc.load_file (filename.c_str ());
  std::cout << "\n" << filename << "\n";
  std::cout << "Load result: " << result.description () << "\n";
  std::cout << "status: " << result.status << "\n";

  doc.traverse (walker);
  std::cout << "karlify:\n";
  karlify (std::cout, "", doc.root ());

  doc.reset ();

  filename = directory + "/tree.xml";
  result = doc.load_file (filename.c_str ());
  std::cout << "\n" << filename << "\n";
  std::cout << "Load result: " << result.description () << "\n";
  std::cout << "status: " << result.status << "\n";

  doc.traverse (walker);
  std::cout << "karlify:\n";
  karlify (std::cout, "", doc.root ());


  doc.reset ();

  filename = directory + "/xgconsole.xml";
  result = doc.load_file (filename.c_str ());
  std::cout << "\n" << filename << "\n";
  std::cout << "Load result: " << result.description () << "\n";
  std::cout << "status: " << result.status << "\n";

  doc.traverse (walker);
  std::cout << "karlify:\n";
  karlify (std::cout, "", doc.root ());


  doc.reset ();

}

int main (int argc, char ** argv)
{
  handle_arguments (argc, argv);

  test_simple_xml ();

  return 0;
}
