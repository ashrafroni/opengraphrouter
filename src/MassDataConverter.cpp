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
#include "MassDataConverter.h"
#include "PointShapeCreator.h"
#include "GeometryDefinition.h"
#include "ogrsf_frmts.h"

static int nGroupTransactions = 200;
static int nFIDToFetch = OGRNullFID;
static int bPreserveFID = FALSE;
static int bSkipFailures = FALSE;


bool BusStopOrdering(const BusStopPointInfo& busStop1, const BusStopPointInfo& busStop2)
{
	return busStop1.lSequence < busStop2.lSequence;		
}


CMassDataConverter::CMassDataConverter(void)
{
}

CMassDataConverter::~CMassDataConverter(void)
{
}


void CMassDataConverter::setBusRoutePath(std::string strBusRoutePath)
{
	SHPHandle shapeHandle;
	DBFHandle dbfHandle;

	std::string strShpPath = strBusRoutePath + ".shp";
	std::string strDbfPath = strBusRoutePath + ".dbf";


	shapeHandle = SHPOpen(strShpPath.c_str(),"rb");
	dbfHandle = DBFOpen(strDbfPath.c_str(),"rb");


	int iFieldIndexRN = DBFGetFieldIndex(dbfHandle,"ROUTE_NUM");
	int iFieldIndexDIR = DBFGetFieldIndex(dbfHandle,"DIRECTION");
	int iFieldIndexBID = DBFGetFieldIndex(dbfHandle,"BUS_ID");
	int iFieldIndexRMAJ = DBFGetFieldIndex(dbfHandle,"ROUTE_MAJ");
	int iFieldIndexRDESC = DBFGetFieldIndex(dbfHandle,"ROUTE_DESC");


	int iRecCount = shapeHandle->nRecords;
	int iRecIndex;
	double dTotalLength = 0.00;
	for(iRecIndex = 0; iRecIndex < iRecCount; iRecIndex++)
	{

		SHPObject * pObj = SHPReadObject( shapeHandle, iRecIndex);
		int iCount = pObj->nVertices;
		int iPartCount = pObj->nParts;
		int iPartIndex;

		VectorOfRawPointVector vecRawPointVector;
		LongVector vecPosIndex;
		
		for(iPartIndex = 0; iPartIndex < iPartCount; iPartIndex++)		
			vecPosIndex.push_back(pObj->panPartStart[iPartIndex]);
		vecPosIndex.push_back(iCount);
//		if(iPartCount > 1)
//		{
//			int ipa = 0;
//		}

		RawPointVector vecAllPoints;
		
		for(iPartIndex = 0; iPartIndex < iPartCount; iPartIndex++)
		{

			int iStart = vecPosIndex[iPartIndex];
			int iEnd = vecPosIndex[iPartIndex + 1];
			RawPointVector vecRawPoints;
			for(int i = iStart; i < iEnd; i++)
			{
				OGRRawPoint ogrp;
				ogrp.x = pObj->padfX[i];
				ogrp.y = pObj->padfY[i];
				vecRawPoints.push_back(ogrp);
				vecAllPoints.push_back(ogrp);
			}
			vecRawPointVector.push_back(vecRawPoints);
		}
				



		SHPDestroyObject(pObj);
		BusRoute busRoute;
		busRoute.vecRoutePoints = vecAllPoints;
		busRoute.vecRawPointVector= vecRawPointVector;//vecRawPoints;
		busRoute.dRouteNumber = DBFReadDoubleAttribute(dbfHandle,iRecIndex,iFieldIndexRN);
		busRoute.iDirection = DBFReadIntegerAttribute(dbfHandle,iRecIndex,iFieldIndexDIR);
		busRoute.lBusID = DBFReadIntegerAttribute(dbfHandle,iRecIndex,iFieldIndexBID);
		busRoute.lRouteMaj = DBFReadIntegerAttribute(dbfHandle,iRecIndex,iFieldIndexRMAJ);
		busRoute.strDescription = DBFReadStringAttribute(dbfHandle,iRecIndex,iFieldIndexRDESC);

		m_busID2BusRouteDetails[busRoute.lBusID] = busRoute;


	}
	SHPClose(shapeHandle);
	DBFClose(dbfHandle);

	m_strBusRoutePath = strBusRoutePath;	
}
void CMassDataConverter::setMBTAPointListPath(std::string strMBTAPointList)
{
	m_strMBTAPointList = strMBTAPointList;	

	SHPHandle shapeHandle;
	DBFHandle dbfHandle;

	std::string strShpPath = m_strMBTAPointList + ".shp";
	std::string strDbfPath = m_strMBTAPointList + ".dbf";


	shapeHandle = SHPOpen(strShpPath.c_str(),"rb");
	dbfHandle = DBFOpen(strDbfPath.c_str(),"rb");


	int iFieldIndexSID = DBFGetFieldIndex(dbfHandle,"STOP_ID");
	int iFieldIndexSNAME = DBFGetFieldIndex(dbfHandle,"STOP_NAME");


	int iRecCount = shapeHandle->nRecords;
	int iRecIndex;
	double dTotalLength = 0.00;
	for(iRecIndex = 0; iRecIndex < iRecCount; iRecIndex++)
	{

		BusStopInfo busStopInfo;
		SHPObject * pObj = SHPReadObject( shapeHandle, iRecIndex);
		int iCount = pObj->nVertices;		
		OGRRawPoint ogrp;
		ogrp.x = pObj->padfX[0];
		ogrp.y = pObj->padfY[0];
		SHPDestroyObject(pObj);
		busStopInfo.ogrpStopPoint = ogrp;
		busStopInfo.lStopID= DBFReadIntegerAttribute(dbfHandle,iRecIndex,iFieldIndexSID);
		busStopInfo.strStopName = DBFReadStringAttribute(dbfHandle,iRecIndex,iFieldIndexSNAME);
		m_busStopInfo[busStopInfo.lStopID] = busStopInfo;
	}
	SHPClose(shapeHandle);
	DBFClose(dbfHandle);
}



