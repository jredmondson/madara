# !/usr/bin/python

import os, sys

final_file = os.environ['MADARA_ROOT'] + "/lib/madara.pyd"
source_dll_file = os.environ['MADARA_ROOT'] + "/lib/madara.pyd.dll"
source_lib_file = os.environ['MADARA_ROOT'] + "/lib/madara.pyds.lib"


# delete any lib/madara.pyd if it existed
if os.path.isfile(final_file):
  os.remove(final_file)

if os.path.isfile(source_dll_file):
  # renaming directory ''tutorialsdir"
  os.rename(source_dll_file, final_file)

elif os.path.isfile(source_lib_file):
  # renaming directory ''tutorialsdir"
  os.rename(source_lib_file, final_file)
