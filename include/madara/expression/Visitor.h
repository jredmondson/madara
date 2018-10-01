/* -*- C++ -*- */
#ifndef _MADARA_VISITOR_H_
#define _MADARA_VISITOR_H_

namespace madara
{
namespace expression
{
class LeafNode;
class VariableNode;
class VariableDecrementNode;
class VariableDivideNode;
class VariableIncrementNode;
class VariableMultiplyNode;
class VariableCompareNode;
class ListNode;
class CompositeArrayReference;
class CompositeConstArray;
class CompositeNegateNode;
class CompositePostdecrementNode;
class CompositePostincrementNode;
class CompositePredecrementNode;
class CompositePreincrementNode;
class CompositeNotNode;
class CompositeAddNode;
class CompositeBothNode;
class CompositeSequentialNode;
class CompositeSquareRootNode;
class CompositeFunctionNode;
class CompositeForLoop;
class CompositeImpliesNode;
class CompositeAssignmentNode;
class CompositeAndNode;
class CompositeOrNode;
class CompositeEqualityNode;
class CompositeInequalityNode;
class CompositeLessThanEqualNode;
class CompositeLessThanNode;
class CompositeGreaterThanEqualNode;
class CompositeGreaterThanNode;
class CompositeSubtractNode;
class CompositeDivideNode;
class CompositeMultiplyNode;
class CompositeModulusNode;
class CompositeReturnRightNode;
class SystemCallClearVariable;
class SystemCallCos;
class SystemCallDeleteVariable;
class SystemCallEval;
class SystemCallExpandEnv;
class SystemCallExpandStatement;
class SystemCallFragment;
class SystemCallGeneric;
class SystemCallGetClock;
class SystemCallGetTime;
class SystemCallGetTimeSeconds;
class SystemCallIsinf;
class SystemCallLogLevel;
class SystemCallPow;
class SystemCallPrint;
class SystemCallPrintSystemCalls;
class SystemCallRandDouble;
class SystemCallRandInt;
class SystemCallReadFile;
class SystemCallSetClock;
class SystemCallSetFixed;
class SystemCallSetPrecision;
class SystemCallSetScientific;
class SystemCallSin;
class SystemCallSize;
class SystemCallSleep;
class SystemCallSqrt;
class SystemCallTan;
class SystemCallToBuffer;
class SystemCallToDouble;
class SystemCallToDoubles;
class SystemCallToHostDirs;
class SystemCallToInteger;
class SystemCallToIntegers;
class SystemCallToString;
class SystemCallType;
class SystemCallWriteFile;

/**
 * @class Visitor
 * @brief Abstract base class for all visitors to all classes that derive
 *        from @a ComponentNode.
 *
 *        This class plays the role of the "visitor" in the Visitor
 *        pattern.
 */
class Visitor
{
public:
  /// Visit a LeafNode.
  virtual void visit(const LeafNode& node) = 0;

  /// Visit an array reference.
  virtual void visit(const CompositeConstArray& node) = 0;

  /// Visit an array reference.
  virtual void visit(const CompositeArrayReference& node) = 0;

  /// Visit a VariableNode.
  virtual void visit(const VariableNode& node) = 0;

  /// Visit a VariableDecrementNode.
  virtual void visit(const VariableDecrementNode& node) = 0;

  /// Visit a VariableDivideNode.
  virtual void visit(const VariableDivideNode& node) = 0;

  /// Visit a VariableIncrementNode.
  virtual void visit(const VariableIncrementNode& node) = 0;

  /// Visit a VariableMultiplyNode.
  virtual void visit(const VariableMultiplyNode& node) = 0;

  /// Visit a VariableCompareNode.
  virtual void visit(const VariableCompareNode& node) = 0;

  /// Visit a ListNode.
  virtual void visit(const ListNode& node) = 0;

  /// Visit a CompositeNegateNode.
  virtual void visit(const CompositeNegateNode& node) = 0;

  /// Visit a CompositePostdecrementNode.
  virtual void visit(const CompositePostdecrementNode& node) = 0;

  /// Visit a CompositePostincrementNode.
  virtual void visit(const CompositePostincrementNode& node) = 0;

  /// Visit a CompositePredecrementNode.
  virtual void visit(const CompositePredecrementNode& node) = 0;

  /// Visit a CompositePreIncrementNode.
  virtual void visit(const CompositePreincrementNode& node) = 0;

  /// Visit a CompositeSquareRootNode.
  virtual void visit(const CompositeSquareRootNode& node) = 0;

  /// Visit a CompositeNotNode.
  virtual void visit(const CompositeNotNode& node) = 0;

  /// Visit a CompositeAddNode.
  virtual void visit(const CompositeAddNode& node) = 0;

  /// Visit a CompositeAssignmentNode.
  virtual void visit(const CompositeAssignmentNode& node) = 0;

  /// Visit a CompositeAndNode.
  virtual void visit(const CompositeAndNode& node) = 0;

  /// Visit a CompositeOrNode.
  virtual void visit(const CompositeOrNode& node) = 0;

  /// Visit a CompositeEqualityNode.
  virtual void visit(const CompositeEqualityNode& node) = 0;

  /// Visit a CompositeInequalityNode.
  virtual void visit(const CompositeInequalityNode& node) = 0;

  /// Visit a CompositeLessThanEqualNode.
  virtual void visit(const CompositeGreaterThanEqualNode& node) = 0;

  /// Visit a CompositeLessThanNode.
  virtual void visit(const CompositeGreaterThanNode& node) = 0;

  /// Visit a CompositeLessThanEqualNode.
  virtual void visit(const CompositeLessThanEqualNode& node) = 0;

