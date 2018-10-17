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
package ai.madara.knowledge.containers;

import ai.madara.exceptions.MadaraDeadObjectException;
import ai.madara.knowledge.KnowledgeBase;
import ai.madara.knowledge.KnowledgeRecord;
import ai.madara.knowledge.UpdateSettings;
import ai.madara.knowledge.Variables;

/**
 * A facade for a map of variable names to values within a knowledge base
 **/

public class FlexMap extends BaseContainer {
	private native long jni_FlexMap();

	private native long jni_FlexMap(long cptr);

	private static native void jni_freeFlexMap(long cptr);

	private native void jni_clear(long cptr);

	private native void jni_erase(long cptr, java.lang.String key);

	private native java.lang.String jni_getDelimiter(long cptr);

	private native void jni_setDelimiter(long cptr, java.lang.String delimiter);

	private native void jni_setString(long cptr, java.lang.String value);

	private native void jni_setDouble(long cptr, double value);

	private native void jni_set(long cptr, long type, long value);

	private native java.lang.String jni_getName(long cptr);

	private native void jni_setName(long cptr, long type, long kb, java.lang.String name);

	private native java.lang.String[] jni_keys(long cptr, boolean firstLevel);

	private native long jni_get(long cptr, java.lang.String key);

	private native long jni_getIndex(long cptr, int index);

	private native long jni_toRecord(long cptr);

	private native void jni_modify(long cptr);

	private native long jni_toMapContainer(long cptr);

	private native void jni_setSettings(long cptr, long settings);

	private native boolean jni_isTrue(long cptr);

	private native boolean jni_isFalse(long cptr);

	private boolean manageMemory = true;

	/**
	 * Default constructor
	 **/
	public FlexMap() {
		setCPtr(jni_FlexMap());
	}

	/**
	 * Copy constructor
	 * 
	 * @param input instance to copy
	 **/
	public FlexMap(FlexMap input) {
		setCPtr(jni_FlexMap(input.getCPtr()));
	}

	/**
	 * Creates a java object instance from a C/C++ pointer
	 *
	 * @param cptr C pointer to the object
	 * @return a new java instance of the underlying pointer
	 */
	public static FlexMap fromPointer(long cptr) {
		FlexMap ret = new FlexMap();
		ret.setCPtr(cptr);
		ret.manageMemory = true;
		return ret;
	}

	/**
	 * Creates a java object instance from a C/C++ pointer
	 *
	 * @param cptr         C pointer to the object
	 * @param shouldManage if true, manage the pointer
	 * @return a new java instance of the underlying pointer
	 */
	public static FlexMap fromPointer(long cptr, boolean shouldManage) {
		FlexMap ret = new FlexMap();
		ret.manageMemory = shouldManage;
		ret.setCPtr(cptr);
		return ret;
	}

	/**
	 * Gets all keys in the map
	 *
	 * @return all keys in the map
	 * @throws MadaraDeadObjectException throws exception if object is already
	 *                                   released
	 */
	public java.lang.String[] keys() throws MadaraDeadObjectException {
		return jni_keys(getCPtr(), false);
	}

	/**
	 * Gets keys in the map, with option to get only first level keys
	 *
	 * @param firstLevel if true, only return the immediate subkeys and not full key
	 *                   names
	 * @return the keys in the map
	 * @throws MadaraDeadObjectException throws exception if object is already
	 *                                   released
	 */
	public java.lang.String[] keys(boolean firstLevel) throws MadaraDeadObjectException {
		return jni_keys(getCPtr(), firstLevel);
	}

	/**
	 * Creates a FlexMap at the keyed location
	 *
	 * @param key the next level of the FlexMap
	 * @return FlexMap pointing to the keyed location
	 * @throws MadaraDeadObjectException throws exception if object is already
	 *                                   released
	 */
	public FlexMap get(java.lang.String key) throws MadaraDeadObjectException {
		return FlexMap.fromPointer(jni_get(getCPtr(), key));
	}

