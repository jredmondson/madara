//###############################################################################################################################################
//
//	Meta and Builder Object Network V2.0 for GME
//	BON2Component.cpp
//
//###############################################################################################################################################

/*
	Copyright (c) Vanderbilt University, 2000-2004
	ALL RIGHTS RESERVED

	Vanderbilt University disclaims all warranties with regard to this
	software, including all implied warranties of merchantability
	and fitness.  In no event shall Vanderbilt University be liable for
	any special, indirect or consequential damages or any damages
	whatsoever resulting from loss of use, data or profits, whether
	in an action of contract, negligence or other tortious action,
	arising out of or in connection with the use or performance of
	this software.
*/

#include "stdafx.h"
#include <sstream>
#include <vector>
#include <algorithm>
#include <Console.h>
#include "BON2Component.h"
#include "../Utility/tinyxml.h"
#include "../Utility/Utility.h"

// I have to paste this into KATSBonX.h/cpp whenever I change
// the paradigm
//namespace KATS_BON
//{
//  class Process
//  {
//  public:
//    bool operator< (const Process & right)
//    {
//      if(this->getOrder () < right->getOrder ())
//        return true;
//
//      return this->getId () < right->getId ();
//    }
//  };
//}


namespace BON
{

  const char * interpreter_version = "0.4.12";

