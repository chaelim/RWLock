/**
 *      File: RWLockTest.cpp
 *    Author: CS Lim
 *   Purpose: Mmultithreaded R/W lock performance testing program
 * 
 */


#include "stdafx.h"
#pragma hdrstop

#pragma comment(lib, "RWLock.lib")

using namespace std;

template <typename T, size_t N>
char ( &_ArraySizeHelper( T (&array)[N] ))[N];

#define countof( array ) (sizeof( _ArraySizeHelper( array ) ))

#define CACHE_LINE  64
#define CACHE_ALIGN __declspec(align(CACHE_LINE))

//===========================================================================
// R/W lock abstract class
//===========================================================================
struct __declspec(novtable) RwLock
{
public:
    virtual void EnterRead () = 0;
    virtual void LeaveRead () = 0;
    virtual void EnterWrite () = 0;
    virtual void LeaveWrite () = 0;

    virtual char* GetName () = 0;
};

//===========================================================================
// Windows slim reader/writer (SRW) lock.
//===========================================================================
class CSRWLock : public RwLock
{
public:
    CSRWLock ()
    {
        InitializeSRWLock(&m_lock);
    }
    void EnterRead ()
    {
        AcquireSRWLockShared(&m_lock);
    }
    void LeaveRead ()
    {
        ReleaseSRWLockShared(&m_lock);
    }
    void EnterWrite ()
    {
        AcquireSRWLockExclusive(&m_lock);
    }
    void LeaveWrite ()
    {
        ReleaseSRWLockExclusive(&m_lock);
    }

    char * GetName ()
    {
        return "SRWLock";
    }

private:
    SRWLOCK m_lock;
};

class CAsymRWLockTest : public RwLock
{
public:
    CAsymRWLockTest ()
    {
    }
    void EnterRead ()
    {
        m_lock.EnterRead();
    }
    void LeaveRead ()
    {
        m_lock.LeaveRead();
    }
    void EnterWrite ()
    {
        m_lock.EnterWrite();
    }
    void LeaveWrite ()
    {
        m_lock.LeaveWrite();
    }

    char * GetName ()
    {
        return "Asymmetric";
    }

private:
    CRWLock m_lock;
};

class CPerProcRWLockTest : public RwLock
{
public:
    CPerProcRWLockTest ()
    {
    }
    void EnterRead ()
    {
        m_lock.EnterRead();
    }
    void LeaveRead ()
    {
        m_lock.LeaveRead();
    }
    void EnterWrite ()
    {
        m_lock.EnterWrite();
    }
    void LeaveWrite ()
    {
        m_lock.LeaveWrite();
    }

    char * GetName ()
    {
        return "Per-Proc";
    }

private:
    CRWLock2 m_lock;
};

//===========================================================================
// Using Critical Section to use it as base line performance and can compare
// RWLock's worst case (100% writer) with simple critical section.
//===========================================================================
class CCritsectRwLock : public RwLock
{
public:
    void EnterRead ()
    {
        m_lock.Enter();
    }
    void LeaveRead ()
    {
        m_lock.Leave();
    }
    void EnterWrite ()
    {
        m_lock.Enter();
    }
    void LeaveWrite ()
    {
        m_lock.Leave();
    }

    char * GetName ()
    {
        return "Critsect";
    }

private:
    CCritSect m_lock;
};


//===========================================================================
// Test Consts and Globals
//===========================================================================
const unsigned MAX_RWLOCKS = 12;
const unsigned MAX_THREADS = 128;
const unsigned TOTAL_TEST_TIME_MS = 5000;
const unsigned TOTAL_TEST_ITEM = 10000;

struct TestItem
{
    volatile CACHE_ALIGN unsigned data;
};

typedef list<TestItem *> TEST_LIST;

struct TestCase
{
    char *      name;
    float        readRate;    // Read ratio
    RwLock *    rwLocks[MAX_RWLOCKS];

};

//===========================================================================
// Per thread stat
//===========================================================================
struct ThreadStat
{
    RwLock *        rwLock;
    float            readRate;
    int                threadIdx;
    __int64            startTime;
    __int64         endTime;
    ULONG64            cpuCycles;
    unsigned        iterRead;
    unsigned        iterWrite;
};

struct ThreadStatAligned : ThreadStat
{
    // Padded data for cache line align
    uint8_t    pad[
        CACHELINE_SIZE - (sizeof(ThreadStat) % CACHELINE_SIZE)
    ];
};

long            g_numProcessors = 0;
volatile long    g_totalThreads = 0;

volatile long    g_readyWaitThreads;
volatile bool    g_exit = false;
HANDLE            g_runTestEvent;

CACHE_ALIGN bool                g_runTest = false;
CACHE_ALIGN CAsymRWLockTest        g_asymRWLock;
CACHE_ALIGN CPerProcRWLockTest    g_perProcRWLock;
CACHE_ALIGN CSRWLock            g_slimRWLock;
CACHE_ALIGN CCritsectRwLock        g_critsectRwLock;
CACHE_ALIGN TEST_LIST            g_testList;
CACHE_ALIGN TEST_LIST            g_freeList;

