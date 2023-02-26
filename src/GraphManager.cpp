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
#include "GraphManager.h"
#include "LabelManager.h"
#include "ShortestPathBGL.h"
#include "GeometryDefinition.h"
#include "GraphFileManager.h"
#include "CommonDefinition.h"


#include "utils.h"


#ifndef max
#define max(a,b)            (((a) > (b)) ? (a) : (b))
#endif

#ifndef min
#define min(a,b)            (((a) < (b)) ? (a) : (b))
#endif


GraphManager::GraphManager(void)
:rTreeQ(4, 2)
{
}

GraphManager::~GraphManager(void)
{
	
}

EdgeInfoVector* GraphManager::GetEdgeVecPtr()
{
	return &edgeInfoVector;
}

VertexInfoVector* GraphManager::GetVertexVecPtr()
{
	return &vertexInfoVector;
}


bool GraphManager::ReadDataSource(LayerInfoVector& vecLayerInfo)
{
	m_vecLayerInfo = vecLayerInfo;
	long lShapeIndex,lShapeCount = vecLayerInfo.size();
	long lTotalFeatureCount = 0;
	for(lShapeIndex = 0 ; lShapeIndex < lShapeCount; lShapeIndex++)
	{
		std::string strPath = vecLayerInfo[lShapeIndex].m_strInputPath + vecLayerInfo[lShapeIndex].m_strLayerName + ".shp";
		std::string strShapeFilePath = strPath;
		poDS = OGRSFDriverRegistrar::Open( strShapeFilePath.c_str(), FALSE );
		if( poDS == NULL )
		{
			exit( 1 );
		}	
		int iPosLen = strShapeFilePath.length() - strShapeFilePath.find_last_of(".");
		int iPosSlash = strShapeFilePath.find_last_of("\\");
		int iStrLen = strShapeFilePath.length();
		std::string strLayerName = strShapeFilePath.substr(iPosSlash + 1,iStrLen - iPosLen - iPosSlash - 1);
		OGRLayer  *poLayer;
		poLayer = poDS->GetLayerByName( strLayerName.c_str());		
		OGRLayer  *poLayer1 = poDS->GetLayerByName( strLayerName.c_str());		
		long lFeatureCount = 0;
		lFeatureCount = poLayer->GetFeatureCount();
		lTotalFeatureCount+= lFeatureCount;
		m_vecLayerFeatureCount.push_back(lFeatureCount);
		m_vecLayerNames.push_back(strLayerName);
	}
	
	int iLayerCount = poDS->GetLayerCount();

	pArcNodeInfo = new ArcNodeInfo[lTotalFeatureCount];
	ReadDataSourceAndConvert();
	return true;		
}



long GraphManager::GetOptimizationCostIndex(std::string strMinimizingParam)
{
	long lCostFieldIndex,lCostFieldCount = vecCostFields.size();
	for(lCostFieldIndex = 0; lCostFieldIndex < lCostFieldCount; lCostFieldIndex++)
	{
		if(vecCostFields[lCostFieldIndex].m_strFieldName.compare(strMinimizingParam) == 0)
			return lCostFieldIndex;
	}
	return -1;
}

StringVector GraphManager::getLayerNameVec()
{
	return m_vecLayerNames;
}

