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
#ifndef CONFIGURATIONMANAGER_H
#define CONFIGURATIONMANAGER_H
#include "GraphFileManager.h"
#include "GraphManager.h"
#include <string>



enum TaskType
{
	TaskTypeBuildGraph,
	TaskTypeBuildGraphAndService,
	TaskTypeReadGraphAndService,
	TaskTypeUnknown
};

class ConfigurationManager
{
public:
	ConfigurationManager(void);
	~ConfigurationManager(void);

public:
	bool ReadConfigurationFile();
	bool ConfigureServer(GraphManager* pGraphManager, GraphFileManager* pGraphFileManager);
	TaskType GetTaskType(){return m_taskType;}

private:
	bool ReadLayerConfigurationData(FILE* fpCfg);
	bool Validate();
	bool ParseTask(FILE* fp);
	bool ParseLayers(FILE* fp);
	bool ParseLayer(FILE* fp);
private:
	std::string m_strTask;
	
	std::string m_strPath;	
	StringLongPairVector m_vecCostFieldNames;	
	std::string m_strGraphLayerName;
	double m_dFairPerKm;
	TaskType m_taskType;
	LayerInfoVector m_vecLayerInfo;	
};

#endif
