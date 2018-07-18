To generate new JNI bindings, you should do the following:

cd $MADARA_ROOT/port/java/src/main/java
javah -d $MADARA_ROOT/port/java/jni ai.madara.<whatever you're generating>
