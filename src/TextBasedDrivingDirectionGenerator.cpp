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
#include "TextBasedDrivingDirectionGenerator.h"
#include "GeometryDefinition.h"


TextBasedDrivingDirectionGenerator::TextBasedDrivingDirectionGenerator(void)
{
}

TextBasedDrivingDirectionGenerator::~TextBasedDrivingDirectionGenerator(void)
{
}


void TextBasedDrivingDirectionGenerator::SetGraphManagerPtr(GraphManager* pGraphManager)
{
	m_pGraphManager = pGraphManager;
}




std::string TextBasedDrivingDirectionGenerator::GetDirection(double dAngle)
{
	bool bIsPositive = true;
	double dTempAngle = dAngle;
	
	
	if (dTempAngle > 180)
		dTempAngle -= 360;
	else if (dTempAngle < -180)
		dTempAngle += 360;
	
	if (dTempAngle < 0)
	{
		bIsPositive = false;
		dTempAngle = - dTempAngle;
	}


	if (dTempAngle >= 0 && dTempAngle < 45)
		return "Go on";

	else if (dTempAngle >= 45 && dTempAngle < 135)
	{
		if (bIsPositive)
			return "Stay Left";
		else
			return "Stay Right";
	}
	else if(dTempAngle > 135)
	{
		if (bIsPositive)
			return "Sharp Left";
		else
			return "Sharp Right";
	}
	return "Wrong";
}


std::string TextBasedDrivingDirectionGenerator::GetCardinalDirection(double dAngle)
{
	if (dAngle >= 45 && dAngle < 135)
		return "North";
	else if (dAngle >= 135 && dAngle < 225)
		return "West";
	else if (dAngle >= 225 && dAngle < 315)
		return "South";
	else
		return "East";
}

std::string TextBasedDrivingDirectionGenerator::GenerateDDText(DDFeature& ddf, bool bIsFinished)
{
	std::string strDD = "";
	std::ostringstream  output;
	
	output.setf(std::ios::fixed,std::ios::floatfield);
	output << "<edge id='" << ddf.lRoutingEdgeIndex <<"'>";
	if(!bIsFinished)
	{
		if(ddf.lRoutingEdgeIndex == 1)
			output << "<instruction>From Start point drive" << " "<< GetDirection(ddf.dRelativeAngle) <<" "<<ddf.strStreetName << "</instruction>";
		else
			output << "<instruction>" << GetDirection(ddf.dRelativeAngle)<<" " << ddf.dRelativeAngle <<" , stay " << ddf.strStreetName << "</instruction>";
	}
	else
		output << "<instruction> Arrive at destination" <<ddf.strStreetName << "</instruction>";

	output << "<dist units='m'>" << ddf.dDistanceUnit * 1000 << "</dist>";
	output << "<acc_dist units='km'>" << ddf.dAccDistanceUnit << "</acc_dist>";
	output << "<acc_time units='min'>" << ddf.dAccTimeUnit <<"</acc_time>";

	output << "<wkt>LINESTRING(";
	
	long lPointIndex,lPointCount = ddf.vecLineString.size();
	for(lPointIndex = 0; lPointIndex < lPointCount; lPointIndex++)
	{
		output << ddf.vecLineString[lPointIndex].x << " " << ddf.vecLineString[lPointIndex].y;
		if(lPointIndex != lPointCount - 1)
			output << ",";	
	}
	output <<")</wkt>";
	output << "</edge>";
	strDD = output.str();
	return strDD;
}

