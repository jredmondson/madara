#include "stdafx.h"
#include "KATSBonX.h"
#include "KATSVisitor.h"


namespace BON
{

IMPLEMENT_BONEXTENSION( KATS_BON::Barriers, "Barriers" );
IMPLEMENT_BONEXTENSION( KATS_BON::Domains, "Domains" );
IMPLEMENT_BONEXTENSION( KATS_BON::Hosts, "Hosts" );
IMPLEMENT_BONEXTENSION( KATS_BON::Processes, "Processes" );
IMPLEMENT_BONEXTENSION( KATS_BON::Transports, "Transports" );
IMPLEMENT_ABSTRACT_BONEXTENSION( KATS_BON::ConfigureBase );
IMPLEMENT_ABSTRACT_BONEXTENSION( KATS_BON::Ordered );
IMPLEMENT_ABSTRACT_BONEXTENSION( KATS_BON::ProcessBase );
IMPLEMENT_BONEXTENSION( KATS_BON::GroupRef, "GroupRef" );
IMPLEMENT_BONEXTENSION( KATS_BON::Group, "Group" );
IMPLEMENT_BONEXTENSION( KATS_BON::Observer, "Observer" );
IMPLEMENT_BONEXTENSION( KATS_BON::Process, "Process" );
IMPLEMENT_BONEXTENSION( KATS_BON::Sleep, "Sleep" );
IMPLEMENT_BONEXTENSION( KATS_BON::Transport, "Transport" );
IMPLEMENT_BONEXTENSION( KATS_BON::Barrier, "Barrier" );
IMPLEMENT_BONEXTENSION( KATS_BON::Domain, "Domain" );
IMPLEMENT_BONEXTENSION( KATS_BON::Host, "Host" );
IMPLEMENT_BONEXTENSION( KATS_BON::Kill, "Kill" );
IMPLEMENT_BONEXTENSION( KATS_BON::BarrierRef, "BarrierRef" );
IMPLEMENT_BONEXTENSION( KATS_BON::DomainRef, "DomainRef" );
IMPLEMENT_BONEXTENSION( KATS_BON::HostRef, "HostRef" );
IMPLEMENT_BONEXTENSION( KATS_BON::TransportRef, "TransportRef" );


} // namespace BON

//********************************************************************************
// 
//********************************************************************************
void KATS_BON::BarriersImpl::accept( KATS_BON::KATSVisitor *pVisitor)
{
	// visit the KATS_BON::Barriers
	pVisitor->visitBarriers( KATS_BON::Barriers( this));

	// then its children
	std::set<BON::Folder> subfolders = BON::FolderImpl::getChildFolders();
	for( std::set<BON::Folder>::const_iterator it = subfolders.begin(); it != subfolders.end(); ++it)
	{
		(*it)->accept( pVisitor);
	}

	std::set<BON::FCO> children = BON::FolderImpl::getRootFCOs();
	for( std::set<BON::FCO>::const_iterator it = children.begin(); it != children.end(); ++it)
	{
		(*it)->accept( pVisitor);
	}
}


//********************************************************************************
// getter for kind "Barrier"
//********************************************************************************
std::set<KATS_BON::Barrier> KATS_BON::BarriersImpl::getBarrier()
{
	std::set<KATS_BON::Barrier> res;
	std::set<BON::Object> kinds = FolderImpl::getChildObjects("Barrier");
	for( std::set<BON::Object>::iterator i = kinds.begin(); i != kinds.end(); ++i)
	{
		KATS_BON::Barrier elem(*i);
		ASSERT(elem);
		res.insert(elem);
	}
	return res;
}


//********************************************************************************
// 
//********************************************************************************
void KATS_BON::DomainsImpl::accept( KATS_BON::KATSVisitor *pVisitor)
{
	// visit the KATS_BON::Domains
	pVisitor->visitDomains( KATS_BON::Domains( this));

	// then its children
	std::set<BON::Folder> subfolders = BON::FolderImpl::getChildFolders();
	for( std::set<BON::Folder>::const_iterator it = subfolders.begin(); it != subfolders.end(); ++it)
	{
		(*it)->accept( pVisitor);
	}

	std::set<BON::FCO> children = BON::FolderImpl::getRootFCOs();
	for( std::set<BON::FCO>::const_iterator it = children.begin(); it != children.end(); ++it)
	{
		(*it)->accept( pVisitor);
	}
}


//********************************************************************************
// getter for kind "Domain"
//********************************************************************************
std::set<KATS_BON::Domain> KATS_BON::DomainsImpl::getDomain()
{
	std::set<KATS_BON::Domain> res;
	std::set<BON::Object> kinds = FolderImpl::getChildObjects("Domain");
	for( std::set<BON::Object>::iterator i = kinds.begin(); i != kinds.end(); ++i)
	{
		KATS_BON::Domain elem(*i);
		ASSERT(elem);
		res.insert(elem);
	}
	return res;
}


