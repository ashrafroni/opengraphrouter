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
// opengraphrouter.cpp : Defines the entry point for the console application.
//

#include <stdio.h>
#include "ogrsf_frmts.h"
#include "GraphManager.h"
#include "RequestManager.h"
#include "TimeScheduleHandler.h"
#include "ApplicationDataStore.h"
#include <ogrsf_frmts/shape/shapefil.h>
#include <ogrsf_frmts/kml/ogr_kml.h>
#include "TimerInitiator.h"
#include "PointShapeCreator.h"
#include "GeometryDefinition.h"
#include "MassDataConverter.h"

#include "fcgi_stdio.h"



ApplicationDataStore g_appDataStore;


static char xmlHeader[] = 
"Content-Length:%u\r\n"
"Content-Type:application/xml\r\n"
"\r\n"
"%s";








bool getRequestString(std::string& strReqString)
{
	char *buffer;		
	buffer = getenv("REQUEST_METHOD");
	if(buffer == NULL)
		return false;

	if (strcmp(buffer,"POST") == 0)
	{
		int	cl;
		buffer = NULL;
		buffer = getenv("CONTENT_LENGTH");

		if(buffer == NULL || strlen(buffer) == 0)
			return false;	

		if((cl = atoi(buffer)) == 0)	
			return false;

		char* request = new char[cl+2];

		int iBlocksRead = 0;			
		iBlocksRead = fread(request, cl, 1, stdin);
		if(iBlocksRead != 1)
		{
			delete [] request;
			return false;
		}

		request[cl] = 0;
		strReqString = request;

		delete [] request;
	}
	else if (strcmp(buffer, "GET") == 0)
	{
		buffer = getenv("QUERY_STRING");

		if(buffer == NULL)	
			return false;		
		strReqString= buffer;

	} 	
	else
		return false;

	return true;
}



bool makeShape(VectorOfPointVector& vecShape,DoubleVector& vecCost,std::string strLayerFileName)
{
	SHPHandle shapeHandle;
	DBFHandle dbfHandle;
	std::string strLayerName = "D:\\output\\" + strLayerFileName;
	std::string strShapeName = "D:\\output\\" + strLayerFileName + std::string(".shp");
	
	shapeHandle = SHPCreate(strShapeName.c_str(),SHPT_ARC);
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
	}
	int iRet = DBFAddField(dbfHandle,"Length",FTDouble,15,8);
	long lFeatureIndex,lFeatureCount = vecShape.size();
	for(lFeatureIndex = 0; lFeatureIndex < lFeatureCount; lFeatureIndex++)
	{
		OGRPointVector* pVec =  &vecShape[lFeatureIndex];
		long lFeaturePointCount = pVec->size();	
		double * padfX = new double[lFeaturePointCount];
		double * padfY = new double[lFeaturePointCount];
		double * padfZ = new double[lFeaturePointCount];
		double * padfM = new double[lFeaturePointCount];

		long lPointIndex,lPointCount = pVec->size();
		for(lPointIndex = 0; lPointIndex < lPointCount; lPointIndex++)
		{
			double dx = pVec->at(lPointIndex).getX();
			double dy = pVec->at(lPointIndex).getY();
			padfX[lPointIndex] = dx;
			padfY[lPointIndex] = dy;
			padfZ[lPointIndex] = 0;
			padfM[lPointIndex] = 0;
		}
		int* panPartStart = new int[1];
		panPartStart[0] = 0;		
		SHPObject * pObj = SHPCreateObject(SHPT_ARC,lFeatureIndex,1,panPartStart,NULL,lPointCount,padfX,padfY,padfZ,padfM);
		int iShapeWrite = SHPWriteObject( shapeHandle, -1, pObj);
		int idbfWrite1 = DBFWriteDoubleAttribute(dbfHandle,lFeatureIndex,0,vecCost[lFeatureIndex]);
		delete[] padfX;
		delete[] padfY;
		delete[] padfZ;
		delete[] padfM;
	}
	SHPClose(shapeHandle);
	DBFClose(dbfHandle);
	return true;
}



