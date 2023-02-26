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
#include "ShortestPathDikjstra.h"
#include <boost/pending/mutable_queue.hpp>
#include "GeometryDefinition.h"


class Prioritize 
{
public:
	int operator() ( const Vertex_def& p1,const Vertex_def& p2 ) 
	{
		return p1.vcost> p2.vcost;
	}
};

class PrioritizePublic
{
public:
	int operator() ( const Vertex_def_public& p1,const const Vertex_def_public& p2 ) 
	{
		return p1.vcost> p2.vcost;
	}
};



ShortestPathDikjstra::ShortestPathDikjstra(void)
{

}

ShortestPathDikjstra::~ShortestPathDikjstra(void)
{

}


bool ShortestPathDikjstra::Initialize(EdgeInfoVector* pEdgeInfoVector,VertexInfoVector* pVertexInfoVector, long lNumberOfLayers)
{
	m_pEdgeInfoVector = pEdgeInfoVector;
	m_pVertexInfoVector = pVertexInfoVector;

	long nVertexCount = m_pVertexInfoVector->size();
	plPrev = new long[nVertexCount + 1];
	plPrevEdge = new long[nVertexCount + 1];
	bIsVertexVisited = new bool[nVertexCount + 1];
	dVertexCost = new double[nVertexCount + 1];
	return true;
}


bool ShortestPathDikjstra::GetShortestPath(LongVector& vecRoutingPointVertex, LongVector& vecEdgeIndex,double& dAccCost)
{
	long lRoutingPointIndex,lRoutingPointCount = vecRoutingPointVertex.size();
	LongVector vecIndex;
	dAccCost = 0.0000000000;
	double dTempCost;
	for(lRoutingPointIndex = 0; lRoutingPointIndex < lRoutingPointCount - 1; lRoutingPointIndex++)
	{
		if(!GetShortestPath(vecRoutingPointVertex[0],vecRoutingPointVertex[1],vecIndex,dTempCost))
			return false;	
		vecEdgeIndex.insert(vecEdgeIndex.end(),vecIndex.begin(),vecIndex.end());	
		dAccCost += dTempCost;
	}	
	return true;
}

bool ShortestPathDikjstra::GetShortestPathVertex(LongVector& vecRoutingPointVertex, LongVector& vecVertexIndex, double& dAccCost)
{
	long lRoutingPointIndex,lRoutingPointCount = vecRoutingPointVertex.size();
	LongVector vecIndex;
	dAccCost = 0.0000000;
	double dTempCost;
	for(lRoutingPointIndex = 0; lRoutingPointIndex < lRoutingPointCount - 1; lRoutingPointIndex++)
	{
		if(!GetShortestPathVertex(vecRoutingPointVertex[0],vecRoutingPointVertex[1],vecIndex,dTempCost))
			return false;	
		vecVertexIndex.insert(vecVertexIndex.end(),vecIndex.begin(),vecIndex.end());		
		dAccCost += dTempCost;
	}	
	return true;
}




bool ShortestPathDikjstra::GetShortestPathVertex(long lStartNodeIndex, long lDestinationNodeIndex,LongVector& vecVertexLabel, double& dCostOfRoute)
{
	long nVertexCount = m_pVertexInfoVector->size();

	long lShortestPathOpParamIndex = 1;
	std::priority_queue<Vertex_def,std::vector <Vertex_def>, Prioritize >  pQueue;
	long lVertexIndex;
	for(lVertexIndex = 0; lVertexIndex < nVertexCount; lVertexIndex++)
	{
		dVertexCost[lVertexIndex] = DBL_MAX;
		plPrev[lVertexIndex] = -1;
		plPrevEdge[lVertexIndex] = -1;
		bIsVertexVisited[lVertexIndex] = false;
	}
	dVertexCost[lStartNodeIndex] = 0;
	plPrev[lStartNodeIndex]	= -1;	
	
	Vertex_def vDef;
	vDef.id = lStartNodeIndex;
	vDef.vcost = 0.000000;
	bIsVertexVisited[lStartNodeIndex] = true;
	pQueue.push(vDef);
	Vertex_def vTopVertex;

	while(!pQueue.empty())
	{
		
		vTopVertex = pQueue.top();
		
		pQueue.pop();
		double dPrevCost = vTopVertex.vcost;

		if( vTopVertex.id == lDestinationNodeIndex)
			break;

		long lConnectedEdgeCount = m_pVertexInfoVector->at(vTopVertex.id).connectedEdgeLabel.size();
		long lEdgeIndex;
		for(lEdgeIndex = 0; lEdgeIndex < lConnectedEdgeCount; lEdgeIndex++)
		{
			long lEdgeLabel = m_pVertexInfoVector->at(vTopVertex.id).connectedEdgeLabel[lEdgeIndex];
			double dCost = m_pEdgeInfoVector->at(lEdgeLabel).costVector[0];
			long lNeighbourVertexLabel = m_pVertexInfoVector->at(vTopVertex.id).connectedVertexLabel[lEdgeIndex];

			if(m_pVertexInfoVector->at(vTopVertex.id).vecDirection[lEdgeIndex] == ForwardDirection || 
			   m_pVertexInfoVector->at(vTopVertex.id).vecDirection[lEdgeIndex] == BothDirection )
			{
				
				double dCurrentCost = dPrevCost + dCost;
				bool bIsVisited = bIsVertexVisited[lNeighbourVertexLabel];

				if(dVertexCost[lNeighbourVertexLabel] > dCurrentCost)
				{
					plPrev[lNeighbourVertexLabel] = vTopVertex.id;
					plPrevEdge[lNeighbourVertexLabel] = lEdgeLabel;
					dVertexCost[lNeighbourVertexLabel] = dCurrentCost;
					bIsVertexVisited[lNeighbourVertexLabel] = true;
					Vertex_def vNewNode;
					vNewNode.id = lNeighbourVertexLabel;
					vNewNode.vcost = dCurrentCost;

					if(!bIsVisited)
						pQueue.push(vNewNode);
				}
			}
		}
	}

	double dTotalCost = vTopVertex.vcost;
	
	long lParentNodeIndex = lDestinationNodeIndex;
	for(;;)
	{
		if(plPrev[lParentNodeIndex] == -1)
			break;

		vecVertexLabel.push_back(lParentNodeIndex);
		lParentNodeIndex = plPrev[lParentNodeIndex];
	}
	vecVertexLabel.push_back(lStartNodeIndex);	
	dCostOfRoute = dTotalCost;
	std::reverse(vecVertexLabel.begin(),vecVertexLabel.end());	
	return true;
}





