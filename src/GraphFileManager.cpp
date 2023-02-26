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
#include "GraphFileManager.h"

GraphFileManager::GraphFileManager(void)
{
}

GraphFileManager::~GraphFileManager(void)
{
}


bool GraphFileManager::GeneratGraphFiles(EdgeInfoVector* pEdgeInfoVector,VertexInfoVector* pVertexInfoVector)
{
	
	m_pEdgeInfoVector = pEdgeInfoVector;
	m_pVertexInfoVector = pVertexInfoVector;
	WriteEdgeFile();
	WriteVertexFile();
	return true;
}

void GraphFileManager::SetFilePath(std::string strFilePath)
{
	m_strFilePath = strFilePath;
}

void GraphFileManager::SetFileName(std::string strFileName)
{
	m_strFileName = strFileName;
}


bool GraphFileManager::ReadGraphFile(EdgeInfoVector* pEdgeInfoVector,VertexInfoVector* pVertexInfoVector)
{
	m_pEdgeInfoVector = pEdgeInfoVector;
	m_pVertexInfoVector = pVertexInfoVector;
	m_pEdgeInfoVector->clear();
	m_pVertexInfoVector->clear();
	ReadEdgeFile();
	ReadVertexFile();
	return true;
}

bool GraphFileManager::WriteEdgeTextFile()
{
	//Edge File
	long lFileVersion = 9997;
	std::string strFileNameWithPath = m_strFilePath + m_strFileName + ".edg";
	FILE* fpGraph = fopen(strFileNameWithPath.c_str(),"w");
	if(fpGraph == NULL)
		return false;
	//File version
	fprintf(fpGraph,"%ld\n",lFileVersion);
	//Number of Edges.
	long lEdgeCount = m_pEdgeInfoVector->size();
	fprintf(fpGraph,"%ld\n",lEdgeCount);	
	//Edge Dump
	long lEdgeIndex;

	for(lEdgeIndex = 0; lEdgeIndex < lEdgeCount; lEdgeIndex++)
	{
		EdgeInfo edgeInfo = m_pEdgeInfoVector->at(lEdgeIndex);
		fprintf(fpGraph,"%ld,",edgeInfo.edgeLabel);	

		fprintf(fpGraph,"%ld,",edgeInfo.sourceLayerIndex);	

		fprintf(fpGraph,"%ld,",edgeInfo.edgeGeomatryID);	

		fprintf(fpGraph,"%ld,",edgeInfo.graphIndex);	

		fprintf(fpGraph,"%ld,",edgeInfo.startNodeLabel);	

		fprintf(fpGraph,"%ld,",edgeInfo.endNodeLabel);	

		//fprintf(fpGraph,"%lf,",edgeInfo.cost);	

		long lPoingIndex,lPointCount = edgeInfo.vecFeaturePoints.size();
		//Number of point in that edge feature
		fprintf(fpGraph,"%ld,",lPointCount);	

		for(lPoingIndex = 0; lPoingIndex < lPointCount; lPoingIndex++)
		{
			fprintf(fpGraph,"%lf,",edgeInfo.vecFeaturePoints[lPoingIndex].x);	
			fprintf(fpGraph,"%lf,",edgeInfo.vecFeaturePoints[lPoingIndex].y);	
		}
		fprintf(fpGraph,"\n");		
	}
	fclose(fpGraph);

	return true;
}
bool GraphFileManager::WriteEdgeFile()
{
	//Edge File
	long lFileVersion = 9997;
	std::string strFileNameWithPath = m_strFilePath + m_strFileName + ".edg";
	FILE* fpGraph = fopen(strFileNameWithPath.c_str(),"wb");
	if(fpGraph == NULL)
		return false;
	//File version
	fwrite(&lFileVersion,sizeof(long),1,fpGraph);
	//Number of Edges.
	long lEdgeCount = m_pEdgeInfoVector->size();
	
	fwrite(&lEdgeCount,sizeof(long),1,fpGraph);
	//Edge Dump
	long lEdgeIndex;

	for(lEdgeIndex = 0; lEdgeIndex < lEdgeCount; lEdgeIndex++)
	{
		EdgeInfo edgeInfo = m_pEdgeInfoVector->at(lEdgeIndex);
		
		fwrite(&edgeInfo.edgeLabel,sizeof(long),1,fpGraph);
		
		fwrite(&edgeInfo.sourceLayerIndex,sizeof(long),1,fpGraph);
		
		fwrite(&edgeInfo.edgeGeomatryID,sizeof(long),1,fpGraph);
		
		fwrite(&edgeInfo.edgeGeomatryID,sizeof(long),1,fpGraph);
		
		fwrite(&edgeInfo.startNodeLabel,sizeof(long),1,fpGraph);
		
		fwrite(&edgeInfo.endNodeLabel,sizeof(long),1,fpGraph);
		

		long lCostIndex,lCostFieldCount = edgeInfo.costVector.size();
		fwrite(&lCostFieldCount,sizeof(long),1,fpGraph);

		for(lCostIndex = 0; lCostIndex < lCostFieldCount; lCostIndex++)
		{
			fwrite(&edgeInfo.costVector[lCostIndex],sizeof(double),1,fpGraph);
		}
		
		

		fwrite(&edgeInfo.startAngle,sizeof(double),1,fpGraph);

		fwrite(&edgeInfo.endAngle,sizeof(double),1,fpGraph);

		

		long lPoingIndex,lPointCount = edgeInfo.vecFeaturePoints.size();

		//Number of point in that edge feature		
		fwrite(&lPointCount,sizeof(long),1,fpGraph);

		for(lPoingIndex = 0; lPoingIndex < lPointCount; lPoingIndex++)
		{
			fwrite(&edgeInfo.vecFeaturePoints[lPoingIndex].x,sizeof(double),1,fpGraph);
			fwrite(&edgeInfo.vecFeaturePoints[lPoingIndex].y,sizeof(double),1,fpGraph);
		}		

		fwrite(&edgeInfo.direction,sizeof(long),1,fpGraph);

		fwrite(&edgeInfo.segmentSpeedValue,sizeof(double),1,fpGraph);

		int iLength = edgeInfo.strStreetName.length();

		fwrite(&iLength,sizeof(int),1,fpGraph);

		fwrite(edgeInfo.strStreetName.c_str(),edgeInfo.strStreetName.length(),1,fpGraph);

		
	}

	fclose(fpGraph);

	return true;
}

