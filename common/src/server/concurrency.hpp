/// \file concurrency.hpp
/// \brief Framework for a concurrent environment.
/// \author Ben Radford 
/// \date 19th September 2007
///
/// Copyright (c) 2007 Ben Radford.
///
/// Modifications (most recent first):
/// - 27/10/07 Implemented read-only run jobs and added thread local storage.
/// - 08/10/07 Added the Worker::self() method.


#ifndef CONCURRENCY_HPP
#define CONCURRENCY_HPP


#include <list>
#include <memory>
#include <pthread.h>
#include "lock.hpp"


/// Represents a job that needs to be run.
/// In this framework tasks that need to be completed should inherit from %Job
/// and override the abstract run() method. The value returned from run() 
/// indicates what action should  be taken. Longer running jobs, such as those
/// serving clients will likely return YIELD most of the time. This causes the
/// job to be suspended so that other jobs have a chance to run but the job will
/// be run again later. A job should return FINISH to indicate that it has
/// completed its task.
class Job {
    public:
        friend class JobPool;
        typedef std::unique_ptr<Job> Ptr;
        
        enum RetType {
            YIELD,  ///< The job is temporarily yielding.
            FINISH  ///< The job has been completed.
        };
        
        Job();
        virtual ~Job();
        virtual RetType run() = 0;
        
    private:
        RetType safeRun();
        virtual bool readOnly();
        
        Lock<Job> _runLock;   ///< Lock for running the job.
        Lock<Job> _nodeLock;  ///< Lock for accessing the linked list node.
        
        Job* _next;  ///< Next job in the JobPool linked list.
        Job* _prev;  ///< Previous job in the JobPool linked list.
};


/// Special Job which does nothing.
/// %EmptyJob is required internally by JobPool. When run it immediately returns
/// YIELD.
class EmptyJob : public Job {
    public:
        virtual RetType run();
};


/// A %JobPool holds Job objects so that Worker objects may run them.
/// After a %Job has been added it is owned by the %JobPool and will be 
/// destroyed when the %Job returns Job::FINISH from its Job::run() method. 
class JobPool {
    public:
        JobPool();
        ~JobPool();
        
        void add(Job::Ptr job);
        Job* runNextJob(Job* last = 0);
        void releaseJob(Job* job);
        
        int count();
        
    private:
        bool acquireRunLock(Job* job);
        void releaseRunLock(Job* job);
        void removeJobFromList(Job* job);
        
        Job* _head;  ///< First job in linked list.
        Job* _tail;  ///< Last job in linked list.
        
        EmptyJob _empty1;  ///< Used for internal implementation.
        EmptyJob _empty2;  ///< Used for internal implementation.
        
        int _count;  ///< Number of jobs in pool.
};


/// Represents the entity which runs jobs.
/// %Worker is essentially just a thread wrapper. Creating a new %Worker object
/// causes a new thread to be created. The thread will run jobs in the JobPool
/// specified in the %Worker constructor. A %Worker thread can be destroyed by
/// calling the terminate() method. This will end the thread after the job it 
/// is currently running yields. Note that the destructor will automatically
/// call terminate and then wait for the thread to end. Therefore if a %Worker
/// object goes out of scope its thread will be destroyed. %Worker objects do
/// not own jobs themselves so the user is free to add and remove workers as 
/// they wish. One heuristic for the number of workers to have is the number of
/// processor cores available. Be aware that adding more workers than there are
/// jobs will lead to wasted CPU time because some workers will just spend time
/// fruitlessly looking for a job to do.
class Worker {
    public:
        typedef pthread_t Identifier;      ///< Unique to each worker.
        typedef pthread_key_t StorageKey;  ///< Key for local storage.
        
        Worker(JobPool& jobs);
        ~Worker();
        
        void terminate();
        
        static Identifier self();
        
    private:
        Worker(const Worker&);             ///< This method is undefined.
        Worker& operator=(const Worker&);  ///< This method is undefined.
        
        static void* threadMain(void* args);
        pthread_t _thread;  ///< Thread identifier.
        
        JobPool& _jobs;   ///< JobPool object from which to do jobs.
        bool _terminate;  ///< Indicates whether to stop worker thread.
};


#endif  // CONCURRENCY_HPP