void createPointShape()
{
	StringVector vecBlank;
	vecBlank.push_back("1200");
	vecBlank.push_back("1500");

	CPointShapeCreator pointCreator;
	pointCreator.createShape("D:\\TransitNode");
	pointCreator.addField("bus1");
	pointCreator.addField("bus2");
	//x=28.146819000000001 y=-25.706807000000001 z=0.00000000000000000 }	OGRPoint *

	OGRPoint ogrp;
	//	-25.691612, 28.137279 
	ogrp.setX(28.146128999999998);
	ogrp.setY(-25.694289999999999);
	pointCreator.insertShape(ogrp,vecBlank);

	//	-25.694209, 28.146183
	ogrp.setX(28.137278999999999);
	ogrp.setY(-25.691611999999999);
	pointCreator.insertShape(ogrp,vecBlank);

	//	-25.696625, 28.161587
	ogrp.setX(28.161579999999997);
	ogrp.setY(-25.696636999999999);
	pointCreator.insertShape(ogrp,vecBlank);

	//	-25.699243, 28.173294
	ogrp.setX(28.173289000000000);
	ogrp.setY(-25.699216999999997 );
	pointCreator.insertShape(ogrp,vecBlank);

	//	-25.677839, 28.178582
	ogrp.setX(28.178563000000000);
	ogrp.setY(-25.677859999999999);
	pointCreator.insertShape(ogrp,vecBlank);

	//	-25.668732, 28.169936
	ogrp.setX(28.169920999999999);
	ogrp.setY(-25.668713000000000);
	pointCreator.insertShape(ogrp,vecBlank);

	//	-25.660414, 28.143029
	ogrp.setX(28.143125999999999);
	ogrp.setY(-25.660418000000000);
	pointCreator.insertShape(ogrp,vecBlank);

	
	OGRPoint testPoint1;
	testPoint1.setX(28.146818999999997);
	testPoint1.setY(-25.706806999999998);
	pointCreator.insertShape(testPoint1,vecBlank);
	
	testPoint1.setX(28.154283999999997);
	testPoint1.setY(-25.697309999999998);
	pointCreator.insertShape(testPoint1,vecBlank);
	
	testPoint1.setX(28.158187999999999);
	testPoint1.setY(-25.659348999999999);
	pointCreator.insertShape(testPoint1,vecBlank);
	pointCreator.close();	
}


void createPublicLayer()
{
	double dCost1,dCost2,dCost5;
	OGRPointVector pointVectorResult1,pointVectorResult2;
	OGRPoint testPoint1,testPoint2,testPoint3;
	OGRPointVector pointVector,pointVectorPublicRoute;

	testPoint1.setX(28.146942);
	testPoint1.setY(-25.706910);
	pointVectorPublicRoute.push_back(testPoint1);

	
	testPoint1.setX(28.154465);
	testPoint1.setY(-25.697477);
	pointVectorPublicRoute.push_back(testPoint1);
	


	testPoint1.setX(28.173289000000000);
	testPoint1.setY(-25.699216999999997);
	pointVectorPublicRoute.push_back(testPoint1);

	testPoint1.setX(28.169920999999999);
	testPoint1.setY(-25.668713000000000);
	pointVectorPublicRoute.push_back(testPoint1);

	
	testPoint1.setX(28.158352);
	testPoint1.setY(-25.659507);
	pointVectorPublicRoute.push_back(testPoint1);

	VectorOfPointVector vecLayerPoints;
	DoubleVector vecLength;

	long lPrPointIndex,lPrCount = pointVectorPublicRoute.size();
	for(lPrPointIndex = 0; lPrPointIndex < lPrCount - 1; lPrPointIndex++)
	{
		pointVector.clear();
		pointVectorResult1.clear();
		pointVector.push_back(pointVectorPublicRoute.at(lPrPointIndex));
		pointVector.push_back(pointVectorPublicRoute.at(lPrPointIndex + 1));
		g_appDataStore.m_GraphManager.GetShortestPath(pointVector,pointVectorResult1,dCost5);		
		vecLayerPoints.push_back(pointVectorResult1);
		vecLength.push_back(dCost5);
	}
	makeShape(vecLayerPoints,vecLength,"bus2");
}




