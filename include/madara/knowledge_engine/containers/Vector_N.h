
#ifndef _MADARA_CONTAINERS_VECTOR_N_H_
#define _MADARA_CONTAINERS_VECTOR_N_H_

#include <vector>
#include <string>
#include "madara/Lock_Type.h"
#include "madara/knowledge_engine/Knowledge_Base.h"
#include "madara/knowledge_engine/Thread_Safe_Context.h"
#include "madara/knowledge_engine/Knowledge_Update_Settings.h"

/**
 * @file Vector_N.h
 * @author James Edmondson <jedmondson@gmail.com>
 *
 * DEPRECATED: This file contains a C++ object that manages interactions for a
 * multidimensional array of variables
 **/

namespace Madara
{
  namespace Knowledge_Engine
  {
    namespace Containers
    {
      /**
       * @class Vector_N
       * @brief DEPRECATED: This class stores an n-dimensional array. The
       *        array represents accessing elements in a manner similar to
       *        int my_array [0][1][2][...][N]. This is a DEPRECATED class
       *        that will be removed before the next major release.
       */
      class MADARA_Export Vector_N
      {
      public:
        /// A vector of size_ts is the supported indexing method
        typedef  std::vector <size_t>   Index;
        
        /**
         * Default constructor
         * @param  settings     settings for evaluating the vector
         **/
        Vector_N (const Knowledge_Update_Settings & settings = Knowledge_Update_Settings ());
      
        /**
         * Constructor
         * @param  name         name of the vector in the knowledge base
         * @param  knowledge    the knowledge base that will contain the vector
         * @param  settings     settings for evaluating the vector
         **/
        Vector_N (const std::string & name,
                Knowledge_Base & knowledge,
                const Knowledge_Update_Settings & settings = Knowledge_Update_Settings ());
      
        /**
         * Constructor
         * @param  name         name of the vector in the knowledge base
         * @param  knowledge    the knowledge base that will contain the vector
         * @param  settings     settings for evaluating the vector
         **/
        Vector_N (const std::string & name,
                Variables & knowledge,
                const Knowledge_Update_Settings & settings = Knowledge_Update_Settings ());
      
        /**
         * Copy constructor
         **/
        Vector_N (const Vector_N & rhs);

        /**
         * Destructor
         **/
        ~Vector_N ();
        
        /**
         * Assignment operator
         * @param  rhs    value to copy
         **/
        void operator= (const Vector_N & rhs);

        /**
         * Returns the name of the vector
         * @return name of the vector
         **/
        std::string get_name (void) const;
        
        /**
         * Sets the variable name that this refers to
         * @param var_name  the name of the variable in the knowledge base
         * @param knowledge  the knowledge base the variable is housed in
         **/
        void set_name (const std::string & var_name,
          Knowledge_Base & knowledge);
        
        /**
         * Sets the variable name that this refers to
         * @param var_name  the name of the variable in the knowledge base
         * @param knowledge  the knowledge base the variable is housed in
         **/
        void set_name (const std::string & var_name,
          Variables & knowledge);

        /**
         * Retrieves a copy of the record from the map.
         * @param  index  the index of the variable entry. Type Index is
         *                a typdef of a STL vector of size_ts, so
         *                fill this vector with location information. For
         *                instance, to access array[1][5][8], you could
         *                pass an index initialized in the following way:<br />
         *                Index index;<br />
         *                index.push_back (1);<br />
         *                index.push_back (5);<br />
         *                index.push_back (8);
         * @return the value of the entry. Modifications to this will
         *         not be reflected in the context. This is a local copy.
         **/
        Knowledge_Record operator[] (const Index & index) const;
      
        
        /**
         * Retrieves a copy of the record from the vector.
         * @param  x      column of a one dimensional array
         * @return the value of the entry. Modifications to this will
         *         not be reflected in the context. This is a local copy.
         **/
        Knowledge_Record operator () (size_t x) const; 
      
        /**
         * Retrieves a copy of the record from the vector.
         * @param  x      row of a two dimensional array
         * @param  y      column of a two dimensional array
         * @return the value of the entry. Modifications to this will
         *         not be reflected in the context. This is a local copy.
         **/
        Knowledge_Record operator () (size_t x, size_t y) const; 
      
        /**
         * Retrieves a copy of the record from the vector.
         * @param  x      first dimension of a three dimensional array
         * @param  y      second dimension of a three dimensional array
         * @param  z      third dimension of a three dimensional array
         * @return the value of the entry. Modifications to this will
         *         not be reflected in the context. This is a local copy.
         **/
        Knowledge_Record operator () (size_t x, size_t y,
          size_t z) const;
      
