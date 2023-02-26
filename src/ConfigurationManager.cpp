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
#include "ConfigurationManager.h"
#include "ConfigTags.h"

#include <algorithm>



ConfigurationManager::ConfigurationManager(void)
{
	m_taskType = TaskTypeUnknown;
}

ConfigurationManager::~ConfigurationManager(void)
{
}

bool ConfigurationManager::ReadLayerConfigurationData(FILE* fpCfg)
{
	
	return true;
}

bool ConfigurationManager::ReadConfigurationFile()
{
	FILE* fpCfg = fopen("config.cfg","r");
	if(fpCfg == NULL)
		return false;
	char chLine[1024];
	bool bIsTaskNameFound,bIsEndFound,bIsTaskFound;
	bIsTaskNameFound = bIsEndFound = bIsTaskFound = false;

	ParseTask(fpCfg);
	ParseLayers(fpCfg);
	return Validate();
}

bool ConfigurationManager::Validate()
{
//	if(m_taskType == TaskTypeBuildGraph)
//	{
//		if(m_strLayerName.length() < 1)//|| m_vecCostFieldNames.size() < 1
//			return false;
//	}
//	else if(m_taskType == TaskTypeReadGraphAndService)
//	{
//		if(m_strLayerName.length() < 1)
//			return false;
//	}

	return true;
}




bool ConfigurationManager::ConfigureServer(GraphManager* pGraphManager, GraphFileManager* pGraphFileManager)
{
	DataBaseParameters dbfParam;
	
	if(m_taskType == TaskTypeBuildGraph)
	{		
		pGraphManager->ReadDataSource(m_vecLayerInfo);
		pGraphManager->AnalyzeNetworks();
		pGraphFileManager->SetFileName(m_strGraphLayerName);
		pGraphFileManager->SetFilePath(m_strPath);
		pGraphFileManager->GeneratGraphFiles(pGraphManager->GetEdgeVecPtr(), pGraphManager->GetVertexVecPtr());
		pGraphManager->InitGraphManager();

	}
	else if(m_taskType == TaskTypeReadGraphAndService) 
	{
		pGraphFileManager->SetFileName(m_strGraphLayerName);
		pGraphFileManager->SetFilePath(m_strPath);
		pGraphFileManager->ReadGraphFile(pGraphManager->GetEdgeVecPtr(), pGraphManager->GetVertexVecPtr());
		pGraphManager->InitGraphManager();		

	}
	return true;
}


bool ConfigurationManager::ParseTask(FILE* fp)
{
	int iResult = fseek(fp, 0L, SEEK_SET);
	char chLine[1024];
	int iCount = 0;
	
	while(fscanf(fp,"%[^\n]",chLine)!= EOF)
	{
		fgetc(fp);
		if(chLine[0] == '#')
			continue;
		
		std::string strConfigCom = chLine;
		std::string strCommandTag = strConfigCom;
		std::string strTagName;
		std::string strTagValue;
		
		std::transform(strCommandTag.begin(), strCommandTag.end(), strCommandTag.begin(),toupper);	

		if(strCommandTag.compare(ConfigFileTags::tagTask) == 0)
		{
			iCount++;			
		}
		else if(strCommandTag.compare(ConfigFileTags::tagEnd) == 0)
		{
			iCount++;
			break;
		}
		else 
		{
			//int iPos = strCommandTag.find(ConfigFileTags::tagName);
			//if(iPos == std::string::npos)
				//continue;
			int iPosfc = strCommandTag.find('"');
			if(iPosfc == std::string::npos)
				continue;
			int iPoslc = strCommandTag.find('"',iPosfc + 1);
			if(iPoslc == std::string::npos)
				continue;
			strTagValue = strConfigCom.substr(iPosfc + 1,iPoslc - iPosfc - 1);
			iPosfc = strCommandTag.find(' ');
			strTagName = strCommandTag.substr(0,iPosfc);			
			iCount++;

			if(strTagName.compare(ConfigFileTags::tagName) == 0)
			{
				if(strTagValue.compare(ConfigFileTags::tagBuildGraph) == 0)
				{
					m_taskType = TaskTypeBuildGraph;
				}
				else if(strTagValue.compare(ConfigFileTags::tagReadGraphService) == 0)
				{
					m_taskType = TaskTypeReadGraphAndService;
				}
				else if(strTagValue.compare(ConfigFileTags::tagBuildGraphAndService) == 0)
				{
					m_taskType = TaskTypeReadGraphAndService;
				}
			}
			else if(strTagName.compare(ConfigFileTags::tagOutputPath) == 0)
			{
				m_strPath = strTagValue;
			}
			else if(strTagName.compare(ConfigFileTags::tagOutputLayerName) == 0)
			{
				m_strGraphLayerName = strTagValue; 
			}

		}
	}
	
	return true;
}


