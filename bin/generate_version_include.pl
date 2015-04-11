eval '(exit $?0)' && eval 'exec perl -w -S $0 ${1+"$@"}'
    & eval 'exec perl -w -S $0 $argv:q'
    if 0;
    
###################################################
# @author   James Edmondson <jedmondson@gmail.com>
# 
# This perl script is useful for generating a dynamic
# version number for inclusion with C++. After the
###################################################
    
open VERSION_FILE, "<",  $ENV{'MADARA_ROOT'} . "/VERSION.txt" or
    die "Unable to open ../VERSION.txt for reading.";
  $version = <VERSION_FILE>;
close VERSION_FILE;

chomp ($version);

open OUTPUT_FILE, ">",  $ENV{'MADARA_ROOT'} . "/include/madara/Version.h" or
    die "Unable to open Version.h for writing.";
  print OUTPUT_FILE  "std::string version (\"" . $version . "\");\n";
close OUTPUT_FILE;

