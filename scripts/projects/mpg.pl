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
my @transports;
my @receive_filters;
my @send_filters;
my @threads = ();
my $path = '.';
my ($script, $script_dir) = fileparse($0);
my $verbose;
my $thread_hz = "hertz";

# setup options parser
GetOptions(
  'app|a=s' => \@apps,
  'on-receive|receive=s' => \@receive_filters,
  'on-send|send=s' => \@send_filters,
  'path|dir|p=s' => \$path,
  'thread|t|nt=s' => \@threads,
  'transport|r|nr=s' => \@transports,
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
  --thread|t|nt name        create or use an existing custom thread
  --transport|r|nr name     create or use an existing custom network transport
  --on-send|send name       create or use an on-send filter 
  --on-receive|receive name create or use an on-receive filter 
  --path|-p|--dir dir       the directory path to create custom apps
  --verbose|-v              print detailed debug info\n";
  
  print("$output\n");
  exit 0;
}
else
{
  if ($verbose)
  {
    my $output = "
$script is using the following configuration:
  apps = " . (scalar @apps > 0 ?
    ("\n    " . join ("\n    ", @apps)) : 'no') . "
  thread = " . (scalar @threads > 0 ?
    ("\n    " . join ("\n    ", @threads)) : 'no') . "
  transport = " . (scalar @transports > 0 ?
    ("\n    " . join ("\n    ", @transports)) : 'no') . "
  on_receive = " . (scalar @receive_filters > 0 ?
    ("\n    " . join ("\n    ", @receive_filters)) : 'no') . "
  on_send = " . (scalar @send_filters > 0 ?
    ("\n    " . join ("\n    ", @send_filters)) : 'no') . "
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

my @new_filters = @receive_filters;
push(@new_filters, @send_filters);

for my $filter (@new_filters)
{
  if (not -f "$path/src/filters/$filter.cpp")
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
  else
  {
    print "src/filters/$filter already exists. Not creating a new one.\n"
  }
}

##################### create thread for user ########################

for my $thread (@threads)
{
  if (not -f "$path/src/threads/$thread.cpp")
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
  else
  {
    print "src/threads/$thread already exists. Not creating a new one.\n";
  }
}

##################### create transport for user ########################

for my $transport (@transports)
{
  if (not -f "$path/src/transports/$transport.cpp")
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
  else
  {
    print "src/transports/$transport already exists. Not creating a new one.\n";
  }
}

##################### create app for user ########################

