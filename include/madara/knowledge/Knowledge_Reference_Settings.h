#ifndef _KNOWLEDGE_REFERENCE_SETTINGS_H_
#define _KNOWLEDGE_REFERENCE_SETTINGS_H_

/**
 * @file Knowledge_Base.h
 * @author James Edmondson <jedmondson@gmail.com>
 *
 * This file contains the Knowledge_Reference_Settings class, which
 * provides settings for applying knowledge updates
 */

#include "madara/MADARA_export.h"
#include "madara/utility/stdint.h"

namespace madara
{
  namespace knowledge
  {
    /**
     * Settings for applying knowledge updates
     **/
    class MADARA_Export Knowledge_Reference_Settings
    {
    public:
      /**
       * Constructor
       **/
      Knowledge_Reference_Settings ()
        : expand_variables (true), never_exit (false)
      {
      }
  
      /**
       * Constructor
       **/
      Knowledge_Reference_Settings (bool t_expand_variables)
        : expand_variables (t_expand_variables), never_exit (false)
      {
      }
      
      /**
       * Copy constructor
       **/
      Knowledge_Reference_Settings (const Knowledge_Reference_Settings & rhs)
        : expand_variables (rhs.expand_variables), never_exit (rhs.never_exit)
      {
      }


      /**
       * Destructor
       **/
      ~Knowledge_Reference_Settings ()
      {
      }

      /**
      * Toggle for always attempting to expand variables (true) or never
      * expanding variables (false)
      **/
      bool expand_variables;

      /**
       * Never allow MADARA to exit, even with fatal errors or invalid state
       **/
      bool never_exit;
    };
  }
}
#endif
