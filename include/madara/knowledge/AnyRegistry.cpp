#include "Any.h"

#include "capnp/schema.h"

using namespace madara;
using namespace knowledge;

static std::map<const char *, const TypeHandlers *,
                compare_const_char_ptr> type_builders;

void AnyRegistry::register_type_impl(const char *name,
    const TypeHandlers *handler)
{
  type_builders.emplace(name, handler);
}

static std::map<const char *, capnp::StructSchema,
                compare_const_char_ptr> schemas;

void AnyRegistry::register_schema(const char *name,
    const capnp::StructSchema &schema)
{
  schemas.emplace(name, schema);
}

Any AnyRegistry::construct(const char *name)
{
  auto biter = type_builders.find(name);
  if (biter != type_builders.end()) {
    auto handler = biter->second;
    return Any(handler, handler->construct_default());
  }
  auto siter = schemas.find(name);
  if (siter != schemas.end()) {
    return Any(type<RegCapnObject>{}, siter->first, siter->second);
  }
  throw exceptions::BadAnyAccess(std::string("Type ") + name +
      " is not registered");
}

inline ConstAny AnyRegistry::construct_const(const char *name)
{
  return construct(name);
}