void offlineTest()
{
	double dCost1,dCost2,dCost5;
	OGRPointVector pointVectorResult1,pointVectorResult2;
	OGRPoint testPoint1,testPoint2,testPoint3;
	OGRPointVector pointVectorPublicRoute;


	OGRRawPoint ogrpFirst = g_appDataStore.m_GraphManager.GetVertexVecPtr()->at(0).vertexPoint;
	long lNodeCount = g_appDataStore.m_GraphManager.GetVertexVecPtr()->size();
	OGRRawPoint ogrpLast = g_appDataStore.m_GraphManager.GetVertexVecPtr()->at(lNodeCount - 1).vertexPoint;

	//28.147040,-25.709601
	//28.180440,-25.687168
	//28.125397,-25.675000
	//28.130214,-25.667349
	//28.150225,-25.699898
	//28.133008,-25.694106
	//28.161362,-25.699531
	testPoint1.setX(28.161362);
	testPoint1.setY(-25.699531);		

	//No route for below point please check 
	//testPoint1.setX(28.180440);
	//testPoint1.setY(-25.687168);		
	pointVectorPublicRoute.push_back(testPoint1);

	VectorOfPointVector vecLayerPoints;
	DoubleVector vecLength;
	//28.151340,-25.653209
	//28.180219,-25.681692
	//28.200175,-25.684515
	//28.172558,-25.699329
	//28.143143,-25.667718
	testPoint1.setX(28.143143);
	testPoint1.setY(-25.667718);	

	//No route for below point please check 
	//testPoint1.setX(28.151340);
	//testPoint1.setY(-25.653209);		
	pointVectorPublicRoute.push_back(testPoint1);
	std::string strTest;
	//g_appDataStore.m_plainShortestPath.GetShortestPath(pointVectorPublicRoute,strTest);
//	g_appDataStore.m_GraphManager.GetBiDirectionalShortestPathAStar(pointVectorPublicRoute,pointVectorResult1,dCost1);
//	vecLayerPoints.push_back(pointVectorResult1);
//	vecLength.push_back(dCost1);
//	makeShape(vecLayerPoints,vecLength,"BiDirecAstar");
//	pointVectorResult1.clear();
//	vecLayerPoints.clear();
//	vecLength.clear();


	//	pointVectorResult1.clear();
	//

//	g_appDataStore.m_GraphManager.GetAccourateShortestPath(pointVectorPublicRoute,pointVectorResult1,dCost1);
//	vecLayerPoints.push_back(pointVectorResult1);
//	vecLength.push_back(dCost1);
//	makeShape(vecLayerPoints,vecLength,"Dijk");
//	pointVectorResult1.clear();
//	vecLayerPoints.clear();
//	vecLength.clear();

	
//	g_appDataStore.m_GraphManager.GetShortestPathAStar(pointVectorPublicRoute,pointVectorResult1,dCost1);
//	vecLayerPoints.push_back(pointVectorResult1);
//	vecLength.push_back(dCost1);
//	makeShape(vecLayerPoints,vecLength,"Astar");
//	pointVectorResult1.clear();
//	vecLayerPoints.clear();
//	vecLength.clear();


	g_appDataStore.m_GraphManager.GetAccourateShortestPathUsingPublic(pointVectorPublicRoute,235,pointVectorResult1,dCost1);
	vecLayerPoints.push_back(pointVectorResult1);
	vecLength.push_back(dCost1);
	makeShape(vecLayerPoints,vecLength,"Public");
}



