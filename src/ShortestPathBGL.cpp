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
#include "ShortestPathBGL.h"

ShortestPathBGL::ShortestPathBGL(void)
{
	lCurrentCostIndex = 0;
	pGraph = NULL;
}

ShortestPathBGL::~ShortestPathBGL(void)
{
}


bool ShortestPathBGL::CreateGraph(long lCostIndex)
{

	if(pGraph != NULL)
	{
		delete pGraph;
		pGraph =  NULL;
	}


	long lEdgeCount = m_pEdgeInfoVector->size();		
	long lVertxCount = m_pVertexInfoVector->size();

	pGraph = new graph_t(lVertxCount);
	long lEdgeIndex;

	for(lEdgeIndex = 0 ; lEdgeIndex < lEdgeCount; lEdgeIndex++)
	{			
		edge_descriptor e; bool inserted;
		tie(e, inserted) = add_edge(m_pEdgeInfoVector->at(lEdgeIndex).startNodeLabel, m_pEdgeInfoVector->at(lEdgeIndex).endNodeLabel, (*pGraph));
		(*pGraph)[e].vcost = m_pEdgeInfoVector->at(lEdgeIndex).costVector[lCostIndex];	
		(*pGraph)[e].id = m_pEdgeInfoVector->at(lEdgeIndex).edgeLabel;

		tie(e, inserted) = add_edge(m_pEdgeInfoVector->at(lEdgeIndex).endNodeLabel, m_pEdgeInfoVector->at(lEdgeIndex).startNodeLabel, (*pGraph));
		(*pGraph)[e].vcost = m_pEdgeInfoVector->at(lEdgeIndex).costVector[lCostIndex];	
		(*pGraph)[e].id = m_pEdgeInfoVector->at(lEdgeIndex).edgeLabel;
	}
	return true;
}

bool ShortestPathBGL::Initialize(EdgeInfoVector* pEdgeInfoVector,VertexInfoVector* pVertexInfoVector, long lNumberOfLayers)
{
	m_pEdgeInfoVector =  pEdgeInfoVector;
	m_pVertexInfoVector = pVertexInfoVector;
	CreateGraph(lCurrentCostIndex);
	
	return true;
}


bool ShortestPathBGL::GetShortestPathVertex(long lSourceVertexLabel,long lDestinationVertexLabel, LongVector& vecVertexLabel, double& dCostOfRoute)
{
	std::vector<vertex_descriptor> predecessors(num_vertices((*pGraph)));

	vertex_descriptor srcVertex = vertex(lSourceVertexLabel, (*pGraph));

	if (srcVertex < 0)	
		return false;


	vertex_descriptor destVertex = vertex(lDestinationVertexLabel, (*pGraph));
	if (destVertex < 0)	
		return false;


	std::vector<double> distances(num_vertices((*pGraph)));
	dijkstra_shortest_paths((*pGraph), srcVertex,predecessor_map(&predecessors[0]).weight_map(get(&Vertex_def::vcost, (*pGraph))).distance_map(&distances[0]));


	vecVertexLabel.push_back(destVertex);
	
	while (destVertex != srcVertex) 
	{
		if (destVertex == predecessors[destVertex]) 		
			return false;		
		destVertex = predecessors[destVertex];		
		vecVertexLabel.push_back(destVertex);		
	}	
	
	std::reverse(vecVertexLabel.begin(),vecVertexLabel.end());	
	long lVIndex,lVCount = vecVertexLabel.size();
	LongVector vecEdgeLabel;
	for(lVIndex = lVCount - 1; lVIndex > 0; lVIndex--)
	{
		long lSrcV = vecVertexLabel[lVIndex];
		long lDestV = vecVertexLabel[lVIndex - 1];
		//AH:Error
		long lEdgeIndex = -1;//m_pVertexInfoVector->at(lSrcV).getEdgeLabelWithVertex(lDestV);
		vecEdgeLabel.push_back(lEdgeIndex);
	}


	double dTotalCost = 0.00000000;
	long lEdgeIndex,lEdgeCount = vecEdgeLabel.size();
	for(lEdgeIndex = 0; lEdgeIndex < lEdgeCount; lEdgeIndex++)
	{
		dTotalCost += m_pEdgeInfoVector->at(vecEdgeLabel[lEdgeIndex]).costVector[lCurrentCostIndex];
	}
	dCostOfRoute = dTotalCost;

	return true;
}