  /**
   * Comparison for derived classes of Ordered
   * @param   lhs   left hand side argument
   * @param   rhs   right hand side argument
   **/
  bool Comparison (KATS_BON::Ordered & lhs,
    KATS_BON::Ordered & rhs)
  {
    return (lhs->getOrder () < rhs->getOrder ());
  }


//###############################################################################################################################################
//
// 	C L A S S : BON::Component
//
//###############################################################################################################################################

Component::Component()
	: m_bIsInteractive( false )
{
}

Component::~Component()
{
	if( m_project ) {
		m_project->finalizeObjects();
		finalize( m_project );
		m_project = NULL;
	}
}

// ====================================================
// This method is called after all the generic initialization is done
// This should be empty unless application-specific initialization is needed

void Component::initialize( Project& project )
{
	// ======================
	// Insert application specific code here
#ifdef GME_ADDON
	project->setAutoCommit( false);
#endif
}

// ====================================================
// This method is called before the whole BON2 project released and disposed
// This should be empty unless application-specific finalization is needed

void Component::finalize( Project& project )
{
	// ======================
	// Insert application specific code here
}

// ====================================================
// This is the obsolete component interface
// This present implementation either tries to call InvokeEx, or does nothing except of a notification

void Component::invoke( Project& project, const std::set<FCO>& setModels, long lParam )
{
	#ifdef SUPPORT_OLD_INVOKE
		Object focus;
		invokeEx( project, focus, setModels, lParam );
	#else
		if( m_bIsInteractive )
			AfxMessageBox("This BON2 Component does not support the obsolete invoke mechanism!");
	#endif
}


void Component::process_process_base (KATS_BON::ProcessBase & current,
                                      TiXmlElement & xml_setup)
{
  // did the user set debug mode?
  if(current->isDebug ())
  {
    TiXmlElement element ("debug");
    xml_setup.InsertEndChild (element);
  }
  
  // did the user set realtime scheduling?
  if(current->isRealtime ())
  {
    TiXmlElement element ("realtime");
    xml_setup.InsertEndChild (element);
  }

  // does the user want timing information printed?
  if(current->isTiming ())
  {
    TiXmlElement element ("timing");
    xml_setup.InsertEndChild (element);
  }


  // get the transport
  std::set<KATS_BON::TransportRef> transports = current->getTransportRef ();
  std::set<KATS_BON::TransportRef>::iterator tbegin = transports.begin();
  if(tbegin != transports.end ())
  {
    KATS_BON::TransportRef tref = *tbegin;
    KATS_BON::Transport transport = tref->getTransport ();

    Util::GenRefCounted * ref_ptr = transport.getCounted (false);

    TiXmlElement element ("transport");

    // check to see if the user initialized the reference to an
    // actual transport
    if(ref_ptr)
    {
      std::stringstream buffer;
      buffer << "transports/";
      buffer << transport->getName ();
      buffer << ".xml";

      element.SetAttribute ("file", buffer.str ().c_str ());
      xml_setup.InsertEndChild (element);
    }
  }
 

  // get the host name
  std::set<KATS_BON::HostRef> hosts = current->getHostRef ();
  std::set<KATS_BON::HostRef>::iterator hbegin = hosts.begin();
  if(hbegin != hosts.end ())
  {
    KATS_BON::HostRef href = *hbegin;
    KATS_BON::Host host = href->getHost ();

    Util::GenRefCounted * ref_ptr = host.getCounted (false);

    TiXmlElement element ("host");

    // check to see if the user initialized the reference to an
    // actual instance. If not, we use the reference's name as
    // a convenience
    if(ref_ptr)
    {
      // we have a real reference
      TiXmlText text (host->getName ().c_str ());
      if(host->getOverride () != "")
      {
        text.SetValue (host->getOverride ().c_str ());
      }

      text.SetValue (KATS::Utility::expand_model_vars (current,
        text.ValueStr ()));

      element.InsertEndChild (text);
    }
    else
    {
      TiXmlText text (href->getName ().c_str ());

      text.SetValue (KATS::Utility::expand_model_vars (current,
        text.ValueStr ()));

      element.InsertEndChild (text);
    }
    xml_setup.InsertEndChild (element);
  }

  // get the domain name
  std::set<KATS_BON::DomainRef> domains = current->getDomainRef ();
  std::set<KATS_BON::DomainRef>::iterator dbegin = domains.begin();
  if(dbegin != domains.end ())
  {
    KATS_BON::DomainRef dref = *dbegin;
    KATS_BON::Domain domain = dref->getDomain ();

    Util::GenRefCounted * ref_ptr = domain.getCounted (false);

    TiXmlElement element ("domain");

    // check to see if the user initialized the reference to an
    // actual instance. If not, we use the reference's name as
    // a convenience
    if(ref_ptr)
    {
      // we have a real reference
      TiXmlText text (domain->getName ().c_str ());
      if(domain->getOverride () != "")
      {
        text.SetValue (domain->getOverride ().c_str ());
      }

      text.SetValue (KATS::Utility::expand_model_vars (current,
        text.ValueStr ()));

      element.InsertEndChild (text);
    }
    else
    {
      TiXmlText text (dref->getName ().c_str ());

      text.SetValue (KATS::Utility::expand_model_vars (current,
        text.ValueStr ()));

      element.InsertEndChild (text);
    }
    xml_setup.InsertEndChild (element);
  }

  // get the barrier name
  std::set<KATS_BON::BarrierRef> barriers = current->getBarrierRef ();
  std::set<KATS_BON::BarrierRef>::iterator bbegin = barriers.begin();
  size_t barrier_count = 0;
  bool override_processes = false;
  if(bbegin != barriers.end ())
  {
    KATS_BON::BarrierRef bref = *bbegin;
    KATS_BON::Barrier barrier = bref->getBarrier ();

    Util::GenRefCounted * ref_ptr = barrier.getCounted (false);

    TiXmlElement element ("barrier");

    // check to see if the user initialized the reference to an
    // actual instance. If not, we use the reference's name as
    // a convenience
    if(ref_ptr)
    {
      // set the override_processes based on the boolean attribute

      override_processes = bref->isOverrideProcesses ();
      barrier_count = barrier->getReferredBy ().size ();

      // we have a real reference
      if(barrier->getOverride () != "")
        element.SetAttribute ("name", barrier->getOverride ().c_str ());
      else
        element.SetAttribute ("name", barrier->getName ().c_str ());
    }
    else
      element.SetAttribute ("name", bref->getName ().c_str ());

    element.SetAttribute ("name", KATS::Utility::expand_model_vars (current, 
                          element.Attribute ("name")));

    xml_setup.InsertEndChild (element);
  }
 
  // did the user set an id in the process ring?
  if(current->getId () != "")
  {
    std::stringstream buffer;
    buffer << current->getId ();

    TiXmlElement element ("id");
    TiXmlText text (buffer.str ().c_str ());

    text.SetValue (KATS::Utility::expand_model_vars (current,
      text.ValueStr ()));

    element.InsertEndChild (text);

    xml_setup.InsertEndChild (element);
  }

  // did the user set the number of processes in the barrier?
  if(override_processes || current->getProcesses () != "")
  {
    std::stringstream buffer;
    buffer << current->getProcesses ();

    TiXmlElement element ("processes");
    TiXmlText text (buffer.str ().c_str ());

    if(override_processes)
    {
      std::stringstream buffer;
      buffer << barrier_count;
      text.SetValue (buffer.str ());
    }
    else
    {
      text.SetValue (KATS::Utility::expand_model_vars (current,
        text.ValueStr ()));
    }

    element.InsertEndChild (text);

    xml_setup.InsertEndChild (element);
  }

  // did the user set an log level?
  if(current->getLogLevel () != "")
  {
    std::stringstream buffer;
    buffer << current->getLogLevel ();

    TiXmlElement element ("loglevel");
    TiXmlText text (buffer.str ().c_str ());

    text.SetValue (KATS::Utility::expand_model_vars (current,
      text.ValueStr ()));

    element.InsertEndChild (text);

    xml_setup.InsertEndChild (element);
  }

  // did the user set a stderr redirect?
  if(current->getDuplicates () != "")
  {
    TiXmlElement element ("duplicates");
    TiXmlText text (current->getDuplicates ().c_str ());

    text.SetValue (KATS::Utility::expand_model_vars (current,
      text.ValueStr ()));

    element.InsertEndChild (text);
    xml_setup.InsertEndChild (element);
  }

  // did the user set a delay?
  if(current->getDelay () != "")
  {
    std::stringstream buffer;
    buffer << current->getDelay ();

    TiXmlElement element ("delay");
    TiXmlText text (buffer.str ().c_str ());

    text.SetValue (KATS::Utility::expand_model_vars (current,
      text.ValueStr ()));

    element.InsertEndChild (text);

    xml_setup.InsertEndChild (element);
  }
  
  // did the user set a precondition?
  if(current->getPrecondition () != "")
  {
    TiXmlElement element ("precondition");
    TiXmlText text (current->getPrecondition ().c_str ());

    text.SetValue (KATS::Utility::expand_model_vars (current,
      text.ValueStr ()));

    element.InsertEndChild (text);
    xml_setup.InsertEndChild (element);
  }

  // did the user set a postdelay?
  if(current->getPostDelay () != "")
  {
    TiXmlElement element ("postdelay");
    TiXmlText text (current->getPostDelay ().c_str ());

    text.SetValue (KATS::Utility::expand_model_vars (current,
      text.ValueStr ()));

    element.InsertEndChild (text);
    xml_setup.InsertEndChild (element);
  }

  // did the user set a postlaunch?
  if(current->getPostLaunch () != "")
  {
    TiXmlElement element ("postlaunch");
    TiXmlText text (current->getPostLaunch ().c_str ());

    text.SetValue (KATS::Utility::expand_model_vars (current,
      text.ValueStr ()));

    element.InsertEndChild (text);
    xml_setup.InsertEndChild (element);
  }

  // did the user set a postcondition?
  if(current->getPostcondition () != "")
  {
    TiXmlElement element ("postcondition");
    TiXmlText text (current->getPostcondition ().c_str ());

    text.SetValue (KATS::Utility::expand_model_vars (current,
      text.ValueStr ()));

    element.InsertEndChild (text);
    xml_setup.InsertEndChild (element);
  }

  // what is the working directory?
  if(current->getWorkingDir () != "")
  {
    TiXmlElement element ("workingdir");
    TiXmlText text (current->getWorkingDir ().c_str ());

    text.SetValue (KATS::Utility::expand_model_vars (current,
      text.ValueStr ()));

    element.InsertEndChild (text);
    xml_setup.InsertEndChild (element);
  }

  // get the kill settings
  std::set<KATS_BON::Kill> kills = current->getKill ();
  std::set<KATS_BON::Kill>::iterator kbegin = kills.begin();
  if(kbegin != kills.end ())
  {
    KATS_BON::Kill kill = *kbegin;
    if(kill->getTime () != "")
    {
      TiXmlElement element ("kill");
      TiXmlElement time ("time");

      std::stringstream time_buff;
      time_buff << kill->getTime ();
      TiXmlText time_text (time_buff.str ().c_str ());

      time_text.SetValue (KATS::Utility::expand_model_vars (current,
        time_text.ValueStr ()));

      time.InsertEndChild (time_text);

      element.InsertEndChild (time);

      // has a signal been defined?
      if(kill->getSignal () != "")
      {
        TiXmlElement signal ("signal");

        // insert the signal into the xml element
        std::stringstream signal_buff;
        signal_buff << kill->getSignal ();
        TiXmlText signal_text (signal_buff.str ().c_str ());

        signal_text.SetValue (KATS::Utility::expand_model_vars (current,
          signal_text.ValueStr ()));

        signal.InsertEndChild (signal_text);

        element.InsertEndChild (signal);
      }

      xml_setup.InsertEndChild (element);
    }
  }

  // did the user set a stdin redirect?
  if(current->getStdin () != "")
  {
    TiXmlElement element ("stdin");
    TiXmlText text (current->getStdin ().c_str ());

    text.SetValue (KATS::Utility::expand_model_vars (current,
      text.ValueStr ()));

    element.InsertEndChild (text);
    xml_setup.InsertEndChild (element);
  }

  // did the user set a stderr redirect?
  if(current->getStderr () != "")
  {
    TiXmlElement element ("stderr");
    TiXmlText text (current->getStderr ().c_str ());

    text.SetValue (KATS::Utility::expand_model_vars (current,
      text.ValueStr ()));

    element.InsertEndChild (text);
    xml_setup.InsertEndChild (element);
  }

  // did the user set a stdout redirect?
  if(current->getStdout () != "")
  {
    TiXmlElement element ("stdout");

    TiXmlText text (current->getStdout ().c_str ());

    text.SetValue (KATS::Utility::expand_model_vars (current,
      text.ValueStr ()));

    element.InsertEndChild (text);

    xml_setup.InsertEndChild (element);
  }
}

void Component::process_group_ref (KATS_BON::GroupRef & current,
                                 TiXmlElement & parent)
{
  KATS_BON::Group group = current->getGroup ();

  // if the user has actually redirect pasted a group into this ref,
  // then create a new xml element
  if(group.getCounted (false))
  {
    TiXmlElement xml_setup ("group");

    std::stringstream buffer;
    buffer << group->getName ();
    buffer << ".xml";

    xml_setup.SetAttribute ("file", buffer.str ());

    // do we want to override the group's settings?
    if(current->isOverrideSettings ())
    {
      // did the user set debug mode?
      if(current->isDebug ())
      {
        TiXmlElement element ("debug");
        xml_setup.InsertEndChild (element);
      }
      
      // did the user set realtime scheduling?
      if(current->isRealtime ())
      {
        TiXmlElement element ("realtime");
        xml_setup.InsertEndChild (element);
      }

      // does the user want timing information printed?
      if(current->isTiming ())
      {
        TiXmlElement element ("timing");
        xml_setup.InsertEndChild (element);
      }

      // did the user set an id in the process ring?
      if(current->getId () != "")
      {
        std::stringstream buffer;
        buffer << current->getId ();

        TiXmlElement element ("id");
        TiXmlText text (buffer.str ().c_str ());

        text.SetValue (KATS::Utility::expand_model_vars (current,
          text.ValueStr ()));

        element.InsertEndChild (text);

        xml_setup.InsertEndChild (element);
      }

      // did the user set the number of processes in the barrier?
      if(current->getProcesses () != "")
      {
        std::stringstream buffer;
        buffer << current->getProcesses ();

        TiXmlElement element ("processes");
        TiXmlText text (buffer.str ().c_str ());

        text.SetValue (KATS::Utility::expand_model_vars (current,
          text.ValueStr ()));

        element.InsertEndChild (text);

        xml_setup.InsertEndChild (element);
      }

      // did the user set an log level?
      if(current->getLogLevel () != "")
      {
        std::stringstream buffer;
        buffer << current->getLogLevel ();

        TiXmlElement element ("loglevel");
        TiXmlText text (buffer.str ().c_str ());

        text.SetValue (KATS::Utility::expand_model_vars (current,
          text.ValueStr ()));

        element.InsertEndChild (text);

        xml_setup.InsertEndChild (element);
      }

      // did the user set a delay?
      if(current->getDelay () != "")
      {
        std::stringstream buffer;
        buffer << current->getDelay ();

        TiXmlElement element ("delay");
        TiXmlText text (buffer.str ().c_str ());

        text.SetValue (KATS::Utility::expand_model_vars (current,
          text.ValueStr ()));

        element.InsertEndChild (text);

        xml_setup.InsertEndChild (element);
      }
      
      // did the user set a host?
      if(current->getOverrideHost () != "")
      {
        TiXmlElement element ("host");
        TiXmlText text (current->getOverrideHost ().c_str ());

        text.SetValue (KATS::Utility::expand_model_vars (current,
          text.ValueStr ()));

        element.InsertEndChild (text);
        xml_setup.InsertEndChild (element);
      }

      // did the user set a host?
      if(current->getOverrideDomain () != "")
      {
        TiXmlElement element ("domain");
        TiXmlText text (current->getOverrideDomain ().c_str ());

        text.SetValue (KATS::Utility::expand_model_vars (current,
          text.ValueStr ()));

        element.InsertEndChild (text);
        xml_setup.InsertEndChild (element);
      }

      // did the user set a barrier?
      if(current->getOverrideBarrier () != "")
      {
        TiXmlElement element ("barrier");
        TiXmlText text (current->getOverrideBarrier ().c_str ());

        text.SetValue (KATS::Utility::expand_model_vars (current,
          text.ValueStr ()));

        element.SetAttribute ("name", text.Value ());

        xml_setup.InsertEndChild (element);
      }
 
      // did the user set a precondition?
      if(current->getPrecondition () != "")
      {
        TiXmlElement element ("precondition");
        TiXmlText text (current->getPrecondition ().c_str ());

        text.SetValue (KATS::Utility::expand_model_vars (current,
          text.ValueStr ()));

        element.InsertEndChild (text);
        xml_setup.InsertEndChild (element);
      }

      // did the user set a postdelay?
      if(current->getPostDelay () != "")
      {
        TiXmlElement element ("postdelay");
        TiXmlText text (current->getPostDelay ().c_str ());

        text.SetValue (KATS::Utility::expand_model_vars (current,
          text.ValueStr ()));

        element.InsertEndChild (text);
        xml_setup.InsertEndChild (element);
      }

      // did the user set a postlaunch?
      if(current->getPostLaunch () != "")
      {
        TiXmlElement element ("postlaunch");
        TiXmlText text (current->getPostLaunch ().c_str ());

        text.SetValue (KATS::Utility::expand_model_vars (current,
          text.ValueStr ()));

        element.InsertEndChild (text);
        xml_setup.InsertEndChild (element);
      }

      // did the user set a postcondition?
      if(current->getPostcondition () != "")
      {
        TiXmlElement element ("postcondition");
        TiXmlText text (current->getPostcondition ().c_str ());

        text.SetValue (KATS::Utility::expand_model_vars (current,
          text.ValueStr ()));

        element.InsertEndChild (text);
        xml_setup.InsertEndChild (element);
      }

      // what is the working directory?
      if(current->getWorkingDir () != "")
      {
        TiXmlElement element ("workingdir");
        TiXmlText text (current->getWorkingDir ().c_str ());

        text.SetValue (KATS::Utility::expand_model_vars (current,
          text.ValueStr ()));

        element.InsertEndChild (text);
        xml_setup.InsertEndChild (element);
      }

      // did the user set a stdin redirect?
      if(current->getStdin () != "")
      {
        TiXmlElement element ("stdin");
        TiXmlText text (current->getStdin ().c_str ());

        text.SetValue (KATS::Utility::expand_model_vars (current,
          text.ValueStr ()));

        element.InsertEndChild (text);
        xml_setup.InsertEndChild (element);
      }

      // did the user set a stderr redirect?
      if(current->getStderr () != "")
      {
        TiXmlElement element ("stderr");
        TiXmlText text (current->getStderr ().c_str ());

        text.SetValue (KATS::Utility::expand_model_vars (current,
          text.ValueStr ()));

        element.InsertEndChild (text);
        xml_setup.InsertEndChild (element);
      }

      // did the user set a stderr redirect?
      if(current->getDuplicates () != "")
      {
        TiXmlElement element ("duplicates");
        TiXmlText text (current->getDuplicates ().c_str ());

        text.SetValue (KATS::Utility::expand_model_vars (current,
          text.ValueStr ()));

        element.InsertEndChild (text);
        xml_setup.InsertEndChild (element);
      }

      // did the user set a stdout redirect?
      if(current->getStdout () != "")
      {
        TiXmlElement element ("stdout");

        TiXmlText text (current->getStdout ().c_str ());

        text.SetValue (KATS::Utility::expand_model_vars (current,
          text.ValueStr ()));

        element.InsertEndChild (text);

        xml_setup.InsertEndChild (element);
      }
    }

    parent.InsertEndChild (xml_setup);
  }
}

void Component::process_process (KATS_BON::Process & current,
                                 TiXmlElement & parent)
{
  TiXmlElement xml_setup ("process");

  // fill in the process base xml settings
  process_process_base (current, xml_setup);

  // what is the executable?
  if(current->getExecutable () != "")
  {
    TiXmlElement element ("executable");
    TiXmlText text (current->getExecutable ().c_str ());

    text.SetValue (KATS::Utility::expand_model_vars (current,
      text.ValueStr ()));

    element.InsertEndChild (text);
    xml_setup.InsertEndChild (element);
  }

  // what is the working directory?
  if(current->getCommandLine () != "")
  {
    TiXmlElement element ("commandline");
    TiXmlText text (current->getCommandLine ().c_str ());

    text.SetValue (KATS::Utility::expand_model_vars (current,
      text.ValueStr ()));

    element.InsertEndChild (text);
    xml_setup.InsertEndChild (element);
  }

  parent.InsertEndChild (xml_setup);
}

void Component::process_sleep (KATS_BON::Sleep & current,
                                 TiXmlElement & parent)
{
  TiXmlElement xml_setup ("sleep");

  // fill in the process base xml settings
  process_process_base (current, xml_setup);

  parent.InsertEndChild (xml_setup);
}

void Component::process_observer (KATS_BON::Observer & current,
                                 TiXmlElement & parent)
{
  TiXmlElement xml_setup ("observer");

  // fill in the process base xml settings
  process_process_base (current, xml_setup);

  parent.InsertEndChild (xml_setup);
}

void Component::process_process_group (KATS_BON::Group & current)
{
	using namespace GMEConsole;
  Console::Out::WriteLine(CString ("......Processing the process group ") + 
      current->getName ().c_str () + "...");

  TiXmlElement xml_group ("group");
  TiXmlElement xml_setup ("setup");

  // did the user set parallel process launch?
  if(current->isParallel ())
  {
    TiXmlElement element ("parallel");
    xml_setup.InsertEndChild (element);
  }

  // fill in the process base xml settings
  process_process_base (current, xml_setup);

  xml_group.InsertEndChild (xml_setup);

  // iterate through all of the processes

  std::set <KATS_BON::Ordered> ordereds = current->getOrdered ();

  std::vector <KATS_BON::Ordered> sorted_candidates;

  for(std::set <KATS_BON::Ordered>::iterator ordered_i = ordereds.begin ();
                        ordered_i != ordereds.end (); ++ordered_i)
  {
    //process_process (*process_i, xml_group);
    if((*ordered_i)->isEnabled ())
      sorted_candidates.push_back (*ordered_i);
  }

  std::sort (sorted_candidates.begin (), sorted_candidates.end (),
    Comparison);

  for(size_t i = 0; i < sorted_candidates.size (); ++i)
  {
    // this is probably not the most efficient way to handle this
    // but it is also not likely to take that long and is easy to follow

    // try to cast to each ordered type
    KATS_BON::Process process = sorted_candidates[i];
    KATS_BON::GroupRef group_ref = sorted_candidates[i];
    KATS_BON::Sleep sleep = sorted_candidates[i];
    KATS_BON::Observer observer = sorted_candidates[i];

    // check to see if the cast resulted in a valid reference counted object
    if(process.getCounted (false))
      process_process (process, xml_group);
    else if(group_ref.getCounted (false))
      process_group_ref (group_ref, xml_group);
    else if(sleep.getCounted (false))
      process_sleep (sleep, xml_group);
    else if(observer.getCounted (false))
      process_observer (observer, xml_group);
  }

  // write the file out
  {
    // create an xml directory
    ::CreateDirectory ("xml", 0);
    std::stringstream buffer;
    buffer << "xml\\";
    buffer << current->getName ();
    buffer << ".xml";

    TiXmlDocument xml_doc (buffer.str ().c_str ());
    xml_doc.InsertEndChild (xml_group);

    Console::Out::WriteLine(CString ("......Writing to file ") + 
      buffer.str ().c_str ());

    xml_doc.SaveFile (buffer.str ().c_str ());
  }
}

void Component::process_transport (KATS_BON::Transport & current)
{
	using namespace GMEConsole;
  Console::Out::WriteLine(CString ("......Processing the transport ") + 
      current->getName ().c_str () + "...");

  TiXmlElement xml_setup ("transport");

  // what is the type?
  {
    TiXmlElement element ("type");

    char * types [] = {
      "None",
      "Splice",
      "NDDS"
    };

    TiXmlText text (types[current->getType ()]);

    element.InsertEndChild (text);
    xml_setup.InsertEndChild (element);
  }

  // what is the persistence?
  {
    TiXmlElement element ("persistence");

    char * types [] = {
      "Volatile",
      "Persistent"
    };

    TiXmlText text (types[current->getPersistence ()]);

    element.InsertEndChild (text);
    xml_setup.InsertEndChild (element);
  }

  // write the file out
  {
    // create an xml directory
    ::CreateDirectory ("xml", 0);
    ::CreateDirectory ("xml\\transports", 0);
    std::stringstream buffer;
    buffer << "xml\\transports\\";
    buffer << current->getName ();
    buffer << ".xml";

    TiXmlDocument xml_doc (buffer.str ().c_str ());
    xml_doc.InsertEndChild (xml_setup);

    Console::Out::WriteLine(CString ("......Writing to file ") + 
      buffer.str ().c_str ());

    xml_doc.SaveFile (buffer.str ().c_str ());
  }
}

void Component::process_transports_folder (KATS_BON::Transports & current)
{
	using namespace GMEConsole;
	Console::Out::WriteLine("....Iterating through process groups...");
  std::set<KATS_BON::Transport> groups = current->getTransport ();
  for(std::set<KATS_BON::Transport>::iterator group = groups.begin(); group != groups.end(); ++group)
  {
    process_transport (*group);
  }
}

void Component::process_processes_folder (KATS_BON::Processes & process_folder)
{
	using namespace GMEConsole;
	Console::Out::WriteLine("....Iterating through process groups...");
  std::set<KATS_BON::Group> groups = process_folder->getGroup ();
  for(std::set<KATS_BON::Group>::iterator group = groups.begin(); group != groups.end(); ++group)
  {
    process_process_group (*group);
  }
}

// ====================================================
// This is the main component method for Interpereters and Plugins.
// May also be used in case of invokeable Add-Ons

void Component::invokeEx( Project& project, FCO& currentFCO, const std::set<FCO>& setSelectedFCOs, long lParam )
{
#ifdef GME_ADDON
	project->setAutoCommit( false);
#endif
	using namespace GMEConsole;
  std::stringstream buffer;
  buffer << "KATS GenerateXML Interpreter version ";
  buffer << interpreter_version;
  buffer << "...";

  Console::Out::WriteLine(buffer.str ().c_str ());
	Console::Out::WriteLine("Processing KATS model...");
	// ======================
	// TODO: Insert application specific code here

	Console::Out::WriteLine("..Iterating through Processes folders...");

  std::set<BON::Folder> rootFolders = project->getRootFolder ()->getChildFolders ();
  for(std::set<BON::Folder>::iterator folder = rootFolders.begin(); folder != rootFolders.end(); ++folder)
  {
    if((*folder)->getFolderMeta ().name () == "Processes")
    {
      KATS_BON::Processes process_folder = (*folder);
      Console::Out::WriteLine(CString ("....Processing ") +
        process_folder->getName ().c_str () + " folder...");
      process_processes_folder (process_folder);
    }
    else if((*folder)->getFolderMeta ().name () == "Transports")
    {
      KATS_BON::Transports transports_folder = (*folder);
      Console::Out::WriteLine(CString ("....Processing ") +
        transports_folder->getName ().c_str () + " folder...");
      process_transports_folder (transports_folder);
    }
  }

	Console::Out::WriteLine("Finished processing KATS model...");
}

// ====================================================
// GME currently does not use this function
// You only need to implement it if other invokation mechanisms are used

void Component::objectInvokeEx( Project& project, Object& currentObject, const std::set<Object>& setSelectedObjects, long lParam )
	{
		if( m_bIsInteractive )
			AfxMessageBox("This BON2 Component does not support objectInvokeEx method!");
	}

// ====================================================
// Implement application specific parameter-mechanism in these functions

Util::Variant Component::getParameter( const std::string& strName )
{
	// ======================
	// Insert application specific code here

	return Util::Variant();
}

void Component::setParameter( const std::string& strName, const Util::Variant& varValue )
{
	// ======================
	// Insert application specific code here
}

#ifdef GME_ADDON

// ====================================================
// If the component is an Add-On, then this method is called for every Global Event

void Component::globalEventPerformed( globalevent_enum event )
{
	// ======================
	// Insert application specific code here
}

// ====================================================
// If the component is an Add-On, then this method is called for every Object Event

void Component::objectEventPerformed( Object& object, unsigned long event, VARIANT v )
{
	// ======================
	// Insert application specific code here
}

#endif // GME_ADDON

}; // namespace BON

