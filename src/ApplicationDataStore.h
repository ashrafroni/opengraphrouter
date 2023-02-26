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
#ifndef APPLICATIONDATASTORE_H
#define APPLICATIONDATASTORE_H


#include "GraphFileManager.h"
#include "GraphManager.h"
#include "ConfigurationManager.h"
#include "TextBasedDrivingDirectionGenerator.h"
#include "PlainShortestPathGenerator.h"




class ApplicationDataStore
{
public:
	ApplicationDataStore()
	{
		m_textDDGenerator.SetGraphManagerPtr(&m_GraphManager);
		m_plainShortestPath.SetGraphManagerPtr(&m_GraphManager);
	}

	
public:
	GraphFileManager m_GraphFileManager;
	GraphManager m_GraphManager;
	ConfigurationManager m_ConfigurationManager;
	TextBasedDrivingDirectionGenerator m_textDDGenerator;
	CPlainShortestPathGenerator m_plainShortestPath;
};
#endif
