package ai.madara.tests.basic;

import org.junit.Assert;
import org.junit.Test;

import ai.madara.exceptions.MadaraDeadObjectException;
import ai.madara.knowledge.BadAnyAccess;
import ai.madara.knowledge.KnowledgeBase;
import ai.madara.knowledge.KnowledgeRecord;
import ai.madara.knowledge.Variables;
import ai.madara.tests.BaseTest;
import ai.madara.tests.capnp.Geo;
import ai.madara.transport.TransportContext;
import ai.madara.transport.filters.AggregateFilter;
import ai.madara.transport.filters.Packet;

public class TestMadaraTransports extends BaseTest {

	private class NetworkFilter implements Runnable, AggregateFilter {
		public boolean received = false;

		public void run() {
			try {
				while (!received) {
					Thread.sleep(1000);
				}
			} catch (Exception e) {
				e.printStackTrace();
			}

		}

		@Override
		public void filter(Packet packet, TransportContext context, Variables variables)
				throws MadaraDeadObjectException {

			try {

				KnowledgeRecord kr = variables.get("location");
				if (kr.isValid()) {
					Geo.Point.Reader reader = kr.toAny().reader(Geo.Point.factory);
					Assert.assertEquals(reader.getX(), BaseTest.DEFAULT_GEO_POINT[0], 0);
					Assert.assertEquals(reader.getY(), BaseTest.DEFAULT_GEO_POINT[1], 0);
					Assert.assertEquals(reader.getZ(), BaseTest.DEFAULT_GEO_POINT[2], 0);
				}
				received = true;
			} catch (Exception e) {
				e.printStackTrace();
			}
		}

	}

	@Test
	public void testMulticast() throws MadaraDeadObjectException, BadAnyAccess, InterruptedException {

		final KnowledgeBase kb = initKnowledgeBase();
		KnowledgeBase receiveKb = new KnowledgeBase();
		NetworkFilter filter = new NetworkFilter();

		attachMulticast(kb, null, null);
		attachMulticast(receiveKb, filter, null);

		kb.set(".id", "0");

		Thread sender = new Thread() {
			public void run() {
				try {
					// Sleep for a while and send the modified values
					Thread.sleep(2000);
					for (int i = 0; i < 2; i++) {
						kb.set("location", getGeoPoint());
						kb.sendModifieds();
					}
				} catch (Exception e) {
					e.printStackTrace();
				}
			}
		};

		Thread fThread = new Thread(filter);
		fThread.start();

		sender.start();
		fThread.join(20000);
		Assert.assertTrue(filter.received);


		freeResources(kb);
		freeResources(receiveKb);
	}

	@Test
	public void testZMQ() throws MadaraDeadObjectException, BadAnyAccess, InterruptedException {

		final KnowledgeBase kb = initKnowledgeBase();
		KnowledgeBase receiveKb = initKnowledgeBase();
		NetworkFilter filter = new NetworkFilter();

		attachZMQ(kb, new String[] { "tcp://127.0.0.1:20000", "tcp://127.0.0.1:21000" }, null, null);
		attachZMQ(receiveKb, new String[] { "tcp://127.0.0.1:21000", "tcp://127.0.0.1:20000" }, filter, null);

		kb.set(".id", "0");
		kb.set("location", getGeoPoint());

		Thread sender = new Thread() {
			public void run() {
				try {
					// Sleep for a while and send the modified values
					Thread.sleep(2000);
					for (int i = 0; i < 2; i++) {
						kb.set("location", getGeoPoint());
						kb.sendModifieds();
					}
				} catch (Exception e) {
					e.printStackTrace();
				}
			}
		};

		Thread filterThread = new Thread(filter);
		filterThread.start();

		sender.start();
		filterThread.join(20000);
		Assert.assertTrue(filter.received);

		freeResources(kb);
		freeResources(receiveKb);
	}


	@Test
	public void testUDP() throws MadaraDeadObjectException, BadAnyAccess, InterruptedException {

		final KnowledgeBase kb = initKnowledgeBase();
		KnowledgeBase receiveKb = initKnowledgeBase();
		NetworkFilter filter = new NetworkFilter();

		attachUDP(kb, new String[] { "127.0.0.1:20000", "127.0.0.1:21000" }, null, null);
		attachUDP(receiveKb, new String[] { "127.0.0.1:21000", "127.0.0.1:20000" }, filter, null);

		kb.set(".id", "0");
		kb.set("location", getGeoPoint());

		Thread sender = new Thread() {
			public void run() {
				try {
					// Sleep for a while and send the modified values
					Thread.sleep(2000);
					for (int i = 0; i < 2; i++) {
						kb.set("location", getGeoPoint());
						kb.sendModifieds();
					}
				} catch (Exception e) {
					e.printStackTrace();
				}
			}
		};

		Thread filterThread = new Thread(filter);
		filterThread.start();

		sender.start();
		filterThread.join(20000);
		Assert.assertTrue(filter.received);

		freeResources(kb);
		freeResources(receiveKb);
	}

}
