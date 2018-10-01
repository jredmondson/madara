#!/bin/bash

FILE_ROOTS=(
    include/madara/
    tests
    tools
    port/python/src
    port/java/jni
  )

if which clang-format-5.0 >& /dev/null; then true; else
  echo Install clang-format-5.0 package
  exit 1
fi

cd $MADARA_ROOT

find "${FILE_ROOTS[@]}" -name '*.h' -o -name '*.cpp' -o -name '*.inl' |
  xargs -- clang-format-5.0 -i -style=file "$@"
