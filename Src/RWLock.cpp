/**
 *      File: RWLock.cpp
 *    Author: CS Lim
 *   Purpose: Implement asymmetric reader writer lock based on
 *                Dmitriy Vyukov's implementation at www.1024cores.net
 *
 *   Notes:
 *      - Linearly scales up to number of readers same as number of cores
 *      - Only Vista or Windows Server 2008 and later are supported
 *        due to using FlushProcessWriteBuffers() API and thread local storage
 *        (in case of implemented inside a dll)
 *      - MAX_RWLOCK_READER_COUNT limits total number of threads
 *      - Reentrance support:
 *          R -> R (Re-entrance of Reader lock)
 *              Case #1 If no writer pending then reacquire reader lock.
 *              Case #2 If writer is pending then Wait for writer to complete.
 *
 *
 *          W -> R : Writer entering Reader lock allowed and keep maintain
 *                     writer status.
 *
 *          W -> W : Re-entrance of Writer lock allowed.
 *
 *  !!! IMPORTANT !!!
 *          R -> W : Upgrading read lock to write lock is "NOT"
 *                   supported and can cause deadlock.
 */

#include "stdafx.h"
#pragma  hdrstop


//===========================================================================
// Private variables
//===========================================================================

// Unique thread index [1 .. (MAX_RWLOCK_READER_COUNT - 1)]
static __declspec(thread) unsigned t_curThreadIndex;
static long s_threadIndex = 0;


//===========================================================================
// CRWLock implementation
//===========================================================================
CRWLock::CRWLock() {
    m_writerPending = false;

    for (unsigned i = 0; i < COUNT_OF(m_readers); i++)
        m_readers[i] = false;
}

void CRWLock::EnterRead() {
    // Initialize per-thread index if this is first call from current thread
    if (t_curThreadIndex == 0) {
        t_curThreadIndex = AtomicIncrement(&s_threadIndex);
        _ASSERT(t_curThreadIndex < MAX_RWLOCK_READER_COUNT);
    }

    m_readers[t_curThreadIndex] = true;

    // Pending write lock exists?
    //    No explicit #StoreLoad but it will be implicitly executed
    //    by FlushProcessWriteBuffers() in EnterWrite()
    if (m_writerPending) {
        // If writer is pending then signal that we see it
        // and wait for writer to complete
        m_readers[t_curThreadIndex] = false;

        m_critSect.Enter();
        m_readers[t_curThreadIndex] = true;
        m_critSect.Leave();
    }

    // Prevent compiler re-ordering
    // Need to order caller code inside critical section
    _ReadWriteBarrier();
}

void CRWLock::LeaveRead() {
    _ASSERT(t_curThreadIndex != 0);

    // Prevent compiler re-ordering
    // Need to order caller code inside critical section
    _ReadWriteBarrier();
    m_readers[t_curThreadIndex] = false;
}

void CRWLock::EnterWrite() {
    if (t_curThreadIndex == 0) {
        t_curThreadIndex = AtomicIncrement(&s_threadIndex);
        _ASSERT(t_curThreadIndex < MAX_RWLOCK_READER_COUNT);
    }

    // Writer enters critical section
    m_critSect.Enter();

    // Signal we (writer) are waiting for reader(s) to complete
    m_writerPending = true;

    // FlushProcessWriteBuffers() API (From MSDN):
    //  - Implicitly execute full memory barrier on all other processors.
    //  - Generates an interprocessor interrupt (IPI) to all processors that
    //    are part of the current process affinity.
    //  - Uses IPI to "synchronously" signal all processors.
    //  - It guarantees the visibility of write operations performed on one
    //    processor to the other processors.
    //  - Supported since Windows Vista and Windows Server 2008.
    FlushProcessWriteBuffers();

    // Here we are sure that:
    //       (1) writer will see (m_readers[i]    == true)
    //    or (2) reader will see (m_writerPending == true)
    // so no race conditions
    for (unsigned i = 0; i < COUNT_OF(m_readers); i++) {
        // Wait for all readers to complete
        while (m_readers[i]) {
            // Yield CPU to another thread
            // @@@ TODO: Backoff
            SwitchToThread();
        }
    }
}

void CRWLock::LeaveWrite() {
    _ASSERT(t_curThreadIndex != 0);

    m_writerPending = false;
    m_critSect.Leave();
}

void InitRWLock() {
    s_threadIndex = 0;
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
