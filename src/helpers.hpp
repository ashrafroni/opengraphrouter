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
// Boost.SpatialIndex - geometry helper functions
//
// Copyright 2008 Federico J. Fernandez.
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
//  See http://www.boost.org/ for latest version.
//


#ifndef BOOST_SPATIAL_INDEX_HELPERS_HPP
#define BOOST_SPATIAL_INDEX_HELPERS_HPP

namespace boost
{
  namespace spatial_index
  {


    /**
     * \brief Given two boxes, returns the minimal box that contains them
     */
    template < typename Point >
      geometry::box < Point > enlarge_box(const geometry::box < Point > &b1,
                                          const geometry::box < Point > &b2)
    {
      Point min(geometry::get < 0 > (b1.min()) < geometry::get < 0 >
                (b2.min())? geometry::get < 0 > (b1.min()) : geometry::get < 0 >
                (b2.min()),
                geometry::get < 1 > (b1.min()) < geometry::get < 1 >
                (b2.min())? geometry::get < 1 > (b1.min()) : geometry::get < 1 >
                (b2.min()));

      Point max(geometry::get < 0 > (b1.max()) > geometry::get < 0 >
                (b2.max())? geometry::get < 0 > (b1.max()) : geometry::get < 0 >
                (b2.max()),
                geometry::get < 1 > (b1.max()) > geometry::get < 1 >
                (b2.max())? geometry::get < 1 > (b1.max()) : geometry::get < 1 >
                (b2.max()));

        return geometry::box < Point > (min, max);
    }


    /**
     * \brief Compute the area of the union of b1 and b2
     */
    template < typename Point >
      double compute_union_area(const geometry::box < Point > &b1,
                                const geometry::box < Point > &b2)
    {
      geometry::box < Point > enlarged_box = enlarge_box(b1, b2);
      return geometry::area(enlarged_box);
    }


    /**
     * \brief Checks if boxes overlap
     */
    template < typename Point >
      bool overlaps(const geometry::box < Point > &b1,
                    const geometry::box < Point > &b2)
    {

      bool overlaps_x, overlaps_y;
      overlaps_x = overlaps_y = false;

      if (geometry::get < 0 > (b1.min()) >= geometry::get < 0 > (b2.min())
          && geometry::get < 0 > (b1.min()) <= geometry::get < 0 > (b2.max()))
        {
          overlaps_x = true;
        }
      if (geometry::get < 0 > (b1.max()) >= geometry::get < 0 > (b2.min())
          && geometry::get < 0 > (b1.min()) <= geometry::get < 0 > (b2.max()))
        {
          overlaps_x = true;
        }

      if (geometry::get < 1 > (b1.min()) >= geometry::get < 1 > (b2.min())
          && geometry::get < 1 > (b1.min()) <= geometry::get < 1 > (b2.max()))
        {
          overlaps_y = true;
        }
      if (geometry::get < 1 > (b1.max()) >= geometry::get < 1 > (b2.min())
          && geometry::get < 1 > (b1.min()) <= geometry::get < 1 > (b2.max()))
        {
          overlaps_y = true;
        }

      return overlaps_x && overlaps_y;
    }


  }                             // namespace spatial_index
}                               // namespace boost

#endif // BOOST_SPATIAL_INDEX_HELPERS_HPP