        /**
         * Retrieves a copy of the record from the vector.
         * @param  i1      first dimension index of a four dimensional array
         * @param  i2      second dimension index of a four dimensional array
         * @param  i3      third dimension index of a four dimensional array
         * @param  i4      fourth dimension index of a four dimensional array
         * @return the value of the entry. Modifications to this will
         *         not be reflected in the context. This is a local copy.
         **/
        Knowledge_Record operator () (size_t i1, size_t i2,
          size_t i3, size_t i4) const;

        /**
         * Read a file into the index
         * @param filename           file to read
         * @param  index  the index of the variable entry. Type Index is
         *                a typdef of a STL vector of size_ts, so
         *                fill this vector with location information. For
         *                instance, to access array[1][5][8], you could
         *                pass an index initialized in the following way:<br />
         *                Index index;<br />
         *                index.push_back (1);<br />
         *                index.push_back (5);<br />
         *                index.push_back (8);
         */
        int read_file (const Index & index, 
                       const std::string & filename);
      
        /**
         * Read a file into the index
         * @param filename           file to read
         * @param  index  the index of the variable entry. Type Index is
         *                a typdef of a STL vector of size_ts, so
         *                fill this vector with location information. For
         *                instance, to access array[1][5][8], you could
         *                pass an index initialized in the following way:<br />
         *                Index index;<br />
         *                index.push_back (1);<br />
         *                index.push_back (5);<br />
         *                index.push_back (8);
         * @param settings           settings to use when evaluating/updating
         */
        int read_file (const Index & index, 
                       const std::string & filename, 
          const Knowledge_Update_Settings & settings);

        /**
         * Atomically sets the value of an index to an arbitrary string.
         * @param  index  the index of the variable entry. Type Index is
         *                a typdef of a STL vector of size_ts, so
         *                fill this vector with location information. For
         *                instance, to access array[1][5][8], you could
         *                pass an index initialized in the following way:<br />
         *                Index index;<br />
         *                index.push_back (1);<br />
         *                index.push_back (5);<br />
         *                index.push_back (8);
         * @param   value     new value of the variable
         * @param   size      indicates the size of the value buffer
         * @return   0 if the value was set. -1 if null key
         **/
        int set_file (const Index & index,
          const unsigned char * value, size_t size);
      
        /**
         * Atomically sets the value of an index to an arbitrary string.
         * @param  index  the index of the variable entry. Type Index is
         *                a typdef of a STL vector of size_ts, so
         *                fill this vector with location information. For
         *                instance, to access array[1][5][8], you could
         *                pass an index initialized in the following way:<br />
         *                Index index;<br />
         *                index.push_back (1);<br />
         *                index.push_back (5);<br />
         *                index.push_back (8);
         * @param   value     new value of the variable
         * @param   size      indicates the size of the value buffer
         * @param   settings  settings for applying the update
         * @return   0 if the value was set. -1 if null key
         **/
        int set_file (const Index & index,
          const unsigned char * value, size_t size, 
          const Knowledge_Update_Settings & settings);

        /**
         * Atomically sets the value of an index to a JPEG image
         * @param  index  the index of the variable entry. Type Index is
         *                a typdef of a STL vector of size_ts, so
         *                fill this vector with location information. For
         *                instance, to access array[1][5][8], you could
         *                pass an index initialized in the following way:<br />
         *                Index index;<br />
         *                index.push_back (1);<br />
         *                index.push_back (5);<br />
         *                index.push_back (8);
         * @param   value     new value of the variable
         * @param   size      indicates the size of the value buffer
         * @return   0 if the value was set. -1 if null key
         **/
        int set_jpeg (const Index & index,
          const unsigned char * value, size_t size);
      
        /**
         * Atomically sets the value of an index to a JPEG image
         * @param  index  the index of the variable entry. Type Index is
         *                a typdef of a STL vector of size_ts, so
         *                fill this vector with location information. For
         *                instance, to access array[1][5][8], you could
         *                pass an index initialized in the following way:<br />
         *                Index index;<br />
         *                index.push_back (1);<br />
         *                index.push_back (5);<br />
         *                index.push_back (8);
         * @param   value     new value of the variable
         * @param   size      indicates the size of the value buffer
         * @param   settings  settings for applying the update
         * @return   0 if the value was set. -1 if null key
         **/
        int set_jpeg (const Index & index,
          const unsigned char * value, size_t size, 
          const Knowledge_Update_Settings & settings);

