
%module Madara

%rename Knowledge_Base KnowledgeBase;

%{
#include "madara/Knowledge_Engine/Knowledge_Base.h"
%}

//Include std::string support
%include "std_string.i"

//Ignore MADARA_Export
#define MADARA_Export
%include "madara/Knowledge_Engine/Knowledge_Base.h"
