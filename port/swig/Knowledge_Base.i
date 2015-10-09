
%module Madara

%{
#include "madara/knowledge/KnowledgeBase.h"
%}

//Include std::string support
%include "std_string.i"

//Ignore MADARA_Export
#define MADARA_Export
%include "madara/knowledge/KnowledgeBase.h"