bool ShortestPathDikjstra::GetAccShortestPathPublic(RoutingPointInfo& startPoint, RoutingPointInfo& endPoint,long lStartingTime, OGRPointVector& vecStartingPartialPart,OGRPointVector& vecEndingPartialPart,LongVector& vecFeatureIndex ,double& dAccCost)
{
	long nVertexCount = m_pVertexInfoVector->size();
	long lShortestPathOpParamIndex = 1;
	long lExploredNodeCount = 0;

	std::priority_queue<Vertex_def,std::vector <Vertex_def_public>, PrioritizePublic >  pQueue;
	Vertex_def_public vTopVertex;
	long lVertexIndex;
	for(lVertexIndex = 0; lVertexIndex < nVertexCount; lVertexIndex++)
	{
		dVertexCost[lVertexIndex] = DBL_MAX;
		plPrev[lVertexIndex] = -1;
		plPrevEdge[lVertexIndex] = -1;
		bIsVertexVisited[lVertexIndex] = false;
	}


	Vertex_def_public vDef;

	// s---------------m--------------->e
	// So the value of edge from middle to end have to be added
	if(m_pEdgeInfoVector->at(startPoint.lFeatureIndex).direction == ForwardDirection)
	{
		long lNodeLabel = m_pEdgeInfoVector->at(startPoint.lFeatureIndex).endNodeLabel;
		double dCost = m_pEdgeInfoVector->at(startPoint.lFeatureIndex).costVector[1];
		double dFeatureLength = m_pEdgeInfoVector->at(startPoint.lFeatureIndex).costVector[0];
		double dPartialLength = (dFeatureLength  * (100 - startPoint.dPartialValuePercent))/100;
		double dTime = ceil(dPartialLength * 3);

		dVertexCost[lNodeLabel] = (dCost * (100 - startPoint.dPartialValuePercent))/100;
		plPrev[lNodeLabel]	= -1;	

		vDef.id = lNodeLabel;
		vDef.vcost = dVertexCost[lNodeLabel];
		vDef.lCurrentTime = lStartingTime + dTime;
		vDef.lCurLayerIndex = 0;

		pQueue.push(vDef);
		bIsVertexVisited[lNodeLabel] = true;
		lExploredNodeCount++; 

	}
	// s<---------------m---------------e
	// So the value of edge from middle to start have to be added
	else if(m_pEdgeInfoVector->at(startPoint.lFeatureIndex).direction == ReverseDirection)
	{
		long lNodeLabel = m_pEdgeInfoVector->at(startPoint.lFeatureIndex).startNodeLabel;		
		double dCost = m_pEdgeInfoVector->at(startPoint.lFeatureIndex).costVector[1];

		double dFeatureLength = m_pEdgeInfoVector->at(startPoint.lFeatureIndex).costVector[0];
		double dPartialLength = dFeatureLength  * startPoint.dPartialValuePercent/100;
		double dTime = ceil(dPartialLength * 3);

		dVertexCost[lNodeLabel] = dCost * startPoint.dPartialValuePercent/100;
		plPrev[lNodeLabel]	= -1;

		vDef.id = lNodeLabel;
		vDef.vcost = dVertexCost[lNodeLabel];
		vDef.lCurrentTime = lStartingTime + dTime;
		vDef.lCurLayerIndex = 0;

		pQueue.push(vDef);
		bIsVertexVisited[lNodeLabel] = true;
		lExploredNodeCount++;

	}
	// s<---------------m--------------->e
	// So the value of edge from middle to start and middle to end have to be added
	else
	{

		double dCost = m_pEdgeInfoVector->at(startPoint.lFeatureIndex).costVector[1];
		double dFeatureLength = m_pEdgeInfoVector->at(startPoint.lFeatureIndex).costVector[0];

		double dPartialLength1 = dFeatureLength  * startPoint.dPartialValuePercent/100;
		double dTime1 = ceil(dPartialLength1 * 3);

		long lStartNode = m_pEdgeInfoVector->at(startPoint.lFeatureIndex).startNodeLabel;		
		dVertexCost[lStartNode] = (dCost * startPoint.dPartialValuePercent)/100;
		plPrev[lStartNode]	= -1;

		vDef.id = lStartNode;
		vDef.vcost = dVertexCost[lStartNode];
		vDef.lCurrentTime = lStartingTime + dTime1;
		vDef.lCurLayerIndex = 0;
		pQueue.push(vDef);
		bIsVertexVisited[lStartNode] = true;


		long lEndNode = m_pEdgeInfoVector->at(startPoint.lFeatureIndex).endNodeLabel;
		dVertexCost[lEndNode] = (dCost * (100 - startPoint.dPartialValuePercent))/100;
		double dPartialLength2 = (dFeatureLength  * (100 - startPoint.dPartialValuePercent))/100; 
		double dTime2 = ceil(dPartialLength2 * 3);

		plPrev[lEndNode]	= -1;	

		vDef.id = lEndNode;
		vDef.vcost = dVertexCost[lEndNode];
		vDef.lCurrentTime = lStartingTime + dTime2;
		vDef.lCurLayerIndex = 0;

		pQueue.push(vDef);
		bIsVertexVisited[lEndNode] = true;

		lExploredNodeCount++;
		lExploredNodeCount++;

	}

	LongVector vecDestinationNode;
	// s---------------m--------------->e
	//Like the upper case
	// If the destination is mid point than the destination node will be only starting node
	if(m_pEdgeInfoVector->at(endPoint.lFeatureIndex).direction == ForwardDirection)
	{
		vecDestinationNode.push_back(m_pEdgeInfoVector->at(endPoint.lFeatureIndex).startNodeLabel);
	}
	// s<--------------m---------------e
	// Like the upper case
	// If the destination is mid point than the destination node will be only ending node
	else if(m_pEdgeInfoVector->at(endPoint.lFeatureIndex).direction == ReverseDirection)
	{
		vecDestinationNode.push_back(m_pEdgeInfoVector->at(endPoint.lFeatureIndex).endNodeLabel);
	}
	// s<--------------m--------------->e
	// Like the upper case
	// If the destination is mid point than the destination node may both way + the distance from the end points.
	else
	{
		vecDestinationNode.push_back(m_pEdgeInfoVector->at(endPoint.lFeatureIndex).startNodeLabel);
		vecDestinationNode.push_back(m_pEdgeInfoVector->at(endPoint.lFeatureIndex).endNodeLabel);
	}



	long lDestnationCount = vecDestinationNode.size();

	while(!pQueue.empty())	
	{

		vTopVertex = pQueue.top();
		pQueue.pop();
		double dPrevCost = vTopVertex.vcost;

		LongVector::iterator itVec = std::find(vecDestinationNode.begin(),vecDestinationNode.end(),vTopVertex.id);
		if(itVec != vecDestinationNode.end())
		{
			lDestnationCount--;
			vecDestinationNode.erase(itVec);
			if(lDestnationCount == 0)
				break;
		}


		long lConnectedEdgeCount = m_pVertexInfoVector->at(vTopVertex.id).connectedEdgeLabel.size();
		long lStartEdgeIndex,lEndEdgeIndex;
		lStartEdgeIndex = 0;
		lEndEdgeIndex = lConnectedEdgeCount;


		VertexInfo* pVertexInfo = &m_pVertexInfoVector->at(vTopVertex.id);
		if(pVertexInfo->bIsTransitPoint)
		{
//			TimeLayerIndex timeLayerIndex;
//			//if(vTopVertex.lCurLayerIndex > 0)
//
//			if(pVertexInfo->timeScheduleHandler.getClosestSchedule(timeLayerIndex,vTopVertex.lCurrentTime))
//			{
//				if(timeLayerIndex.lLayerIndex > 0)
//				{
//					lStartEdgeIndex = pVertexInfo->vecLayerIndex[timeLayerIndex.lLayerIndex - 1];
//					lEndEdgeIndex = pVertexInfo->vecLayerIndex[timeLayerIndex.lLayerIndex];
//				}
//				else
//				{
//					lStartEdgeIndex = 0;
//					lEndEdgeIndex = pVertexInfo->vecLayerIndex[0];
//				}				
//			}
		}

		

		long lEdgeIndex;
		for(lEdgeIndex = lStartEdgeIndex; lEdgeIndex < lEndEdgeIndex; lEdgeIndex++)
		{
			long lEdgeLabel = m_pVertexInfoVector->at(vTopVertex.id).connectedEdgeLabel[lEdgeIndex];
			double dCost = m_pEdgeInfoVector->at(lEdgeLabel).costVector[1];
			double dLength = m_pEdgeInfoVector->at(lEdgeLabel).costVector[0];
			double dTime = ceil(dLength * 3);
			long lNeighbourVertexLabel = m_pVertexInfoVector->at(vTopVertex.id).connectedVertexLabel[lEdgeIndex];

			if(m_pVertexInfoVector->at(vTopVertex.id).vecDirection[lEdgeIndex] == ForwardDirection || 
				m_pVertexInfoVector->at(vTopVertex.id).vecDirection[lEdgeIndex] == BothDirection )
			{

				double dCurrentCost = dPrevCost + dCost;
				bool bIsVisited = bIsVertexVisited[lNeighbourVertexLabel];

				if(dVertexCost[lNeighbourVertexLabel] > dCurrentCost)
				{
					plPrev[lNeighbourVertexLabel] = vTopVertex.id;
					plPrevEdge[lNeighbourVertexLabel] = lEdgeLabel;
					dVertexCost[lNeighbourVertexLabel] = dCurrentCost;
					bIsVertexVisited[lNeighbourVertexLabel] = true;
					Vertex_def_public vNewNode;
					vNewNode.id = lNeighbourVertexLabel;
					vNewNode.vcost = dCurrentCost;
					vNewNode.lCurrentTime = vTopVertex.lCurrentTime + dTime;
					if(!bIsVisited)
					{
						pQueue.push(vNewNode);
						lExploredNodeCount++;
					}
				}
			}
		}
	}

	double dDestCost = -1;
	long lDestinationNodeIndex;
	//
	// s--------------m--------------->e
	if(m_pEdgeInfoVector->at(endPoint.lFeatureIndex).direction == ForwardDirection)
	{
		long lDStartNodeLabel = m_pEdgeInfoVector->at(endPoint.lFeatureIndex).startNodeLabel;
		double dDestCost = m_pEdgeInfoVector->at(endPoint.lFeatureIndex).costVector[1];				
		double dPartialCost = (dDestCost * startPoint.dPartialValuePercent)/100;
		double dTotalCost = dVertexCost[lDStartNodeLabel] + dPartialCost;
		dDestCost = dTotalCost;
		lDestinationNodeIndex = lDStartNodeLabel;
	}
	// s<--------------m---------------e
	// Like the upper case

	else if(m_pEdgeInfoVector->at(endPoint.lFeatureIndex).direction == ReverseDirection)
	{
		long lDEndNodeLabel = m_pEdgeInfoVector->at(endPoint.lFeatureIndex).endNodeLabel;
		double dDestCost = m_pEdgeInfoVector->at(endPoint.lFeatureIndex).costVector[1];				
		double dPartialCost = (dDestCost * (100 - startPoint.dPartialValuePercent))/100;
		double dTotalCost = dVertexCost[lDEndNodeLabel] + dPartialCost;
		dDestCost = dTotalCost;
		lDestinationNodeIndex = lDEndNodeLabel;
	}
	// s<--------------m--------------->e
	// Like the upper case	
	else
	{
		long lDStartNodeLabel = m_pEdgeInfoVector->at(endPoint.lFeatureIndex).startNodeLabel;
		double dDestCost = m_pEdgeInfoVector->at(endPoint.lFeatureIndex).costVector[1];				
		double dStartPartialCost = (dDestCost * endPoint.dPartialValuePercent)/100;
		double dStartTotalCost = dVertexCost[lDStartNodeLabel] + dStartPartialCost;

		long lDEndNodeLabel = m_pEdgeInfoVector->at(endPoint.lFeatureIndex).endNodeLabel;		
		double dEndPartialCost = (dDestCost * (100 - endPoint.dPartialValuePercent))/100;
		double dEndTotalCost = dVertexCost[lDEndNodeLabel] + dEndPartialCost;
		if(dEndTotalCost < dStartTotalCost)
		{
			dDestCost = dEndTotalCost;
			lDestinationNodeIndex = lDEndNodeLabel;
			GeoMatryDefs::GetPartialPartPoints(m_pEdgeInfoVector->at(endPoint.lFeatureIndex).vecFeaturePoints,endPoint.lStartingPointIndex + 1,m_pEdgeInfoVector->at(endPoint.lFeatureIndex).vecFeaturePoints.size() - 1,false,endPoint.intersectionPoint,vecEndingPartialPart);
		}
		else 
		{
			dDestCost = dStartTotalCost;
			lDestinationNodeIndex = lDStartNodeLabel;
			GeoMatryDefs::GetPartialPartPoints(m_pEdgeInfoVector->at(endPoint.lFeatureIndex).vecFeaturePoints,0,endPoint.lStartingPointIndex,true,endPoint.intersectionPoint,vecEndingPartialPart);
		}

	}	

	double dTotalCost = vTopVertex.vcost;

	long lParentNodeIndex = lDestinationNodeIndex;
	long lStartingNodeLabel;
	for(;;)
	{
		if(plPrev[lParentNodeIndex] == -1)
		{
			break;
		}
		vecFeatureIndex.push_back(plPrevEdge[lParentNodeIndex]);		
		lParentNodeIndex = plPrev[lParentNodeIndex];
	}
	std::reverse(vecFeatureIndex.begin(),vecFeatureIndex.end());	
	lStartingNodeLabel = lParentNodeIndex;
	if(lStartingNodeLabel == m_pEdgeInfoVector->at(startPoint.lFeatureIndex).startNodeLabel)
	{
		GeoMatryDefs::GetPartialPartPoints(m_pEdgeInfoVector->at(startPoint.lFeatureIndex).vecFeaturePoints,0,startPoint.lStartingPointIndex,true,startPoint.intersectionPoint,vecStartingPartialPart);
	}
	else if(lStartingNodeLabel == m_pEdgeInfoVector->at(startPoint.lFeatureIndex).endNodeLabel)
	{
		GeoMatryDefs::GetPartialPartPoints(m_pEdgeInfoVector->at(startPoint.lFeatureIndex).vecFeaturePoints,startPoint.lStartingPointIndex + 1,m_pEdgeInfoVector->at(startPoint.lFeatureIndex).vecFeaturePoints.size() - 1,false,startPoint.intersectionPoint,vecStartingPartialPart);
	}

	printf("Dijkstra:[%ld]\n",lExploredNodeCount);

	return true;
}

