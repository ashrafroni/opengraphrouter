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
#include "PointShapeCreator.h"

CPointShapeCreator::CPointShapeCreator(void)
{
	m_lPointIndex = 0;
}

CPointShapeCreator::~CPointShapeCreator(void)
{
}



bool CPointShapeCreator::createShape(std::string strLayerName)
{
	std::string strFileName = strLayerName;
	strFileName += ".shp";

	shapeHandle = SHPCreate(strLayerName.c_str(),SHPT_POINT);
	if(shapeHandle == NULL)
	{
		printf("shapeHandle null\n");
		return false;
	}

	std::string strdatabase = strLayerName + ".dbf";
	dbfHandle = DBFCreate(strdatabase.c_str());
	if(dbfHandle == NULL)
	{
		printf("dbfHandle null\n");
		return false;		
	}
	return true;		
}

void CPointShapeCreator::addField(std::string strDBFFieldName)
{
	int iRet = DBFAddField(dbfHandle,strDBFFieldName.c_str(),FTString,300,0);
}

void CPointShapeCreator::insertShape(OGRPoint ogrp,long lFieldValue)
{
	double * padfX = new double[1];
	double * padfY= new double[1];
	double * padfZ = new double[1];
	double * padfM = new double[1];


	padfX[0] = ogrp.getX();
	padfY[0] = ogrp.getY();
	padfZ[0] = 0;
	padfM[0] = 0;

	int* panPartStart = new int[1];
	panPartStart[0] = 0;
	SHPObject * pObj = SHPCreateObject(SHPT_POINT,m_lPointIndex,0,NULL,NULL,1,padfX,padfY,padfZ,padfM);
	int iWrite = SHPWriteObject( shapeHandle, -1, pObj );
	SHPDestroyObject(pObj);

	//long lValueIndex,lValueCount = vecFieldValue.size();
	//for(lValueIndex = 0; lValueIndex < lValueCount; lValueIndex++)
	//{
	char chNumber[100];
	sprintf(chNumber,"%ld",lFieldValue);
	iWrite = DBFWriteStringAttribute(dbfHandle,m_lPointIndex,0,chNumber);
	//}
	delete[] padfX;
	delete[] padfY;
	delete[] padfZ;
	delete[] padfM;
	m_lPointIndex++;
}

void CPointShapeCreator::insertShape(RawPointVector& vecPoints,LongVector& vecIDS)
{
	long lPointIndex,lPointCount = vecPoints.size();
	for(lPointIndex = 0; lPointIndex < lPointCount; lPointIndex++)
	{
		insertShape(vecPoints[lPointIndex],vecIDS[lPointIndex]);
	}

}
void CPointShapeCreator::insertShape(RawPointVector& vecPoints)
{
	long lPointIndex,lPointCount = vecPoints.size();
	for(lPointIndex = 0; lPointIndex < lPointCount; lPointIndex++)
	{
		insertShape(vecPoints[lPointIndex],lPointIndex);
	}
}



void CPointShapeCreator::insertShape(OGRRawPoint ogrp,long lFieldValue)
{
	OGRPoint ogr;
	ogr.setX(ogrp.x);
	ogr.setY(ogrp.y);
	insertShape(ogr,lFieldValue);
}

void CPointShapeCreator::insertShape(OGRPoint ogrp,StringVector& vecFieldValue)
{
	double * padfX = new double[1];
	double * padfY= new double[1];
	double * padfZ = new double[1];
	double * padfM = new double[1];


	padfX[0] = ogrp.getX();
	padfY[0] = ogrp.getY();
	padfZ[0] = 0;
	padfM[0] = 0;

	int* panPartStart = new int[1];
	panPartStart[0] = 0;
	SHPObject * pObj = SHPCreateObject(SHPT_POINT,m_lPointIndex,0,NULL,NULL,1,padfX,padfY,padfZ,padfM);
	int iWrite = SHPWriteObject( shapeHandle, -1, pObj );
	SHPDestroyObject(pObj);

	long lValueIndex,lValueCount = vecFieldValue.size();
	for(lValueIndex = 0; lValueIndex < lValueCount; lValueIndex++)
	{
		iWrite = DBFWriteStringAttribute(dbfHandle,m_lPointIndex,lValueIndex,vecFieldValue[lValueIndex].c_str());
	}
	delete[] padfX;
	delete[] padfY;
	delete[] padfZ;
	delete[] padfM;
	m_lPointIndex++;
}

void CPointShapeCreator::close()
{
	SHPClose(shapeHandle);
	DBFClose(dbfHandle);	
}