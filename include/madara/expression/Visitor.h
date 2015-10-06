/* -*- C++ -*- */
#ifndef _MADARA_VISITOR_H_
#define _MADARA_VISITOR_H_

namespace madara
{
  namespace expression
  {
    class Leaf_Node;
    class Variable_Node;
    class Variable_Decrement_Node;
    class Variable_Divide_Node;
    class Variable_Increment_Node;
    class Variable_Multiply_Node;
    class Variable_Compare_Node;
    class List_Node;
    class Composite_Array_Reference;
    class Composite_Const_Array;
    class Composite_Negate_Node;
    class Composite_Postdecrement_Node;
    class Composite_Postincrement_Node;
    class Composite_Predecrement_Node;
    class Composite_Preincrement_Node;
    class Composite_Not_Node;
    class Composite_Add_Node;
    class Composite_Both_Node;
    class Composite_Sequential_Node;
    class Composite_Function_Node;
    class Composite_For_Loop;
    class Composite_Implies_Node;
    class Composite_Assignment_Node;
    class Composite_And_Node;
    class Composite_Or_Node;
    class Composite_Equality_Node;
    class Composite_Inequality_Node;
    class Composite_Less_Than_Equal_Node;
    class Composite_Less_Than_Node;
    class Composite_Greater_Than_Equal_Node;
    class Composite_Greater_Than_Node;
    class Composite_Subtract_Node;
    class Composite_Divide_Node;
    class Composite_Multiply_Node;
    class Composite_Modulus_Node;
    class Composite_Return_Right_Node;
    class System_Call_Delete_Variable;
    class System_Call_Eval;
    class System_Call_Expand_Env;
    class System_Call_Expand_Statement;
    class System_Call_Fragment;
    class System_Call_Get_Clock;
    class System_Call_Get_Time;
    class System_Call_Log_Level;
    class System_Call_Print;
    class System_Call_Print_System_Calls;
    class System_Call_Rand_Double;
    class System_Call_Rand_Int;
    class System_Call_Read_File;
    class System_Call_Set_Clock;
    class System_Call_Set_Precision;
    class System_Call_Size;
    class System_Call_Sleep;
    class System_Call_To_Buffer;
    class System_Call_To_Double;
    class System_Call_To_Doubles;
    class System_Call_To_Host_Dirs;
    class System_Call_To_Integer;
    class System_Call_To_Integers;
    class System_Call_To_String;
    class System_Call_Type;
    class System_Call_Write_File;

    /**
     * @class Visitor
     * @brief Abstract base class for all visitors to all classes that derive
     *        from @a Component_Node.
     *
     *        This class plays the role of the "visitor" in the Visitor
     *        pattern. 
     */
    class Visitor
    {
    public:
      /// Visit a Leaf_Node.
      virtual void visit (const Leaf_Node &node) = 0;

      /// Visit an array reference.
      virtual void visit (const Composite_Const_Array &node) = 0;

      /// Visit an array reference.
      virtual void visit (const Composite_Array_Reference &node) = 0;

      /// Visit a Variable_Node.
      virtual void visit (const Variable_Node &node) = 0;
      
      /// Visit a Variable_Decrement_Node.
      virtual void visit (const Variable_Decrement_Node &node) = 0;
      
      /// Visit a Variable_Divide_Node.
      virtual void visit (const Variable_Divide_Node &node) = 0;
      
      /// Visit a Variable_Increment_Node.
      virtual void visit (const Variable_Increment_Node &node) = 0;
      
      /// Visit a Variable_Multiply_Node.
      virtual void visit (const Variable_Multiply_Node &node) = 0;
      
      /// Visit a Variable_Compare_Node.
      virtual void visit (const Variable_Compare_Node &node) = 0;
      
      /// Visit a List_Node.
      virtual void visit (const List_Node &node) = 0;

      /// Visit a Composite_Negate_Node.
      virtual void visit (const Composite_Negate_Node &node) = 0;

      /// Visit a Composite_Postdecrement_Node.
      virtual void visit (const Composite_Postdecrement_Node &node) = 0;

      /// Visit a Composite_Postincrement_Node.
      virtual void visit (const Composite_Postincrement_Node &node) = 0;
      
      /// Visit a Composite_Predecrement_Node.
      virtual void visit (const Composite_Predecrement_Node &node) = 0;

      /// Visit a Composite_PreIncrement_Node.
      virtual void visit (const Composite_Preincrement_Node &node) = 0;

      /// Visit a Composite_Not_Node.
      virtual void visit (const Composite_Not_Node &node) = 0;

      /// Visit a Composite_Add_Node.
      virtual void visit (const Composite_Add_Node &node) = 0;

      /// Visit a Composite_Assignment_Node.
      virtual void visit (const Composite_Assignment_Node &node) = 0;

      /// Visit a Composite_And_Node.
      virtual void visit (const Composite_And_Node &node) = 0;

      /// Visit a Composite_Or_Node.
      virtual void visit (const Composite_Or_Node &node) = 0;

      /// Visit a Composite_Equality_Node.
      virtual void visit (const Composite_Equality_Node &node) = 0;

