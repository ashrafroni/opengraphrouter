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
#ifndef CONFIGTAGS_H
#define CONFIGTAGS_H


namespace ConfigFileTags
{
	static const char* tagTask = "TASK";
	static const char* tagName = "NAME";
	static const char* tagEnd = "END";
	static const char* tagLayer = "LAYER";
	

	static const char* tagBuildGraph = "BUILD_GRAPH";
	static const char* tagBuildGraphAndService = "BUILD_GRAPH_SERVICE";
	static const char* tagLayerName = "LAYERNAME";
	static const char* tagInputPath = "INPUTPATH";
	static const char* tagCostFieldName = "COSTFIELDNAME";
	static const char* tagOutputPath = "OUTPUTPATH";
	static const char* tagOutputLayerName = "OUTPUTLAYERNAME";
	
	static const char* tagReadGraphService = "READ_GRAPH_SERVICE";

	static const char* tagDirectionField = "DIRECTION_FIELD";
	static const char* tagForwardDirValue = "FORWARDDIRVAL";
	static const char* tagReverseDirValue = "REVERSEDIRVAL";
	
	static const char* tagBothWayDirValue = "BOTHWAYDIRVALUE";
	static const char* tagLengthFieldName = "LENGTHFIELD";
	static const char* tagValueAuto = "AUTO";
	static const char* tagSegmentSpeedFieldName = "SEGMENTSPEEDFIELD";
	static const char* tagStreetNameField = "STREETFIELD";
	static const char* tagFairPerKm		= "FAIRPERKM";

	
};
//?request=GetPath&routepoints=-131.763957,58.894665$-114.055304,49.024089
//?request=GetPathBGL&routepoints=-131.763957,58.894665$-114.055304,49.024089
namespace RequestTags
{
	static const char* tagRequest = "REQUEST";
	static const char* tagGetPath = "GETPATH";	
	static const char* tagGetDD = "GETDD";	
	static const char* tagGetDrivingDirectionBGL = "GETDDBGL";	
	static const char* tagGetPathBGL = "GETPATHBGL";
	static const char* tagGetPathUsingPublicRoute = "GETPATHUSINGPUBLICROUTE";	
	static const char* tagRoutePoints = "ROUTEPOINTS";
}


//namespace XMLOutputTags
//{
////	route
////		edge
////		instruction
////		dist
////		units
////		time
////		acc_dist
////		acc_time
////		wkt
////		LINESTRING
//}

#endif
