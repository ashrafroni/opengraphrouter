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
#include "TimerInitiator.h"

CTimerInitiator::CTimerInitiator(void)
{
}

CTimerInitiator::~CTimerInitiator(void)
{
}

void CTimerInitiator::setLayerFullPath(std::string strLayerName)
{
	m_strLayerPath = strLayerName;
}
bool CTimerInitiator::Initialize(EdgeInfoVector* pEdgeInfoVector,VertexInfoVector* pVertexInfoVector, StringVector& vecLayNames)
{
	m_pEdgeInfoVector = pEdgeInfoVector;
	m_pVertexInfoVector = pVertexInfoVector;
	m_vecLayNames = vecLayNames;
	return true;
}

bool CTimerInitiator::InitializeTransitNode()
{
	m_strLayerPath = m_strLayerPath + ".shp";
	OGRDataSource* poDS = OGRSFDriverRegistrar::Open( m_strLayerPath.c_str(), FALSE );
	
	if( poDS == NULL )
	{
		exit( 1 );
	}	

	int iPosLen = m_strLayerPath.length() - m_strLayerPath.find_last_of(".");
	int iPosSlash = m_strLayerPath.find_last_of("\\");
	int iStrLen = m_strLayerPath.length();
	std::string strLayerName = m_strLayerPath.substr(iPosSlash + 1,iStrLen - iPosLen - iPosSlash - 1);
	OGRLayer  *poLayer;
	poLayer = poDS->GetLayerByName( strLayerName.c_str());
	OGRFeature *poFeature;
	poLayer->ResetReading();

	long featureIndex,lLayerFeatureCount;
	lLayerFeatureCount = poLayer->GetFeatureCount();
	OGRFeatureDefn *poFDefn = poLayer->GetLayerDefn();
	int iFieldCount = poFDefn->GetFieldCount();
	
	int iFieldIndex;
	StringVector vecFieldNames;
	LongVector vecLayerIndex;
	LongVector vecFieldIndex;
	for(iFieldIndex = 0; iFieldIndex < iFieldCount; iFieldIndex++)
	{
		std::string strFieldName = poFDefn->GetFieldDefn(iFieldIndex)->GetNameRef();
		vecFieldNames.push_back(strFieldName);		
		int iIndex,iCount = m_vecLayNames.size();
		int iLayerIndex = -1;
		for(iIndex = 0; iIndex < iCount; iIndex++)
		{
			if(m_vecLayNames[iIndex].compare(strFieldName) == 0)
			{
				iLayerIndex = iIndex;
				break;
			}
		}
		vecFieldIndex.push_back(iFieldIndex);
		vecLayerIndex.push_back(iLayerIndex);
	}

	for(featureIndex = 0; featureIndex < lLayerFeatureCount; featureIndex++)	
	{
		poFeature = poLayer->GetFeature(featureIndex);		

		OGRGeometry *poGeometry;
		poGeometry = poFeature->GetGeometryRef();

		if( poGeometry != NULL && wkbFlatten(poGeometry->getGeometryType()) == wkbPoint )	
		{
			OGRPoint *poPoint = (OGRPoint*) poGeometry;
			long lVerTexIndex = GetLabelOfVertex(*poPoint);
			if(lVerTexIndex < 0)
				continue;

			VertexInfo* pVInfo = &m_pVertexInfoVector->at(lVerTexIndex);
			pVInfo->bIsTransitPoint = true;

			long lLayerCount = vecLayerIndex.size();
			long lLayerIndex;

			for(lLayerIndex = 0; lLayerIndex < lLayerCount; lLayerIndex++)
			{
				long lLayerFieldIndex = vecFieldIndex[lLayerIndex];
				std::string strValue = poFeature->GetFieldAsString(lLayerFieldIndex);				
				StringTokenizer strTok;
				strTok.parse(strValue,",");
				StringVector vecTokens;
				if(!strTok.getTokens(vecTokens))
					continue;
				long lTokenIndex,lTokenCount = vecTokens.size();
				long lLIndex = vecLayerIndex[lLayerIndex];
				for(lTokenIndex = 0; lTokenIndex < lTokenCount; lTokenIndex++)
				{
					long lTime = atol(vecTokens[lTokenIndex].c_str());
					pVInfo->timeScheduleHandler.addTime(lLIndex,lTime);
				}		
			}	
		}
	}	
	OGRFeature::DestroyFeature( poFeature );
	OGRDataSource::DestroyDataSource( poDS );
	return true;
}


long CTimerInitiator::GetLabelOfVertex(OGRPoint ogrpVertex)
{
	if(m_pVertexInfoVector == NULL)
		return -1;

	long vertexLabel = -1;

	OGRRawPoint vertexPoint;
	vertexPoint.x = ogrpVertex.getX();
	vertexPoint.y = ogrpVertex.getY();

	long lStartIndex, lEndIndex, lMidIndexX;
	long lPrevStartIndex, lPrevEndIndex;
	long lNodeCount = m_pVertexInfoVector->size();
	lStartIndex = 0;
	lEndIndex = lNodeCount;
	OGRRawPoint curPoint;
	while (lEndIndex > lStartIndex)
	{
		lPrevStartIndex = lStartIndex;
		lPrevEndIndex = lEndIndex;

		lMidIndexX = (lStartIndex + lEndIndex)/2;
		VertexInfo vtxInfo = m_pVertexInfoVector->at(lMidIndexX);
		curPoint = vtxInfo.vertexPoint;

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
