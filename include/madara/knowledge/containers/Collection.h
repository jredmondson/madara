
#ifndef _MADARA_CONTAINERS_COLLECTION_H_
#define _MADARA_CONTAINERS_COLLECTION_H_

#include <vector>
#include <string>
#include "madara/LockType.h"
#include "Barrier.h"
#include "BaseContainer.h"
#include "Counter.h"
#include "String.h"
#include "Integer.h"
#include "Double.h"
#include "Map.h"
#include "FlexMap.h"
#include "StringVector.h"
#include "DoubleVector.h"
#include "IntegerVector.h"
#include "BufferVector.h"
#include "NativeDoubleVector.h"
#include "NativeIntegerVector.h"
#include "Vector.h"

/**
 * @file Collection.h
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
      /**
       * @class Collection
       * @brief A collection of MADARA containers that can be used for
       *        aggregate operations on all containers in the collection
       */
      class MADARA_EXPORT Collection : public BaseContainer
      {
      public:
        /**
         * Constructor
         **/
        Collection ();

        /**
        * Copy constructor
        * @param  rhs  the source object to copy
        **/
        Collection (const Collection & rhs);

        /**
         * Destructor
         **/
        virtual ~Collection ();

        /**
        * Mark all values in the container as modified
        **/
        void modify (void);

        /**
        * Returns the type of the containers along with name and any other
        * useful information.
        *
        * @return info in format {container}: {name}{ = value, if appropriate}
        **/
        std::string get_debug_info (void);

        /**
        * Clones this container
        * @return  a deep copy of the container that must be managed
        *          by the user (i.e., you have to delete the return value)
        **/
        virtual BaseContainer * clone (void) const;

        /**
        * Adds a class implementing BaseContainer to the collection
        * @param container  the container to add
        **/
        void add (const BaseContainer & container);

        /**
         * Clears the collection of all containers
         **/
        void clear (void);

        /**
        * Sets the update settings for all containers
        * @param  settings  the new settings to use
        **/
        void set_settings (
          const KnowledgeUpdateSettings & settings);

        /**
        * Returns the size of the collection
        * @return size of the collection
        **/
        size_t size (void) const;

        /**
        * Determines if all values in the collection are true
        * @return true if all values are true
        **/
        bool is_true (void) const;

        /**
        * Determines if the value of the collection is false
        * @return true if at least one value is false
        **/
        bool is_false (void) const;

      protected:

        /**
        * Polymorphic is true method which can be used to determine if
        * all values in the container are true
        **/
        virtual bool is_true_ (void) const;

        /**
        * Polymorphic is false method which can be used to determine if
        * at least one value in the container is false
        **/
        virtual bool is_false_ (void) const;

        /**
        * Polymorphic modify method used by collection containers. This
        * method calls the modify method for this class. We separate the
        * faster version (modify) from this version (modify_) to allow
        * users the opportunity to have a fastery version that does not
        * use polymorphic functions (generally virtual functions are half
        * as efficient as normal function calls)
        **/
        virtual void modify_ (void);

        /**
        * Returns the type of the container along with name and any other
        * useful information. The provided information should be useful
        * for developers wishing to debug container operations, especially
        * as it pertains to pending network operations (i.e., when used
        * in conjunction with modify)
        *
        * @return info in format {container}: {name}{ = value, if appropriate}
        **/
        virtual std::string get_debug_info_ (void);

        /**
         * The underlying collection of containers
         **/
        std::vector <BaseContainer *> vector_;
      };
    }
  }
}

#include "Collection.inl"


#endif // _MADARA_CONTAINERS_COLLECTION_H_