bool GraphManager::ReadDataSourceAndConvert()
{
	long lLayerOffset = 0;

	long lLayerIndex,lLayerCount = m_vecLayerNames.size();
	for(lLayerIndex = 0; lLayerIndex < lLayerCount; lLayerIndex++)
	{
		strLayerName = m_vecLayerNames[lLayerIndex];
		std::string strPath = m_vecLayerInfo[lLayerIndex].m_strInputPath + m_vecLayerInfo[lLayerIndex].m_strLayerName + ".shp";		
		std::string strShapeFilePath = strPath;
		poDS = OGRSFDriverRegistrar::Open( strShapeFilePath.c_str(), FALSE );
		if( poDS == NULL )
		{
			exit( 1 );
		}	

		int iPathSourceIndex = lLayerIndex;
		DataBaseParameters dbfParameter = m_vecLayerInfo[lLayerIndex].m_dbfParameters;
		
		int iLayerCount = poDS->GetLayerCount();
		OGRLayer  *poLayer;
		poLayer = poDS->GetLayerByName( strLayerName.c_str() );

		OGRFeature *poFeature;

		poLayer->ResetReading();

		long lLayerFeatureCount = poLayer->GetFeatureCount();
		this->featureCount += lLayerFeatureCount;


		///////////////////////////////
		OGRFeatureDefn *poFDefn = poLayer->GetLayerDefn();
		int iField;
		poFeature = poLayer->GetFeature(0);
		IntVector vecInteger;
		vecCostFields = dbfParameter.vecCost;

		for( iField = 0; iField < dbfParameter.vecCost.size(); iField++ )
		{
			int iFieldIndex = poFDefn->GetFieldIndex(dbfParameter.vecCost[iField].m_strFieldName.c_str());
			if(iFieldIndex != -1)
				vecInteger.push_back(iFieldIndex);
		}
		long lStreetNameFieldIndex = poFDefn->GetFieldIndex(dbfParameter.strStreetName.c_str());
		long lDirectionFieldIndex = poFDefn->GetFieldIndex(dbfParameter.strDirField.c_str());
		long lSegmentTimeFieldIndex = poFDefn->GetFieldIndex(dbfParameter.strSegmentSpeedFieldName.c_str());



		long featureIndex;
		for(featureIndex = 0; featureIndex < lLayerFeatureCount; featureIndex++)	
		{
			poFeature = poLayer->GetFeature(featureIndex);
			long lArcInfoIndex = lLayerOffset + featureIndex;

			OGRGeometry *poGeometry;
			poGeometry = poFeature->GetGeometryRef();

			if( poGeometry != NULL && wkbFlatten(poGeometry->getGeometryType()) == wkbLineString )	
			{
				OGRLineString *poLine = (OGRLineString *) poGeometry;
				int nPointCount = poLine->getNumPoints();
				OGRPoint poPoint;

				poLine->getPoint(0,&poPoint);

				pArcNodeInfo[lArcInfoIndex].startPoint.x = poPoint.getX();
				pArcNodeInfo[lArcInfoIndex].startPoint.y = poPoint.getY();

				poLine->getPoint(nPointCount - 1,&poPoint);
				pArcNodeInfo[lArcInfoIndex].endPoint.x = poPoint.getX();
				pArcNodeInfo[lArcInfoIndex].endPoint.y = poPoint.getY();
				pArcNodeInfo[lArcInfoIndex].vecFeaturePoints.reserve(nPointCount);
				pArcNodeInfo[lArcInfoIndex].dataSrcID = lLayerIndex;
				if(nPointCount > 1)
				{
					OGRPoint stPoint1,stPoint2,edPoint1,edPoint2;
					poLine->getPoint(0,&stPoint1);
					poLine->getPoint(1,&stPoint2);
					pArcNodeInfo[lArcInfoIndex].startAngle = GeoMatryDefs::GetAngle(stPoint1.getX(),stPoint1.getY(),stPoint2.getX(),stPoint2.getY());					

					poLine->getPoint(nPointCount-1,&edPoint1);
					poLine->getPoint(nPointCount-2,&edPoint2);
					pArcNodeInfo[lArcInfoIndex].endAngle = GeoMatryDefs::GetAngle(edPoint2.getX(),edPoint2.getY(),edPoint1.getX(),edPoint1.getY());

					long lPointIndex;
					for(lPointIndex = 0; lPointIndex < nPointCount; lPointIndex++)
					{
						poLine->getPoint(lPointIndex,&stPoint1);
						OGRRawPoint rPoint;
						rPoint.x = stPoint1.getX();
						rPoint.y = stPoint1.getY();
						pArcNodeInfo[lArcInfoIndex].vecFeaturePoints.push_back(rPoint);

					}
					if(dbfParameter.bIsLengthAuto)
					{
						double dLength = GeoMatryDefs::GetLength(pArcNodeInfo[lArcInfoIndex].vecFeaturePoints);
						pArcNodeInfo[lArcInfoIndex].costVector.push_back(dLength);

						if(m_vecLayerInfo[lLayerIndex].m_dFairCostPerKm > 0.0000)
							pArcNodeInfo[lArcInfoIndex].costVector.push_back(dLength * m_vecLayerInfo[lLayerIndex].m_dFairCostPerKm);						
					}
					
				}					


				pArcNodeInfo[lArcInfoIndex].edgeGeomatryID = featureIndex;
				pArcNodeInfo[lArcInfoIndex].dataSrcID = iPathSourceIndex;

				pArcNodeInfo[lArcInfoIndex].costVector.reserve(vecInteger.size());
				for( iField = 0; iField < vecInteger.size(); iField++ )
				{
					OGRFieldDefn *poFieldDefn = poFDefn->GetFieldDefn( vecInteger[iField]);
					if( poFieldDefn->GetType() == OFTReal )
					{
						double dValue = poFeature->GetFieldAsDouble(vecInteger[iField]);
						pArcNodeInfo[lArcInfoIndex].costVector.push_back(dValue);					
					}
				}

				//
				if(lStreetNameFieldIndex  > -1)
				{
					std::string strName = poFeature->GetFieldAsString(lStreetNameFieldIndex);
					strName = StringOperation::TRIMWHITESPACE(strName);
					pArcNodeInfo[lArcInfoIndex].strStreetName = strName;
				}

				if(lDirectionFieldIndex > -1)
				{
					std::string strDirValue =  poFeature->GetFieldAsString(lDirectionFieldIndex);
					strDirValue = StringOperation::TRIMWHITESPACE(strDirValue);	
					if(strDirValue.compare(dbfParameter.strForwardDIRVal) == 0)
					{
						pArcNodeInfo[lArcInfoIndex].direction = ForwardDirection;
					}
					else if(strDirValue.compare(dbfParameter.strReverseDIRVal) == 0)
					{
						pArcNodeInfo[lArcInfoIndex].direction = ReverseDirection;
					}
					else if(strDirValue.compare(dbfParameter.strBothwayDIRVal) == 0)
					{
						pArcNodeInfo[lArcInfoIndex].direction = BothDirection;
					}

				}
				else 
					pArcNodeInfo[lArcInfoIndex].direction = BothDirection;

				if(lSegmentTimeFieldIndex > -1)
				{
					pArcNodeInfo[lArcInfoIndex].segmentSpeedValue = poFeature->GetFieldAsDouble(lSegmentTimeFieldIndex);
				}
				else
					pArcNodeInfo[lArcInfoIndex].segmentSpeedValue = 0.00;

			}
			OGRFeature::DestroyFeature( poFeature );
			
		}
		lLayerOffset += m_vecLayerFeatureCount[lLayerIndex];
	}
	OGRDataSource::DestroyDataSource( poDS );
	return true;
}

bool GraphManager::BuildRTree()
{
	long lEdgeIndex,lEdgeCount = edgeInfoVector.size();
	for(lEdgeIndex = 0; lEdgeIndex < lEdgeCount; lEdgeIndex++)
	{
		//Rtree Insertion

		long lFeaturePointCount = edgeInfoVector[lEdgeIndex].vecFeaturePoints.size();
		if(edgeInfoVector[lEdgeIndex].sourceLayerIndex != 0)
			continue;

		if(lFeaturePointCount < 2)
			continue;
		MapExtent mapExtent;
		long lPointIndex;
		for(lPointIndex = 0; lPointIndex < lFeaturePointCount; lPointIndex++)
		{
			OGRRawPoint rawPoint;
			rawPoint = edgeInfoVector[lEdgeIndex].vecFeaturePoints.at(lPointIndex);			
			mapExtent.unionwith(rawPoint);
		}
		
		OGRRawPoint stPoint1,stPoint2,edPoint1,edPoint2;
		OGRPoint point1,point2;
		double dXMax,dXMin,dYMax,dYMin;

		
		geometry::point_xy<double> startingPoint(mapExtent.dXmin,mapExtent.dYmin);
		geometry::point_xy<double> endingPoint(mapExtent.dXmax,mapExtent.dYmax);

		//Bounding box of the polyline
		geometry::box<geometry::point_xy<double>> bbox(startingPoint,endingPoint);

		long lEdgeLabel = edgeInfoVector[lEdgeIndex].edgeLabel;
		//inserting the bbox
		rTreeQ.insert(bbox,lEdgeLabel);
	}


	return true;
}


