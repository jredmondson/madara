#ifndef  _MADARA_FILE_SETTINGS_H_
#define  _MADARA_FILE_SETTINGS_H_

/**
 * @file File_Settings.h
 * @author James Edmondson <jedmondson@gmail.com>
 *
 * This file contains the File_Settings class, which contains policy
 * information concerning how a file is prioritized, modified, and handled.
 **/

namespace madara
{
  namespace knowledge
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

#endif // _MADARA_FILE_SETTINGS_H_