        /**
         * Sets a knowledge variable to a specified value
         *
         * @param  index  the index of the variable entry. Type Index is
         *                a typdef of a STL vector of size_ts, so
         *                fill this vector with location information. For
         *                instance, to access array[1][5][8], you could
         *                pass an index initialized in the following way:<br />
         *                Index index;<br />
         *                index.push_back (1);<br />
         *                index.push_back (5);<br />
         *                index.push_back (8);
         * @param value           value to set at location
         * @return                0 if successful, -1 if key is null, and
         *                        -2 if quality isn't high enough
         **/
        int set (const Index & index,
          Madara::Knowledge_Record::Integer value);

        /**
         * Sets a knowledge variable to a specified value
         *
         * @param  index  the index of the variable entry. Type Index is
         *                a typdef of a STL vector of size_ts, so
         *                fill this vector with location information. For
         *                instance, to access array[1][5][8], you could
         *                pass an index initialized in the following way:<br />
         *                Index index;<br />
         *                index.push_back (1);<br />
         *                index.push_back (5);<br />
         *                index.push_back (8);
         * @param value           value to set at location
         * @param settings        settings for applying the update
         * @return                0 if successful, -1 if key is null, and
         *                        -2 if quality isn't high enough
         **/
        int set (const Index & index,
          Madara::Knowledge_Record::Integer value, 
          const Knowledge_Update_Settings & settings);
        
        /**
         * Sets an index within an array to a specified value
         *
         * @param  index  the index of the variable entry. Type Index is
         *                a typdef of a STL vector of size_ts, so
         *                fill this vector with location information. For
         *                instance, to access array[1][5][8], you could
         *                pass an index initialized in the following way:<br />
         *                Index index;<br />
         *                index.push_back (1);<br />
         *                index.push_back (5);<br />
         *                index.push_back (8);
         * @param sub_index       index of the location in the array
         * @param value           value to set at location
         * @return                0 if successful, -1 if key is null, and
         *                        -2 if quality isn't high enough
         **/
        int set_index (const Index & index,
          size_t sub_index,
          Madara::Knowledge_Record::Integer value);

        /**
         * Sets an index within an array to a specified value
         *
         * @param  index  the index of the variable entry. Type Index is
         *                a typdef of a STL vector of size_ts, so
         *                fill this vector with location information. For
         *                instance, to access array[1][5][8], you could
         *                pass an index initialized in the following way:<br />
         *                Index index;<br />
         *                index.push_back (1);<br />
         *                index.push_back (5);<br />
         *                index.push_back (8);
         * @param sub_index       index of the location in the array
         * @param value           value to set at location
         * @param settings        settings for applying the update
         * @return                0 if successful, -1 if key is null, and
         *                        -2 if quality isn't high enough
         **/
        int set_index (const Index & index,
          size_t sub_index,
          Madara::Knowledge_Record::Integer value,
          const Knowledge_Update_Settings & settings);
        
        /**
         * Sets a knowledge variable to a specified value
         *
         * @param  index  the index of the variable entry. Type Index is
         *                a typdef of a STL vector of size_ts, so
         *                fill this vector with location information. For
         *                instance, to access array[1][5][8], you could
         *                pass an index initialized in the following way:<br />
         *                Index index;<br />
         *                index.push_back (1);<br />
         *                index.push_back (5);<br />
         *                index.push_back (8);
         * @param value           array of integers to set at the location
         * @param size            number of elements in the array
         * @return                0 if successful, -1 if key is null, and
         *                        -2 if quality isn't high enough
         **/
        int set (const Index & index,
          const Madara::Knowledge_Record::Integer * value,
          uint32_t size);
       
        /**
         * Sets a knowledge variable to a specified value
         *
         * @param  index  the index of the variable entry. Type Index is
         *                a typdef of a STL vector of size_ts, so
         *                fill this vector with location information. For
         *                instance, to access array[1][5][8], you could
         *                pass an index initialized in the following way:<br />
         *                Index index;<br />
         *                index.push_back (1);<br />
         *                index.push_back (5);<br />
         *                index.push_back (8);
         * @param value           array of integers to set at the location
         * @param size            number of elements in the array
         * @param settings        settings for applying the update
         * @return                0 if successful, -1 if key is null, and
         *                        -2 if quality isn't high enough
         **/
        int set (const Index & index,
          const Madara::Knowledge_Record::Integer * value,
          uint32_t size,
          const Knowledge_Update_Settings & settings);
       
