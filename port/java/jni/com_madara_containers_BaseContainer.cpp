#include "com_madara_containers_BaseContainer.h"
#include "madara/knowledge/containers/BaseContainer.h"

namespace knowledge = madara::knowledge;
namespace containers = knowledge::containers;
typedef  containers::BaseContainer   BaseContainer;

jboolean JNICALL Java_com_madara_containers_BaseContainer_jni_1modifyIfTrue
(JNIEnv *, jobject, jlong cptr, jlong container_ptr)
{
  bool result = false;
  BaseContainer * current = (BaseContainer *)cptr;
  BaseContainer * container = (BaseContainer *)container_ptr;

  if (current && container)
  {
    result = current->modify_if_true (*container);
  }

  return result;
}

jboolean JNICALL Java_com_madara_containers_BaseContainer_jni_1modifyIfFalse
(JNIEnv *, jobject, jlong cptr, jlong container_ptr)
{
  bool result = true;
  BaseContainer * current = (BaseContainer *)cptr;
  BaseContainer * container = (BaseContainer *)container_ptr;

  if (current && container)
  {
    result = current->modify_if_false (*container);
  }

  return result;
}
