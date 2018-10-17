/*********************************************************************
 * Copyright (c) 2018 Carnegie Mellon University. All Rights Reserved.
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
package ai.madara.knowledge.containers;

import ai.madara.MadaraJNI;
import ai.madara.exceptions.MadaraDeadObjectException;
import ai.madara.knowledge.KnowledgeBase;
import ai.madara.knowledge.KnowledgeRecord;
import ai.madara.knowledge.Variables;

/**
 * A facade for a dynamically typed vector within a knowledge base.
 **/

public class CircularBufferConsumer extends MadaraJNI {
	private native long jni_CircularBufferConsumer();

	private native long jni_CircularBufferConsumer(long cptr);

	private static native void jni_freeCircularBufferConsumer(long cptr);

	private native long jni_consumeRecord(long cptr);

	private native Object[] jni_consumeEarliestRecordVector(long cptr, int count);

	private native Object[] jni_consumeLatestRecordVector(long cptr, int count);

	private native long jni_inspectRecord(long cptr, int position);

	private native Object[] jni_inspectRecordVector(long cptr, int position, int count);

	private native long jni_peekRecord(long cptr);

	private native Object[] jni_peekRecordVector(long cptr, int count);

	private native java.lang.String jni_getName(long cptr);

	private native void jni_setName(long cptr, long type, long kb, java.lang.String name);

	private native void jni_setIndex(long cptr, long index);

	private native long jni_remaining(long cptr);

	private native void jni_resync(long cptr);

	private native long jni_size(long cptr);

	private native long jni_count(long cptr);

	private native void jni_resize(long cptr);

	private boolean manageMemory = true;

	/**
	 * Default constructor
	 **/
	public CircularBufferConsumer() {
		setCPtr(jni_CircularBufferConsumer());
	}

	/**
	 * Copy constructor
	 * 
	 * @param input instance to copy
	 **/
	public CircularBufferConsumer(CircularBufferConsumer input) {
		setCPtr(jni_CircularBufferConsumer(input.getCPtr()));
	}

	/**
	 * Creates a java object instance from a C/C++ pointer
	 *
	 * @param cptr C pointer to the object
	 * @return a new java instance of the underlying pointer
	 */
	public static CircularBufferConsumer fromPointer(long cptr) {
		CircularBufferConsumer ret = new CircularBufferConsumer();
		ret.manageMemory = true;
		ret.setCPtr(cptr);
		return ret;
	}

	/**
	 * Creates a java object instance from a C/C++ pointer
	 *
	 * @param cptr         C pointer to the object
	 * @param shouldManage if true, manage the pointer
	 * @return a new java instance of the underlying pointer
	 */
	public static CircularBufferConsumer fromPointer(long cptr, boolean shouldManage) {
		CircularBufferConsumer ret = new CircularBufferConsumer();
		ret.manageMemory = shouldManage;
		ret.setCPtr(cptr);
		return ret;
	}

	/**
	 * Inspects the record at the specified position from local index
	 *
	 * @param position position from local to inspect
	 * @return the record at the position
	 * @throws MadaraDeadObjectException throws exception if object is already
	 *                                   released
	 */
	public KnowledgeRecord inspect(int position) throws MadaraDeadObjectException {
		return KnowledgeRecord.fromPointer(jni_inspectRecord(getCPtr(), position));
	}

	/**
	 * Inspects the record at the specified position from local index
	 *
	 * @param position position from local to inspect
	 * @param count    max number of elements to return
	 * @return the records at the position
	 * @throws MadaraDeadObjectException throws exception if object is already
	 *                                   released
	 */
	public KnowledgeRecord[] inspect(int position, int count) throws MadaraDeadObjectException {
		Object[] objs = jni_inspectRecordVector(getCPtr(), position, count);
		KnowledgeRecord[] records = new KnowledgeRecord[objs.length];
		for(int i = 0; i < objs.length; ++i) {
			records[i] = (KnowledgeRecord) objs[i];
		}
		return records;
	}

	/**
	 * Attempts to consume a record from the local index
	 * 
	 * @return the record
	 * @throws MadaraDeadObjectException throws exception if object is already
	 *                                   released
	 */
	public KnowledgeRecord consume() throws MadaraDeadObjectException {
		return KnowledgeRecord.fromPointer(jni_consumeRecord(getCPtr()));
	}

	/**
	 * Consumes earliest records in the buffer up to a certain count
	 *
	 * @param count max number of elements to return
	 * @return the records
	 * @throws MadaraDeadObjectException throws exception if object is already
	 *                                   released
	 */
	public KnowledgeRecord[] consumeEarliest(int count) throws MadaraDeadObjectException {
		Object[] objs = jni_consumeEarliestRecordVector(getCPtr(), count);
		KnowledgeRecord[] records = new KnowledgeRecord[objs.length];
		for(int i = 0; i < objs.length; ++i) {
			records[i] = (KnowledgeRecord) objs[i];
		}
		return records;
	}