bool GraphManager::GetClosestVertexIndex(OGRPoint pointSelected,RoutingPointInfo& routingPointInfo,long& lGraphIndex,double dSearchRadius)
{
	long selectedVertexIndex;
	//Test Code delete under:AH
	double searchRadius = dSearchRadius;
	double latThreshodDistance = searchRadius/g_latDistPerDeg;
	double longThreshodDistance = searchRadius/g_lonDistPerDeg;

	OGRPoint searchExtentPoint1,searchExtentPoint2;


	searchExtentPoint1.setX(pointSelected.getX() - 2 * longThreshodDistance);
	searchExtentPoint1.setY(pointSelected.getY() - 2 * latThreshodDistance);

	searchExtentPoint2.setX(pointSelected.getX() + 2 * longThreshodDistance);
	searchExtentPoint2.setY(pointSelected.getY() + 2 * latThreshodDistance);

	geometry::point_xy<double> upperleftPoint(searchExtentPoint1.getX(),searchExtentPoint1.getY());
	geometry::point_xy<double> bottomRight(searchExtentPoint2.getX(),searchExtentPoint2.getY());

	geometry::box<geometry::point_xy<double> > bboxTest(upperleftPoint,bottomRight);
	std::deque< unsigned int > qClosestEdges = rTreeQ.find(bboxTest);


	unsigned int i = 0;	
	std::deque<unsigned int>::const_iterator dit = qClosestEdges.begin();
	OGRPoint ogrIntersectionPoint;

	double lowestDistance = DBL_MAX;
	long edgeIndex = -1;
	long lStartPointIndex;
	for( ;dit != qClosestEdges.end(); ++dit) 
	{
		long lStartPointIndexTemp;
		long edgeLabel = (long)*dit;
		OGRPoint ogrTempInPoint;
		double tempDistance = GeoMatryDefs::DistanceOfPolyLineFeatureFromAPoint(edgeInfoVector[edgeLabel].vecFeaturePoints,pointSelected,lStartPointIndexTemp,ogrTempInPoint);
		if(lowestDistance > tempDistance)
		{
			lowestDistance = tempDistance;
			edgeIndex = edgeLabel;
			ogrIntersectionPoint = ogrTempInPoint;
			lStartPointIndex = lStartPointIndexTemp;
		}
	}
	if(edgeIndex < 0)
		return false;	

	OGRPoint startingPoint,endingPoint;

	routingPointInfo.lFeatureIndex = edgeIndex;
	//routingPointInfo.selectedPoint = ogrIntersectionPoint;
	routingPointInfo.selectedPoint.setX(ogrIntersectionPoint.getX());
	routingPointInfo.selectedPoint.setY(ogrIntersectionPoint.getY());
	lGraphIndex = edgeInfoVector[edgeIndex].graphIndex;
	double dDividentPercent;
	GeoMatryDefs::CalculateWeightSeparation(edgeInfoVector[edgeIndex].vecFeaturePoints,ogrIntersectionPoint,lStartPointIndex,lStartPointIndex + 1,dDividentPercent);
	routingPointInfo.dPartialValuePercent = dDividentPercent;
	return true;
}

bool GraphManager::GetClosestEdgeIndex(OGRPoint pointSelected,RoutingPointInfo& routingPointInfo,long& lGraphIndex,double dSearchRadius)
{
	long selectedVertexIndex;
	//Test Code delete under:AH
	double searchRadius = dSearchRadius * 5;
	double latThreshodDistance = searchRadius/g_latDistPerDeg;
	double longThreshodDistance = searchRadius/g_lonDistPerDeg;

	OGRPoint searchExtentPoint1,searchExtentPoint2;


	searchExtentPoint1.setX(pointSelected.getX() - 2 * longThreshodDistance);
	searchExtentPoint1.setY(pointSelected.getY() - 2 * latThreshodDistance);

	searchExtentPoint2.setX(pointSelected.getX() + 2 * longThreshodDistance);
	searchExtentPoint2.setY(pointSelected.getY() + 2 * latThreshodDistance);

	geometry::point_xy<double> upperleftPoint(searchExtentPoint1.getX(),searchExtentPoint1.getY());
	geometry::point_xy<double> bottomRight(searchExtentPoint2.getX(),searchExtentPoint2.getY());

	geometry::box<geometry::point_xy<double> > bboxTest(upperleftPoint,bottomRight);
	std::deque< unsigned int > qClosestEdges = rTreeQ.find(bboxTest);


	unsigned int i = 0;	
	std::deque<unsigned int>::const_iterator dit = qClosestEdges.begin();

	double lowestDistance = DBL_MAX;
	long edgeIndex = -1;
	for( ;dit != qClosestEdges.end(); ++dit) 
	{

		long edgeLabel = (long)*dit;
		long lStartPointIndex;
		OGRPoint intersectionPoint;
		double tempDistance = GeoMatryDefs::DistanceOfPolyLineFeatureFromAPoint(edgeInfoVector[edgeLabel].vecFeaturePoints,pointSelected,lStartPointIndex,intersectionPoint);
		if(lowestDistance > tempDistance)
		{
			lowestDistance = tempDistance;
			edgeIndex = edgeLabel;
			routingPointInfo.lStartingPointIndex = lStartPointIndex;
			routingPointInfo.intersectionPoint = intersectionPoint;
			routingPointInfo.selectedPoint = pointSelected;
		}
	}
	if(edgeIndex < 0)
		return false;	


	OGRPoint ogrIntersectionPoint;
	routingPointInfo.lFeatureIndex = edgeIndex;
	
	double dDividentPercent;
	GeoMatryDefs::CalculateWeightSeparation(edgeInfoVector[edgeIndex].vecFeaturePoints,routingPointInfo.intersectionPoint,routingPointInfo.lStartingPointIndex,routingPointInfo.lStartingPointIndex + 1,dDividentPercent);

	routingPointInfo.dPartialValuePercent = dDividentPercent;
	lGraphIndex = edgeInfoVector[edgeIndex].graphIndex;
	
	//lEdgeIndex = edgeIndex;

	return true;

}