bool ShortestPathBGL::GetShortestPath(long lSourceVertexLabel,long lDestinationVertexLabel, LongVector& vecEdgeLabel, double& dCostOfRoute)
{
	
	std::vector<vertex_descriptor> predecessors(num_vertices((*pGraph)));

	vertex_descriptor srcVertex = vertex(lSourceVertexLabel, (*pGraph));

	if (srcVertex < 0)	
		return false;
	

	vertex_descriptor destVertex = vertex(lDestinationVertexLabel, (*pGraph));
	if (destVertex < 0)	
		return false;
	

	std::vector<double> distances(num_vertices((*pGraph)));

	dijkstra_shortest_paths((*pGraph), srcVertex,predecessor_map(&predecessors[0]).weight_map(get(&Vertex_def::vcost, (*pGraph))).distance_map(&distances[0]));

	std::vector<int> path_vect;
	path_vect.push_back(destVertex);
	while (destVertex != srcVertex) 
	{
		if (destVertex == predecessors[destVertex]) 		
			return false;		
		destVertex = predecessors[destVertex];		
		path_vect.push_back(destVertex);		
	}
	
	long lVIndex,lVCount = path_vect.size();
	for(lVIndex = lVCount - 1; lVIndex > 0; lVIndex--)
	{
		long lSrcV = path_vect[lVIndex];
		long lDestV = path_vect[lVIndex - 1];
		//AH:Error
		long lEdgeIndex = -1;//m_pVertexInfoVector->at(lSrcV).getEdgeLabelWithVertex(lDestV);
		vecEdgeLabel.push_back(lEdgeIndex);
	}


	double dTotalCost = 0.00000000;
	long lEdgeIndex,lEdgeCount = vecEdgeLabel.size();
	for(lEdgeIndex = 0; lEdgeIndex < lEdgeCount; lEdgeIndex++)
	{
		dTotalCost += m_pEdgeInfoVector->at(vecEdgeLabel[lEdgeIndex]).costVector[lCurrentCostIndex];
	}	
	dCostOfRoute = dTotalCost;
	
	return true;
}


bool ShortestPathBGL::GetShortestPath(LongVector& vecRoutingPointVertex, LongVector& vecEdgeIndex, double& dAccCost) 
{
	long lRoutingPointIndex,lRoutingPointCount = vecRoutingPointVertex.size();
	LongVector vecIndex;
	double dTempValue;
	dAccCost = 0.000000000;
	for(lRoutingPointIndex = 0; lRoutingPointIndex < lRoutingPointCount - 1; lRoutingPointIndex++)
	{
		if(!GetShortestPath(vecRoutingPointVertex[0],vecRoutingPointVertex[1],vecIndex,dTempValue))
			return false;	
		vecEdgeIndex.insert(vecEdgeIndex.end(),vecIndex.begin(),vecIndex.end());		
		dAccCost += dTempValue;
	}
	return true;
}


bool ShortestPathBGL::GetShortestPathVertex(LongVector& vecRoutingPointVertex, LongVector& vecVertexIndex, double& dAccCost)
{
	long lRoutingPointIndex,lRoutingPointCount = vecRoutingPointVertex.size();
	LongVector vecIndex;
	double dTempValue;
	dAccCost = 0.0000000000000;
	for(lRoutingPointIndex = 0; lRoutingPointIndex < lRoutingPointCount - 1; lRoutingPointIndex++)
	{
		if(!GetShortestPathVertex(vecRoutingPointVertex[0],vecRoutingPointVertex[1],vecIndex,dTempValue))
			return false;	
		vecVertexIndex.insert(vecVertexIndex.end(),vecIndex.begin(),vecIndex.end());		
		dAccCost += dTempValue;
	}
	return true;
}



