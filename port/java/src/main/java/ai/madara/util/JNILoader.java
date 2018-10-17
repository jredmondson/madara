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
package ai.madara.util;

import java.io.File;
import java.io.FileOutputStream;
import java.io.InputStream;
import java.security.CodeSource;
import java.security.MessageDigest;
import java.util.Properties;

/**
 * JNILoader is used to load native libraries at runtime when they exist
 * inside a jar file. On android, it assumes the files are in libs/armeabi
 */
public class JNILoader
{
  //We use the OS name and arch in a few places
  private static final String OS_NAME;
  private static final String OS_ARCH;

  private static final String CHECKSUM;

  //Everything gets ignored if we are on Android
  private static final boolean IS_ANDROID;

  static
  {
    Properties props = System.getProperties();
    OS_NAME = props.getProperty("os.name").toLowerCase().replace(" ", "");
    OS_ARCH = props.getProperty("os.arch");
    String vmName = System.getProperty("java.vm.name");
    IS_ANDROID = vmName != null && vmName.contains("Dalvik");

    String checksumResult = null;
    try
    {
      CodeSource src = JNILoader.class.getProtectionDomain().getCodeSource();
      if(src.getLocation().toString().endsWith(".jar"))
      {
        InputStream in = src.getLocation().openStream();
        MessageDigest md = MessageDigest.getInstance("MD5");
        byte[] buffer = new byte[1024];
        int count = 0;
        while((count = in.read(buffer)) > 0)
          md.update(buffer, 0, count);

        byte[] checksum = md.digest();

        StringBuilder sb = new StringBuilder();
        for(byte b : checksum)
          sb.append(Integer.toHexString(b & 0xFF));
        checksumResult = sb.toString();
        if(checksumResult != null && checksumResult.length() == 0)
          checksumResult = null;
      }
    }
    catch (Exception e)
    {
      e.printStackTrace();
    } finally
    {
      CHECKSUM = checksumResult;
    }
  }


  /**
   * Loads a library by name as long as the preconditions are met.
   * <br> * The library is in lib/os_name/os_arch inside the jar file
   * <br> * the os_name folder is lower case and has spaces removed
   * <br> * The library follows proper naming  (ie. libTest.so, libTest.dylib, Test.dll)
   *
   * @param library The name of the library to load
   */
  @Deprecated
  public static void load(String library)
  {
    System.loadLibrary(library);
    if(true)
      return;

    //If it is android, we have to assume library files are already in libs/armeabi
    if(IS_ANDROID)
    {
      System.loadLibrary(library);
      return;
    }

    //Resolve the library
    Lib lib = resolve(library);
    if(lib == null)
      return;

    //Extract the library
    String extractedPath = extract(lib);

    System.out.println("Loading: " + extractedPath);

    //If it extracted properly, load it
    if(extractedPath != null)
      System.load(extractedPath);
  }


  /**
   * Resolves a library's and opens an input stream to it
   *
   * @param library The library to resolve
   * @return Lib object containing name and input stream
   */
  private static Lib resolve(String library)
  {
    //Get the properly formated name. This will return libTest.so/libTest.dylib/Test.dll etc
    String libName = System.mapLibraryName(library);

    //Create the file path: /lib/os_name/os_arch/lib_name
    StringBuilder filePath = new StringBuilder();
    filePath.append(File.separatorChar);
    filePath.append("lib").append(File.separatorChar);
    filePath.append(OS_NAME).append(File.separatorChar);
    filePath.append(OS_ARCH).append(File.separatorChar);
    filePath.append(libName);


    InputStream in = JNILoader.class.getResourceAsStream(filePath.toString());

    //If we fail to open the stream, return null
    if(in != null)
      return new Lib(libName, in);

    return null;
  }


  /**
   * Extrat a lib to the local temp directory
   *
   * @param lib Lib object containing lib name and a valid input stream
   * @return path to the extracted library
   */
  private static String extract(Lib lib)
  {
    if(lib == null)
      return null;

    //Create the temp directory name
    StringBuilder tempDirName = new StringBuilder();
    //tempDirName.append (System.getProperty ("java.io.tmpdir"));
    tempDirName.append("jniloader").append(File.separator);
    tempDirName.append(OS_NAME).append(File.separator);
    tempDirName.append(OS_ARCH).append(File.separator);
    if(CHECKSUM != null)
      tempDirName.append(CHECKSUM).append(File.separator);

    //Make sure the directories exist
    File tempDir = new File(System.getProperty("java.io.tmpdir"), tempDirName.toString());
    tempDir.mkdirs();

    //If this file is already extracted, just return the absolute path
    File outFile = new File(tempDir, lib.name);
    if(outFile.exists())
      return outFile.getAbsolutePath();


    //Write the library file, 2048 bytes at a time.
    try
    {
      FileOutputStream out = new FileOutputStream(outFile);

      byte[] buffer = new byte[2048];
      int count = 0;

      while((count = lib.in.read(buffer)) > 0)
        out.write(buffer, 0, count);

      lib.in.close();
      out.flush();
      out.close();

      return outFile.getAbsolutePath();

    }
    catch (Exception e)
    {
      e.printStackTrace();
    }


    return null;
  }

  private static class Lib
  {
    private String name;
    private InputStream in;

    private Lib(String name, InputStream in)
    {
      this.name = name;
      this.in = in;
    }
  }

}
