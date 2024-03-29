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
// Boost.SpatialIndex - rtree leaf implementation
//
// Copyright 2008 Federico J. Fernandez.
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
//  See http://www.boost.org/ for latest version.
//


#ifndef BOOST_SPATIAL_INDEX_RTREE_LEAF_HPP
#define BOOST_SPATIAL_INDEX_RTREE_LEAF_HPP

#include <geometry/area.hpp>

namespace boost
{
  namespace spatial_index
  {

    template < typename Point, typename Value >
      class rtree_leaf:public rtree_node < Point, Value >
    {
    public:
      /// container type for the leaves
      typedef std::vector < std::pair < geometry::box < Point >,
        Value > >leaves_map;

    public:
      /**
       * \brief Creates an empty leaf
       */
      rtree_leaf(void)
      {
      }


      /**
       * \brief Creates a new leaf, with 'parent' as parent
       */
      rtree_leaf(const boost::shared_ptr < rtree_node < Point, Value > >&parent)
        : rtree_node < Point, Value > (parent, 0)
      {
      }


      /**
       * \brief Search for elements in 'e' in the Rtree. Add them to r.
       *        If exact_match is true only return the elements having as
       *        key the box 'e'. Otherwise return everything inside 'e'.
       */
      virtual void find(const geometry::box < Point > &e,
                        std::deque < Value > &r, const bool exact_match)
      {
        for(typename leaves_map::const_iterator it = nodes_.begin();
             it != nodes_.end(); ++it) {
          if (exact_match) {
            if (it->first.max() == e.max() && it->first.min() == e.min()) {
              r.push_back(it->second);
            }
          } else {
            if (overlaps(it->first, e)) {
              r.push_back(it->second);
            }
          }
        }
      }


      /**
       * \brief Compute bounding box for this leaf
       */
      virtual geometry::box < Point > compute_box(void) const
      {
        if (nodes_.empty())
        {
          return geometry::box < Point > ();
        }

        typename leaves_map::const_iterator it = nodes_.begin();
        geometry::box < Point > r = it->first;
        it++;
        for(; it != nodes_.end(); ++it) {
          r = enlarge_box(r, it->first);
        }
        return r;
      }


      /**
       * \brief True if we are a leaf
       */
      virtual bool is_leaf(void) const
      {
        return true;
      }


      /**
       * \brief Number of elements in the tree
       */
      virtual unsigned int elements(void) const
      {
        return nodes_.size();
      }


      /**
       * \brief Insert a new element, with key 'e' and value 'v'
       */
      virtual void insert(const geometry::box < Point > &e, const Value & v)
      {
        nodes_.push_back(std::make_pair(e, v));
      }


      /**
       * \brief Proyect leaves of this node.
       */
      virtual std::vector < std::pair < geometry::box < Point >, Value > >
      get_leaves(void) const
      {
        return nodes_;
      }


      /**
       * \brief Add a new child (node) to this node
       */
      virtual void add_node(const geometry::box < Point > &b,
                            const boost::shared_ptr < rtree_node < Point,
                            Value > >&n)
      {
        throw std::logic_error("Can't add node to leaf node.");
      }


      /**
       * \brief Add a new leaf to this node
       */
      virtual void add_value(const geometry::box < Point > &b, const Value & v)
      {
        nodes_.push_back(std::make_pair(b, v));
      }


      /**
       * \brief Proyect value in position 'i' in the nodes container
       */
      virtual Value get_value(const unsigned int i) const
      {
        return nodes_[i].second;
      }


      /**
       * \brief Box projector for leaf
       */
      virtual geometry::box < Point > get_box(const unsigned int i) const
      {
        return nodes_[i].first;
      }


      /**
       * \brief Remove value with key 'k' in this leaf
       */
      virtual void remove(const geometry::box < Point > &k)
      {

        for(typename leaves_map::iterator it = nodes_.begin();
             it != nodes_.end(); ++it) {
          if (it->first.min() == k.min() && it->first.max() == k.max()) {
            nodes_.erase(it);
            return;
          }
        }
        throw std::logic_error("Node not found.");
      }


      /**
       * \brief Proyect boxes from this node
       */
      virtual std::vector < geometry::box < Point > >get_boxes(void) const
      {
        std::vector < geometry::box < Point > >r;
        for(typename leaves_map::const_iterator it = nodes_.begin();
            it != nodes_.end(); ++it) {
          r.push_back(it->first);
        }
        return r;
      }


      /**
       * \brief Print leaf (mainly for debug)
       */
      virtual void print(void) const
      {
        std::cerr << "\t" << " --> Leaf --------" << std::endl;
        std::cerr << "\t" << "  Size: " << nodes_.size() << std::endl;
        for(typename leaves_map::const_iterator it = nodes_.begin();
             it != nodes_.end(); ++it)
        {

          std::cerr << "\t" << "  | ";
          std::cerr << "( " << geometry::get < 0 >
            (it->first.min()) << " , " << geometry::get < 1 >
            (it->first.min()) << " ) x ";
          std::cerr << "( " << geometry::get < 0 >
            (it->first.max()) << " , " << geometry::get < 1 >
            (it->first.max()) << " )";
          std::cerr << " -> ";
          std::cerr << it->second;
          std::cerr << " | " << std::endl;;
        }
        std::cerr << "\t" << " --< Leaf --------" << std::endl;
      }

    private:
      /// leaves of this node
      leaves_map nodes_;
    };



  }                             // namespace spatial_index
}                               // namespace boost

#endif // BOOST_SPATIAL_INDEX_RTREE_LEAF_HPP
