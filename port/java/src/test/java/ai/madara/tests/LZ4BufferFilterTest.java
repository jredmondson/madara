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

package ai.madara.tests;

import org.junit.Assert;
import org.junit.Test;

import ai.madara.exceptions.MadaraDeadObjectException;
import ai.madara.filters.compression.LZ4BufferFilter;

/**
 * This class is a tester for the LZ4BufferFilter class
 */
public class LZ4BufferFilterTest {
	//@Test
	public void test() throws MadaraDeadObjectException {
		LZ4BufferFilter filter = new LZ4BufferFilter();

		byte[] buffer = new byte[500];

		buffer[0] = 'H';
		buffer[1] = 'e';
		buffer[2] = 'l';
		buffer[3] = 'l';
		buffer[4] = 'o';
		buffer[5] = ' ';
		buffer[6] = 'W';
		buffer[7] = 'o';
		buffer[8] = 'r';
		buffer[9] = 'l';
		buffer[10] = 'd';
		buffer[11] = '!';
		buffer[12] = 0;
		buffer[13] = 0;

		String plaintext = new String(buffer, 0, (int) 13);

		// Encode
		long size = filter.encode(buffer, 13, 500);
		String encoded = new String(buffer, 0, (int) size);

		Assert.assertNotEquals(encoded, plaintext);

		// Decode
		size = filter.decode(buffer, size, size);
		String decoded = new String(buffer, 0, (int) size);

		Assert.assertNotEquals(encoded, decoded);
		Assert.assertEquals(decoded, plaintext);

	}

}
