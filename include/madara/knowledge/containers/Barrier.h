
#ifndef _MADARA_CONTAINERS_BARRIER_H_
#define _MADARA_CONTAINERS_BARRIER_H_

#include <vector>
#include <string>
#include "madara/LockType.h"
#include "madara/knowledge/KnowledgeBase.h"
#include "madara/knowledge/ThreadSafeContext.h"
#include "madara/knowledge/KnowledgeUpdateSettings.h"
#include "BaseContainer.h"

/**
 * @file Barrier.h
 * @author James Edmondson <jedmondson@gmail.com>
 *
 * This file contains a distributed barrier that may be
 * updated and read by many nodes
 **/

namespace madara
{
namespace knowledge
{
namespace containers
{
/**
 * @class Barrier
 * @brief This class stores an integer within a variable context
 */
class MADARA_EXPORT Barrier : public BaseContainer
{
public:
  /// trait that describes the value type
  typedef knowledge::KnowledgeRecord::Integer type;

  /**
   * Default constructor
   **/
  Barrier(const KnowledgeUpdateSettings& settings = KnowledgeUpdateSettings());

  /**
   * Constructor
   * @param  name       name of the integer in the knowledge base
   * @param  knowledge  the knowledge base that will contain the vector
   * @param  settings   settings for evaluating the vector
   **/
  Barrier(const std::string& name, KnowledgeBase& knowledge,
      const KnowledgeUpdateSettings& settings = KnowledgeUpdateSettings());

  /**
   * Constructor
   * @param  name      the name of the map within the variable context
   * @param  knowledge the variable context
   * @param  settings  settings to apply by default
   **/
  Barrier(const std::string& name, Variables& knowledge,
      const KnowledgeUpdateSettings& settings = KnowledgeUpdateSettings());

  /**
   * Default constructor
   * @param  name       name of the integer in the knowledge base
   * @param  knowledge  the knowledge base that will contain the vector
   * @param  id         the id of the barrier in the barrier ring
   * @param  participants   the number of participants in the barrier ring
   * @param  settings   settings for evaluating the vector
   **/
  Barrier(const std::string& name, KnowledgeBase& knowledge, int id,
      int participants,
      const KnowledgeUpdateSettings& settings = KnowledgeUpdateSettings());

  /**
   * Default constructor
   * @param  name       name of the integer in the knowledge base
   * @param  knowledge  the knowledge base that will contain the vector
   * @param  id         the id of the barrier in the barrier ring
   * @param  participants  the number of participants in the barrier ring
   * @param  settings   settings for evaluating the vector
   **/
  Barrier(const std::string& name, Variables& knowledge, int id,
      int participants,
      const KnowledgeUpdateSettings& settings = KnowledgeUpdateSettings());

  /**
   * Copy constructor
   **/
  Barrier(const Barrier& rhs);

  /**
   * Destructor
   **/
  ~Barrier();

  /**
   * Assignment operator
   * @param  rhs    value to copy
   **/
  void operator=(const Barrier& rhs);

  /**
   * Returns the id of the barrier in the barrier ring
   * @return the id of the barrier
   **/
  size_t get_id(void) const;

  /**
   * Returns the number of participants in the barrier ring
   * @return the number of participants barriering
   **/
  size_t get_participants(void) const;

  /**
   * Returns the current barrier round
   * @return the current barrier round number
   **/
  inline type get_round(void) const
  {
    return to_integer();
  }

  /**
   * Sets the barrier to a specific round
   * @return the barrier round number
   **/
  void set(type value);

  /**
   * Sets the variable name that this refers to
   * @param var_name  the name of the variable in the knowledge base
   * @param knowledge  the knowledge base the variable is housed in
   * @param id         the id of the barrier in the barrier ring
   * @param participants  the number of participants in the barrier ring
   **/
  void set_name(const std::string& var_name, KnowledgeBase& knowledge, int id,
      int participants);

  /**
   * Sets the variable name that this refers to
   * @param var_name  the name of the variable in the knowledge base
   * @param knowledge  the knowledge base the variable is housed in
   * @param id         the id of the barrier in the barrier ring
   * @param participants  the number of participants in the barrier ring
   **/
  void set_name(const std::string& var_name, Variables& knowledge, int id,
      int participants);

