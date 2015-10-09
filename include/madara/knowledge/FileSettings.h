#ifndef  _MADARA_FILE_SETTINGS_H_
#define  _MADARA_FILE_SETTINGS_H_

/**
 * @file FileSettings.h
 * @author James Edmondson <jedmondson@gmail.com>
 *
 * This file contains the FileSettings class, which contains policy
 * information concerning how a file is prioritized, modified, and handled.
 **/

namespace madara
{
  namespace knowledge
  {
    /**
     * @class FileSettings
     * @brief This class provides an interface into the KnowledgeEngine
     *        that governs how files are prioritized, modified, and handled
     **/
    class FileSettings
    {
    public:
      int priority;
    };
  }
}

#endif // _MADARA_FILE_SETTINGS_H_
