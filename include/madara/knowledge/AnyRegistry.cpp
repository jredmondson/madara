#include "Any.h"

using namespace madara;
using namespace knowledge;

static std::map<const char *, const TypeHandlers *,
                compare_const_char_ptr> type_builders;

void AnyRegistry::register_type_impl(const char *name,
    const TypeHandlers *handler)
{
  type_builders.emplace(name, handler);
}

Any AnyRegistry::construct(const char *name)
{
  std::cerr << "Num registered: " << type_builders.size() << std::endl;
  auto iter = type_builders.find(name);
  if (iter == type_builders.end()) {
    throw exceptions::BadAnyAccess(std::string("Type ") + name +
        " is not registered");
  }
  auto handler = iter->second;
  return Any(handler, handler->construct_default());
}

inline ConstAny AnyRegistry::construct_const(const char *name)
{
  return construct(name);
}