//********************************************************************************
// 
//********************************************************************************
void KATS_BON::HostsImpl::accept( KATS_BON::KATSVisitor *pVisitor)
{
	// visit the KATS_BON::Hosts
	pVisitor->visitHosts( KATS_BON::Hosts( this));

	// then its children
	std::set<BON::Folder> subfolders = BON::FolderImpl::getChildFolders();
	for( std::set<BON::Folder>::const_iterator it = subfolders.begin(); it != subfolders.end(); ++it)
	{
		(*it)->accept( pVisitor);
	}

	std::set<BON::FCO> children = BON::FolderImpl::getRootFCOs();
	for( std::set<BON::FCO>::const_iterator it = children.begin(); it != children.end(); ++it)
	{
		(*it)->accept( pVisitor);
	}
}


//********************************************************************************
// getter for kind "Host"
//********************************************************************************
std::set<KATS_BON::Host> KATS_BON::HostsImpl::getHost()
{
	std::set<KATS_BON::Host> res;
	std::set<BON::Object> kinds = FolderImpl::getChildObjects("Host");
	for( std::set<BON::Object>::iterator i = kinds.begin(); i != kinds.end(); ++i)
	{
		KATS_BON::Host elem(*i);
		ASSERT(elem);
		res.insert(elem);
	}
	return res;
}


//********************************************************************************
// 
//********************************************************************************
void KATS_BON::ProcessesImpl::accept( KATS_BON::KATSVisitor *pVisitor)
{
	// visit the KATS_BON::Processes
	pVisitor->visitProcesses( KATS_BON::Processes( this));

	// then its children
	std::set<BON::Folder> subfolders = BON::FolderImpl::getChildFolders();
	for( std::set<BON::Folder>::const_iterator it = subfolders.begin(); it != subfolders.end(); ++it)
	{
		(*it)->accept( pVisitor);
	}

	std::set<BON::FCO> children = BON::FolderImpl::getRootFCOs();
	for( std::set<BON::FCO>::const_iterator it = children.begin(); it != children.end(); ++it)
	{
		(*it)->accept( pVisitor);
	}
}


//********************************************************************************
// getter for kind "Group"
//********************************************************************************
std::set<KATS_BON::Group> KATS_BON::ProcessesImpl::getGroup()
{
	std::set<KATS_BON::Group> res;
	std::set<BON::Object> kinds = FolderImpl::getChildObjects("Group");
	for( std::set<BON::Object>::iterator i = kinds.begin(); i != kinds.end(); ++i)
	{
		KATS_BON::Group elem(*i);
		ASSERT(elem);
		res.insert(elem);
	}
	return res;
}


//********************************************************************************
// 
//********************************************************************************
void KATS_BON::TransportsImpl::accept( KATS_BON::KATSVisitor *pVisitor)
{
	// visit the KATS_BON::Transports
	pVisitor->visitTransports( KATS_BON::Transports( this));

	// then its children
	std::set<BON::Folder> subfolders = BON::FolderImpl::getChildFolders();
	for( std::set<BON::Folder>::const_iterator it = subfolders.begin(); it != subfolders.end(); ++it)
	{
		(*it)->accept( pVisitor);
	}

	std::set<BON::FCO> children = BON::FolderImpl::getRootFCOs();
	for( std::set<BON::FCO>::const_iterator it = children.begin(); it != children.end(); ++it)
	{
		(*it)->accept( pVisitor);
	}
}


//********************************************************************************
// getter for kind "Transport"
//********************************************************************************
std::set<KATS_BON::Transport> KATS_BON::TransportsImpl::getTransport()
{
	std::set<KATS_BON::Transport> res;
	std::set<BON::Object> kinds = FolderImpl::getChildObjects("Transport");
	for( std::set<BON::Object>::iterator i = kinds.begin(); i != kinds.end(); ++i)
	{
		KATS_BON::Transport elem(*i);
		ASSERT(elem);
		res.insert(elem);
	}
	return res;
}


//********************************************************************************
// 
//********************************************************************************
void KATS_BON::ConfigureBaseImpl::accept( KATS_BON::KATSVisitor *pVisitor)
{
	// visit the KATS_BON::ConfigureBase
	pVisitor->visitConfigureBase( KATS_BON::ConfigureBase( this));
}


//********************************************************************************
// 
//********************************************************************************
std::string KATS_BON::ConfigureBaseImpl::getDelay() 
{
	return FCOImpl::getAttribute("Delay")->getStringValue();
}


//********************************************************************************
// 
//********************************************************************************
std::string KATS_BON::ConfigureBaseImpl::getDuplicates() 
{
	return FCOImpl::getAttribute("Duplicates")->getStringValue();
}


