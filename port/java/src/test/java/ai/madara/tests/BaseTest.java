package ai.madara.tests;

import org.junit.After;
import org.junit.Before;

import ai.madara.exceptions.MadaraDeadObjectException;
import ai.madara.knowledge.KnowledgeBase;
import ai.madara.logger.GlobalLogger;
import ai.madara.logger.LogLevels;
import ai.madara.transport.QoSTransportSettings;
import ai.madara.transport.TransportType;
import ai.madara.transport.filters.AggregateFilter;

public class BaseTest {
	private KnowledgeBase kb;

	@Before
	public void initKB() throws MadaraDeadObjectException {
		kb = new KnowledgeBase();
		KnowledgeBase.setLogLevel(LogLevels.LOG_DETAILED);
		kb.attachLogger(GlobalLogger.toLogger());
	}

	@After
	public void freeResources() {
		kb.free();
	}

	/**
	 * Initializes KB with Multicast IP - "239.255.0.1:4150"
	 *
	 * @param receiveFilter
	 * @param sendFilter
	 * @throws MadaraDeadObjectException
	 */
	public void initKBWithMulticast(AggregateFilter receiveFilter, AggregateFilter sendFilter)
			throws MadaraDeadObjectException {
		
		QoSTransportSettings transport = new QoSTransportSettings();
		QoSTransportSettings qoSTransportSettings = new QoSTransportSettings();
		qoSTransportSettings.setHosts(new String[] { "239.255.0.1:4150" });
		qoSTransportSettings.setType(TransportType.MULTICAST_TRANSPORT);
		qoSTransportSettings.addReceiveFilter(receiveFilter);
		qoSTransportSettings.addSendFilter(sendFilter);
		kb.attachTransport("multicast", transport);
		kb.attachLogger(GlobalLogger.toLogger());

	}

	public void initKbWithQoS(QoSTransportSettings transport) throws MadaraDeadObjectException {
		kb.attachTransport("transport", transport);
	}

	public KnowledgeBase getKb() {
		return kb;
	}

	public void setKb(KnowledgeBase kb) {
		this.kb = kb;
	}

}
