import madara

engine = madara.knowledge
kb = engine.KnowledgeBase()
kr = engine.KnowledgeRecord()

madara_fails = 0

print "\nTesting KR functions..."

# Test double
kr.set (3.52)

if (kr.exists () and kr.to_double () == 3.52):
  print "  Testing KR set double and exists: SUCCESS"
else:
  print "  Testing KR set double and exists: FAIL"
  madara_fails += 1

# Test integer
kr.set (1)

if (kr.exists () and kr.to_integer () == 1):
  print "  Testing KR set long and exists: SUCCESS"
else:
  print "  Testing KR set long and exists: FAIL"
  madara_fails += 1

# Test array of integers
kr.set (madara.from_pylongs ([1, 2, 3, 4]))
array = kr.to_integers ()

if (kr.exists () and len(array) == 4 and array[0] == 1 and array[1] == 2 and 
  array[2] == 3 and array[3] == 4):
  print "  Testing KR set integer array and value check: SUCCESS"
else:
  print "  Testing KR set integer array and value check: FAIL"
  madara_fails += 1

# Test array of doubles
kr.set (madara.from_pydoubles ([1.1, 2.2, 3.3, 4.4]))
array = kr.to_doubles ()

if (kr.exists () and len(array) == 4 and array[0] == 1.1 and array[1] == 2.2 
  and array[2] == 3.3 and array[3] == 4.4):
  print "  Testing KR set double array and value check: SUCCESS"
else:
  print "  Testing KR set double array and value check: FAIL"
  madara_fails += 1

# Test string to string conversion
kr.set("string")
string = kr.to_string () 

if (len(string) == 6 and string == 'string'):
  print "  Testing KR set string and value check: SUCCESS"
else:
  print "  Testing KR set string and value check: FAIL"
  madara_fails += 1

# Test string to integer conversion
kr.set("100")
my_value = kr.to_integer ()

if (my_value == 100):
  print "  Testing if string to integer conversion works: SUCCESS"
else:
  print "  Testing if string to integer conversion works: FAIL"
  madara_fails += 1

# Test string to double conversion
my_value = kr.to_double ()

if (my_value == 100):
  print "  Testing if string to double conversion works: SUCCESS"
else:
  print "  Testing if string to double conversion works: FAIL"
  madara_fails += 1

# Test double to string conversion
kr.set(1.123)
my_value = kr.to_string()

if (kr.exists () and my_value == '1.123000'):
  print "  Testing if double to string conversion works: SUCCESS"
else:
  print "  Testing if double to string conversion works: FAIL"
  madara_fails += 1

# Test integer to string conversion
kr.set(1)
my_value = kr.to_string()

if (kr.exists () and my_value == '1'):
  print "  Testing if integer to string conversion works: SUCCESS"
else:
  print "  Testing if integer to string conversion works: FAIL"
  madara_fails += 1

# Test integer to double conversion
kr.set(1)

if (kr.exists () and kr.to_double() == 1.0):
  print "  Testing if integer to double conversion works: SUCCESS"
else:
  print "  Testing if integer to double conversion works: FAIL"
  madara_fails += 1

# Test integer array to string conversion
kr.set (madara.from_pylongs ([1, 2, 3, 4]))
array = kr.to_integers ()
my_value = kr.to_string()

if (kr.exists () and my_value == '1, 2, 3, 4'):
  print "  Testing if integer array to string conversion works: SUCCESS"
else:
  print "  Testing if integer array to string conversion works: FAIL"
  madara_fails += 1

# Test double array to string conversion
kr.set (madara.from_pydoubles ([1.100000, 2.200000, 3.300000, 4.400000]))
array = kr.to_doubles ()
my_value = kr.to_string()

if (kr.exists () and my_value == '1.100000, 2.200000, 3.300000, 4.400000'):
  print "  Testing if double array to string conversion works: SUCCESS"
else:
  print "  Testing if double array to string conversion works: FAIL"
  madara_fails += 1

##TODO: run test for ANY data type

# Make sure kr double is getting cleared 
kr.set(1.000)
kr.clear_value()

if (not kr.exists()):
  print "  Testing if double gets cleared: SUCCESS"
else:
  print "  Testing if double gets cleared: FAIL"
  madara_fails += 1

# Make sure kr integer is getting cleared 
kr.set(1)
kr.clear_value()

if (not kr.exists()):
  print "  Testing if integer gets cleared: SUCCESS"
else:
  print "  Testing if integer gets cleared: FAIL"
  madara_fails += 1

# Make sure kr string is getting cleared 
kr.set('string')
kr.clear_value()

if (not kr.exists()):
  print "  Testing if string gets cleared: SUCCESS"
else:
  print "  Testing if string gets cleared: FAIL"
  madara_fails += 1

# Make sure kr double array is getting cleared 
kr.set (madara.from_pydoubles ([1.100000, 2.200000, 3.300000, 4.400000]))
array = kr.to_doubles ()
kr.clear_value()

if (not kr.exists()):
  print "  Testing if double array gets cleared: SUCCESS"
else:
  print "  Testing if double array gets cleared: FAIL"
  madara_fails += 1

# Make sure kr integer array is getting cleared 
kr.set (madara.from_pylongs ([1, 2, 3, 4]))
array = kr.to_integers ()
kr.clear_value()

if (not kr.exists()):
  print "  Testing if integer array gets cleared: SUCCESS"
else:
  print "  Testing if integer array gets cleared: FAIL"
  madara_fails += 1

