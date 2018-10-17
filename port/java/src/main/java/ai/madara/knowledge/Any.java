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
package ai.madara.knowledge;

import java.nio.ByteBuffer;

import org.capnproto.MessageBuilder;
import org.capnproto.StructFactory;

/**
 * This class owns a C++ object which supports Any. Be sure to call free() on
 * this java object when you are done with it.
 */
public class Any extends AnyRef {
    /**
     * Internal constructor. Normally constructed by calling .ref() or .at() on an
     * Any
     * 
     * @param handler Handler
     * @param data    Data
     **/
    protected Any(long handler, long data) {
	super(handler, data);
    }

    private static native String jni_construct(String type, long[] out);

    private void emplace_impl(String type) throws BadAnyAccess {
	long[] out = new long[2];
	err(jni_construct(type, out));
	handler_ = out[0];
	data_ = out[1];
    }

    /**
     * Construct an Any storing a default-constructed C++ type of the given tag. Tag
     * must have been registered with Any::register_type&lt;T&gt;("tag") in C++, or
     * with one of the Java register* static functions (in AnyRef).
     * 
     * @param tag Tag
     * @throws BadAnyAccess if the given tag is unregistered
     **/
    public Any(String tag) throws BadAnyAccess {
	super(0, 0);
	emplace_impl(tag);
    }

    private static native String jni_emplace_capnp(String tag, byte[] data, long[] out);

    private <Factory extends StructFactory> void emplace_capnp_impl(Factory factory, MessageBuilder msg)
	    throws BadAnyAccess {
	String factory_name = factory.getClass().getName();
	String tag = registered_factories.get(factory_name);
	if(tag == null) {
	    throw new BadAnyAccess("Unknown type tag: " + tag);
	}

	long size = org.capnproto.Serialize.computeSerializedSizeInWords(msg) * 8;

	org.capnproto.ArrayOutputStream stream = new org.capnproto.ArrayOutputStream(ByteBuffer.allocate((int) size));

	try {
	    org.capnproto.Serialize.write(stream, msg);
	} catch (java.io.IOException e) {
	    // Shouldn't happen
	    throw new RuntimeException(e);
	}

	long[] out = new long[2];
	err(jni_emplace_capnp(tag, stream.buf.array(), out));
	handler_ = out[0];
	data_ = out[1];
    }

    /**
     * Construct a new Any with a Cap'n Proto message. Tag must have been registered
     * with Any.registerClass(), or with Any::register_schema() in C++
     * 
     * @param factory Factory object
     * @param msg     MessageBuilder object
     * @throws BadAnyAccess Exception in case of bad access
     **/
    public <Factory extends StructFactory> Any(Factory factory, MessageBuilder msg) throws BadAnyAccess {
	super(0, 0);
	emplace_capnp_impl(factory, msg);
    }

    private static native String jni_free(long handler, long data);

    /**
     * Free the held C++ object. Safe to call on an empty Any
     **/
    public void free() {
	err_unchecked(jni_free(handler_, data_));
	handler_ = 0;
	data_ = 0;
    }

    /**
     * Synonym for free(). This is the name used in the C++ interface.
     **/
    public void clear() {
	free();
    }

    /**
     * Call free() if thie value is destroyed by Java, in case someone forgot to
     * call free().
     **/
    @Override
    protected void finalize() throws Throwable {
	try {
	    free();
	} catch (Throwable t) {
	    throw t;
	} finally {
	    super.finalize();
	}
    }

    /**
     * Replace the held object, if any, with a new default-constructed C++ type of
     * the given tag. Tag must have been registered with
     * Any::register_type&lt;T&gt;("tag") in C++, or with one of the Java register* static
     * functions (in AnyRef).
     * 
     * @param type Type
     * @throws BadAnyAccess if the given tag is unregistered
     **/
    public void emplace(String type) throws BadAnyAccess {
	free();
	emplace_impl(type);
    }

    /**
     * Replace the held object, if any, with a Cap'n Proto message. Tag must have
     * been registered with Any.registerClass(), or with Any::register_schema() in
     * C++
     * 
     * @param factory Factory
     * @param <Factory> Factory class
     * @param msg     MessageBuilder
     * @throws BadAnyAccess Exception in case of bad access
     **/
    public <Factory extends StructFactory> void emplace(Factory factory, MessageBuilder msg) throws BadAnyAccess {
	free();
	emplace_capnp_impl(factory, msg);
    }
}