        /**
         * Sets a knowledge variable to a specified value
         *
         * @param  index  the index of the variable entry. Type Index is
         *                a typdef of a STL vector of size_ts, so
         *                fill this vector with location information. For
         *                instance, to access array[1][5][8], you could
         *                pass an index initialized in the following way:<br />
         *                Index index;<br />
         *                index.push_back (1);<br />
         *                index.push_back (5);<br />
         *                index.push_back (8);
         * @param value           array of integers to set at the location
         * @return                0 if successful, -1 if key is null, and
         *                        -2 if quality isn't high enough
         **/
        int set (const Index & index,
          const std::vector <Knowledge_Record::Integer> & value);
       
        /**
         * Sets a knowledge variable to a specified value
         *
         * @param  index  the index of the variable entry. Type Index is
         *                a typdef of a STL vector of size_ts, so
         *                fill this vector with location information. For
         *                instance, to access array[1][5][8], you could
         *                pass an index initialized in the following way:<br />
         *                Index index;<br />
         *                index.push_back (1);<br />
         *                index.push_back (5);<br />
         *                index.push_back (8);
         * @param value           array of integers to set at the location
         * @param settings        settings for applying the update
         * @return                0 if successful, -1 if key is null, and
         *                        -2 if quality isn't high enough
         **/
        int set (const Index & index,
          const std::vector <Knowledge_Record::Integer> & value,
          const Knowledge_Update_Settings & settings);

        /**
         * Sets a knowledge variable to a specified value
         *
         * @param  index  the index of the variable entry. Type Index is
         *                a typdef of a STL vector of size_ts, so
         *                fill this vector with location information. For
         *                instance, to access array[1][5][8], you could
         *                pass an index initialized in the following way:<br />
         *                Index index;<br />
         *                index.push_back (1);<br />
         *                index.push_back (5);<br />
         *                index.push_back (8);
         * @param value           value to set at location
         * @return                0 if successful, -1 if key is null, and
         *                        -2 if quality isn't high enough
         **/
        int set (const Index & index, double value);

        /**
         * Sets a knowledge variable to a specified value
         *
         * @param  index  the index of the variable entry. Type Index is
         *                a typdef of a STL vector of size_ts, so
         *                fill this vector with location information. For
         *                instance, to access array[1][5][8], you could
         *                pass an index initialized in the following way:<br />
         *                Index index;<br />
         *                index.push_back (1);<br />
         *                index.push_back (5);<br />
         *                index.push_back (8);
         * @param value           value to set at location
         * @param settings        settings for applying the update
         * @return                0 if successful, -1 if key is null, and
         *                        -2 if quality isn't high enough
         **/
        int set (const Index & index, double value, 
          const Knowledge_Update_Settings & settings);

        /**
         * Sets an index within an array to a specified value
         *
         * @param  index  the index of the variable entry. Type Index is
         *                a typdef of a STL vector of size_ts, so
         *                fill this vector with location information. For
         *                instance, to access array[1][5][8], you could
         *                pass an index initialized in the following way:<br />
         *                Index index;<br />
         *                index.push_back (1);<br />
         *                index.push_back (5);<br />
         *                index.push_back (8);
         * @param sub_index       index of the location in the array
         * @param value           value to set at location
         * @return                0 if successful, -1 if key is null, and
         *                        -2 if quality isn't high enough
         **/
        int set_index (const Index & index,
          size_t sub_index,
          double value);

        /**
         * Sets an index within an array to a specified value
         *
         * @param  index  the index of the variable entry. Type Index is
         *                a typdef of a STL vector of size_ts, so
         *                fill this vector with location information. For
         *                instance, to access array[1][5][8], you could
         *                pass an index initialized in the following way:<br />
         *                Index index;<br />
         *                index.push_back (1);<br />
         *                index.push_back (5);<br />
         *                index.push_back (8);
         * @param sub_index       index of the location in the array
         * @param value           value to set at location
         * @param settings        settings for applying the update
         * @return                0 if successful, -1 if key is null, and
         *                        -2 if quality isn't high enough
         **/
        int set_index (const Index & index,
          size_t sub_index,
          double value,
          const Knowledge_Update_Settings & settings);

        /**
         * Sets a knowledge variable to a specified value
         *
         * @param  index  the index of the variable entry. Type Index is
         *                a typdef of a STL vector of size_ts, so
         *                fill this vector with location information. For
         *                instance, to access array[1][5][8], you could
         *                pass an index initialized in the following way:<br />
         *                Index index;<br />
         *                index.push_back (1);<br />
         *                index.push_back (5);<br />
         *                index.push_back (8);
         * @param value           array of doubles to set at the location
         * @param size            number of elements in the array
         * @return                0 if successful, -1 if key is null, and
         *                        -2 if quality isn't high enough
         **/
        int set (const Index & index,
          const double * value,
          uint32_t size);
       
