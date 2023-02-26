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
#ifndef GRAPHDEFINITION_H
#define GRAPHDEFINITION_H


#include "utils.h"
#include "CommonDefinition.h"
#include "ogr_geometry.h"
#include "TimeScheduleHandler.h"





typedef struct ArcNodeInfo_S 
{
	OGRRawPoint startPoint,endPoint;
	double startAngle,endAngle;
	long edgeGeomatryID;
	int dataSrcID;
	DoubleVector costVector;
	RawPointVector vecFeaturePoints;
	std::string strStreetName;
	LineDirection direction;	
	double segmentSpeedValue;

	LineDirection streetDirection;
}
ArcNodeInfo;

struct RoutingPointInfo
{
	OGRPoint selectedPoint;
	long lFeatureIndex;
	double dPartialValuePercent;
	double dPartialCost;
	long lStartingPointIndex;
	OGRPoint intersectionPoint;
};
typedef std::vector<RoutingPointInfo> RoutingPointInfoVector;


typedef struct VertexInfo_S 
{
	//Label of the end points of  a polyline.
	//Here the middle point are ignored and that points will be handled in different way.
	long vertexLabel;

	//The Original point. This is not necessary to store it.
	OGRRawPoint vertexPoint;	

	//The Vertex connected to this vertex
	//The will be in sync with the connectedEdgeLabel; i.e if a vertex connected with 
	// 1,2,3 edge with 1,5,6 index this will be in the same index
	LongVector connectedVertexLabel;

	//The edgeLabel(s) connected with this node.
	LongVector connectedEdgeLabel;

	//Direction of the connected edge
	DirectionEnumVector vecDirection;

	//
	IntVector vecLayerIndex;

	CTimeScheduleHandler timeScheduleHandler;

	bool bIsTransitPoint;
}
VertexInfo;
typedef std::vector<VertexInfo> VertexInfoVector;



typedef struct EdgeInfo_S 
{
	//This is the global edge label
	long edgeLabel;
	//The index of the source layer.For multiple layer.
	long sourceLayerIndex;
	//The index of the edge in the source layer. This may need for future processing
	long edgeGeomatryID;

	//This is necessary
	long graphIndex;

	long startNodeLabel;

	long endNodeLabel;

	DoubleVector costVector;
	//double cost;
	//
	double reverseCost;

	RawPointVector vecFeaturePoints;

	double startAngle,endAngle;

	//new
	std::string strStreetName;

	//0 -Both 1-means forward 2 means Reverse
	//long direction;
	LineDirection direction;
	
	//This is not a minimization parameter at all.
	double segmentSpeedValue;

	//This will be used specially for the transit and bus route.
	double dSegmentTravelTime;


}EdgeInfo;


struct Vertex_def
{
	double vcost;
	long id;	
};
struct Vertex_def_public
{
	double vcost;
	long id;
	long lCurrentTime;
	long lCurLayerIndex;
};


	
struct Vertex_def_AStar
{
	double astarcost;
	long id;
	double oroginalCost;
};

enum NodeTravellingDirection
{
	NodeTravellingDirectionForward,
	NodeTravellingDirectionReverse,
	NodeTravellingDirectionUnified,
	NodeTravellingDirectionUnknown

};
struct Vertex_def_BDAStar
{
	double astarcost;
	long id;
	double oroginalCost;
	NodeTravellingDirection nodeTravellingDirection;
};



typedef std::vector<EdgeInfo> EdgeInfoVector;




//	long getEdgeLabelWithVertex(long lVertexLabel)
//	{
//		long lEdgeLabel = -1;
//		long lVertexIndex,lVertexCount = connectedVertexLabel.size();
//		for(lVertexIndex = 0; lVertexIndex < lVertexCount; lVertexIndex++)
//		{
//			if(connectedVertexLabel[lVertexIndex] == lVertexLabel)
//			{
//				lEdgeLabel = connectedEdgeLabel[lVertexIndex];
//				break;
//			}
//		}
//		return lEdgeLabel;
//	}

#endif
