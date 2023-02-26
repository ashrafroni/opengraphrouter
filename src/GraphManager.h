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
#ifndef GRAPHMANAGER_H
#define GRAPHMANAGER_H

#include "ogr_geometry.h"
#include "ogrsf_frmts.h"
#include "utils.h"
#include "GraphDefinition.h"
#include "rtree.hpp"
#include <iostream>
#include <queue>
#include <functional>
#include "ShortestPathBGL.h"
#include "ShortestPathDikjstra.h"
#include "ShortestPathAStar.h"
#include "BiDirectionalAStar.h"




typedef geometry::point_xy<double> point_type;
typedef unsigned int value_type;



class GraphManager
{
public:
	GraphManager(void);
	~GraphManager(void);

private:
	ArcNodeInfo* pArcNodeInfo;
	long featureCount;	
	EdgeInfoVector edgeInfoVector;
	VertexInfoVector vertexInfoVector;
	StringLongPairVector costFieldNameAndIndexVector;
	StringLongPairVector vecCostFields;

	bool* bIsVertexVisited;
	OGRDataSource  *poDS;
	std::string strLayerName;

	long* plPrev;
	long* plPrevEdge;
	ShortestPathBGL bgl;
	ShortestPathDikjstra defDjk;
	CShortestPathAStar astar;
	CBiDirectionalAStar biDirectional;

	StringVector m_vecLayerNames;
	LongVector m_vecLayerFeatureCount;
	LayerInfoVector m_vecLayerInfo;

	boost::spatial_index::spatial_index<point_type, value_type,boost::spatial_index::rtree<point_type, value_type> > rTreeQ;

public:

	void InitGraphManager();	
	bool ReadDataSource(LayerInfoVector& vecLayerInfo);		
	void WriteToFile(std::string strFilePath);

	bool SeparateComponent();
	bool AnalyzeNetworks();
	bool SaveGraph();

	
	StringVector getLayerNameVec();
	bool ReadDataSourceAndConvert();
	bool GetShortestPath(OGRPointVector& vecRoutingViaPoints, LongVector& vecEdgeIndex,double& dAccCost);
	bool GetShortestPathVertex(OGRPointVector& vecRoutingViaPoints, LongVector& vecVertexIndex,double& dAccCost);
	bool GetShortestPath(OGRPointVector& vecRoutingViaPoints, OGRPointVector& vecRoutingPoints,double& dAccCost);

	
	bool GetAccourateShortestPath(OGRPointVector& vecRoutingViaPoints, OGRPointVector& vecRoutingPoints,double& dAccCost);
	bool GetShortestPathAStar(OGRPointVector& vecRoutingViaPoints, OGRPointVector& vecRoutingPoints,double& dAccCost);
	bool GetAccourateShortestPathUsingPublic(OGRPointVector& vecRoutingViaPoints,long lStartingTime, OGRPointVector& vecRoutingPoints,double& dAccCost);


	bool GetShortestPathBGL(OGRPointVector& vecRoutingViaPoints, LongVector& vecEdgeIndex,double& dAccCost);
	bool GetShortestPathBGLVertex(OGRPointVector& vecRoutingViaPoints, LongVector& vecVertexIndex,double& dAccCost);
	bool GetShortestPathBGL(OGRPointVector& vecRoutingViaPoints, OGRPointVector& vecRoutingPoints,double& dAccCost);


	bool GetBiDirectionalShortestPathAStar(OGRPointVector& vecRoutingViaPoints, OGRPointVector& vecRoutingPoints,double& dAccCost);
	
	EdgeInfoVector* GetEdgeVecPtr();
	VertexInfoVector* GetVertexVecPtr();

private:
	bool RunBFS(long lStartIndex, long graphID);
	bool GetClosestVertexIndex(OGRPoint pointSelected,long &vertexIndex,long &lEdgeIndex,long& lGraphIndex,double dSearchRadius = 2.0);
	bool GetClosestVertexIndex(OGRPoint pointSelected,RoutingPointInfo& routingPointInfo,long& lGraphIndex,double dSearchRadius = 2.0);
	bool GetClosestEdgeIndex(OGRPoint pointSelected,RoutingPointInfo& routingPointInfo,long& lGraphIndex,double dSearchRadius = 2.0);

	bool GetFaturePoints(LongVector vecEdgeIndex, OGRPointVector& vecRoutingPoints);
	bool GetFaturePoints(LongVector vecEdgeIndex,OGRPointVector& vecStartingPoints,OGRPointVector& vecEndPoints, OGRPointVector& vecRoutingPoints);
	bool AddWithLines(OGRPointVector& vecLinePoints,OGRPointVector& vecLinePointsToAdd);
	

	bool BuildRTree();

	long GetOptimizationCostIndex(std::string strMinimizingParam);
	
};

#endif
