
#ifndef _MADARA_CONTAINERS_BASE_CONTAINER_H_
#define _MADARA_CONTAINERS_BASE_CONTAINER_H_

#include <vector>
#include <string>
#include "madara/Lock_Type.h"
#include "madara/knowledge_engine/Knowledge_Base.h"
#include "madara/knowledge_engine/Thread_Safe_Context.h"
#include "madara/knowledge_engine/Knowledge_Update_Settings.h"

/**
 * @file Base_Container.h
 * @author James Edmondson <jedmondson@gmail.com>
 *
 * This file contains a C++ object that manages interactions for an
 * array of variables
 **/

namespace Madara
{
  namespace Knowledge_Engine
  {
    namespace Containers
    {
      /// forward declare Collection class for friending
      class Collection;

      /**
       * @class Base_Container
       * @brief This class is an abstract base class for all containers
       */
      class MADARA_Export Base_Container
      {
      public:
        /// Allow Collection class to access protected/private areas
        friend class Collection;


        /**
         * Constructor
         * @param  name       prefix of the variable in the knowledge base
         * @param  settings   settings for updating knowledge
         **/
        Base_Container (const std::string & name = "",
                const Knowledge_Update_Settings & settings =
                  Knowledge_Update_Settings ());

        /**
        * Copy constructor
        * @param  rhs  the source object to copy
        **/
        Base_Container (const Base_Container & rhs);

        /**
         * Destructor
         **/
        virtual ~Base_Container ();

        /**
         * Returns the name of the variable
         * @return name of the variable
         **/
        std::string get_name (void) const;
        
        /**
         * Sets the update settings for the variable
         * @param  settings  the new settings to use
         **/
        void set_settings (
          const Knowledge_Update_Settings & settings);

        /**
         * Clones this container
         * @return  a deep copy of the container that must be managed
         *          by the user (i.e., you have to delete the return value)
         **/
        virtual Base_Container * clone (void) const = 0;

      protected:

        /**
        * Polymorphic modify method used by collection containers. This
        * method calls the modify method for this class. We separate the
        * faster version (modify) from this version (modify_) to allow
        * users the opportunity to have a fastery version that does not
        * use polymorphic functions (generally virtual functions are half
        * as efficient as normal function calls)
        **/
        virtual void modify_ (void) = 0;

        /**
        * Returns the type of the container along with name and any other
        * useful information. The provided information should be useful
        * for developers wishing to debug container operations, especially
        * as it pertains to pending network operations (i.e., when used
        * in conjunction with modify)
        *
        * @return info in format <container>: <name>< = value, if appropriate>
        **/
        virtual std::string get_debug_info_ (void) = 0;

        /// guard for access and changes
        typedef ACE_Guard<MADARA_LOCK_TYPE> Guard;
      
        /**
         * Mutex for local changes
         **/
        mutable MADARA_LOCK_TYPE mutex_;

        /**
         * Variable context that we are modifying
         **/
        Thread_Safe_Context * context_;

        /**
         * Prefix of variable
         **/
        std::string name_;

        /**
         * Settings for modifications
         **/
        Knowledge_Update_Settings settings_;
      };
    }
  }
}

#include "Base_Container.inl"


#endif // _MADARA_CONTAINERS_BASE_CONTAINER_H_