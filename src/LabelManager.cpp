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
#include "LabelManager.h"



bool PointOrderFunc(const OGRRawPoint& point1, const OGRRawPoint& point2)
{
	if(point1.x == point2.x)
		return point1.y < point2.y;
	return point1.x < point2.x;
}


LabelManager::LabelManager(void)
{
}

LabelManager::~LabelManager(void)
{

}

bool LabelManager::BuildNodeEdgeVector(ArcNodeInfo* pArcNodeInfo,long nPointCount,EdgeInfoVector* pEdgeInfoVector, VertexInfoVector* pVertexInfoVector, LongVector& vecLayerFeatureCount)
{
	nFeatureCount = nPointCount;
	labelPointsBuffer.reserve(nFeatureCount * 2);
	pEdgeInfoVector->reserve(nFeatureCount);
	pVertexInfoVector->reserve(nFeatureCount * 2);


	m_pEdgeInfoVector = pEdgeInfoVector;
	m_pVertexInfoVector = pVertexInfoVector;
	long lANInfoIndex;
	

	//Generate Vertex Point list
	for(lANInfoIndex = 0; lANInfoIndex < nFeatureCount; lANInfoIndex++)
	{
		labelPointsBuffer.push_back(pArcNodeInfo[lANInfoIndex].startPoint);
		labelPointsBuffer.push_back(pArcNodeInfo[lANInfoIndex].endPoint);
	}
	long lIndex = 0;
	long lDeletedPointCount = 0;

	//Sort all the vertex points 
	std::sort(labelPointsBuffer.begin(), labelPointsBuffer.end(),PointOrderFunc);
	
	long featureIndex,featureCount = labelPointsBuffer.size();

	//Remove the duplicate points
	//as this is sorted so the duplicate points will be consecutive
	long lPointIndex,lPointCount = labelPointsBuffer.size();
	OGRRawPoint curPoint,nextPoint;

	curPoint = labelPointsBuffer[0];
	labelPointsOrig.push_back(curPoint);
	for(lPointIndex = 1; lPointIndex < lPointCount; lPointIndex++)
	{
		nextPoint = labelPointsBuffer[lPointIndex];

		if(curPoint.x == nextPoint.x && curPoint.y == nextPoint.y)
		{
			lDeletedPointCount++;
		}
		else
			labelPointsOrig.push_back(nextPoint);

		curPoint = labelPointsBuffer[lPointIndex];		
	}
	long lCurSize = labelPointsOrig.size();
	labelPointsBuffer.clear();
	pVertexInfoVector->resize(lCurSize);
	
	featureCount = labelPointsOrig.size();


	long lLayerIndex = 0;
	lANInfoIndex = 0;
	long lTotalFeatureCount = 0;
	for(; lLayerIndex < vecLayerFeatureCount.size(); lLayerIndex++)
	{
		long lLayerFeatureCount = vecLayerFeatureCount[lLayerIndex];
		lTotalFeatureCount += lLayerFeatureCount;
		for(; lANInfoIndex < lTotalFeatureCount; lANInfoIndex++)
		{
			EdgeInfo edgeInfo;
			edgeInfo.edgeLabel = lANInfoIndex;		
			edgeInfo.edgeGeomatryID = lANInfoIndex;
			edgeInfo.sourceLayerIndex = pArcNodeInfo[lANInfoIndex].dataSrcID;		

			long vertexLabelSP = GetLabelOfVertexNew(pArcNodeInfo[lANInfoIndex].startPoint);
			pVertexInfoVector->at(vertexLabelSP).vertexLabel = vertexLabelSP;
			pVertexInfoVector->at(vertexLabelSP).bIsTransitPoint = false;
			pVertexInfoVector->at(vertexLabelSP).vertexPoint = pArcNodeInfo[lANInfoIndex].startPoint;
			pVertexInfoVector->at(vertexLabelSP).connectedEdgeLabel.push_back(lANInfoIndex);



			long vertexLabelEP = GetLabelOfVertexNew(pArcNodeInfo[lANInfoIndex].endPoint);
			pVertexInfoVector->at(vertexLabelEP).bIsTransitPoint = false;
			pVertexInfoVector->at(vertexLabelEP).vertexLabel = vertexLabelEP;
			pVertexInfoVector->at(vertexLabelEP).vertexPoint = pArcNodeInfo[lANInfoIndex].endPoint;		
			pVertexInfoVector->at(vertexLabelEP).connectedEdgeLabel.push_back(lANInfoIndex);		

			pVertexInfoVector->at(vertexLabelSP).connectedVertexLabel.push_back(vertexLabelEP);
			pVertexInfoVector->at(vertexLabelEP).connectedVertexLabel.push_back(vertexLabelSP);

			if(pArcNodeInfo[lANInfoIndex].direction == ForwardDirection)
			{
				pVertexInfoVector->at(vertexLabelSP).vecDirection.push_back(ForwardDirection);
				pVertexInfoVector->at(vertexLabelEP).vecDirection.push_back(ReverseDirection);
			}
			else if(pArcNodeInfo[lANInfoIndex].direction == ReverseDirection)
			{
				pVertexInfoVector->at(vertexLabelSP).vecDirection.push_back(ReverseDirection);
				pVertexInfoVector->at(vertexLabelEP).vecDirection.push_back(ForwardDirection);
			}
			else if(pArcNodeInfo[lANInfoIndex].direction == BothDirection)
			{
				pVertexInfoVector->at(vertexLabelSP).vecDirection.push_back(BothDirection);
				pVertexInfoVector->at(vertexLabelEP).vecDirection.push_back(BothDirection);
			}

			edgeInfo.startNodeLabel = vertexLabelSP;
			edgeInfo.endNodeLabel  = vertexLabelEP;

			edgeInfo.costVector = pArcNodeInfo[lANInfoIndex].costVector;
			edgeInfo.vecFeaturePoints = pArcNodeInfo[lANInfoIndex].vecFeaturePoints;
			edgeInfo.startAngle = pArcNodeInfo[lANInfoIndex].startAngle;
			edgeInfo.endAngle = pArcNodeInfo[lANInfoIndex].endAngle;

			edgeInfo.direction = pArcNodeInfo[lANInfoIndex].direction;

			edgeInfo.segmentSpeedValue = pArcNodeInfo[lANInfoIndex].segmentSpeedValue;
			edgeInfo.strStreetName = pArcNodeInfo[lANInfoIndex].strStreetName;

			pArcNodeInfo[lANInfoIndex].vecFeaturePoints.clear();
			pEdgeInfoVector->push_back(edgeInfo);	
		}

		//Pushing the layer vertex index for future use;
		long lVertexIndex,lVertexCount = pVertexInfoVector->size();
		for(lVertexIndex = 0;lVertexIndex < lVertexCount; lVertexIndex++)
		{
			long lConnectedVertexCount = pVertexInfoVector->at(lVertexIndex).connectedVertexLabel.size();
			pVertexInfoVector->at(lVertexIndex).vecLayerIndex.push_back(lConnectedVertexCount);
		}
	}		
	return true;
}