CACHE_ALIGN HANDLE g_threads[MAX_THREADS];
CACHE_ALIGN ThreadStatAligned g_threadStats[MAX_THREADS];

TestCase g_testCases[] =
{
    { 
        "R(99%)/W(1%)",
        (float)0.99f,
        { 
            &g_asymRWLock, &g_perProcRWLock,
            &g_slimRWLock, &g_critsectRwLock, 
        }
    },
    { 
        "R(95%)/W(5%)",
        (float)0.95f,
        { 
            &g_asymRWLock, &g_perProcRWLock,
            &g_slimRWLock, &g_critsectRwLock, 
        }
    },
    { 
        "R(90%)/W(10%)",
        (float)0.99f,
        { 
            &g_asymRWLock, &g_perProcRWLock,
            &g_slimRWLock, &g_critsectRwLock, 
        }
    },
    { 
        "R(80%)/W(20%)",
        (float)0.80f,
        { 
            &g_asymRWLock, &g_perProcRWLock,
            &g_slimRWLock, &g_critsectRwLock, 
        }
    },
    { 
        "R(70%)/W(30%)",
        (float)0.70f,
        { 
            &g_asymRWLock, &g_perProcRWLock,
            &g_slimRWLock, &g_critsectRwLock, 
        }
    },
    { 
        "R(50%)/W(50%)",
        (float)0.50f,
        { 
            &g_asymRWLock, &g_perProcRWLock,
            &g_slimRWLock, &g_critsectRwLock, 
        }
    },
    { 
        "R(30%)/W(70%)",
        (float)0.30f,
        { 
            &g_asymRWLock, &g_perProcRWLock,
            &g_slimRWLock, &g_critsectRwLock, 
        }
    },
    { 
        "R(10%)/W(90%)",
        (float)0.10f,
        { 
            &g_asymRWLock, &g_perProcRWLock,
            &g_slimRWLock, &g_critsectRwLock, 
        }
    },
};



//===========================================================================
// Timing functions
//===========================================================================
static __int64 GetPerfCounters ()
{
    LARGE_INTEGER li;
    QueryPerformanceCounter(&li);
    return li.QuadPart;
}

static __int64 GetPerfFreq ()
{
    LARGE_INTEGER li;
    QueryPerformanceFrequency(&li);
    return li.QuadPart;
}

static void ReadList()
{
    volatile unsigned seq = g_testList.front()->data;
    TEST_LIST::iterator it;
    for (it = g_testList.begin() ; it != g_testList.end(); it++)
    {
        if ((*it)->data != seq)
        {
            _ASSERT(false);
        }
    }
}

static void WriteList()
{
    TestItem * item;
    unsigned data;
    item = g_testList.front();
    data = item->data;

    g_testList.pop_front();
    g_freeList.push_back(item);
    item->data = (unsigned)-1;

    item = g_freeList.front();
    g_freeList.pop_front();
    g_testList.push_back(item);
    item->data = data;
}

static DWORD WINAPI ThreadProc (LPVOID lpParameter)
{
    ThreadStat * threadStat = (ThreadStat *) lpParameter;

    AtomicIncrement(&g_readyWaitThreads);
    WaitForSingleObject(g_runTestEvent, INFINITE);
    AtomicDecrement(&g_readyWaitThreads);

    CRandomMersenne ranObject(threadStat->threadIdx);

    float readRate  = threadStat->readRate;
    float rnd        = (float)ranObject.Random();

    // Run test
    threadStat->startTime = GetPerfCounters(); 
    QueryThreadCycleTime(GetCurrentThread(), &threadStat->cpuCycles);
    while (g_runTest)
    {
        if (rnd < readRate || readRate == 1.0f)
        {
            threadStat->rwLock->EnterRead();
            rnd    = (float)ranObject.Random();
            ReadList();
            threadStat->iterRead++;
            threadStat->rwLock->LeaveRead();

        }
        else
        {
            threadStat->rwLock->EnterWrite();
            rnd    = (float)ranObject.Random();
            ReadList();
            WriteList();
            threadStat->iterWrite++;
            threadStat->rwLock->LeaveWrite();
        }

    }
    threadStat->endTime = GetPerfCounters();
    ULONG64 cpuCycles;
    QueryThreadCycleTime(GetCurrentThread(), &cpuCycles);
    threadStat->cpuCycles = cpuCycles - threadStat->cpuCycles;

    return 0;
}


void InitTest ()
{
    // Init test data. Add all test items to free list.
    TestItem * item;
    for (int i = 0; i < TOTAL_TEST_ITEM; i++)
    {
        item = (TestItem *)_aligned_malloc(sizeof(TestItem), CACHE_LINE);
        g_freeList.push_back(item);
        item++;
    }

    g_runTestEvent  = CreateEvent(NULL, true, false, NULL);

    SYSTEM_INFO info;
    GetSystemInfo(&info);
    g_numProcessors = info.dwNumberOfProcessors; 
    g_totalThreads  = info.dwNumberOfProcessors * 2;
    printf("Number of Processors: %d\n", info.dwNumberOfProcessors);
}

