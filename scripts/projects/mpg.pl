#!/usr/bin/perl

###################################################
# @brief The MADARA Project Generator
# @author James Edmondson <jedmondson@gmail.com>
# @date 2018
###################################################

use strict;
use warnings;
use Getopt::Long qw(GetOptions);
use File::Path qw(make_path);
use File::Basename;
use File::Copy qw(copy);
use POSIX;
use Scalar::Util qw(looks_like_number);

my @apps = ();
my $help;
my $madararoot = $ENV{MADARA_ROOT};
my @new_apps = ();
my @new_transports;
my @new_receive_filters;
my @new_send_filters;
my @new_threads = ();
my $path = '.';
my ($script, $script_dir) = fileparse($0);
my $verbose;

# setup options parser
GetOptions(
  'app|a=s' => \@new_apps,
  'on-receive|receive=s' => \@new_receive_filters,
  'on-send|send=s' => \@new_send_filters,
  'path|dir|p=s' => \$path,
  'new-thread|nt=s' => \@new_threads,
  'new-transport|nr=s' => \@new_transports,
  'verbose|v' => \$verbose
  ) or $help = "yes";

#check for help request  
if ($help)
{
  my $output = " 
$script purpose:

  Configures MADARA applications

options:
  --app|-a name             adds or updates a generated MADARA application
  --new-thread|nt name      create infrastructure for custom thread
  --new-transport|nr name   create infrastructure for custom network transport
  --on-send|send name       create an on-send filter 
  --on-receive|receive name create an on-receive filter 
  --path|-p|--dir dir       the directory path to a simulation
  --verbose|-v              print detailed debug info\n";
  
  print("$output\n");
}
else
{
  if ($verbose)
  {
    my $output = "
$script is using the following configuration:
  apps = " . (scalar @new_apps > 0 ?
    ("\n    " . join ("\n    ", @new_apps)) : 'no') . "
  new_thread = " . (scalar @new_threads > 0 ?
    ("\n    " . join ("\n    ", @new_threads)) : 'no') . "
  new_transport = " . (scalar @new_transports > 0 ?
    ("\n    " . join ("\n    ", @new_transports)) : 'no') . "
  on_receive = " . (scalar @new_receive_filters > 0 ?
    ("\n    " . join ("\n    ", @new_receive_filters)) : 'no') . "
  on_send = " . (scalar @new_send_filters > 0 ?
    ("\n    " . join ("\n    ", @new_send_filters)) : 'no') . "
  path = $path
  verbose = yes\n";
  
    print("$output\n");
  }
}

# create all useful directories

my $bin_path = "$path/bin";
my $docs_path = "$path/docs";
my $filters_path = "$path/src/filters";
my $src_path = "$path/src";
my $threads_path = "$path/src/threads";
my $transports_path = "$path/src/transports";

if (not -d "$path")
{
  make_path("$path");
}

if (not -d "$bin_path")
{
  make_path("$bin_path");
}

if (not -d "$docs_path")
{
  make_path("$docs_path");
}

if (not -d "$filters_path")
{
  make_path("$filters_path");
}

if (not -d "$src_path")
{
  make_path("$src_path");
}

if (not -d "$threads_path")
{
  make_path("$threads_path");
}

if (not -d "$transports_path")
{
  make_path("$transports_path");
}

# copy useful files to root

copy "$script_dir/common/doxygen_help_gen.mpb", "$path/";
copy "$script_dir/common/using_boost.mpb", "$path/";
copy "$script_dir/common/using_capnp.mpb", "$path/";
copy "$script_dir/common/using_madara.mpb", "$path/";
copy "$script_dir/common/using_zmq.mpb", "$path/";
copy "$script_dir/common/VERSION.txt", "$path/";
copy "$script_dir/common/workspace.mwc", "$path/";
copy "$script_dir/linux/action.sh", "$path/";
copy "$script_dir/windows/action.bat", "$path/";

chmod 0755, "$path/action.sh";

# copy useful files to docs

copy "$script_dir/common/docs/Documentation.mpc", "$path/docs/";
copy "$script_dir/common/docs/Doxyfile.dxy", "$path/docs/";
copy "$script_dir/common/docs/get_version.pl", "$path/docs/";
copy "$script_dir/common/docs/MainPage.md", "$path/docs/";

# copy useful files to src

copy "$script_dir/common/src/Namespaces.h", "$path/src/";

##################### create filter for user ########################

my @new_filters = @new_receive_filters;
push(@new_filters, @new_send_filters);

for my $filter (@new_filters)
{
  copy "$script_dir/common/src/filter.cpp", "$path/src/filters/$filter.cpp"
    or die "Copy failed: $!";
  copy "$script_dir/common/src/filter.h", "$path/src/filters/$filter.h"
    or die "Copy failed: $!";

  # save an upper case version of the filter name
  my $filter_uc = uc $filter;

  # read the cpp contents and insert our filter name
  my $contents;
  open cpp_file, "$path/src/filters/$filter.cpp" or
    die "ERROR: Couldn't open $path/src/filters/$filter.cpp\n"; 
    $contents = join("", <cpp_file>); 
  close cpp_file;

  $contents =~ s/MyFilter/$filter/g;

  open cpp_file, ">$path/src/filters/$filter.cpp" or
    die "ERROR: Couldn't open $path/src/filters/$filter.cpp\n"; 
    print cpp_file $contents;
  close cpp_file;

  # read the header contents and insert our filter name
  open h_file, "$path/src/filters/$filter.h" or
    die "ERROR: Couldn't open $path/src/filters/$filter.h\n"; 
    $contents = join("", <h_file>); 
  close h_file;

  $contents =~ s/MyFilter/$filter/g;
  $contents =~ s/MYFILTER/$filter_uc/g;

  open h_file, ">$path/src/filters/$filter.h" or
    die "ERROR: Couldn't open $path/src/filters/$filter.h\n"; 
    print h_file $contents;
  close h_file;
}

