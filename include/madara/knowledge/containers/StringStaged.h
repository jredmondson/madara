
#ifndef _MADARA_KNOWLEDGE_CONTAINERS_STRINGSTAGED_H_
#define _MADARA_KNOWLEDGE_CONTAINERS_STRINGSTAGED_H_

#include <vector>
#include <string>
#include "madara/LockType.h"
#include "madara/knowledge/KnowledgeBase.h"
#include "madara/knowledge/ThreadSafeContext.h"
#include "madara/knowledge/KnowledgeUpdateSettings.h"
#include "BaseContainer.h"

/**
 * @file StringStaged.h
 * @author James Edmondson <jedmondson@gmail.com>
 *
 * This file contains a C++ object that manages interactions for an Integer
 * through staged reads and writes from a knowledge base
 **/

namespace madara
{
namespace knowledge
{
namespace containers
{
/**
 * @class StringStaged
 * @brief Stages a string value to and from the knowledge base. This
 *        container is highly optimized for using a staged value (an
 *        intermediate value between the user application and the
 *        knowledge base) for common operations and then writing that
 *        staged value to the knowledge base after operations are
 *        complete with the write method. The value is only updated
 *        from the knowledge base on construction and when read is
 *        called.
 */
class MADARA_EXPORT StringStaged : public BaseContainer
{
public:
  /// trait that describes the value type
  typedef std::string type;

  /**
   * Default constructor
   * @param  settings   settings for updating knowledge
   **/
  StringStaged(
      const KnowledgeUpdateSettings& settings = KnowledgeUpdateSettings());

  /**
   * Constructor
   * @param  name       name of the integer in the knowledge base
   * @param  knowledge  the knowledge base that will contain the vector
   * @param  settings   settings for updating knowledge
   **/
  StringStaged(const std::string& name, KnowledgeBase& knowledge,
      const KnowledgeUpdateSettings& settings = KnowledgeUpdateSettings());

  /**
   * Constructor
   * @param  name      the name of the map within the variable context
   * @param  knowledge the variable context
   * @param  settings  settings for updating knowledge
   **/
  StringStaged(const std::string& name, Variables& knowledge,
      const KnowledgeUpdateSettings& settings = KnowledgeUpdateSettings());

  /**
   * Default constructor
   * @param  name       name of the integer in the knowledge base
   * @param  knowledge  the knowledge base that will contain the vector
   * @param  value      new value of the variable in the knowledge base
   * @param  settings   settings for updating knowledge
   **/
  StringStaged(const std::string& name, KnowledgeBase& knowledge,
      const type& value,
      const KnowledgeUpdateSettings& settings = KnowledgeUpdateSettings());

  /**
   * Default constructor
   * @param  name       name of the integer in the knowledge base
   * @param  knowledge  the knowledge base that will contain the vector
   * @param  value      new value of the variable in the knowledge base
   * @param  settings   settings for updating knowledge
   **/
  StringStaged(const std::string& name, Variables& knowledge, const type& value,
      const KnowledgeUpdateSettings& settings = KnowledgeUpdateSettings());

  /**
   * Copy constructor
   **/
  StringStaged(const StringStaged& rhs);

  /**
   * Destructor
   **/
  virtual ~StringStaged();

  /**
   * Mark the value as modified. The StringStaged retains the same value
   * but will resend its value as if it had been modified.
   **/
  void modify(void);

  /**
   * Assignment operator
   * @param  rhs    value to copy
   **/
  void operator=(const StringStaged& rhs);

  /**
   * Reads the value from the knowledge base
   **/
  void read(void);

  /**
   * Writes the value to the knowledge base
   **/
  void write(void);

  /**
   * Sets the variable name that this refers to
   * @param var_name  the name of the variable in the knowledge base
   * @param knowledge  the knowledge base the variable is housed in
   * @param sync     synchronize the value from the underlying knowledge
   **/
  void set_name(
      const std::string& var_name, KnowledgeBase& knowledge, bool sync = true);

  /**
   * Sets the variable name that this refers to
   * @param var_name  the name of the variable in the knowledge base
   * @param knowledge  the knowledge base the variable is housed in
   * @param sync     synchronize the value from the underlying knowledge
   **/
  void set_name(
      const std::string& var_name, Variables& knowledge, bool sync = true);

  /**
   * Sets the variable name that this refers to
   * @param var_name  the name of the variable in the knowledge base
   * @param knowledge  the knowledge base the variable is housed in
   * @param sync     synchronize the value from the underlying knowledge
   **/
  void set_name(const std::string& var_name, ThreadSafeContext& knowledge,
      bool sync = true);

  /**
   * Exchanges the integer at this location with the integer at another
   * location.
   * @param  other   the other integer to exchange with
   **/
  void exchange(StringStaged& other);

  /**
   * Sets the value of the variable
   * @param  value  the new value of the variable
   * @return the updated value (should be same as value param)
   **/
  type operator=(const type& value);

  /**
   * Checks for equality
   * @param  value  the value to compare to
   * @return true if equal, false otherwise
   **/
  bool operator==(const StringStaged& value) const;

  /**
   * Checks for inequality
   * @param  value  the value to compare to
   * @return true if inequal, false otherwise
   **/
  bool operator!=(const StringStaged& value) const;

  /**
   * Checks for equality
   * @param  value  the value to compare to
   * @return true if equal, false otherwise
   **/
  bool operator==(const type& value) const;

  /**
   * Checks for inequality
   * @param  value  the value to compare to
   * @return true if inequal, false otherwise
   **/
  bool operator!=(const type& value) const;

  /**
   * Checks for less than relationship
   * @param  value  the value to compare to
   * @return true if less than
   **/
  bool operator<(const type& value) const;

  /**
   * Checks for less than or equal to relationship
   * @param  value  the value to compare to
   * @return true if less than or equal to
   **/
  bool operator<=(const type& value) const;

  /**
   * Checks for greater than relationship
   * @param  value  the value to compare to
   * @return true if greater than
   **/
  bool operator>(const type& value) const;

  /**
   * Checks for greater than or equal to relationship
   * @param  value  the value to compare to
   * @return true if greater than or equal to
   **/
  bool operator>=(const type& value) const;

  /**
   * Returns the value of the variable
   * @return the value of the variable
   **/
  type operator*(void)const;

  /**
   * Checks to see if the variable has ever been assigned a value
   * @return true if the record has been set to a value. False if
   *         uninitialized
   **/
  bool exists(void) const;

  /**
   * Increments by a value
   * @param  value  the value to add
   * @return the new value
   **/
  type operator+=(const type& value);

  /**
   * Decrements by a value
   * @param  value  the value to remove
   * @return the new value
   **/
  type operator-=(const type& value);

  /**
   * Returns the value as a knowledge::KnowledgeRecord. This
   * is useful for referencing clock and other record info.
   * @return the value as a knowledge::KnowledgeRecord
   **/
  knowledge::KnowledgeRecord to_record(void) const;

  /**
   * Returns the value as a double
   * @return the value as a double
   **/
  double to_double(void) const;

  /**
   * Returns the value as an integer (same as *)
   * @return the value as an integer
   **/
  knowledge::KnowledgeRecord::Integer to_integer(void) const;

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

  /**
   * The staged value
   **/
  type value_;

  /**
   * Tracks if value_ has changed since last read
   **/
  bool has_changed_;
};
}  // containers
}  // knowledge
}  // madara

#include "StringStaged.inl"

#endif  // _MADARA_KNOWLEDGE_CONTAINERS_STRINGSTAGED_H_