//
//bool ShortestPathBGL::Initialize(EdgeInfoVector* pEdgeInfoVector,VertexInfoVector* pVertexInfoVector)
//{
//	long lEdgeCount = pEdgeInfoVector->size();
//	Edge *edges = new Edge[lEdgeCount];
//	double *weights = new double[lEdgeCount];
//	long lVertxCount = pVertexInfoVector->size();
//	Edge* edge_array = new Edge[lEdgeCount];
//
//
//	pGraph = new graph_t(lVertxCount);
//	long lEdgeIndex;
//
//	for(lEdgeIndex = 0 ; lEdgeIndex < lEdgeCount; lEdgeIndex++)
//	{		
//
//		edge_descriptor e; bool inserted;
//		tie(e, inserted) = add_edge(pEdgeInfoVector->at(lEdgeIndex).startNodeLabel, pEdgeInfoVector->at(lEdgeIndex).endNodeLabel, (*pGraph));
//		(*pGraph)[e].vcost = pEdgeInfoVector->at(lEdgeIndex).costVector[0];	
//		(*pGraph)[e].id = pEdgeInfoVector->at(lEdgeIndex).edgeLabel;
//
//
//
//		tie(e, inserted) = add_edge(pEdgeInfoVector->at(lEdgeIndex).endNodeLabel, pEdgeInfoVector->at(lEdgeIndex).startNodeLabel, (*pGraph));
//		(*pGraph)[e].vcost = pEdgeInfoVector->at(lEdgeIndex).costVector[0];	
//		(*pGraph)[e].id = pEdgeInfoVector->at(lEdgeIndex).edgeLabel;
//	}
//
//
//
//
//
//	std::vector<vertex_descriptor> predecessors(num_vertices((*pGraph)));
//
//	vertex_descriptor _source = vertex(0, (*pGraph));
//
//	if (_source < 0 /*|| _source >= num_nodes*/) 
//	{
//		//*err_msg = (char *) "Starting vertex not found";
//		return -1;
//	}
//
//	vertex_descriptor _target = vertex(100, (*pGraph));
//	if (_target < 0 /*|| _target >= num_nodes*/)
//	{
//		//*err_msg = (char *) "Ending vertex not found";
//		return -1;
//	}
//
//	std::vector<double> distances(num_vertices((*pGraph)));
//
//	dijkstra_shortest_paths((*pGraph), _source,predecessor_map(&predecessors[0]).weight_map(get(&Vertex_def::vcost, (*pGraph))).distance_map(&distances[0]));
//
//	std::vector<int> path_vect;
//	int max = 100000000;
//
//	while (_target != _source) 
//	{
//		if (_target == predecessors[_target]) 
//		{
//			//*err_msg = (char *) "No path found";
//			return 0;
//		}
//		_target = predecessors[_target];
//
//		printf("%d,",_target);
//		path_vect.push_back(_target);
//		if (!max--) 
//		{
//			//*err_msg = (char *) "Overflow";
//			return -1;
//		}
//	}
//	printf("\n");
//	return true;
//}







////////////////////////////////////////////////////////////////////////////////////
//	long lEdgeCount = pEdgeInfoVector->size();
//	Edge *edges = new Edge[lEdgeCount];
//	double *weights = new double[lEdgeCrount];
//	long lVertxCount = pVertexInfoVector->size();
//	Edge* edge_array = new Edge[lEdgeCount];
//	long lEdgeIndex;
//	for(lEdgeIndex = 0 ; lEdgeIndex < lEdgeCount; lEdgeIndex++)
//	{
//		edge_array[lEdgeIndex] = Edge(pEdgeInfoVector->at(lEdgeIndex).startNodeLabel, pEdgeInfoVector->at(lEdgeIndex).endNodeLabel);
//		weights[lEdgeIndex] = pEdgeInfoVector->at(lEdgeIndex).costVector[0];
//	}

//#if defined(BOOST_MSVC) && BOOST_MSVC <= 1300
//	pGraph = new graph_t(lVertxCount);
//	property_map<graph_t, edge_weight_t>::type weightmap = get(edge_weight, *pGraph);
//	for (std::size_t j = 0; j < lEdgeCount; ++j) 
//	{
//		edge_descriptor e; 
//		bool inserted;
//		tie(e, inserted) = add_edge(edge_array[j].first, edge_array[j].second, *pGraph);
//		weightmap[e] = weights[j];
//	}
//	
//	std::vector<vertex_descriptor> p(num_vertices(*pGraph));
//	std::vector<int> d(num_vertices(*pGraph));
//	vertex_descriptor s = vertex(0, *pGraph);
//
//	property_map<graph_t, vertex_index_t>::type indexmap = get(vertex_index, *pGraph);
//
//	//DijkstraVisitor dijkstra;
//	//vis.finish_vertex(u, g)
//	dijkstra_visitor<> vis;
//	vis.finish_vertex(10, *pGraph);
//	dijkstra_shortest_paths(*pGraph, s, &p[0], &d[0], weightmap, indexmap, 
//	std::less<int>(), closed_plus<int>(),(std::numeric_limits<int>::max)(), 0,default_dijkstra_visitor());
//
//	std::cout << "distances and parents:" << std::endl;
//	graph_traits < graph_t >::vertex_iterator vi, vend;
//	for (tie(vi, vend) = vertices(*pGraph); vi != vend; ++vi) 
//	{
//		std::cout << "distance(" << *vi << ") = " << d[*vi] << ", ";
//		std::cout << "parent(" << *vi << ") = " << p[*vi] << std::endl;
//	}
//
//int isww = 0;

