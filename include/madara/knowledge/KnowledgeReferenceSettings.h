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
  KnowledgeReferenceSettings()
   : expand_variables(true), never_exit(false),
     exception_on_unitialized(false)
  {
  }

  /**
   * Constructor
   * @param t_expand_variables  if true, perform variable expansion
   * @param t_exception         if uninitialized variable, throw exception
   **/
  KnowledgeReferenceSettings(bool t_expand_variables, bool t_exception=false)
    : expand_variables(t_expand_variables), never_exit(false),
     exception_on_unitialized(t_exception)
  {
  }

  /**
   * Copy constructor
   * @param rhs                 reference to copy
   **/
  KnowledgeReferenceSettings(const KnowledgeReferenceSettings& rhs)
    : expand_variables(rhs.expand_variables), never_exit(rhs.never_exit),
      exception_on_unitialized(rhs.exception_on_unitialized)
  {
  }

  /**
   * Destructor
   **/
  ~KnowledgeReferenceSettings() {}

  /**
   * Toggle for always attempting to expand variables (true) or never
   * expanding variables (false)
   **/
  bool expand_variables;

  /**
   * Never allow MADARA to exit, even with fatal errors or invalid state
   **/
  bool never_exit;

  /// throw an exception if reference is on uninitialized variable
  bool exception_on_unitialized;
};
}
}
#endif  // _MADARA_KNOWLEDGE_REFERENCE_SETTINGS_H_
