
#ifndef _MADARA_DOUBLE_H_
#define _MADARA_DOUBLE_H_

#include <vector>
#include <string>
#include "madara/LockType.h"
#include "madara/knowledge/KnowledgeBase.h"
#include "madara/knowledge/ThreadSafeContext.h"
#include "madara/knowledge/KnowledgeUpdateSettings.h"

#include "BaseContainer.h"

/**
 * @file Double.h
 * @author James Edmondson <jedmondson@gmail.com>
 *
 * This file contains a C++ object that manages interactions for a
 * double inside of a variable context
 **/

namespace madara
{
namespace knowledge
{
namespace containers
{
/**
 * @class Double
 * @brief This class stores a double within a variable context
 */
class MADARA_EXPORT Double : public BaseContainer
{
public:
  /// trait that describes the value type
  typedef double type;

  /**
   * Default constructor
   * @param  settings   settings for updating knowledge
   **/
  Double(const KnowledgeUpdateSettings& settings = KnowledgeUpdateSettings());

  /**
   * Constructor
   * @param  name       name of the integer in the knowledge base
   * @param  knowledge  the knowledge base that will contain the vector
   * @param  settings   settings for updating knowledge
   **/
  Double(const std::string& name, KnowledgeBase& knowledge,
      const KnowledgeUpdateSettings& settings = KnowledgeUpdateSettings());

  /**
   * Constructor
   * @param  name      the name of the map within the variable context
   * @param  knowledge the variable context
   * @param  settings  settings for updating knowledge
   **/
  Double(const std::string& name, Variables& knowledge,
      const KnowledgeUpdateSettings& settings = KnowledgeUpdateSettings());

  /**
   * Default constructor
   * @param  name       name of the integer in the knowledge base
   * @param  knowledge  the knowledge base that will contain the vector
   * @param  value      new value of the variable in the knowledge base
   * @param  settings   settings for updating knowledge
   **/
  Double(const std::string& name, KnowledgeBase& knowledge, type value,
      const KnowledgeUpdateSettings& settings = KnowledgeUpdateSettings());

  /**
   * Default constructor
   * @param  name       name of the integer in the knowledge base
   * @param  knowledge  the knowledge base that will contain the vector
   * @param  value      new value of the variable in the knowledge base
   * @param  settings   settings for updating knowledge
   **/
  Double(const std::string& name, Variables& knowledge, type value,
      const KnowledgeUpdateSettings& settings = KnowledgeUpdateSettings());

  /**
   * Copy constructor
   **/
  Double(const Double& rhs);

  /**
   * Destructor
   **/
  virtual ~Double();

  /**
   * Mark the value as modified. The Double retains the same value
   * but will resend its value as if it had been modified.
   **/
  void modify(void);

  /**
   * Assignment operator
   * @param  rhs    value to copy
   **/
  void operator=(const Double& rhs);

  /**
   * Sets the variable name that this refers to
   * @param var_name  the name of the variable in the knowledge base
   * @param knowledge  the knowledge base the variable is housed in
   **/
  void set_name(const std::string& var_name, KnowledgeBase& knowledge);

  /**
   * Sets the variable name that this refers to
   * @param var_name  the name of the variable in the knowledge base
   * @param knowledge  the knowledge base the variable is housed in
   **/
  void set_name(const std::string& var_name, Variables& knowledge);

  /**
   * Sets the variable name that this refers to
   * @param var_name  the name of the variable in the knowledge base
   * @param knowledge  the knowledge base the variable is housed in
   **/
  void set_name(const std::string& var_name, ThreadSafeContext& knowledge);

  /**
   * Exchanges the integer at this location with the integer at another
   * location.
   * @param  other   the other integer to exchange with
   **/
  void exchange(containers::Double& other);

  /**
   * Sets the value of the variable
   * @param  value  the new value of the variable
   * @return the updated value (should be same as value param)
   **/
  type operator=(type value);

  /**
   * Returns the value of the variable
   * @return the value of the variable
   **/
  type operator*(void)const;

  /**
   * Increments by a value
   * @param  value  the value to add
   * @return the new value
   **/
  type operator+=(type value);

  /**
   * Decrements by a value
   * @param  value  the value to remove
   * @return the new value
   **/
  type operator-=(type value);

  /**
   * Increments the value of the variable and returns
   * the result.
   * @return the new value of the variable
   **/
  type operator++(void);

