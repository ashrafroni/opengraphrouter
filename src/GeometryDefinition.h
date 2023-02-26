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
#ifndef GEOMETRYDEFINITION_H
#define GEOMETRYDEFINITION_H

#ifndef DBL_MAX
#define DBL_MAX HUGE_VAL
#endif

#define PI (3.1415926535897932384626433832795)

#define determineDist(a,b) sqrt(((a).dX-(b).dX)*((a).dX-(b).dX)+((a).dY-(b).dY)*((a).dY-(b).dY))

const double g_earthRadius = 6371.0;
const double g_lonDistPerDeg = 97.0;
const double g_latDistPerDeg = 111.0;



class GeoMatryDefs
{


public:
	static double GetPointDistance(OGRPoint firstPoint, OGRPoint secondPoint)
	{
		double dLatitude,dLongitude;
		dLatitude = (PI*(firstPoint.getY()-secondPoint.getY())/180.0);
		dLongitude = (PI*(firstPoint.getX()-secondPoint.getX())/180.0);

		double dSinLat,dSinLon,dCosY,dCosX;
		dSinLat = sin(dLatitude / 2.0);
		dSinLon = sin(dLongitude / 2.0);
		dCosY = cos((PI * firstPoint.getY())/180.0);
		dCosX = cos((PI * secondPoint.getY())/180.0);

		double dCumu = dSinLat * dSinLat + (dCosY * dCosX) * dSinLon * dSinLon;

		double dSqrt1 = sqrt(dCumu);
		double dSqrt2 = sqrt(1.0 - dCumu);
		double dTmp = atan2(dSqrt1,dSqrt2);

		dTmp *= 2.0;

		double dDistance = g_earthRadius * dTmp;
		return dDistance;
	}

	static double GetPointDistance(OGRRawPoint  firstPoint, OGRRawPoint secondPoint)
	{
		double dLatitude,dLongitude;
		dLatitude = (PI*(firstPoint.y-secondPoint.y)/180.0);
		dLongitude = (PI*(firstPoint.x-secondPoint.x)/180.0);

		double dSinLat,dSinLon,dCosY,dCosX;
		dSinLat = sin(dLatitude / 2.0);
		dSinLon = sin(dLongitude / 2.0);
		dCosY = cos((PI * firstPoint.y)/180.0);
		dCosX = cos((PI * secondPoint.y)/180.0);

		double dCumu = dSinLat * dSinLat + (dCosY * dCosX) * dSinLon * dSinLon;

		double dSqrt1 = sqrt(dCumu);
		double dSqrt2 = sqrt(1.0 - dCumu);
		double dTmp = atan2(dSqrt1,dSqrt2);

		dTmp *= 2.0;

		double dDistance = g_earthRadius * dTmp;
		return dDistance;
	}




	static double GetProjectedDistance(OGRPoint nPoint,OGRPoint startPoint,OGRPoint endPoint,OGRPoint& nearestPoint)
	{
		double xDistance,yDistance,magnitudeSquared;
		double distance1,distance2,distancef;

		xDistance = endPoint.getX() - startPoint.getX();
		yDistance = endPoint.getY() - startPoint.getY();

		magnitudeSquared = xDistance * xDistance + yDistance * yDistance;

		double dTemp =((nPoint.getX() - startPoint.getX())*xDistance + (nPoint.getY() - startPoint.getY()) * yDistance) / magnitudeSquared;

		if(dTemp < 0.0 || dTemp > 1.0)
		{
			distance1 = GetPointDistance(nPoint,startPoint);
			distance2 = GetPointDistance(nPoint,endPoint);
			distancef = (distance1 < distance2)? distance1 : distance2;
			nearestPoint = (distance1 < distance2) ? startPoint: endPoint;		
		}
		else
		{
			startPoint.setX(startPoint.getX() + dTemp * xDistance);
			startPoint.setY(startPoint.getY()+ dTemp * yDistance);
			nearestPoint = startPoint;
			distancef = GetPointDistance(nPoint,startPoint);		
		}
		return distancef;
	}