bool GraphManager::GetClosestVertexIndex(OGRPoint pointSelected,long &vertexIndex,long &lEdgeIndex, long& lGraphIndex ,double dSearchRadius)
{
	long selectedVertexIndex;
	//Test Code delete under:AH
	double searchRadius = dSearchRadius;
	double latThreshodDistance = searchRadius/g_latDistPerDeg;
	double longThreshodDistance = searchRadius/g_lonDistPerDeg;

	OGRPoint searchExtentPoint1,searchExtentPoint2;


	searchExtentPoint1.setX(pointSelected.getX() - 2 * longThreshodDistance);
	searchExtentPoint1.setY(pointSelected.getY() - 2 * latThreshodDistance);

	searchExtentPoint2.setX(pointSelected.getX() + 2 * longThreshodDistance);
	searchExtentPoint2.setY(pointSelected.getY() + 2 * latThreshodDistance);

	geometry::point_xy<double> upperleftPoint(searchExtentPoint1.getX(),searchExtentPoint1.getY());
	geometry::point_xy<double> bottomRight(searchExtentPoint2.getX(),searchExtentPoint2.getY());

	geometry::box<geometry::point_xy<double> > bboxTest(upperleftPoint,bottomRight);
	std::deque< unsigned int > qClosestEdges = rTreeQ.find(bboxTest);


	unsigned int i = 0;	
	std::deque<unsigned int>::const_iterator dit = qClosestEdges.begin();

	double lowestDistance = DBL_MAX;
	long edgeIndex = -1;
	for( ;dit != qClosestEdges.end(); ++dit) 
	{

		long edgeLabel = (long)*dit;
		double tempDistance = GeoMatryDefs::DistanceOfPolyLineFeatureFromAPoint(edgeInfoVector[edgeLabel].vecFeaturePoints,pointSelected);
		if(lowestDistance > tempDistance)
		{
			lowestDistance = tempDistance;
			edgeIndex = edgeLabel;
		}
	}
	if(edgeIndex < 0)
		return false;	

	OGRPoint startingPoint,endingPoint;
	
	
	long lPointCount = edgeInfoVector[edgeIndex].vecFeaturePoints.size();	

	startingPoint.setX(edgeInfoVector[edgeIndex].vecFeaturePoints.at(0).x);
	startingPoint.setY(edgeInfoVector[edgeIndex].vecFeaturePoints.at(0).y);

	endingPoint.setX(edgeInfoVector[edgeIndex].vecFeaturePoints.at(lPointCount - 1).x);
	endingPoint.setY(edgeInfoVector[edgeIndex].vecFeaturePoints.at(lPointCount - 1).y);
	
	
	double dStartNodeDistance = GeoMatryDefs::GetPointDistance(startingPoint,pointSelected);
	double dEndNodeDistance = GeoMatryDefs::GetPointDistance(endingPoint,pointSelected);

	if(dStartNodeDistance < dEndNodeDistance)
	{
		selectedVertexIndex = edgeInfoVector[edgeIndex].startNodeLabel;
	}
	else
	{
		selectedVertexIndex = edgeInfoVector[edgeIndex].endNodeLabel;
	}
	lGraphIndex = edgeInfoVector[edgeIndex].graphIndex;
	vertexIndex = selectedVertexIndex;
	lEdgeIndex = edgeIndex;

	return true;
}


void GraphManager::WriteToFile(std::string strFilePath)
{
	long featureIndex;
	FILE* fp = fopen("c:\\NodeArcList.txt","w");

	for(featureIndex = 0; featureIndex < featureCount; featureIndex++)
	{

		fprintf(fp,"[%lf,%lf],[%lf,%lf],%lf,%lf,%ld,%ld\n",
			pArcNodeInfo[featureIndex].startPoint.x,pArcNodeInfo[featureIndex].startPoint.y,
			pArcNodeInfo[featureIndex].endPoint.x,pArcNodeInfo[featureIndex].endPoint.y,
			pArcNodeInfo[featureIndex].startAngle,pArcNodeInfo[featureIndex].endAngle,
			pArcNodeInfo[featureIndex].edgeGeomatryID,pArcNodeInfo[featureIndex].dataSrcID);

	}
	fclose(fp);

	printf("NodeArc writing Finished\n");
}


bool GraphManager::SaveGraph()
{


	long nEdgeCount = edgeInfoVector.size();

	FILE* fpEdge = fopen("c:\\edge.txt","w");
	long lEdgeIndex;
	fprintf(fpEdge,"edgeLabel,sourceLayerIndex,edgeGeomatryID,graphIndex,startNodeLabel,endNodeLabel\n");
	for(lEdgeIndex = 0; lEdgeIndex < nEdgeCount; lEdgeIndex++)
	{
		fprintf(fpEdge,"[%ld,%ld,%ld,%ld],[%ld,%ld]\n",edgeInfoVector[lEdgeIndex].edgeLabel,edgeInfoVector[lEdgeIndex].sourceLayerIndex,
			edgeInfoVector[lEdgeIndex].edgeGeomatryID,edgeInfoVector[lEdgeIndex].graphIndex,
			edgeInfoVector[lEdgeIndex].startNodeLabel,edgeInfoVector[lEdgeIndex].endNodeLabel);
	}
	fclose(fpEdge);
	FILE* fpVertex = fopen("c:\\vertex.txt","w");
	long lVertexIndex,lVertexCount = vertexInfoVector.size();
	fprintf(fpVertex,"vertexLabel,[vertexPoint.x,vertexPoint.y],graphIndex,ConnectedEdgeIndex(s)\n");
	for(lVertexIndex = 0; lVertexIndex < lVertexCount; lVertexIndex++)
	{
		

		//AH:Error
		//fprintf(fpVertex,"%ld,[%lf,%lf],%ld,",vertexInfoVector[lVertexIndex].vertexLabel,vertexInfoVector[lVertexIndex].vertexPoint.x,vertexInfoVector[lVertexIndex].vertexPoint.y,vertexInfoVector[lVertexIndex].graphIndex);
		long lCEdgeLabelIndex,lCEdgeLabelCount = vertexInfoVector[lVertexIndex].connectedEdgeLabel.size();	

		fprintf(fpVertex,"[");
		for(lCEdgeLabelIndex = 0;lCEdgeLabelIndex < lCEdgeLabelCount; lCEdgeLabelIndex++)
		{
			fprintf(fpVertex,"%ld,",vertexInfoVector[lVertexIndex].connectedEdgeLabel[lCEdgeLabelIndex]);
		}
		fprintf(fpVertex,"]");

		fprintf(fpVertex,"[");
		for(lCEdgeLabelIndex = 0;lCEdgeLabelIndex < lCEdgeLabelCount; lCEdgeLabelIndex++)
		{
			long lEdgeL = vertexInfoVector[lVertexIndex].connectedEdgeLabel[lCEdgeLabelIndex];
			fprintf(fpVertex,"%ld,%ld,",edgeInfoVector[lEdgeL].startNodeLabel,edgeInfoVector[lEdgeL].endNodeLabel);
		}
		fprintf(fpVertex,"]");

		fprintf(fpVertex,"\n");
	}
	fclose(fpVertex);

	//printf("Graph Saving finished\n");

	return true;

} 