void CMassDataConverter::setMBTABusStopPointList(std::string strMBTABusStopPointList)
{
	m_strMBTABusStopPointList = strMBTABusStopPointList + ".dbf";	
	DBFHandle dbfHandle = DBFOpen(m_strMBTABusStopPointList.c_str(),"r");
	int iFieldCount = dbfHandle->nFields;
	int iRecCount = DBFGetRecordCount(dbfHandle);
	int iFieldIndexRN = DBFGetFieldIndex(dbfHandle,"ROUTE_NUM");
	int iFieldIndexDIR = DBFGetFieldIndex(dbfHandle,"DIRECTION");
	int iFieldIndexBID = DBFGetFieldIndex(dbfHandle,"BUS_ID");
	int iFieldIndexSID = DBFGetFieldIndex(dbfHandle,"STOP_ID");
	int iFieldIndexSQN = DBFGetFieldIndex(dbfHandle,"SEQUENCE");

	int iRecIndex = 0;
	for(iRecIndex = 0; iRecIndex < iRecCount; iRecIndex++)
	{
		BusStopPointInfo busStopInfo;
		
		busStopInfo.dRouteNumber = DBFReadDoubleAttribute(dbfHandle,iRecIndex,iFieldIndexRN);
		busStopInfo.iDirection = DBFReadIntegerAttribute(dbfHandle,iRecIndex,iFieldIndexDIR);
		busStopInfo.lBusID = DBFReadIntegerAttribute(dbfHandle,iRecIndex,iFieldIndexBID);
		busStopInfo.lStopID = DBFReadIntegerAttribute(dbfHandle,iRecIndex,iFieldIndexSID);
		busStopInfo.lSequence = DBFReadIntegerAttribute(dbfHandle,iRecIndex,iFieldIndexSQN);

		BusStopPointVector* pBusStopVec = NULL;
		
		ID2BustopInfoVector::iterator itMap = m_busStopPointInfoStore.find(busStopInfo.lBusID);
		if(itMap == m_busStopPointInfoStore.end())
		{
			pBusStopVec = new BusStopPointVector();			
			m_busStopPointInfoStore[busStopInfo.lBusID] = pBusStopVec;
		}
		else
		{
			pBusStopVec = itMap->second;
		}
		pBusStopVec->push_back(busStopInfo);
	}
	
	ID2BustopInfoVector::iterator itMap = m_busStopPointInfoStore.begin();
	for(; itMap != m_busStopPointInfoStore.end(); itMap++)
	{
		BusStopPointVector* pBusStopVec = NULL;
		pBusStopVec = itMap->second;
		std::sort(pBusStopVec->begin(),pBusStopVec->end(),BusStopOrdering);
	}
	DBFClose(dbfHandle);

	
}
void CMassDataConverter::setMBTABusArcNameBDF(std::string strMBTABusArcNameBDF)
{
	m_strMBTABusArcNameBDF = strMBTABusArcNameBDF;
}

