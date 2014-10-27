#!/usr/bin/env python

## @package maml
# The Madara Android Monkeyrunner Library is meant for blackbox
# testing Android activities via the specialized functions that
# wrap around the Monkeyrunner library. The Monkeyrunner library
# is included in the Android SDK.

# Imports the monkeyrunner modules used by this program
import os, subprocess
import sys
import time
import random
import string
import re
from com.android.monkeyrunner import MonkeyRunner, MonkeyDevice

## MAML version number (major.minor.revision)
version = "0.1.3"

## uninstall a particular type of package
# @param  device   the monkey runner device object
# @param  package  the package to uninstall (e.g. com.google.contacts)
def uninstall (device, package):
  
  device.removePackage (package)

## Installs a particular type of package
# @param  device   the monkey runner device object
# @param  apk      the apk to install (e.g. com.google.contacts)
def install (device, apk):
  
  device.installPackage (apk)

## Uninstalls and then install a particular type of package
# @param  device   the monkey runner device object
# @param  package  the package to uninstall (e.g. com.google.contacts)
# @param  apk      the apk to install (e.g. com.google.contacts)
def reinstall (device, package, apk):
  uninstall (device, package)
  install (device, apk)

## Moves the directional pad up
# @param  device   the phone or emulator we are instrumenting
# @param  num      the number of inputs to go backwards
# @param  sleep_time   Time to sleep after performing all actions
def up (device, num=1, sleep_time=0):
  for i in range (num):
    device.press ('KEYCODE_DPAD_UP', MonkeyDevice.DOWN_AND_UP)

  if sleep_time > 0:
    sleep (sleep_time)

## Moves the directional pad down
# @param  device   the phone or emulator we are instrumenting
# @param  num      the number of inputs to go backwards
# @param  sleep_time   Time to sleep after performing all actions
def down (device, num=1, sleep_time=0):
  for i in range (num):
    device.press ('KEYCODE_DPAD_DOWN', MonkeyDevice.DOWN_AND_UP)

  if sleep_time > 0:
    sleep (sleep_time)

## Moves the directional pad left
# @param  device   the phone or emulator we are instrumenting
# @param  num      the number of inputs to go backwards
# @param  sleep_time   Time to sleep after performing all actions
def left (device, num=1, sleep_time=0):
  for i in range (num):
    device.press ('KEYCODE_DPAD_LEFT', MonkeyDevice.DOWN_AND_UP)

  if sleep_time > 0:
    sleep (sleep_time)

## Moves the directional pad right
# @param  device   the phone or emulator we are instrumenting
# @param  num      the number of inputs to go backwards
# @param  sleep_time   Time to sleep after performing all actions
def right (device, num=1, sleep_time=0):
  for i in range (num):
    device.press ('KEYCODE_DPAD_RIGHT', MonkeyDevice.DOWN_AND_UP)

  if sleep_time > 0:
    sleep (sleep_time)

## Presses the direction pad down, generally to use a button
# @param  device   the phone or emulator we are instrumenting
# @param  sleep_time   Time to sleep after performing all actions
def press (device, sleep_time=0):
  device.press ('KEYCODE_DPAD_CENTER', MonkeyDevice.DOWN_AND_UP)

  if sleep_time > 0:
    sleep (sleep_time)

## Click at a position on the screen
# @param  device   the phone or emulator we are instrumenting
# @param  x        the x coordinate to click on
# @param  y        the y coordinate to click on
# @param  num      the number of times to click the position
# @param  sleep_time   Time to sleep after performing all actions
def click (device, x=0, y=0, num=1, sleep_time=0):
  for i in range (num):
    device.touch (x, y, MonkeyDevice.DOWN_AND_UP)

  if sleep_time > 0:
    sleep (sleep_time)

## Long click (aka long press) at a position on the screen
# @param  device   the phone or emulator we are instrumenting
# @param  x        the x coordinate to click on
# @param  y        the y coordinate to click on
# @param  seconds  number of seconds to hold the press (float e.g 2.0)
# @param  sleep_time   Time to sleep after performing all actions
def long_click (device, x=0, y=0, seconds=2.0, sleep_time=0):
  device.drag ((x,y), (x,y), seconds)

  if sleep_time > 0:
    sleep (sleep_time)


## Wrapper for Monkeyrunner drag function. Best to use this in
# case I get the shell commands working faster in the adb-only version.
# @param  device   the phone or emulator we are instrumenting
# @param  start  the (x,y) coordinate to start the drag at
# @param  end    the (x,y) coordinate to end the drag at
# @param  seconds  number of seconds to execute the drag
# @param  sleep_time   Time to sleep after performing all actions
def drag (device, start=(0,0), end=(0,0), seconds=2.0, sleep_time=0):
  device.drag (start, end, seconds, steps)

  if sleep_time > 0:
    sleep (sleep_time)


