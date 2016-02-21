
#ifndef _MADARA_CONTAINERS_BASE_CONTAINER_H_
#define _MADARA_CONTAINERS_BASE_CONTAINER_H_

#include <vector>
#include <string>
#include "madara/LockType.h"
#include "madara/knowledge/KnowledgeBase.h"
#include "madara/knowledge/ThreadSafeContext.h"
#include "madara/knowledge/KnowledgeUpdateSettings.h"

/**
 * @file BaseContainer.h
 * @author James Edmondson <jedmondson@gmail.com>
 *
 * This file contains a C++ object that manages interactions for an
 * array of variables
 **/

namespace madara
{
  namespace knowledge
  {
    namespace containers
    {
      /// forward declare Collection class for friending
      class Collection;

      /**
       * @class BaseContainer
       * @brief This class is an abstract base class for all containers
       */
      class MADARA_Export BaseContainer
      {
      public:
        /// Allow Collection class to access protected/private areas
        friend class Collection;


        /**
         * Constructor
         * @param  name       prefix of the variable in the knowledge base
         * @param  settings   settings for updating knowledge
         **/
        BaseContainer (const std::string & name = "",
                const KnowledgeUpdateSettings & settings =
                  KnowledgeUpdateSettings ());

        /**
        * Copy constructor
        * @param  rhs  the source object to copy
        **/
        BaseContainer (const BaseContainer & rhs);

        /**
         * Destructor
         **/
        virtual ~BaseContainer ();

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
          const KnowledgeUpdateSettings & settings);

        /**
         * Clones this container
         * @return  a deep copy of the container that must be managed
         *          by the user (i.e., you have to delete the return value)
         **/
        virtual BaseContainer * clone (void) const = 0;

        /**
        * Modifies the container if the argument is true
        * @param  conditional  the container that must be true to modify
        * @return  true if the container was modified
        **/
        virtual bool modify_if_true (BaseContainer & conditional);

        /**
        * Modifies the container if the argument is false
        * @param  conditional  the container that must be false to modify
        * @return  true if the container was modified
        **/
        virtual bool modify_if_false (BaseContainer & conditional);

      protected:

        /**
        * Polymorphic is true method which can be used to determine if
        * all values in a container are true
        **/
        virtual bool is_true_ (void) const = 0;

        /**
        * Polymorphic is false method which can be used to determine if
        * all values in a container are false
        **/
        virtual bool is_false_ (void) const = 0;

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
        
      
        /**
         * Mutex for local changes
         **/
        mutable MADARA_LOCK_TYPE mutex_;

        /**
         * Variable context that we are modifying
         **/
        ThreadSafeContext * context_;

        /**
         * Prefix of variable
         **/
        std::string name_;

        /**
         * Settings for modifications
         **/
        KnowledgeUpdateSettings settings_;
      };
    }
  }
}

#include "BaseContainer.inl"


#endif // _MADARA_CONTAINERS_BASE_CONTAINER_H_