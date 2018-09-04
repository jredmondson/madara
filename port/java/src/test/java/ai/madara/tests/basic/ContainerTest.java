/*********************************************************************
 * Copyright (c) 2013-2015 Carnegie Mellon University. All Rights Reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following acknowledgments and disclaimers.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * 3. The names "Carnegie Mellon University," "SEI" and/or
 * "Software Engineering Institute" shall not be used to endorse or promote
 * products derived from this software without prior written permission. For
 * written permission, please contact permission@sei.cmu.edu.
 *
 * 4. Products derived from this software may not be called "SEI" nor may "SEI"
 * appear in their names without prior written permission of
 * permission@sei.cmu.edu.
 *
 * 5. Redistributions of any form whatsoever must retain the following
 * acknowledgment:
 *
 * This material is based upon work funded and supported by the Department of
 * Defense under Contract No. FA8721-05-C-0003 with Carnegie Mellon University
 * for the operation of the Software Engineering Institute, a federally funded
 * research and development center. Any opinions, findings and conclusions or
 * recommendations expressed in this material are those of the author(s) and
 * do not necessarily reflect the views of the United States Department of
 * Defense.
 *
 * NO WARRANTY. THIS CARNEGIE MELLON UNIVERSITY AND SOFTWARE ENGINEERING
 * INSTITUTE MATERIAL IS FURNISHED ON AN "AS-IS" BASIS. CARNEGIE MELLON
 * UNIVERSITY MAKES NO WARRANTIES OF ANY KIND, EITHER EXPRESSED OR IMPLIED,
 * AS TO ANY MATTER INCLUDING, BUT NOT LIMITED TO, WARRANTY OF FITNESS FOR
 * PURPOSE OR MERCHANTABILITY, EXCLUSIVITY, OR RESULTS OBTAINED FROM USE OF THE
 * MATERIAL. CARNEGIE MELLON UNIVERSITY DOES NOT MAKE ANY WARRANTY OF ANY KIND
 * WITH RESPECT TO FREEDOM FROM PATENT, TRADEMARK, OR COPYRIGHT INFRINGEMENT.
 *
 * This material has been approved for public release and unlimited
 * distribution.
 *
 * @author James Edmondson <jedmondson@gmail.com>
 *********************************************************************/

package ai.madara.tests.basic;

import org.junit.Assert;
import org.junit.Test;

import ai.madara.exceptions.MadaraDeadObjectException;
import ai.madara.knowledge.KnowledgeBase;
import ai.madara.knowledge.KnowledgeRecord;
import ai.madara.knowledge.UpdateSettings;
import ai.madara.knowledge.containers.CircularBuffer;
import ai.madara.knowledge.containers.CircularBufferConsumer;
import ai.madara.knowledge.containers.Collection;
import ai.madara.knowledge.containers.Double;
import ai.madara.knowledge.containers.DoubleVector;
import ai.madara.knowledge.containers.FlexMap;
import ai.madara.knowledge.containers.Integer;
import ai.madara.knowledge.containers.IntegerVector;
import ai.madara.knowledge.containers.NativeDoubleVector;
import ai.madara.knowledge.containers.Queue;
import ai.madara.knowledge.containers.StringVector;
import ai.madara.knowledge.containers.Vector;
import ai.madara.tests.BaseTest;

/**
 * This class is a tester for the ai.madara.knowledge.containers package
 */
public class ContainerTest extends BaseTest {

	@Test
	public void testFlexMap() throws MadaraDeadObjectException {
		KnowledgeBase knowledge = getKb();
		UpdateSettings settings = new UpdateSettings();
		settings.setTreatGlobalsAsLocals(true);
		FlexMap map = new FlexMap();
		map.setName(knowledge, "");
		map.setSettings(settings);

		// create some flex maps within the knowledge base
		FlexMap records = map.get("records");
		FlexMap robert = records.get("robert");
		FlexMap cassie = records.get("cassie");

		// FlexMap contains some garbage collection via finalize, but it may
		// never be called by the GC due to Java's own quirkiness. We create
		// some flexmaps here that will not be explicitly freed, and this is
		// technically ok, but in a real application, if you are done with one
		// you are better suited to keep a reference to them and free the C
		// resources.

		robert.get("age").set(49);
		robert.get("name").set("Robert Jenkins");
		robert.get("salary").set(30500.00);

		cassie.get("age").set(22);
		cassie.get("name").set("Cassandra Collins");
		cassie.get("salary").set(57000.00);

		Assert.assertEquals(knowledge.get(".records.robert.name").toString(), "Robert Jenkins");
		Assert.assertEquals(knowledge.get(".records.robert.age").toLong(), 49);

		Assert.assertEquals(knowledge.get(".records.robert.salary").toDouble(), 30500.00, 0);

		Assert.assertNotEquals(knowledge.get(".records.cassie.name").toString(), "Robert Jenkins");
		Assert.assertEquals(knowledge.get(".records.cassie.age").toLong(), 22);

		Assert.assertEquals(knowledge.get(".records.cassie.salary").toLong(), 57000);

		java.lang.String[] keys = robert.keys(true);

		Assert.assertNotEquals(keys.length, 0);

		// change the map delimiter
		map.setDelimiter(";");

		System.out.println("Retrieving first level keys of FlexMap");

		keys = map.keys(true);

		Assert.assertEquals(keys.length, 0);

		// change the map delimiter
		map.setDelimiter(".");

		System.out.println("Retrieving first level keys of FlexMap");

		keys = robert.keys(true);

		Assert.assertNotEquals(keys.length, 0);

		// order doesn't really matter here
		cassie.free();
		robert.free();
		records.free();
		settings.free();
	}

