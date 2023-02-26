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
#include "TimeScheduleHandler.h"

CTimeScheduleHandler::CTimeScheduleHandler(void)
{
}


CTimeScheduleHandler::~CTimeScheduleHandler(void)
{
}


void CTimeScheduleHandler::addTime(TimeLayerIndex timeLayerIndex)
{
	long lCount = vecTimeLayerIndex.size();
	if(lCount == 0)
	{
		vecTimeLayerIndex.push_back(timeLayerIndex);
		return;
	}
	long lPos = getClosestScheduleIndex(timeLayerIndex.lTimeSchedule);
	TimeLayerIndexVector::iterator itVec = vecTimeLayerIndex.begin() + lPos;	
	vecTimeLayerIndex.insert(itVec,timeLayerIndex);
}

void CTimeScheduleHandler::addTime(long lLayerIndex,long lTimeSchedule)
{
	TimeLayerIndex timeLayerIndex;
	timeLayerIndex.lLayerIndex = lLayerIndex;
	timeLayerIndex.lTimeSchedule= lTimeSchedule;
	addTime(timeLayerIndex);
}
bool CTimeScheduleHandler::getClosestScheduleOfExistingLayer(TimeLayerIndex& timeLayerIndex, long lTime,long lLayerIndex)
{
	long lIndex = getClosestScheduleIndex(lTime);
	long lTimeLayerEntryCount = vecTimeLayerIndex.size();
	if(lIndex < 0 || lIndex >= lTimeLayerEntryCount)
		return false;
	TimeLayerIndex tl = vecTimeLayerIndex[lIndex];
	if(tl.lTimeSchedule <  lTime)
		return false;

	timeLayerIndex = vecTimeLayerIndex[lIndex];	
	if(timeLayerIndex.lLayerIndex != lLayerIndex)
	{
//		long lIndex1;
//		for(lIndex1  = lIndex; lIndex1 < lTimeLayerEntryCount; lIndex1++)
//		{
//			TimeLayerIndex tl = vecTimeLayerIndex[lIndex1];
//			if(t1.lLayerIndex == lLayerIndex)
//			{			
//				timeLayerIndex  = vecTimeLayerIndex[lIndex];	
//				return true;
//			}
//		}
	}
	return true;
}

bool CTimeScheduleHandler::getClosestSchedule(TimeLayerIndex& timeLayerIndex, long lTime)
{
	long lIndex = getClosestScheduleIndex(lTime);
	if(lIndex < 0 || lIndex >= vecTimeLayerIndex.size())
		return false;
	TimeLayerIndex tl = vecTimeLayerIndex[lIndex];
	if(tl.lTimeSchedule <  lTime)
		return false;
	timeLayerIndex = vecTimeLayerIndex[lIndex];	
	return true;
}



long CTimeScheduleHandler::getClosestScheduleIndex(long lTime)
{
	long lCount = vecTimeLayerIndex.size();
	long lStart,lMid,lEnd;
	lStart = 0;
	lEnd = lCount;
	lMid = (lStart + lEnd)/2;
	long lPos = 0;

	while(lStart < lEnd)
	{
		lMid = (lStart + lEnd)/2;
		long lCurTimeSchedule = vecTimeLayerIndex[lMid].lTimeSchedule;
		if(lTime > lCurTimeSchedule)
		{
			lStart = lMid + 1;
			lPos = lStart;
		}
		else
		{
			lEnd = lMid;
			lPos = lEnd;
		}
	}
	return lPos;
}