bool GraphManager::GetShortestPathBGL(OGRPointVector& vecRoutingViaPoints, LongVector& vecEdgeIndex ,double& dAccCost)
{
	long lPointCount = vecRoutingViaPoints.size();
	if(lPointCount < 2)
		return false;

	long lViaPointIndex;
	double dMaxAllowableDistance = 0.01;
	LongVector vecVertex;
	LongVector vecGraph;
	for(lViaPointIndex = 0; lViaPointIndex < lPointCount; lViaPointIndex++)
	{
		long lVertexIndex,lEdgeIndex,lGraphIndex;
		bool bIsGotVerTexIndex = GetClosestVertexIndex(vecRoutingViaPoints[lViaPointIndex],lVertexIndex,lEdgeIndex,lGraphIndex,dMaxAllowableDistance);
		if(!bIsGotVerTexIndex) 
			return false;
		vecGraph.push_back(lGraphIndex);
		vecVertex.push_back(lVertexIndex);
		if(vecGraph.size() > 1 && vecGraph[lViaPointIndex] != vecGraph[lViaPointIndex - 1])
		{
			return false;
		}
	}

	if(!bgl.GetShortestPath(vecVertex,vecEdgeIndex,dAccCost))
		return false;

	return true;
}

bool GraphManager::GetShortestPathBGLVertex(OGRPointVector& vecRoutingViaPoints, LongVector& vecVertexIndex,double& dAccCost)
{
	long lPointCount = vecRoutingViaPoints.size();
	if(lPointCount < 2)
		return false;

	long lViaPointIndex;
	double dMaxAllowableDistance = 0.01;
	LongVector vecVertex;
	LongVector vecGraph;
	for(lViaPointIndex = 0; lViaPointIndex < lPointCount; lViaPointIndex++)
	{
		long lVertexIndex,lEdgeIndex,lGraphIndex;
		bool bIsGotVerTexIndex = GetClosestVertexIndex(vecRoutingViaPoints[lViaPointIndex],lVertexIndex,lEdgeIndex,lGraphIndex,dMaxAllowableDistance);
		if(!bIsGotVerTexIndex) 
			return false;
		vecGraph.push_back(lGraphIndex);
		vecVertex.push_back(lVertexIndex);
		if(vecGraph.size() > 1 && vecGraph[lViaPointIndex] != vecGraph[lViaPointIndex - 1])
		{
			return false;
		}
	}

	if(!bgl.GetShortestPathVertex(vecVertex,vecVertexIndex,dAccCost))
		return false;	
	return true;
}


bool GraphManager::GetShortestPathBGL(OGRPointVector& vecRoutingViaPoints, OGRPointVector& vecRoutingPoints,double& dAccCost)
{
	LongVector vecEdgeIndex;
	if(!GetShortestPathBGL(vecRoutingViaPoints,vecEdgeIndex,dAccCost))
		return false;
	if(!GetFaturePoints(vecEdgeIndex,vecRoutingPoints))
		return false;
	return true;
}


bool GraphManager::GetShortestPath(OGRPointVector& vecRoutingViaPoints, LongVector& vecEdgeIndex ,double& dAccCost)
{
	long lPointCount = vecRoutingViaPoints.size();
	if(lPointCount < 2)
		return false;

	long lViaPointIndex;
	double dMaxAllowableDistance = 0.01;
	LongVector vecVertex;
	LongVector vecGraph;
	for(lViaPointIndex = 0; lViaPointIndex < lPointCount; lViaPointIndex++)
	{
		long lVertexIndex,lEdgeIndex,lGraphIndex;
		bool bIsGotVerTexIndex = GetClosestVertexIndex(vecRoutingViaPoints[lViaPointIndex],lVertexIndex,lEdgeIndex,lGraphIndex,dMaxAllowableDistance);
		if(!bIsGotVerTexIndex) 
			return false;
		vecGraph.push_back(lGraphIndex);
		vecVertex.push_back(lVertexIndex);
		if(vecGraph.size() > 1 && vecGraph[lViaPointIndex] != vecGraph[lViaPointIndex - 1])
		{
			return false;
		}
	}

	if(!defDjk.GetShortestPath(vecVertex,vecEdgeIndex,dAccCost))
		return false;

	return true;
}


bool GraphManager::GetAccourateShortestPathUsingPublic(OGRPointVector& vecRoutingViaPoints,long lStartingTime, OGRPointVector& vecRoutingPoints,double& dAccCost)
{
	long lPointCount = vecRoutingViaPoints.size();
	if(lPointCount < 2)
		return false;

	long lViaPointIndex;
	double dMaxAllowableDistance = 0.01;
	LongVector vecVertex;
	LongVector vecGraph;
	RoutingPointInfoVector vecRPoint;
	for(lViaPointIndex = 0; lViaPointIndex < lPointCount; lViaPointIndex++)
	{
		long lVertexIndex,lEdgeIndex,lGraphIndex;
		RoutingPointInfo routingPointInfo;
		bool bIsGotVerTexIndex = GetClosestEdgeIndex(vecRoutingViaPoints[lViaPointIndex],routingPointInfo,lGraphIndex,dMaxAllowableDistance);
		if(!bIsGotVerTexIndex) 
			return false;
		vecGraph.push_back(lGraphIndex);
		vecVertex.push_back(lVertexIndex);
		if(vecGraph.size() > 1 && vecGraph[lViaPointIndex] != vecGraph[lViaPointIndex - 1])
		{
			return false;
		}
		vecRPoint.push_back(routingPointInfo);
	}

	OGRPointVector vecStartingPartialPart;
	OGRPointVector vecEndingPartialPart;
	LongVector vecFeatureIndex;

	if(!defDjk.GetAccShortestPathPublic(vecRPoint[0],vecRPoint[1],lStartingTime,vecStartingPartialPart,vecEndingPartialPart,vecFeatureIndex,dAccCost))
		return false;

	if(!GetFaturePoints(vecFeatureIndex,vecStartingPartialPart,vecEndingPartialPart,vecRoutingPoints))
		return false;
	return true;
}

