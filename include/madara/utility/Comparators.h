#pragma once

/**
 * @file Comparators.h
 * @author James Edmondson <jedmondson@gmail.com>
 *
 * This file contains the functor classes for comparison-related operations,
 * generally with non-C++ types
 **/

namespace madara
{
  namespace utility
  {
    /**
    * @class ComparisonLessThan
    * @brief Less than comparator for non-object-oriented types. Currently
    *        supports c strings only.
    */
    struct ComparisonLessThan
    {
      
      /**
      * Compares two c strings for less than relationship, e.g. in sorting
      * @param l the left item to compare (trend toward less than)
      * @param r the right item to compare (trend toward greater than)
      **/
      bool operator()(const char* l, const char* r) const
      {
        return std::strcmp(l, r) < 0;
      }
    };

    /**
    * @class ComparisonGreaterThan
    * @brief Less than comparator for non-object-oriented types. Currently
    *        supports c strings only.
    */
    struct ComparisonGreaterThan
    {
      /**
      * Compares two c strings for greater than relationship, e.g. in sorting
      * @param l the left item to compare (trend toward greater than)
      * @param r the right item to compare (trend toward less than)
      **/
      bool operator()(const char* l, const char* r) const
      {
        return std::strcmp(l, r) > 0;
      }
    };
  }
}

