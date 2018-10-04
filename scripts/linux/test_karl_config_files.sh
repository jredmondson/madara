#!/bin/bash
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
# Copyright 2015 Carnegie Mellon University
#
# This material is based upon work funded and supported by the
# Department of Defense under Contract No. FA8721-05-C-0003 with
# Carnegie Mellon University for the operation of the Software
# Engineering Institute, a federally funded research and development
# center.
#
# Any opinions, findings and conclusions or recommendations expressed
# in this material are those of the author(s) and do not necessarily
# reflect the views of the United States Department of Defense.
#
# NO WARRANTY. THIS CARNEGIE MELLON UNIVERSITY AND SOFTWARE
# ENGINEERING INSTITUTE MATERIAL IS FURNISHED ON AN "AS-IS"
# BASIS. CARNEGIE MELLON UNIVERSITY MAKES NO WARRANTIES OF ANY KIND,
# EITHER EXPRESSED OR IMPLIED, AS TO ANY MATTER INCLUDING, BUT NOT
# LIMITED TO, WARRANTY OF FITNESS FOR PURPOSE OR MERCHANTABILITY,
# EXCLUSIVITY, OR RESULTS OBTAINED FROM USE OF THE MATERIAL. CARNEGIE
# MELLON UNIVERSITY DOES NOT MAKE ANY WARRANTY OF ANY KIND WITH
# RESPECT TO FREEDOM FROM PATENT, TRADEMARK, OR COPYRIGHT
# INFRINGEMENT.
#
# This material has been approved for public release and unlimited
# distribution.
#
# DM-0002489
#
# karl configurartion files - tests
# each test loads a config file containing karl interpeter logic too
#

MADARA_FAILS=0
TESTLINE=""

# test simple load of one config file which also points to a karl logic file
echo "Test simple load of one config file which also points to a karl logic file"
'$MADARA_ROOT/bin/karl' -cf $MADARA_ROOT/tests/settings/karl1.cfg &> log1 

input="log1"
while IFS= read -r line
do
  #echo "line: $line endline"
  TESTLINE=$line
done < "$input"

if [ "$TESTLINE" != "test_result=2" ] ; then
  echo "Test 1: Fail"
  ((MADARA_FAILS++))    
else
    echo "Test 1: Pass"
fi

# recursion limit test, should fail
echo "Recursion limit test, fallout at 10"
'$MADARA_ROOT/bin/karl' -cf $MADARA_ROOT/tests/settings/karl.cfg &> log2 

TESTLINE=""
input="log2"
while IFS= read -r line
do
  #echo "line: $line endline"
  TESTLINE=$line
done < "$input"

if [ "$TESTLINE" == "test_result=1" ] ; then
  echo "Test 2: Fail"
  ((MADARA_FAILS++))
else
    echo "Test 2: Pass"
fi

# test one karl config loading another karl config
echo "Test one karl config loading another karl config"
'$MADARA_ROOT/bin/karl' -cf $MADARA_ROOT/tests/settings/karl2.cfg &> log3 

TESTLINE=""
input="log3"
while IFS= read -r line
do
  #echo "line: $line endline"
  TESTLINE=$line
done < "$input"

if [ "$TESTLINE" == "test_result=2" ] ; then
  echo "Test 3: Fail"
  ((MADARA_FAILS++))    
else
    echo "Test 3: Pass"
fi

# test one karl config loading a karl logic file which contains commands to tripper an exception
echo "Test one karl config loading a karl logic file which contains commands to tripper an exception"
'$MADARA_ROOT/bin/karl' -cf $MADARA_ROOT/tests/settings/karl4.cfg &> log4 

TESTLINE=""
input="log4"
while IFS= read -r line
do
  #echo "line: $line endline"
  TESTLINE=$line
done < "$input"

if [ "$TESTLINE" != "test_result=1" ] ; then
  echo "Test 4: Fail"
  ((MADARA_FAILS++))    
else
    echo "Test 4: Pass"
fi

# echo test result information
echo "MADARA_FAILS=$MADARA_FAILS"

exit $MADARA_FAILS