	@Test
	public void testVector() throws MadaraDeadObjectException {
		KnowledgeBase knowledge = new KnowledgeBase();

		Vector list = new Vector();
		UpdateSettings settings = new UpdateSettings();
		settings.setTreatGlobalsAsLocals(true);
		list.setSettings(settings);

		list.setName(knowledge, "device.sensors");

		list.resize(4);

		list.set(0, "first element");
		list.set(1, "second element");
		list.set(2, "third element");
		list.set(3, "fourth element");

		KnowledgeRecord[] my_array = list.toArray();

		Assert.assertEquals(my_array.length, list.size());

		System.out.println("  Printing " + my_array.length + " array elements");

		for (int i = 0; i < my_array.length; ++i) {
			Assert.assertEquals(my_array[i].toString(), list.get(i).toString());
		}

		list.resize(0);

		Assert.assertEquals(list.size(), 0);
		list.pushback(10);
		list.pushback("second element");

		Assert.assertEquals(list.size(), 2);
	}

	@Test
	public void testInteger() throws MadaraDeadObjectException {
		KnowledgeBase knowledge = getKb();

		Integer counter = new Integer();
		UpdateSettings settings = new UpdateSettings();
		settings.setTreatGlobalsAsLocals(true);
		counter.setSettings(settings);

		counter.setName(knowledge, "counter");

		// 0 += 1 == 1
		Assert.assertTrue(counter.inc() == 1);

		// 1 += 10 == 11
		Assert.assertEquals(11, counter.inc(10));

		// 11 -= 5 == 6
		Assert.assertEquals(6, counter.dec(5));

		// --6 == 5
		Assert.assertEquals(5, counter.dec());

	}

	@Test
	public void testCircularBuffer() throws MadaraDeadObjectException {

		KnowledgeBase knowledge = getKb();
		CircularBuffer producer = new CircularBuffer();
		UpdateSettings settings = new UpdateSettings();
		settings.setTreatGlobalsAsLocals(true);
		producer.setSettings(settings);

		producer.setName(knowledge, "buffer");
		producer.resize(10);

		KnowledgeRecord[] init_values = new KnowledgeRecord[10];
		KnowledgeRecord[] records;

		for (long i = 0; i < 10; ++i)
			init_values[(int) i] = new KnowledgeRecord(i);

		producer.add(init_values[0]);

		Assert.assertEquals(producer.get().toLong(), 0);

		producer.add(init_values);
		records = producer.getEarliest(10);

		Assert.assertEquals(records.length, 10);

		// clean up
		for (KnowledgeRecord record : records) {
			record.free();
		}

		records = producer.getLatest(10);

		Assert.assertEquals(records.length, 10);

		// clean up
		for (KnowledgeRecord record : records) {
			record.free();
		}

		// clean up
		for (KnowledgeRecord record : init_values) {
			record.free();
		}
	}

	@Test
	public void testCircularBufferConsumer() throws MadaraDeadObjectException {

		KnowledgeBase knowledge = getKb();
		CircularBuffer producer = new CircularBuffer();
		CircularBufferConsumer consumer = new CircularBufferConsumer();
		UpdateSettings settings = new UpdateSettings();
		settings.setTreatGlobalsAsLocals(true);
		producer.setSettings(settings);

		producer.setName(knowledge, "buffer");
		consumer.setName(knowledge, "buffer");
		producer.resize(10);
		consumer.resize();

		KnowledgeRecord[] init_values = new KnowledgeRecord[10];
		KnowledgeRecord[] records;

		for (long i = 0; i < 10; ++i)
			init_values[(int) i] = new KnowledgeRecord(i);

		producer.add(init_values[0]);

		Assert.assertEquals(consumer.consume().toLong(), 0);

		producer.add(init_values);
		records = consumer.consumeEarliest(10);

		Assert.assertEquals(records.length, 10);

		// clean up
		for (KnowledgeRecord record : records) {
			record.free();
		}

		producer.add(init_values);

		records = consumer.consumeLatest(10);

		Assert.assertEquals(records.length, 10);

		// clean up
		for (KnowledgeRecord record : records) {
			record.free();
		}

		// clean up
		for (KnowledgeRecord record : init_values) {
			record.free();
		}

	}

