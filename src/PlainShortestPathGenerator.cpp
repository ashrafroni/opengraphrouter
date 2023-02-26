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
#include "PlainShortestPathGenerator.h"

CPlainShortestPathGenerator::CPlainShortestPathGenerator(void)
{
	m_pGraphManager = NULL;
}

CPlainShortestPathGenerator::~CPlainShortestPathGenerator(void)
{
}
	
void CPlainShortestPathGenerator::SetGraphManagerPtr(GraphManager* pGraphManager)
{
	m_pGraphManager = pGraphManager;
}


//bool CPlainShortestPathGenerator::GetBiDirectionalShortestPathAStar(OGRPointVector& vecRoutingViaPoints, OGRPointVector& vecRoutingPoints,double& dAccCost);bool GetBiDirectionalShortestPathAStar(OGRPointVector& vecRoutingViaPoints, OGRPointVector& vecRoutingPoints,double& dAccCost)
//{
//
//}

bool CPlainShortestPathGenerator::GetShortestPathAStar(OGRPointVector& vecRoutingViaPoints,std::string& strResponse)
{

	if(m_pGraphManager == NULL)
	{
		strResponse = "NULL Pointer";
		return false;
	}
	std::ostringstream outResponse;
	OGRPointVector vecPoints;
	double dCost;
	if(!m_pGraphManager->GetShortestPathAStar(vecRoutingViaPoints,vecPoints,dCost))
	{
		outResponse << "Failed to generate shortest path";
	}
	else
	{
		long lPointIndex,lPointCount = vecPoints.size();
		std::string strTest = "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\" ?>";
		outResponse << strTest;
		outResponse << "<RoutingPoints>";
		for(lPointIndex = 0; lPointIndex < lPointCount; lPointIndex++)
		{
			OGRPoint ogrPoint = vecPoints[lPointIndex];
			outResponse <<"(" << ogrPoint.getX() << "," << ogrPoint.getY() << ")";
		}
		outResponse << "</RoutingPoints>";
	}
	strResponse = outResponse.str();
	return true;
}

bool CPlainShortestPathGenerator::GetShortestPath(OGRPointVector& vecRoutingViaPoints,std::string& strResponse)
{
	if(m_pGraphManager == NULL)
	{
		strResponse = "NULL Pointer";
		return false;
	}
	std::ostringstream outResponse;
	OGRPointVector vecPoints;
	double dCost;
	if(!m_pGraphManager->GetAccourateShortestPath(vecRoutingViaPoints,vecPoints,dCost))
	{
		outResponse << "Failed to generate shortest path";
	}
	else
	{
		long lPointIndex,lPointCount = vecPoints.size();
		std::string strTest = "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\" ?>";
		outResponse << strTest;
		outResponse << "<RoutingPoints>";
		for(lPointIndex = 0; lPointIndex < lPointCount; lPointIndex++)
		{
			OGRPoint ogrPoint = vecPoints[lPointIndex];
			outResponse <<"(" << ogrPoint.getX() << "," << ogrPoint.getY() << ")";
		}
		outResponse << "</RoutingPoints>";
	}
	strResponse = outResponse.str();
	return true;
}

bool CPlainShortestPathGenerator::GetShortestPathUsingPublicTransport(OGRPointVector& vecRoutingViaPoints,long lStartingTime,std::string& strResponse)
{
	if(m_pGraphManager == NULL)
	{
		strResponse = "NULL Pointer";
		return false;
	}
	std::ostringstream outResponse;
	OGRPointVector vecPoints;
	double dCost;
	if(!m_pGraphManager->GetAccourateShortestPathUsingPublic(vecRoutingViaPoints,lStartingTime,vecPoints,dCost))
	{
		outResponse << "Failed to generate shortest path";
	}
	else
	{
		long lPointIndex,lPointCount = vecPoints.size();
		std::string strTest = "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\" ?>";
		outResponse << strTest;
		outResponse << "<RoutingPoints>";
		for(lPointIndex = 0; lPointIndex < lPointCount; lPointIndex++)
		{
			OGRPoint ogrPoint = vecPoints[lPointIndex];
			outResponse <<"(" << ogrPoint.getX() << "," << ogrPoint.getY() << ")";
		}
		outResponse << "</RoutingPoints>";
	}
	strResponse = outResponse.str();
	return true;
}