//********************************************************************************
// 
//********************************************************************************
std::string KATS_BON::ConfigureBaseImpl::getId() 
{
	return FCOImpl::getAttribute("Id")->getStringValue();
}


//********************************************************************************
// 
//********************************************************************************
std::string KATS_BON::ConfigureBaseImpl::getLogLevel() 
{
	return FCOImpl::getAttribute("LogLevel")->getStringValue();
}


//********************************************************************************
// 
//********************************************************************************
std::string KATS_BON::ConfigureBaseImpl::getPostDelay() 
{
	return FCOImpl::getAttribute("PostDelay")->getStringValue();
}


//********************************************************************************
// 
//********************************************************************************
std::string KATS_BON::ConfigureBaseImpl::getPostLaunch() 
{
	return FCOImpl::getAttribute("PostLaunch")->getStringValue();
}


//********************************************************************************
// 
//********************************************************************************
std::string KATS_BON::ConfigureBaseImpl::getPostcondition() 
{
	return FCOImpl::getAttribute("Postcondition")->getStringValue();
}


//********************************************************************************
// 
//********************************************************************************
std::string KATS_BON::ConfigureBaseImpl::getPrecondition() 
{
	return FCOImpl::getAttribute("Precondition")->getStringValue();
}


//********************************************************************************
// 
//********************************************************************************
std::string KATS_BON::ConfigureBaseImpl::getProcesses() 
{
	return FCOImpl::getAttribute("Processes")->getStringValue();
}


//********************************************************************************
// 
//********************************************************************************
std::string KATS_BON::ConfigureBaseImpl::getStderr() 
{
	return FCOImpl::getAttribute("Stderr")->getStringValue();
}


//********************************************************************************
// 
//********************************************************************************
std::string KATS_BON::ConfigureBaseImpl::getStdin() 
{
	return FCOImpl::getAttribute("Stdin")->getStringValue();
}


//********************************************************************************
// 
//********************************************************************************
std::string KATS_BON::ConfigureBaseImpl::getStdout() 
{
	return FCOImpl::getAttribute("Stdout")->getStringValue();
}


//********************************************************************************
// 
//********************************************************************************
std::string KATS_BON::ConfigureBaseImpl::getWorkingDir() 
{
	return FCOImpl::getAttribute("WorkingDir")->getStringValue();
}


//********************************************************************************
// 
//********************************************************************************
bool KATS_BON::ConfigureBaseImpl::isDebug() 
{
	return FCOImpl::getAttribute("Debug")->getBooleanValue();
}


//********************************************************************************
// 
//********************************************************************************
bool KATS_BON::ConfigureBaseImpl::isRealtime() 
{
	return FCOImpl::getAttribute("Realtime")->getBooleanValue();
}


//********************************************************************************
// 
//********************************************************************************
bool KATS_BON::ConfigureBaseImpl::isTiming() 
{
	return FCOImpl::getAttribute("Timing")->getBooleanValue();
}


//********************************************************************************
// 
//********************************************************************************
void KATS_BON::ConfigureBaseImpl::setDebug( bool val)
{
	FCOImpl::getAttribute("Debug")->setBooleanValue( val);
}


//********************************************************************************
// 
//********************************************************************************
void KATS_BON::ConfigureBaseImpl::setDelay( const std::string& val)
{
	FCOImpl::getAttribute("Delay")->setStringValue( val);
}


//********************************************************************************
// 
//********************************************************************************
void KATS_BON::ConfigureBaseImpl::setDuplicates( const std::string& val)
{
	FCOImpl::getAttribute("Duplicates")->setStringValue( val);
}


//********************************************************************************
// 
//********************************************************************************
void KATS_BON::ConfigureBaseImpl::setId( const std::string& val)
{
	FCOImpl::getAttribute("Id")->setStringValue( val);
}


//********************************************************************************
// 
//********************************************************************************
void KATS_BON::ConfigureBaseImpl::setLogLevel( const std::string& val)
{
	FCOImpl::getAttribute("LogLevel")->setStringValue( val);
}


//********************************************************************************
// 
//********************************************************************************
void KATS_BON::ConfigureBaseImpl::setPostDelay( const std::string& val)
{
	FCOImpl::getAttribute("PostDelay")->setStringValue( val);
}


//********************************************************************************
// 
//********************************************************************************
void KATS_BON::ConfigureBaseImpl::setPostLaunch( const std::string& val)
{
	FCOImpl::getAttribute("PostLaunch")->setStringValue( val);
}


//********************************************************************************
// 
//********************************************************************************
void KATS_BON::ConfigureBaseImpl::setPostcondition( const std::string& val)
{
	FCOImpl::getAttribute("Postcondition")->setStringValue( val);
}


