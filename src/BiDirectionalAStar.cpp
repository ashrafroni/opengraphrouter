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
#include "BiDirectionalAStar.h"
#include "GeometryDefinition.h"

class PrioritizeBDAStar
{
public:
	int operator() ( const Vertex_def_BDAStar& p1,const Vertex_def_BDAStar& p2 ) 
	{
		return p1.astarcost> p2.astarcost;
	}
};
CBiDirectionalAStar::CBiDirectionalAStar(void)
{
}

CBiDirectionalAStar::~CBiDirectionalAStar(void)
{
}

bool CBiDirectionalAStar::Initialize(EdgeInfoVector* pEdgeInfoVector,VertexInfoVector* pVertexInfoVector, long lNumberOfLayers)
{
	m_pEdgeInfoVector = pEdgeInfoVector;
	m_pVertexInfoVector = pVertexInfoVector;

	long nVertexCount = m_pVertexInfoVector->size();
	plPrev = new long[nVertexCount + 1];
	plPrevEdge = new long[nVertexCount + 1];
	bIsVertexVisited = new bool[nVertexCount + 1];
	dVertexCost = new double[nVertexCount + 1];		
	pTrDirection = new NodeTravellingDirection[nVertexCount + 1];
	
	return true;
}

bool CBiDirectionalAStar::GetShortestPath(LongVector& vecRoutingPointVertex, LongVector& vecEdgeIndex,double& dAccCost)
{
	return true;
}

bool CBiDirectionalAStar::GetShortestPathVertex(LongVector& vecRoutingPointVertex, LongVector& vecVertexIndex,double& dAccCost) 
{
	return true;
}


