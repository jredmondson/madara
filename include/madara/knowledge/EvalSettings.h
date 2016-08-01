#ifndef _MADARA_EVAL_SETTINGS_
#define _MADARA_EVAL_SETTINGS_

/**
 * @file EvalSettings.h
 * @author James Edmondson <jedmondson@gmail.com>
 *
 * This file contains the EvalSettings class, which
 * provides settings for using the eval mechanism in the Knowledge Engine
 */

#include <string>
#include <map>
#include "madara/knowledge/KnowledgeUpdateSettings.h"

namespace madara
{

  namespace knowledge
  {

    /**
     * @class EvalSettings
     * @brief Encapsulates settings for an evaluation statement
     **/
    struct EvalSettings : public KnowledgeUpdateSettings
    {
      /* Convenient pre-defined settings objects */

      /** Settings to immediately send modifieds, but do expand variables */
      static const EvalSettings DEFAULT;

      /** Settings to immediately send modifieds, and not expand variables */
      static const EvalSettings NO_EXPAND;

      /** Settings to delay sending modifieds, but do expand variables */
      static const EvalSettings DELAY;

      /** Settings to delay sending modifieds, and not expand variables */
      static const EvalSettings DELAY_NO_EXPAND;

       /**
        * Constructor
        **/
       EvalSettings ()
         : KnowledgeUpdateSettings (), delay_sending_modifieds (false),
           pre_print_statement (""), post_print_statement ("")
       {
       }

       
       /**
        * Constructor
        * @param  t_delay_sending_modifieds true to send modifieds
        * @param  t_treat_globals_as_locals true if global variables should
        *                                   be marked as not to be sent to the
        *                                   transport
        * @param  t_signal_updates          indicates whether to signal updates
        *                                   to a separate thread that may be
        *                                   waiting. This is only potentially
        *                                   useful to multithreaded MADARA apps
        * @param  t_always_overwrite        always overwrite, despite quality
        * @param  t_always_expand           always try to expand variable names
        * @param  t_track_local_changes     if true, keep track of modifications
        *                                   to local variables for checkpoint
        *                                   saving purposes.
        * @param  t_pre_print_statement     statement to print out before
        *                                   evaluations
        * @param  t_post_print_statement    statement to print out after
        *                                   evaluations
        **/
       EvalSettings (bool t_delay_sending_modifieds,
         bool t_treat_globals_as_locals = false,
         bool t_signal_updates = true,
         bool t_always_overwrite = false,
         bool t_always_expand = true,
         bool t_track_local_changes = false,
         std::string t_pre_print_statement = "",
         std::string t_post_print_statement = "")
         : KnowledgeUpdateSettings (t_treat_globals_as_locals,
                                      t_signal_updates, t_always_overwrite,
                                      t_always_expand, t_track_local_changes),
           delay_sending_modifieds (t_delay_sending_modifieds),
           pre_print_statement (t_pre_print_statement),
           post_print_statement (t_post_print_statement)
       {
       }
       
       /**
        * Copy constructor
        * @param   rhs   Eval settings instance to copy
        **/
       EvalSettings (const EvalSettings & rhs)
         : KnowledgeUpdateSettings (rhs), 
           delay_sending_modifieds (rhs.delay_sending_modifieds),
           pre_print_statement (rhs.pre_print_statement),
           post_print_statement (rhs.post_print_statement),
           send_list (rhs.send_list)
       {
       }

       /**
        * Toggle for sending modifieds in a single update event
        * after each evaluation.
        **/
       bool delay_sending_modifieds;

       /**
        * Statement to print before evaluations
        **/
       std::string pre_print_statement;

       /**
        * Statement to print after evaluations
        **/
       std::string post_print_statement;

       /**
        * Map of record names that are allowed to be sent after operation.
        * The map is only valid if @see delay_sending_modifieds is false.
        **/
       std::map <std::string, bool> send_list;
     };
  }
}
#endif //_MADARA_EVAL_SETTINGS_