void CMassDataConverter::setOutputPath(std::string strOutputFolder)
{
	m_strOutputFolder = strOutputFolder;
}


int TranslateLayer( OGRDataSource *poSrcDS, 
				   OGRLayer * poSrcLayer,
				   OGRDataSource *poDstDS,
				   char **papszLCO,
				   const char *pszNewLayerName,
				   int bTransform, 
				   OGRSpatialReference *poOutputSRS,
				   OGRSpatialReference *poSourceSRS,
				   char **papszSelFields,
				   int bAppend, int eGType, int bOverwrite,
				   double dfMaxSegmentLength)

{
	OGRLayer    *poDstLayer;
	OGRFeatureDefn *poFDefn;
	OGRErr      eErr;
	int         bForceToPolygon = FALSE;
	int         bForceToMultiPolygon = FALSE;

	if( pszNewLayerName == NULL )
		pszNewLayerName = poSrcLayer->GetLayerDefn()->GetName();

	if( wkbFlatten(eGType) == wkbPolygon )
		bForceToPolygon = TRUE;
	else if( wkbFlatten(eGType) == wkbMultiPolygon )
		bForceToMultiPolygon = TRUE;

	/* -------------------------------------------------------------------- */
	/*      Setup coordinate transformation if we need it.                  */
	/* -------------------------------------------------------------------- */
	OGRCoordinateTransformation *poCT = NULL;

	if( bTransform )
	{
		if( poSourceSRS == NULL )
			poSourceSRS = poSrcLayer->GetSpatialRef();

		if( poSourceSRS == NULL )
		{
			exit( 1 );
		}

		CPLAssert( NULL != poSourceSRS );
		CPLAssert( NULL != poOutputSRS );

		poCT = OGRCreateCoordinateTransformation( poSourceSRS, poOutputSRS );
		if( poCT == NULL )
		{
			char        *pszWKT = NULL;

			poSourceSRS->exportToPrettyWkt( &pszWKT, FALSE );
			fprintf( stderr,  "Source:\n%s\n", pszWKT );

			poOutputSRS->exportToPrettyWkt( &pszWKT, FALSE );
			fprintf( stderr,  "Target:\n%s\n", pszWKT );
			exit( 1 );
		}
	}

	/* -------------------------------------------------------------------- */
	/*      Get other info.                                                 */
	/* -------------------------------------------------------------------- */
	poFDefn = poSrcLayer->GetLayerDefn();

	if( poOutputSRS == NULL )
		poOutputSRS = poSrcLayer->GetSpatialRef();

	/* -------------------------------------------------------------------- */
	/*      Find the layer.                                                 */
	/* -------------------------------------------------------------------- */
	int iLayer = -1;
	poDstLayer = NULL;

	for( iLayer = 0; iLayer < poDstDS->GetLayerCount(); iLayer++ )
	{
		OGRLayer        *poLayer = poDstDS->GetLayer(iLayer);

		if( poLayer != NULL 
			&& EQUAL(poLayer->GetLayerDefn()->GetName(),pszNewLayerName) )
		{
			poDstLayer = poLayer;
			break;
		}
	}

	/* -------------------------------------------------------------------- */
	/*      If the user requested overwrite, and we have the layer in       */
	/*      question we need to delete it now so it will get recreated      */
	/*      (overwritten).                                                  */
	/* -------------------------------------------------------------------- */
	if( poDstLayer != NULL && bOverwrite )
	{
		if( poDstDS->DeleteLayer( iLayer ) != OGRERR_NONE )
		{
			fprintf( stderr, 
				"DeleteLayer() failed when overwrite requested.\n" );
			return FALSE;
		}
		poDstLayer = NULL;
	}

	/* -------------------------------------------------------------------- */
	/*      If the layer does not exist, then create it.                    */
	/* -------------------------------------------------------------------- */
	if( poDstLayer == NULL )
	{
		if( eGType == -2 )
			eGType = poFDefn->GetGeomType();

		if( !poDstDS->TestCapability( ODsCCreateLayer ) )
		{
			fprintf( stderr, 
				"Layer %s not found, and CreateLayer not supported by driver.", 
				pszNewLayerName );
			return FALSE;
		}

		CPLErrorReset();

		poDstLayer = poDstDS->CreateLayer( pszNewLayerName, poOutputSRS,
			(OGRwkbGeometryType) eGType, 
			papszLCO );

		if( poDstLayer == NULL )
			return FALSE;

		bAppend = FALSE;
	}

	/* -------------------------------------------------------------------- */
	/*      Otherwise we will append to it, if append was requested.        */
	/* -------------------------------------------------------------------- */
	else if( !bAppend )
	{
		fprintf( stderr, "FAILED: Layer %s already exists, and -append not specified.\n"
			"        Consider using -append, or -overwrite.\n",
			pszNewLayerName );
		return FALSE;
	}
	else
	{
		if( CSLCount(papszLCO) > 0 )
		{
			fprintf( stderr, "WARNING: Layer creation options ignored since an existing layer is\n"
				"         being appended to.\n" );
		}
	}

	/* -------------------------------------------------------------------- */
	/*      Add fields.  Default to copy all field.                         */
	/*      If only a subset of all fields requested, then output only      */
	/*      the selected fields, and in the order that they were            */
	/*      selected.                                                       */
	/* -------------------------------------------------------------------- */
	int         iField;

	if (papszSelFields && !bAppend )
	{
		for( iField=0; papszSelFields[iField] != NULL; iField++)
		{
			int iSrcField = poFDefn->GetFieldIndex(papszSelFields[iField]);
			if (iSrcField >= 0)
				poDstLayer->CreateField( poFDefn->GetFieldDefn(iSrcField) );
			else
			{
				fprintf( stderr, "Field '%s' not found in source layer.\n", 
					papszSelFields[iField] );
				//				if( !bSkipFailures )
				//					return FALSE;
			}
		}
	}
	else if( !bAppend )
	{
		for( iField = 0; iField < poFDefn->GetFieldCount(); iField++ )
			poDstLayer->CreateField( poFDefn->GetFieldDefn(iField) );
	}

	/* -------------------------------------------------------------------- */
	/*      Transfer features.                                              */
	/* -------------------------------------------------------------------- */
	OGRFeature  *poFeature;
	int         nFeaturesInTransaction = 0;

	poSrcLayer->ResetReading();

	if( nGroupTransactions )
		poDstLayer->StartTransaction();

	while( TRUE )
	{
		OGRFeature      *poDstFeature = NULL;

		if( nFIDToFetch != OGRNullFID )
		{
			// Only fetch feature on first pass.
			if( nFeaturesInTransaction == 0 )
				poFeature = poSrcLayer->GetFeature(nFIDToFetch);
			else
				poFeature = NULL;
		}
		else
			poFeature = poSrcLayer->GetNextFeature();

		if( poFeature == NULL )
			break;

		if( ++nFeaturesInTransaction == nGroupTransactions )
		{
			poDstLayer->CommitTransaction();
			poDstLayer->StartTransaction();
			nFeaturesInTransaction = 0;
		}

		CPLErrorReset();
		poDstFeature = OGRFeature::CreateFeature( poDstLayer->GetLayerDefn() );

		if( poDstFeature->SetFrom( poFeature, TRUE ) != OGRERR_NONE )
		{
			if( nGroupTransactions )
				poDstLayer->CommitTransaction();

			CPLError( CE_Failure, CPLE_AppDefined,
				"Unable to translate feature %ld from layer %s.\n",
				poFeature->GetFID(), poFDefn->GetName() );

			OGRFeature::DestroyFeature( poFeature );
			OGRFeature::DestroyFeature( poDstFeature );
			return FALSE;
		}

		if( bPreserveFID )
			poDstFeature->SetFID( poFeature->GetFID() );

		if (poDstFeature->GetGeometryRef() != NULL && dfMaxSegmentLength > 0)
			poDstFeature->GetGeometryRef()->segmentize(dfMaxSegmentLength);

		if( poCT && poDstFeature->GetGeometryRef() != NULL )
		{
			eErr = poDstFeature->GetGeometryRef()->transform( poCT );
			if( eErr != OGRERR_NONE )
			{
				if( nGroupTransactions )
					poDstLayer->CommitTransaction();

				fprintf( stderr, "Failed to reproject feature %d (geometry probably out of source or destination SRS).\n", 
					(int) poFeature->GetFID() );
				if( !bSkipFailures )
				{
					OGRFeature::DestroyFeature( poFeature );
					OGRFeature::DestroyFeature( poDstFeature );
					return FALSE;
				}
			}
		}

		if( poDstFeature->GetGeometryRef() != NULL && bForceToPolygon )
		{
			poDstFeature->SetGeometryDirectly( 
				OGRGeometryFactory::forceToPolygon(
				poDstFeature->StealGeometry() ) );
		}

		if( poDstFeature->GetGeometryRef() != NULL && bForceToMultiPolygon )
		{
			poDstFeature->SetGeometryDirectly( 
				OGRGeometryFactory::forceToMultiPolygon(
				poDstFeature->StealGeometry() ) );
		}

		OGRFeature::DestroyFeature( poFeature );

		CPLErrorReset();
		if( poDstLayer->CreateFeature( poDstFeature ) != OGRERR_NONE 
			&& !bSkipFailures )
		{
			if( nGroupTransactions )
				poDstLayer->RollbackTransaction();

			OGRFeature::DestroyFeature( poDstFeature );
			return FALSE;
		}

		OGRFeature::DestroyFeature( poDstFeature );
	}

	if( nGroupTransactions )
		poDstLayer->CommitTransaction();

	/* -------------------------------------------------------------------- */
	/*      Cleaning                                                        */
	/* -------------------------------------------------------------------- */
	delete poCT;

	return TRUE;
}



