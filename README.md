# Evaluation of recently developed Reader Writer Locks
_(Implemented recently designed or proposed reader writer locks on Windows platform and measured performance)_

## Windows Reader Writer Lock
* Slim Reader/Writer (SRW) Locks

## Asymmetric reader/writer lock
* Original C++ implementation by Dmitriy Vyukov at [www.1024cores.net](http://www.1024cores.net/home/lock-free-algorithms/reader-writer-problem)
* This asymmetric reader writer lock is optimized for readers. In most cases, readers don't need to enter any mutex or critical section. In other words, readers are linearly scale and that is not very common.
* It's using **[FlushProcessWriteBuffers](http://msdn.microsoft.com/en-us/library/windows/desktop/ms683148\(v=vs.85\).aspx)** API only available since **Windows Vista**.
* This is most beneficial for a program running on many-core machine dealing with high concurrency with majority of readers and few writers.

## Reader Writer lock using Per-proc data
* Similar to distributed reader writer lock at [www.1024cores.net] (http://www.1024cores.net/home/lock-free-algorithms/reader-writer-problem/distributed-reader-writer-mutex)
* To reduce data contention, using per-processor SRW and using current processor number to distribute readers.

## References
* [Reader Writer locks](http://en.wikipedia.org/wiki/Readers%E2%80%93writer_lock) particulary useful if you have many readers but only few writers.
