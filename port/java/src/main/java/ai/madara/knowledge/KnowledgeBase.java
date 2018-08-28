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

import ai.madara.MadaraJNI;
import ai.madara.exceptions.MadaraDeadObjectException;
import ai.madara.logger.GlobalLogger;
import ai.madara.logger.LogLevels;
import ai.madara.logger.Logger;
import ai.madara.transport.TransportSettings;
import ai.madara.transport.TransportType;

/**
 * This class provides a distributed knowledge base to users
 **/
public class KnowledgeBase extends MadaraJNI {
    // Native Methods
    private native long jni_KnowledgeBase();

    private native long jni_KnowledgeBase(String host, int transport, String domain);

    private native long jni_KnowledgeBase(String host, long config);

    private native long jni_KnowledgeBase(long original);

    private native String jni_getID(long cptr);

    private native String jni_debugModifieds(long cptr);

    private native void jni_closeTransports(long cptr);

    private native void jni_attachLogger(long cptr, long logger);

    private native void jni_attachTransport(long cptr, String id, long settings);

    private native long jni_getLogger(long cptr);

    private native long jni_evaluate(long cptr, String expression, long evalSettings);

    private native long jni_evaluate(long cptr, long expression, long evalSettings);

    private native void jni_evaluateNoReturn(long cptr, String expression, long evalSettings);

    private native void jni_evaluateNoReturn(long cptr, long expression, long evalSettings);

    private native long jni_compile(long cptr, String expression);

    private native void jni_defineFunction(long cptr, String name, MadaraFunction function);

    private native void jni_defineFunction(long cptr, String name, String expression);

    private native void jni_print(long cptr, String statement);

    private native void jni_print(long cptr);

    private native void jni_clear(long cptr);

    private native boolean jni_exists(long cptr, String statement);

    private native void jni_sendModifieds(long cptr);

    private native void jni_sendModifieds(long cptr, long evalSettings);

    private native void jni_clearModifieds(long cptr);

    private native long jni_get(long cptr, String name);

    private static native void jni_setInteger(long cptr, String name, long value);

    private static native void jni_setDouble(long cptr, String name, double value);

    private static native void jni_setString(long cptr, String name, String value);

    private static native void jni_setIntegerArray(long cptr, String name, long[] value);

    private static native void jni_setDoubleArray(long cptr, String name, double[] value);

    private static native void jni_setFile(long cptr, String name, byte[] value);

    private static native void jni_setImage(long cptr, String name, byte[] value);

    private static native String jni_setAny(long cptr, String name, long handler, long data);

    private static native void jni_setIntegerSettings(long cptr, String name, long value, long settings);

    private static native void jni_setDoubleSettings(long cptr, String name, double value, long settings);

    private static native void jni_setStringSettings(long cptr, String name, String value, long settings);

    private static native void jni_setIntegerArraySettings(long cptr, String name, long[] value, long settings);

    private static native void jni_setDoubleArraySettings(long cptr, String name, double[] value, long settings);

    private static native void jni_setFileSettings(long cptr, String name, byte[] value, long settings);

    private static native void jni_setImageSettings(long cptr, String name, byte[] value, long settings);

    private static native String jni_setAnySettings(long cptr, String name, long handler, long data, long settings);

    private native void jni_freeKnowledgeBase(long cptr);

    private native long jni_wait(long cptr, String expression, long waitSettings);

    private native long jni_wait(long cptr, long expression, long waitSettings);

    private native void jni_waitNoReturn(long cptr, String expression, long waitSettings);

    private native void jni_waitNoReturn(long cptr, long expression, long waitSettings);

    private native long[] jni_toKnowledgeList(long cptr, String subject, int start, int end);

    private native void jni_toKnowledgeMap(long cptr, String expression, MapReturn ret);

    private native void jni_toMap(long cptr, String prefix, String suffix, MapReturn ret);

    private native long jni_saveContext(long cptr, String filename);

    private native long jni_saveAsKarl(long cptr, String filename);

    private native long jni_saveCheckpoint(long cptr, String filename, boolean resetModifieds);

    private native java.lang.String jni_toString(long cptr, java.lang.String arrayDelimiter,
	    java.lang.String recordDelimiter, java.lang.String keyvalDelimiter);

    private native long jni_loadContext(long cptr, String filename, boolean useId, long settings);

    private boolean manageMemory = true;

    /**
     * Creates a KnowledgeBase with default settings
     **/
    public KnowledgeBase() {
	setCPtr(jni_KnowledgeBase());
    }

    /**
     * Creates a KnowledgeBase
     *
     * @param host      hostname/ip of this machine
     * @param transport to use for knowledge dissemination
     * @param domain    knowledge domain we want to join
     **/
    public KnowledgeBase(String host, TransportType transport, String domain) {
	setCPtr(jni_KnowledgeBase(host, transport.value(), domain));
    }

