#ifndef  _FILE_SETTINGS_H_
#define  _FILE_SEtTINGS_H_

/**
 * @file File_Settings.h
 * @author James Edmondson <jedmondson@gmail.com>
 *
 * This file contains the File_Settings class, which contains policy
 * information concerning how a file is prioritized, modified, and handled.
 **/

namespace Madara
{
  namespace Knowledge_Engine
  {
    /**
     * @class File_Settings
     * @brief This class provides an interface into the Knowledge_Engine
     *        that governs how files are prioritized, modified, and handled
     **/
    class File_Settings
    {
    public:
      int priority;
    };
  }
}

#endif