//********************************************************************************
// 
//********************************************************************************
void KATS_BON::ConfigureBaseImpl::setPrecondition( const std::string& val)
{
	FCOImpl::getAttribute("Precondition")->setStringValue( val);
}


//********************************************************************************
// 
//********************************************************************************
void KATS_BON::ConfigureBaseImpl::setProcesses( const std::string& val)
{
	FCOImpl::getAttribute("Processes")->setStringValue( val);
}


//********************************************************************************
// 
//********************************************************************************
void KATS_BON::ConfigureBaseImpl::setRealtime( bool val)
{
	FCOImpl::getAttribute("Realtime")->setBooleanValue( val);
}


//********************************************************************************
// 
//********************************************************************************
void KATS_BON::ConfigureBaseImpl::setStderr( const std::string& val)
{
	FCOImpl::getAttribute("Stderr")->setStringValue( val);
}


//********************************************************************************
// 
//********************************************************************************
void KATS_BON::ConfigureBaseImpl::setStdin( const std::string& val)
{
	FCOImpl::getAttribute("Stdin")->setStringValue( val);
}


//********************************************************************************
// 
//********************************************************************************
void KATS_BON::ConfigureBaseImpl::setStdout( const std::string& val)
{
	FCOImpl::getAttribute("Stdout")->setStringValue( val);
}


//********************************************************************************
// 
//********************************************************************************
void KATS_BON::ConfigureBaseImpl::setTiming( bool val)
{
	FCOImpl::getAttribute("Timing")->setBooleanValue( val);
}


//********************************************************************************
// 
//********************************************************************************
void KATS_BON::ConfigureBaseImpl::setWorkingDir( const std::string& val)
{
	FCOImpl::getAttribute("WorkingDir")->setStringValue( val);
}


//********************************************************************************
// 
//********************************************************************************
void KATS_BON::OrderedImpl::accept( KATS_BON::KATSVisitor *pVisitor)
{
	// visit the KATS_BON::Ordered
	pVisitor->visitOrdered( KATS_BON::Ordered( this));
}


//********************************************************************************
// 
//********************************************************************************
long KATS_BON::OrderedImpl::getOrder() 
{
	return FCOImpl::getAttribute("Order")->getIntegerValue();
}


//********************************************************************************
// 
//********************************************************************************
bool KATS_BON::OrderedImpl::isEnabled() 
{
	return FCOImpl::getAttribute("Enabled")->getBooleanValue();
}


//********************************************************************************
// 
//********************************************************************************
void KATS_BON::OrderedImpl::setEnabled( bool val)
{
	FCOImpl::getAttribute("Enabled")->setBooleanValue( val);
}


//********************************************************************************
// 
//********************************************************************************
void KATS_BON::OrderedImpl::setOrder( const long val)
{
	FCOImpl::getAttribute("Order")->setIntegerValue( val);
}


//********************************************************************************
// 
//********************************************************************************
void KATS_BON::ProcessBaseImpl::accept( KATS_BON::KATSVisitor *pVisitor)
{
	// visit the KATS_BON::ProcessBase
	pVisitor->visitProcessBase( KATS_BON::ProcessBase( this));

	// then its children
	std::set<BON::FCO> children = ModelImpl::getChildFCOs();
	for( std::set<BON::FCO>::const_iterator it = children.begin(); it != children.end(); ++it)
	{
		(*it)->accept( pVisitor);
	}
}


//********************************************************************************
// getter for role "BarrierRef" among "KATS_BON::BarrierRef"s
//********************************************************************************
std::set<KATS_BON::BarrierRef> KATS_BON::ProcessBaseImpl::getBarrierRef()
{
	std::set<KATS_BON::BarrierRef> res;
	std::set<BON::FCO> roles = ModelImpl::getChildFCOsAs("BarrierRef");
	for( std::set<BON::FCO>::iterator i = roles.begin(); i != roles.end(); ++i)
	{
		KATS_BON::BarrierRef elem(*i);
		ASSERT(elem);
		res.insert(elem);
	}
	return res;
}


//********************************************************************************
// getter for role "DomainRef" among "KATS_BON::DomainRef"s
//********************************************************************************
std::set<KATS_BON::DomainRef> KATS_BON::ProcessBaseImpl::getDomainRef()
{
	std::set<KATS_BON::DomainRef> res;
	std::set<BON::FCO> roles = ModelImpl::getChildFCOsAs("DomainRef");
	for( std::set<BON::FCO>::iterator i = roles.begin(); i != roles.end(); ++i)
	{
		KATS_BON::DomainRef elem(*i);
		ASSERT(elem);
		res.insert(elem);
	}
	return res;
}


