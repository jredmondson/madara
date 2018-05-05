eval '(exit $?0)' && eval 'exec perl -w -S $0 ${1+"$@"}'
    & eval 'exec perl -w -S $0 $argv:q'
    if 0;

###################################################
# @author   James Edmondson <jedmondson@gmail.com>
#
# This perl script is useful for generating a dynamic
# version number for inclusion with C++. After the
###################################################

my $txt_fname = $ENV{'MADARA_ROOT'} . "/VERSION.txt";
my $h_fname = $ENV{'MADARA_ROOT'} . "/include/madara/Version.h";
my $txt_mdate = (stat $txt_fname)[9];
my $h_mdate = (stat $h_fname)[9];

if ($txt_mdate < $h_mdate) {
  exit 0;
}

open VERSION_FILE, "<", $txt_fname  or
    die "Unable to open ../VERSION.txt for reading.";
  $version = <VERSION_FILE>;
close VERSION_FILE;

chomp ($version);

$git_sha = `git rev-parse HEAD`;
chomp ($git_sha);

open OUTPUT_FILE, ">",  $h_fname or
    die "Unable to open Version.h for writing.";
  print OUTPUT_FILE  "std::string version (\"" . $version . " (" . $git_sha . ")\");\n";
close OUTPUT_FILE;