void InitThreads (RwLock * rwLock, float readRate, int threadCount)
{
    ZeroMemory(&g_threadStats, sizeof(g_threadStats));
    for (int i = 0; i < threadCount; i++)
    {
        g_threadStats[i].threadIdx = i;
        g_threadStats[i].rwLock = rwLock;
        g_threadStats[i].readRate = readRate;

        DWORD threadId;
        g_threads[i] = (HANDLE) CreateThread(
            (LPSECURITY_ATTRIBUTES) 0,
            0,    // stack size
            ThreadProc,
            (LPVOID)&g_threadStats[i],    // argument
            0,
            &threadId
        );

        // Set test thread priority higher
        SetThreadPriority(g_threads[i], THREAD_PRIORITY_ABOVE_NORMAL);
    }
}

void RunOneTest (
    unsigned    testId,
    RwLock *    rwLock,
    float        readRate,
    unsigned    workSize,
    unsigned    threadCount
) {
    // Create test threads
    InitThreads(rwLock, readRate, threadCount);

    // Wait until all threads are redy and signal to start testing
    while ((unsigned)g_readyWaitThreads < threadCount)
    {
        Sleep(100);
    }
    SetEvent(g_runTestEvent);

    ////////////////
    // Start test //
    ////////////////
    g_runTest = true;
    Sleep(TOTAL_TEST_TIME_MS);

    // Cause all threads exit
    g_runTest = false;
    
    // Wait all thread exit
    WaitForMultipleObjects(threadCount, g_threads, true, INFINITE);
    ResetEvent(g_runTestEvent);
    //////////////
    // End test //
    //////////////



    // Collect test result stored in per-thread data
    __int64 procCounter = 0;
    __int64 totalReads  = 0;
    __int64 totalWrties = 0;
    ULONG64    totalCpuCycles = 0;
    for (unsigned i = 0; i < threadCount; i++)
    {
        ThreadStatAligned *threadStat = &g_threadStats[i];
        if (threadStat->startTime)
        {
            procCounter += threadStat->endTime - threadStat->startTime;
            totalCpuCycles += threadStat->cpuCycles;
            totalReads  += threadStat->iterRead;
            totalWrties += threadStat->iterWrite;

            // clear threads stats for next test
            threadStat->startTime = 0;
            threadStat->endTime   = 0;
            threadStat->cpuCycles = 0;
            threadStat->iterRead  = 0;
            threadStat->iterWrite = 0;
        }
    }

    // Average total perf counter for a thread
    float freq = (float) procCounter / (float) threadCount;
    // Frequency per second
    freq = freq / (float)GetPerfFreq();               

    printf(
        "%3d, %5d, %12s,  %2d, %10.1f, %10.1f, %10.1f, %8.1f\n",
        testId,
        workSize,
        rwLock->GetName(),
        threadCount,
        (float)totalReads  / freq,
        (float)totalWrties / freq,
        (float)(totalReads + totalWrties) / freq,
        (float)((double)totalCpuCycles / (totalReads + totalWrties))
    );
}

static void RunTests ()
{
    SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_TIME_CRITICAL);

    __int64 startCounter = GetPerfCounters();

    int testId = 0;

    // For differnt dummy load sizes
    for (int worksize = 100; worksize <= 1000; worksize *= 2)
    {
        TestItem * item;

        // Move all to free list
        while (!g_testList.empty())
        {
            item = g_testList.front();
            g_testList.pop_front();
            g_freeList.push_back(item);
        }

        // Add worksize items (dummy load)
        for (int i = 0; i < worksize; i++)
        {
            item = g_freeList.front();
            g_freeList.pop_front();

            item->data = worksize;
            g_testList.push_back(item);
        }    

        // For each test to run
        for (int i = 0; i < countof(g_testCases); i++)
        {
            printf("=== %s ===\n", g_testCases[i].name);
                    //"%3d, %5d, %12s  %2d, %10.1f, %10.1f, %10.1f, %I64d\n",
            printf("      Work    Name     Threads  Reads/sec  Writes/sec   Total/sec   CPU/Op\n");

            // Run this test for each desired thread count
            for (int threadCount = 1; threadCount <= g_totalThreads; threadCount++)
            {
                for (RwLock ** rwLock = g_testCases[i].rwLocks; *rwLock != NULL; rwLock++)
                {
                    testId++;
                    RunOneTest(
                        testId,
                        *rwLock,
                        g_testCases[i].readRate,
                        worksize,
                        threadCount
                    );
                }

                InitRWLock();
            }
        }

        printf("\n");
    }

    float totalRunTime = (float)(GetPerfCounters() - startCounter) / (float)GetPerfFreq();
    printf ("Done.  Duration: %10.4f\n", totalRunTime);
}

void Cleanup()
{
    TestItem * item;
    while (!g_testList.empty())
    {
        item = g_testList.front();
        free(item);
    }
    while (!g_freeList.empty())
    {
        item = g_freeList.front();
        free(item);
    }
}

int main(int argc, char * argv[])
{
    InitTest();
    RunTests();
    Cleanup();
    return 0;
}