# Make sure kr any type is getting cleared 
kr.set(1)
my_value = kr.to_any()
kr.clear_value()

if (not kr.exists()):
  print "  Testing if any type gets cleared: SUCCESS"
else:
  print "  Testing if any type gets cleared: FAIL"
  madara_fails += 1

# Testing get precision method
kr.set(5.2345)
kr.set_precision(3)

if (kr.get_precision() == 3):
  print "  Testing of setting precision: SUCCESS"
else:
  print "  Testing of setting precision: FAIL"
  madara_fails += 1

# Testing set_scientific
kr.set(234.553)
kr.set_scientific()
my_value = kr.to_string()

if (my_value == '2.346e+02'):
  print "  Testing of setting scientific format: SUCCESS"
else:
  print "  Testing of setting scientific format: FAIL"
  madara_fails += 1

# Testing set_fixed
kr.set_fixed()
my_value = kr.to_string()

if (my_value == '234.553'):
  print "  Testing of setting fixed format: SUCCESS"
else:
  print "  Testing of setting fixed format: FAIL"
  madara_fails += 1

# Testing write and read files
kr.set(3.123)
my_value = kr.to_double()
kr.to_file('int.txt')
kr.clear_value()
kr.read_file('int.txt')
my_value2 = kr.to_double()

if (my_value2 == 3.123):
  print "  Testing of write and read files: SUCCESS"
else:
  print "  Testing of write and read files: FAIL"
  madara_fails += 1

# Testing of increment and decrement on integer array 
kr.set(madara.from_pylongs([1, 2, 3, 4]))
kr.inc_index(0)
kr.dec_index(3)

if (kr.to_integers() == 2, 2, 3, 3):
  print "  Testing of increment and decrement on integer array: SUCCESS"
else:
  print "  Testing of increment and decrement on integer array: FAIL"
  madara_fails += 1

# Testing of increment and decrement on double array 
kr.set(madara.from_pydoubles([1.1, 2.2, 3.3, 4.4]))
kr.inc_index(0)
kr.dec_index(3)

if (kr.to_doubles() == 2.1, 2.2, 3.3, 3.4):
  print "  Testing of increment and decrement on double array: SUCCESS"
else:
  print "  Testing of increment and decrement on double array: FAIL"
  madara_fails += 1

# Testing resize functionality on integer array
kr.set(madara.from_pylongs([1, 2, 3, 4]))
kr.resize(6)

if (kr.to_integers() == 1, 2, 3, 4, 0, 0):
  print "  Testing of resize on integer array: SUCCESS"
else:
  print "  Testing of resize on integer array: FAIL"
  madara_fails += 1

# Testing resize functionality on double array
kr.set(madara.from_pydoubles([1.1, 2.2, 3.3, 4.4]))
kr.resize(6)

if (kr.to_doubles() == 1.1, 2.2, 3.3, 4.4, 0, 0):
  print "  Testing of resize on double array: SUCCESS"
else:
  print "  Testing of resize on double array: FAIL"
  madara_fails += 1

# Check if KR is true for int/double/string
kr.set(3)

if (kr.is_true() > 0):
  print "  Finding out if KR is true: SUCCESS"
else:
  print "  Finding out if KR is true: FAIL"
  madara_fails += 1

# Check if KR is false for int/double/string
kr.set(3)
kr.clear_value()

if (kr.is_true() == 0):
  print "  Finding out if KR is false after clearing: SUCCESS"
else:
  print "  Finding out if KR is false after clearing: FAIL"
  madara_fails += 1

# Testing retrieve index for int/double
kr.set(madara.from_pylongs([1, 2, 3, 4]))

if (kr.retrieve_index(3) == 4):
  print "  Retrieve index check: SUCCESS"
else:
  print "  Retrieve index check: FAIL"
  madara_fails += 1

# Make sure kr is getting reset 
kr.set(1)
kr.reset_value()

if (not kr.exists()):
  print "  Testing if KR gets reset: SUCCESS"
else:
  print "  Testing if KR gets reset: FAIL"
  madara_fails += 1

# Find out status of KR
kr.set(1)

if (kr.status() > 0):
  print "  Finding out if KR status is true: SUCCESS"
else:
  print "  Finding out if KR status is true: FAIL"
  madara_fails += 1

# Test if status is zero after resetting KR 
kr.reset_value()

if (kr.status() == 0):
  print "  Finding out if KR status is false: SUCCESS"
else:
  print "  Finding out if KR status is false: FAIL"
  madara_fails += 1

# Finding out size of arrays 
kr.set('hello')

if (kr.size() == 6):
  print "  Finding out if size method works: SUCCESS"
else:
  print "  Finding out if size method works: FAIL"
  madara_fails += 1

# Fragment strings and arrays
kr.set('This is my string')

if (kr.fragment(0, 6) == 'This is'):
  print "  Fragmenting of strings/doubles works: SUCCESS"
else:
  print "  Fragmenting of strings/doubles works: FAIL"
  madara_fails += 1

# Test if matrix can be input into KR
# kr.set(kr.to_any([1, 2, 3, 4],
#           [5, 6, 7, 8]))


if (madara_fails > 0):
  print "\nOVERALL: FAIL: " + str(madara_fails) + " number of failed tests\n"
else:
  print "\nOVERALL: SUCCESS\n"

exit (madara_fails)



# type_dict = {'integer': 1, 'double': 1.123, 'string': 'string', 'integers':
#  [1, 2, 3, 4], 'doubles': [1.111, 2.222, 3.333, 4.444]}