	/**
	 * Creates a FlexMap at the indexed location
	 *
	 * @param index an index into the FlexMap
	 * @return FlexMap pointing to the indexed location
	 * @throws MadaraDeadObjectException throws exception if object is already
	 *                                   released
	 */
	public FlexMap get(int index) throws MadaraDeadObjectException {
		return FlexMap.fromPointer(jni_getIndex(getCPtr(), index));
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
	 * Gets the delimiter that separates the name of the map with its elements
	 *
	 * @return the delimiter that separates map name with elements
	 * @throws MadaraDeadObjectException throws exception if object is already
	 *                                   released
	 */
	public java.lang.String getDelimiter() throws MadaraDeadObjectException {
		return jni_getDelimiter(getCPtr());
	}

	/**
	 * Sets the delimiter that separates the name of the map with its elements
	 *
	 * @param delimiter the separator between the map and elements
	 * @throws MadaraDeadObjectException throws exception if object is already
	 *                                   released
	 */
	public void setDelimiter(java.lang.String delimiter) throws MadaraDeadObjectException {
		jni_setDelimiter(getCPtr(), delimiter);
	}

	/**
	 * Mark the map as modified. The maps retains the same values but will resend
	 * all values as if they had been modified.
	 * 
	 * @throws MadaraDeadObjectException throws exception if object is already
	 *                                   released
	 **/
	public void modify() throws MadaraDeadObjectException {
		jni_modify(getCPtr());
	}

	/**
	 * Returns true if the container evaluates to true
	 * 
	 * @return true if container has all true values
	 * @throws MadaraDeadObjectException throws exception if object is already
	 *                                   released
	 **/
	public boolean isTrue() throws MadaraDeadObjectException {
		return jni_isTrue(getCPtr());
	}

	/**
	 * Returns true if the container evaluates to false
	 * 
	 * @return true if container has any false values or is uninitialized
	 * @throws MadaraDeadObjectException throws exception if object is already
	 *                                   released
	 **/
	public boolean isFalse() throws MadaraDeadObjectException {
		return jni_isFalse(getCPtr());
	}

	/**
	 * Clears the variables in the map
	 * 
	 * @throws MadaraDeadObjectException throws exception if object is already
	 *                                   released
	 **/
	public void clear() throws MadaraDeadObjectException {
		jni_clear(getCPtr());
	}

	/**
	 * Erases a variable located at key in the map. This also deletes the variable
	 * from the knowledge base.
	 * 
	 * @param key the element key
	 * @throws MadaraDeadObjectException throws exception if object is already
	 *                                   released
	 **/
	public void erase(java.lang.String key) throws MadaraDeadObjectException {
		jni_erase(getCPtr(), key);
	}

	/**
	 * Sets the location to a string
	 *
	 * @param value new value
	 * @throws MadaraDeadObjectException throws exception if object is already
	 *                                   released
	 */
	public void set(java.lang.String value) throws MadaraDeadObjectException {
		jni_setString(getCPtr(), value);
	}

	/**
	 * Sets the location to a double
	 *
	 * @param value new value
	 * @throws MadaraDeadObjectException throws exception if object is already
	 *                                   released
	 */
	public void set(double value) throws MadaraDeadObjectException {
		jni_setDouble(getCPtr(), value);
	}

	/**
	 * Sets the location to an integer value
	 *
	 * @param value new value
	 * @throws MadaraDeadObjectException throws exception if object is already
	 *                                   released
	 */
	public void set(long value) throws MadaraDeadObjectException {
		jni_set(getCPtr(), 0, value);
	}

	/**
	 * Sets the location to a KnowledgeRecord value
	 *
	 * @param key   the location within the virtual map
	 * @param value new value
	 * @throws MadaraDeadObjectException throws exception if object is already
	 *                                   released
	 */
	public void set(java.lang.String key, KnowledgeRecord value) throws MadaraDeadObjectException {
		jni_set(getCPtr(), 1, value.getCPtr());
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
	 * Sets the settings for updating variables in the Knowledge Base
	 *
	 * @param settings the settings to use for updating the Knowledge Base
	 * @throws MadaraDeadObjectException throws exception if object is already
	 *                                   released
	 */
	public void setSettings(UpdateSettings settings) throws MadaraDeadObjectException {
		jni_setSettings(getCPtr(), settings.getCPtr());
	}

	/**
	 * Returns a value at the specified key
	 *
	 * @return the value at the index as a knowledge record
	 * @throws MadaraDeadObjectException throws exception if object is already
	 *                                   released
	 */
	public KnowledgeRecord toRecord() throws MadaraDeadObjectException {
		return KnowledgeRecord.fromPointer(jni_toRecord(getCPtr()));
	}

	/**
	 * Creates a Map container from the current location. Maps maintain lists of
	 * keys and are more appropriate for frequent accesses of sub keys (i.e., they
	 * are better performance, while FlexMap is more flexible).
	 *
	 * @return the value at the index as a knowledge record
	 * @throws MadaraDeadObjectException throws exception if object is already
	 *                                   released
	 */
	public Map toMapContainer() throws MadaraDeadObjectException {
		return Map.fromPointer(jni_toMapContainer(getCPtr()));
	}

	/**
	 * Deletes the C instantiation. To prevent memory leaks, this <b>must</b> be
	 * called before an instance gets garbage collected
	 */
	public void free() {
		if(manageMemory) {
			jni_freeFlexMap(getCPtr());
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
