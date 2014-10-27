
/**
 * @file hello_world.cpp
 * @author James Edmondson <jedmondson@gmail.com>
 *
 * This file contains a simple application that evaluates a user input
 * and performs actions based on that input.
 **/

#include "madara/knowledge_engine/Knowledge_Base.h"
#include <iostream>
#include <string>

int main (int argc, char * argv[])
{
  Madara::Knowledge_Engine::Knowledge_Base knowledge;
  
  // Prompt the user for their height
  std::string my_height;
  knowledge.print ("How tall are you in inches?: ");
  std::getline (std::cin, my_height);

  // Save the height to the ".height" variable
  knowledge.set (".height", my_height);

  /**
   * Try to deduce what the person should be doing based on three famous
   * figures. Obviously, this is a sure fire prediction!"
   **/
  knowledge.evaluate (
    ".Lebron_James_height = 80;"
    ".Napoleon_Bonaparte_height = 66;"
    ".Danny_DeVito_height = 60;"

    /* if the user height is less than Danny DeVito's height, then they
     * should shoot for being an Actor */
    "(.height < .Danny_DeVito_height => "
    "  .job = 'an actor'"
    ")"

    // otherwise, maybe some other condition holds true (|| means "or")
    "||"

    /* OK, how about if they are just shorter than Napoleon, they should
     * try conquering all of Europe */
    "(.height < .Napoleon_Bonaparte_height =>"
    "  .job = 'a conqueror of Europe'"
    ")"
    
    // otherwise, maybe some other condition holds true (|| means "or")
    "||"

    /* OK, well if you're taller than Lebron James, then just do whatever
     * you want." */
    "(.height > .Lebron_James_height =>"
    "  .job = 'whatever the heck you want to be. Just dont beat the rest' + "
    "         ' of us up.'"
    ")"
    
    // otherwise, maybe some other condition holds true (|| means "or")
    "||"
    
    /* This condition covers the range between Napoleon and Lebron James." */
    "("
    "  .job = 'a basketball player.'"
    ")"

    /* In MADARA KaRL, a semicolon ";" separates two expressions that must
     * always be evaluated and is useful to separate two types of knowledge
     * operations that must always be performed. In this case, we just
     * determined the future job of the user. Now, we should print what they
     * should be. */

    ";"

    "#print ('As a particularly bad career counsellor, I recommend that you '"
    "        + 'become ' + .job)"

    );

  return 0;
}