bool TextBasedDrivingDirectionGenerator::GetDrivingDirection(OGRPointVector& vecRoutingViaPoints,std::string strMinimizingParam,std::string& strResponse)
{
	LongVector vecEdgeIndex;
	double dCost;
	m_pGraphManager->GetShortestPath(vecRoutingViaPoints,vecEdgeIndex,dCost);
	

	std::string strTest = "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\" ?>";
	strTest += "<route>";
	RawPointVector routingPoints;
	if(vecEdgeIndex.size() < 1)
		return false;
	long lLineIndex,lRoutingLineCount = vecEdgeIndex.size();
	long lPointIndex,lPointCount;
	long featureIndex = vecEdgeIndex[0];
	EdgeInfoVector* pEdgeInfoVector = m_pGraphManager->GetEdgeVecPtr();
	
	RawPointVector* pVec =  &pEdgeInfoVector->at(featureIndex).vecFeaturePoints;
	routingPoints.insert(routingPoints.begin(),pVec->begin(),pVec->end());
	DDFeature ddfFeature;
	ddfFeature.lRoutingEdgeIndex = 1;
	ddfFeature.dDistanceUnit = pEdgeInfoVector->at(featureIndex).costVector[0];
	ddfFeature.dAccDistanceUnit = ddfFeature.dDistanceUnit;
	ddfFeature.dTimeUnit = pEdgeInfoVector->at(featureIndex).segmentSpeedValue * ddfFeature.dDistanceUnit;
	ddfFeature.dAccTimeUnit = ddfFeature.dTimeUnit;
	ddfFeature.dRelativeAngle = pEdgeInfoVector->at(featureIndex).startAngle * 180 / PI;
	ddfFeature.vecLineString = pEdgeInfoVector->at(featureIndex).vecFeaturePoints;
	ddfFeature.strStreetName = pEdgeInfoVector->at(featureIndex).strStreetName;
	strTest += GenerateDDText(ddfFeature);


	for(lLineIndex = 1; lLineIndex < lRoutingLineCount - 1; lLineIndex++)
	{
		featureIndex = vecEdgeIndex[lLineIndex];	
		long lSize = routingPoints.size();
		lPointCount = pEdgeInfoVector->at(featureIndex).vecFeaturePoints.size();
		RawPointVector* pVec =  &pEdgeInfoVector->at(featureIndex).vecFeaturePoints;
		OGRRawPoint firstPoint,lastPoint;
		firstPoint = pVec->at(0);
		lastPoint = pVec->at(lPointCount - 1);

		//Points are in forward order or same order
		if(routingPoints[lSize - 1].x == firstPoint.x && routingPoints[lSize - 1].y == firstPoint.y)
		{
			double dAngle1,dAngle2;
			OGRRawPoint lastPoint,secLastPoint;
			lPointCount = routingPoints.size();
			lastPoint = routingPoints[lPointCount - 1];
			secLastPoint = routingPoints[lPointCount - 2];
			double dCurAngle = GeoMatryDefs::GetAngle(secLastPoint.x, secLastPoint.y, lastPoint.x,lastPoint.y);
			dAngle1 = pEdgeInfoVector->at(featureIndex).startAngle;			
			ddfFeature.dRelativeAngle =  (dAngle1 - dCurAngle) * 180 / PI;
			routingPoints.insert(routingPoints.end(),pVec->begin(),pVec->end());
		}

		//Points are in reverse order
		else if(routingPoints[lSize - 1].x == lastPoint.x && routingPoints[lSize - 1].y == lastPoint.y)
		{
			long lPointCount = routingPoints.size();

			OGRRawPoint lastPoint,secLastPoint;
			lastPoint = routingPoints[lPointCount - 1];
			secLastPoint = routingPoints[lPointCount - 2];
			double dCurAngle = GeoMatryDefs::GetAngle(secLastPoint.x, secLastPoint.y, lastPoint.x,lastPoint.y);
			lPointCount = pEdgeInfoVector->at(featureIndex).vecFeaturePoints.size();
			lastPoint = pEdgeInfoVector->at(featureIndex).vecFeaturePoints[lPointCount - 1];
			secLastPoint = pEdgeInfoVector->at(featureIndex).vecFeaturePoints[lPointCount - 2];
			double dAngle = GeoMatryDefs::GetAngle(lastPoint.x,lastPoint.y,secLastPoint.x, secLastPoint.y);
			ddfFeature.dRelativeAngle = (dAngle - dCurAngle) * 180 / PI;
			RawPointVector vecPoints = (*pVec);
			std::reverse(vecPoints.begin(),vecPoints.end());
			routingPoints.insert(routingPoints.end(),vecPoints.begin(),vecPoints.end());

		}

		ddfFeature.lRoutingEdgeIndex++;
		ddfFeature.dDistanceUnit = pEdgeInfoVector->at(featureIndex).costVector[0];
		ddfFeature.dAccDistanceUnit += ddfFeature.dDistanceUnit;
		ddfFeature.dTimeUnit = pEdgeInfoVector->at(featureIndex).segmentSpeedValue * ddfFeature.dDistanceUnit;
		ddfFeature.dAccTimeUnit += ddfFeature.dTimeUnit;
		ddfFeature.strStreetName = pEdgeInfoVector->at(featureIndex).strStreetName;
		ddfFeature.vecLineString = pEdgeInfoVector->at(featureIndex).vecFeaturePoints;
		strTest += GenerateDDText(ddfFeature);
	}

	featureIndex = vecEdgeIndex[lRoutingLineCount - 1];	
	ddfFeature.lRoutingEdgeIndex++;
	ddfFeature.dDistanceUnit = pEdgeInfoVector->at(featureIndex).costVector[0];
	ddfFeature.dAccDistanceUnit += ddfFeature.dDistanceUnit;
	ddfFeature.dTimeUnit = pEdgeInfoVector->at(featureIndex).segmentSpeedValue * ddfFeature.dDistanceUnit;
	ddfFeature.dAccTimeUnit += ddfFeature.dTimeUnit;
	ddfFeature.strStreetName = pEdgeInfoVector->at(featureIndex).strStreetName;
	ddfFeature.vecLineString = pEdgeInfoVector->at(featureIndex).vecFeaturePoints;
	strTest += GenerateDDText(ddfFeature,true);
	strTest += "</route>";
	strResponse = strTest;
	return true;
}

