
/**
 * @file files.cpp
 * @author James Edmondson <jedmondson@gmail.com>
 *
 * This file contains a simple file reading application
 * that prompts a user for a file and then reads the file into
 * the knowledge base before printing the file size and type.
 **/

#include "madara/knowledge_engine/Knowledge_Base.h"
#include <iostream>
#include <string>

int main (int argc, char * argv[])
{
  /**
   * Create a default knowledge base. If we wanted to share the file,
   * we would need to pass the knowledge base a networking transport.
   * To see an example of this, look at the tutorial on distributed
   * knowledge. The counting_agents.cpp file is a good start.
   **/
  Madara::Knowledge_Engine::Knowledge_Base knowledge;
  
  // Prompt the user for their name
  std::string my_file;
  knowledge.print ("Please enter a file to read into the knowledge base: ");
  std::getline (std::cin, my_file);

  /**
   * Read the file into the .file variable in the knowledge base. Note that
   * if you make a distributed application and want to transfer the file to
   * all agents on the network transport, you'll want to change the variable
   * to a global one by removing the period. A period in front of a variable
   * name means that the variable is local information only.
   *
   * We can also use the system call #read_file to read a file into the engine
   * within a KaRL logic. Similarly, we could call #write_file from within
   * a logic to write a variable to a file. See #print_system_calls () for
   * a listing of all available system calls. Alternatively, you can run
   * system_calls from MADARA_ROOT/bin.
   **/
  knowledge.read_file (".file", my_file);
  knowledge.set (".filename", my_file);
  
  /**
   * When performing an evaluate call, we can ask the knowledge engine to
   * print a statement before or after the evaluate is completed. We harness
   * the post_print_statement setting to print out information about the file
   **/
  Madara::Knowledge_Engine::Eval_Settings eval_settings;
  eval_settings.post_print_statement = 
    "  Filename: {.filename}\n"
    "    Size: {.file.size}\n"
    "    Type: {.file.type.string}\n";


  knowledge.evaluate (
    // Set .file.size to the size of the file we read
    ".file.size = #size (.file);"

    /**
     * Set the .file.type variable to the integer type of the file. See
     * #print_system_calls () or #print_system_call ('#type') for more
     * information on types.
     **/
    ".file.type = #type (.file);"
    
    /**
     * Take the integer form of the types and convert them to strings
     **/

     /**
      * If file.type is 3, then the type is unknown
      **/
     "(.file.type == 3 => .file.type.string = 'Unknown')"

     // or
     "||"
     
     /**
      * If file.type is 4, then the type is XML
      **/
     "(.file.type == 4 => .file.type.string = 'XML')"
     
     // or
     "||"
     
     /**
      * If file.type is 5, then the type is Text file
      **/
     "(.file.type == 5 => .file.type.string = 'Text')"
     
     // or
     "||"
     
     /**
      * If file.type is 50, then the type is a JPEG image
      **/
     "(.file.type == 50 => .file.type.string = 'JPEG')",
     eval_settings);

  return 0;
}