bool GraphManager::GetBiDirectionalShortestPathAStar(OGRPointVector& vecRoutingViaPoints, OGRPointVector& vecRoutingPoints,double& dAccCost)
{
	long lPointCount = vecRoutingViaPoints.size();
	if(lPointCount < 2)
		return false;

	long lViaPointIndex;
	double dMaxAllowableDistance = 0.01;
	LongVector vecVertex;
	LongVector vecGraph;
	RoutingPointInfoVector vecRPoint;
	for(lViaPointIndex = 0; lViaPointIndex < lPointCount; lViaPointIndex++)
	{
		long lVertexIndex,lEdgeIndex,lGraphIndex;
		RoutingPointInfo routingPointInfo;
		bool bIsGotVerTexIndex = GetClosestEdgeIndex(vecRoutingViaPoints[lViaPointIndex],routingPointInfo,lGraphIndex,dMaxAllowableDistance);
		if(!bIsGotVerTexIndex) 
			return false;
		vecGraph.push_back(lGraphIndex);
		vecVertex.push_back(lVertexIndex);
		if(vecGraph.size() > 1 && vecGraph[lViaPointIndex] != vecGraph[lViaPointIndex - 1])
		{
			return false;
		}
		vecRPoint.push_back(routingPointInfo);
	}

	OGRPointVector vecStartingPartialPart;
	OGRPointVector vecEndingPartialPart;
	LongVector vecFeatureIndex;

	if(!biDirectional.GetAccShortestPath(vecRPoint[0],vecRPoint[1],vecStartingPartialPart,vecEndingPartialPart,vecFeatureIndex,dAccCost))
		return false;

	if(!GetFaturePoints(vecFeatureIndex,vecStartingPartialPart,vecEndingPartialPart,vecRoutingPoints))
		return false;
	return true;	

}
bool GraphManager::GetShortestPathAStar(OGRPointVector& vecRoutingViaPoints, OGRPointVector& vecRoutingPoints,double& dAccCost)
{
	long lPointCount = vecRoutingViaPoints.size();
	if(lPointCount < 2)
		return false;

	long lViaPointIndex;
	double dMaxAllowableDistance = 0.01;
	LongVector vecVertex;
	LongVector vecGraph;
	RoutingPointInfoVector vecRPoint;
	for(lViaPointIndex = 0; lViaPointIndex < lPointCount; lViaPointIndex++)
	{
		long lVertexIndex,lEdgeIndex,lGraphIndex;
		RoutingPointInfo routingPointInfo;
		bool bIsGotVerTexIndex = GetClosestEdgeIndex(vecRoutingViaPoints[lViaPointIndex],routingPointInfo,lGraphIndex,dMaxAllowableDistance);
		if(!bIsGotVerTexIndex) 
			return false;
		vecGraph.push_back(lGraphIndex);
		vecVertex.push_back(lVertexIndex);
		if(vecGraph.size() > 1 && vecGraph[lViaPointIndex] != vecGraph[lViaPointIndex - 1])
		{
			return false;
		}
		vecRPoint.push_back(routingPointInfo);
	}

	OGRPointVector vecStartingPartialPart;
	OGRPointVector vecEndingPartialPart;
	LongVector vecFeatureIndex;

	if(!astar.GetAccShortestPath(vecRPoint[0],vecRPoint[1],vecStartingPartialPart,vecEndingPartialPart,vecFeatureIndex,dAccCost))
		return false;

	if(!GetFaturePoints(vecFeatureIndex,vecStartingPartialPart,vecEndingPartialPart,vecRoutingPoints))
		return false;
	return true;	
}
bool GraphManager::GetAccourateShortestPath(OGRPointVector& vecRoutingViaPoints, OGRPointVector& vecRoutingPoints,double& dAccCost)
{

	long lPointCount = vecRoutingViaPoints.size();
	if(lPointCount < 2)
		return false;

	long lViaPointIndex;
	double dMaxAllowableDistance = 0.01;
	LongVector vecVertex;
	LongVector vecGraph;
	RoutingPointInfoVector vecRPoint;
	for(lViaPointIndex = 0; lViaPointIndex < lPointCount; lViaPointIndex++)
	{
		long lVertexIndex,lEdgeIndex,lGraphIndex;
		RoutingPointInfo routingPointInfo;
		bool bIsGotVerTexIndex = GetClosestEdgeIndex(vecRoutingViaPoints[lViaPointIndex],routingPointInfo,lGraphIndex,dMaxAllowableDistance);
		if(!bIsGotVerTexIndex) 
			return false;
		vecGraph.push_back(lGraphIndex);	
		vecVertex.push_back(lVertexIndex);
		if(vecGraph.size() > 1 && vecGraph[lViaPointIndex] != vecGraph[lViaPointIndex - 1])
		{
			return false;
		}
		vecRPoint.push_back(routingPointInfo);
	}

	OGRPointVector vecStartingPartialPart;
	OGRPointVector vecEndingPartialPart;
	LongVector vecFeatureIndex;

	if(!defDjk.GetAccShortestPath(vecRPoint[0],vecRPoint[1],vecStartingPartialPart,vecEndingPartialPart,vecFeatureIndex,dAccCost))
		return false;
	
	if(!GetFaturePoints(vecFeatureIndex,vecStartingPartialPart,vecEndingPartialPart,vecRoutingPoints))
		return false;

	return true;
}