      /// Visit a Composite_Inequality_Node.
      virtual void visit (const Composite_Inequality_Node &node) = 0;

      /// Visit a Composite_Less_Than_Equal_Node.
      virtual void visit (const Composite_Greater_Than_Equal_Node &node) = 0;

      /// Visit a Composite_Less_Than_Node.
      virtual void visit (const Composite_Greater_Than_Node &node) = 0;

      /// Visit a Composite_Less_Than_Equal_Node.
      virtual void visit (const Composite_Less_Than_Equal_Node &node) = 0;

      /// Visit a Composite_Less_Than_Node.
      virtual void visit (const Composite_Less_Than_Node &node) = 0;

      /// Visit a Composite_Subtract_Node.
      virtual void visit (const Composite_Subtract_Node &node) = 0;

      /// Visit a Composite_Divide_Node.
      virtual void visit (const Composite_Divide_Node &node) = 0;

      /// Visit a Composite_Multiply_Node.
      virtual void visit (const Composite_Multiply_Node &node) = 0;

      /// Visit a Composite_Modulus_Node.
      virtual void visit (const Composite_Modulus_Node &node) = 0;

      /// Visit a Composite_Both_Node.
      virtual void visit (const Composite_Both_Node &node) = 0;

      /// Visit a Composite_Return_Right_Node.
      virtual void visit (const Composite_Return_Right_Node &node) = 0;
      
      /// Visit a Composite_Sequential_Node.
      virtual void visit (const Composite_Sequential_Node &node) = 0;
      
      /// Visit a Composite_Function_Node.
      virtual void visit (const Composite_Function_Node &node) = 0;
      
      /// Visit a Composite_Function_Node.
      virtual void visit (const Composite_For_Loop &node) = 0;

      /// Visit a Composite_Implies_Node.
      virtual void visit (const Composite_Implies_Node &node) = 0;
      
      /// Visit a System_Call_Delete_Variable.
      virtual void visit (const System_Call_Delete_Variable &node) = 0;
      
      /// Visit a System_Call_Eval.
      virtual void visit (const System_Call_Eval &node) = 0;
      
      /// Visit a System_Call_Expand_Env.
      virtual void visit (const System_Call_Expand_Env &node) = 0;
      
      /// Visit a System_Call_Expand_Statement.
      virtual void visit (const System_Call_Expand_Statement &node) = 0;
      
      /// Visit a System_Call_Expand_Statement.
      virtual void visit (const System_Call_Fragment &node) = 0;
      
      /// Visit a System_Call_Get_Clock.
      virtual void visit (const System_Call_Get_Clock &node) = 0;
      
      /// Visit a System_Call_Get_Time.
      virtual void visit (const System_Call_Get_Time &node) = 0;
      
      /// Visit a System_Call_Log_Level.
      virtual void visit (const System_Call_Log_Level &node) = 0;
      
      /// Visit a System_Call_Read_File.
      virtual void visit (const System_Call_Print &node) = 0;
      
      /// Visit a System_Call_Print_System_Calls.
      virtual void visit (const System_Call_Print_System_Calls &node) = 0;
      
      /// Visit a System_Call_Rand_Double.
      virtual void visit (const System_Call_Rand_Double &node) = 0;
      
      /// Visit a System_Call_Rand_Int.
      virtual void visit (const System_Call_Rand_Int &node) = 0;
      
      /// Visit a System_Call_Read_File.
      virtual void visit (const System_Call_Read_File &node) = 0;
      
      /// Visit a System_Call_Set_Clock.
      virtual void visit (const System_Call_Set_Clock &node) = 0;
      
      /// Visit a System_Call_Size.
      virtual void visit (const System_Call_Size &node) = 0;
      
      /// Visit a System_Call_Sleep.
      virtual void visit (const System_Call_Sleep &node) = 0;
      
      /// Visit a System_Call_To_Buffer.
      virtual void visit (const System_Call_To_Buffer &node) = 0;
      
      /// Visit a System_Call_To_Double.
      virtual void visit (const System_Call_To_Double &node) = 0;
      
      /// Visit a System_Call_To_Doubles.
      virtual void visit (const System_Call_To_Doubles &node) = 0;
      
      /// Visit a System_Call_To_Host_Dirs.
      virtual void visit (const System_Call_To_Host_Dirs &node) = 0;
      
      /// Visit a System_Call_To_Integer.
      virtual void visit (const System_Call_To_Integer &node) = 0;
      
      /// Visit a System_Call_To_Integers.
      virtual void visit (const System_Call_To_Integers &node) = 0;
      
      /// Visit a System_Call_To_String.
      virtual void visit (const System_Call_To_String &node) = 0;
      
      /// Visit a System_Call_Type.
      virtual void visit (const System_Call_Type &node) = 0;
      
      /// Visit a System_Call_Write_File.
      virtual void visit (const System_Call_Write_File &node) = 0;
      
      /// Visit a System_Call_To_Buffer.
      virtual void visit (const System_Call_Set_Precision &node) = 0;
      
      /// No-op destructor to hold things together.
      virtual ~Visitor (void) = 0;
    };
  }
}

#endif /* _MADARA_VISITOR_H_ */
