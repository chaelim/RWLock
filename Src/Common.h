/**
 *      File: Common.h
 *    Author: CS Lim
 *   Purpose: Implement one of the fastest Reader/Writer locks in Windows
 */

#ifndef COMMON_H
#define COMMON_H

#if defined (_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#define COUNT_OF(x) ((sizeof(x)/sizeof(0[x])) / ((size_t)(!(sizeof(x) % sizeof(0[x])))))

const unsigned CACHELINE_SIZE = 64;

inline long AtomicIncrement (long volatile * addend)
{
    // Returns the resulting incremented value
    return _InterlockedIncrement(addend);
}

inline long AtomicDecrement (long volatile * addend)
{
    // Returns the resulting incremented value
    return _InterlockedDecrement(addend);
}


//===========================================================================
// Simple Win32 Crtical Section wrapper class
//===========================================================================
class CCritSect
{
private:
    CRITICAL_SECTION    m_crit;

public:
    CCritSect ();
    ~CCritSect ();
    void Enter ();
    void Leave ();
};

//===========================================================================
// CCritSect inline implementation
//===========================================================================
inline CCritSect::CCritSect ()
{
    InitializeCriticalSection(&m_crit);
}

inline CCritSect::~CCritSect ()
{
    DeleteCriticalSection(&m_crit);
}

inline void CCritSect::Enter ()
{
    EnterCriticalSection(&m_crit);
}

inline void CCritSect::Leave ()
{
    LeaveCriticalSection(&m_crit);
}

#endif /* COMMON_H */

//===========================================================================
// MIT License
//
// Copyright (c) 2010 by Chae Seong Lim
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