        /**
         * Sets a knowledge variable to a specified value
         *
         * @param  index  the index of the variable entry. Type Index is
         *                a typdef of a STL vector of size_ts, so
         *                fill this vector with location information. For
         *                instance, to access array[1][5][8], you could
         *                pass an index initialized in the following way:<br />
         *                Index index;<br />
         *                index.push_back (1);<br />
         *                index.push_back (5);<br />
         *                index.push_back (8);
         * @param value           array of doubles to set at the location
         * @param size            number of elements in the array
         * @param settings        settings for applying the update
         * @return                0 if successful, -1 if key is null, and
         *                        -2 if quality isn't high enough
         **/
        int set (const Index & index,
          const double * value,
          uint32_t size,
          const Knowledge_Update_Settings & settings);
       
        /**
         * Sets a knowledge variable to a specified value
         *
         * @param  index  the index of the variable entry. Type Index is
         *                a typdef of a STL vector of size_ts, so
         *                fill this vector with location information. For
         *                instance, to access array[1][5][8], you could
         *                pass an index initialized in the following way:<br />
         *                Index index;<br />
         *                index.push_back (1);<br />
         *                index.push_back (5);<br />
         *                index.push_back (8);
         * @param value           array of doubles to set at the location
         * @return                0 if successful, -1 if key is null, and
         *                        -2 if quality isn't high enough
         **/
        int set (const Index & index,
          const std::vector <double> & value);
        
        /**
         * Sets a knowledge variable to a specified value
         *
         * @param  index  the index of the variable entry. Type Index is
         *                a typdef of a STL vector of size_ts, so
         *                fill this vector with location information. For
         *                instance, to access array[1][5][8], you could
         *                pass an index initialized in the following way:<br />
         *                Index index;<br />
         *                index.push_back (1);<br />
         *                index.push_back (5);<br />
         *                index.push_back (8);
         * @param value           array of doubles to set at the location
         * @param settings        settings for applying the update
         * @return                0 if successful, -1 if key is null, and
         *                        -2 if quality isn't high enough
         **/
        int set (const Index & index,
          const std::vector <double> & value,
          const Knowledge_Update_Settings & settings);
        
        /**
         * Sets a knowledge variable to a specified value
         *
         * @param  index  the index of the variable entry. Type Index is
         *                a typdef of a STL vector of size_ts, so
         *                fill this vector with location information. For
         *                instance, to access array[1][5][8], you could
         *                pass an index initialized in the following way:<br />
         *                Index index;<br />
         *                index.push_back (1);<br />
         *                index.push_back (5);<br />
         *                index.push_back (8);
         * @param value           value to set at location
         * @return                0 if successful, -1 if key is null, and
         *                        -2 if quality isn't high enough
         **/
        int set (const Index & index, const std::string & value);

        /**
         * Sets a knowledge variable to a specified value
         *
         * @param  index  the index of the variable entry. Type Index is
         *                a typdef of a STL vector of size_ts, so
         *                fill this vector with location information. For
         *                instance, to access array[1][5][8], you could
         *                pass an index initialized in the following way:<br />
         *                Index index;<br />
         *                index.push_back (1);<br />
         *                index.push_back (5);<br />
         *                index.push_back (8);
         * @param value           value to set at location
         * @param settings        settings for applying the update
         * @return                0 if successful, -1 if key is null, and
         *                        -2 if quality isn't high enough
         **/
        int set (const Index & index, const std::string & value, 
          const Knowledge_Update_Settings & settings);

        /**
         * Sets the update settings for the variable
         * @param  settings  the new settings to use
         * @return the old update settings
         **/
        Knowledge_Update_Settings set_settings (
          const Knowledge_Update_Settings & settings);

        /**
         * Sets the quality of writing to a certain variable from this entity
         *
         * @param  index  the index of the variable entry. Type Index is
         *                a typdef of a STL vector of size_ts, so
         *                fill this vector with location information. For
         *                instance, to access array[1][5][8], you could
         *                pass an index initialized in the following way:<br />
         *                Index index;<br />
         *                index.push_back (1);<br />
         *                index.push_back (5);<br />
         *                index.push_back (8);
         * @param quality         quality of writing to this location
         * @param settings        settings for referring to knowledge variables
         **/
        void set_quality (const Index & index, uint32_t quality,
               const Knowledge_Reference_Settings & settings =
                       Knowledge_Reference_Settings (false));
      

      private:
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

      /// provide the Array_N alias for the Vector_N class
      typedef  Vector_N   Array_N;
    }
  }
}

#endif // _MADARA_CONTAINERS_VECTOR_N_H_