	static double GetProjectedDistance(OGRPoint nPoint,OGRPoint startPoint,OGRPoint endPoint)
	{
		double xDistance,yDistance,magnitudeSquared;
		double distance1,distance2,distancef;

		xDistance = endPoint.getX() - startPoint.getX();
		yDistance = endPoint.getY() - startPoint.getY();

		magnitudeSquared = xDistance * xDistance + yDistance * yDistance;

		double dTemp =((nPoint.getX() - startPoint.getX())*xDistance + (nPoint.getY() - startPoint.getY()) * yDistance) / magnitudeSquared;

		if(dTemp < 0.0 || dTemp > 1.0)
		{
			distance1 = GetPointDistance(nPoint,startPoint);
			distance2 = GetPointDistance(nPoint,endPoint);
			distancef = (distance1 < distance2)?distance1:distance2;

		}
		else
		{
			startPoint.setX(startPoint.getX() + dTemp * xDistance);
			startPoint.setY(startPoint.getY()+ dTemp * yDistance);
			distancef = GetPointDistance(nPoint,startPoint);		
		}
		return distancef;
	}


	static double DistanceOfPolyLineFeatureFromAPoint(OGRLineString *poLine,OGRPoint ogrPoint)
	{
		int pointIndex,nPointCount = poLine->getNumPoints();	
		double dMinDistance	= DBL_MAX;
		for(pointIndex = 0; pointIndex < nPointCount - 1; pointIndex++)
		{
			OGRPoint poPoint1,poPoint2;
			poLine->getPoint(pointIndex,&poPoint1);
			poLine->getPoint(pointIndex + 1,&poPoint2);
			double tempDistance = GetProjectedDistance(ogrPoint,poPoint1,poPoint2);
			if(dMinDistance > tempDistance)
			{
				dMinDistance = tempDistance;
			}
		}
		return dMinDistance;
	}

	static double DistanceOfPolyLineFeatureFromAPoint(RawPointVector& vecRawPoint,OGRPoint ogrPoint)
	{
		int pointIndex,nPointCount = vecRawPoint.size();	
		double dMinDistance	= DBL_MAX;
		for(pointIndex = 0; pointIndex < nPointCount - 1; pointIndex++)
		{
			OGRPoint poPoint1,poPoint2;

			poPoint1.setX(vecRawPoint[pointIndex].x);
			poPoint1.setY(vecRawPoint[pointIndex].y);
			
			poPoint2.setX(vecRawPoint[pointIndex + 1].x);
			poPoint2.setY(vecRawPoint[pointIndex + 1].y);

			
			double tempDistance = GetProjectedDistance(ogrPoint,poPoint1,poPoint2);
			if(dMinDistance > tempDistance)
			{
				dMinDistance = tempDistance;
			}
		}
		return dMinDistance;
	}


	static bool CalculateWeightSeparation(RawPointVector& vecRawPoint, OGRPoint ogrpDividingPoint, long lStartPointIndex,long lEndPointIndex, double& dL1)
	{
		long lPointIndex, lPointCount = vecRawPoint.size();
		if(lStartPointIndex < 0 || lStartPointIndex  >= lPointCount)
			return false;

		if(lEndPointIndex < 0 || lEndPointIndex  > lPointCount)
			return false;

		double dCost1 = 0.00,dCost2 = 0.00,dAggCost = 0.00;

		for(lPointIndex = 0; lPointIndex < lPointCount - 1; lPointIndex++)
		{
			dAggCost += GetPointDistance(vecRawPoint[lPointIndex],vecRawPoint[lPointIndex + 1]);
		}

		for(lPointIndex = 0; lPointIndex <= lStartPointIndex - 1; lPointIndex++)
		{
			dCost1 += GetPointDistance(vecRawPoint[lPointIndex],vecRawPoint[lPointIndex + 1]);
		}

		for(lPointIndex = lEndPointIndex; lPointIndex < lPointCount - 1; lPointIndex++)
		{
			dCost2 += GetPointDistance(vecRawPoint[lPointIndex],vecRawPoint[lPointIndex + 1]);
		}

		OGRRawPoint ogrp;
		ogrp.x = ogrpDividingPoint.getX();
		ogrp.y = ogrpDividingPoint.getY();

		dCost1 += GetPointDistance(vecRawPoint[lStartPointIndex],ogrp);
		dCost2 += GetPointDistance(ogrp,vecRawPoint[lEndPointIndex]);	
		dL1 = (dCost1 / dAggCost) * 100;
		return true;
	}

