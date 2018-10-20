package ai.madara.tests;

import org.capnproto.MessageBuilder;
import org.junit.After;
import org.junit.Before;

import ai.madara.exceptions.MadaraDeadObjectException;
import ai.madara.knowledge.Any;
import ai.madara.knowledge.BadAnyAccess;
import ai.madara.knowledge.KnowledgeBase;
import ai.madara.tests.capnp.Geo;
import ai.madara.transport.QoSTransportSettings;
import ai.madara.transport.TransportType;
import ai.madara.transport.filters.AggregateFilter;

public class BaseTest {
	private KnowledgeBase knowledgeBase;

	public static double[] DEFAULT_GEO_POINT = { 40.417286, -82.907120, 0 };

	static {
		System.loadLibrary("MADARA");
	}

	@Before
	public void initKB() throws MadaraDeadObjectException {
		registerAnyTypes();
		knowledgeBase = new KnowledgeBase();
		//KnowledgeBase.setLogLevel(LogLevels.LOG_DETAILED);
		//knowledgeBase.attachLogger(GlobalLogger.toLogger());
	}

	private void registerAnyTypes() {
		Any.registerStringVector("strvec");
		Any.registerDoubleVector("dblvec");
		Any.registerStringToStringMap("smap");
		Any.registerClass("Point", Geo.Point.factory);
	}

	@After
	public void freeResources() {
		if (knowledgeBase != null) {
			knowledgeBase.free();
		}
	}

	public Any getGeoPoint() throws BadAnyAccess {
		MessageBuilder msg = new MessageBuilder();
		Geo.Point.Builder builder = msg.initRoot(Geo.Point.factory);

		builder.setX(DEFAULT_GEO_POINT[0]);
		builder.setY(DEFAULT_GEO_POINT[1]);
		builder.setZ(DEFAULT_GEO_POINT[2]);

		Any ohio = new Any(Geo.Point.factory, msg);
		return ohio;
	}

	/**
	 * Initializes KB with Multicast IP - "239.255.0.1:4150"
	 *
	 * @param receiveFilter
	 * @param sendFilter
	 * @throws MadaraDeadObjectException throws exception if object is already
	 *                                   released
	 */
	public void attachMulticast(KnowledgeBase knowledgeBase, AggregateFilter receiveFilter, AggregateFilter sendFilter)
			throws MadaraDeadObjectException {

		QoSTransportSettings qoSTransportSettings = new QoSTransportSettings();
		qoSTransportSettings.setHosts(new String[] { "239.255.0.1:4150" });
		qoSTransportSettings.setType(TransportType.MULTICAST_TRANSPORT);
		if (receiveFilter != null) {
			qoSTransportSettings.addReceiveFilter(receiveFilter);
		}

		if (sendFilter != null) {
			qoSTransportSettings.addSendFilter(sendFilter);
		}
		knowledgeBase.attachTransport("", qoSTransportSettings);

	}

	public void attachQoS(String id, QoSTransportSettings transport) throws MadaraDeadObjectException {
		knowledgeBase.attachTransport(id, transport);
	}

	public KnowledgeBase getDefaultKnowledgeBase() {
		return knowledgeBase;
	}

}
