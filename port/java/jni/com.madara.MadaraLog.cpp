/*********************************************************************
 * Usage of this software requires acceptance of the SMASH-CMU License,
 * which can be found at the following URL:
 *
 * https://code.google.com/p/smash-cmu/wiki/License
 *********************************************************************/

#include "com.madara.MadaraLog.h"

#include "madara/logger/Global_Logger.h"

namespace logger = madara::logger;


void JNICALL Java_com_madara_MadaraLog_jni_1setLogLevel (JNIEnv *env,
  jclass clazz, jint debugLevel)
{
  logger::global_logger->set_level (debugLevel);
}

jint JNICALL Java_com_madara_MadaraLog_jni_1getLogLevel (JNIEnv *, jclass)
{
  return (jint) logger::global_logger->get_level ();;
}