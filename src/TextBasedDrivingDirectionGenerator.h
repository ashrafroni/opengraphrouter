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
#ifndef TEXTBASEDDRIVINGDIRECTIONGENERATOR_H
#define TEXTBASEDDRIVINGDIRECTIONGENERATOR_H

#include "GraphDefinition.h"
#include "GraphManager.h"


struct DDFeature
{
	long lRoutingEdgeIndex;
	double dRelativeAngle;
	double dDistanceUnit;
	double dTimeUnit;
	double dAccDistanceUnit;
	double dAccTimeUnit;
	RawPointVector vecLineString;
	std::string strStreetName;


};

class TextBasedDrivingDirectionGenerator
{
public:
	TextBasedDrivingDirectionGenerator(void);
	~TextBasedDrivingDirectionGenerator(void);




public:
	void SetGraphManagerPtr(GraphManager* pGraphManager);
	bool GetDrivingDirection(OGRPointVector& vecRoutingViaPoints,std::string strMinimizingParam,std::string& strResponse); 
	bool GetDrivingDirectionBGL(OGRPointVector& vecRoutingViaPoints,std::string strMinimizingParam,std::string& strResponse); 
	

private:
	std::string GenerateDDText(DDFeature& ddf , bool bIsFinished = false);
	std::string GetCardinalDirection(double dAngle);
	std::string GetDirection(double dAngle);

private:
	GraphManager* m_pGraphManager;
};

#endif
