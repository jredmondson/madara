#!/usr/bin/env python

## @package maal_monitor
# monitors CPU and Memory usage on an Android smartphone. Does not require monkeyrunner
# "python maal_monitor.py --help" to see available parameters.


# Imports os and system modules which we need to import Ammo scripts
import os
import sys
import re

# Import the Ammo script library
sys.path.append(os.path.dirname(sys.argv[0]))
import maal
from optparse import OptionParser

# command line arguments
def main ():
  parser = OptionParser (add_help_option=False)
  parser.add_option ("-s", "--simplified", dest="simplified", default=None,
                     help="show a simplified output",
                     action='store_true')
  parser.add_option ("--device", "--serial", dest="device", default=None,
                     help="The Android serial number for the device to call",
                     action='store')
  parser.add_option("--outfile", "--file", dest="outfile", default=None,
                    help="filename to save stats to (default is stdout)",
                    action='store')
  parser.add_option ("-h", "--help", dest="help", default=None,
                     help="show usage information for this script",
                     action='store_true')


  # options can be used with options.{dest}. Args is a list of all
  # unused command line arguments
  (options, args) = parser.parse_args ()

  if options.help:
    parser.print_help ()
    sys.exit (3)

  args=[]
  args.append ('cat')
  args.append ('/proc/stat')

  (stat_out, stat_err) = maal.adb ('shell', args, options.device)

  output_text = ""
  
  if options.simplified:

    match = re.search ('cpu\s+(\d+)\s+(\d+)\s+(\d+)\s+(\d+)\s+(\d+)\s+(\d+)\s+(\d+)\s+',
                     stat_out)

    output_text += "Clockticks: "

    if match:
      (user_ticks, low_user_ticks, system_ticks, idle_ticks, 
       io_ticks, irq_ticks, soft_irq_ticks) = match.group (1, 2, 3, 4, 5, 6, 7)
      output_text += "User " + user_ticks + ". "
      output_text += "System " + system_ticks + ". "
      output_text += "IO " + io_ticks + ". "

    output_text += "\nProcesses: "

    match = re.search ('ctxt\s+(\d+)\s+btime\s+\d+\s+processes\s+(\d+)',
                     stat_out)
    if match:
      (switches, processes) = match.group (1, 2)
      output_text += "Num " + processes + ". Switches " + switches + "."
  else:
    output_text = stat_out 

  if options.outfile:
    stats_file = open (options.outfile, 'w')
    stats_file.write (output_text)
    stats_file.close ()
  else:
    print output_text


if __name__ == "__main__":
    main()