## Presses the menu button down
# @param  device   the phone or emulator we are instrumenting
# @param  sleep_time   Time to sleep after performing all actions
def menu (device, sleep_time=0):
  device.press ('KEYCODE_MENU', MonkeyDevice.DOWN_AND_UP)

  if sleep_time > 0:
    sleep (sleep_time)

## Presses the back/return button down
# @param  device   the phone or emulator we are instrumenting
# @param  num      the number of times to hit the back/return button
# @param  sleep_time   Time to sleep after performing all actions
def back (device, num=1, sleep_time=0):
  for i in range (num):
    device.press ('KEYCODE_BACK', MonkeyDevice.DOWN_AND_UP)

  if sleep_time > 0:
    sleep (sleep_time)

## Sleeps for a number of seconds
# @param  seconds  seconds to sleep
def sleep (seconds):
    time.sleep (seconds)

## Moves cursor to front of the text field
# @param  device   the phone or emulator we are instrumenting
# @param  sleep_time   Time to sleep after performing action
def move_home (device, sleep_time=0):
  device.press ('KEYCODE_MOVE_HOME', MonkeyDevice.DOWN_AND_UP)

  if sleep_time > 0:
    sleep (sleep_time)

## Moves cursor to end of the line
# @param  device   the phone or emulator we are instrumenting
# @param  sleep_time   Time to sleep after performing action
def move_end (device, sleep_time=0):
  device.press ('KEYCODE_MOVE_END', MonkeyDevice.DOWN_AND_UP)

  if sleep_time > 0:
    sleep (sleep_time)

## Moves cursor a page down
# @param  device   the phone or emulator we are instrumenting
# @param  sleep_time   Time to sleep after performing action
def page_down (device, sleep_time=0):
  device.press ('KEYCODE_PAGE_DOWN', MonkeyDevice.DOWN_AND_UP)

  if sleep_time > 0:
    sleep (sleep_time)

## Clears the current input (EditText or whatever) by moving to
# the end of the input and deleting a specified number of times
# @param  device   the phone or emulator we are instrumenting
# @param  num      the number of maximum deletes necessary
# @param  sleep_time   Time to sleep after clearing the text
def clear (device, num=100, sleep_time=0):
  device.press ('KEYCODE_MOVE_END', MonkeyDevice.DOWN_AND_UP)
  for i in range (num):
    device.press ('KEYCODE_DEL', MonkeyDevice.DOWN_AND_UP)

  if sleep_time > 0:
    sleep (sleep_time)

## Types a string into an EditText. Appends the text by default.
# @param  device   the phone or emulator we are instrumenting
# @param  text     the number of maximum deletes necessary
# @param  clear_input  should the focus be cleared first?
# @param  sleep_time   time to sleep after typing the text
def type (device, text, clear_input=False, sleep_time=0):
  tokens = text.split ()
  num = len (tokens) - 1

  if clear_input:
    clear (device);

  for i in range (len (tokens)):
    if i != 0:
      device.press ('KEYCODE_SPACE', MonkeyDevice.DOWN_AND_UP)
    device.type (tokens[i])

  # special case - the user has provided just a space
  if text == ' ':
    device.press ('KEYCODE_SPACE', MonkeyDevice.DOWN_AND_UP)

  if sleep_time > 0:
    sleep (sleep_time)