//********************************************************************************
// getter for role "HostRef" among "KATS_BON::HostRef"s
//********************************************************************************
std::set<KATS_BON::HostRef> KATS_BON::ProcessBaseImpl::getHostRef()
{
	std::set<KATS_BON::HostRef> res;
	std::set<BON::FCO> roles = ModelImpl::getChildFCOsAs("HostRef");
	for( std::set<BON::FCO>::iterator i = roles.begin(); i != roles.end(); ++i)
	{
		KATS_BON::HostRef elem(*i);
		ASSERT(elem);
		res.insert(elem);
	}
	return res;
}


//********************************************************************************
// getter for role "Kill" among "KATS_BON::Kill"s
//********************************************************************************
std::set<KATS_BON::Kill> KATS_BON::ProcessBaseImpl::getKill()
{
	std::set<KATS_BON::Kill> res;
	std::set<BON::FCO> roles = ModelImpl::getChildFCOsAs("Kill");
	for( std::set<BON::FCO>::iterator i = roles.begin(); i != roles.end(); ++i)
	{
		KATS_BON::Kill elem(*i);
		ASSERT(elem);
		res.insert(elem);
	}
	return res;
}


//********************************************************************************
// getter for role "TransportRef" among "KATS_BON::TransportRef"s
//********************************************************************************
std::set<KATS_BON::TransportRef> KATS_BON::ProcessBaseImpl::getTransportRef()
{
	std::set<KATS_BON::TransportRef> res;
	std::set<BON::FCO> roles = ModelImpl::getChildFCOsAs("TransportRef");
	for( std::set<BON::FCO>::iterator i = roles.begin(); i != roles.end(); ++i)
	{
		KATS_BON::TransportRef elem(*i);
		ASSERT(elem);
		res.insert(elem);
	}
	return res;
}


//********************************************************************************
// 
//********************************************************************************
void KATS_BON::GroupRefImpl::accept( KATS_BON::KATSVisitor *pVisitor)
{
	// visit the KATS_BON::GroupRef
	pVisitor->visitGroupRef( KATS_BON::GroupRef( this));
}


//********************************************************************************
// 
//********************************************************************************
std::string KATS_BON::GroupRefImpl::getOverrideBarrier() 
{
	return FCOImpl::getAttribute("OverrideBarrier")->getStringValue();
}


//********************************************************************************
// 
//********************************************************************************
std::string KATS_BON::GroupRefImpl::getOverrideDomain() 
{
	return FCOImpl::getAttribute("OverrideDomain")->getStringValue();
}


//********************************************************************************
// 
//********************************************************************************
std::string KATS_BON::GroupRefImpl::getOverrideHost() 
{
	return FCOImpl::getAttribute("OverrideHost")->getStringValue();
}


//********************************************************************************
// 
//********************************************************************************
bool KATS_BON::GroupRefImpl::isOverrideSettings() 
{
	return FCOImpl::getAttribute("OverrideSettings")->getBooleanValue();
}


//********************************************************************************
// 
//********************************************************************************
void KATS_BON::GroupRefImpl::setOverrideBarrier( const std::string& val)
{
	FCOImpl::getAttribute("OverrideBarrier")->setStringValue( val);
}


//********************************************************************************
// 
//********************************************************************************
void KATS_BON::GroupRefImpl::setOverrideDomain( const std::string& val)
{
	FCOImpl::getAttribute("OverrideDomain")->setStringValue( val);
}


//********************************************************************************
// 
//********************************************************************************
void KATS_BON::GroupRefImpl::setOverrideHost( const std::string& val)
{
	FCOImpl::getAttribute("OverrideHost")->setStringValue( val);
}


//********************************************************************************
// 
//********************************************************************************
void KATS_BON::GroupRefImpl::setOverrideSettings( bool val)
{
	FCOImpl::getAttribute("OverrideSettings")->setBooleanValue( val);
}


//********************************************************************************
// 
//********************************************************************************
KATS_BON::Group KATS_BON::GroupRefImpl::getGroup()
{
	BON::FCO r = BON::ReferenceImpl::getReferred();
	return KATS_BON::Group(r);
}


//********************************************************************************
// 
//********************************************************************************
void KATS_BON::GroupImpl::accept( KATS_BON::KATSVisitor *pVisitor)
{
	// visit the KATS_BON::Group
	pVisitor->visitGroup( KATS_BON::Group( this));

	// then its children
	std::set<BON::FCO> children = ModelImpl::getChildFCOs();
	for( std::set<BON::FCO>::const_iterator it = children.begin(); it != children.end(); ++it)
	{
		(*it)->accept( pVisitor);
	}
}


//********************************************************************************
// 
//********************************************************************************
bool KATS_BON::GroupImpl::isParallel() 
{
	return FCOImpl::getAttribute("Parallel")->getBooleanValue();
}


