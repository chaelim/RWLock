/**
 *      File: RWLock.h
 *    Author: CS Lim
 *   Purpose: Implement asymmetric reader writer lock in Windows
 */

#ifndef CRWLOCK_H
#define CRWLOCK_H

#if defined (_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

// Maximum supported reader threads
const unsigned MAX_RWLOCK_READER_COUNT = 64;

//===========================================================================
// CRWLock Declaration
//===========================================================================
class CRWLock
{
private:
    CCritSect       m_critSect; 
    unsigned        m_ownerThreadId;

    // Private flag for every reader
    volatile uint8_t    m_readers[MAX_RWLOCK_READER_COUNT]; 
    volatile bool       m_writerPending;

public:
    CRWLock ();
    void EnterRead ();
    void EnterWrite ();
    void LeaveRead ();
    void LeaveWrite ();
};

void InitRWLock ();

#endif /* CRWLOCK_H */

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