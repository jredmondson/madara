#!/bin/bash
#
# Copyright (c) 2021-2024 James Edmondson and Koshee LLC All Rights Reserved.
#
# Copyright (c) 2015-2018 Carnegie Mellon University. All Rights Reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
#
# 1. Redistributions of source code must retain the above copyright
# notice, this list of conditions and the following acknowledgments
# and disclaimers.
#
# 2. Redistributions in binary form must reproduce the above copyright
# notice, this list of conditions and the following disclaimer in the
# documentation and/or other materials provided with the distribution.
#
# 3. The names "Carnegie Mellon University," "SEI" and/or "Software
# Engineering Institute" shall not be used to endorse or promote
# products derived from this software without prior written
# permission. For written permission, please contact
# permission@sei.cmu.edu.
#
# 4. Products derived from this software may not be called "SEI" nor
# may "SEI" appear in their names without prior written permission of
# permission@sei.cmu.edu.
#
# 5. Redistributions of any form whatsoever must retain the following
# acknowledgment:
#

# Build a custom project created by mpg.pl


SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )
PROJECT_DIR=$SCRIPT_DIR

pushd $(pwd)

cd $PROJECT_DIR
if [ ! -d $PROJECT_DIR/build ]; then
  mkdir build
fi

cd build
cmake ..
cmake --build . --config Release -- -j$(nproc)

popd