## Runs an adb command. Note that this does not necessary run a shell
# command as well. This is the equivalent of saying "adb -s serial cmd args"
# @param  cmd      the command to run in adb
# @param  args     the command args. this should be a list not a string
# @param  serial   the phone or emulator serial we are running a command to
# @param  sleep_time   Time to sleep after running the command, if any
# @param  print_stderr Whether or not to print the stderr of the command
# @param  print_stdout Whether or not to print the stdout of the command
# @param  echo         Whether or not to print the command that is executed
# @returns  a tuple containing stdout and stderr (stdout, stderr) printouts
def adb (cmd, args, serial, sleep_time=0, print_stderr=False,
                 print_stdout=False, echo=False):
   
  popen_args = ['adb']
 
  if serial:
    popen_args.append ('-s');
    popen_args.append (serial);

  popen_args.append (cmd);
  popen_args.extend (args);

  if echo:
    print "  " + ' '.join (popen_args)
  process = subprocess.Popen (popen_args, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
  (my_out, my_err) = process.communicate ()

  if print_stderr:
    print "  MAML: STDERR for " + ' '.join (popen_args)
    print my_err
  if print_stdout:
    print "  MAML: STDOUT for " + ' '.join (popen_args)
    print my_out

  if sleep_time > 0:
    sleep (sleep_time)

  return (my_out, my_err)



## Runs an adb logcat command
# @param  serial   the phone or emulator serial we are running a command to
# @param  flush    if true, flushes the current logcat   
# @param  file     saves the logcat contents to the specified file
# @param  filters  also called tags in logcat. These help filter the logcat contents
#                  so you can find specific information. For instance, 'MyApp:I' would
#                  show all (I)nformation level logs for the app 'MyApp'. If you want
#                  multiple filters, simply add more filters into the list (note this
#                  is not a string)
# @param  format   format the output according to logcat formats ('brief', 'process', etc.
#                  are valid options). See the Android adb logcat help for more options.
#                  'brief' is default for adb logcat.
# @param  sleep_time   Time to sleep after running the command, if any
# @returns  a tuple containing stdout and stderr (stdout, stderr) printouts
def logcat (serial, flush=False, file=None, filters=[], format=None, sleep_time=0):

  args = []

  if flush:
    args.append ('-c')
  else:
    args.append ('-d')

  if format:
    args.append ('-v')
    args.append (format)

  if len (filters) > 0:
    args.extend (filters)

  (my_out, my_err) = adb ('logcat', args, serial, sleep_time)

  if file:
    outfile = open (file, 'w')
    outfile.write (my_out)
    outfile.close ()

  if sleep_time > 0:
    sleep (sleep_time)

  return (my_out, my_err)
  

## Types a random string into the current focus
# @param  device       the phone or emulator serial we are running a command to
# @param  length       length of random string to generate
# @param  types        types of random characters to generate. The types are
#                      the following and can be appended into a sequence:
#                      a - all printable character types
#                      d - digits/numbers 
#                      l - lowercase characters
#                      p - punctuation characters 
#                      s - spaces only 
#                      u - uppercase characters
#                      w - whitespace characters (space, tab, linefeed, etc.)
# @param  clear_input  should the focus be cleared first?
# @param  sleep_time   time to sleep after running the command, if any
def random_type (device, length=15, types='a', clear_input = False, sleep_time = 0):
  # build the choices for randomization
  choices = "";

  for i in range (len(types)):
    if types[i] == 'u':
      # u is for upper case
      choices += string.uppercase
    elif types[i] == 'l':
      # l is for lower case
      choices += string.lowercase
    elif types[i] == 'd':
      # d is for digits
      choices += string.digits
    elif types[i] == 'p':
      # p is for all punctuation types
      choices += string.punctuation
    elif types[i] == 's':
      # s is just a space
      choices += ' '
    elif types[i] == 'w':
      # w is for whitespace characters (space, tab, linefeed, etc.)
      choices += string.whitespace
    elif types[i] == 'a':
      # if user specified all, then we're done so break
      choices = string.printable
      break

  text = ''.join (random.choice (choices) for x in range (length))
  type (device, text, clear_input=clear_input, sleep_time=sleep_time)


## prints CPU and memory usage information for the connected device
# @param  serial       the phone or emulator serial we are running a command to
# @param  append_file  a file to write to (default is stdout)
# @param  simplified   use a simplified print format (default is top output) 
# @param  processes    if not-simplified, the number of cpu-intensive processes
#                      to print to the file or terminal
# @param  sleep_time   time to sleep after running the command, if any
def print_device_stats (serial=None, append_file=False, simplified=False, processes=5, sleep_time=0):
  
  args = []

  args.append ('top')
  args.append ('-m')
  args.append (str (processes))
  args.append ('-n')
  args.append ('1')

  (cpu_out, cpu_err) = adb ('shell', args, serial, sleep_time)

  args = []
  args.append ('cat /proc/meminfo')

  (mem_out, mem_err) = adb ('shell', args, serial, sleep_time)

  match = re.search ('MemTotal:\s+(\d+\s[^\s]+)\s+MemFree:\s+(\d+\s[^\s]+)', 
                     mem_out)
    
  mem_total = "Error"
  mem_free = "Error"

  if match:
    (mem_total, mem_free) = match.group (1, 2)

  device_output = 'Memory: ' + mem_free + ' free of ' + mem_total 

  if simplified:

    match = re.search ('User\s+(\d+)%,\s+System\s+(\d+)%', cpu_out)
    #match = re.search ('User\s+(\d+)%, System', cpu_out)

    if match:
      (cpu_user, cpu_sys) = match.group (1,2)
    
      cpu_total = int (cpu_user) + int (cpu_sys)
      cpu_out = "Total " + str (cpu_total) + "% (User: " + str (cpu_user) + "% "
      cpu_out += "Sys: " + str (cpu_sys) + "%)"
    else:
      cpu_out = "Error in top output"

    device_output += '. CPU: ' + cpu_out
  else:
    cpu_out = cpu_out.rstrip ()
    cpu_out = cpu_out.lstrip ()

    device_output += "\n" + cpu_out
    device_output += "\n-------------"


  if append_file:
   append_file.write (mem_out)
  else:
   print device_output
  
  if sleep_time > 0:
    sleep (sleep_time)

