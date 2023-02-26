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
#ifndef SHORTESTPATHBGL_H
#define SHORTESTPATHBGL_H

#include "ShortestPathGenerator.h"
#include <iostream>
#include <utility> 
#include <algorithm>
#include <boost/graph/graph_traits.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/dijkstra_shortest_paths.hpp>
using namespace boost;



typedef std::pair<int, int> Edge;
//typedef adjacency_list<vecS, vecS, bidirectionalS> Graph;
//typedef adjacency_list < listS, vecS, directedS,no_property, property < edge_weight_t, int > > graph_t;
typedef adjacency_list < listS, vecS, directedS, no_property, Vertex_def> graph_t;
typedef graph_traits < graph_t >::vertex_descriptor vertex_descriptor;
typedef graph_traits < graph_t >::edge_descriptor edge_descriptor;

class ShortestPathBGL : public ShortestPathGenerator
{
public:
	ShortestPathBGL(void);
	~ShortestPathBGL(void);

public:
	bool Initialize(EdgeInfoVector* pEdgeInfoVector,VertexInfoVector* pVertexInfoVector, long lNumberOfLayers);
	bool GetShortestPath(LongVector& vecRoutingPointVertex, LongVector& vecEdgeIndex,double& dAccCost);
	bool GetShortestPathVertex(LongVector& vecRoutingPointVertex, LongVector& vecVertexIndex ,double& dAccCost);
	bool GetShortestPath(RoutingPointInfo& startPoint, RoutingPointInfo& endPoint, OGRPointVector& vecRoutingPoints, double& dAccCost){return true;};

private:
	bool GetShortestPath(long lSourceVertexLabel,long lDestinationVertexLabel, LongVector& vecEdgeLabel, double& dCostOfRoute);
	bool GetShortestPathVertex(long lSourceVertexLabel,long lDestinationVertexLabel, LongVector& vecVertexLabel, double& dCostOfRoute);
	bool CreateGraph(long lCostIndex);


private:
	graph_t* pGraph; 
	EdgeInfoVector* m_pEdgeInfoVector;
	VertexInfoVector* m_pVertexInfoVector;
	long lCurrentCostIndex;
};


#endif
