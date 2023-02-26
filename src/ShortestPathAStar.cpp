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
#include "ShortestPathAStar.h"
#include "GeometryDefinition.h"

class PrioritizeAStar
{
public:
	int operator() ( const Vertex_def_AStar& p1,const Vertex_def_AStar& p2 ) 
	{
		return p1.astarcost> p2.astarcost;
	}
};

CShortestPathAStar::CShortestPathAStar(void)
{
}

CShortestPathAStar::~CShortestPathAStar(void)
{
}


bool CShortestPathAStar::Initialize(EdgeInfoVector* pEdgeInfoVector,VertexInfoVector* pVertexInfoVector, long lNumberOfLayers)
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

bool CShortestPathAStar::GetShortestPath(LongVector& vecRoutingPointVertex, LongVector& vecEdgeIndex,double& dAccCost)
{
	return true;
}

bool CShortestPathAStar::GetShortestPathVertex(LongVector& vecRoutingPointVertex, LongVector& vecVertexIndex,double& dAccCost) 
{
	return true;
}


bool CShortestPathAStar::GetAccShortestPath(RoutingPointInfo& startPoint, RoutingPointInfo& endPoint, OGRPointVector& vecStartingPartialPart,OGRPointVector& vecEndingPartialPart,LongVector& vecFeatureIndex ,double& dAccCost)
{
	long nVertexCount = m_pVertexInfoVector->size();
	long lExploredNodeCount = 0;
	long lShortestPathOpParamIndex = 1;

	//m_pointExplored.createShape("D:\\output\\AstarNodeExplore");
	//m_pointExplored.addField("ID");


	std::priority_queue<Vertex_def_AStar,std::vector <Vertex_def_AStar>, PrioritizeAStar >  pQueue;
	Vertex_def_AStar vTopVertex;
	long lVertexIndex;
	for(lVertexIndex = 0; lVertexIndex < nVertexCount; lVertexIndex++)
	{
		dVertexCost[lVertexIndex] = DBL_MAX;
		plPrev[lVertexIndex] = -1;
		plPrevEdge[lVertexIndex] = -1;
		bIsVertexVisited[lVertexIndex] = false;
	}


	Vertex_def_AStar vDef;

	OGRRawPoint destinationPoint;
	destinationPoint.x = endPoint.intersectionPoint.getX();
	destinationPoint.y = endPoint.intersectionPoint.getY();

	// s---------------m--------------->e
	// So the value of edge from middle to end have to be added
	if(m_pEdgeInfoVector->at(startPoint.lFeatureIndex).direction == ForwardDirection)
	{
		long lNodeLabel = m_pEdgeInfoVector->at(startPoint.lFeatureIndex).endNodeLabel;
		double dCost = m_pEdgeInfoVector->at(startPoint.lFeatureIndex).costVector[0];
		dVertexCost[lNodeLabel] = (dCost * (100 - startPoint.dPartialValuePercent))/100;
		plPrev[lNodeLabel]	= -1;	

		vDef.id = lNodeLabel;		
		vDef.oroginalCost  = dVertexCost[lNodeLabel];
		vDef.astarcost = dVertexCost[lNodeLabel] + GeoMatryDefs::GetPointDistance(m_pVertexInfoVector->at(lNodeLabel).vertexPoint,destinationPoint);
		
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
		vDef.oroginalCost = dVertexCost[lNodeLabel];
		vDef.astarcost = dVertexCost[lNodeLabel] + GeoMatryDefs::GetPointDistance(m_pVertexInfoVector->at(lNodeLabel).vertexPoint,destinationPoint);
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
		vDef.oroginalCost= dVertexCost[lStartNode];
		vDef.astarcost = dVertexCost[lStartNode] + GeoMatryDefs::GetPointDistance(m_pVertexInfoVector->at(lStartNode).vertexPoint,destinationPoint);
		pQueue.push(vDef);
		bIsVertexVisited[lStartNode] = true;


		long lEndNode = m_pEdgeInfoVector->at(startPoint.lFeatureIndex).endNodeLabel;
		dVertexCost[lEndNode] = (dCost * (100 - startPoint.dPartialValuePercent))/100;
		plPrev[lEndNode]	= -1;	

		vDef.id = lEndNode;
		vDef.oroginalCost = dVertexCost[lEndNode];
		vDef.astarcost = dVertexCost[lStartNode] + GeoMatryDefs::GetPointDistance(m_pVertexInfoVector->at(lEndNode).vertexPoint,destinationPoint);
		
		pQueue.push(vDef);
		bIsVertexVisited[lEndNode] = true;
		
		//Test
		//m_pointExplored.insertShape(m_pVertexInfoVector->at(lStartNode).vertexPoint,lExploredNodeCount);
		//
		lExploredNodeCount+= 1;

		//Test
		//m_pointExplored.insertShape(m_pVertexInfoVector->at(lEndNode).vertexPoint,lExploredNodeCount);
		//

		lExploredNodeCount+= 1;


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
		double dPrevCost = vTopVertex.astarcost;

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
			
			long lVCount = m_pVertexInfoVector->at(vTopVertex.id).connectedVertexLabel.size();
			long lVertexLabel = m_pVertexInfoVector->at(vTopVertex.id).connectedVertexLabel[lEdgeIndex];


			OGRRawPoint ogrpNeighbour = m_pVertexInfoVector->at(lVertexLabel).vertexPoint;
			OGRRawPoint ogrpVTop = m_pVertexInfoVector->at(vTopVertex.id).vertexPoint;

			OGRRawPoint ogrDestPoint;
			ogrDestPoint.x = endPoint.intersectionPoint.getX();
			ogrDestPoint.y = endPoint.intersectionPoint.getY();

			double dDistance = GeoMatryDefs::GetPointDistance(ogrDestPoint,ogrpVTop);
			double dCost = m_pEdgeInfoVector->at(lEdgeLabel).costVector[0];  

			long lNeighbourVertexLabel = m_pVertexInfoVector->at(vTopVertex.id).connectedVertexLabel[lEdgeIndex];

			if(m_pVertexInfoVector->at(vTopVertex.id).vecDirection[lEdgeIndex] == ForwardDirection || 
				m_pVertexInfoVector->at(vTopVertex.id).vecDirection[lEdgeIndex] == BothDirection )
			{

				//Here the change we have to made
				double dCurrentCost = dPrevCost + dCost;
				double dCurrentCostWithDistance = dPrevCost + dCost + dDistance;
				bool bIsVisited = bIsVertexVisited[lNeighbourVertexLabel];

				if(dVertexCost[lNeighbourVertexLabel] > dCurrentCost)
				{
					plPrev[lNeighbourVertexLabel] = vTopVertex.id;
					plPrevEdge[lNeighbourVertexLabel] = lEdgeLabel;
					dVertexCost[lNeighbourVertexLabel] = dCurrentCost;
					bIsVertexVisited[lNeighbourVertexLabel] = true;
					Vertex_def_AStar vNewNode;
					vNewNode.id = lNeighbourVertexLabel;
					vNewNode.oroginalCost = vTopVertex.oroginalCost + dCost;
					vNewNode.astarcost = dCurrentCostWithDistance;
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

	double dDstPartialCost = -1;
	double dTotalCost;
	long lDestinationNodeIndex;
	//
	// s--------------m--------------->e
	if(m_pEdgeInfoVector->at(endPoint.lFeatureIndex).direction == ForwardDirection)
	{
		long lDStartNodeLabel = m_pEdgeInfoVector->at(endPoint.lFeatureIndex).startNodeLabel;
		double dDestCost = m_pEdgeInfoVector->at(endPoint.lFeatureIndex).costVector[0];				
		double dPartialCost = (dDestCost * startPoint.dPartialValuePercent)/100;
		double dTotalCost = dVertexCost[lDStartNodeLabel] + dPartialCost;
		dDstPartialCost = dPartialCost;
		lDestinationNodeIndex = lDStartNodeLabel;
	}
	// s<--------------m---------------e
	// Like the upper case

	else if(m_pEdgeInfoVector->at(endPoint.lFeatureIndex).direction == ReverseDirection)
	{
		long lDEndNodeLabel = m_pEdgeInfoVector->at(endPoint.lFeatureIndex).endNodeLabel;
		double dDestCost = m_pEdgeInfoVector->at(endPoint.lFeatureIndex).costVector[0];				
		double dPartialCost = (dDestCost * (100 - startPoint.dPartialValuePercent))/100;
		double dTotalCost = dVertexCost[lDEndNodeLabel] + dPartialCost;
		dDstPartialCost = dPartialCost;
		lDestinationNodeIndex = lDEndNodeLabel;
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
			dDstPartialCost = dEndPartialCost;
			lDestinationNodeIndex = lDEndNodeLabel;			
			GeoMatryDefs::GetPartialPartPoints(m_pEdgeInfoVector->at(endPoint.lFeatureIndex).vecFeaturePoints,endPoint.lStartingPointIndex + 1,m_pEdgeInfoVector->at(endPoint.lFeatureIndex).vecFeaturePoints.size() - 1,false,endPoint.intersectionPoint,vecEndingPartialPart);
		}
		else 
		{
			dDstPartialCost = dStartPartialCost;
			lDestinationNodeIndex = lDStartNodeLabel;			
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

	dTotalCost += dDstPartialCost;

	lStartingNodeLabel = lParentNodeIndex;
	if(lStartingNodeLabel == m_pEdgeInfoVector->at(startPoint.lFeatureIndex).startNodeLabel)
	{
		GeoMatryDefs::GetPartialPartPoints(m_pEdgeInfoVector->at(startPoint.lFeatureIndex).vecFeaturePoints,0,startPoint.lStartingPointIndex,true,startPoint.intersectionPoint,vecStartingPartialPart);
		double dCost = m_pEdgeInfoVector->at(startPoint.lFeatureIndex).costVector[0];
		dTotalCost += (dCost * startPoint.dPartialValuePercent)/100;	 
	}

	else if(lStartingNodeLabel == m_pEdgeInfoVector->at(startPoint.lFeatureIndex).endNodeLabel)
	{
		GeoMatryDefs::GetPartialPartPoints(m_pEdgeInfoVector->at(startPoint.lFeatureIndex).vecFeaturePoints,startPoint.lStartingPointIndex + 1,m_pEdgeInfoVector->at(startPoint.lFeatureIndex).vecFeaturePoints.size() - 1,false,startPoint.intersectionPoint,vecStartingPartialPart);
		double dCost = m_pEdgeInfoVector->at(startPoint.lFeatureIndex).costVector[0];
		dTotalCost += (dCost * (100 - startPoint.dPartialValuePercent))/100;
	}
	dAccCost = dTotalCost;

	//Test
	//m_pointExplored.close();
	//

	printf("AStar:[%ld] cost[%lf]\n",lExploredNodeCount,dAccCost);

	return true;
}		