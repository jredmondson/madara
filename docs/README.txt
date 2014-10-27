MADARA HELP DOCUMENTATION INFORMATION

0. INTRODUCTION

  VERSION: 0.9.42
  CONTACT: James Edmondson <jedmondson@gmail.com>
  WEBSITE: http://madara.googlecode.com

  This file provides general overviews about the help documentation in this
  directory and where to find additional help documentation where applicable.
  The other files in this directory are input for the Doxygen program to 
  generate the latest documentation for all of the MADARA tools.

1. OBTAINING PREREQUISITES

  Project: Doxygen (free)
  URL: http://www.doxygen.org

       Click on the downloads link and download the binaries/setup file for
       your operating system. It is unlikely that you will want to download
       the source. On many Linux systems, you can use the system package
       manager of your choice to download Doxygen. Most modern versions
       should work.

  Project: Graphviz (free)
  URL: http://www.graphviz.org

       Click on downloads and navigate to your operating system. Without this
       package, you will not be able to generate helpful documentation.

2. GENERATING DEVELOPER DOCUMENTATION

  2.1. KaRL

       The (K)nowledge (a)nd (R)easoning (L)anguage is a module of MADARA that
       supports distributed knowledge and reasoning for real-time and
       continuous systems. Generating documentation is as easy as typing the
       following from the $MADARA_ROOT/docs directory in a terminal
       ('cmd' in Windows):

         doxygen Doxyfile_KaRL

  2.2. KATS

       The (K)aRL (A)utomated (T)esting (S)uite is a module of MADARA that
       provides a distributed automated testing framework for blackbox or
       whitebox testing. Generating documentation is as easy as typing the
       following from the $MADARA_ROOT/docs directory in a terminal
       ('cmd' in Windows):

         doxygen Doxyfile_KATS

       For blackbox testing of heavy weight processes, you
       will probably find the built-in help more useful. Built-in help is
       provided for kats_batch, kats_process, kats_observer, and kats_sleep.
       Simply provide -h or --help to these processes for more information,
       like the following:

         $MADARA_ROOT/bin/kats_batch --help
         $MADARA_ROOT/bin/kats_process --help
         $MADARA_ROOT/bin/kats_observer --help
         $MADARA_ROOT/bin/kats_sleep --help

  2.3. MAML

       The (M)adara (A)ndroid (M)onkeyrunner (L)ibrary is a Python library for
       automating Android smart phone testing. The library provides an
       intuitive encapsulation of Android Monkeyrunner for KeyEvent generation
       which requires no installation of additional packages on the phone.
       Generating documentation is as easy as typing the
       following from the $MADARA_ROOT/docs directory in a terminal
       ('cmd' in Windows):

         doxygen Doxyfile_MAML

       MAML requires an Android smart phone to be plugged into the computer
       you are running commands from. We plan on supporting running the maml
       library directly from a command line with GME modeling support, but for
       now, you will need to import the library into another Python script that
       you will be writing the application specific testing logic into.

       Importing MAML can be done via the following (at the top of your
       Python file):

         from com.android.monkeyrunner import MonkeyRunner, MonkeyDevice
         sys.path.append(os.environ['MADARA_ROOT'] + '/lib')
         import maml

       The first line imports the Android MonkeyRunner library, which we use in
       $MADARA_ROOT/lib/maml.py. The next two lines add the $MADAR_ROOT/lib dir
       to the system path, and import the maml library, using the additions to
       the system path. We also provide a non-MonkeyRunner version of the
       library, but this is experimental and will be much slower as it uses
       adb directly and the 'adb shell input' calls create and destroy shells
       with every KeyInput. We plan to investigate optimizing this library
       as time permits. The adb shell issues should be fixable. 

         sys.path.append(os.environ['MADARA_ROOT'] + '/lib')
         import maml_adb_only

3 COMMITTING DEVELOPER DOCUMENTATION

Developer documentation is made available online at:

https://madara.googlecode.com/svn/docs/karl/html/index.html

Updating this documentation is done through the SVN repository by enabling
certain auto-props in your client side SVN settings and checking out the
docs repo to the docs/karl directory. The process for getting your machine
configured looks like the following:

  3.1. Configuring SVN

      Open your Subversion Configuration File. On TortoiseSVN (Windows), this
      is done by right clicking on a folder, going to TortoiseSVN->Settings,
      and then pressing the Edit button next to the Subversion Configuration 
      File on the General tab. Linux/Mac users should be able to edit the file
      at ~/.subversion/config. Inside of the file, you need to do the following:

      1) Uncomment or add "enable-auto-props = yes"
      2) Add the following to the config file after the [auto-props] tag. The
         only necessary ones are the *.css, *.html, *.js, *.gif, and *.png for
         Doxygen. The others are to ensure native line endings for your host
         OS during checkout/commit.


*.c = svn:eol-style=native
*.cpp = svn:eol-style=native
*.h = svn:eol-style=native
*.mpb = sv:eol-style=native
*.mpc = sv:eol-style=native
*.mwc = sv:eol-style=native
*.sh = svn:eol-style=native
*.txt = svn:eol-style=native
*.jpg = svn:mime-type=image/jpeg
*.css  = svn:mime-type=text/css
*.html = svn:mime-type=text/html
*.js   = svn:mime-type=text/javascript
*.gif  = svn:mime-type=image/gif
*.png  = svn:mime-type=image/png

  3.2. Checking out the Docs repo

      The doxygen documentation is not included in the trunk to provide a smaller
      checkout for main code development. To checkout or commit docs, you'll need
      to configure your directories to the appropriate svn directory.

      Because the "karl" directory is generated into the current folder, we recommend
      using this structure to your advantage. Create a "karl" directory in this folder
      or delete everything out of the "karl" directory if it exists and do an svn
      checkout to that folder that points to https://madara.googlecode.com/svn/docs/karl.