bool CMassDataConverter::changeProjection(std::string strSourcePath, std::string strDestinationPath)
{


	const char  *pszFormat = "ESRI Shapefile";
	//int nGroupTransactions = 200;
	int nFIDToFetch = OGRNullFID;
	int bPreserveFID = FALSE;
	int bSkipFailures = FALSE;
	int         bTransform = TRUE;
	char        **papszDSCO = NULL, **papszLCO = NULL;
	OGRSpatialReference *poOutputSRS = NULL;
	OGRSpatialReference *poSourceSRS = NULL;
	char        **papszSelFields = NULL;
	int         bAppend = FALSE;
	int         eGType = -2;
	int			bOverwrite = FALSE;
	double       dfMaxSegmentLength = 0;


	OGRDataSource *poDS;
	OGRDataSource *poODS;	
	strSourcePath += ".shp";
	poDS = OGRSFDriverRegistrar::Open( strSourcePath.c_str());
	if( poDS == NULL )
	{
		return false;	

	}
	//strShapeFilePath = "d:\\tnew\\MBTABUSROUTES_ARC";
	std::string strOutPut = strDestinationPath + ".shp";
	std::string strDel = strDestinationPath + ".shp";
	//delete the existing files
	_unlink( strDel.c_str() );
	strDel = strDestinationPath + ".shx";
	_unlink( strDel.c_str() );
	strDel = strDestinationPath + ".dbf";
	_unlink( strDel.c_str() );
	strDel = strDestinationPath + ".prj";
	_unlink( strDel.c_str() );
	
	



	OGRSFDriverRegistrar *poR = OGRSFDriverRegistrar::GetRegistrar();
	OGRSFDriver          *poDriver = NULL;
	int                  iDriver;
	for( iDriver = 0; iDriver < poR->GetDriverCount() && poDriver == NULL; iDriver++)
	{
		if( EQUAL(poR->GetDriver(iDriver)->GetName(),pszFormat) )
		{
			poDriver = poR->GetDriver(iDriver);
		}
	}
	if( !poDriver->TestCapability( ODrCCreateDataSource ) )
	{
		return false;
	}
	//std::string strOutPut = "D:\\CWorks\\NazrulBhai\\prediction\\TestRoni.shp";


	//std::string strOutPut = "";

	poODS = poDriver->CreateDataSource( strOutPut.c_str(), NULL);
	if( poODS == NULL )
	{
		return false;
	}

	poOutputSRS = new OGRSpatialReference();

	poOutputSRS->SetGeogCS( "WGS84","WGS_1984","WGS84",SRS_WGS84_SEMIMAJOR, 
		SRS_WGS84_INVFLATTENING,"Greenwich", 0.0,"degree",0.0);


	for( int iLayer = 0; iLayer < poDS->GetLayerCount(); iLayer++ )
	{
		OGRLayer        *poLayer = poDS->GetLayer(iLayer);

		if( poLayer == NULL )
		{
			return false;
		}
		
		{		
			if( !TranslateLayer( poDS, poLayer, poODS, papszLCO, 
				strDestinationPath.c_str(), bTransform, poOutputSRS,
				poSourceSRS, papszSelFields, bAppend, eGType,
				bOverwrite, dfMaxSegmentLength ) 
				&& !bSkipFailures )
			{
				CPLError( CE_Failure, CPLE_AppDefined, 
					"Terminating translation prematurely after failed\n"
					"translation of layer %s (use -skipfailures to skip errors)\n", 
					poLayer->GetLayerDefn()->GetName() );

				exit( 1 );
			}
		}
	}
	//delete poOutputSRS;
	//delete poSourceSRS;
	delete poODS;
	delete poDS;

	CSLDestroy(papszSelFields);
	///	CSLDestroy( papszArgv );
	//CSLDestroy( papszLayers );
	CSLDestroy( papszDSCO );
	CSLDestroy( papszLCO );
	return true;
}


