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
import org.junit.Assert;
import org.junit.Test;

import ai.madara.knowledge.Any;
import ai.madara.tests.BaseTest;
import ai.madara.tests.capnp.Geo;

/**
 * This class is a tester for Any and AnyRef
 */
public class TestAny extends BaseTest {
	@Test
	public void testAny() throws InterruptedException, Exception {

		// System.err.println("Loading libdatatypes_shared.so");
		// System.loadLibrary("datatypes_shared");

		Any.registerStringVector("strvec");
		Any.registerDoubleVector("dblvec");
		Any.registerStringToStringMap("smap");

		Any.registerClass("Point", Geo.Point.factory);

		Any a0 = new Any("smap");
		a0.at("hello").assign("world");

		Assert.assertEquals(a0.at("hello").toString(), "world");

		Any a1 = new Any("strvec");
		a1.at(5).assign("fifth");
		a1.at(7).assign("seventh");
		a1.at(2).assign("second");

		for (long i = 0, n = a1.size(); i < n; ++i) {
			System.err.println("a1[" + i + "] = " + a1.at(i));
		}

		Assert.assertFalse(a1.at(0).empty());
		Assert.assertNotNull(a1.at(2));
		Assert.assertEquals(a1.at(2).toString(), "second");
		/*
		 * Any p0 = new Any("Point"); System.err.println("p0 = " + p0);
		 * 
		 * Any p1 = new Any("Pose"); System.err.println("p1 = " + p1);
		 * 
		 * kb.set("p0", p0); kb.set("p1", p1);
		 * 
		 * System.err.println("kb.p0 = " + kb.get("p0")); System.err.println("kb.p1 = "
		 * + kb.get("p1"));
		 */

		MessageBuilder msg = new MessageBuilder();
		Geo.Point.Builder builder = msg.initRoot(Geo.Point.factory);
		builder.setX(12);
		builder.setY(32);
		builder.setZ(47);
		Any c0 = new Any(Geo.Point.factory, msg);

		Geo.Point.Reader reader = c0.reader(Geo.Point.factory);
		Assert.assertEquals(reader.getX(), 12, 0.0);
		Assert.assertEquals(reader.getY(), 32, 0.01);
		Assert.assertEquals(reader.getZ(), 47, 0);

	}
}