bool ShortestPathDikjstra::GetAccShortestPath(RoutingPointInfo& startPoint, RoutingPointInfo& endPoint, OGRPointVector& vecStartingPartialPart,OGRPointVector& vecEndingPartialPart,LongVector& vecFeatureIndex ,double& dAccCost)
{
	long nVertexCount = m_pVertexInfoVector->size();
	long lExploredNodeCount = 0;

	long lShortestPathOpParamIndex = 1;

//	m_pointExplored.createShape("D:\\output\\dijkstraNodeExplore");
//	m_pointExplored.addField("ID");

	std::priority_queue<Vertex_def,std::vector <Vertex_def>, Prioritize >  pQueue;
	Vertex_def vTopVertex;
	long lVertexIndex;
	for(lVertexIndex = 0; lVertexIndex < nVertexCount; lVertexIndex++)
	{
		dVertexCost[lVertexIndex] = DBL_MAX;
		plPrev[lVertexIndex] = -1;
		plPrevEdge[lVertexIndex] = -1;
		bIsVertexVisited[lVertexIndex] = false;
	}


	Vertex_def vDef;

	// s---------------m--------------->e
	// So the value of edge from middle to end have to be added
	if(m_pEdgeInfoVector->at(startPoint.lFeatureIndex).direction == ForwardDirection)
	{
		long lNodeLabel = m_pEdgeInfoVector->at(startPoint.lFeatureIndex).endNodeLabel;
		double dCost = m_pEdgeInfoVector->at(startPoint.lFeatureIndex).costVector[0];
		dVertexCost[lNodeLabel] = (dCost * (100 - startPoint.dPartialValuePercent))/100;
		plPrev[lNodeLabel]	= -1;	

		

		vDef.id = lNodeLabel;
		vDef.vcost = dVertexCost[lNodeLabel];
		pQueue.push(vDef);
		bIsVertexVisited[lNodeLabel] = true;


		//Test 
		//m_pointExplored.insertShape(m_pVertexInfoVector->at(lNodeLabel).vertexPoint,lExploredNodeCount);
		//	

		lExploredNodeCount++;

	}
	// s<---------------m---------------e
	// So the value of edge from middle to start have to be added
	else if(m_pEdgeInfoVector->at(startPoint.lFeatureIndex).direction == ReverseDirection)
	{
		long lNodeLabel = m_pEdgeInfoVector->at(startPoint.lFeatureIndex).startNodeLabel;		
		double dCost = m_pEdgeInfoVector->at(startPoint.lFeatureIndex).costVector[0];
		dVertexCost[lNodeLabel] = dCost * startPoint.dPartialValuePercent/100;
		plPrev[lNodeLabel]	= -1;

		vDef.id = lNodeLabel;
		vDef.vcost = dVertexCost[lNodeLabel];
		pQueue.push(vDef);
		bIsVertexVisited[lNodeLabel] = true;
		//Test
		//m_pointExplored.insertShape(m_pVertexInfoVector->at(lNodeLabel).vertexPoint,lExploredNodeCount);
		//
		lExploredNodeCount++;

	}
	// s<---------------m--------------->e
	// So the value of edge from middle to start and middle to end have to be added
	else
	{

		double dCost = m_pEdgeInfoVector->at(startPoint.lFeatureIndex).costVector[0];

		long lStartNode = m_pEdgeInfoVector->at(startPoint.lFeatureIndex).startNodeLabel;		
		dVertexCost[lStartNode] = (dCost * startPoint.dPartialValuePercent)/100;
		plPrev[lStartNode]	= -1;

		vDef.id = lStartNode;
		vDef.vcost = dVertexCost[lStartNode];
		pQueue.push(vDef);
		bIsVertexVisited[lStartNode] = true;


		long lEndNode = m_pEdgeInfoVector->at(startPoint.lFeatureIndex).endNodeLabel;
		dVertexCost[lEndNode] = (dCost * (100 - startPoint.dPartialValuePercent))/100;
		plPrev[lEndNode]	= -1;	

		vDef.id = lEndNode;
		vDef.vcost = dVertexCost[lEndNode];
		pQueue.push(vDef);
		bIsVertexVisited[lEndNode] = true;
		lExploredNodeCount++;
		
		//Test
		//m_pointExplored.insertShape(m_pVertexInfoVector->at(lStartNode).vertexPoint,lExploredNodeCount);
		//

		lExploredNodeCount++;
		//Test
		//m_pointExplored.insertShape(m_pVertexInfoVector->at(lEndNode).vertexPoint,lExploredNodeCount);
		//

		



	}

	LongVector vecDestinationNode;
	// s---------------m--------------->e
	//Like the upper case
	// If the destination is mid point than the destination node will be only starting node
	if(m_pEdgeInfoVector->at(endPoint.lFeatureIndex).direction == ForwardDirection)
	{
		vecDestinationNode.push_back(m_pEdgeInfoVector->at(endPoint.lFeatureIndex).startNodeLabel);
	}
	// s<--------------m---------------e
	// Like the upper case
	// If the destination is mid point than the destination node will be only ending node
	else if(m_pEdgeInfoVector->at(endPoint.lFeatureIndex).direction == ReverseDirection)
	{
		vecDestinationNode.push_back(m_pEdgeInfoVector->at(endPoint.lFeatureIndex).endNodeLabel);
	}
	// s<--------------m--------------->e
	// Like the upper case
	// If the destination is mid point than the destination node may both way + the distance from the end points.
	else
	{
		vecDestinationNode.push_back(m_pEdgeInfoVector->at(endPoint.lFeatureIndex).startNodeLabel);
		vecDestinationNode.push_back(m_pEdgeInfoVector->at(endPoint.lFeatureIndex).endNodeLabel);
	}



	long lDestnationCount = vecDestinationNode.size();

	while(!pQueue.empty())	
	{

		vTopVertex = pQueue.top();
		pQueue.pop();
		double dPrevCost = vTopVertex.vcost;

		LongVector::iterator itVec = std::find(vecDestinationNode.begin(),vecDestinationNode.end(),vTopVertex.id);
		if(itVec != vecDestinationNode.end())
		{
			lDestnationCount--;
			vecDestinationNode.erase(itVec);
			if(lDestnationCount == 0)
				break;
		}


		//long lConnectedEdgeCount = m_pVertexInfoVector->at(vTopVertex.id).connectedEdgeLabel.size();
		long lConnectedEdgeCount = m_pVertexInfoVector->at(vTopVertex.id).vecLayerIndex[0];


		long lEdgeIndex;
		for(lEdgeIndex = 0; lEdgeIndex < lConnectedEdgeCount; lEdgeIndex++)
		{
			long lEdgeLabel = m_pVertexInfoVector->at(vTopVertex.id).connectedEdgeLabel[lEdgeIndex];
			double dCost = m_pEdgeInfoVector->at(lEdgeLabel).costVector[0];

			long lNeighbourVertexLabel = m_pVertexInfoVector->at(vTopVertex.id).connectedVertexLabel[lEdgeIndex];

			if(m_pVertexInfoVector->at(vTopVertex.id).vecDirection[lEdgeIndex] == ForwardDirection || 
				m_pVertexInfoVector->at(vTopVertex.id).vecDirection[lEdgeIndex] == BothDirection )
			{

				double dCurrentCost = dPrevCost + dCost;
				bool bIsVisited = bIsVertexVisited[lNeighbourVertexLabel];

				if(dVertexCost[lNeighbourVertexLabel] > dCurrentCost)
				{
					plPrev[lNeighbourVertexLabel] = vTopVertex.id;
					plPrevEdge[lNeighbourVertexLabel] = lEdgeLabel;
					dVertexCost[lNeighbourVertexLabel] = dCurrentCost;
					bIsVertexVisited[lNeighbourVertexLabel] = true;
					Vertex_def vNewNode;
					vNewNode.id = lNeighbourVertexLabel;
					vNewNode.vcost = dCurrentCost;

					if(!bIsVisited)
					{
						pQueue.push(vNewNode);
						//Test
						//m_pointExplored.insertShape(m_pVertexInfoVector->at(lNeighbourVertexLabel).vertexPoint,lExploredNodeCount);
						//
						lExploredNodeCount++;
					}
				}
			}
		}
	}

	double dDestFeatureCost = -1;
	double dTotalCost;
	long lDestinationNodeIndex;
	//
	// s--------------m--------------->e
	LineDirection direction = m_pEdgeInfoVector->at(endPoint.lFeatureIndex).direction ;
	if(m_pEdgeInfoVector->at(endPoint.lFeatureIndex).direction == ForwardDirection)
	{
		long lDStartNodeLabel = m_pEdgeInfoVector->at(endPoint.lFeatureIndex).startNodeLabel;
		double dDestCost = m_pEdgeInfoVector->at(endPoint.lFeatureIndex).costVector[0];				
		double dPartialCost = (dDestCost * endPoint.dPartialValuePercent)/100;
		double dTotalCost = dVertexCost[lDStartNodeLabel] + dPartialCost;
		dDestCost = dTotalCost;
		lDestinationNodeIndex = lDStartNodeLabel;
		dDestFeatureCost = dPartialCost;
	}
	// s<--------------m---------------e
	// Like the upper case

	else if(m_pEdgeInfoVector->at(endPoint.lFeatureIndex).direction == ReverseDirection)
	{
		long lDEndNodeLabel = m_pEdgeInfoVector->at(endPoint.lFeatureIndex).endNodeLabel;
		double dDestCost = m_pEdgeInfoVector->at(endPoint.lFeatureIndex).costVector[0];				
		double dPartialCost = (dDestCost * (100 - endPoint.dPartialValuePercent))/100;
		double dTotalCost = dVertexCost[lDEndNodeLabel] + dPartialCost;
		dDestCost = dTotalCost;
		lDestinationNodeIndex = lDEndNodeLabel;
		dDestFeatureCost = dPartialCost;
	}
	// s<--------------m--------------->e
	// Like the upper case	
	else
	{
		long lDStartNodeLabel = m_pEdgeInfoVector->at(endPoint.lFeatureIndex).startNodeLabel;
		double dDestCost = m_pEdgeInfoVector->at(endPoint.lFeatureIndex).costVector[0];				
		double dStartPartialCost = (dDestCost * endPoint.dPartialValuePercent)/100;
		double dStartTotalCost = dVertexCost[lDStartNodeLabel] + dStartPartialCost;

		long lDEndNodeLabel = m_pEdgeInfoVector->at(endPoint.lFeatureIndex).endNodeLabel;		
		double dEndPartialCost = (dDestCost * (100 - endPoint.dPartialValuePercent))/100;
		double dEndTotalCost = dVertexCost[lDEndNodeLabel] + dEndPartialCost;
		
		if(dEndTotalCost < dStartTotalCost)
		{
			
			dDestFeatureCost = dEndPartialCost;
			lDestinationNodeIndex = lDEndNodeLabel;
			dTotalCost = dVertexCost[lDestinationNodeIndex];
			GeoMatryDefs::GetPartialPartPoints(m_pEdgeInfoVector->at(endPoint.lFeatureIndex).vecFeaturePoints,endPoint.lStartingPointIndex + 1,m_pEdgeInfoVector->at(endPoint.lFeatureIndex).vecFeaturePoints.size() - 1,false,endPoint.intersectionPoint,vecEndingPartialPart);
		}
		else 
		{
			
			dDestFeatureCost = dStartPartialCost;
			lDestinationNodeIndex = lDStartNodeLabel;
			dTotalCost = dVertexCost[lDestinationNodeIndex];
			GeoMatryDefs::GetPartialPartPoints(m_pEdgeInfoVector->at(endPoint.lFeatureIndex).vecFeaturePoints,0,endPoint.lStartingPointIndex,true,endPoint.intersectionPoint,vecEndingPartialPart);
		}

	}	

	dTotalCost = 0;

	long lParentNodeIndex = lDestinationNodeIndex;
	long lStartingNodeLabel;
	for(;;)
	{
		if(plPrev[lParentNodeIndex] == -1)
		{
			break;
		}
		vecFeatureIndex.push_back(plPrevEdge[lParentNodeIndex]);		
		lParentNodeIndex = plPrev[lParentNodeIndex];
	}
	std::reverse(vecFeatureIndex.begin(),vecFeatureIndex.end());	
	DoubleVector vecPathCost;

	long lEdgeIndex, lEdgeCount = vecFeatureIndex.size();
	for(lEdgeIndex = 0;  lEdgeIndex < lEdgeCount; lEdgeIndex++)
	{
		double dEdgeWeight = m_pEdgeInfoVector->at(vecFeatureIndex[lEdgeIndex]).costVector[0];
		dTotalCost += dEdgeWeight;
		vecPathCost.push_back(dEdgeWeight);
	}

	dTotalCost += dDestFeatureCost;


	double dStartingEdgeCost = m_pEdgeInfoVector->at(startPoint.lFeatureIndex).costVector[0];	
	lStartingNodeLabel = lParentNodeIndex;
	
	if(lStartingNodeLabel == m_pEdgeInfoVector->at(startPoint.lFeatureIndex).startNodeLabel)
	{
		
		dTotalCost += dStartingEdgeCost * startPoint.dPartialValuePercent/100;
		GeoMatryDefs::GetPartialPartPoints(m_pEdgeInfoVector->at(startPoint.lFeatureIndex).vecFeaturePoints,0,startPoint.lStartingPointIndex,true,startPoint.intersectionPoint,vecStartingPartialPart);
	}
	else if(lStartingNodeLabel == m_pEdgeInfoVector->at(startPoint.lFeatureIndex).endNodeLabel)
	{
		dTotalCost += dStartingEdgeCost * ((100 - startPoint.dPartialValuePercent)/100);
		GeoMatryDefs::GetPartialPartPoints(m_pEdgeInfoVector->at(startPoint.lFeatureIndex).vecFeaturePoints,startPoint.lStartingPointIndex + 1,m_pEdgeInfoVector->at(startPoint.lFeatureIndex).vecFeaturePoints.size() - 1,false,startPoint.intersectionPoint,vecStartingPartialPart);
	}

	//Test
	//m_pointExplored.close();
	//

	dAccCost = 	dTotalCost;
	
	printf("Dijkstra:[%ld] cost[%lf]\n",lExploredNodeCount,dAccCost);
	return true;
}