bool ConfigurationManager::ParseLayers(FILE* fp)
{
	int iResult = fseek(fp, 0L, SEEK_SET);
	char chLine[1024];
	while(fscanf(fp,"%s",chLine)!= EOF)
	{
		if(chLine[0] == '#')
			continue;
		std::string strCommand = chLine;
		if(strCommand.compare(ConfigFileTags::tagLayer) == 0)
		{
			if(!ParseLayer(fp))
				return false;
		}
	}
	return true;
}
bool ConfigurationManager::ParseLayer(FILE* fp)
{
	LayerInfo layerInfo;
	layerInfo.m_dFairCostPerKm = -1.0000;

	char chLine[1024];
	int iCount = 0;
	std::string strCommand;
	std::string strTaskName;
	while(fscanf(fp,"%[^\n]",chLine)!= EOF)
	{
		fgetc(fp);
		if(chLine[0] == '#')
			continue;
		strCommand = chLine;
		if(strCommand.length() < 1)
			continue;
		if(strCommand.compare(ConfigFileTags::tagEnd) == 0)
			break;

		
		int iPosfc = strCommand.find('"');
		if(iPosfc == std::string::npos)
			continue;
		int iPoslc = strCommand.find('"',iPosfc + 1);
		if(iPoslc == std::string::npos)
			continue;
		strTaskName = strCommand.substr(iPosfc + 1,iPoslc - iPosfc - 1);

		
		if(strCommand.find(ConfigFileTags::tagName) != std::string::npos)
		{
			layerInfo.m_strLayerName = 	strTaskName;
		}
		else if(strCommand.find(ConfigFileTags::tagInputPath) != std::string::npos)
		{
			
			layerInfo.m_strInputPath = strTaskName;
		}
		
		else if(strCommand.find(ConfigFileTags::tagLengthFieldName) != std::string::npos)
		{
			StringLongPair strLength;
			strLength.m_strFieldName = strTaskName;
			layerInfo.m_dbfParameters.vecCost.push_back(strLength);
		}
		else if(strCommand.find(ConfigFileTags::tagStreetNameField) != std::string::npos)
		{
			layerInfo.m_dbfParameters.strStreetName = strTaskName;
		}
		else if(strCommand.find(ConfigFileTags::tagDirectionField) != std::string::npos)
		{
			layerInfo.m_dbfParameters.strDirField = strTaskName;
		}
		else if(strCommand.find(ConfigFileTags::tagForwardDirValue) != std::string::npos)
		{
			layerInfo.m_dbfParameters.strForwardDIRVal = strTaskName;
		}
		else if(strCommand.find(ConfigFileTags::tagReverseDirValue) != std::string::npos)
		{
			layerInfo.m_dbfParameters.strReverseDIRVal = strTaskName;
		}
		else if(strCommand.find(ConfigFileTags::tagBothWayDirValue) != std::string::npos)
		{
			layerInfo.m_dbfParameters.strBothwayDIRVal = strTaskName;
		}
		else if(strCommand.find(ConfigFileTags::tagFairPerKm) != std::string::npos)
		{
			layerInfo.m_dFairCostPerKm = atof(strTaskName.c_str());
		}

	}
	m_vecLayerInfo.push_back(layerInfo);
	return true;
}