##################### create thread for user ########################

for my $thread (@new_threads)
{
  copy "$script_dir/common/src/thread.cpp", "$path/src/threads/$thread.cpp"
    or die "Copy failed: $!";
  copy "$script_dir/common/src/thread.h", "$path/src/threads/$thread.h"
    or die "Copy failed: $!";

  # save an upper case version of the thread name
  my $thread_uc = uc $thread;

  # read the cpp contents and insert our thread name
  my $contents;
  open cpp_file, "$path/src/threads/$thread.cpp" or
    die "ERROR: Couldn't open $path/src/threads/$thread.cpp\n"; 
    $contents = join("", <cpp_file>); 
  close cpp_file;

  $contents =~ s/MyThread/$thread/g;

  open cpp_file, ">$path/src/threads/$thread.cpp" or
    die "ERROR: Couldn't open $path/src/threads/$thread.cpp\n"; 
    print cpp_file $contents;
  close cpp_file;

  # read the header contents and insert our thread name
  open h_file, "$path/src/threads/$thread.h" or
    die "ERROR: Couldn't open $path/src/threads/$thread.h\n"; 
    $contents = join("", <h_file>); 
  close h_file;

  $contents =~ s/MyThread/$thread/g;
  $contents =~ s/MYTHREAD/$thread_uc/g;

  open h_file, ">$path/src/threads/$thread.h" or
    die "ERROR: Couldn't open $path/src/threads/$thread.h\n"; 
    print h_file $contents;
  close h_file;
}

##################### create transport for user ########################

for my $transport (@new_transports)
{
  copy "$script_dir/common/src/transport.cpp", "$path/src/transports/$transport.cpp"
    or die "Copy failed: $!";
  copy "$script_dir/common/src/transport.h", "$path/src/transports/$transport.h"
    or die "Copy failed: $!";
  copy "$script_dir/common/src/transport_thread.cpp", "$path/src/transports/${transport}ReadThread.cpp"
    or die "Copy failed: $!";
  copy "$script_dir/common/src/transport_thread.h", "$path/src/transports/${transport}ReadThread.h"
    or die "Copy failed: $!";


  # save an upper case version of the transport name
  my $transport_uc = uc $transport;

  # read the cpp contents and insert our transport name
  my $contents;
  open cpp_file, "$path/src/transports/$transport.cpp" or
    die "ERROR: Couldn't open $path/src/transports/$transport.cpp\n"; 
    $contents = join("", <cpp_file>); 
  close cpp_file;

  $contents =~ s/MyTransport/$transport/g;

  open cpp_file, ">$path/src/transports/$transport.cpp" or
    die "ERROR: Couldn't open $path/src/transports/$transport.cpp\n"; 
    print cpp_file $contents;
  close cpp_file;

  # read the cpp contents and insert our transport name
  open cpp_file, "$path/src/transports/${transport}ReadThread.cpp" or
    die "ERROR: Couldn't open $path/src/transports/${transport}ReadThread.cpp\n"; 
    $contents = join("", <cpp_file>); 
  close cpp_file;

  $contents =~ s/MyTransport/$transport/g;

  open cpp_file, ">$path/src/transports/${transport}ReadThread.cpp" or
    die "ERROR: Couldn't open $path/src/transports/${transport}ReadThread.cpp\n"; 
    print cpp_file $contents;
  close cpp_file;

  # read the header contents and insert our transport name
  open h_file, "$path/src/transports/$transport.h" or
    die "ERROR: Couldn't open $path/src/transports/$transport.h\n"; 
    $contents = join("", <h_file>); 
  close h_file;

  $contents =~ s/MyTransport/$transport/g;
  $contents =~ s/MYTRANSPORT/$transport_uc/g;

  open h_file, ">$path/src/transports/$transport.h" or
    die "ERROR: Couldn't open $path/src/transports/$transport.h\n"; 
    print h_file $contents;
  close h_file;

  # read the header contents and insert our transport name
  open h_file, "$path/src/transports/${transport}ReadThread.h" or
    die "ERROR: Couldn't open $path/src/transports/${transport}ReadThread.h\n"; 
    $contents = join("", <h_file>); 
  close h_file;

  $contents =~ s/MyTransport/$transport/g;
  $contents =~ s/MYTRANSPORT/$transport_uc/g;

  open h_file, ">$path/src/transports/${transport}ReadThread.h" or
    die "ERROR: Couldn't open $path/src/transports/${transport}ReadThread.h\n"; 
    print h_file $contents;
  close h_file;
}

##################### create app for user ########################

for my $app (@new_apps)
{
  copy "$script_dir/common/src/app.cpp", "$path/src/$app.cpp"
    or die "Copy failed: $!";

  # read the contents of the $sim_path/env.mf file
  my $contents;
  open app_file, "$path/src/$app.cpp" or
    die "ERROR: Couldn't open $path/src/$app.cpp\n"; 
    $contents = join("", <app_file>); 
  close app_file;


  $contents = "
project ($app) : using_madara {
  exeout = bin
  exename = $app

  macros +=  _USE_MATH_DEFINES

  Documentation_Files {
    README.txt
  }

  Header_Files {
    src/
    src/filters
    src/threads
    src/transports
  }

  Source_Files {
    src/$app.cpp
    src/filters
    src/threads
    src/transports
  }
}
";

  open project_file, ">>$path/project.mpc" or
    die "ERROR: Couldn't open $path/project.mpc\n"; 
    print project_file $contents; 
  close app_file;
}
