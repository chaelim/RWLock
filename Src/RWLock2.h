/**
 *      File: RWLock2.h
 *    Author: CS Lim
 *   Purpose: Reader writer lock using Per-Processor data
 */

#ifndef CRWLOCK2_H
#define CRWLOCK2_H

#if defined (_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

//===========================================================================
// CRWLock Declaration
//===========================================================================
class CRWLock2
{
private:
    SRWLOCK	*	m_lock;

public:
    CRWLock2 ();
	~CRWLock2 ();
    void EnterRead ();
    void EnterWrite ();
    void LeaveRead ();
    void LeaveWrite ();
};


#endif /* CRWLOCK2_H */

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