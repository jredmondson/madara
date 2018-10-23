package ai.madara.tests;

import org.capnproto.MessageBuilder;

import ai.madara.exceptions.MadaraDeadObjectException;
import ai.madara.knowledge.Any;
import ai.madara.knowledge.BadAnyAccess;
import ai.madara.knowledge.KnowledgeBase;
import ai.madara.logger.GlobalLogger;
import ai.madara.tests.capnp.Geo;
import ai.madara.tests.capnp.Person;
import ai.madara.transport.QoSTransportSettings;
import ai.madara.transport.TransportType;
import ai.madara.transport.filters.AggregateFilter;

public class BaseTest {

	public static double[] DEFAULT_GEO_POINT = { 40.417286, -82.907120, 0 };

	static {
		System.loadLibrary("MADARA");
		registerAnyTypes();

	}

	public static void registerAnyTypes() {
		// Primitive Types
		Any.registerInt("int");
		Any.registerDouble("double");
		Any.registerString("string");
		Any.registerChar("char");
		Any.registerFloat("float");

		Any.registerStringVector("strvec");
		Any.registerDoubleVector("dblvec");
		Any.registerStringToStringMap("smap");
		Any.registerClass("Point", Geo.Point.factory);
		Any.registerClass("PersonBio", Person.Bio.factory);
		Any.registerClass("PersonDate", Person.Date.factory);
		Any.registerClass("PersonPhone", Person.Bio.PhoneNumber.factory);

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

	/**
	 * Initializes KB with ZMQ
	 *
	 * @param receiveFilter
	 * @param sendFilter
	 * @throws MadaraDeadObjectException throws exception if object is already
	 *                                   released
	 */
	public void attachZMQ(KnowledgeBase knowledgeBase, String[] hosts, AggregateFilter receiveFilter,
			AggregateFilter sendFilter) throws MadaraDeadObjectException {

		QoSTransportSettings qoSTransportSettings = new QoSTransportSettings();
		qoSTransportSettings.setHosts(hosts);
		qoSTransportSettings.setType(TransportType.ZMQ_TRANSPORT);
		if (receiveFilter != null) {
			qoSTransportSettings.addReceiveFilter(receiveFilter);
		}

		if (sendFilter != null) {
			qoSTransportSettings.addSendFilter(sendFilter);
		}
		knowledgeBase.attachTransport("", qoSTransportSettings);

	}

	/**
	 * Initializes KB with ZMQ
	 *
	 * @param receiveFilter
	 * @param sendFilter
	 * @throws MadaraDeadObjectException throws exception if object is already
	 *                                   released
	 */
	public void attachUDP(KnowledgeBase knowledgeBase, String[] hosts, AggregateFilter receiveFilter,
			AggregateFilter sendFilter) throws MadaraDeadObjectException {

		QoSTransportSettings qoSTransportSettings = new QoSTransportSettings();
		qoSTransportSettings.setHosts(hosts);
		qoSTransportSettings.setType(TransportType.UDP_TRANSPORT);
		if (receiveFilter != null) {
			qoSTransportSettings.addReceiveFilter(receiveFilter);
		}

		if (sendFilter != null) {
			qoSTransportSettings.addSendFilter(sendFilter);
		}
		knowledgeBase.attachTransport("", qoSTransportSettings);

	}

	public KnowledgeBase initKnowledgeBase() throws MadaraDeadObjectException {
		KnowledgeBase knowledgeBase = new KnowledgeBase();
		knowledgeBase.attachLogger(GlobalLogger.toLogger());
		return knowledgeBase;
	}

	public void freeResources(KnowledgeBase kb) throws MadaraDeadObjectException {
		kb.clearModifieds();
		kb.closeTransports();
		kb.free();

	}

}