void CMassDataConverter::separateShapesAndCreate()
{

	long lFailCount = 0;
	long lSuccCount = 0;
	long lDistanceError = 0;
	long lMissBusStops = 0;


	long lToleranceDistance = 10;// in meter
	ID2BusRoute::iterator itMap = m_busID2BusRouteDetails.begin();
	for(; itMap != m_busID2BusRouteDetails.end(); itMap++)
	{
		long lBusID = itMap->first;
		RawPointVector vecRoutePoints;
		vecRoutePoints = itMap->second.vecRoutePoints;

		ID2BustopInfoVector::iterator itBusPointMap = m_busStopPointInfoStore.find(lBusID);
		if(itBusPointMap == m_busStopPointInfoStore.end())
			continue;
		
		BusStopPointVector* pBusStopPts = itBusPointMap->second;
		long lIndex,lPointCount = pBusStopPts->size();

		

		bool bIsBusStopMissing = false;
		bool bIsBusStopOuofRoad = false;


		VectorOfRawPointVector vecClippedShapeFeatures;
		DoubleVector vecLength;

		long lStartIndexForCut = 0;
		OGRRawPoint rpPrev;
		RawPointVector vecTransitPoints;
		vecTransitPoints.clear();
		//Cut the shape following the bus stop points
		for(lIndex = 0; lIndex < lPointCount ; lIndex++)	
		{

			long lStopID = pBusStopPts->at(lIndex).lStopID;
			ID2BusStopInfoMap::iterator itBusStopMap = m_busStopInfo.find(lStopID);
			if(itBusStopMap == m_busStopInfo.end())
			{
				//vecMissingBStops.push_back(lStopID);
				printf("Missing %ld,%ld\n",lBusID,lStopID);
				bIsBusStopMissing = true;
				lMissBusStops++;
				break;
			}
			OGRRawPoint ogrpBusStop = itBusStopMap->second.ogrpStopPoint;			

			long lStartIndex;
			OGRPoint ogrIntersectionPoint;
			OGRPoint ogrPoint;
			ogrPoint.setX(ogrpBusStop.x);
			ogrPoint.setY(ogrpBusStop.y);
			double dDistance = GeoMatryDefs::DistanceOfPolyLineFeatureFromAPoint(vecRoutePoints,ogrPoint,lStartIndex,ogrIntersectionPoint);
			if(dDistance * 1000 > lToleranceDistance)
			{
				printf("Distance > 0 %ld,%ld,%lf\n",lBusID,lStopID,dDistance);
				bIsBusStopOuofRoad = true;
				lDistanceError++;
				break;
			}
			vecTransitPoints.push_back(ogrpBusStop);
			
			rpPrev.x = ogrIntersectionPoint.getX();
			rpPrev.y = ogrIntersectionPoint.getY();

			if(lIndex == 0 )
				continue;

			OGRRawPoint ogrIPoint;
			ogrIPoint.x = ogrIntersectionPoint.getX();
			ogrIPoint.y = ogrIntersectionPoint.getY();

			RawPointVector vecPartialPoints;
			if(!(lIndex == 0 || lIndex == lPointCount - 1))
			{				
				long lInd = vecClippedShapeFeatures.size() - 1;
				if(lInd > 0)					
				{
					RawPointVector* pVec = &vecClippedShapeFeatures[lInd];
					lInd = pVec->size() - 1;
					rpPrev = pVec->at(lInd);
					vecPartialPoints.push_back(rpPrev);				
				}



			}
			GeoMatryDefs::GetPartialPartPoints(vecRoutePoints,lStartIndexForCut,lStartIndex,true,ogrIPoint,vecPartialPoints);
			double dPartialFeatureLength = GeoMatryDefs::GetLength(vecPartialPoints);
			vecLength.push_back(dPartialFeatureLength);
			vecClippedShapeFeatures.push_back(vecPartialPoints);
			lStartIndexForCut = lStartIndex + 1;
		}

		bool bIsSucc = !bIsBusStopMissing && !bIsBusStopOuofRoad;
		if(bIsBusStopMissing || bIsBusStopOuofRoad)
			continue;
		lSuccCount++;
		char chBusID[1000];
		CPointShapeCreator pointShape;
		sprintf(chBusID,"d:\\nt\\BusStopsForID%ld",lBusID);
		pointShape.createShape(chBusID);
		pointShape.addField("ID");
		pointShape.insertShape(vecTransitPoints);
		pointShape.close();
		sprintf(chBusID,"BusID%ld",lBusID);
		makePolylineShape(vecClippedShapeFeatures,vecLength,chBusID);
		
		
		
	}
	return;

}


