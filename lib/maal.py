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
import re
import random
import string

## MAML version number (major.minor.revision)
version = "0.3.0"

## gets the state of the device. If this returns ('device',*), then
# the device is connected and operational.
# @param  serial   the phone or emulator we are instrumenting
# @return          tuple of stdout, stderr from adb get-state command        
def getstate (serial):
  return adb (cmd='get-state', serial=serial)

## checks to see if the serial is online
# @param  serial   the phone or emulator we are instrumenting
# @param  package  the package to uninstall (e.g. com.google.contacts)
# @return             True if device is online, False otherwise
def isonline (serial, print_failures=True):
  if not serial:
    serial = get_first_device ()
    if not serial:
      print "No device is connected..."
      return False;

  (status, discard) = getstate (serial)

  if status == 'device':
    return True;
  else:
    print "Device " + serial + " is not connected..."
    return False;

def get_first_device ():
  # start with no serial
  serial = None

  (out, err) = adb (cmd='devices')
  mylist = out.split ('\n')

  #for i in range (len(mylist)):
  #  print "  " + mylist[i]

  if len(mylist) > 1: 
    firstdevice = mylist[1].split ()
    if firstdevice[1] == 'device':
      serial = firstdevice[0];

  return serial  
  

## uninstall a particular type of package
# @param  serial   the phone or emulator we are instrumenting
# @param  wait     if <= 0, wait forever. Otherwise, wait for a time
# @return              tuple of (stdout, stderr)
def connect (serial=None, wait=0):
  if serial:
    if isonline (serial):
      return serial
  
  if wait <= 0:
    adb (cmd='wait-for-serial', serial=serial)
    return get_first_device ()
  else:
    # for each second in the wait
    for i in range (wait):
      # is there a serial provided? if so, check for it being online and
      # return the serial if it comes on
      if serial:
        if isonline (serial):
          return serial

      # if no serial was provided, wait for any device to show up
      else:
        potential = get_first_device ();

        # if we have a device, use that
        if potential:
          return potential
      sleep (1)
  

## uninstall a particular type of package
# @param  serial   the phone or emulator we are instrumenting
# @param  package  the package to uninstall (e.g. com.google.contacts)
# @return              tuple of (stdout, stderr)
def uninstall (serial, package):
  if not isonline (serial):
    return ('unknown', 'unknown')

  return adb (cmd="uninstall", args=[package], serial=serial, echo=True)

## Installs a particular type of package
# @param  serial   the phone or emulator we are instrumenting
# @param  apk      the apk to install (e.g. com.google.contacts)
# @return              tuple of (stdout, stderr)
def install (serial, apk):
  if not isonline (serial):
    return ('unknown', 'unknown')

  return adb (cmd="install", args=[apk], serial=serial, echo=True)

## Uninstalls and then install a particular type of package
# @param  serial   the phone or emulator we are instrumenting
# @param  package  the package to uninstall (e.g. com.google.contacts)
# @param  apk      the apk to install (e.g. com.google.contacts)
# @return              tuple of (stdout, stderr)
def reinstall (serial, package, apk):
  if not isonline (serial):
    return ('unknown', 'unknown')

  uninstall (serial, package)
  return install (serial, apk)

## Moves the directional pad up
# @param  serial   the phone or emulator we are instrumenting
# @param  num      the number of inputs to go backwards
# @param  sleep_time   Time to sleep after performing all actions
# @return              tuple of (stdout, stderr)
def up (serial, num=1, sleep_time=0):
  if not isonline (serial):
    return ('unknown', 'unknown')

  for i in range (num):
    return keyevent (serial, '19')

  if sleep_time > 0:
    sleep (sleep_time)

  # empty error is normal
  return ('unknown','')

## Moves the directional pad down
# @param  serial   the phone or emulator we are instrumenting
# @param  num      the number of inputs to go backwards
# @param  sleep_time   Time to sleep after performing all actions
# @return              tuple of (stdout, stderr)
def down (serial, num=1, sleep_time=0):
  if not isonline (serial):
    return ('unknown', 'unknown')

  for i in range (num):
    keyevent (serial, '20')

  if sleep_time > 0:
    sleep (sleep_time)

  return ('unknown','')

## Moves the directional pad left
# @param  serial   the phone or emulator we are instrumenting
# @param  num      the number of inputs to go backwards
# @param  sleep_time   Time to sleep after performing all actions
# @return              tuple of (stdout, stderr)
def left (serial, num=1, sleep_time=0):
  if not isonline (serial):
    return ('unknown', 'unknown')

  for i in range (num):
    keyevent (serial, '21')

  if sleep_time > 0:
    sleep (sleep_time)

  return ('unknown','')

## Moves the directional pad right
# @param  serial   the phone or emulator we are instrumenting
# @param  num      the number of inputs to go backwards
# @param  sleep_time   Time to sleep after performing all actions
# @return              tuple of (stdout, stderr)
def right (serial, num=1, sleep_time=0):
  if not isonline (serial):
    return ('unknown', 'unknown')

  for i in range (num):
    keyevent (serial, '22')

  if sleep_time > 0:
    sleep (sleep_time)

  return ('unknown','')

## Launches an activity
# @param  serial   the phone or emulator we are instrumenting
# @return              tuple of (stdout, stderr)
def activity (serial, activity, 
              action='android.intent.action.MAIN',
              category='edu.vu.isis.ammo.dash'):
  if not isonline (serial):
    return ('unknown', 'unknown');

  return adb (cmd='shell', serial=serial, echo=True,
       args=['am', 'start',  '-a', action, '-c', category, '-n', activity]);