bool GraphFileManager::ReadEdgeTextFile()
{
	return true;
}
bool GraphFileManager::ReadEdgeFile()
{
	//Edge File
	long lFileVersion;
	std::string strFileNameWithPath = m_strFilePath + m_strFileName + ".edg";
	FILE* fpGraph = fopen(strFileNameWithPath.c_str(),"rb");
	if(fpGraph == NULL)
		return false;

	//File version
	fread(&lFileVersion,sizeof(long),1,fpGraph);
	if(lFileVersion != 9997)
	{
		fclose(fpGraph);
		return false;
	}

	//Number of Edges.
	long lEdgeCount;

	fread(&lEdgeCount,sizeof(long),1,fpGraph);

	m_pEdgeInfoVector->reserve(lEdgeCount);
	//Edge Dump
	long lEdgeIndex;

	for(lEdgeIndex = 0; lEdgeIndex < lEdgeCount; lEdgeIndex++)
	{
		EdgeInfo edgeInfo;

		fread(&edgeInfo.edgeLabel,sizeof(long),1,fpGraph);

		fread(&edgeInfo.sourceLayerIndex,sizeof(long),1,fpGraph);

		fread(&edgeInfo.edgeGeomatryID,sizeof(long),1,fpGraph);

		fread(&edgeInfo.edgeGeomatryID,sizeof(long),1,fpGraph);

		fread(&edgeInfo.startNodeLabel,sizeof(long),1,fpGraph);

		fread(&edgeInfo.endNodeLabel,sizeof(long),1,fpGraph);

		long lCostIndex,lCostFieldCount; 
		fread(&lCostFieldCount,sizeof(long),1,fpGraph);
		edgeInfo.costVector.reserve(lCostFieldCount);

		for(lCostIndex = 0; lCostIndex < lCostFieldCount; lCostIndex++)
		{
			double dCostValue;
			fread(&dCostValue,sizeof(double),1,fpGraph);
			edgeInfo.costVector.push_back(dCostValue);
		}

		fread(&edgeInfo.startAngle,sizeof(double),1,fpGraph);

		fread(&edgeInfo.endAngle,sizeof(double),1,fpGraph);

		long lPoingIndex,lPointCount;
		
		//Number of point in that edge feature		
		fread(&lPointCount,sizeof(long),1,fpGraph);
		edgeInfo.vecFeaturePoints.reserve(lPointCount);

		double x,y;
		for(lPoingIndex = 0; lPoingIndex < lPointCount; lPoingIndex++)
		{
			OGRRawPoint ogrPoint;
			fread(&x,sizeof(double),1,fpGraph);
			fread(&y,sizeof(double),1,fpGraph);
			ogrPoint.x = x;
			ogrPoint.y = y;
			edgeInfo.vecFeaturePoints.push_back(ogrPoint);			
		}

		fread(&edgeInfo.direction,sizeof(long),1,fpGraph);

		fread(&edgeInfo.segmentSpeedValue,sizeof(double),1,fpGraph);

		int iLength;

		fread(&iLength,sizeof(int),1,fpGraph);
		char* ch = new char[iLength + 1];
		
		fread(ch,sizeof(char) * iLength ,1,fpGraph);		
		ch[iLength] = 0;
		edgeInfo.strStreetName = ch;
		m_pEdgeInfoVector->push_back(edgeInfo);
		delete ch;
	}

	fclose(fpGraph);

	return true;
}


