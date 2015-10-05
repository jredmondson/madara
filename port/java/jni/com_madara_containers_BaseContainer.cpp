#include "com_madara_containers_BaseContainer.h"
#include "madara/knowledge/containers/Base_Container.h"

namespace knowledge = madara::knowledge;
namespace containers = knowledge::containers;
typedef  containers::Base_Container   Base_Container;

JNIEXPORT jboolean JNICALL Java_com_madara_containers_BaseContainer_jni_1modifyIfTrue
(JNIEnv *, jobject, jlong cptr, jlong container_ptr)
{
  bool result = false;
  Base_Container * current = (Base_Container *)cptr;
  Base_Container * container = (Base_Container *)container_ptr;

  if (current && container)
  {
    result = current->modify_if_true (*container);
  }

  return result;
}

JNIEXPORT jboolean JNICALL Java_com_madara_containers_BaseContainer_jni_1modifyIfFalse
(JNIEnv *, jobject, jlong cptr, jlong container_ptr)
{
  bool result = true;
  Base_Container * current = (Base_Container *)cptr;
  Base_Container * container = (Base_Container *)container_ptr;

  if (current && container)
  {
    result = current->modify_if_false (*container);
  }

  return result;
}