//********************************************************************************
// 
//********************************************************************************
void KATS_BON::GroupImpl::setParallel( bool val)
{
	FCOImpl::getAttribute("Parallel")->setBooleanValue( val);
}


//********************************************************************************
// getter for role "Group" among "KATS_BON::Group"s
//********************************************************************************
std::set<KATS_BON::Group> KATS_BON::GroupImpl::getGroup()
{
	std::set<KATS_BON::Group> res;
	std::set<BON::FCO> roles = ModelImpl::getChildFCOsAs("Group");
	for( std::set<BON::FCO>::iterator i = roles.begin(); i != roles.end(); ++i)
	{
		KATS_BON::Group elem(*i);
		ASSERT(elem);
		res.insert(elem);
	}
	return res;
}


//********************************************************************************
// getter for role "GroupRef" among "Ordered"s and its descendants
//********************************************************************************
std::set<KATS_BON::GroupRef> KATS_BON::GroupImpl::getGroupRef()
{
	std::set<KATS_BON::GroupRef> res;
	std::set<BON::FCO> roles = ModelImpl::getChildFCOsAs("GroupRef");
	for( std::set<BON::FCO>::iterator i = roles.begin(); i != roles.end(); ++i)
	{
		KATS_BON::GroupRef elem(*i);
		ASSERT(elem);
		res.insert(elem);
	}
	return res;
}


//********************************************************************************
// getter for role "Observer" among "Ordered"s and its descendants
//********************************************************************************
std::set<KATS_BON::Observer> KATS_BON::GroupImpl::getObserver()
{
	std::set<KATS_BON::Observer> res;
	std::set<BON::FCO> roles = ModelImpl::getChildFCOsAs("Observer");
	for( std::set<BON::FCO>::iterator i = roles.begin(); i != roles.end(); ++i)
	{
		KATS_BON::Observer elem(*i);
		ASSERT(elem);
		res.insert(elem);
	}
	return res;
}


//********************************************************************************
// aggregated getter for role "KATS_BON::" among "KATS_BON::Ordered"s and its descendants
//********************************************************************************
std::set<KATS_BON::Ordered> KATS_BON::GroupImpl::getOrdered()
{
	std::set<KATS_BON::Ordered> res;
	const int len = 4;
	std::set<BON::FCO> roles_vec[ len];
	roles_vec[0] = ModelImpl::getChildFCOsAs("Observer");
	roles_vec[1] = ModelImpl::getChildFCOsAs("Process");
	roles_vec[2] = ModelImpl::getChildFCOsAs("Sleep");
	roles_vec[3] = ModelImpl::getChildFCOsAs("GroupRef");
	for( int k = 0; k < len; ++k)
		for( std::set<BON::FCO>::iterator i = roles_vec[k].begin(); i != roles_vec[k].end(); ++i)
		{
			KATS_BON::Ordered elem(*i);
			ASSERT(elem);
			res.insert(elem);
		}
	return res;
}


//********************************************************************************
// getter for role "Process" among "Ordered"s and its descendants
//********************************************************************************
std::set<KATS_BON::Process> KATS_BON::GroupImpl::getProcess()
{
	std::set<KATS_BON::Process> res;
	std::set<BON::FCO> roles = ModelImpl::getChildFCOsAs("Process");
	for( std::set<BON::FCO>::iterator i = roles.begin(); i != roles.end(); ++i)
	{
		KATS_BON::Process elem(*i);
		ASSERT(elem);
		res.insert(elem);
	}
	return res;
}


//********************************************************************************
// getter for role "Sleep" among "Ordered"s and its descendants
//********************************************************************************
std::set<KATS_BON::Sleep> KATS_BON::GroupImpl::getSleep()
{
	std::set<KATS_BON::Sleep> res;
	std::set<BON::FCO> roles = ModelImpl::getChildFCOsAs("Sleep");
	for( std::set<BON::FCO>::iterator i = roles.begin(); i != roles.end(); ++i)
	{
		KATS_BON::Sleep elem(*i);
		ASSERT(elem);
		res.insert(elem);
	}
	return res;
}


//********************************************************************************
// 
//********************************************************************************
void KATS_BON::ObserverImpl::accept( KATS_BON::KATSVisitor *pVisitor)
{
	// visit the KATS_BON::Observer
	pVisitor->visitObserver( KATS_BON::Observer( this));

	// then its children
	std::set<BON::FCO> children = ModelImpl::getChildFCOs();
	for( std::set<BON::FCO>::const_iterator it = children.begin(); it != children.end(); ++it)
	{
		(*it)->accept( pVisitor);
	}
}