  /// Visit a CompositeLessThanNode.
  virtual void visit(const CompositeLessThanNode& node) = 0;

  /// Visit a CompositeSubtractNode.
  virtual void visit(const CompositeSubtractNode& node) = 0;

  /// Visit a CompositeDivideNode.
  virtual void visit(const CompositeDivideNode& node) = 0;

  /// Visit a CompositeMultiplyNode.
  virtual void visit(const CompositeMultiplyNode& node) = 0;

  /// Visit a CompositeModulusNode.
  virtual void visit(const CompositeModulusNode& node) = 0;

  /// Visit a CompositeBothNode.
  virtual void visit(const CompositeBothNode& node) = 0;

  /// Visit a CompositeReturnRightNode.
  virtual void visit(const CompositeReturnRightNode& node) = 0;

  /// Visit a CompositeSequentialNode.
  virtual void visit(const CompositeSequentialNode& node) = 0;

  /// Visit a CompositeFunctionNode.
  virtual void visit(const CompositeFunctionNode& node) = 0;

  /// Visit a CompositeFunctionNode.
  virtual void visit(const CompositeForLoop& node) = 0;

  /// Visit a CompositeImpliesNode.
  virtual void visit(const CompositeImpliesNode& node) = 0;

  /// Visit a SystemCallClearVariable.
  virtual void visit(const SystemCallClearVariable& node) = 0;

  /// Visit a SystemCallCos.
  virtual void visit(const SystemCallCos& node) = 0;

  /// Visit a SystemCallDeleteVariable.
  virtual void visit(const SystemCallDeleteVariable& node) = 0;

  /// Visit a SystemCallEval.
  virtual void visit(const SystemCallEval& node) = 0;

  /// Visit a SystemCallExpandEnv.
  virtual void visit(const SystemCallExpandEnv& node) = 0;

  /// Visit a SystemCallExpandStatement.
  virtual void visit(const SystemCallExpandStatement& node) = 0;

  /// Visit a SystemCallExpandStatement.
  virtual void visit(const SystemCallFragment& node) = 0;

  /// Visit a SystemCallGetClock.
  virtual void visit(const SystemCallGeneric& node) = 0;

  /// Visit a SystemCallGetClock.
  virtual void visit(const SystemCallGetClock& node) = 0;

  /// Visit a SystemCallGetTime.
  virtual void visit(const SystemCallGetTime& node) = 0;

  /// Visit a SystemCallGetTimeSeconds.
  virtual void visit(const SystemCallGetTimeSeconds& node) = 0;

  /// Visit a SystemCallIsinf.
  virtual void visit(const SystemCallIsinf& node) = 0;

  /// Visit a SystemCallLogLevel.
  virtual void visit(const SystemCallLogLevel& node) = 0;

  /// Visit a SystemCallPow.
  virtual void visit(const SystemCallPow& node) = 0;

  /// Visit a SystemCallReadFile.
  virtual void visit(const SystemCallPrint& node) = 0;

  /// Visit a SystemCallPrintSystemCalls.
  virtual void visit(const SystemCallPrintSystemCalls& node) = 0;

  /// Visit a SystemCallRandDouble.
  virtual void visit(const SystemCallRandDouble& node) = 0;

  /// Visit a SystemCallRandInt.
  virtual void visit(const SystemCallRandInt& node) = 0;

  /// Visit a SystemCallReadFile.
  virtual void visit(const SystemCallReadFile& node) = 0;

  /// Visit a SystemCallSetClock.
  virtual void visit(const SystemCallSetClock& node) = 0;

  /// Visit a SystemCallSin.
  virtual void visit(const SystemCallSin& node) = 0;

  /// Visit a SystemCallSize.
  virtual void visit(const SystemCallSize& node) = 0;

  /// Visit a SystemCallSleep.
  virtual void visit(const SystemCallSleep& node) = 0;

  /// Visit a SystemCallSqrt.
  virtual void visit(const SystemCallSqrt& node) = 0;

  /// Visit a SystemCallTan.
  virtual void visit(const SystemCallTan& node) = 0;

  /// Visit a SystemCallToBuffer.
  virtual void visit(const SystemCallToBuffer& node) = 0;

  /// Visit a SystemCallToDouble.
  virtual void visit(const SystemCallToDouble& node) = 0;

  /// Visit a SystemCallToDoubles.
  virtual void visit(const SystemCallToDoubles& node) = 0;

  /// Visit a SystemCallToHostDirs.
  virtual void visit(const SystemCallToHostDirs& node) = 0;

  /// Visit a SystemCallToInteger.
  virtual void visit(const SystemCallToInteger& node) = 0;

  /// Visit a SystemCallToIntegers.
  virtual void visit(const SystemCallToIntegers& node) = 0;

  /// Visit a SystemCallToString.
  virtual void visit(const SystemCallToString& node) = 0;

  /// Visit a SystemCallType.
  virtual void visit(const SystemCallType& node) = 0;

  /// Visit a SystemCallWriteFile.
  virtual void visit(const SystemCallWriteFile& node) = 0;

  /// Visit a SystemCallSetFixed.
  virtual void visit(const SystemCallSetFixed& node) = 0;

  /// Visit a SystemCallSetPrecision.
  virtual void visit(const SystemCallSetPrecision& node) = 0;

  /// Visit a SystemCallSetScientific.
  virtual void visit(const SystemCallSetScientific& node) = 0;

  /// No-op destructor to hold things together.
  virtual ~Visitor(void) = 0;
};
}
}

#endif /* _MADARA_VISITOR_H_ */
