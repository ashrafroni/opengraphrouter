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
#ifndef REQUESTMANAGER_H
#define REQUESTMANAGER_H

#include "GraphDefinition.h"



enum RequestType
{
	RequestTypeDrivingDirection,
	RequestTypeShowShortestPath,
	RequestTypeShowShortestPathUsingPublicRoute,
	RequestTypeUnknown
};

enum SPathGenerator
{
	SPathBGL,
	SPathDef
};

struct RequestStruct
{
	RequestType m_reqType;
	//RawPointVector m_ogpointVector;
	SPathGenerator m_spGenerator;
	OGRPointVector m_ogrPointVector;
};

class RequestManager
{
public:
	RequestManager(void);
	~RequestManager(void);

public:
	bool parseRequest(std::string strRequest,RequestStruct& request);
	bool replaceCharacters(std::string& strRequest);
private:
	std::string m_strRequest;
	RequestType m_requestType;
	
};


#endif
