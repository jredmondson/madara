
#ifndef _MADARA_CONTAINERS_COLLECTION_H_
#define _MADARA_CONTAINERS_COLLECTION_H_

#include <vector>
#include <string>
#include "madara/Lock_Type.h"
#include "Barrier.h"
#include "Base_Container.h"
#include "Counter.h"
#include "String.h"
#include "Integer.h"
#include "Double.h"
#include "Map.h"
#include "Flex_Map.h"
#include "String_Vector.h"
#include "Double_Vector.h"
#include "Integer_Vector.h"
#include "Buffer_Vector.h"
#include "Native_Double_Vector.h"
#include "Native_Integer_Vector.h"
#include "Vector.h"

/**
 * @file Collection.h
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
      /**
       * @class Collection
       * @brief A collection of MADARA containers that can be used for
       *        aggregate operations on all containers in the collection
       */
      class MADARA_Export Collection : public Base_Container
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
        * @return info in format <container>: <name>< = value, if appropriate>
        **/
        std::string get_debug_info (void);

        /**
        * Clones this container
        * @return  a deep copy of the container that must be managed
        *          by the user (i.e., you have to delete the return value)
        **/
        virtual Base_Container * clone (void) const;

        /**
        * Adds a Barrier container to the collection
        * @param container  the container to add
        **/
        void add (const Barrier & container);

        /**
        * Adds a Counter container to the collection
        * @param container  the container to add
        **/
        void add (const Counter & container);

        /**
         * Adds an Integer container to the collection
         * @param container  the container to add
         **/
        void add (const Integer & container);

        /**
        * Adds an Integer_Vector container to the collection
        * @param container  the container to add
        **/
        void add (const Integer_Vector & container);

        /**
        * Adds an Native_Integer_Vector container to the collection
        * @param container  the container to add
        **/
        void add (const Native_Integer_Vector & container);

        /**
        * Adds a Double container to the collection
        * @param container  the container to add
        **/
        void add (const Double & container);

        /**
        * Adds a Double_Vector container to the collection
        * @param container  the container to add
        **/
        void add (const Double_Vector & container);

        /**
        * Adds an Native_Double_Vector container to the collection
        * @param container  the container to add
        **/
        void add (const Native_Double_Vector & container);

        /**
        * Adds a String container to the collection
        * @param container  the container to add
        **/
        void add (const String & container);

        /**
        * Adds a String_Vector container to the collection
        * @param container  the container to add
        **/
        void add (const String_Vector & container);

        /**
        * Adds a Buffer_Vector container to the collection
        * @param container  the container to add
        **/
        void add (const Buffer_Vector & container);

        /**
        * Adds a Map container to the collection
        * @param container  the container to add
        **/
        void add (const Map & container);

        /**
        * Adds a Flex_Map container to the collection
        * @param container  the container to add
        **/
        void add (const Flex_Map & container);

        /**
        * Adds a Vector container to the collection
        * @param container  the container to add
        **/
        void add (const Vector & container);

        /**
        * Adds a Collection container to the collection
        * @param container  the container to add
        **/
        void add (const Collection & container);

        /**
         * Clears the collection of all containers
         **/
        void clear (void);

        /**
        * Sets the update settings for all containers
        * @param  settings  the new settings to use
        **/
        void set_settings (
          const Knowledge_Update_Settings & settings);

        /**
        * Returns the size of the collection
        * @return size of the collection
        **/
        size_t size (void) const;

      protected:

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
        * @return info in format <container>: <name>< = value, if appropriate>
        **/
        virtual std::string get_debug_info_ (void);

        /**
         * The underlying collection of containers
         **/
        std::vector <Base_Container *> vector_;
      };
    }
  }
}

#include "Collection.inl"


#endif // _MADARA_CONTAINERS_COLLECTION_H_