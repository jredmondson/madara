#include "Any.h"

#undef CONST

#ifdef _USE_CAPNP_
#include "capnp/schema.h"
#endif  // _USE_CAPNP_

using namespace madara;
using namespace knowledge;

static std::map<const char*, const TypeHandlers*, compare_const_char_ptr>
    type_builders;

bool AnyRegistry::register_type_impl(
    const char* tag, const TypeHandlers* handler)
{
  return type_builders.emplace(tag, handler).second;
}

#ifdef _USE_CAPNP_
static std::map<const char*, capnp::StructSchema, compare_const_char_ptr>
    schemas;

bool AnyRegistry::register_schema(
    const char* tag, const capnp::StructSchema& schema)
{
  return schemas.emplace(tag, schema).second;
}

const std::pair<const char* const, capnp::StructSchema>&
AnyRegistry::lookup_schema(const char* tag)
{
  auto iter = schemas.find(tag);
  if (iter == schemas.end())
  {
    throw exceptions::BadAnyAccess(
        std::string("Type ") + tag + " is not registered");
  }
  return *iter;
}
#endif

Any AnyRegistry::construct(const char* tag)
{
  auto biter = type_builders.find(tag);
  if (biter != type_builders.end())
  {
    auto handler = biter->second;
    return Any(handler, handler->construct_default());
  }

#ifdef _USE_CAPNP_
  auto siter = schemas.find(tag);
  if (siter != schemas.end())
  {
    return Any(type<RegCapnObject>{}, siter->first, siter->second);
  }
  return Any(type<GenericCapnObject>{});
#else
  return Any();
#endif // _USE_CAPNP_

}

inline ConstAny AnyRegistry::construct_const(const char* tag)
{
  return construct(tag);
}
