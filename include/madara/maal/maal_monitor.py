#!/usr/bin/env python

## @package maal_monitor
# monitors CPU and Memory usage on an Android smartphone. Does not require monkeyrunner
# "python maal_monitor.py --help" to see available parameters.


# Imports os and system modules which we need to import Ammo scripts
import os
import sys

# Import the Ammo script library
sys.path.append(os.path.dirname(sys.argv[0]))
import maal
from optparse import OptionParser

# command line arguments
def main ():
  parser = OptionParser (add_help_option=False)
  parser.add_option ("--period", "--frequency", dest="period", 
                     default=5,
                     help="Period of time to wait between polling for stats",
                     type='int')
  parser.add_option ("--killtime", "--runtime", "--timeout", dest="killtime", 
                     default=120,
                     help="Total time to monitor the device stats",
                     type='int')
  parser.add_option ("--processes", "-p", dest="processes",
                     default=5,
                     help="Number of processes to display in the long format",
                     type='int')
  parser.add_option ("--iters", "-i", "-n", dest="iters", 
                     default=-1,
                     help="Optional way to specify iterations",
                     type='int')
  parser.add_option ("-s", "--simplified", dest="simplified", default=None,
                     help="show a simplified output",
                     action='store_true')
  parser.add_option ("--device", "--serial", dest="device", default=None,
                     help="The Android serial number for the device to call",
                     action='store')
  parser.add_option("--outfile", "--file", dest="outfile", default=None,
                    help="filename to save stats to (default is stdout)",
                    action='store')
  parser.add_option("--logcat", dest="logcat", default=None,
                    help="filename redirect logcat output to",
                    action='store')
  parser.add_option("--clearlog", "--purgelog", "--flushlog", dest="clearlog", default=None,
                    help="clearing out the content of the log files",
                    action='store_true')
  parser.add_option ("-h", "--help", dest="help", default=None,
                     help="show usage information for this script",
                     action='store_true')


  # options can be used with options.{dest}. Args is a list of all
  # unused command line arguments
  (options, args) = parser.parse_args ()

  if options.help:
    parser.print_help ()
    sys.exit (3)

  # clear logcat if the user request a logcat
  if options.clearlog:    
    maal.logcat (options.device, flush=True)

  stats_file = None

  if options.outfile:
    stats_file = open (options.outfile, 'w')

  num_attempts = options.killtime / options.period

  if options.iters > 0:
    num_attempts = options.iters

  for n in range (num_attempts):
    maal.print_device_stats (options.device, stats_file, 
      options.simplified, options.processes)
    if n != num_attempts - 1:
      maal.sleep (options.period)

  if options.outfile:
    stats_file.close ()

  # save logs to the filename specified in --logcat
  if options.logcat:
    maal.logcat (options.device, file=options.logcat) 

if __name__ == "__main__":
    main()

