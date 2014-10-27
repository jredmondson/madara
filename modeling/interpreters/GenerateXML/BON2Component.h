//###############################################################################################################################################
//
//	Meta and Builder Object Network V2.0 for GME
//	BON2Component.h
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

#ifndef BON2Component_h
#define BON2Component_h

#include "BON.h"
#include "BONImpl.h"
#include <ComponentConfig.h>
#include "KATSBonX.h"
#include "../Utility/tinyxml.h"

namespace BON
{

//###############################################################################################################################################
//
// 	C L A S S : BON::Component
//
//###############################################################################################################################################

class Component
{
	//==============================================================
	// IMPLEMENTOR SPECIFIC PART
	// Insert application specific members and method deifinitions here

	//==============================================================
	// BON2 SPECIFIC PART
	// Do not modify anything below

	// Member variables
	public :
		Project 	m_project;
		bool		m_bIsInteractive;

	public:
		Component();
		~Component();

	public:
		void initialize( Project& project );
		void finalize( Project& project );

    void process_transport (KATS_BON::Transport & transport);
    void process_transports_folder (KATS_BON::Transports & folder);
    void process_process_base (KATS_BON::ProcessBase & current,
                                      TiXmlElement & xml_setup);
    void process_process (KATS_BON::Process & process,
                          TiXmlElement & parent);
    void process_sleep (KATS_BON::Sleep & sleep,
                          TiXmlElement & parent);
    void process_observer (KATS_BON::Observer & observer,
                          TiXmlElement & parent);
    void process_group_ref (KATS_BON::GroupRef & group_ref,
                          TiXmlElement & parent);
    void process_process_group (KATS_BON::Group & group);
    void process_processes_folder (KATS_BON::Processes & process_folder);
		void invoke( Project& project, const std::set<FCO>& setModels, long lParam );
		void invokeEx( Project& project, FCO& currentFCO, const std::set<FCO>& setSelectedFCOs, long lParam );
		void objectInvokeEx( Project& project, Object& currentObject, const std::set<Object>& setSelectedObjects, long lParam );
		Util::Variant getParameter( const std::string& strName );
		void setParameter( const std::string& strName, const Util::Variant& varValue );

	#ifdef GME_ADDON
		void globalEventPerformed( globalevent_enum event );
		void objectEventPerformed( Object& object, unsigned long event, VARIANT v );
	#endif
};

}; // namespace BON

#endif // Bon2Component_H