//********************************************************************************
// 
//********************************************************************************
void KATS_BON::ProcessImpl::accept( KATS_BON::KATSVisitor *pVisitor)
{
	// visit the KATS_BON::Process
	pVisitor->visitProcess( KATS_BON::Process( this));

	// then its children
	std::set<BON::FCO> children = ModelImpl::getChildFCOs();
	for( std::set<BON::FCO>::const_iterator it = children.begin(); it != children.end(); ++it)
	{
		(*it)->accept( pVisitor);
	}
}


//********************************************************************************
// 
//********************************************************************************
std::string KATS_BON::ProcessImpl::getCommandLine() 
{
	return FCOImpl::getAttribute("CommandLine")->getStringValue();
}


//********************************************************************************
// 
//********************************************************************************
std::string KATS_BON::ProcessImpl::getExecutable() 
{
	return FCOImpl::getAttribute("Executable")->getStringValue();
}


//********************************************************************************
// 
//********************************************************************************
void KATS_BON::ProcessImpl::setCommandLine( const std::string& val)
{
	FCOImpl::getAttribute("CommandLine")->setStringValue( val);
}


//********************************************************************************
// 
//********************************************************************************
void KATS_BON::ProcessImpl::setExecutable( const std::string& val)
{
	FCOImpl::getAttribute("Executable")->setStringValue( val);
}


//********************************************************************************
// 
//********************************************************************************
void KATS_BON::SleepImpl::accept( KATS_BON::KATSVisitor *pVisitor)
{
	// visit the KATS_BON::Sleep
	pVisitor->visitSleep( KATS_BON::Sleep( this));

	// then its children
	std::set<BON::FCO> children = ModelImpl::getChildFCOs();
	for( std::set<BON::FCO>::const_iterator it = children.begin(); it != children.end(); ++it)
	{
		(*it)->accept( pVisitor);
	}
}


//********************************************************************************
// 
//********************************************************************************
void KATS_BON::TransportImpl::accept( KATS_BON::KATSVisitor *pVisitor)
{
	// visit the KATS_BON::Transport
	pVisitor->visitTransport( KATS_BON::Transport( this));

	// then its children
	std::set<BON::FCO> children = ModelImpl::getChildFCOs();
	for( std::set<BON::FCO>::const_iterator it = children.begin(); it != children.end(); ++it)
	{
		(*it)->accept( pVisitor);
	}
}


//********************************************************************************
// 
//********************************************************************************
KATS_BON::TransportImpl::PersistenceType KATS_BON::TransportImpl::getPersistence()
{
	std::string val = FCOImpl::getAttribute("Persistence")->getStringValue();

	if( val == "Volatile") return VolatilePersistenceType;
	else if( val == "Persistent") return PersistentPersistenceType;
	else throw("None of the possible items");
}


//********************************************************************************
// 
//********************************************************************************
KATS_BON::TransportImpl::TypeType KATS_BON::TransportImpl::getType()
{
	std::string val = FCOImpl::getAttribute("Type")->getStringValue();

	if( val == "None") return NoneTypeType;
	else if( val == "Splice") return SpliceTypeType;
	else if( val == "NDDS") return NDDSTypeType;
	else throw("None of the possible items");
}


//********************************************************************************
// 
//********************************************************************************
void KATS_BON::TransportImpl::setPersistence( TransportImpl::PersistenceType val)
{
	std::string str_val = "";

	if( val == VolatilePersistenceType) str_val = "Volatile";
	else if( val == PersistentPersistenceType) str_val = "Persistent";
	else throw("None of the possible items");

	FCOImpl::getAttribute("Persistence")->setStringValue( str_val);
}


//********************************************************************************
// 
//********************************************************************************
void KATS_BON::TransportImpl::setType( TransportImpl::TypeType val)
{
	std::string str_val = "";

	if( val == NoneTypeType) str_val = "None";
	else if( val == SpliceTypeType) str_val = "Splice";
	else if( val == NDDSTypeType) str_val = "NDDS";
	else throw("None of the possible items");

	FCOImpl::getAttribute("Type")->setStringValue( str_val);
}


//********************************************************************************
// 
//********************************************************************************
void KATS_BON::BarrierImpl::accept( KATS_BON::KATSVisitor *pVisitor)
{
	// visit the KATS_BON::Barrier
	pVisitor->visitBarrier( KATS_BON::Barrier( this));
}


//********************************************************************************
// 
//********************************************************************************
std::string KATS_BON::BarrierImpl::getOverride() 
{
	return FCOImpl::getAttribute("Override")->getStringValue();
}


//********************************************************************************
// 
//********************************************************************************
void KATS_BON::BarrierImpl::setOverride( const std::string& val)
{
	FCOImpl::getAttribute("Override")->setStringValue( val);
}


//********************************************************************************
// 
//********************************************************************************
void KATS_BON::DomainImpl::accept( KATS_BON::KATSVisitor *pVisitor)
{
	// visit the KATS_BON::Domain
	pVisitor->visitDomain( KATS_BON::Domain( this));
}