bool GraphFileManager::WriteVertexTextFile()
{
	return true;
}
bool GraphFileManager::WriteVertexFile()
{
	//Edge File
	long lFileVersion = 9997;
	std::string strFileNameWithPath = m_strFilePath + m_strFileName + ".vtx";
	FILE* fpGraph = fopen(strFileNameWithPath.c_str(),"wb");
	if(fpGraph == NULL)
		return false;
	//File version
	fwrite(&lFileVersion,sizeof(long),1,fpGraph);
	//Number of Edges.
	long lVertexCount = m_pVertexInfoVector->size();

	fwrite(&lVertexCount,sizeof(long),1,fpGraph);
	//Edge Dump
	long lVertexIndex;
	for(lVertexIndex = 0; lVertexIndex < lVertexCount; lVertexIndex++)
	{
		VertexInfo vertexInfo = m_pVertexInfoVector->at(lVertexIndex);

		fwrite(&vertexInfo.vertexLabel,sizeof(long),1,fpGraph);
		fwrite(&vertexInfo.vertexPoint.x,sizeof(double),1,fpGraph);
		fwrite(&vertexInfo.vertexPoint.y,sizeof(double),1,fpGraph);
		//AH:Error
		//fwrite(&vertexInfo.graphIndex,sizeof(long),1,fpGraph);

		long lEdgeIndex,lEdgeCount = vertexInfo.connectedEdgeLabel.size();

		//Number of edge connected with this vertex
		fwrite(&lEdgeCount,sizeof(long),1,fpGraph);
		for(lEdgeIndex = 0; lEdgeIndex < lEdgeCount; lEdgeIndex++)
		{
			fwrite(&vertexInfo.connectedEdgeLabel[lEdgeIndex],sizeof(long),1,fpGraph);
		}

		long lCVIndex,lCVCount = vertexInfo.connectedVertexLabel.size();
		//Number of vertex connected with this vertex
		fwrite(&lCVCount,sizeof(long),1,fpGraph);
		for(lCVIndex = 0; lCVIndex < lCVCount; lCVIndex++)
		{
			fwrite(&vertexInfo.connectedVertexLabel[lCVIndex],sizeof(long),1,fpGraph);
		}
	}

	fclose(fpGraph);
	return true;
}

bool GraphFileManager::ReadVertexTextFile()
{
	return true;
}
bool GraphFileManager::ReadVertexFile()
{
	//Edge File
	long lFileVersion;
	std::string strFileNameWithPath = m_strFilePath + m_strFileName + ".vtx";
	FILE* fpGraph = fopen(strFileNameWithPath.c_str(),"rb");
	if(fpGraph == NULL)
		return false;
	//File version
	fread(&lFileVersion,sizeof(long),1,fpGraph);
	
	long lVertexCount;

	fread(&lVertexCount,sizeof(long),1,fpGraph);
	m_pVertexInfoVector->reserve(lVertexCount);
	
	long lVertexIndex;
	for(lVertexIndex = 0; lVertexIndex < lVertexCount; lVertexIndex++)
	{
		VertexInfo vertexInfo;

		fread(&vertexInfo.vertexLabel,sizeof(long),1,fpGraph);
		fread(&vertexInfo.vertexPoint.x,sizeof(double),1,fpGraph);
		fread(&vertexInfo.vertexPoint.y,sizeof(double),1,fpGraph);
		//AH:Error
		//fread(&vertexInfo.graphIndex,sizeof(long),1,fpGraph);

		long lEdgeIndex,lEdgeCount;

		//Number of edge connected with this vertex
		fread(&lEdgeCount,sizeof(long),1,fpGraph);
		vertexInfo.connectedEdgeLabel.reserve(lEdgeCount);

		for(lEdgeIndex = 0; lEdgeIndex < lEdgeCount; lEdgeIndex++)
		{
			long lEdgeLabel;
			fread(&lEdgeLabel,sizeof(long),1,fpGraph);
			vertexInfo.connectedEdgeLabel.push_back(lEdgeLabel);
			
		}

		long lCVIndex,lCVCount;
		//Number of vertex connected with this vertex
		fread(&lCVCount,sizeof(long),1,fpGraph);
		for(lCVIndex = 0; lCVIndex < lCVCount; lCVIndex++)
		{
			long lVertexLabel;
			fread(&lVertexLabel,sizeof(long),1,fpGraph);
			vertexInfo.connectedVertexLabel.push_back(lVertexLabel);
		}

		m_pVertexInfoVector->push_back(vertexInfo);
	}

	fclose(fpGraph);
	return true;
}