	@Test
	public void testQueue() throws MadaraDeadObjectException {
		KnowledgeBase knowledge = getKb();
		Queue queue = new Queue();
		UpdateSettings settings = new UpdateSettings();
		settings.setTreatGlobalsAsLocals(true);
		queue.setSettings(settings);

		queue.setName(knowledge, "queue");

		queue.resize(10);

		queue.enqueue(10);
		queue.enqueue(12.5);
		queue.enqueue("third");

		Assert.assertEquals(queue.count(), 3);
		Assert.assertEquals(queue.size(), 10);

		KnowledgeRecord value1 = queue.dequeue(true);
		KnowledgeRecord value2 = queue.dequeue(true);
		KnowledgeRecord value3 = queue.dequeue(true);

		Assert.assertTrue(value1.isValid());
		Assert.assertTrue(value2.isValid());
		Assert.assertTrue(value3.isValid());

		Assert.assertEquals(value1.toString(), "10");
		Assert.assertEquals(value1.toLong(), 10);

		Assert.assertEquals(queue.count(), 0);
		Assert.assertEquals(queue.size(), 10);

	}

	@Test
	public void testDoubleVector() throws MadaraDeadObjectException {
		KnowledgeBase knowledge = getKb();
		DoubleVector vector = new DoubleVector();
		UpdateSettings settings = new UpdateSettings();
		settings.setTreatGlobalsAsLocals(true);
		vector.setSettings(settings);

		vector.setName(knowledge, "dblVector");

		vector.resize(10);

		vector.set(0, 10);
		vector.set(1, 12.5);
		vector.set(2, 14.234224214);
		vector.set(7, 341234.141222893);

		Assert.assertEquals(vector.size(), 10);

		KnowledgeRecord value0 = vector.toRecord(0);
		KnowledgeRecord value1 = vector.toRecord(1);
		KnowledgeRecord value2 = vector.toRecord(2);
		KnowledgeRecord value4 = vector.toRecord(4);
		KnowledgeRecord value7 = vector.toRecord(7);

		Assert.assertTrue(value0.isValid());
		Assert.assertTrue(value1.isValid());
		Assert.assertTrue(value2.isValid());
		Assert.assertFalse(value4.isValid());
		Assert.assertTrue(value7.isValid());

		Assert.assertEquals(value0.toDouble(), 10.0, 0);
		Assert.assertEquals(value1.toDouble(), 12.5, 0);

		knowledge.evaluateNoReturn("#set_precision(9)");

		Assert.assertEquals(value1.toString(), "12.500000000");
		Assert.assertEquals(value7.toString(), "341234.141222893");

	}

	@Test
	public void testCollection() throws MadaraDeadObjectException {

		KnowledgeBase knowledge = getKb();

		ai.madara.knowledge.containers.String name = new ai.madara.knowledge.containers.String();
		Integer age = new Integer();
		NativeDoubleVector gps = new NativeDoubleVector();
		Double craziness = new Double();
		StringVector shoppingList = new StringVector();
		IntegerVector shoppingListCompleted = new IntegerVector();
		Collection profile = new Collection();
		boolean ready = false;

		name.setName(knowledge, "name");
		age.setName(knowledge, "age");
		gps.setName(knowledge, "location");
		craziness.setName(knowledge, "currentCraziness");
		shoppingList.setName(knowledge, "shoppingList");
		shoppingListCompleted.setName(knowledge, "shoppingListCompleted");

		name.set("Bob Tompkins");
		age.set(49);
		gps.resize(3);
		gps.set(0, 72);
		gps.set(1, 42);
		gps.set(2, 1500);
		craziness.set(0.7);
		shoppingList.resize(10);
		shoppingList.set(0, "Corn");
		shoppingList.set(1, "Lettuce");
		shoppingList.set(2, "Tomatoes");
		shoppingList.set(3, "Teriyaki");
		shoppingList.set(4, "Chicken Breast");
		shoppingList.set(5, "Milk");
		shoppingList.set(6, "Eggs");
		shoppingList.set(7, "Bread");
		shoppingList.set(8, "Bacon");
		shoppingList.set(9, "Yogurt");
		shoppingListCompleted.resize(10);
		shoppingListCompleted.set(0, 1);
		shoppingListCompleted.set(1, 1);
		shoppingListCompleted.set(2, 1);
		shoppingListCompleted.set(4, 1);
		shoppingListCompleted.set(5, 1);
		shoppingListCompleted.set(6, 1);
		shoppingListCompleted.set(7, 1);
		shoppingListCompleted.set(8, 1);

		profile.add(name);
		profile.add(age);
		profile.add(gps);
		profile.add(craziness);
		profile.add(shoppingList);

		System.out.println("  Printing current modifieds");
		System.out.println(knowledge.debugModifieds());

		System.out.println("  Clearing modified variables");
		knowledge.clearModifieds();

		System.out.println("  Printing current modifieds");
		System.out.println(knowledge.debugModifieds());

		System.out.println("  Trying modifyIfTrue with incomplete shopping list");

		ready = profile.modifyIfTrue(shoppingListCompleted);

		Assert.assertFalse(ready);

		System.out.println("  Printing current modifieds");
		System.out.println(knowledge.debugModifieds());

		shoppingListCompleted.set(3, 1);
		shoppingListCompleted.set(9, 1);

		ready = profile.modifyIfTrue(shoppingListCompleted);

		Assert.assertTrue(ready);

		System.out.println("  Printing current modifieds");
		System.out.println(knowledge.debugModifieds());
	}

}
