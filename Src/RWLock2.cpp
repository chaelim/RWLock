/**
 *      File: RWLock2.cpp
 *    Author: CS Lim
 *   Purpose: Reader writer lock using Per-Processor data
 * 
 */

#include "stdafx.h"
#pragma  hdrstop


//===========================================================================
// Private variables
//===========================================================================

static int s_numProcs = 0;
static __declspec(thread) unsigned t_procId;    

static int GetNumberOfProcessors ()
{
	if (s_numProcs == 0)
	{
		SYSTEM_INFO sysinfo;
		GetSystemInfo( &sysinfo );
		s_numProcs = sysinfo.dwNumberOfProcessors;
	}
	return s_numProcs;
}

//===========================================================================
// CRWLock2 implementation
//===========================================================================
CRWLock2::CRWLock2 ()
{
	m_lock = new SRWLOCK[GetNumberOfProcessors()];
	for (int i = 0; i < GetNumberOfProcessors(); i++)
	{
		InitializeSRWLock(&m_lock[i]);
	}
}

CRWLock2::~CRWLock2 ()
{
	delete [] m_lock;
}

void CRWLock2::EnterRead ()
{
	t_procId = GetCurrentProcessorNumber();
	AcquireSRWLockShared(&m_lock[t_procId]);
}

void CRWLock2::LeaveRead ()
{
	ReleaseSRWLockShared(&m_lock[t_procId]);
}

void CRWLock2::EnterWrite ()
{
	for (int i = 0; i < GetNumberOfProcessors(); i++)
	{
		AcquireSRWLockExclusive(&m_lock[i]);
	}
}

void CRWLock2::LeaveWrite ()
{
	for (int i = 0; i < GetNumberOfProcessors(); i++)
	{
		ReleaseSRWLockExclusive(&m_lock[i]);
	}
}


//===========================================================================
// MIT License
//
// Copyright (c) 2012 by Chae Seong Lim
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//===========================================================================