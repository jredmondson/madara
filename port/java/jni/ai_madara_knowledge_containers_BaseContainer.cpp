#include "ai_madara_knowledge_containers_BaseContainer.h"
#include "madara/knowledge/containers/BaseContainer.h"

namespace knowledge = madara::knowledge;
namespace containers = knowledge::containers;
typedef  containers::BaseContainer   BaseContainer;

jboolean JNICALL
Java_ai_madara_knowledge_containers_BaseContainer_jni_1modifyIfTrue
(JNIEnv * env, jobject, jlong cptr, jlong container_ptr)
{
  bool result = false;
  BaseContainer * current = (BaseContainer *)cptr;
  BaseContainer * container = (BaseContainer *)container_ptr;

  if (current && container)
  {
    result = current->modify_if_true (*container);
  }
  else
  {
    // user has tried to use a deleted object. Clean up and throw
  
    madara::utility::java::throw_dead_obj_exception(env,
      "BaseContainer::modifyIfTrue: "
      "objects are released already");
  }

  return result;
}

jboolean JNICALL
Java_ai_madara_knowledge_containers_BaseContainer_jni_1modifyIfFalse
(JNIEnv * env, jobject, jlong cptr, jlong container_ptr)
{
  bool result = true;
  BaseContainer * current = (BaseContainer *)cptr;
  BaseContainer * container = (BaseContainer *)container_ptr;

  if (current && container)
  {
    result = current->modify_if_false (*container);
  }
  else
  {
    // user has tried to use a deleted object. Clean up and throw
  
    madara::utility::java::throw_dead_obj_exception(env,
      "BaseContainer::modifyIfTrue: "
      "objects are released already");
  }

  return result;
}