long LabelManager::GetLabelOfVertexNew(OGRRawPoint vertexPoint)
{
	long vertexLabel = -1;

	long lStartIndex, lEndIndex, lMidIndexX;
	long lPrevStartIndex, lPrevEndIndex;
	long lNodeCount = labelPointsOrig.size();
	lStartIndex = 0;
	lEndIndex = lNodeCount;
	OGRRawPoint curPoint;
	while (lEndIndex > lStartIndex)
	{
		lPrevStartIndex = lStartIndex;
		lPrevEndIndex = lEndIndex;

		lMidIndexX = (lStartIndex + lEndIndex)/2;
		curPoint = labelPointsOrig.at(lMidIndexX);

		if (vertexPoint.x == curPoint.x &&vertexPoint.y == curPoint.y)	
			break;
		if (vertexPoint.x > curPoint.x)
			lStartIndex = lMidIndexX + 1;
		else if (vertexPoint.x == curPoint.x && vertexPoint.y > curPoint.y)
			lStartIndex = lMidIndexX + 1;
		else
			lEndIndex = lMidIndexX;
	}

	if(vertexPoint.x == curPoint.x && vertexPoint.y == curPoint.y)
	{
		return lMidIndexX;
	}
	return vertexLabel;
}


long LabelManager::GetLabelOfVertex(OGRRawPoint vertexPoint)
{
	long vertexIndex = 0;
	long featureIndex;	
	long nVerteXCount = labelPointsOrig.size();
	bool bIsFound = false;

	long vertexLabel = -1;
	for(featureIndex = 0; featureIndex < nVerteXCount; featureIndex++)
	{
		if(labelPointsOrig[featureIndex].x == vertexPoint.x && labelPointsOrig[featureIndex].y == vertexPoint.y)
		{
			bIsFound = true;
			vertexLabel = featureIndex;			
			break;
		}
	}
	return vertexLabel;
	
}