for my $app (@apps)
{
  if (not -f "$path/src/$app.cpp")
  {
    copy "$script_dir/common/src/app.cpp", "$path/src/$app.cpp"
      or die "Copy failed: $!";
  }
  else
  {
    print "src/$app already exists. Not creating a new one.\n";
  }

  # read the contents of the $sim_path/env.mf file
  my $contents;
  open app_file, "$path/src/$app.cpp" or
    die "ERROR: Couldn't open $path/src/$app.cpp\n"; 
    $contents = join("", <app_file>); 
  close app_file;

  my @threads = @threads;
  my $thread_includes;
  my $thread_creation;


  if (scalar @threads > 0)
  {
    if ($verbose)
    {
      print ("  Custom threads detected. Updating...\n");
    }
    
    for (my $i = 0; $i < scalar @threads; ++$i)
    {
      $thread_includes .= "\n#include \"threads/" .
        $threads[$i] . ".h\"";            
      $thread_creation .= "
  threader.run(${thread_hz}, \"${threads[$i]}\", new threads::${threads[$i]}());";
    }
    
    # change the includes         
$contents =~
  s/\/\/ begin thread includes(.|\s)*\/\/ end thread includes/\/\/ begin thread includes${thread_includes}\n\/\/ end thread includes/;
    
    # change the creation process
$contents =~
  s/\/\/ begin thread creation(.|\s)*\/\/ end thread creation/\/\/ begin thread creation${thread_creation}\n  \/\/ end thread creation/;
  } # end if there are custom threads
  
  my @transports = @transports;
  my $transport_includes;
  my $transport_creation;

  if (scalar @transports > 0)
  {
    if ($verbose)
    {
      print ("  Custom transports detected. Updating...\n");
    }
    
    for (my $i = 0; $i < scalar @transports; ++$i)
    {
      $transport_includes .= "\n#include \"transports/" .
        $transports[$i] . ".h\"";            
      $transport_creation .= "
  kb.attach_transport(new transports::${transports[$i]} (
    kb.get_id(), settings, kb));";
    }
    
    # change the includes         
$contents =~
  s/\/\/ begin transport includes(.|\s)*\/\/ end transport includes/\/\/ begin transport includes${transport_includes}\n\/\/ end transport includes/;
    
    # change the creation process
$contents =~
  s/\/\/ begin transport creation(.|\s)*\/\/ end transport creation/\/\/ begin transport creation${transport_creation}\n  \/\/ end transport creation/;
  } # end if there are custom threads
  
  my $receive_filter_includes;
  my $receive_filter_creation;

  if (scalar @receive_filters > 0)
  {
    if ($verbose)
    {
      print ("  Custom receive filters detected. Updating...\n");
    }
    
    if (not $contents =~ /\/\/ end filter includes/)
    {
      $contents =~
  s/\/\/ end transport includes/\/\/ end transport includes\n\n\/\/ begin filter includes\n\/\/ end filter includes/;
    }
    
    if (not $contents =~ /\/\/ end on receive filters/)
    {
      $contents =~
  s/KnowledgeBase kb;/KnowledgeBase kb;\n\n  \/\/ begin on receive filters\n  \/\/ end on receive filters/;
    }

    for (my $i = 0; $i < scalar @receive_filters; ++$i)
    {
      if (not $contents =~ /#include \"filters\/${receive_filters[$i]}\.h\"/)
      {
        $receive_filter_includes .= "\n#include \"filters/${receive_filters[$i]}.h\"";
      }
      $receive_filter_creation .= "
  filters::${receive_filters[$i]} receive_filter_$i;
  settings.add_receive_filter(&receive_filter_$i);";
    }
    
    if ($receive_filter_includes)
    {
      # change the includes         
      $contents =~
  s/[\n \r]+\/\/ end filter includes/${receive_filter_includes}\n\/\/ end filter includes/;
    }
    # change the creation process
    $contents =~
  s/\/\/ begin on receive filters(.|\s)*\/\/ end on receive filters/\/\/ begin on receive filters${receive_filter_creation}\n  \/\/ end on receive filters/;

  } # end if there are custom read filters

  my $send_filter_includes;
  my $send_filter_creation;
  
  
  if (scalar @send_filters > 0)
  {
    if ($verbose)
    {
      print ("  Custom send filters detected. Updating...\n");
    }
    
    if (not $contents =~ /\/\/ end filter includes/)
    {
    $contents =~
  s/\/\/ end transport includes/\/\/ end transport includes\n\n\/\/ begin filter includes\n\/\/ end filter includes/;
    }
    
    if (not $contents =~ /\/\/ end on send filters/)
    {
    $contents =~
  s/KnowledgeBase kb;/KnowledgeBase kb;\n\n  \/\/ begin on send filters\n  \/\/ end on send filters/;
    }

    for (my $i = 0; $i < scalar @send_filters; ++$i)
    {
      if (not $contents =~ /#include \"filters\/${send_filters[$i]}\.h\"/)
      {
        $send_filter_includes .= "\n#include \"filters/${send_filters[$i]}.h\"";
      }
      $send_filter_creation .= "
  filters::${send_filters[$i]} send_filter_$i;
  settings.add_send_filter(&send_filter_$i);";
    }
    
    if ($send_filter_includes)
    {
      # change the includes         
      $contents =~
  s/[\n \r]+\/\/ end filter includes/${send_filter_includes}\n\/\/ end filter includes/;
    }
    
    # change the creation process
    $contents =~
  s/\/\/ begin on send filters(.|\s)*\/\/ end on send filters/\/\/ begin on send filters${send_filter_creation}\n  \/\/ end on send filters/;

  } # end if there are custom read filters

  open app_file, ">$path/src/$app.cpp" or
    die "ERROR: Couldn't open $path/src/$app.cpp\n"; 
    print app_file $contents; 
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
  close project_file;
}
