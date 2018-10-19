package ai.madara.tests;

import org.junit.After;
import org.junit.Before;

import ai.madara.exceptions.MadaraDeadObjectException;
import ai.madara.knowledge.Any;
import ai.madara.knowledge.KnowledgeBase;
import ai.madara.logger.GlobalLogger;
import ai.madara.logger.LogLevels;
import ai.madara.tests.capnp.Geo;
import ai.madara.transport.QoSTransportSettings;
import ai.madara.transport.TransportType;
import ai.madara.transport.filters.AggregateFilter;

public class BaseTest {
	private KnowledgeBase knowledgetBase;

	static {
		System.loadLibrary("MADARA");
	}
	
	
	@Before
	public void initKB() throws MadaraDeadObjectException {
		registerAnyTypes();
		knowledgetBase = new KnowledgeBase();
		KnowledgeBase.setLogLevel(LogLevels.LOG_DETAILED);
		knowledgetBase.attachLogger(GlobalLogger.toLogger());
	}

	private void registerAnyTypes() {

		Any.registerStringVector("strvec");
		Any.registerDoubleVector("dblvec");
		Any.registerStringToStringMap("smap");
		Any.registerClass("Point", Geo.Point.factory);

	}

	@After
	public void freeResources() {
		if (knowledgetBase != null) {
			knowledgetBase.free();
		}
	}

	/**
	 * Initializes KB with Multicast IP - "239.255.0.1:4150"
	 *
	 * @param receiveFilter
	 * @param sendFilter
	 * @throws MadaraDeadObjectException throws exception if object is already
	 *                                   released
	 */
	public void attachMulticast(AggregateFilter receiveFilter, AggregateFilter sendFilter)
			throws MadaraDeadObjectException {

		QoSTransportSettings transport = new QoSTransportSettings();
		QoSTransportSettings qoSTransportSettings = new QoSTransportSettings();
		qoSTransportSettings.setHosts(new String[] { "239.255.0.1:4150" });
		qoSTransportSettings.setType(TransportType.MULTICAST_TRANSPORT);
		qoSTransportSettings.addReceiveFilter(receiveFilter);
		qoSTransportSettings.addSendFilter(sendFilter);
		knowledgetBase.attachTransport("multicast", transport);
		knowledgetBase.attachLogger(GlobalLogger.toLogger());

	}

	public void attachQoS(String id, QoSTransportSettings transport) throws MadaraDeadObjectException {
		knowledgetBase.attachTransport(id, transport);
	}

	public KnowledgeBase getDefaultKnowledgeBase() {
		return knowledgetBase;
	}

}
