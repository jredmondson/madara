# !/usr/bin/python

import os, sys

final_file = os.environ['MADARA_ROOT'] + "/lib/madara.pyd"
source_file = os.environ['MADARA_ROOT'] + "/lib/madara.pyd.dll"

# delete any lib/madara.pyd if it existed
if os.path.isfile(final_file):
  os.remove(final_file)

# renaming directory ''tutorialsdir"
os.rename(source_file, final_file)