bool GraphManager::GetShortestPathVertex(OGRPointVector& vecRoutingViaPoints, LongVector& vecVertexIndex ,double& dAccCost)
{
	long lPointCount = vecRoutingViaPoints.size();
	if(lPointCount < 2)
		return false;

	long lViaPointIndex;
	double dMaxAllowableDistance = 0.01;
	LongVector vecVertex;
	LongVector vecGraph;
	for(lViaPointIndex = 0; lViaPointIndex < lPointCount; lViaPointIndex++)
	{
		long lVertexIndex,lEdgeIndex,lGraphIndex;
		bool bIsGotVerTexIndex = GetClosestVertexIndex(vecRoutingViaPoints[lViaPointIndex],lVertexIndex,lEdgeIndex,lGraphIndex,dMaxAllowableDistance);
		if(!bIsGotVerTexIndex) 
			return false;
		vecGraph.push_back(lGraphIndex);
		vecVertex.push_back(lVertexIndex);
		if(vecGraph.size() > 1 && vecGraph[lViaPointIndex] != vecGraph[lViaPointIndex - 1])
		{
			return false;
		}
	}

	

	if(!defDjk.GetShortestPathVertex(vecVertex,vecVertexIndex,dAccCost))
		return false;

	return true;
}
bool GraphManager::GetShortestPath(OGRPointVector& vecRoutingViaPoints, OGRPointVector& vecRoutingPoints ,double& dAccCost)
{
	LongVector vecEdgeIndex;
	if(!GetShortestPath(vecRoutingViaPoints,vecEdgeIndex,dAccCost))
		return false;
	if(!GetFaturePoints(vecEdgeIndex,vecRoutingPoints))
		return false;
	return true;
}


bool GraphManager::GetFaturePoints(LongVector vecEdgeIndex,OGRPointVector& vecStartingPoints,OGRPointVector& vecEndPoints, OGRPointVector& vecRoutingPoints)
{

	if(vecEdgeIndex.size() < 1)
		return false;

	RawPointVector vecRPoints;

	long lLineIndex,lRoutingLineCount = vecEdgeIndex.size();

	long lPointIndex,lPointCount;

	long featureIndex = vecEdgeIndex[0];
	lPointCount	= edgeInfoVector[featureIndex].vecFeaturePoints.size();
	RawPointVector* pVec = &edgeInfoVector[featureIndex].vecFeaturePoints;

	vecRPoints.insert(vecRPoints.end(),pVec->begin(),pVec->end());

	for(lLineIndex = 1; lLineIndex < lRoutingLineCount; lLineIndex++)
	{
		featureIndex = vecEdgeIndex[lLineIndex];	
		long lSize = vecRPoints.size();
		pVec = &edgeInfoVector[featureIndex].vecFeaturePoints;
		lPointCount = pVec->size();

		OGRRawPoint point1,point2;
		point1 = pVec->at(0);
		point2 = pVec->at(lPointCount - 1);

		OGRRawPoint sp,ep;
		sp = vecRPoints[0];
		ep = vecRPoints[lSize - 1];

		if(ep.x == point1.x && ep.y == point1.y)
		{
			vecRPoints.insert(vecRPoints.end(),pVec->begin(),pVec->end());
		}
		else if(ep.x == point2.x && ep.y == point2.y)
		{
			RawPointVector::iterator itMap = pVec->end();
			for(lPointIndex = lPointCount - 1; lPointIndex >= 0 ; lPointIndex--)
				vecRPoints.insert(vecRPoints.end(),pVec->at(lPointIndex));
		}
		else if(sp.x == point2.x && sp.y == point2.y)
		{	
			for(lPointIndex = lPointCount - 1; lPointIndex >= 0 ; lPointIndex--)
				vecRPoints.insert(vecRPoints.begin(),pVec->at(lPointIndex));
		}
		else if(sp.x== point1.x && sp.y == point1.y)
		{			
			RawPointVector::iterator itMap = pVec->begin();
			for(; itMap != pVec->end(); itMap++)			
				vecRPoints.insert(vecRPoints.begin(),(*itMap));			
		}
		else
		{
			printf("Roni got a problem\n");
		}
	}
	lPointCount = vecRPoints.size();
	vecRoutingPoints.reserve(lPointCount + 1);
	for(lPointIndex = 0; lPointIndex < lPointCount; lPointIndex++)
	{
		OGRRawPoint ogrp = vecRPoints[lPointIndex];
		OGRPoint ogp;
		ogp.setX(ogrp.x);
		ogp.setY(ogrp.y);
		vecRoutingPoints.push_back(ogp);
	}

	//Join the lines 
	AddWithLines(vecRoutingPoints,vecStartingPoints);
	AddWithLines(vecRoutingPoints,vecEndPoints);


	return true;
}

bool GraphManager::AddWithLines(OGRPointVector& vecLinePoints,OGRPointVector& vecLinePointsToAdd)
{
	OGRPoint sp,ep;
	long lSize = vecLinePoints.size();
	OGRPointVector* pVec = &vecLinePointsToAdd;
	long lPointCount = pVec->size();
	if(lSize == 0 || lPointCount == 0)
		return false;
	sp = vecLinePoints[0];
	ep = vecLinePoints[lSize - 1];
	
	

	OGRPoint point1,point2;
	point1 = pVec->at(0);
	point2 = pVec->at(lPointCount - 1);
	long lPointIndex = 0;

	if(ep.getX() == point1.getX()&& ep.getY() == point1.getY())
	{
		vecLinePoints.insert(vecLinePoints.end(),pVec->begin(),pVec->end());
	}
	else if(ep.getX() == point2.getX() && ep.getY() == point2.getY())
	{
		OGRPointVector::iterator itVec = pVec->end();
		for(lPointIndex = lPointCount - 1; lPointIndex >= 0 ; lPointIndex--)
			vecLinePoints.insert(vecLinePoints.end(),pVec->at(lPointIndex));
	}
	else if(sp.getX() == point2.getX() && sp.getY() == point2.getY())
	{	
		for(lPointIndex = lPointCount - 1; lPointIndex >= 0; lPointIndex--)
			vecLinePoints.insert(vecLinePoints.begin(),pVec->at(lPointIndex));
	}
	else if(sp.getX()== point1.getX() && sp.getY() == point1.getY())
	{			
		OGRPointVector::iterator itVec = pVec->begin();
		for(; itVec != pVec->end() - 1; itVec++)			
			vecLinePoints.insert(vecLinePoints.begin(),(*itVec));			
	}
	else
	{
		printf("Roni got a problem\n");
	}
	return true;
}