void createTimeTableForPublicBus()
{
	//100,200,220,240,260,280,300,320,340,360,380,400,420,440,460,480,500,520,540,560,580,600,620,640,660,680,700,720,740,760,780,800,820,3000,4000,10000,10073
	int iTimeTable[] = {100,200,220,240,260,280,300,320,340,360,380,400,420,440,460,480,500,520,540,560,580,600,620,640,660,680,700,720,740,760,780,800,820,3000,4000,10000,10073};
	SHPHandle shapeHandle;
	//37
	std::string strShapePath = "D:\\output\\bus2.shp";
	shapeHandle = SHPOpen( strShapePath.c_str(),"rb");
	FILE* fpTimeOut = fopen("D:\\output\\time.txt","w");
	FILE* fpNodes = fopen("D:\\output\\node2.txt","w");

	int iRecCount = shapeHandle->nRecords;
	int iRecIndex;
	double dTotalLength = 0.00;
	for(iRecIndex = 0; iRecIndex < iRecCount; iRecIndex++)
	{
		RawPointVector vecPoints;
		SHPObject * pObj = SHPReadObject( shapeHandle, iRecIndex);
		int iCount = pObj->nVertices;
		int iPointIndex;

		for(iPointIndex = 0; iPointIndex < iCount; iPointIndex++)
		{
			OGRRawPoint rp;
			rp.x = pObj->padfX[iPointIndex];
			rp.y = pObj->padfY[iPointIndex];
			vecPoints.push_back(rp);
		}
		OGRRawPoint rps,rpe;
		rps.x =  pObj->padfX[0];
		rps.y =  pObj->padfY[0];
		rpe.x =  pObj->padfX[iCount - 1];
		rpe.y =  pObj->padfY[iCount - 1];


		fprintf(fpNodes,"[%lf,%lf]-[%lf,%lf]\n",rps.x,rps.y, rpe.x,rpe.y);
		

		double dLength = GeoMatryDefs::GetLength(vecPoints);		
		dTotalLength += dLength;
		int iTime = ceil(dTotalLength * 3);

		
		int iTimeIndex = 0;
		for(iTimeIndex = 0; iTimeIndex < 37; iTimeIndex++)
		{
			fprintf(fpTimeOut,"%d,",(iTimeTable[iTimeIndex] + iTime));
		}
		fprintf(fpTimeOut,"\n");
	}	
	fclose(fpTimeOut);
	fclose(fpNodes);
	
}


void separateShapeFeatures()
{

	CMassDataConverter massDataConv;
	massDataConv.setMBTABusStopPointList("D:\\tnew\\MBTABUSSTOPS_PT_LIST");
	massDataConv.setBusRoutePath("D:\\tnew\\MBTABUSROUTES_ARC");
	massDataConv.setMBTAPointListPath("D:\\tnew\\MBTABUSSTOPS_PT");
	massDataConv.setOutputPath("d:\\nt\\");	
	//massDataConv.separateShapes();
	massDataConv.separateShapesAndCreate();





	
	return;

	SHPHandle shapeHandle;
	shapeHandle = SHPOpen("D:\\t\\MBTABUSROUTES_ARC.shp","rb");
	std::string strShapeName = "D:\\SHPTest.shp";
//	SHPHandle shapeHandleNew;
//	DBFHandle dbfhandle;
//	shapeHandleNew = SHPCreate(strShapeName.c_str(),SHPT_ARC);
//	dbfhandle = DBFCreate("D:\\SHPTest.dbf");
//	int iRet = DBFAddField(dbfhandle,"TestF",FTString,80,0);
	

	int iRecCount = shapeHandle->nRecords;
	int iRecIndex;
	double dTotalLength = 0.00;
	for(iRecIndex = 0; iRecIndex < 1; iRecIndex++)
	{
		
		VectorOfPointVector vecShape;
		DoubleVector vecCost;

		SHPObject * pObj = SHPReadObject( shapeHandle, iRecIndex);
		int iCount = pObj->nVertices;
		int iPartCount = pObj->nParts;
		pObj->panPartStart;
		OGRPointVector vecPoints;

		for(int i = 0; i < iCount; i++)
		{
			OGRPoint ogrp;
			ogrp.setX(pObj->padfX[i]);
			ogrp.setY(pObj->padfY[i]);
			vecPoints.push_back(ogrp);
		}

		double dLength = 9.00;//GeoMatryDefs::GetLength(vecPoints);
		vecShape.push_back(vecPoints);
		vecCost.push_back(dLength);
		makeShape(vecShape,vecCost,"12");

//		int iWrite = SHPWriteObject( shapeHandleNew, -1, pObj );
//		DBFWriteStringAttribute(dbfhandle,0,0,"Test");
		SHPDestroyObject(pObj);
	}
	SHPClose(shapeHandle);
//	DBFClose(dbfhandle);
//	SHPClose(shapeHandleNew);
	
}


