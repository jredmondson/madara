#include "stdafx.h"
#include "KATSVisitor.h"


namespace KATS_BON
{

KATSVisitor::KATSVisitor()  { }

KATSVisitor::~KATSVisitor() { }

bool KATSVisitor::visitBarrier( const KATS_BON::Barrier& object )
{
	if ( !object)
		return false;

	return true;
}


bool KATSVisitor::visitDomain( const KATS_BON::Domain& object )
{
	if ( !object)
		return false;

	return true;
}


bool KATSVisitor::visitHost( const KATS_BON::Host& object )
{
	if ( !object)
		return false;

	return true;
}


bool KATSVisitor::visitKill( const KATS_BON::Kill& object )
{
	if ( !object)
		return false;

	return true;
}


bool KATSVisitor::visitGroup( const KATS_BON::Group& object )
{
	if ( !object)
		return false;

	return true;
}


bool KATSVisitor::visitObserver( const KATS_BON::Observer& object )
{
	if ( !object)
		return false;

	return true;
}


bool KATSVisitor::visitProcess( const KATS_BON::Process& object )
{
	if ( !object)
		return false;

	return true;
}


bool KATSVisitor::visitProcessBase( const KATS_BON::ProcessBase& object )
{
	if ( !object)
		return false;

	return true;
}


bool KATSVisitor::visitSleep( const KATS_BON::Sleep& object )
{
	if ( !object)
		return false;

	return true;
}


bool KATSVisitor::visitTransport( const KATS_BON::Transport& object )
{
	if ( !object)
		return false;

	return true;
}


bool KATSVisitor::visitBarrierRef( const KATS_BON::BarrierRef& object )
{
	if ( !object)
		return false;

	return true;
}


bool KATSVisitor::visitDomainRef( const KATS_BON::DomainRef& object )
{
	if ( !object)
		return false;

	return true;
}


bool KATSVisitor::visitGroupRef( const KATS_BON::GroupRef& object )
{
	if ( !object)
		return false;

	return true;
}


bool KATSVisitor::visitHostRef( const KATS_BON::HostRef& object )
{
	if ( !object)
		return false;

	return true;
}


bool KATSVisitor::visitTransportRef( const KATS_BON::TransportRef& object )
{
	if ( !object)
		return false;

	return true;
}


bool KATSVisitor::visitConfigureBase( const KATS_BON::ConfigureBase& object )
{
	if ( !object)
		return false;

	return true;
}


bool KATSVisitor::visitOrdered( const KATS_BON::Ordered& object )
{
	if ( !object)
		return false;

	return true;
}


bool KATSVisitor::visitBarriers( const KATS_BON::Barriers& object )
{
	if ( !object)
		return false;

	return true;
}


bool KATSVisitor::visitDomains( const KATS_BON::Domains& object )
{
	if ( !object)
		return false;

	return true;
}


bool KATSVisitor::visitHosts( const KATS_BON::Hosts& object )
{
	if ( !object)
		return false;

	return true;
}


bool KATSVisitor::visitProcesses( const KATS_BON::Processes& object )
{
	if ( !object)
		return false;

	return true;
}


bool KATSVisitor::visitTransports( const KATS_BON::Transports& object )
{
	if ( !object)
		return false;

	return true;
}



} // namespace