## Sends an Android KeyEvent to the specified phone
# @param  serial   the phone or emulator we are instrumenting
# @param  event    the integer enumeration of an Android KeyEvent (e.g.,
#                  KEYCODE_DPAD_DOWN ismapped to a constant value of 20,
#                  so event would be 20 to press the directional pad down)
# @param  sleep_time   Time to sleep after performing all actions
# @return              tuple of (stdout, stderr)
def keyevent (serial, event, sleep_time=0):
  if not isonline (serial):
    return ('unknown', 'unknown')

  return adb (cmd='shell', args=['input', 'keyevent', event], serial=serial)

  

## Presses the direction pad down, generally to use a button
# @param  serial   the phone or emulator we are instrumenting
# @param  sleep_time   Time to sleep after performing all actions
# @return              tuple of (stdout, stderr)
def press (serial, sleep_time=0):
  if not isonline (serial):
    return ('unknown', 'unknown')

  (out, err) = keyevent (serial, '23')

  if sleep_time > 0:
    sleep (sleep_time)

  return (out, err)


## Presses the menu button down
# @param  serial   the phone or emulator we are instrumenting
# @param  sleep_time   Time to sleep after performing all actions
# @return              tuple of (stdout, stderr)
def menu (serial, sleep_time=0):
  if not isonline (serial):
    return ('unknown', 'unknown')

  (out, err) = keyevent (serial, '1')

  if sleep_time > 0:
    sleep (sleep_time)

  return (out, err)

## Presses the back/return button down
# @param  serial   the phone or emulator we are instrumenting
# @param  num      the number of times to hit the back/return button
# @param  sleep_time   Time to sleep after performing all actions
# @return              tuple of (stdout, stderr)
def back (serial, num=1, sleep_time=0):
  if not isonline (serial):
    return ('unknown', 'unknown')

  for i in range (num):
    keyevent (serial, '4')

  if sleep_time > 0:
    sleep (sleep_time)

## Sleeps for a number of seconds
# @param  seconds  seconds to sleep
# @return              tuple of (stdout, stderr)
def sleep (seconds):

  time.sleep (seconds)

## Moves cursor to front of the text field
# @param  serial   the phone or emulator we are instrumenting
# @param  sleep_time   Time to sleep after performing action
# @return              tuple of (stdout, stderr)
def move_home (serial, sleep_time=0):
  if not isonline (serial):
    return ('unknown', 'unknown')

  keyevent (serial, '122')

  if sleep_time > 0:
    sleep (sleep_time)

## Moves cursor to end of the line
# @param  serial   the phone or emulator we are instrumenting
# @param  sleep_time   Time to sleep after performing action
# @return              tuple of (stdout, stderr)
def move_end (serial, sleep_time=0):
  if not isonline (serial):
    return ('unknown', 'unknown')

  keyevent (serial, '123')

  if sleep_time > 0:
    sleep (sleep_time)

## Moves cursor a page down
# @param  serial   the phone or emulator we are instrumenting
# @param  sleep_time   Time to sleep after performing action
# @return              tuple of (stdout, stderr)
def page_down (serial, sleep_time=0):
  if not isonline (serial):
    return ('unknown', 'unknown')

  keyevent (serial, '93')

  if sleep_time > 0:
    sleep (sleep_time)

## Clears the current input (EditText or whatever) by moving to
# the end of the input and deleting a specified number of times
# @param  serial   the phone or emulator we are instrumenting
# @param  num      the number of maximum deletes necessary
# @param  sleep_time   Time to sleep after clearing the text
# @return              tuple of (stdout, stderr)
def clear (serial, num=100, sleep_time=0):
  if not isonline (serial):
    return ('unknown', 'unknown')

  move_end (serial)

  for i in range (num):
    keyevent (serial, '67')

  if sleep_time > 0:
    sleep (sleep_time)

## Types a string into an EditText. Appends the text by default.
# @param  serial   the phone or emulator we are instrumenting
# @param  text     the number of maximum deletes necessary
# @param  clear_input  should the focus be cleared first?
# @param  sleep_time   time to sleep after typing the text
# @return              tuple of (stdout, stderr)
def type (serial, text, clear_input=False, sleep_time=0):
  if not isonline (serial):
    return ('unknown', 'unknown')

  tokens = text.split ()
  num = len (tokens) - 1


  if clear_input:
    clear (serial);

  for i in range (len (tokens)):
    if i != 0:
      keyevent (serial, '62')
    adb (cmd='shell', args=['input', 'text', tokens[i]], serial=serial)

  # special case - the user has provided just a space
  if text == ' ':
    keyevent (serial, '62')

  if sleep_time > 0:
    sleep (sleep_time)

## Runs an adb shell remove. If you intend to remove a directory,
# ensure that recursive is set to true and that you have permissions
# to remove files or directories.
# @param  serial   the phone or emulator serial we are running a command to
# @param  target   target file or directory to remove
# @return              tuple of (stdout, stderr)
def remove (serial, target, recursive=False):
  if not isonline (serial):
    return ('unknown', 'unknown')

  args = ['rm']

  if recursive:
    args.append ('-r')

  args.append (target)

  return adb (serial, cmd='shell', args=args)

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



## Types a random string into the current focus
# @param  serial       the phone or emulator serial we are running a command to
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
def random_type (serial, length=15, types='a', clear_input = False, sleep_time = 0):
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
  type (serial, text, clear_input=clear_input, sleep_time=sleep_time)

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
  

## prints CPU and memory usage information for the connected device
# @param  device       the phone or emulator serial we are running a command to
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

