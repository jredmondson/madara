#ifndef _MADARA_KNOWLEDGE_REFERENCE_SETTINGS_H_
#define _MADARA_KNOWLEDGE_REFERENCE_SETTINGS_H_

/**
 * @file KnowledgeBase.h
 * @author James Edmondson <jedmondson@gmail.com>
 *
 * This file contains the KnowledgeReferenceSettings class, which
 * provides settings for applying knowledge updates
 */

#include "madara/MadaraExport.h"
#include "madara/utility/StdInt.h"

namespace madara
{
  namespace knowledge
  {
    /**
     * Settings for applying knowledge updates
     **/
    class MADARA_EXPORT KnowledgeReferenceSettings
    {
    public:
      /* Convenient pre-defined settings objects */
      static const KnowledgeReferenceSettings DEFAULT;
      static const KnowledgeReferenceSettings NO_EXPAND;

      /**
       * Constructor
       **/
      KnowledgeReferenceSettings ()
        : expand_variables (true), never_exit (false)
      {
      }
  
      /**
       * Constructor
       **/
      KnowledgeReferenceSettings (bool t_expand_variables)
        : expand_variables (t_expand_variables), never_exit (false)
      {
      }
      
      /**
       * Copy constructor
       **/
      KnowledgeReferenceSettings (const KnowledgeReferenceSettings & rhs)
        : expand_variables (rhs.expand_variables), never_exit (rhs.never_exit)
      {
      }


      /**
       * Destructor
       **/
      ~KnowledgeReferenceSettings ()
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
#endif // _MADARA_KNOWLEDGE_REFERENCE_SETTINGS_H_