	/**
	 * Consumes earliest records in the buffer up to a certain count. Local index
	 * will be updated to the producer's index
	 *
	 * @param count max number of elements to return
	 * @return the records
	 * @throws MadaraDeadObjectException throws exception if object is already
	 *                                   released
	 */
	public KnowledgeRecord[] consumeLatest(int count) throws MadaraDeadObjectException {
		Object[] objs = jni_consumeLatestRecordVector(getCPtr(), count);
		KnowledgeRecord[] records = new KnowledgeRecord[objs.length];
		for(int i = 0; i < objs.length; ++i) {
			records[i] = (KnowledgeRecord) objs[i];
		}
		return records;
	}

	/**
	 * Attempts to look at the most recent added record
	 * 
	 * @return the record
	 * @throws MadaraDeadObjectException throws exception if object is already
	 *                                   released
	 */
	public KnowledgeRecord peek() throws MadaraDeadObjectException {
		return KnowledgeRecord.fromPointer(jni_peekRecord(getCPtr()));
	}

	/**
	 * Attempts to look at the most recent added records up to a certain count
	 *
	 * @param count max number of elements to return
	 * @return the records
	 * @throws MadaraDeadObjectException throws exception if object is already
	 *                                   released
	 */
	public KnowledgeRecord[] peekLatest(int count) throws MadaraDeadObjectException {
		Object[] objs = jni_peekRecordVector(getCPtr(), count);
		KnowledgeRecord[] records = new KnowledgeRecord[objs.length];
		for(int i = 0; i < objs.length; ++i) {
			records[i] = (KnowledgeRecord) objs[i];
		}
		return records;
	}

	/**
	 * Gets the name of the variable
	 *
	 * @return name of the variable within the context
	 * @throws MadaraDeadObjectException throws exception if object is already
	 *                                   released
	 */
	public java.lang.String getName() throws MadaraDeadObjectException {
		return jni_getName(getCPtr());
	}

	/**
	 * Resizes the buffer to match the producer buffer length
	 * 
	 * @throws MadaraDeadObjectException throws exception if object is already
	 *                                   released
	 */
	public void resize() throws MadaraDeadObjectException {
		jni_resize(getCPtr());
	}

	/**
	 * Sets the name and knowledge base being referred to
	 *
	 * @param kb   the knowledge base that contains the name
	 * @param name the variable name
	 * @throws MadaraDeadObjectException throws exception if object is already
	 *                                   released
	 */
	public void setName(KnowledgeBase kb, java.lang.String name) throws MadaraDeadObjectException {
		jni_setName(getCPtr(), 0, kb.getCPtr(), name);
	}

	/**
	 * Sets the name and knowledge base being referred to
	 *
	 * @param vars the variables facade that contains the name
	 * @param name the variable name
	 * @throws MadaraDeadObjectException throws exception if object is already
	 *                                   released
	 */
	public void setName(Variables vars, java.lang.String name) throws MadaraDeadObjectException {
		jni_setName(getCPtr(), 1, vars.getCPtr(), name);
	}

	/**
	 * Returns the remaining records from local index to producer index
	 *
	 * @return the maximum number of records in the queue
	 * @throws MadaraDeadObjectException throws exception if object is already
	 *                                   released
	 */
	public long remaining() throws MadaraDeadObjectException {
		return jni_remaining(getCPtr());
	}

	/**
	 * Returns the maximum size of the queue
	 *
	 * @return the maximum number of records in the queue
	 * @throws MadaraDeadObjectException throws exception if object is already
	 *                                   released
	 */
	public long size() throws MadaraDeadObjectException {
		return jni_size(getCPtr());
	}

	/**
	 * @param index sets index
	 * @throws MadaraDeadObjectException throws exception if object is already
	 *                                   released
	 */
	public void setIndex(long index) throws MadaraDeadObjectException {
		jni_setIndex(getCPtr(), index);
	}

	/**
	 * Returns the number of records currently in the queue
	 *
	 * @return the number of elements in the queue
	 * @throws MadaraDeadObjectException throws exception if object is already
	 *                                   released
	 */
	public long count() throws MadaraDeadObjectException {
		return jni_count(getCPtr());
	}

	/**
	 * Resyncs the local index to the producer index
	 */
	public void resync() {
		jni_resync(getCPtr());
	}

	/**
	 * Deletes the C instantiation. To prevent memory leaks, this <b>must</b> be
	 * called before an instance gets garbage collected
	 */
	public void free() {
		if(manageMemory) {
			jni_freeCircularBufferConsumer(getCPtr());
			setCPtr(0);
		}
	}

	/**
	 * Cleans up underlying C resources
	 * 
	 * @throws Throwable necessary for override but unused
	 */
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
}
