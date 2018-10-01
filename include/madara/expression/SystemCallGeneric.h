#ifndef MADARA_SYSTEM_CALL_GENERIC_H_
#define MADARA_SYSTEM_CALL_GENERIC_H_

#ifndef _MADARA_NO_KARL_

#include <string>
#include <stdexcept>
#include "madara/utility/StdInt.h"
#include "madara/expression/SystemCallNode.h"
#include "madara/expression/Visitor.h"

namespace madara
{
namespace expression
{
/**
 * General purpose system call KaRL node. Construct with the function
 * name, and a callable (that accepts std::vector<KnowledgeRecord>, and
 * returns KnowledgeRecord).
 *
 * Does not do pruning, as it cannot know what the callable does. If your
 * system call can make good use of prune, you should write a custom
 * node instead.
 **/
class SystemCallGeneric : public SystemCallNode
{
public:
  using fn_signature = madara::knowledge::KnowledgeRecord(
      std::vector<madara::knowledge::KnowledgeRecord>);

  using fn_type = std::function<fn_signature>;

  /**
   * Constructor
   **/
  SystemCallGeneric(madara::knowledge::ThreadSafeContext& context,
      const ComponentNodes& nodes, const char* name, fn_type fn)
    : SystemCallNode(context, nodes), name_(name), fn_(fn)
  {
  }

  /**
   * Returns the value of the node
   * @return    value of the node
   **/
  virtual madara::knowledge::KnowledgeRecord item(void) const
  {
    return madara::knowledge::KnowledgeRecord(nodes_.size());
  }

  /**
   * Prunes the expression tree of unnecessary nodes. This implementation
   * assumes it can always change, so does not prune.
   *
   * @param     can_change   set to true if variable nodes are contained
   * @return    value of current contained expression tree
   **/
  virtual madara::knowledge::KnowledgeRecord prune(bool& can_change)
  {
    can_change = true;
    return {};
  }

  /**
   * Evaluates the expression tree.
   * @return    value of current contained expression tree
   **/
  virtual madara::knowledge::KnowledgeRecord evaluate(
      const madara::knowledge::KnowledgeUpdateSettings& settings)
  {
    std::vector<madara::knowledge::KnowledgeRecord> args;
    args.reserve(nodes_.size());
    for (const auto& node : nodes_) {
      args.emplace_back(node->evaluate(settings));
    }
    return fn_(std::move(args));
  }

  /**
   * Accepts a visitor subclassed from the Visitor class
   * @param    visitor   visitor instance to use
   **/
  virtual void accept(Visitor& visitor) const
  {
    visitor.visit(*this);
  }

  const char* name() const
  {
    return name_;
  }

private:
  const char* name_;
  fn_type fn_;
};
}
}

#endif  // _MADARA_NO_KARL_

#endif  // MADARA_SYSTEM_CALL_GENERIC_H_
