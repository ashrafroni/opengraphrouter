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
#ifndef MASSDATACONVERTER_H
#define MASSDATACONVERTER_H

#include "utils.h"
#include <ogrsf_frmts/shape/shapefil.h>

struct BusStopPointInfo
{
	double dRouteNumber;
	int iDirection;
	long lBusID;
	long lStopID;
	long lSequence;
};

typedef std::vector<BusStopPointInfo> BusStopPointVector;
typedef std::map<long,BusStopPointVector*> ID2BustopInfoVector;






struct BusRoute
{
	RawPointVector vecRoutePoints;
	VectorOfRawPointVector vecRawPointVector;
	long lBusID;
	int iDirection;
	std::string strDescription;
	long lRouteMaj;
	double dRouteNumber;
};
typedef std::vector<BusRoute> BusRouteVector;
typedef std::map<long,BusRoute> ID2BusRoute;

struct BusStopInfo
{
	OGRRawPoint ogrpStopPoint;
	long lStopID;
	std::string strStopName;
};

typedef std::map<long,BusStopInfo> ID2BusStopInfoMap;

typedef std::map<long,VectorOfRawPointVector> ID2VecPointVector;




class CMassDataConverter
{
public:
	CMassDataConverter(void);
	~CMassDataConverter(void);

	

	void setBusRoutePath(std::string strBusRoutePath);
	void setMBTAPointListPath(std::string strMBTAPointList);
	void setMBTABusStopPointList(std::string strMBTABusStopPointList);
	void setMBTABusArcNameBDF(std::string strMBTABusArcNameBDF);
	void setOutputPath(std::string strOutputFolder);
	void separateShapes();

	void separateShapesAndCreate();
	bool changeProjection(std::string strSourcePath, std::string strDestinationPath);


private:
	void clipLineWithPoint(RawPointVector& vecPointsOfLine, OGRRawPoint& ogrpClipPoint,RawPointVector& vecClippedLinePoints);
	void loadMBTAPointList();
	void loadMBTARNames();
	void loadMBTAROUTEArc();
	bool makePolylineShape(VectorOfRawPointVector& vecShape,DoubleVector& vecCost,std::string strLayerFileName);
	

private:
	std::string m_strFilePath;

	std::string m_strBusRoutePath;
	std::string m_strMBTAPointList;
	std::string m_strMBTABusStopPointList;
	std::string m_strMBTABusArcNameBDF;

	std::string m_strOutputFolder;

	ID2BustopInfoVector m_busStopPointInfoStore;
	ID2BusRoute m_busID2BusRouteDetails;
	ID2BusStopInfoMap m_busStopInfo;


	ID2VecPointVector mapBusId2RouteShape;	
	LongVector vecValidBusStopID;

	StringVector vecLayerNames;
	RawPointVector vecBusStops;

};


#endif