  /**
   * Sets the variable name that this refers to
   * @param var_name  the name of the variable in the knowledge base
   * @param knowledge  the knowledge base the variable is housed in
   * @param id         the id of the barrier in the barrier ring
   * @param participants  the number of participants in the barrier ring
   **/
  void set_name(const std::string& var_name, ThreadSafeContext& knowledge,
      int id, int participants);

  /**
   * Sets the value of the variable
   * @param  value  the new value of the variable
   * @return the updated value (should be same as value param)
   **/
  type operator=(type value);

  /**
   * Goes to the next barrier round
   **/
  void next(void);

  /**
   * Wait for all other participants to reach your barrier round
   * @return true if barrier round is finished. False otherwise.
   **/
  bool is_done(void);

  /**
   * Mark the value as modified. The barrier retains the same value
   * but will resend its value as if it had been modified.
   **/
  void modify(void);

  /**
   * Returns the barrier round number as a knowledge::KnowledgeRecord. This
   * is useful for referencing clock and other record info.
   * @return the value as a knowledge::KnowledgeRecord
   **/
  knowledge::KnowledgeRecord to_record(void) const;

  /**
   * Returns the barrier round number as a double
   * @return the value as a double
   **/
  double to_double(void) const;

  /**
   * Returns the barrier round number as an integer (same as *)
   * @return the value as an integer
   **/
  knowledge::KnowledgeRecord::Integer to_integer(void) const;

  /**
   * Returns the barrier round number as a string
   * @return the value as a string
   **/
  std::string to_string(void) const;

  /**
   * Checks for equality
   * @param  value  the value to compare to
   * @return true if equal, false otherwise
   **/
  bool operator==(const Barrier& value) const;

  /**
   * Checks for inequality
   * @param  value  the value to compare to
   * @return true if inequal, false otherwise
   **/
  bool operator!=(const Barrier& value) const;

  /**
   * Sets the quality of writing to the barrier variables
   *
   * @param quality         quality of writing to this location
   * @param settings        settings for referring to knowledge variables
   **/
  void set_quality(uint32_t quality,
      const KnowledgeReferenceSettings& settings = KnowledgeReferenceSettings(
          false));

  /**
   * Resizes the barrier, usually when number of participants change
   * @param id        the id of this barrier in the barrier ring
   * @param participants the number of participants in barrier ring
   **/
  void resize(size_t id = 0, size_t participants = 1);

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
   * Determines if the barrier is true
   * @return true if the barrier is true
   **/
  bool is_true(void) const;

  /**
   * Determines if the barrier is false
   * @return true if the barrier is false
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
   * Builds the aggregate barrier logic
   **/
  void build_aggregate_barrier(void);

  /**
   * Checks if current barrier is successful
   * @return  0 if unsuccessful, otherwise it is successful
   **/
  inline type barrier_result(void) const
  {
#ifndef _MADARA_NO_KARL_
    return context_->evaluate(aggregate_barrier_, no_harm).to_integer();
#else
    // note this means that barriers are always successful if no_karl=1
    return type(1);
#endif
  }

  /**
   * Builds the variable that is actually incremented
   **/
  void build_var(void);

  /**
   * Initialize the no harm eval settings
   **/
  void init_noharm(void);

  /**
   * Variable context that we are modifying
   **/
  mutable ThreadSafeContext* context_;

  /**
   * Variable reference
   **/
  VariableReference variable_;

  /**
   * id of this barrier in the barrier ring
   **/
  size_t id_;

  /**
   * the number of participants in the barrier ring
   **/
  size_t participants_;

#ifndef _MADARA_NO_KARL_
  /**
   * Expression for aggregating barrier in one atomic operation
   **/
  CompiledExpression aggregate_barrier_;
#endif

  /**
   * Settings we'll use for all evaluations
   **/
  EvalSettings no_harm;

  /**
   * Holder for variable name to quickly refresh modified status
   **/
  std::string variable_name_;
};
}
}
}

#endif  // _MADARA_CONTAINERS_BARRIER_H_