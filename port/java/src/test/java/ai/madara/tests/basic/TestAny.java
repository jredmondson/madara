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
 * @author David Kyle <david.kyle@shield.ai>
 *********************************************************************/

package ai.madara.tests.basic;

import org.capnproto.MessageBuilder;
import org.capnproto.StructList.Builder;
import org.junit.Assert;
import org.junit.Test;

import ai.madara.knowledge.Any;
import ai.madara.knowledge.BadAnyAccess;
import ai.madara.tests.BaseTest;
import ai.madara.tests.capnp.Geo;
import ai.madara.tests.capnp.Person;
import ai.madara.tests.capnp.Person.Bio.PhoneNumber.Type;
import ai.madara.tests.capnp.Person.Bio.Reader;

/**
 * This class is a tester for Any and AnyRef
 */
public class TestAny extends BaseTest {
	/**
	 * All the Any types are registered in {@link BaseTest#registerAnyTypes()}
	 * 
	 * @throws InterruptedException
	 * @throws Exception
	 */
	@Test
	public void testPrimitiveDataTypes() throws InterruptedException, Exception {

		// System.err.println("Loading libdatatypes_shared.so");
		// System.loadLibrary("datatypes_shared");

		Any anyChar = new Any("char");
		anyChar.assign('A');
		Assert.assertEquals("A", anyChar.toString());

		Any anyInt = new Any("int");
		anyInt.assign(100);
		Assert.assertEquals(100, anyInt.toInteger());

		Any anyFloat = new Any("float");
		anyFloat.assign(123f);
		Assert.assertEquals(123f, anyFloat.toDouble(), 0.0);

		Any anyDouble = new Any("double");
		anyDouble.assign(3.14159);
		Assert.assertEquals((double) 22 / 7, anyDouble.toDouble(), 0.1);

		Any anyStr = new Any("string");
		anyStr.assign("Jungle Book");
		Assert.assertEquals("Jungle Book", anyStr.toString());

	}

	public void testListsTypes() {

	}

	public void testMapTypes() throws BadAnyAccess {
		Any a0 = new Any("smap");
		a0.at("hello").assign("world");

		Assert.assertEquals(a0.at("hello").toString(), "world");

		Any a1 = new Any("strvec");
		a1.at(5).assign("fifth");
		a1.at(7).assign("seventh");
		a1.at(2).assign("second");

		Assert.assertFalse(a1.at(0).empty());
		Assert.assertNotNull(a1.at(2));
		Assert.assertEquals(a1.at(2).toString(), "second");

	}

	@Test
	public void testSimpleObjectType() throws BadAnyAccess {
		Any c0 = getGeoPoint();

		Geo.Point.Reader reader = c0.reader(Geo.Point.factory);
		Assert.assertEquals(reader.getX(), DEFAULT_GEO_POINT[0], 0.0);
		Assert.assertEquals(reader.getY(), DEFAULT_GEO_POINT[1], 0.01);
		Assert.assertEquals(reader.getZ(), DEFAULT_GEO_POINT[2], 0);
	}

	@Test
	public void testComplexObjectType() throws BadAnyAccess {

		MessageBuilder bioMb = new MessageBuilder();
		Person.Bio.Builder bioBuilder = bioMb.initRoot(Person.Bio.factory);
		ai.madara.tests.capnp.Person.Date.Builder dobBuilder = bioBuilder.initBirthdate();
		dobBuilder.setDay((byte) 29);
		dobBuilder.setMonth((byte) 03);
		dobBuilder.setYear((short) 1986);

		bioBuilder.setEmail("amit@hakoonamatata.com");
		bioBuilder.setName("Amit S");

		Builder<ai.madara.tests.capnp.Person.Bio.PhoneNumber.Builder> phones = bioBuilder.initPhones(3);

		ai.madara.tests.capnp.Person.Bio.PhoneNumber.Builder homePhoneBuilder = phones.get(0);
		homePhoneBuilder.setNumber("+1 9900-123-123");
		homePhoneBuilder.setType(Type.HOME);

		bioBuilder.setBirthdate(dobBuilder.asReader());

		// Convert Person object to Any type.
		Any person = new Any(Person.Bio.factory, bioMb);

		// Read from Any type again.
		Reader obj = person.reader(Person.Bio.factory);

		// Assertion
		Assert.assertEquals(obj.getName().toString(), "Amit S");
	}

}