void CMassDataConverter::separateShapes()
{

	FILE* fp = fopen("d:\\missingBusStop.txt","w");
	ID2BusRoute::iterator itMap = m_busID2BusRouteDetails.begin();
	for(; itMap != m_busID2BusRouteDetails.end(); itMap++)
	{
		long lBusID = itMap->first;
		VectorOfRawPointVector vecShape = itMap->second.vecRawPointVector;		
//		if(vecShape.size() < 2 )
//			continue;

//		if(lBusID == 799201)
//		{
//			int ibug = 0;
//		}
		ID2BustopInfoVector::iterator itBusPointMap = m_busStopPointInfoStore.find(lBusID);
		if(itBusPointMap == m_busStopPointInfoStore.end())
			continue;

		char chBusID[100];
		sprintf(chBusID,"%ld",lBusID);
		BusStopPointVector* pBusStopPts = itBusPointMap->second;
		

		CPointShapeCreator pointShape;
		sprintf(chBusID,"d:\\nt\\BusStopsForID%ld",lBusID);
		pointShape.createShape(chBusID);
		pointShape.addField("ID");
		long lIndex,lPointCount = pBusStopPts->size();
		LongVector vecMissingBStops;
		for(lIndex = 1; lIndex < lPointCount - 1; lIndex++)	
		{
			//BusStopPointInfo
			long lStopID = pBusStopPts->at(lIndex).lStopID;
			ID2BusStopInfoMap::iterator itBusStopMap = m_busStopInfo.find(lStopID);
			if(itBusStopMap == m_busStopInfo.end())
			{
				vecMissingBStops.push_back(lStopID);
				continue;
			}


			OGRRawPoint ogrpBusStop = itBusStopMap->second.ogrpStopPoint;			
			long lStartIndex;
			OGRPoint ogrIntersectionPoint;
			OGRPoint ogrPoint;
			ogrPoint.setX(ogrpBusStop.x);
			ogrPoint.setY(ogrpBusStop.y);
			double dDistance = GeoMatryDefs::DistanceOfPolyLineFeatureFromAPoint(vecShape[0],ogrPoint,lStartIndex,ogrIntersectionPoint);
			if(dDistance != 0)
			{
				continue;
			}

			pointShape.insertShape(ogrpBusStop,lIndex);
			int iuiu = 0;	
		}

		

		if(vecMissingBStops.size() > 0)
		{
			fprintf(fp,"Bus ID[%ld]\n",lBusID);
			fprintf(fp,"---------------\n");
			long lMBIndex;
			for(lMBIndex = 0; lMBIndex < vecMissingBStops.size(); lMBIndex++)	
			{
				long lStopID = vecMissingBStops[lMBIndex];
				fprintf(fp,"%ld\n",lStopID);
			}
			fprintf(fp,"---------------\n");
		}

		
		pointShape.close();
		DoubleVector vecDouble;
		
		int iFeatureIndex,iFeatureCount = vecShape.size();
		

		for(iFeatureIndex = 0; iFeatureIndex < iFeatureCount; iFeatureIndex++)
		{
			double dFeatureLength = GeoMatryDefs::GetLength(vecShape[iFeatureIndex]);
			vecDouble.push_back(dFeatureLength);
		}



		sprintf(chBusID,"BUSID%ld",lBusID);
		makePolylineShape(vecShape,vecDouble,chBusID);		
	}
	fclose(fp);
}


bool CMassDataConverter::makePolylineShape(VectorOfRawPointVector& vecShape,DoubleVector& vecCost,std::string strLayerFileName)
{
	SHPHandle shapeHandle;
	DBFHandle dbfHandle;
	std::string strLayerName = m_strOutputFolder + strLayerFileName;
	std::string strShapeName = m_strOutputFolder + strLayerFileName + std::string(".shp");

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
		RawPointVector* pVec =  &vecShape[lFeatureIndex];
		long lFeaturePointCount = pVec->size();	
		double * padfX = new double[lFeaturePointCount];
		double * padfY = new double[lFeaturePointCount];
		double * padfZ = new double[lFeaturePointCount];
		double * padfM = new double[lFeaturePointCount];

		long lPointIndex,lPointCount = pVec->size();
		for(lPointIndex = 0; lPointIndex < lPointCount; lPointIndex++)
		{
			double dx = pVec->at(lPointIndex).x;
			double dy = pVec->at(lPointIndex).y;
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