bool CBiDirectionalAStar::GetAccShortestPath(RoutingPointInfo& startPoint, RoutingPointInfo& endPoint, OGRPointVector& vecStartingPartialPart,OGRPointVector& vecEndingPartialPart,LongVector& vecFeatureIndex ,double& dAccCost)
{
	long nVertexCount = m_pVertexInfoVector->size();
	long lExploredNodeCount = 0;
	long lShortestPathOpParamIndex = 1;

	long lForwardDirOpenNodeCount = 0;
	long lReverseDirOpenNodeCount = 0;



	std::priority_queue<Vertex_def_BDAStar,std::vector <Vertex_def_BDAStar>, PrioritizeBDAStar >  pQueue;
	Vertex_def_BDAStar vTopVertex;
	long lVertexIndex;
	for(lVertexIndex = 0; lVertexIndex < nVertexCount; lVertexIndex++)
	{
		dVertexCost[lVertexIndex] = DBL_MAX;
		plPrev[lVertexIndex] = -1;
		plPrevEdge[lVertexIndex] = -1;
		bIsVertexVisited[lVertexIndex] = false;		
		pTrDirection[lVertexIndex] = NodeTravellingDirectionUnknown;
	}

//	m_pointExplored.createShape("D:\\output\\bidirAStar");
//	m_pointExplored.addField("ID");

	Vertex_def_BDAStar vDef;

	OGRRawPoint destinationPoint;
	destinationPoint.x = endPoint.intersectionPoint.getX();
	destinationPoint.y = endPoint.intersectionPoint.getY();
	OGRRawPoint sourcePoint;
	sourcePoint.x = startPoint.intersectionPoint.getX();
	sourcePoint.y = startPoint.intersectionPoint.getY();

	// s---------------m--------------->e
	// So the value of edge from middle to end have to be added
	if(m_pEdgeInfoVector->at(startPoint.lFeatureIndex).direction == ForwardDirection)
	{
		long lNodeLabel = m_pEdgeInfoVector->at(startPoint.lFeatureIndex).endNodeLabel;
		double dCost = m_pEdgeInfoVector->at(startPoint.lFeatureIndex).costVector[0];
		dVertexCost[lNodeLabel] = (dCost * (100 - startPoint.dPartialValuePercent))/100;
		plPrev[lNodeLabel]	= -1;	
		
		pTrDirection[lNodeLabel]= NodeTravellingDirectionForward;
		

		vDef.id = lNodeLabel;		
		vDef.oroginalCost  = dVertexCost[lNodeLabel];
		vDef.astarcost = dVertexCost[lNodeLabel] + GeoMatryDefs::GetPointDistance(m_pVertexInfoVector->at(lNodeLabel).vertexPoint,destinationPoint);
		vDef.nodeTravellingDirection = NodeTravellingDirectionForward;

		pQueue.push(vDef);
		bIsVertexVisited[lNodeLabel] = true;

		//Test
		//m_pointExplored.insertShape(m_pVertexInfoVector->at(lNodeLabel).vertexPoint,lExploredNodeCount);
		//
		lExploredNodeCount++;
		lForwardDirOpenNodeCount++;

	}
	// s<---------------m---------------e
	// So the value of edge from middle to start have to be added
	else if(m_pEdgeInfoVector->at(startPoint.lFeatureIndex).direction == ReverseDirection)
	{
		long lNodeLabel = m_pEdgeInfoVector->at(startPoint.lFeatureIndex).startNodeLabel;		
		double dCost = m_pEdgeInfoVector->at(startPoint.lFeatureIndex).costVector[0];
		dVertexCost[lNodeLabel] = dCost * startPoint.dPartialValuePercent/100;
		plPrev[lNodeLabel]	= -1;
		
		pTrDirection[lNodeLabel]= NodeTravellingDirectionForward;

		vDef.id = lNodeLabel;
		vDef.oroginalCost = dVertexCost[lNodeLabel];
		vDef.astarcost = dVertexCost[lNodeLabel] + GeoMatryDefs::GetPointDistance(m_pVertexInfoVector->at(lNodeLabel).vertexPoint,destinationPoint);
		vDef.nodeTravellingDirection = NodeTravellingDirectionForward;
		pQueue.push(vDef);
		bIsVertexVisited[lNodeLabel] = true;
		//Test
		m_pointExplored.insertShape(m_pVertexInfoVector->at(lNodeLabel).vertexPoint,lExploredNodeCount);
		//
		lExploredNodeCount++;
		lForwardDirOpenNodeCount++;

	}
	// s<---------------m--------------->e
	// So the value of edge from middle to start and middle to end have to be added
	else
	{

		double dCost = m_pEdgeInfoVector->at(startPoint.lFeatureIndex).costVector[0];

		long lStartNode = m_pEdgeInfoVector->at(startPoint.lFeatureIndex).startNodeLabel;		
		dVertexCost[lStartNode] = (dCost * startPoint.dPartialValuePercent)/100;
		plPrev[lStartNode]	= -1;
		pTrDirection[lStartNode]= NodeTravellingDirectionForward;

		vDef.id = lStartNode;
		vDef.oroginalCost= dVertexCost[lStartNode];
		vDef.astarcost = dVertexCost[lStartNode] + GeoMatryDefs::GetPointDistance(m_pVertexInfoVector->at(lStartNode).vertexPoint,destinationPoint);
		vDef.nodeTravellingDirection = NodeTravellingDirectionForward;
		pQueue.push(vDef);
		bIsVertexVisited[lStartNode] = true;
		


		long lEndNode = m_pEdgeInfoVector->at(startPoint.lFeatureIndex).endNodeLabel;
		dVertexCost[lEndNode] = (dCost * (100 - startPoint.dPartialValuePercent))/100;
		plPrev[lEndNode]	= -1;
		pTrDirection[lEndNode]= NodeTravellingDirectionForward;
		

		vDef.id = lEndNode;
		vDef.oroginalCost = dVertexCost[lEndNode];
		vDef.astarcost = dVertexCost[lStartNode] + GeoMatryDefs::GetPointDistance(m_pVertexInfoVector->at(lEndNode).vertexPoint,destinationPoint);
		vDef.nodeTravellingDirection = NodeTravellingDirectionForward;
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

		lForwardDirOpenNodeCount += 2;
	}


	if(m_pEdgeInfoVector->at(endPoint.lFeatureIndex).direction == ForwardDirection)
	{
		long lNodeLabel = m_pEdgeInfoVector->at(endPoint.lFeatureIndex).startNodeLabel;
		double dCost = m_pEdgeInfoVector->at(endPoint.lFeatureIndex).costVector[0];
		dVertexCost[lNodeLabel] = (dCost * (100 - endPoint.dPartialValuePercent))/100;
		plPrev[lNodeLabel]	= -1;	
		pTrDirection[lNodeLabel]= NodeTravellingDirectionReverse;

		vDef.id = lNodeLabel;		
		vDef.oroginalCost  = dVertexCost[lNodeLabel];
		vDef.astarcost = dVertexCost[lNodeLabel] + GeoMatryDefs::GetPointDistance(m_pVertexInfoVector->at(lNodeLabel).vertexPoint,sourcePoint);
		vDef.nodeTravellingDirection = NodeTravellingDirectionReverse;
		pQueue.push(vDef);
		bIsVertexVisited[lNodeLabel] = true;
		
		//Test
		//m_pointExplored.insertShape(m_pVertexInfoVector->at(lNodeLabel).vertexPoint,lExploredNodeCount);
		//
		lExploredNodeCount++;
		lReverseDirOpenNodeCount++;

	}
	// s<---------------m---------------e
	// So the value of edge from middle to start have to be added
	else if(m_pEdgeInfoVector->at(endPoint.lFeatureIndex).direction == ReverseDirection)
	{
		long lNodeLabel = m_pEdgeInfoVector->at(endPoint.lFeatureIndex).endNodeLabel;		
		double dCost = m_pEdgeInfoVector->at(endPoint.lFeatureIndex).costVector[0];
		dVertexCost[lNodeLabel] = dCost * endPoint.dPartialValuePercent/100;
		plPrev[lNodeLabel]	= -1;
		pTrDirection[lNodeLabel]= NodeTravellingDirectionReverse;

		vDef.id = lNodeLabel;
		vDef.oroginalCost = dVertexCost[lNodeLabel];
		vDef.astarcost = dVertexCost[lNodeLabel] + GeoMatryDefs::GetPointDistance(m_pVertexInfoVector->at(lNodeLabel).vertexPoint,sourcePoint);
		vDef.nodeTravellingDirection = NodeTravellingDirectionReverse;
		pQueue.push(vDef);
		bIsVertexVisited[lNodeLabel] = true;
		//Test
		//m_pointExplored.insertShape(m_pVertexInfoVector->at(lNodeLabel).vertexPoint,lExploredNodeCount);
		//
		lExploredNodeCount++;
		lReverseDirOpenNodeCount++;

	}
	// s<---------------m--------------->e
	// So the value of edge from middle to start and middle to end have to be added
	else
	{

		double dCost = m_pEdgeInfoVector->at(endPoint.lFeatureIndex).costVector[0];

		long lStartNode = m_pEdgeInfoVector->at(endPoint.lFeatureIndex).startNodeLabel;		
		dVertexCost[lStartNode] = (dCost * endPoint.dPartialValuePercent)/100;
		plPrev[lStartNode]	= -1;
		pTrDirection[lStartNode]= NodeTravellingDirectionReverse;

		vDef.id = lStartNode;
		vDef.oroginalCost= dVertexCost[lStartNode];
		vDef.astarcost = dVertexCost[lStartNode] + GeoMatryDefs::GetPointDistance(m_pVertexInfoVector->at(lStartNode).vertexPoint,sourcePoint);
		vDef.nodeTravellingDirection = NodeTravellingDirectionReverse;
		pQueue.push(vDef);
		bIsVertexVisited[lStartNode] = true;
		


		long lEndNode = m_pEdgeInfoVector->at(endPoint.lFeatureIndex).endNodeLabel;
		dVertexCost[lEndNode] = (dCost * (100 - endPoint.dPartialValuePercent))/100;
		plPrev[lEndNode] = -1;	
		pTrDirection[lEndNode]= NodeTravellingDirectionReverse;

		vDef.id = lEndNode;
		vDef.oroginalCost = dVertexCost[lEndNode];
		vDef.astarcost = dVertexCost[lStartNode] + GeoMatryDefs::GetPointDistance(m_pVertexInfoVector->at(lEndNode).vertexPoint,sourcePoint);
		vDef.nodeTravellingDirection = NodeTravellingDirectionReverse;
		pQueue.push(vDef);
		bIsVertexVisited[lEndNode] = true;
		
		//Test
		//m_pointExplored.insertShape(m_pVertexInfoVector->at(lStartNode).vertexPoint,lExploredNodeCount);
		//
		lExploredNodeCount+= 1;
		//Test
		//m_pointExplored.insertShape(m_pVertexInfoVector->at(lEndNode).vertexPoint,lExploredNodeCount);
		//
		lReverseDirOpenNodeCount += 2;
	}




	//long lDestnationCount = vecDestinationNode.size();
	bool bIsFinished = false;

	long lFinishedNode = -1;
	long lFinishedNeighBourVertexLabel = -1;
	long lFinishedFeatureIndex = -1;
	

	while(!pQueue.empty() && !bIsFinished)	
	{

		vTopVertex = pQueue.top();
		pQueue.pop();
		double dPrevCost = vTopVertex.astarcost;
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

			double dDistance = 0;//GeoMatryDefs::GetPointDistance(ogrDestPoint,ogrpVTop);
			double dCost = m_pEdgeInfoVector->at(lEdgeLabel).costVector[0];  

			long lNeighbourVertexLabel = m_pVertexInfoVector->at(vTopVertex.id).connectedVertexLabel[lEdgeIndex];

			LineDirection lineDirection = ForwardDirection;
			
			if(vTopVertex.nodeTravellingDirection == NodeTravellingDirectionForward)
			{
				lineDirection = ForwardDirection;
				dDistance = GeoMatryDefs::GetPointDistance(ogrDestPoint,ogrpVTop);
			}
			else if(vTopVertex.nodeTravellingDirection == NodeTravellingDirectionReverse)
			{
				lineDirection = ReverseDirection;
				dDistance = GeoMatryDefs::GetPointDistance(sourcePoint,ogrpVTop);
			}



			if(m_pVertexInfoVector->at(vTopVertex.id).vecDirection[lEdgeIndex] == lineDirection || 
				m_pVertexInfoVector->at(vTopVertex.id).vecDirection[lEdgeIndex] == BothDirection )
			{

				//Here the change we have to made
				double dCurrentCost = dPrevCost + dCost;
				double dCurrentCostWithDistance = dPrevCost + dCost + dDistance;
				bool bIsVisited = bIsVertexVisited[lNeighbourVertexLabel];

				if(pTrDirection[lNeighbourVertexLabel] != NodeTravellingDirectionUnknown && pTrDirection[lNeighbourVertexLabel] != vTopVertex.nodeTravellingDirection)
				{
					lFinishedNode = vTopVertex.id;
					lFinishedNeighBourVertexLabel = lNeighbourVertexLabel;
					lFinishedFeatureIndex = m_pVertexInfoVector->at(vTopVertex.id).connectedEdgeLabel[lEdgeIndex];
					bIsFinished = true;
					break;
				}
				

				if(dVertexCost[lNeighbourVertexLabel] > dCurrentCost)
				{
					plPrev[lNeighbourVertexLabel] = vTopVertex.id;
					plPrevEdge[lNeighbourVertexLabel] = lEdgeLabel;
					dVertexCost[lNeighbourVertexLabel] = dCurrentCost;
					bIsVertexVisited[lNeighbourVertexLabel] = true;
					Vertex_def_BDAStar vNewNode;
					vNewNode.id = lNeighbourVertexLabel;
					vNewNode.oroginalCost = vTopVertex.oroginalCost + dCost;
					vNewNode.astarcost = dCurrentCostWithDistance;
					vNewNode.nodeTravellingDirection = vTopVertex.nodeTravellingDirection;

					if(vTopVertex.nodeTravellingDirection == NodeTravellingDirectionForward)
					{					
						pTrDirection[lNeighbourVertexLabel] = NodeTravellingDirectionForward;
					}
					else if(vTopVertex.nodeTravellingDirection == NodeTravellingDirectionReverse)
					{		
						pTrDirection[lNeighbourVertexLabel] = NodeTravellingDirectionReverse;
					}
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

	
	long lParentNodeIndex = -1;
	double dDstPartialCost = -1;
	double dTotalCost;
	long lDestinationNodeIndex;
	
	long lStartingNodeLabel,lEndingNodeLabel;
	lParentNodeIndex = lFinishedNode;
	LongVector vecIndexPart1;
	long lNodeLabel1;
	for(;;)
	{
		if(plPrev[lParentNodeIndex] == -1)
		{
			break;
		}
		vecIndexPart1.push_back(plPrevEdge[lParentNodeIndex]);		
		lNodeLabel1 = plPrev[lParentNodeIndex];
		lParentNodeIndex = plPrev[lParentNodeIndex];		
	}
	

	long lNodeLabel2;
	LongVector vecIndexPart2;
	lParentNodeIndex = lFinishedNeighBourVertexLabel;
	for(;;)
	{
		if(plPrev[lParentNodeIndex] == -1)
		{
			break;
		}
		vecIndexPart2.push_back(plPrevEdge[lParentNodeIndex]);		
		lNodeLabel2 = plPrev[lParentNodeIndex];
		lParentNodeIndex = plPrev[lParentNodeIndex];
		
	}

	if(vTopVertex.nodeTravellingDirection == NodeTravellingDirectionForward)
		std::reverse(vecIndexPart1.begin(),vecIndexPart1.end());
	else
		std::reverse(vecIndexPart2.begin(),vecIndexPart2.end());

	if(vTopVertex.nodeTravellingDirection == NodeTravellingDirectionForward)
	{
		vecFeatureIndex.insert(vecFeatureIndex.end(), vecIndexPart1.begin(),vecIndexPart1.end());
		vecFeatureIndex.push_back(lFinishedFeatureIndex);
		vecFeatureIndex.insert(vecFeatureIndex.end(), vecIndexPart2.begin(),vecIndexPart2.end());
		lStartingNodeLabel = lNodeLabel1;
		lEndingNodeLabel = lNodeLabel2; 
	}
	else
	{
		vecFeatureIndex.insert(vecFeatureIndex.end(), vecIndexPart2.begin(),vecIndexPart2.end());
		vecFeatureIndex.push_back(lFinishedFeatureIndex);
		vecFeatureIndex.insert(vecFeatureIndex.end(), vecIndexPart1.begin(),vecIndexPart1.end());
		lStartingNodeLabel = lNodeLabel2;
		lEndingNodeLabel = lNodeLabel1; 
	}	
	dTotalCost = 0;

	long lEdgeIndex, lEdgeCount = vecFeatureIndex.size();
	for(lEdgeIndex = 0;  lEdgeIndex < lEdgeCount; lEdgeIndex++)
	{
		double dEdgeWeight = m_pEdgeInfoVector->at(vecFeatureIndex[lEdgeIndex]).costVector[0];
		dTotalCost += dEdgeWeight;		
	}
	
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


	if(lEndingNodeLabel == m_pEdgeInfoVector->at(endPoint.lFeatureIndex).startNodeLabel)
	{
		GeoMatryDefs::GetPartialPartPoints(m_pEdgeInfoVector->at(endPoint.lFeatureIndex).vecFeaturePoints,0,startPoint.lStartingPointIndex,true,endPoint.intersectionPoint,vecEndingPartialPart);
		double dCost = m_pEdgeInfoVector->at(endPoint.lFeatureIndex).costVector[0];
		dTotalCost += (dCost * endPoint.dPartialValuePercent)/100;

	}
	else if(lEndingNodeLabel == m_pEdgeInfoVector->at(endPoint.lFeatureIndex).endNodeLabel)
	{
		GeoMatryDefs::GetPartialPartPoints(m_pEdgeInfoVector->at(endPoint.lFeatureIndex).vecFeaturePoints,endPoint.lStartingPointIndex + 1,m_pEdgeInfoVector->at(endPoint.lFeatureIndex).vecFeaturePoints.size() - 1,false,endPoint.intersectionPoint,vecEndingPartialPart);
		double dCost = m_pEdgeInfoVector->at(endPoint.lFeatureIndex).costVector[0];
		dTotalCost += (dCost * (100 - endPoint.dPartialValuePercent))/100;
	}

	//m_pointExplored.close();	
	


	dAccCost = dTotalCost;
	printf("BiDirectionalAStar:[%ld] cost[%lf]\n",lExploredNodeCount,dAccCost);
	

	return true;
}