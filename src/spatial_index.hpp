/////////////////////////////////////////////////////////////////////////////
//
// OpenGraphRouter is being developed and release under an MIT-style license.
// 
// Copyright (c) 2010 Mohammad Ashraf Hossain
// 
// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the 
// Software is furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included
// in all copies of this Software or works derived from this Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
// 
/////////////////////////////////////////////////////////////////////////////
//
// Boost.SpatialIndex - spatial index interface
//
// Copyright 2008 Federico J. Fernandez.
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
//  See http://www.boost.org/ for latest version.
//


#ifndef BOOST_SPATIAL_INDEX_SPATIAL_INDEX_HPP
#define BOOST_SPATIAL_INDEX_SPATIAL_INDEX_HPP

#include <vector>
#include <deque>

#include <geometry/geometry.hpp>
#include <geometry/box.hpp>

namespace boost
{
  namespace spatial_index
  {

    /// spatial index interface
    template < typename Point, typename Value, typename Index >
      class spatial_index
    {
    private:
      /// non constructible
      spatial_index() {
      }
    };


    /// forward declaration for quadtree
    template < typename Point, typename Value > class quadtree;

    /// forward declaration for rtree
    template < typename Point, typename Value > class rtree;


    /// specialization for quadtree
    template < typename Point, typename Value >
      class spatial_index < Point, Value, quadtree < Point, Value > > {
    public:
      /// quadtree constructor
      spatial_index(const geometry::box < Point > &b,
                    const unsigned int M):i_(b, M)
      {
      }

      /// insert data 'v' with key 'k'
      void insert(const Point & k, const Value & v)
      {
        i_.insert(k, v);
      }

      /// remove data with key 'k'
      void remove(const Point & k)
      {
        i_.remove(k);
      }

      /// bulk insert data from values
      void bulk_insert(std::vector < Value > &values,
                       std::vector < Point > &points)
      {
        i_.bulk_insert(values, points);
      }

      /// find element with key 'k'
      Value find(const Point & k)
      {
        return i_.find(k);
      }

      /// find element in the defined rectangle
      /// TODO: change container
      std::deque < Value > find(const geometry::box < Point > &r) {
        return i_.find(r);
      }

      /// element count in the index
      unsigned int elements(void) const
      {
        return i_.elements();
      }

      /// debug print
      void print(void)
      {
        i_.print();
      }

    private:
      quadtree < Point, Value > i_;
    };

    /// specialization for rtree
    template < typename Point, typename Value >
      class spatial_index < Point, Value, rtree < Point, Value > > {
    public:

      /// rtree constructor
      spatial_index(const unsigned int m, const unsigned int M):i_(m, M)
      {
      }

      /// insert data 'v' with key 'k'
      void insert(const Point & k, const Value & v)
      {
        i_.insert(k, v);
      }

      /// insert data with envelope 'e' with key 'k'
      void insert(const geometry::box < Point > &e, const Value & v)
      {
        i_.insert(e, v);
      }

      /// remove data with key 'k'
      void remove(const Point & k)
      {
        i_.remove(k);
      }

      /// remove data with key 'k'
      void remove(const geometry::box < Point > &k)
      {
        i_.remove(k);
      }

      /// bulk insert data from values
      void bulk_insert(std::vector < Value > &values,
                       std::vector < Point > &points)
      {
        i_.bulk_insert(values, points);
      }

      /// find element with key 'k'
      Value find(const Point & k)
      {
        return i_.find(k);
      }

      /// find element in the defined rectangle
      /// TODO: change container
      std::deque < Value > find(const geometry::box < Point > &r) {
        return i_.find(r);
      }

      /// element count in the index
      unsigned int elements(void) const
      {
        return i_.elements();
      }

      /// debug print
      void print(void)
      {
        i_.print();
      }

    private:
      rtree < Point, Value > i_;
    };


  }                             // namespace spatial_index
}                               // namespace boost

#endif // BOOST_SPATIAL_INDEX_SPATIAL_INDEX_HPP