//********************************************************************************
// 
//********************************************************************************
std::string KATS_BON::DomainImpl::getOverride() 
{
	return FCOImpl::getAttribute("Override")->getStringValue();
}


//********************************************************************************
// 
//********************************************************************************
void KATS_BON::DomainImpl::setOverride( const std::string& val)
{
	FCOImpl::getAttribute("Override")->setStringValue( val);
}


//********************************************************************************
// 
//********************************************************************************
void KATS_BON::HostImpl::accept( KATS_BON::KATSVisitor *pVisitor)
{
	// visit the KATS_BON::Host
	pVisitor->visitHost( KATS_BON::Host( this));
}


//********************************************************************************
// 
//********************************************************************************
std::string KATS_BON::HostImpl::getOverride() 
{
	return FCOImpl::getAttribute("Override")->getStringValue();
}


//********************************************************************************
// 
//********************************************************************************
void KATS_BON::HostImpl::setOverride( const std::string& val)
{
	FCOImpl::getAttribute("Override")->setStringValue( val);
}


//********************************************************************************
// 
//********************************************************************************
void KATS_BON::KillImpl::accept( KATS_BON::KATSVisitor *pVisitor)
{
	// visit the KATS_BON::Kill
	pVisitor->visitKill( KATS_BON::Kill( this));
}


//********************************************************************************
// 
//********************************************************************************
std::string KATS_BON::KillImpl::getSignal() 
{
	return FCOImpl::getAttribute("Signal")->getStringValue();
}


//********************************************************************************
// 
//********************************************************************************
std::string KATS_BON::KillImpl::getTime() 
{
	return FCOImpl::getAttribute("Time")->getStringValue();
}


//********************************************************************************
// 
//********************************************************************************
void KATS_BON::KillImpl::setSignal( const std::string& val)
{
	FCOImpl::getAttribute("Signal")->setStringValue( val);
}


//********************************************************************************
// 
//********************************************************************************
void KATS_BON::KillImpl::setTime( const std::string& val)
{
	FCOImpl::getAttribute("Time")->setStringValue( val);
}


//********************************************************************************
// 
//********************************************************************************
void KATS_BON::BarrierRefImpl::accept( KATS_BON::KATSVisitor *pVisitor)
{
	// visit the KATS_BON::BarrierRef
	pVisitor->visitBarrierRef( KATS_BON::BarrierRef( this));
}


//********************************************************************************
// 
//********************************************************************************
bool KATS_BON::BarrierRefImpl::isOverrideProcesses() 
{
	return FCOImpl::getAttribute("OverrideProcesses")->getBooleanValue();
}


//********************************************************************************
// 
//********************************************************************************
void KATS_BON::BarrierRefImpl::setOverrideProcesses( bool val)
{
	FCOImpl::getAttribute("OverrideProcesses")->setBooleanValue( val);
}


//********************************************************************************
// 
//********************************************************************************
KATS_BON::Barrier KATS_BON::BarrierRefImpl::getBarrier()
{
	BON::FCO r = BON::ReferenceImpl::getReferred();
	return KATS_BON::Barrier(r);
}


//********************************************************************************
// 
//********************************************************************************
void KATS_BON::DomainRefImpl::accept( KATS_BON::KATSVisitor *pVisitor)
{
	// visit the KATS_BON::DomainRef
	pVisitor->visitDomainRef( KATS_BON::DomainRef( this));
}


//********************************************************************************
// 
//********************************************************************************
KATS_BON::Domain KATS_BON::DomainRefImpl::getDomain()
{
	BON::FCO r = BON::ReferenceImpl::getReferred();
	return KATS_BON::Domain(r);
}


//********************************************************************************
// 
//********************************************************************************
void KATS_BON::HostRefImpl::accept( KATS_BON::KATSVisitor *pVisitor)
{
	// visit the KATS_BON::HostRef
	pVisitor->visitHostRef( KATS_BON::HostRef( this));
}


//********************************************************************************
// 
//********************************************************************************
KATS_BON::Host KATS_BON::HostRefImpl::getHost()
{
	BON::FCO r = BON::ReferenceImpl::getReferred();
	return KATS_BON::Host(r);
}


//********************************************************************************
// 
//********************************************************************************
void KATS_BON::TransportRefImpl::accept( KATS_BON::KATSVisitor *pVisitor)
{
	// visit the KATS_BON::TransportRef
	pVisitor->visitTransportRef( KATS_BON::TransportRef( this));
}


//********************************************************************************
// 
//********************************************************************************
KATS_BON::Transport KATS_BON::TransportRefImpl::getTransport()
{
	BON::FCO r = BON::ReferenceImpl::getReferred();
	return KATS_BON::Transport(r);
}


