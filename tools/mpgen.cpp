
#include <stdio.h>
#include <stdlib.h>

#include <string>
#include <vector>
#include <iostream>
#include <ostream>
#include <fstream>
#include <sstream>

#include "madara/utility/Utility.h"

namespace utility = madara::utility;

std::string project_name ("program");
std::string output_dir (".");
std::vector <std::string> headers;
std::vector <std::string> sources;
bool create_sample (false);

// handle command line arguments
void handle_arguments (int argc, char ** argv)
{
  for (int i = 1; i < argc; ++i)
  {
    std::string arg1 (argv[i]);
    
    if (arg1 == "-c" || arg1 == "--create-sample")
    {
      create_sample = true;
    }
    else if (arg1 == "-e" || arg1 == "--header-file")
    {
      if (i + 1 < argc)
      {
        headers.push_back (argv[i + 1]);
      }
      ++i;
    }
    else if (arg1 == "-o" || arg1 == "--output-dir")
    {
      if (i + 1 < argc)
      {
        output_dir = argv[i + 1];
      }
      ++i;
    }
    else if (arg1 == "-p" || arg1 == "--project-name")
    {
      if (i + 1 < argc)
      {
        project_name = argv[i + 1];
      }
      ++i;
    }
    else if (arg1 == "-s" || arg1 == "--source-file")
    {
      if (i + 1 < argc)
      {
        sources.push_back (argv[i + 1]);
      }
      ++i;
    }
    else
    {
      std::cerr <<
        "\nProgram summary for " << argv[0] << " [options]:\n\n" \
        "Generates a platform-neutral project file for compiling MADARA apps."\
        "\n\noptions:\n" \
        "  [-c|--create-sample]     Create sample source file.\n" \
        "  [-e|--header-file file]  Header file to add to project.\n" \
        "  [-o|--output-dir dir]    directory to output to. If the dir\n" \
        "                           does not exist, it will be created.\n" \
        "                           Default output is current directory.\n" \
        "  [-p|--project-name proj] Name for this project. No spaces.\n" \
        "  [-s|--source-file file]  Source cpp file to add to project.\n" \
        "\n";
      exit (0);
    }
  }
}

bool copy_mpbs (void)
{
  bool result (false);
  std::string madara_root = getenv ("MADARA_ROOT");

  if (madara_root != "")
  {
    // read mpb files
    std::string using_ace = utility::file_to_string (madara_root + "/using_ace.mpb");
    std::string using_madara = utility::file_to_string (madara_root + "/using_madara.mpb");

    // write mpb files
    utility::write_file (output_dir + "/using_ace.mpb",
      (void *)using_ace.c_str (), using_ace.size ());
    utility::write_file (output_dir + "/using_madara.mpb",
      (void *)using_madara.c_str (), using_madara.size ());

    result = true;
  }
  else
  {
    std::cerr << "ERROR: Environment variable MADARA_ROOT is not set.\n";
  }

  return result;
}

bool write_workspace_file (void)
{
  bool result (false);
  std::ofstream workspace;
  std::string filename (output_dir + "/" + project_name + ".mwc");

  workspace.open (filename.c_str ());
  if (workspace.is_open ())
  {
    workspace << "workspace {\n";
    workspace << "  " + project_name + ".mpc\n";
    workspace << "}\n";

    result = true;
  }
  else
  {
    std::cerr << "ERROR: Unable to open " << filename << " for writing\n";
  }

  return result;
}

bool write_sample_source (void)
{
  bool result (false);
  std::ofstream sample;
  std::string filename (output_dir + "/" + project_name + ".cpp");

  sample.open (filename.c_str ());
  if (sample.is_open ())
  {
    sample << "#include \"madara/knowledge/KnowledgeBase.h\"\n\n";
    sample << "namespace knowledge = madara::knowledge;\n\n";
    sample << "int main (int argc, char ** argv)\n";
    sample << "{\n";
    sample << "  knowledge::KnowledgeBase knowledge;\n\n";
    sample << "  knowledge.print (\"Hello World!\\n\");\n\n";
    sample << "  return 0;\n";
    sample << "}\n";

    result = true;
  }
  else
  {
    std::cerr << "ERROR: Unable to open " << filename << " for writing\n";
  }

  return result;
}

bool write_project_file (void)
{
  bool result (false);
  std::ofstream project;
  std::string filename (output_dir + "/" + project_name + ".mpc");

  project.open (filename.c_str ());
  if (project.is_open ())
  {
    project << "project (" << project_name <<
               ") : using_madara, using_ace {\n";

    project << "  exeout = bin\n";
    project << "  exename = " << project_name << "\n\n";
    project << "  // The following can disable all warnings on Linux.\n";
    project << "  // verbatim (gnuace, local) {\n";
    project << "  //  CPPFLAGS += -w\n";
    project << "  // }\n\n";
  
    project << "  // Documentation files can be added here\n";
    project << "  DocumentationFiles {\n";
    project << "  }\n\n";
  
    project << "  BuildFiles {\n";
    project << "    " << project_name << ".mwc\n";
    project << "    " << project_name << ".mpc\n";
    project << "  }\n\n";

    project << "  HeaderFiles {\n";

    for (size_t i = 0; i < headers.size (); ++i)
    {
      project << "    " << headers[i] << "\n";
    }

    project << "  }\n\n";

    project << "  SourceFiles {\n";

    for (size_t i = 0; i < sources.size (); ++i)
    {
      project << "    " << sources[i] << "\n";
    }

    if (create_sample)
    {
      project << "    " << project_name << ".cpp\n";
    }

    project << "  }\n";
    project << "}\n";

    result = true;
  }
  else
  {
    std::cerr << "ERROR: Unable to open " << filename << " for writing\n";
  }

  return result;
}

int main (int argc, char ** argv)
{
  // handle all user arguments
  handle_arguments (argc, argv);

  // create the output dir if necessary
  if (output_dir != ".")
  {
    // create the mkdir command for the output directory
    std::string mkdir ("mkdir ");

#ifndef WIN32
    mkdir += "-p ";
#endif

    mkdir += output_dir;

    system (mkdir.c_str ());
  }

  copy_mpbs ();
  write_workspace_file ();
  write_project_file ();

  if (create_sample)
    write_sample_source ();

  return 0;
}