  /**
   * Decrements the value of the variable and returns
   * the result.
   * @return the new value of the variable
   **/
  type operator--(void);

  /**
   * Checks to see if the variable has ever been assigned a value
   * @return true if the record has been set to a value. False if
   *         uninitialized
   **/
  bool exists(void) const;

  /**
   * Checks for equality
   * @param  value  the value to compare to
   * @return true if equal, false otherwise
   **/
  bool operator==(type value) const;

  /**
   * Checks for equality
   * @param  value  the value to compare to
   * @return true if equal, false otherwise
   **/
  bool operator==(const Double& value) const;

  /**
   * Checks for inequality
   * @param  value  the value to compare to
   * @return true if inequal, false otherwise
   **/
  bool operator!=(type value) const;

  /**
   * Checks for inequality
   * @param  value  the value to compare to
   * @return true if inequal, false otherwise
   **/
  bool operator!=(const Double& value) const;

  /**
   * Checks for less than relationship
   * @param  value  the value to compare to
   * @return true if less than
   **/
  bool operator<(type value) const;

  /**
   * Checks for less than or equal to relationship
   * @param  value  the value to compare to
   * @return true if less than or equal to
   **/
  bool operator<=(type value) const;

  /**
   * Checks for greater than relationship
   * @param  value  the value to compare to
   * @return true if greater than
   **/
  bool operator>(type value) const;

  /**
   * Checks for greater than or equal to relationship
   * @param  value  the value to compare to
   * @return true if greater than or equal to
   **/
  bool operator>=(type value) const;

  /**
   * Returns the value as an integer
   * @return the value as an integer
   **/
  knowledge::KnowledgeRecord::Integer to_integer(void) const;

  /**
   * Returns the value as a knowledge::KnowledgeRecord. This
   * is useful for referencing clock and other record info.
   * @return the value as a knowledge::KnowledgeRecord
   **/
  knowledge::KnowledgeRecord to_record(void) const;

  /**
   * Returns the value as a double (alias of *)
   * @return the value as a double
   **/
  double to_double(void) const;

  /**
   * Returns the value as a string
   * @return the value as a string
   **/
  std::string to_string(void) const;

  /**
   * Sets the quality of writing to the variable
   *
   * @param quality         quality of writing to this location
   * @param settings        settings for referring to knowledge variables
   **/
  void set_quality(uint32_t quality,
      const KnowledgeReferenceSettings& settings = KnowledgeReferenceSettings(
          false));

  /**
   * Returns the type of the container along with name and any other
   * useful information. The provided information should be useful
   * for developers wishing to debug container operations, especially
   * as it pertains to pending network operations (i.e., when used
   * in conjunction with modify)
   *
   * @return info in format {container}: {name}{ = value, if appropriate}
   **/
  std::string get_debug_info(void);

  /**
   * Clones this container
   * @return  a deep copy of the container that must be managed
   *          by the user (i.e., you have to delete the return value)
   **/
  virtual BaseContainer* clone(void) const;

  /**
   * Determines if the value is true
   * @return true if the values is not zero
   **/
  bool is_true(void) const;

  /**
   * Determines if the value is zero
   * @return true if the value is zero
   **/
  bool is_false(void) const;

private:
  /**
   * Polymorphic is true method which can be used to determine if
   * all values in the container are true
   **/
  virtual bool is_true_(void) const;

  /**
   * Polymorphic is false method which can be used to determine if
   * at least one value in the container is false
   **/
  virtual bool is_false_(void) const;

  /**
   * Polymorphic modify method used by collection containers. This
   * method calls the modify method for this class. We separate the
   * faster version (modify) from this version (modify_) to allow
   * users the opportunity to have a fastery version that does not
   * use polymorphic functions (generally virtual functions are half
   * as efficient as normal function calls)
   **/
  virtual void modify_(void);

  /**
   * Returns the type of the container along with name and any other
   * useful information. The provided information should be useful
   * for developers wishing to debug container operations, especially
   * as it pertains to pending network operations (i.e., when used
   * in conjunction with modify)
   *
   * @return info in format {container}: {name}{ = value, if appropriate}
   **/
  virtual std::string get_debug_info_(void);

  /**
   * Variable context that we are modifying
   **/
  ThreadSafeContext* context_;

  /**
   * Variable reference
   **/
  VariableReference variable_;
};
}
}
}

#endif  // _MADARA_DOUBLE_H_