bool GraphManager::GetFaturePoints(LongVector vecEdgeIndex, OGRPointVector& vecRoutingPoints )
{
	if(vecEdgeIndex.size() < 1)
		return false;

	RawPointVector vecRPoints;

	long lLineIndex,lRoutingLineCount = vecEdgeIndex.size();

	long lPointIndex,lPointCount;

	long featureIndex = vecEdgeIndex[0];
	lPointCount	= edgeInfoVector[featureIndex].vecFeaturePoints.size();
	RawPointVector* pVec = &edgeInfoVector[featureIndex].vecFeaturePoints;

	vecRPoints.insert(vecRPoints.end(),pVec->begin(),pVec->end());

	for(lLineIndex = 1; lLineIndex < lRoutingLineCount; lLineIndex++)
	{
		featureIndex = vecEdgeIndex[lLineIndex];	
		long lSize = vecRPoints.size();
		pVec = &edgeInfoVector[featureIndex].vecFeaturePoints;
		lPointCount = pVec->size();

		OGRRawPoint point1,point2;
		point1 = pVec->at(0);
		point2 = pVec->at(lPointCount - 1);

		OGRRawPoint sp,ep;
		sp = vecRPoints[0];
		ep = vecRPoints[lSize - 1];

		if(ep.x == point1.x && ep.y == point1.y)
		{
			vecRPoints.insert(vecRPoints.end(),pVec->begin(),pVec->end());
		}
		else if(ep.x == point2.x && ep.y == point2.y)
		{
			RawPointVector::iterator itMap = pVec->end();
			for(lPointIndex = lPointCount - 1; lPointIndex >= 0 ; lPointIndex--)
				vecRPoints.insert(vecRPoints.end(),pVec->at(lPointIndex));
		}
		else if(sp.x == point2.x && sp.y == point2.y)
		{	
			for(lPointIndex = lPointCount - 1; lPointIndex >= 0 ; lPointIndex--)
				vecRPoints.insert(vecRPoints.begin(),pVec->at(lPointIndex));
		}
		else if(sp.x== point1.x && sp.y == point1.y)
		{			
			RawPointVector::iterator itMap = pVec->begin();
			for(; itMap != pVec->end(); itMap++)			
				vecRPoints.insert(vecRPoints.begin(),(*itMap));			
		}
		else
		{
			printf("Roni got a problem\n");
		}
	}
	lPointCount = vecRPoints.size();
	vecRoutingPoints.reserve(lPointCount + 1);
	for(lPointIndex = 0; lPointIndex < lPointCount; lPointIndex++)
	{
		OGRRawPoint ogrp = vecRPoints[lPointIndex];
		OGRPoint ogp;
		ogp.setX(ogrp.x);
		ogp.setY(ogrp.y);
		vecRoutingPoints.push_back(ogp);
	}


	return true;
}

bool GraphManager::AnalyzeNetworks()
{
	LabelManager labelManager; 
	labelManager.BuildNodeEdgeVector(pArcNodeInfo,featureCount,&edgeInfoVector,&vertexInfoVector,m_vecLayerFeatureCount);
	//WriteToFile("");
	SeparateComponent();
	//SaveGraph();
	delete[] pArcNodeInfo;
	pArcNodeInfo = NULL;
	return true;
}

void GraphManager::InitGraphManager()
{
	//Init
	BuildRTree();
	long lLayerCount = m_vecLayerNames.size();
	bgl.Initialize(&edgeInfoVector,&vertexInfoVector,lLayerCount);
	defDjk.Initialize(&edgeInfoVector,&vertexInfoVector,lLayerCount);
	astar.Initialize(&edgeInfoVector,&vertexInfoVector,lLayerCount);
	biDirectional.Initialize(&edgeInfoVector,&vertexInfoVector,lLayerCount);
}

bool GraphManager::SeparateComponent()
{
	long graphID = 0;
	long nVertexCount = vertexInfoVector.size();
	bIsVertexVisited = new bool[nVertexCount];
	long index;
	//Init
	for(index = 0; index < nVertexCount; index++)
	{
		bIsVertexVisited[index] = false;
	}

	for(index = 0; index < nVertexCount; index++)
	{
		if(bIsVertexVisited[index] == false)
		{
			RunBFS(index,graphID);
			graphID++;
		}
	}
	return true;
}

bool GraphManager::RunBFS(long lStartIndex, long graphID)
{

	LongVector vecVisitedNode;
	vecVisitedNode.reserve(vertexInfoVector.size());
	vecVisitedNode.push_back(lStartIndex);
	long lMaxSize = 0;
	while(vecVisitedNode.size() > 0)
	{
		LongVector::iterator itVec = vecVisitedNode.begin();
		long lVertexIndex = *itVec;
		bIsVertexVisited[lVertexIndex] = true;
		vecVisitedNode.erase(itVec);

		long lConnectedEdgeIndex = 0, nEdgeCount = vertexInfoVector[lVertexIndex].connectedEdgeLabel.size(); 
		for(lConnectedEdgeIndex = 0;  lConnectedEdgeIndex < nEdgeCount; lConnectedEdgeIndex++ )
		{
			long lEdgeIndex = vertexInfoVector[lVertexIndex].connectedEdgeLabel[lConnectedEdgeIndex];
			
			edgeInfoVector[lEdgeIndex].graphIndex = graphID;
			if(!bIsVertexVisited[edgeInfoVector[lEdgeIndex].startNodeLabel])
			{
				vecVisitedNode.push_back(edgeInfoVector[lEdgeIndex].startNodeLabel);
				bIsVertexVisited[edgeInfoVector[lEdgeIndex].startNodeLabel] = true;
			}

			if(!bIsVertexVisited[edgeInfoVector[lEdgeIndex].endNodeLabel])
			{
				vecVisitedNode.push_back(edgeInfoVector[lEdgeIndex].endNodeLabel);
				bIsVertexVisited[edgeInfoVector[lEdgeIndex].endNodeLabel] = true;
			}

			
			if(vecVisitedNode.size() > lMaxSize)
				lMaxSize = vecVisitedNode.size();
		}
	}
	
	return true;
}

