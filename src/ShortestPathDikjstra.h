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
#ifndef SHORTESTPATHDIKJSTRA_H
#define SHORTESTPATHDIKJSTRA_H

#include "utils.h"
#include "GraphDefinition.h"
#include "ShortestPathGenerator.h"
#include "PointShapeCreator.h"


class ShortestPathDikjstra : public ShortestPathGenerator
{
public:
	ShortestPathDikjstra(void);
	~ShortestPathDikjstra(void);

public:
	bool Initialize(EdgeInfoVector* pEdgeInfoVector,VertexInfoVector* pVertexInfoVector, long lNumberOfLayers);
	bool GetShortestPath(LongVector& vecRoutingPointVertex, LongVector& vecEdgeIndex, double& dAccCost);
	bool GetShortestPathVertex(LongVector& vecRoutingPointVertex, LongVector& vecVertexIndex, double& dAccCost);
	bool GetShortestPath(RoutingPointInfoVector& vecRoutingPointsInfo, OGRPointVector& vecRoutingPoints, double& dAccCost);
	bool GetAccShortestPath(RoutingPointInfo& startPoint, RoutingPointInfo& endPoint, OGRPointVector& vecRoutingPoints, double& dAccCost);
	bool GetAccShortestPath(RoutingPointInfo& startPoint, RoutingPointInfo& endPoint, OGRPointVector& vecStartingPartialPart,OGRPointVector& vecEndingPartialPart,LongVector& vecFeatureIndex ,double& dAccCost);
	bool GetAccShortestPathPublic(RoutingPointInfo& startPoint, RoutingPointInfo& endPoint,long lStartingTime, OGRPointVector& vecStartingPartialPart,OGRPointVector& vecEndingPartialPart,LongVector& vecFeatureIndex ,double& dAccCost);


	

private:
	bool GetShortestPath(long lStartNodeIndex, long lDestinationNodeIndex,LongVector& vecRouteEdge, double& dCostOfRoute);
	bool GetShortestPathVertex(long lStartNodeIndex, long lDestinationNodeIndex,LongVector& vecVertexLabel, double& dCostOfRoute);

	//bool GetPartialPartPoints(RawPointVector& vecFeaturePoints,long lStartIndex, long lEndIndex, bool bIsForward, OGRPoint& intersectionPoint,OGRPointVector& vecPartialPartPoints);


private:	
	long* plPrev;
	long* plPrevEdge;
	bool* bIsVertexVisited;
	double* dVertexCost;

	EdgeInfoVector* m_pEdgeInfoVector;
	VertexInfoVector* m_pVertexInfoVector;
	CPointShapeCreator m_pointExplored;

};

#endif
