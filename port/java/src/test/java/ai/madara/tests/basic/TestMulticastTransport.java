package ai.madara.tests.basic;

import org.junit.Test;

import ai.madara.exceptions.MadaraDeadObjectException;
import ai.madara.knowledge.BadAnyAccess;
import ai.madara.knowledge.EvalSettings;
import ai.madara.knowledge.KnowledgeBase;
import ai.madara.knowledge.Variables;
import ai.madara.tests.BaseTest;
import ai.madara.transport.QoSTransportSettings;
import ai.madara.transport.TransportContext;
import ai.madara.transport.filters.LogAggregate;
import ai.madara.transport.filters.Packet;

public class TestMulticastTransport extends BaseTest {

	private class NetworkFilter extends LogAggregate {
		private boolean keepRunning = true;

		public void run() {
			while (keepRunning) {
				try {
					System.out.println("Waiting..");
					Thread.sleep(1000);
				} catch (InterruptedException e) {
					e.printStackTrace();
				}
			}
		}

		@Override
		public void filter(Packet packet, TransportContext context, Variables variables)
				throws MadaraDeadObjectException {
			keepRunning = false;
			System.out.println("Received...");
		}

	}

	@Test
	public void testMulticast() throws MadaraDeadObjectException, BadAnyAccess, InterruptedException {

		final KnowledgeBase kb = getDefaultKnowledgeBase();

		NetworkFilter filter = new NetworkFilter();
		attachMulticast(kb, filter, null);

		kb.set(".id", "0");
		kb.set("location", getGeoPoint());

		Thread sender = new Thread() {
			public void run() {
				try {
					Thread.sleep(2000);

					System.out.println("Sending...");
					EvalSettings settings = new EvalSettings();
					settings.setDelaySendingModifieds(false);
					kb.evaluate("test=1");

				} catch (Exception e) {
					e.printStackTrace();
				}
			}
		};

		// filter.start();

		sender.start();

		Thread.sleep(10000);
		System.out.println("Multicast Test Over.");

	}

}