bool ShortestPathDikjstra::GetShortestPath(RoutingPointInfoVector& vecRoutingPointsInfo, OGRPointVector& vecRoutingPoints, double& dAccCost)
{
	return true;
}

bool ShortestPathDikjstra::GetShortestPath(long lStartNodeIndex, long lDestinationNodeIndex,LongVector& vecRouteEdge, double& dCostOfRoute)
{
	long nVertexCount = m_pVertexInfoVector->size();
	
	
	long lShortestPathOpParamIndex = 1;

	std::priority_queue<Vertex_def,std::vector <Vertex_def>, Prioritize >  pQueue;

	long lVertexIndex;
	for(lVertexIndex = 0; lVertexIndex < nVertexCount; lVertexIndex++)
	{
		dVertexCost[lVertexIndex] = DBL_MAX;
		plPrev[lVertexIndex] = -1;
		plPrevEdge[lVertexIndex] = -1;
		bIsVertexVisited[lVertexIndex] = false;
	}
	dVertexCost[lStartNodeIndex] = 0;
	plPrev[lStartNodeIndex]	= -1;	


	printf("[%lf,%lf]-",m_pVertexInfoVector->at(lStartNodeIndex).vertexPoint.x,m_pVertexInfoVector->at(lStartNodeIndex).vertexPoint.y);
	printf("[%lf,%lf]\n",m_pVertexInfoVector->at(lDestinationNodeIndex).vertexPoint.x,m_pVertexInfoVector->at(lDestinationNodeIndex).vertexPoint.y);


	Vertex_def vDef;
	vDef.id = lStartNodeIndex;
	vDef.vcost = 0.000000;
	pQueue.push(vDef);
	bIsVertexVisited[lStartNodeIndex] = true;
	Vertex_def vTopVertex;

	while(!pQueue.empty())
	{

		vTopVertex = pQueue.top();

		pQueue.pop();
		double dPrevCost = vTopVertex.vcost;

		if( vTopVertex.id == lDestinationNodeIndex)
			break;	

	
		long lConnectedEdgeCount = m_pVertexInfoVector->at(vTopVertex.id).connectedEdgeLabel.size();
		long lEdgeIndex;
		for(lEdgeIndex = 0; lEdgeIndex < lConnectedEdgeCount; lEdgeIndex++)
		{
			long lEdgeLabel = m_pVertexInfoVector->at(vTopVertex.id).connectedEdgeLabel[lEdgeIndex];
			double dCost = m_pEdgeInfoVector->at(lEdgeLabel).costVector[0];
			
			long lNeighbourVertexLabel = m_pVertexInfoVector->at(vTopVertex.id).connectedVertexLabel[lEdgeIndex];

			if(m_pVertexInfoVector->at(vTopVertex.id).vecDirection[lEdgeIndex] == ForwardDirection || 
				m_pVertexInfoVector->at(vTopVertex.id).vecDirection[lEdgeIndex] == BothDirection )
			{

				double dCurrentCost = dPrevCost + dCost;
				bool bIsVisited = bIsVertexVisited[lNeighbourVertexLabel];

				if(dVertexCost[lNeighbourVertexLabel] > dCurrentCost)
				{
					plPrev[lNeighbourVertexLabel] = vTopVertex.id;
					plPrevEdge[lNeighbourVertexLabel] = lEdgeLabel;
					dVertexCost[lNeighbourVertexLabel] = dCurrentCost;
					bIsVertexVisited[lNeighbourVertexLabel] = true;
					Vertex_def vNewNode;
					vNewNode.id = lNeighbourVertexLabel;
					vNewNode.vcost = dCurrentCost;

					if(!bIsVisited)
						pQueue.push(vNewNode);
				}
			}
		}
	}

	
	double dTotalCost = vTopVertex.vcost;

	long lParentNodeIndex = lDestinationNodeIndex;
	for(;;)
	{
		if(plPrev[lParentNodeIndex] == -1)
		{
			break;
		}
		vecRouteEdge.push_back(plPrevEdge[lParentNodeIndex]);		
		lParentNodeIndex = plPrev[lParentNodeIndex];
	}
	std::reverse(vecRouteEdge.begin(),vecRouteEdge.end());	


	dCostOfRoute = dTotalCost;

	return true;
}

