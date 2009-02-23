#include <core.hpp>
#include "concurrency.hpp"


////////// Job //////////

/// Construct a Job.
Job::Job()
    : _runLock(*this), _nodeLock(*this), _next(0), _prev(0)
{
    
}

/// Destroy Job.
Job::~Job()
{
    
}

/// Exception safe wrapper for run().
/// Allowing exceptions to propagate further up the call chain than this would
/// leave the locks in the JobPool in an inconsistent state and cause the entire
/// system to fail. Instead we catch exceptions here when they occur and simply
/// terminate the %Job.
Job::RetType Job::safeRun()
{
    try {
        return run();
    } catch (std::exception& e) {
        Log::log->error(e.what());
        return FINISH;
    }
}

/// Indicates whether the job has changeable stage.
/// In the case that a job does not write to any of its state, it should 
/// override this method to return true. The job pool will then allow it to be
/// run by multiple workers concurrently. The main purpose of this is to 
/// guarantee jobs such as \em sleep are run once every round by all workers.
bool Job::readOnly()
{
    return false;
}


////////// EmptyJob //////////

/// Immediately returns Job::YIELD.
Job::RetType EmptyJob::run()
{
    return YIELD;
}


////////// JobPool //////////

/// Construct a JobPool.
JobPool::JobPool()
    : _head(&_empty1), _tail(&_empty2), _count(0)
{
    _empty1._next = &_empty2;
    _empty2._prev = &_empty1;
}

/// Destroy %JobPool and all its jobs.
/// Before calling this method users should ensure all Worker objects using the
/// %JobPool have been terminated.
/// \pre There are no Worker objects using this %JobPool.
JobPool::~JobPool()
{
    while (_tail != &_empty2) {
        _tail = _tail->_prev;
        Job::Ptr tempPtr(_tail->_next);
    }
}

/// Add a job to the pool.
/// \param job The job to add.
void JobPool::add(Job::Ptr job)
{
    // Get rw lock on the tail node.
    while (_tail->_nodeLock.rwTryLock() == 0);
    
    // Tag job onto end of list.
    _tail->_next = job.release();
    _tail->_next->_prev = _tail;
    _tail = _tail->_next;
    _count++;
    
    // Unlock old tail node.
    _tail->_prev->_nodeLock.rwUnlock();
}

/// Finds a Job that needs running and runs it.
/// This method searches the pool for a %Job that needs to be done. It does this
/// by attempting to acquire the run lock on successive jobs. When it manages to
/// get this lock it can safely run that job. Depending on the return value of
/// the Job::run() method it will either release the run lock and return or
/// remove the job from the pool then return.
/// \param last The last job run by the calling thread.
/// \return The job that was run this time.
Job* JobPool::runNextJob(Job* last)
{
    Job* curr = 0;
    
    if (last == 0) {
        last = _head;
        last->_nodeLock.roWaitLock();
    }
    
    do {
        curr = (last->_next != 0 ? last->_next : _head);
        curr->_nodeLock.roWaitLock();
        last->_nodeLock.roUnlock();
        last = curr;
    } while (!acquireRunLock(curr));
    
    if (curr->safeRun() == Job::YIELD) {
        releaseRunLock(curr);
    } else {
        curr->_nodeLock.roUnlock();
        removeJobFromList(curr);
        releaseRunLock(curr);
        Job::Ptr tempPtr(curr);
        curr = _head;
        _count--;
    }
    
    return curr;
}

/// Releases a node read-only lock on the specified job.
/// The %JobPool implementation has workers move around the job list rather than
/// manipulating the list to give jobs to workers. As such a Worker holds a lock
/// on the last Job it ran. When it gains a lock on its next job this lock will
/// be removed. This method is provided so that the lock a Worker holds on its
/// last job can be removed before that %Worker terminates. Failure by a %Worker
/// to call this method before terminating may result in deadlock if jobs are
/// removed later.
/// \param job The job to release.
void JobPool::releaseJob(Job* job)
{
    if (job != 0)
        job->_nodeLock.roUnlock();
}

