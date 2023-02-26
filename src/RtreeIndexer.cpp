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
// RtreeIndexer.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "rtree.hpp"
#include <iostream>


int _tmain(int argc, _TCHAR* argv[])
{
	typedef geometry::point_xy<double> point_type;
	typedef unsigned int value_type;


	{
		boost::spatial_index::spatial_index<point_type, value_type, 
			boost::spatial_index::rtree<point_type, value_type> > q(4, 2);

		std::cerr << std::endl;
		std::cerr << " --> bulk insert" << std::endl;
		std::cerr << std::endl;

		std::vector<unsigned int> data;
		std::vector< geometry::point_xy<double> > points;

		points.push_back(geometry::point_xy<double>(2.0,2.0));
		points.push_back(geometry::point_xy<double>(1.0,3.0));
		points.push_back(geometry::point_xy<double>(1.5,5.0));
		points.push_back(geometry::point_xy<double>(5.5,5.0));
		points.push_back(geometry::point_xy<double>(2.0,5.0));

		data.push_back(1);
		data.push_back(2);
		data.push_back(3);
		data.push_back(4);
		data.push_back(5);

		q.bulk_insert(data, points);

		// q.print();
	}
	return 0;
}