    /**
     * Creates a KnowledgeBase
     *
     * @param host   hostname/ip of this machine
     * @param config transport settings to use for dissemination
     **/
    public KnowledgeBase(String host, TransportSettings config) {
	setCPtr(jni_KnowledgeBase(host, config.getCPtr()));
    }

    /**
     * Copy constructor
     *
     * @param original knowledge base to copy from
     **/
    public KnowledgeBase(KnowledgeBase original) {
	setCPtr(jni_KnowledgeBase(original.getCPtr()));
    }

    /**
     * Creates a java object instance from a C/C++ pointer
     *
     * @param cptr C pointer to the object
     * @return a new java instance of the underlying pointer
     **/
    public static KnowledgeBase fromPointer(long cptr) {
	KnowledgeBase ret = new KnowledgeBase();
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
     **/
    public static KnowledgeBase fromPointer(long cptr, boolean shouldManage) {
	KnowledgeBase ret = new KnowledgeBase();
	ret.manageMemory = shouldManage;
	ret.setCPtr(cptr);
	return ret;
    }

    /**
     * Get KnowledgeBase ID
     *
     * @return KnowledgeBase ID
     * @throws MadaraDeadObjectException throws exception if object is already
     *                                   released
     **/
    public String getID() throws MadaraDeadObjectException {
	return jni_getID(getCPtr());
    }

    /**
     * Evaluates an expression. <br>
     * The returned KnowledgeRecord <b>must</b> be freed
     * ({@link ai.madara.knowledge.KnowledgeRecord#free () KnowledgeRecord.free ()})
     * at some point. If it is to be ignored, consider using
     * {@link #evaluateNoReturn (String)}
     *
     * @param expression KaRL expression to evaluate
     * @return value of expression
     * @throws KnowledgeBaseLockedException If called from a MadaraFunction
     * @throws MadaraDeadObjectException    throws exception if object is already
     *                                      released
     **/
    public KnowledgeRecord evaluate(String expression) throws MadaraDeadObjectException {
	return evaluate(expression, EvalSettings.DEFAULT_EVAL_SETTINGS);
    }

    /**
     * Evaluates an expression.
     *
     * @param expression KaRL expression to evaluate
     * @throws KnowledgeBaseLockedException If called from a MadaraFunction
     * @throws MadaraDeadObjectException    throws exception if object is already
     *                                      released
     **/
    public void evaluateNoReturn(String expression) throws MadaraDeadObjectException {
	evaluateNoReturn(expression, EvalSettings.DEFAULT_EVAL_SETTINGS);
    }

    /**
     * Evaluates an expression. The returned KnowledgeRecord <b>must</b> be freed
     * ({@link ai.madara.knowledge.KnowledgeRecord#free () KnowledgeRecord.free ()})
     * at some point. If it is to be ignored, consider using
     * {@link #evaluateNoReturn (String,EvalSettings)}
     *
     * @param expression   KaRL expression to evaluate
     * @param evalSettings Settings for evaluating and printing
     * @return value of expression
     * @throws KnowledgeBaseLockedException If called from a MadaraFunction
     * @throws MadaraDeadObjectException    throws exception if object is already
     *                                      released
     **/
    public KnowledgeRecord evaluate(String expression, EvalSettings evalSettings) throws MadaraDeadObjectException {
	return KnowledgeRecord.fromPointer(jni_evaluate(getCPtr(), expression, evalSettings.getCPtr()));
    }

    /**
     * Evaluates an expression.
     *
     * @param expression   KaRL expression to evaluate
     * @param evalSettings evalSettings Settings for evaluating and printing
     * @throws KnowledgeBaseLockedException If called from a MadaraFunction
     * @throws MadaraDeadObjectException    throws exception if object is already
     *                                      released
     **/
    public void evaluateNoReturn(String expression, EvalSettings evalSettings) throws MadaraDeadObjectException {
	jni_evaluateNoReturn(getCPtr(), expression, evalSettings.getCPtr());
    }

    /**
     * Evaluates an expression. The returned KnowledgeRecord <b>must</b> be freed
     * ({@link ai.madara.knowledge.KnowledgeRecord#free () KnowledgeRecord.free ()})
     * at some point. If it is to be ignored, consider using
     * {@link #evaluateNoReturn (CompiledExpression)}
     *
     * @param expression KaRL expression to evaluate (result of
     *                   {@link #compile (String)})
     * @return value of expression
     * @throws KnowledgeBaseLockedException If called from a MadaraFunction
     * @throws MadaraDeadObjectException    throws exception if object is already
     *                                      released
     **/
    public KnowledgeRecord evaluate(CompiledExpression expression) throws MadaraDeadObjectException {
	return evaluate(expression, EvalSettings.DEFAULT_EVAL_SETTINGS);
    }

    /**
     * Evaluates an expression.
     *
     * @param expression KaRL expression to evaluate (result of
     *                   {@link #compile (String)})
     * @throws MadaraDeadObjectException    throws exception if object is already
     *                                      released
     * @throws KnowledgeBaseLockedException If called from a MadaraFunction
     **/
    public void evaluateNoReturn(CompiledExpression expression) throws MadaraDeadObjectException {
	evaluateNoReturn(expression, EvalSettings.DEFAULT_EVAL_SETTINGS);
    }

    /**
     * Evaluates an expression. The returned KnowledgeRecord <b>must</b> be freed
     * ({@link ai.madara.knowledge.KnowledgeRecord#free () KnowledgeRecord.free ()})
     * at some point. If it is to be ignored, consider using
     * {@link #evaluateNoReturn (CompiledExpression,EvalSettings)}
     *
     * @param expression   KaRL expression to evaluate (result of
     *                     {@link #compile (String)})
     * @param evalSettings Settings for evaluating and printing
     * @return value of expression
     * @throws KnowledgeBaseLockedException If called from a MadaraFunction
     * @throws MadaraDeadObjectException    throws exception if object is already
     *                                      released
     **/
    public KnowledgeRecord evaluate(CompiledExpression expression, EvalSettings evalSettings)
	    throws MadaraDeadObjectException {
	return KnowledgeRecord.fromPointer(jni_evaluate(getCPtr(), expression.getCPtr(), evalSettings.getCPtr()));
    }

    /**
     * Evaluates an expression.
     *
     * @param expression   KaRL expression to evaluate (result of
     *                     {@link #compile (String)})
     * @param evalSettings Settings for evaluating and printing
     * @throws KnowledgeBaseLockedException If called from a MadaraFunction
     * @throws MadaraDeadObjectException    throws exception if object is already
     *                                      released
     **/
    public void evaluateNoReturn(CompiledExpression expression, EvalSettings evalSettings)
	    throws MadaraDeadObjectException {
	jni_evaluateNoReturn(getCPtr(), expression.getCPtr(), evalSettings.getCPtr());
    }

    /**
     * Compiles a KaRL expression into an expression tree.
     *
     * @param expression expression to compile
     * @return {@link ai.madara.knowledge.KnowledgeBase.CompiledExpression
     *         CompiledExpression}: compiled, optimized expression tree
     * @throws KnowledgeBaseLockedException If called from a MadaraFunction
     * @throws MadaraDeadObjectException    throws exception if object is already
     *                                      released
     **/
    public CompiledExpression compile(String expression) throws MadaraDeadObjectException {
	return new CompiledExpression(jni_compile(getCPtr(), expression));
    }

    /**
     * <b>Currently unsupported</b><br>
     * Defines a function.
     *
     * @param name     name of the function
     * @param function Implementation of MadaraFunction
     * @throws KnowledgeBaseLockedException If called from a MadaraFunction
     * @throws MadaraDeadObjectException    throws exception if object is already
     *                                      released
     **/
    public void defineFunction(String name, MadaraFunction function) throws MadaraDeadObjectException {
	jni_defineFunction(getCPtr(), name, function);
    }

    /**
     * Attaches a logger that will manage all output from the KnowledgeBase
     *
     * @param logger a logger to use for managing information and debugging
     * @throws MadaraDeadObjectException throws exception if object is already
     *                                   released
     **/
    public void attachLogger(Logger logger) throws MadaraDeadObjectException {
	jni_attachLogger(getCPtr(), logger.getCPtr());
    }

    /**
     * Attaches a new transport to the Knowledge Base
     *
     * @param id       unique identifier for this agent
     * @param settings settings for the new transport
     * @throws MadaraDeadObjectException throws exception if object is already
     *                                   released
     **/
    public void attachTransport(String id, ai.madara.transport.TransportSettings settings)
	    throws MadaraDeadObjectException {
	jni_attachTransport(getCPtr(), id, settings.getCPtr());
    }

    /**
     * Returns the logger that manages all output from the KnowledgeBase
     *
     * @return the logger being used for managing information and debugging
     * @throws MadaraDeadObjectException throws exception if object is already
     *                                   released
     **/
    public Logger getLogger() throws MadaraDeadObjectException {
	return Logger.fromPointer(jni_getLogger(getCPtr()), false);
    }

    /**
     * Retrieves a stringified list of all modified variables that are ready to send
     * over transport on next send_modifieds call
     *
     * @return stringified list of the modified knowledge records
     * @throws MadaraDeadObjectException throws exception if object is already
     *                                   released
     */
    public String debugModifieds() throws MadaraDeadObjectException {
	return jni_debugModifieds(getCPtr());
    }

    /**
     * Closes any transports attached to the Knowledge Base
     * 
     * @throws MadaraDeadObjectException throws exception if object is already
     *                                   released
     */
    public void closeTransports() throws MadaraDeadObjectException {
	jni_closeTransports(getCPtr());
    }

    /**
     * Defines a MADARA KaRL function.
     *
     * @param name       name of the function
     * @param expression KaRL function body
     * @throws KnowledgeBaseLockedException If called from a MadaraFunction
     * @throws MadaraDeadObjectException    throws exception if object is already
     *                                      released
     **/
    public void defineFunction(String name, String expression) throws MadaraDeadObjectException {
	jni_defineFunction(getCPtr(), name, expression);
    }

    /**
     * Clears the knowledge base.
     *
     * @throws KnowledgeBaseLockedException If called from a MadaraFunction
     * @throws MadaraDeadObjectException    throws exception if object is already
     *                                      released
     **/
    public void clear() throws MadaraDeadObjectException {
	jni_clear(getCPtr());
    }

    /**
     * Checks to see if record exists
     * 
     * @param name name of record to check
     * @return true if the record exists, false otherwise
     * @throws MadaraDeadObjectException throws exception if object is already
     *                                   released
     **/
    public boolean exists(String name) throws MadaraDeadObjectException {
	return jni_exists(getCPtr(), name);
    }

    /**
     * Retrieves a knowledge value.
     *
     * @param name knowledge name
     * @return value of knowledge
     * @throws KnowledgeBaseLockedException If called from a MadaraFunction
     * @throws MadaraDeadObjectException    throws exception if object is already
     *                                      released
     **/
    public KnowledgeRecord get(String name) throws MadaraDeadObjectException {
	return KnowledgeRecord.fromPointer(jni_get(getCPtr(), name));
    }

    /**
     * Sets a knowledge value to a specified value.
     *
     * @param name  knowledge name
     * @param value value to set
     * @throws KnowledgeBaseLockedException If called from a MadaraFunction
     * @throws MadaraDeadObjectException    throws exception if object is already
     *                                      released
     **/
    public void set(String name, long value) throws MadaraDeadObjectException {
	jni_setInteger(getCPtr(), name, value);
    }

    /**
     * Sets a knowledge value to a specified value.
     *
     * @param name  knowledge name
     * @param value value to set
     * @throws KnowledgeBaseLockedException If called from a MadaraFunction
     * @throws MadaraDeadObjectException    throws exception if object is already
     *                                      released
     **/
    public void set(String name, double value) throws MadaraDeadObjectException {
	jni_setDouble(getCPtr(), name, value);
    }

    /**
     * Sets a knowledge value to a specified value.
     *
     * @param name  knowledge name
     * @param value value to set
     * @throws KnowledgeBaseLockedException If called from a MadaraFunction
     * @throws MadaraDeadObjectException    throws exception if object is already
     *                                      released
     **/
    public void set(String name, String value) throws MadaraDeadObjectException {
	jni_setString(getCPtr(), name, value);
    }

    /**
     * Sets a knowledge value to a specified value.
     *
     * @param name  knowledge name
     * @param value value to set
     * @throws KnowledgeBaseLockedException If called from a MadaraFunction
     * @throws MadaraDeadObjectException    throws exception if object is already
     *                                      released
     **/
    public void set(String name, double[] value) throws MadaraDeadObjectException {
	jni_setDoubleArray(getCPtr(), name, value);
    }

    /**
     * Sets a knowledge variable to a specified long array.
     *
     * @param name  knowledge name
     * @param value value to set
     * @throws KnowledgeBaseLockedException If called from a MadaraFunction
     * @throws MadaraDeadObjectException    throws exception if object is already
     *                                      released
     **/
    public void set(String name, long[] value) throws MadaraDeadObjectException {
	jni_setIntegerArray(getCPtr(), name, value);
    }

    /**
     * Sets a knowledge variable to a specified byte array.
     *
     * @param name  knowledge name
     * @param value value to set
     * @throws KnowledgeBaseLockedException If called from a MadaraFunction
     * @throws MadaraDeadObjectException    throws exception if object is already
     *                                      released
     **/
    public void setFile(String name, byte[] value) throws MadaraDeadObjectException {
	jni_setFile(getCPtr(), name, value);
    }

    /**
     * Sets a knowledge variable to a specified byte array.
     *
     * @param name  knowledge name
     * @param value value to set
     * @throws KnowledgeBaseLockedException If called from a MadaraFunction
     * @throws MadaraDeadObjectException    throws exception if object is already
     *                                      released
     **/
    public void setImage(String name, byte[] value) throws MadaraDeadObjectException {
	jni_setImage(getCPtr(), name, value);
    }

    /**
     * Sets a knowledge value to a specified Any.
     *
     * @param name  knowledge name
     * @param value value to set
     * @throws KnowledgeBaseLockedException If called from a MadaraFunction
     * @throws MadaraDeadObjectException    throws exception if object is already
     *                                      released
     **/
    public void set(String name, Any value) throws MadaraDeadObjectException {
	String err = jni_setAny(getCPtr(), name, value.handler_, value.data_);
	if (err != null) {
	    throw new RuntimeException("Unexpected BadAnyAccess: " + err);
	}
    }

    /**
     * Sets a knowledge value to a specified value.
     *
     * @param name     knowledge name
     * @param value    value to set
     * @param settings settings for evaluating the set command
     * @throws KnowledgeBaseLockedException If called from a MadaraFunction
     * @throws MadaraDeadObjectException    throws exception if object is already
     *                                      released
     **/
    public void set(String name, long value, EvalSettings settings) throws MadaraDeadObjectException {
	jni_setIntegerSettings(getCPtr(), name, value, settings.getCPtr());
    }

    /**
     * Sets a knowledge value to a specified value.
     *
     * @param name     knowledge name
     * @param value    value to set
     * @param settings settings for evaluating the set command
     * @throws KnowledgeBaseLockedException If called from a MadaraFunction
     * @throws MadaraDeadObjectException    throws exception if object is already
     *                                      released
     **/
    public void set(String name, double value, EvalSettings settings) throws MadaraDeadObjectException {
	jni_setDoubleSettings(getCPtr(), name, value, settings.getCPtr());
    }

    /**
     * Sets a knowledge value to a specified value.
     *
     * @param name     knowledge name
     * @param value    value to set
     * @param settings settings for evaluating the set command
     * @throws KnowledgeBaseLockedException If called from a MadaraFunction
     * @throws MadaraDeadObjectException    throws exception if object is already
     *                                      released
     **/
    public void set(String name, String value, EvalSettings settings) throws MadaraDeadObjectException {
	jni_setStringSettings(getCPtr(), name, value, settings.getCPtr());
    }

    /**
     * Sets a knowledge value to a specified value.
     *
     * @param name     knowledge name
     * @param value    value to set
     * @param settings settings for evaluating the set command
     * @throws KnowledgeBaseLockedException If called from a MadaraFunction
     * @throws MadaraDeadObjectException    throws exception if object is already
     *                                      released
     **/
    public void set(String name, double[] value, EvalSettings settings) throws MadaraDeadObjectException {
	jni_setDoubleArraySettings(getCPtr(), name, value, settings.getCPtr());
    }

    /**
     * Sets a knowledge value to a specified value.
     *
     * @param name     knowledge name
     * @param value    value to set
     * @param settings settings for evaluating the set command
     * @throws KnowledgeBaseLockedException If called from a MadaraFunction
     * @throws MadaraDeadObjectException    throws exception if object is already
     *                                      released
     **/
    public void set(String name, long[] value, EvalSettings settings) throws MadaraDeadObjectException {
	jni_setIntegerArraySettings(getCPtr(), name, value, settings.getCPtr());
    }

    /**
     * Sets a knowledge variable to a specified file byte array.
     *
     * @param name     knowledge name
     * @param value    value to set
     * @param settings settings for evaluating the set command
     * @throws KnowledgeBaseLockedException If called from a MadaraFunction
     * @throws MadaraDeadObjectException    throws exception if object is already
     *                                      released
     **/
    public void setFile(String name, byte[] value, EvalSettings settings) throws MadaraDeadObjectException {
	jni_setFileSettings(getCPtr(), name, value, settings.getCPtr());
    }

    /**
     * Sets a knowledge variable to a specified image byte array.
     *
     * @param name     knowledge name
     * @param value    value to set
     * @param settings settings for evaluating the set command
     * @throws KnowledgeBaseLockedException If called from a MadaraFunction
     * @throws MadaraDeadObjectException    throws exception if object is already
     *                                      released
     **/
    public void setImage(String name, byte[] value, EvalSettings settings) throws MadaraDeadObjectException {
	jni_setImageSettings(getCPtr(), name, value, settings.getCPtr());
    }

    /**
     * Sets a knowledge value to a specified Any.
     *
     * @param name     knowledge name
     * @param value    value to set
     * @param settings settings for evaluating the set command
     * @throws KnowledgeBaseLockedException If called from a MadaraFunction
     * @throws MadaraDeadObjectException    throws exception if object is already
     *                                      released
     **/
    public void set(String name, Any value, EvalSettings settings) throws MadaraDeadObjectException {
	String err = jni_setAnySettings(getCPtr(), name, value.handler_, value.data_, settings.getCPtr());

	if (err != null) {
	    throw new RuntimeException("Unexpected BadAnyAccess: " + err);
	}
    }

    /**
     * Sends all modifications to global variables since last send modifieds, eval,
     * wait, or set statement.
     * 
     * @throws MadaraDeadObjectException throws exception if object is already
     *                                   released
     **/
    public void sendModifieds() throws MadaraDeadObjectException {
	jni_sendModifieds(getCPtr());
    }

    /**
     * Clears all modified variables that were going to be sent with next call to
     * sendModifieds (also a part of eval, set, wait and similar functions)
     * 
     * @throws MadaraDeadObjectException throws exception if object is already
     *                                   released
     **/
    public void clearModifieds() throws MadaraDeadObjectException {
	jni_clearModifieds(getCPtr());
    }

    /**
     * Sends all modifications to global variables since last send modifieds, eval,
     * wait, or set statement.
     * 
     * @param settings settings to use for considering records to send
     * @throws MadaraDeadObjectException throws exception if object is already
     *                                   released
     **/
    public void sendModifieds(EvalSettings settings) throws MadaraDeadObjectException {
	jni_sendModifieds(getCPtr(), settings.getCPtr());
    }

    /**
     * Prints a statement with variable expansion
     *
     * @param statement The statement to print
     * @throws MadaraDeadObjectException throws exception if object is already
     *                                   released
     **/
    public void print(String statement) throws MadaraDeadObjectException {
	jni_print(getCPtr(), statement);
    }

    /**
     * Prints all knowledge in knowledge base
     * 
     * @throws MadaraDeadObjectException throws exception if object is already
     *                                   released
     **/
    public void print() throws MadaraDeadObjectException {
	jni_print(getCPtr());
    }

    /**
     * Deletes the C++ instantiation. To prevent memory leaks, this <b>must</b> be
     * called before an instance of KnowledgeBase gets garbage collected
     *
     * @throws KnowledgeBaseLockedException If called from a MadaraFunction
     **/
    public void free() {
	if (manageMemory) {
	    jni_freeKnowledgeBase(getCPtr());
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

    /**
     * Waits for an expression to be non-zero. <br>
     * <br>
     * The returned KnowledgeRecord <b>must</b> be freed
     * ({@link ai.madara.knowledge.KnowledgeRecord#free () KnowledgeRecord.free ()})
     * at some point. If it is to be ignored, consider using
     * {@link #waitNoReturn (String)}
     *
     * @param expression KaRL expression to evaluate
     * @return value of expression
     * @throws KnowledgeBaseLockedException If called from a MadaraFunction
     **/
    public KnowledgeRecord wait(String expression) {
	return wait(expression, WaitSettings.DEFAULT_WAIT_SETTINGS);
    }

    /**
     * Waits for an expression to be non-zero. <br>
     * <br>
     * The returned KnowledgeRecord <b>must</b> be freed
     * ({@link ai.madara.knowledge.KnowledgeRecord#free () KnowledgeRecord.free ()})
     * at some point. If it is to be ignored, consider using
     * {@link #waitNoReturn (CompiledExpression)}
     *
     * @param expression KaRL expression to evaluate (result of
     *                   {@link #compile (String)})
     * @return value of expression
     * @throws MadaraDeadObjectException    throws exception if object is already
     *                                      released
     * @throws KnowledgeBaseLockedException If called from a MadaraFunction
     **/
    public KnowledgeRecord wait(CompiledExpression expression) throws MadaraDeadObjectException {
	return wait(expression, WaitSettings.DEFAULT_WAIT_SETTINGS);
    }

    /**
     * Waits for an expression to be non-zero. <br>
     * Provides additional settings for fine-tuning the time to wait and atomic
     * print statements. <br>
     * <br>
     * The returned KnowledgeRecord <b>must</b> be freed
     * ({@link ai.madara.knowledge.KnowledgeRecord#free () KnowledgeRecord.free ()})
     * at some point. If it is to be ignored, consider using
     * {@link #waitNoReturn (String,WaitSettings)}
     *
     * @param expression KaRL expression to evaluate
     * @param settings   Settings for waiting
     * @return value of expression
     * @throws KnowledgeBaseLockedException If called from a MadaraFunction
     **/
    public KnowledgeRecord wait(String expression, WaitSettings settings) {
	return KnowledgeRecord.fromPointer(jni_wait(getCPtr(), expression, settings.getCPtr()));
    }

    /**
     * Waits for an expression to be non-zero. <br>
     * Provides additional settings for fine-tuning the time to wait and atomic
     * print statements. <br>
     * <br>
     * The returned KnowledgeRecord <b>must</b> be freed
     * ({@link ai.madara.knowledge.KnowledgeRecord#free () KnowledgeRecord.free ()})
     * at some point. If it is to be ignored, consider using
     * {@link #waitNoReturn (CompiledExpression,WaitSettings)}
     *
     * @param expression KaRL expression to evaluate (result of
     *                   {@link #compile (String)})
     * @param settings   Settings for waiting
     * @return value of expression
     * @throws MadaraDeadObjectException    throws exception if object is already
     *                                      released
     * @throws KnowledgeBaseLockedException If called from a MadaraFunction
     **/
    public KnowledgeRecord wait(CompiledExpression expression, WaitSettings settings) throws MadaraDeadObjectException {
	return new KnowledgeRecord(jni_wait(getCPtr(), expression.getCPtr(), settings.getCPtr()));
    }

    /**
     * Waits for an expression to be non-zero.
     *
     * @param expression KaRL expression to evaluate
     * @throws KnowledgeBaseLockedException If called from a MadaraFunction
     **/
    public void waitNoReturn(String expression) {
	waitNoReturn(expression, WaitSettings.DEFAULT_WAIT_SETTINGS);
    }

    /**
     * Waits for an expression to be non-zero.
     *
     * @param expression KaRL expression to evaluate (result of
     *                   {@link #compile (String)})
     * @throws KnowledgeBaseLockedException If called from a MadaraFunction
     **/
    public void waitNoReturn(CompiledExpression expression) {
	waitNoReturn(expression, WaitSettings.DEFAULT_WAIT_SETTINGS);
    }

    /**
     * Waits for an expression to be non-zero.
     *
     * @param expression KaRL expression to evaluate
     * @param settings   Settings for waiting
     * @throws KnowledgeBaseLockedException If called from a MadaraFunction
     **/
    public void waitNoReturn(String expression, WaitSettings settings) {
	jni_waitNoReturn(getCPtr(), expression, settings.getCPtr());
    }

    /**
     * Waits for an expression to be non-zero.
     *
     * @param expression KaRL expression to evaluate (result of
     *                   {@link #compile (String)})
     * @param settings   Settings for waiting
     * @throws KnowledgeBaseLockedException If called from a MadaraFunction
     **/
    public void waitNoReturn(CompiledExpression expression, WaitSettings settings) {
	jni_waitNoReturn(getCPtr(), expression.getCPtr(), settings.getCPtr());
    }

    /**
     * Fills a {@link ai.madara.knowledge.KnowledgeList KnowledgeList} with
     * {@link ai.madara.knowledge.KnowledgeRecord KnowledgeRecords} that begin with
     * a common subject and have a finite range of integer values. <br>
     * <br>
     * The returned {@link ai.madara.knowledge.KnowledgeList KnowledgeList}
     * <b>must</b> be freed using {@link ai.madara.knowledge.KnowledgeList#free
     * KnowledgeList.free ()} before the object goes out of scope.
     *
     * @param subject The common subject of the variable names. For instance, if we
     *                are looking for a range of vars like "var0", "var1", "var2",
     *                then the common subject would be "var".
     * @param start   An inclusive start index
     * @param end     An inclusive end index
     * @return {@link ai.madara.knowledge.KnowledgeList KnowledgeList} containing
     *         the requested {@link ai.madara.knowledge.KnowledgeRecord
     *         KnowledgeRecords}
     * @throws KnowledgeBaseLockedException If called from a MadaraFunction
     **/
    public KnowledgeList toKnowledgeList(String subject, int start, int end) {
	return new KnowledgeList(jni_toKnowledgeList(getCPtr(), subject, start, end));
    }

    /**
     * Saves the knowledge base to a file
     * 
     * @param filename the file to save the knowledge base to
     * @return the number of bytes written to the file
     **/
    public long saveContext(String filename) {
	return jni_saveContext(getCPtr(), filename);
    }

    /**
     * Saves the knowledge base to a file in a human-readable format that can be
     * read in with evaluate or wait statements.
     * 
     * @param filename the file to save the knowledge base to
     * @return the number of bytes written to the file
     **/
    public long saveAsKarl(String filename) {
	return jni_saveAsKarl(getCPtr(), filename);
    }

    /**
     * Loads the knowledge base from a file
     * 
     * @param filename the file to load the knowledge base from
     * @param useId    if true, read the unique ID from the file. If false, do not
     *                 use the unique ID from the file.
     * @param settings the settings for updating values in the context from file
     * @return the number of bytes read
     **/
    public long loadContext(String filename, boolean useId, EvalSettings settings) {
	return jni_loadContext(getCPtr(), filename, useId, settings.getCPtr());
    }

    /**
     * Saves a snapshot of changes to the context since the last saveContext,
     * saveCheckpoint, or sendModifieds. This is an incremental snapshot from last
     * time the modification list was cleared and can be much faster than a
     * saveContext.
     * 
     * @param filename       the file to save the knowledge base changes to
     * @param resetModifieds if true, clear the modification list (this is a bad
     *                       idea if you should be sending modifications over a
     *                       transport as well as saving them to a file.
     * @return the number of bytes written to the file
     **/
    public long saveCheckpoint(String filename, boolean resetModifieds) {
	return jni_saveCheckpoint(getCPtr(), filename, resetModifieds);
    }

    /**
     * Fills a {@link ai.madara.knowledge.KnowledgeMap KnowledgeMap} with
     * {@link ai.madara.knowledge.KnowledgeRecord KnowledgeRecords} that match an
     * expression. <br>
     * <br>
     * The returned {@link ai.madara.knowledge.KnowledgeMap KnowledgeMap}
     * <b>must</b> be freed using {@link ai.madara.knowledge.KnowledgeMap#free
     * KnowledgeMap.free ()} before the object goes out of scope.
     *
     * @param expression An expression that matches the variable names that are of
     *                   interest. Wildcards may only be at the end
     * @return {@link ai.madara.knowledge.KnowledgeMap KnowledgeMap} containing the
     *         requested {@link ai.madara.knowledge.KnowledgeRecord
     *         KnowledgeRecords}
     * @throws KnowledgeBaseLockedException If called from a MadaraFunction
     **/
    public KnowledgeMap toKnowledgeMap(String expression) {
	MapReturn jniRet = new MapReturn();
	jni_toKnowledgeMap(getCPtr(), expression, jniRet);

	return new KnowledgeMap(jniRet.keys, jniRet.vals);
    }

    /**
     * Fills a {@link ai.madara.knowledge.KnowledgeMap KnowledgeMap} with
     * {@link ai.madara.knowledge.KnowledgeRecord KnowledgeRecords} that match a
     * prefix and suffix. <br>
     * <br>
     * The returned {@link ai.madara.knowledge.KnowledgeMap KnowledgeMap}
     * <b>must</b> be freed using {@link ai.madara.knowledge.KnowledgeMap#free
     * KnowledgeMap.free ()} before the object goes out of scope.
     *
     * @param prefix the prefix of the variables (e.g., in "{prefix}*{suffix}")
     * @param suffix the suffix of the variables (e.g., in "{prefix}*{suffix}")
     * @return {@link ai.madara.knowledge.KnowledgeMap KnowledgeMap} containing the
     *         requested {@link ai.madara.knowledge.KnowledgeRecord
     *         KnowledgeRecords}
     * @throws KnowledgeBaseLockedException If called from a MadaraFunction
     **/
    public KnowledgeMap toKnowledgeMap(String prefix, String suffix) {
	MapReturn jniRet = new MapReturn();
	jni_toMap(getCPtr(), prefix, suffix, jniRet);

	return new KnowledgeMap(jniRet.keys, jniRet.vals);
    }

    /**
     * Prints the knowledge base to a string for debugging purposes
     *
     * @return string representation of the knowledge base
     **/
    @Override
    public java.lang.String toString() {
	return jni_toString(getCPtr(), ", ", ";\n", "=");
    }

    /**
     * Prints the knowledge base to a string for debugging purposes
     *
     * @param arrayDelimiter  delimiter to use between array elements
     * @param recordDelimiter delimiter to use in between records
     * @param keyvalDelimiter delimiter to use in between keys and values
     * @return string representation of the knowledge base
     **/
    public java.lang.String toString(java.lang.String arrayDelimiter, java.lang.String recordDelimiter,
	    java.lang.String keyvalDelimiter) {
	return jni_toString(getCPtr(), arrayDelimiter, recordDelimiter, keyvalDelimiter);
    }

    /**
     * Sets the log level to dictate the detail of MADARA logging.
     *
     * @param logLevel The log level to set
     **/
    public static void setLogLevel(LogLevels logLevel) {
	GlobalLogger.setLevel(logLevel.value());
    }

    /**
     * Compiled, optimized KaRL logic.
     **/
    public static class CompiledExpression extends MadaraJNI {

	private native void jni_freeCompiledExpression(long cptr);

	/**
	 * Create a Java representation of a compiled expression
	 *
	 * @param cptr pointer to a C++ instance of CompiledExpression
	 **/
	public CompiledExpression(long cptr) {
	    setCPtr(cptr);
	}

	/**
	 * Deletes the C++ instantiation. To prevent memory leaks, this <b>must</b> be
	 * called before an instance of KnowledgeBase gets garbage collected
	 **/
	public void free() {
	    jni_freeCompiledExpression(getCPtr());
	    setCPtr(0);
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

    /**
     * KnowledgeBaseLockedException will be thrown if methods are called from inside
     * a MadaraFunction
     **/
    public class KnowledgeBaseLockedException extends RuntimeException {
	private static final long serialVersionUID = 2818595961365992639L;

	private KnowledgeBaseLockedException() {
	    super(KnowledgeBase.this + " is currently locked. Are you invoking it from inside a MadaraFunction?");
	}
    }

    static class MapReturn {
	public String[] keys;
	public long[] vals;
    }
}