/// Used to find out the job count.
/// \return Number of jobs in pool.
int JobPool::count()
{
    return _count;
}

/// Attempt to lock the job so that other workers do not run it.
/// This method tries to get the run lock for the specified job. If the job is
/// read-only (as determined by the Job::readOnly() method) it does nothing. It
/// could acquire an ro lock but this is not necessary because the job's state
/// will never change anyway.
/// \param job The job to lock.
/// \return Success of locking operation.
bool JobPool::acquireRunLock(Job* job)
{
    return (job->readOnly() || job->_runLock.rwTryLock() != 0);
}

/// Release the lock on a job so other workers may run it.
/// \param job The job to unlock.
/// \pre The job must have been lock previously with JobPool::acquireRunLock().
void JobPool::releaseRunLock(Job* job)
{
    if (!job->readOnly())
        job->_runLock.rwUnlock();
}

/// Removes the specified job from the pool's linked list.
/// This method will first acquire node read-write locks on the previous, 
/// specified and next job, in that order. Therefore the current thread must not
/// be holding node locks on any of those jobs or dead-lock will occur. Once the
/// locks are held it is able to safely remove the specified job from the list.
/// Finally it releases all held locks in the order they were acquired.
/// \param job The job to remove from the linked list.
/// \pre The calling thread holds no node locks on the previous, specified and
///      next jobs.
void JobPool::removeJobFromList(Job* job)
{
    // Lock previous job node.
    if (job->_prev != 0)  // Should this be in the while guard too?
        while (job->_prev->_nodeLock.rwTryLock() == 0);
    
    // Lock current job node.
    job->_nodeLock.rwWaitLock();
    
    // Lock next job node.
    if (job->_next != 0)
        job->_next->_nodeLock.rwWaitLock();
    
    // Remove current job node from list.
    if (job->_prev != 0)
        job->_prev->_next = job->_next;
    if (job->_next != 0)
        job->_next->_prev = job->_prev;
    
    // Set new tail if necessary.
    if (job == _tail)
        _tail = job->_prev;
    
    // Unlock previous job node.
    if (job->_prev != 0)
        job->_prev->_nodeLock.rwUnlock();
    
    // Unlock current job node.
    job->_nodeLock.rwUnlock();
    
    // Unlock next job node.
    if (job->_next != 0)
        job->_next->_nodeLock.rwUnlock();
}


////////// Worker //////////

/// Create a %Worker to do jobs in the specified JobPool.
/// \param jobs The JobPool from which to do jobs.
Worker::Worker(JobPool& jobs)
    : _jobs(jobs), _terminate(false)
{
    pthread_create(&_thread, 0, &threadMain, this);
}

/// Destroy %Worker object.
/// Calling this will cause the thread to terminate after the current job 
/// yields. The destructor will block until the thread has been destroyed.
Worker::~Worker()
{
    terminate();
    
    pthread_join(_thread, 0);
}

/// Manually terminate %Worker thread.
/// This method causes the thread to terminate after the current job yields. It
/// returns immediately, so the caller should not expect the thread to be
/// destroyed right away. The %Worker object is still valid after the thread
/// has been terminated but has no use. The thread cannot be restarted since
/// this task is performed during a %Worker's construction.
void Worker::terminate()
{
    _terminate = true;
}

/// Get identity of running worker.
/// This method can be called statically to get an identifier unique to the 
/// running worker thread.
/// \return Identifier for the current worker.
Worker::Identifier Worker::self()
{
    return pthread_self();
}

/// Main function executed by thread.
/// \param args Pointer to %Worker object.
/// \return Always zero.
void* Worker::threadMain(void* args)
{
    Job* job = 0;
    Worker* worker = reinterpret_cast<Worker*>(args);
    
    while (!worker->_terminate)
        job = worker->_jobs.runNextJob(job);
    
    worker->_jobs.releaseJob(job);
    
    return 0;
}