//#else
//	graph_t g(edge_array, edge_array + num_arcs, weights, num_nodes);
//	property_map<graph_t, edge_weight_t>::type weightmap = get(edge_weight, g);
//#endif


//typedef std::pair<int, int> Edge;

//pGraph = new Graph(lEdgeCount,weights,lVertxCount);
//add_edge(0,1,*pGraph);


//////////////////////////////////////////////////////////////////////////////////



//bool ShortestPathBGL::edgeGraphTest(EdgeInfoVector* pEdgeInfoVector,VertexInfoVector* pVertexInfoVector)
//{
//	//	long lEdgeCount = pEdgeInfoVector->size();
//	//	Edge *edges = new Edge[lEdgeCount];
//	//	double *weights = new double[lEdgeCount];
//	//	long lVertxCount = pVertexInfoVector->size();
//	//	Edge* edge_array = new Edge[lEdgeCount];
//	//	long lEdgeIndex;
//	//	for(lEdgeIndex = 0 ; lEdgeIndex < lEdgeCount; lEdgeIndex++)
//	//	{
//	//		edge_array[lEdgeIndex] = Edge(pEdgeInfoVector->at(lEdgeIndex).startNodeLabel, pEdgeInfoVector->at(lEdgeIndex).endNodeLabel);
//	//		weights[lEdgeIndex] = pEdgeInfoVector->at(lEdgeIndex).cost;
//	//	}
//	////#if defined(BOOST_MSVC) && BOOST_MSVC <= 1300
//	//	pGraph = new graph_t(lVertxCount);
//	//	property_map<graph_t, edge_weight_t>::type weightmap = get(edge_weight, *pGraph);
//	//	for (std::size_t j = 0; j < lEdgeCount; ++j) 
//	//	{
//	//		edge_descriptor e; 
//	//		bool inserted;
//	//		tie(e, inserted) = add_edge(edge_array[j].first, edge_array[j].second, *pGraph);
//	//		weightmap[e] = weights[j];
//	//
//	//		tie(e, inserted) = add_edge(edge_array[j].second, edge_array[j].first, *pGraph);
//	//		weightmap[e] = weights[j];
//	//	}
//	//	
//	//	std::vector<vertex_descriptor> p(num_vertices(*pGraph));
//	//	std::vector<int> d(num_vertices(*pGraph));
//	//	vertex_descriptor s = vertex(0, *pGraph);
//	//
//	//	property_map<graph_t, vertex_index_t>::type indexmap = get(vertex_index, *pGraph);
//	//
//	//	//DijkstraVisitor dijkstra;
//	//	//vis.finish_vertex(u, g)
//	//	dijkstra_visitor<> vis;
//	//
//	//	int _target = 555;
//	//	vis.finish_vertex(_target, *pGraph);
//	//	dijkstra_shortest_paths(*pGraph, s, &p[0], &d[0], weightmap, indexmap,std::less<int>(), closed_plus<int>(),(std::numeric_limits<int>::max)(), 0,default_dijkstra_visitor());
//	//	
//	//	std::cout << "distances and parents:" << std::endl;
//	//	std::vector<int> path_vect;
//	//
//	//	while (_target != s) 
//	//	{
//	//		if (_target == p[_target]) 
//	//		{
//	//			//*err_msg = (char *) "No path found";
//	//			return 0;
//	//		}
//	//		_target = p[_target];
//	//
//	//		printf("%d,",_target);
//	//		path_vect.push_back(_target);
//	//		
//	//	}
//	//
//	//
//	//
//	//	std::vector<int> path_Edge;
//	//	long lVIndex,lVCount = path_vect.size();
//	//	for(lVIndex = lVCount - 1; lVIndex > 0; lVIndex--)
//	//	{
//	//		long lSrcV = path_vect[lVIndex];
//	//		long lDestV = path_vect[lVIndex - 1];
//	//		long lEdgeIndex = pVertexInfoVector->at(lSrcV).getEdgeLabelWithVertex(lDestV);
//	//		path_Edge.push_back(lEdgeIndex);
//	//	}
//	//
//	//int isww = 0;
//	//
//	//
//	////#else
//	////	graph_t g(edge_array, edge_array + num_arcs, weights, num_nodes);
//	////	property_map<graph_t, edge_weight_t>::type weightmap = get(edge_weight, g);
//	////#endif
//
//
//	return true;
//}