int main(void)
{
	OGRRegisterAll();
	
	

	OGRPoint testPoint1,testPoint2,testPoint3;
	std::string strReq;// =  "request=GetDD&routepoints=-131.763957,58.894665$-114.055304,49.024089";
	FILE* fp = fopen("request.txt","w");
	bool bIsLoaded = g_appDataStore.m_ConfigurationManager.ReadConfigurationFile();
	bool bIsDataLoaded = false;
	if(bIsLoaded)
	{		
		bIsDataLoaded = g_appDataStore.m_ConfigurationManager.ConfigureServer(&g_appDataStore.m_GraphManager, &g_appDataStore.m_GraphFileManager);		
	}
	else
	{
		fprintf(fp,"Wrong Configuration Format\n");	
		fflush(fp);
	}

	
	


	OGRPointVector pointVector;	
	OGRPointVector pointVector1,pointVectorResult;
	
	
	LongVector vecResultEdgeBGL,vecResultEdge,vecResultVertexBGL,vecResultVertex;

	


	////////////////////////////////////////////////////////////
//	if(g_appDataStore.m_ConfigurationManager.GetTaskType() != TaskTypeBuildGraphAndService)
//		return 1;
	
	int iReqCount = 0;	
	clock_t		startClock;
	clock_t		endClock;

	while(FCGI_Accept()>= 0)
	{
		RequestManager reqManager;
		RequestStruct reqStruct;	

		if (!getRequestString(strReq))
			continue;
		//strReq = "http:///server?request=GETPATHUSINGPUBLICROUTE&routepoints=28.165310,-25.699259$28.172155,-25.675418";
		//strReq = "http:///server?request=GETPATH&routepoints=28.169936,-25.668732$28.143029,-25.660414";
		int iPos = strReq.find("?");
		strReq = strReq.substr(iPos + 1, strReq.length());

		fprintf(fp,"[%d]%s\n",iReqCount,strReq.c_str());
		fflush(fp);
		reqManager.parseRequest(strReq,reqStruct);
		std::string strResponseB;
		startClock = clock();
		if(reqStruct.m_reqType == RequestTypeDrivingDirection && reqStruct.m_spGenerator == SPathBGL)
		{			
			g_appDataStore.m_textDDGenerator.GetDrivingDirectionBGL(reqStruct.m_ogrPointVector,"",strResponseB);			
		}
		else if(reqStruct.m_reqType == RequestTypeDrivingDirection && reqStruct.m_spGenerator == SPathDef)
		{			
			g_appDataStore.m_textDDGenerator.GetDrivingDirectionBGL(reqStruct.m_ogrPointVector,"",strResponseB);			
		}
		else if(reqStruct.m_reqType == RequestTypeShowShortestPath)
		{
			g_appDataStore.m_plainShortestPath.GetShortestPath(reqStruct.m_ogrPointVector,strResponseB);
		}
		else if(reqStruct.m_reqType == RequestTypeShowShortestPathUsingPublicRoute)
		{
			g_appDataStore.m_plainShortestPath.GetShortestPathUsingPublicTransport(reqStruct.m_ogrPointVector,100,strResponseB);
		}
		printf(xmlHeader, strResponseB.length(),strResponseB.c_str());
		endClock = clock();
		double dEndTime = ((double)(endClock - startClock))/((double)CLOCKS_PER_SEC);
		fprintf(fp,"Time[%lf]\n",dEndTime);
		fflush(fp);
		FCGI_Finish();
		
	}
	fclose(fp);
	return 0;
}

