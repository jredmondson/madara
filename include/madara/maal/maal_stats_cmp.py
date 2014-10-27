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

def parse_simple_stats (input_text):

  match = re.search ('User\s+(\d+)\.\s+System\s+(\d+)\.\s+IO\s+(\d+)\.',
                     input_text)

  if match:
    (user_ticks, system_ticks, io_ticks) = match.group (1, 2, 3)

  match = re.search ('Num\s+(\d+)\.\s+Switches\s+(\d+)\.',
                     input_text)

  if match:
    (processes, switches) = match.group (1, 2)

  return (user_ticks, system_ticks, io_ticks, processes, switches)


def parse_proc_stats (input_text):

  match = re.search ('cpu\s+(\d+)\s+(\d+)\s+(\d+)\s+(\d+)\s+(\d+)\s+(\d+)\s+(\d+)\s+',
                     stat_out)

  if match:
    (user_ticks, low_user_ticks, system_ticks, idle_ticks, 
     io_ticks, irq_ticks, soft_irq_ticks) = match.group (1, 2, 3, 4, 5, 6, 7)

  match = re.search ('ctxt\s+(\d+)\s+btime\s+\d+\s+processes\s+(\d+)',
                     stat_out)
  if match:
    (switches, processes) = match.group (1, 2)

  return (user_ticks, system_ticks, io_ticks, processes, switches)


def parse_stats (input_text):

  if re.match ("ctxt", input_text):
    return parse_proc_stats (input_text)
  else:
    return parse_simple_stats (input_text)


# command line arguments
def main ():
  parser = OptionParser (add_help_option=False)
  parser.add_option ("-s", "--simplified", dest="simplified", default=None,
                     help="show a simplified output",
                     action='store_true')
  parser.add_option ("--device", "--serial", dest="device", default=None,
                     help="The Android serial number for the device to call",
                     action='store')
  parser.add_option("--infile1", "--firstfile", dest="file1", default=None,
                    help="1st filename read stats from",
                    action='store')
  parser.add_option("--infile2", "--secondfile", dest="file2", default=None,
                    help="2nd filename read stats from",
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

  # check for the existence of both files

  if not options.file1 or not options.file2:
    print "ERROR: In order to use this script, you must specify two input files\n"
    parser.print_help ()
    sys.exit (3)

  file1 = open (options.file1, 'r')
  file1_input = file1.read ()
  file1.close ()

  file2 = open (options.file2, 'r')
  file2_input = file2.read ()
  file2.close ()

  file1_stats = parse_stats (file1_input)
  file2_stats = parse_stats (file2_input)

  cmp_stats = (abs (int (file1_stats[0]) - int (file2_stats[0])),
               abs (int (file1_stats[1]) - int (file2_stats[1])),
               abs (int (file1_stats[2]) - int (file2_stats[2])),
               abs (int (file1_stats[3]) - int (file2_stats[3])),
               abs (int (file1_stats[4]) - int (file2_stats[4]))
              )

  output_text = ""
  
  (user_ticks, system_ticks, io_ticks, processes, switches) = cmp_stats

  output_text += "Clockticks: "

  output_text += "User " + str (user_ticks) + ". "
  output_text += "System " + str (system_ticks) + ". "
  output_text += "IO " + str (io_ticks) + ". "

  output_text += "\nProcesses: "

  output_text += "Num " + str (processes) + ". "
  output_text += "Switches " + str (switches) + "."

  if options.outfile:
    stats_file = open (options.outfile, 'w')
    stats_file.write (output_text)
    stats_file.close ()
  else:
    print output_text


if __name__ == "__main__":
    main()

