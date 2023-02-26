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
#ifndef COMMONDEFINITION_H
#define COMMONDEFINITION_H

#include "utils.h"

#ifndef DBL_MAX
#define DBL_MAX HUGE_VAL
#endif

#ifndef max
#define maxab(a,b)            (((a) > (b)) ? (a) : (b))
#endif

#ifndef min
#define minab(a,b)            (((a) < (b)) ? (a) : (b))
#endif


struct LatLonRect
{
	double dXmin;
	double dYmin;
	double dXmax;
	double dYmax;
};
typedef std::vector<LatLonRect> LatLonRectVector;


enum LineDirection 
{
	ForwardDirection,
	ReverseDirection,
	BothDirection
};
typedef std::vector<LineDirection> DirectionEnumVector;


struct StringLongPair
{
	std::string m_strFieldName;
	long m_lFieldIndex;
};
typedef std::vector<StringLongPair> StringLongPairVector;


struct DataBaseParameters
{
	StringLongPairVector vecCost;
	std::string strDirField;

	std::string strStreetName;
	bool bIsLengthAuto;

	std::string strForwardDIRVal;
	std::string strReverseDIRVal;
	std::string strBothwayDIRVal;
	std::string strSegmentSpeedFieldName;

};

typedef std::vector<DataBaseParameters> DataBaseParameterVector;


struct LayerInfo
{
	std::string m_strLayerName;
	std::string m_strInputPath;
	std::string m_strOutputPath;
	double m_dFairCostPerKm;
	DataBaseParameters m_dbfParameters;
};
typedef std::vector<LayerInfo> LayerInfoVector;



class MapExtent : public LatLonRect
{
public:
	MapExtent(){init();};
	~MapExtent(){};

public:
	void init()
	{
		dXmin = DBL_MAX;
		dYmin = DBL_MAX;
		dXmax = -DBL_MAX;
		dYmax = -DBL_MAX;	
	};

	double getwidth() { return dXmax - dXmin;};
	double getheight() { return dYmax - dYmin;};

	void unionwith(OGRPoint& ogp)
	{
		if (ogp.getX() < dXmin)
			dXmin = ogp.getX();

		if (ogp.getX() > dXmax)
			dXmax = ogp.getX();

		if (ogp.getY() < dYmin)
			dYmin = ogp.getY();

		if (ogp.getY() > dYmax)
			dYmax = ogp.getY();
	}

	void unionwith(OGRRawPoint& ogrp)
	{
		if (ogrp.x < dXmin)
			dXmin = ogrp.x;

		if (ogrp.x > dXmax)
			dXmax = ogrp.x;

		if (ogrp.y < dYmin)
			dYmin = ogrp.y;

		if (ogrp.y > dYmax)
			dYmax = ogrp.y;
	}

	void unionwith(LatLonRect& rect)
	{
		if (rect.dXmin < dXmin)
			dXmin = rect.dXmin;

		if (rect.dXmax > dXmax)
			dXmax = rect.dXmax;

		if (rect.dYmin < dYmin)
			dYmin = rect.dYmin;

		if (rect.dYmax > dYmax)
			dYmax = rect.dYmax;
	}
	void unionWith(LatLonRect& box)
	{
		if (box.dXmin < dXmin)
			dXmin = box.dXmin;

		if (box.dXmax > dXmax)
			dXmax = box.dXmax;

		if (box.dYmin < dYmin)
			dYmin = box.dYmin;

		if (box.dYmax > dYmax)
			dYmax = box.dYmax;
	}

	void operator =(const LatLonRect& er)
	{
		this->dXmin = er.dXmin;
		this->dYmin = er.dYmin;
		this->dXmax = er.dXmax;
		this->dYmax = er.dYmax;
	}


	bool iscontain(double dx, double dy)
	{
		OGRPoint p;
		p.setX(dx);
		p.setY(dy);
		return iscontain(p);
	}
	bool iscontain(OGRPoint &ogp)
	{
		if (ogp.getX() >= dXmin && ogp.getX() <= dXmax && ogp.getY() >= dYmin && ogp.getY() <= dYmax)
			return true;
		return false;
	}
	bool iscontain(LatLonRect &rect)
	{
		if (rect.dXmin >= dXmin &&rect.dYmin >= dYmin &&rect.dXmax <= dXmax &&rect.dYmax <= dYmax)
			return true;
		return false;
	}
	bool iswithin(LatLonRect &rect)
	{
		if (dXmin >= rect.dXmin && dYmin >= rect.dYmin && dXmax <= rect.dXmax && dYmax <= rect.dYmax)
			return true;
		return false;
	}
	bool isIsolatedFrom(LatLonRect &rect)
	{
		if (rect.dXmax < dXmin || rect.dXmin > dXmax || rect.dYmin > dYmax || rect.dYmax < dYmin)
			return true;
		return false;
	}

	void setLatLonRect(double dxmin, double dymin, double dxmax, double dymax)
	{
		this->dXmin = dxmin;
		this->dYmin = dymin;
		this->dXmax = dxmax;
		this->dYmax = dymax;
	}
	void setLatLonRect(LatLonRect* pRect)
	{
		this->dXmin = pRect->dXmin;
		this->dYmin = pRect->dYmin;
		this->dXmax = pRect->dXmax;
		this->dYmax = pRect->dYmax;
	}

	void moveextent(double dOffsetX, double dOffsetY)
	{
		

		dXmin += dOffsetX;
		dYmin += dOffsetY;

		dXmax += dOffsetX;
		dYmax += dOffsetY;
	}

	bool getoverlappinglatlonrect(LatLonRect& rect, LatLonRect& erOverlappingRect)
	{
		bool bIsOverlapped = false;
		erOverlappingRect.dXmin = maxab(dXmin, rect.dXmin);
		erOverlappingRect.dXmax = minab(dXmax, rect.dXmax);
		erOverlappingRect.dYmin = maxab(dYmin, rect.dYmin);
		erOverlappingRect.dYmax = minab(dYmax, rect.dYmax);

		if (erOverlappingRect.dXmax > erOverlappingRect.dXmin && erOverlappingRect.dYmax > erOverlappingRect.dYmin)
			bIsOverlapped = true;
		else
			bIsOverlapped = false;
		return bIsOverlapped;
	}
};


#endif
