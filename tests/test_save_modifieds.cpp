#include "madara/utility/Utility.h"
#include "madara/knowledge/KnowledgeBase.h"
#include "madara/knowledge/containers/Integer.h"

#include <iostream>

namespace logger = madara::logger;
namespace knowledge = madara::knowledge;
namespace containers = knowledge::containers;
typedef knowledge::KnowledgeRecord   KnowledgeRecord;
typedef KnowledgeRecord::Integer     Integer;

int main (int,char **)
{
  madara_logger_ptr_log (logger::global_logger.get(), logger::LOG_ALWAYS,
    "**** TESTING SAVE MODIFEDS ****.\n");

  knowledge::KnowledgeBase knowledge;

  knowledge::VariableReferences references = knowledge.save_modifieds ();
  knowledge::EvalSettings locals_as_globals, defaults;
  locals_as_globals.treat_locals_as_globals = true;

  madara_logger_ptr_log (logger::global_logger.get(), logger::LOG_ALWAYS,
    "  Testing initial save modifieds (should be 0): %s\n",
    references.size () == 0 ? "SUCCESS" : "FAIL");

  madara_logger_ptr_log (logger::global_logger.get(), logger::LOG_ALWAYS,
    "  Testing modification of global variables with default settings...\n");

  madara_logger_ptr_log (logger::global_logger.get(), logger::LOG_ALWAYS,
    "    Testing saved_modifieds with Int container inits (should be 0): ");

  containers::Integer my_global_1 ("my_global_1", knowledge);
  containers::Integer my_global_2 ("my_global_2", knowledge);
  containers::Integer my_local_1 (".my_local_1", knowledge);
  containers::Integer my_local_2 (".my_local_2", knowledge);

  references = knowledge.save_modifieds ();

  madara_logger_ptr_log (logger::global_logger.get(), logger::LOG_ALWAYS,
    "%s\n",
    references.size () == 0 ? "SUCCESS" : "FAIL");

  knowledge.clear_modifieds ();

  my_global_1 = 1;
  
  references = knowledge.save_modifieds ();

  madara_logger_ptr_log (logger::global_logger.get(), logger::LOG_ALWAYS,
    "  Testing modifieds after 1 global var set: %s\n",
    references.size () == 1 ? "SUCCESS" : "FAIL");

  knowledge.clear_modifieds ();

  my_global_1 = 1;
  my_global_2 = 1;
  
  references = knowledge.save_modifieds ();

  madara_logger_ptr_log (logger::global_logger.get(), logger::LOG_ALWAYS,
    "  Testing modifieds after 2 global var set: %s\n",
    references.size () == 2 ? "SUCCESS" : "FAIL");

  knowledge.clear_modifieds ();

  my_global_1 = 1;
  my_global_2 = 1;
  
  my_local_1 = 1;
  my_local_2 = 1;
  
  madara_logger_ptr_log (logger::global_logger.get(), logger::LOG_ALWAYS,
    "    Checking locals as globals in EvalSettings: %s\n",
    my_local_1.get_settings ().treat_locals_as_globals == false &&
    my_local_2.get_settings ().treat_locals_as_globals == false ? "SUCCESS" : "FAIL");

  references = knowledge.save_modifieds ();

  madara_logger_ptr_log (logger::global_logger.get(), logger::LOG_ALWAYS,
    "  Testing modifieds after 2 global var set, 2 local var set: %s\n",
    references.size () == 2 ? "SUCCESS" : "FAIL");

  knowledge.clear_modifieds ();

  my_local_1.set_settings (locals_as_globals);
  my_local_2.set_settings (locals_as_globals);

  madara_logger_ptr_log (logger::global_logger.get(), logger::LOG_ALWAYS,
    "    Checking locals as globals in EvalSettings: %s\n",
    my_local_1.get_settings ().treat_locals_as_globals == true &&
    my_local_2.get_settings ().treat_locals_as_globals == true ? "SUCCESS" : "FAIL");

  my_global_1 = 1;
  my_global_2 = 1;
  
  my_local_1 = 1;
  my_local_2 = 1;
  
  references = knowledge.save_modifieds ();

  madara_logger_ptr_log (logger::global_logger.get(), logger::LOG_ALWAYS,
    "  Testing modifieds after treat_locals_as_globals = true (%d): %s\n",
    (int)references.size (), references.size () == 4 ? "SUCCESS" : "FAIL");


  knowledge.clear_modifieds ();

  my_local_1.set_settings (defaults);
  my_local_2.set_settings (defaults);

  my_global_1 = 1;
  my_global_2 = 1;
  
  my_local_1 = 1;
  my_local_2 = 1;
  
  references = knowledge.save_modifieds ();

  madara_logger_ptr_log (logger::global_logger.get(), logger::LOG_ALWAYS,
    "  Testing reset local container settings. Only globals should modify "
    " (%d): %s\n",
    (int)references.size (), references.size () == 2 ? "SUCCESS" : "FAIL");



  return 0;
}
