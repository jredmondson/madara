#include "Any.h"

#ifndef MADARA_KNOWLEDGE_ANY_REGISTRY_H_
#define MADARA_KNOWLEDGE_ANY_REGISTRY_H_

/**
 * @file AnyRegistry.h
 * @author David Kyle <david.kyle@shield.ai>
 *
 * This file contains the AnyRegistry class
 **/

#include <memory>
#include <map>
#include <functional>
#include <type_traits>

#include "madara/utility/SupportTest.h"
#include "madara/MadaraExport.h"
#include "madara/utility/StdInt.h"
#include "madara/logger/GlobalLogger.h"
#include "madara/utility/IntTypes.h"
#include "madara/exceptions/BadAnyAccess.h"

namespace capnp
{
class StructSchema;
}

namespace madara
{
namespace knowledge
{
class Any;
class ConstAny;
struct TypeHandlers;

struct compare_const_char_ptr
{
  bool operator()(const char* l, const char* r) const
  {
    return std::strcmp(l, r) < 0;
  }
};

class AnyRegistry
{
public:
  /**
   * Register a type with the Any system, using a tag which will be used to
   * portably identify this type across processes.
   *
   * @param name the tag to use. Should be portable across all systems which
   *   will use this type (or one with compatible marshalled format). Pointer
   *   given is saved as-is, and the string pointed to is not copied, so ensure
   *   the given string's lifetime lasts until process termination.
   * @return true if the type is now registered, false if that tag is
   * already taken
   **/
  template<typename T>
  static bool register_type(const char* name);

  /**
   * Register a schema with the Any system, using a tag which will be used to
   * portably identify this type across processes.
   *
   * @param name the tag to use. Should be portable across all systems which
   *   will use this type (or one with compatible marshalled format). Pointer
   *   given is saved as-is, and the string pointed to is not copied, so ensure
   *   the given string's lifetime lasts until process termination.
   * @param schema Cap'n Proto schema object representing type
   * @return true if the schema is now registered, false if that tag is
   * already taken
   **/
  static bool register_schema(
      const char* name, const capnp::StructSchema& schema);

  static const std::pair<const char* const, capnp::StructSchema>& lookup_schema(
      const char* tag);

  template<typename T>
  static const char* const& get_type_name()
  {
    return *get_type_name_ptr<T>();
  }

  static Any construct(const char* name);
  static ConstAny construct_const(const char* name);

protected:
  static bool register_type_impl(const char* name, const TypeHandlers* handler);

  template<typename T>
  static const char** get_type_name_ptr()
  {
    static const char* type_name = nullptr;
    return &type_name;
  }
};

#define MADARA_ANY_REGISTER_TYPE(type) \
  ::madara::knowledge::Any::register_type<type>(#type)
}
}

#endif  // MADARA_KNOWLEDGE_ANY_REGISTRY_H_
