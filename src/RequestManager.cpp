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
#include "RequestManager.h"
#include "utils.h"
#include "RequestManager.h"
#include "ConfigTags.h"



//?request=GetPath&routepoints=-131.763957,58.894665$-114.055304,49.024089
//?request=GetPathBGL&routepoints=-131.763957,58.894665$-114.055304,49.024089
//?request=GetDDBGL&routepoints=-131.763957,58.894665$-114.055304,49.024089
//?request=GetDD&routepoints=-131.763957,58.894665$-114.055304,49.024089

RequestManager::RequestManager(void)
{
}

RequestManager::~RequestManager(void)
{
}




bool RequestManager::parseRequest(std::string strRequest,RequestStruct& request)
{
	
	std::string strRequestTemp = strRequest;
	replaceCharacters(strRequestTemp);
	
	StringTokenizer strTok;
	strTok.parse(strRequestTemp,"&");
	StringVector vecTokens;
	strTok.getTokens(vecTokens);
	long lTokenIndex,lTokenCount = vecTokens.size();
	for(lTokenIndex = 0; lTokenIndex < lTokenCount; lTokenIndex++)
	{
		std::string strTag = vecTokens.at(lTokenIndex);
		StringTokenizer strTok1;
		strTok1.parse(strTag,"=");
		if(strTok1.getTokenCount() < 2)
			return false;
		std::string strTagName,strTagValue;
		strTok1.getToken(strTagName,0);
		std::transform(strTagName.begin(), strTagName.end(), strTagName.begin(),toupper);	

		strTok1.getToken(strTagValue,1);
		
		if(strTagName.compare(RequestTags::tagRequest) == 0)
		{
			std::transform(strTagValue.begin(), strTagValue.end(), strTagValue.begin(),toupper);	

			if(strTagValue.compare(RequestTags::tagGetPathBGL ) == 0)
			{
				request.m_reqType = RequestTypeShowShortestPath;
				request.m_spGenerator = SPathBGL;
			}
			else if(strTagValue.compare(RequestTags::tagGetPath ) == 0)
			{
				request.m_reqType = RequestTypeShowShortestPath;
				request.m_spGenerator = SPathDef;
			}
			else if(strTagValue.compare(RequestTags::tagGetDD) == 0)
			{
				request.m_reqType = RequestTypeDrivingDirection;
				request.m_spGenerator = SPathDef;
			}
			else if(strTagValue.compare(RequestTags::tagGetDrivingDirectionBGL ) == 0)
			{
				request.m_reqType = RequestTypeDrivingDirection;
				request.m_spGenerator = SPathBGL;
			}
			else if(strTagValue.compare(RequestTags::tagGetPathUsingPublicRoute) == 0	)
			{
				request.m_reqType = RequestTypeShowShortestPathUsingPublicRoute;
				request.m_spGenerator = SPathDef;
			}
		}

		else if(strTagName.compare(RequestTags::tagRoutePoints) == 0)
		{
			StringTokenizer strTokRPoints;
			strTokRPoints.parse(strTagValue,"$");
			StringVector vecPontsToken;
			strTokRPoints.getTokens(vecPontsToken);
			long lPointIndex,lPointCount = vecPontsToken.size();

			for(lPointIndex = 0; lPointIndex < lPointCount; lPointIndex++)	
			{
				StringTokenizer strTokPos;
				strTokPos.parse(vecPontsToken[lPointIndex],",");
				if(strTokPos.getTokenCount() < 2)
					return false;
				std::string strLat,strLon;
				strTokPos.getToken(strLat,0);
				strTokPos.getToken(strLon,1);
				OGRRawPoint rawPoint;
				double dX = atof(strLat.c_str());
				double dY = atof(strLon.c_str());
				OGRPoint ogPoint;
				ogPoint.setX(dX);
				ogPoint.setY(dY);

				request.m_ogrPointVector.push_back(ogPoint);
			}
		}
	}
	return true;
}


bool RequestManager::replaceCharacters(std::string& strRequest)
{
	long lPosition = 0;
	for(;;)
	{					
		lPosition = strRequest.find("%2C", 0);
		if(lPosition != std::string::npos)
		{		
			strRequest.replace(lPosition, 3, ",");
		}
		else 
			break;
	}

	for(;;)
	{					
		lPosition = strRequest.find("%24", 0);
		if(lPosition != std::string::npos)
		{		
			strRequest.replace(lPosition, 3, "$");
		}
		else 
			break;
	}
    return true;
}