bool TextBasedDrivingDirectionGenerator::GetDrivingDirectionBGL(OGRPointVector& vecRoutingViaPoints,std::string strMinimizingParam,std::string& strResponse)
{
	LongVector vecEdgeIndex;
	double dCost;
	std::string strTest = "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\" ?>";
	strTest += "<route>";

	if(!m_pGraphManager->GetShortestPathBGL(vecRoutingViaPoints,vecEdgeIndex,dCost))
	{
		strTest += "Falied to generate Driving direction";
		strTest += "</route>";
		strResponse = strTest;
		return false;
	}


	
	
	RawPointVector routingPoints;
	if(vecEdgeIndex.size() < 1)
		return false;
	long lLineIndex,lRoutingLineCount = vecEdgeIndex.size();
	long lPointIndex,lPointCount;
	long featureIndex = vecEdgeIndex[0];
	EdgeInfoVector* pEdgeInfoVector = m_pGraphManager->GetEdgeVecPtr();

	RawPointVector* pVec =  &pEdgeInfoVector->at(featureIndex).vecFeaturePoints;
	routingPoints.insert(routingPoints.begin(),pVec->begin(),pVec->end());
	DDFeature ddfFeature;
	ddfFeature.lRoutingEdgeIndex = 1;
	ddfFeature.dDistanceUnit = pEdgeInfoVector->at(featureIndex).costVector[0];
	ddfFeature.dAccDistanceUnit = ddfFeature.dDistanceUnit;
	ddfFeature.dTimeUnit = pEdgeInfoVector->at(featureIndex).segmentSpeedValue * ddfFeature.dDistanceUnit;
	ddfFeature.dAccTimeUnit = ddfFeature.dTimeUnit;
	ddfFeature.dRelativeAngle = pEdgeInfoVector->at(featureIndex).startAngle * 180 / PI;
	ddfFeature.vecLineString = pEdgeInfoVector->at(featureIndex).vecFeaturePoints;
	ddfFeature.strStreetName = pEdgeInfoVector->at(featureIndex).strStreetName;
	strTest += GenerateDDText(ddfFeature);


	for(lLineIndex = 1; lLineIndex < lRoutingLineCount - 1; lLineIndex++)
	{
		featureIndex = vecEdgeIndex[lLineIndex];	
		long lSize = routingPoints.size();
		lPointCount = pEdgeInfoVector->at(featureIndex).vecFeaturePoints.size();
		RawPointVector* pVec =  &pEdgeInfoVector->at(featureIndex).vecFeaturePoints;
		OGRRawPoint firstPoint,lastPoint;
		firstPoint = pVec->at(0);
		lastPoint = pVec->at(lPointCount - 1);

		//Points are in forward order or same order
		if(routingPoints[lSize - 1].x == firstPoint.x && routingPoints[lSize - 1].y == firstPoint.y)
		{
			double dAngle1,dAngle2;
			OGRRawPoint lastPoint,secLastPoint;
			lPointCount = routingPoints.size();
			lastPoint = routingPoints[lPointCount - 1];
			secLastPoint = routingPoints[lPointCount - 2];
			double dCurAngle = GeoMatryDefs::GetAngle(secLastPoint.x, secLastPoint.y, lastPoint.x,lastPoint.y);
			dAngle1 = pEdgeInfoVector->at(featureIndex).startAngle;			
			ddfFeature.dRelativeAngle =  (dAngle1 - dCurAngle) * 180 / PI;
			routingPoints.insert(routingPoints.end(),pVec->begin(),pVec->end());
		}

		//Points are in reverse order
		else if(routingPoints[lSize - 1].x == lastPoint.x && routingPoints[lSize - 1].y == lastPoint.y)
		{
			long lPointCount = routingPoints.size();

			OGRRawPoint lastPoint,secLastPoint;
			lastPoint = routingPoints[lPointCount - 1];
			secLastPoint = routingPoints[lPointCount - 2];
			double dCurAngle = GeoMatryDefs::GetAngle(secLastPoint.x, secLastPoint.y, lastPoint.x,lastPoint.y);
			lPointCount = pEdgeInfoVector->at(featureIndex).vecFeaturePoints.size();
			lastPoint = pEdgeInfoVector->at(featureIndex).vecFeaturePoints[lPointCount - 1];
			secLastPoint = pEdgeInfoVector->at(featureIndex).vecFeaturePoints[lPointCount - 2];
			double dAngle = GeoMatryDefs::GetAngle(lastPoint.x,lastPoint.y,secLastPoint.x, secLastPoint.y);
			ddfFeature.dRelativeAngle = (dAngle - dCurAngle) * 180 / PI;
			RawPointVector vecPoints = (*pVec);
			std::reverse(vecPoints.begin(),vecPoints.end());
			routingPoints.insert(routingPoints.end(),vecPoints.begin(),vecPoints.end());

		}

		ddfFeature.lRoutingEdgeIndex++;
		ddfFeature.dDistanceUnit = pEdgeInfoVector->at(featureIndex).costVector[0];
		ddfFeature.dAccDistanceUnit += ddfFeature.dDistanceUnit;
		ddfFeature.dTimeUnit = pEdgeInfoVector->at(featureIndex).segmentSpeedValue * ddfFeature.dDistanceUnit;
		ddfFeature.dAccTimeUnit += ddfFeature.dTimeUnit;
		ddfFeature.strStreetName = pEdgeInfoVector->at(featureIndex).strStreetName;
		ddfFeature.vecLineString = pEdgeInfoVector->at(featureIndex).vecFeaturePoints;
		strTest += GenerateDDText(ddfFeature);
	}

	featureIndex = vecEdgeIndex[lRoutingLineCount - 1];	
	ddfFeature.lRoutingEdgeIndex++;
	ddfFeature.dDistanceUnit = pEdgeInfoVector->at(featureIndex).costVector[0];
	ddfFeature.dAccDistanceUnit += ddfFeature.dDistanceUnit;
	ddfFeature.dTimeUnit = pEdgeInfoVector->at(featureIndex).segmentSpeedValue * ddfFeature.dDistanceUnit;
	ddfFeature.dAccTimeUnit += ddfFeature.dTimeUnit;
	ddfFeature.strStreetName = pEdgeInfoVector->at(featureIndex).strStreetName;
	ddfFeature.vecLineString = pEdgeInfoVector->at(featureIndex).vecFeaturePoints;
	strTest += GenerateDDText(ddfFeature,true);
	strTest += "</route>";

	//	FILE* fp = fopen("C:\\DD.xml","w");
	//	fwrite(strTest.c_str(),strTest.length() * sizeof(char),1,fp);
	//	fclose(fp);
	strResponse = strTest;
	return true;
}