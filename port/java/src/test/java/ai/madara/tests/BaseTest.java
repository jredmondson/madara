package ai.madara.tests;

import ai.madara.exceptions.MadaraDeadObjectException;
import ai.madara.knowledge.KnowledgeBase;
import ai.madara.logger.GlobalLogger;
import ai.madara.transport.QoSTransportSettings;
import ai.madara.transport.TransportType;
import ai.madara.transport.filters.AggregateFilter;

public class BaseTest {

  public static double[] DEFAULT_GEO_POINT = { 40.417286, -82.907120, 0 };

  static {
    System.loadLibrary("MADARA");
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
