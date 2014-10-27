#ifndef KATSVISITOR_H
#define KATSVISITOR_H

#include "BON.h"
#include "BONImpl.h"
#include "Extensions.h"
#include "KATSBonX.h"

namespace KATS_BON
{

class KATSVisitor
	: public BON::Visitor
{
	public : 
		KATSVisitor();
		virtual ~KATSVisitor();

	public :
		virtual bool visitBarrier( const KATS_BON::Barrier& object );
		virtual bool visitDomain( const KATS_BON::Domain& object );
		virtual bool visitHost( const KATS_BON::Host& object );
		virtual bool visitKill( const KATS_BON::Kill& object );
		virtual bool visitGroup( const KATS_BON::Group& object );
		virtual bool visitObserver( const KATS_BON::Observer& object );
		virtual bool visitProcess( const KATS_BON::Process& object );
		virtual bool visitProcessBase( const KATS_BON::ProcessBase& object );
		virtual bool visitSleep( const KATS_BON::Sleep& object );
		virtual bool visitTransport( const KATS_BON::Transport& object );
		virtual bool visitBarrierRef( const KATS_BON::BarrierRef& object );
		virtual bool visitDomainRef( const KATS_BON::DomainRef& object );
		virtual bool visitGroupRef( const KATS_BON::GroupRef& object );
		virtual bool visitHostRef( const KATS_BON::HostRef& object );
		virtual bool visitTransportRef( const KATS_BON::TransportRef& object );
		virtual bool visitConfigureBase( const KATS_BON::ConfigureBase& object );
		virtual bool visitOrdered( const KATS_BON::Ordered& object );
		virtual bool visitBarriers( const KATS_BON::Barriers& object );
		virtual bool visitDomains( const KATS_BON::Domains& object );
		virtual bool visitHosts( const KATS_BON::Hosts& object );
		virtual bool visitProcesses( const KATS_BON::Processes& object );
		virtual bool visitTransports( const KATS_BON::Transports& object );
};

} // namespace

#endif // KATSVISITOR_H