	static double DistanceOfPolyLineFeatureFromAPoint(RawPointVector& vecRawPoint,OGRPoint ogrPoint,long& lStartIndex,OGRPoint& ogrIntersectionPoint)
	{
		int pointIndex,nPointCount = vecRawPoint.size();	
		double dMinDistance	= DBL_MAX;
		OGRPoint ogrIntPoint;
		for(pointIndex = 0; pointIndex < nPointCount - 1; pointIndex++)
		{
			OGRPoint poPoint1,poPoint2;

			poPoint1.setX(vecRawPoint[pointIndex].x);
			poPoint1.setY(vecRawPoint[pointIndex].y);

			poPoint2.setX(vecRawPoint[pointIndex + 1].x);
			poPoint2.setY(vecRawPoint[pointIndex + 1].y);


			double tempDistance = GetProjectedDistance(ogrPoint,poPoint1,poPoint2,ogrIntPoint);
			if(dMinDistance > tempDistance)
			{
				lStartIndex = pointIndex;
				dMinDistance = tempDistance;
				ogrIntersectionPoint = ogrIntPoint;
			}
		}
		return dMinDistance;
	}

	static double GetLength(RawPointVector& vecRawPoint)
	{
		double dLength = 0.000000000;
		int pointIndex,nPointCount = vecRawPoint.size();		
		for(pointIndex = 0; pointIndex < nPointCount - 1; pointIndex++)
		{
			OGRRawPoint poPoint1,poPoint2;
			poPoint1 = vecRawPoint[pointIndex];
			poPoint2 = vecRawPoint[pointIndex + 1];
			dLength += GetPointDistance(poPoint1,poPoint2);
		}	
		return dLength;
	}

	static double GetAngle(double x1,double y1,double x2,double y2)
	{
		if (x1 == x2 && y1 == y2)
			return 0;
		double dAngle;
		if (y1 > y2)
			dAngle = 2.0 * PI - acos((x2-x1)/sqrt((x1-x2)*(x1-x2)+(y1-y2)*(y1-y2)));
		else
			dAngle = acos((x2-x1)/sqrt((x1-x2)*(x1-x2)+(y1-y2)*(y1-y2)));

		return dAngle;
	}

	static bool GetPartialPartPoints(RawPointVector& vecFeaturePoints,long lStartIndex, long lEndIndex, bool bIsForward, OGRPoint& intersectionPoint,OGRPointVector& vecPartialPartPoints)
	{
		long lIndex;
		for(lIndex = lStartIndex; lIndex <= lEndIndex; lIndex++)
		{
			OGRPoint ogrp;
			ogrp.setX(vecFeaturePoints[lIndex].x);
			ogrp.setY(vecFeaturePoints[lIndex].y);
			vecPartialPartPoints.push_back(ogrp);
		}
		if(bIsForward)
			vecPartialPartPoints.push_back(intersectionPoint);
		else
			vecPartialPartPoints.insert(vecPartialPartPoints.begin(),intersectionPoint);

		return true;
	}

	static bool GetPartialPartPoints(RawPointVector& vecFeaturePoints,long lStartIndex, long lEndIndex, bool bIsForward, OGRRawPoint& intersectionPoint,RawPointVector& vecPartialPartPoints)
	{
		long lIndex;
		for(lIndex = lStartIndex; lIndex <= lEndIndex; lIndex++)
		{
			OGRRawPoint ogrp;
			ogrp = vecFeaturePoints[lIndex];			
			vecPartialPartPoints.push_back(ogrp);
		}
		if(bIsForward)
			vecPartialPartPoints.push_back(intersectionPoint);
		else
			vecPartialPartPoints.insert(vecPartialPartPoints.begin(),intersectionPoint);

		return true;
	}

};

#endif
