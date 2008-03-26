/*
 * Copyright (c) 2007-2008 Digital Bazaar, Inc.  All rights reserved.
 */
#include "db/rt/ThreadPool.h"

using namespace std;
using namespace db::rt;

ThreadPool::ThreadPool(unsigned int poolSize, size_t stackSize) :
   mThreadSemaphore(poolSize, true)
{
   // set stack size
   mThreadStackSize = stackSize;
   
   // default thread expire time to 0 (no expiration)
   mThreadExpireTime = 0;
}

ThreadPool::~ThreadPool()
{
   // terminate all threads
   terminateAllThreads();
}

PooledThread* ThreadPool::getIdleThread()
{
   PooledThread* rval = NULL;
   
   // lock lists for modification
   mListLock.lock();
   {
      while(rval == NULL && !mIdleThreads.empty())
      {
         rval = mIdleThreads.front();
         mIdleThreads.pop_front();
         
         // lock thread's job lock until it is assigned a job or marked expired
         // 
         // Note: This must be done because the thread could come out of
         // its idle state while we are checking it. If, when we check it,
         // the thread is not expired, then we might accidentally assign it
         // a job, and in the thread's code it will be interrupted just after
         // we check it, resulting in the assigned job never being handled.
         rval->getJobLock()->lock();
         if(rval->isExpired() || rval->isInterrupted())
         {
            // unlock thread, collect thread for clean up
            rval->getJobLock()->unlock();
            mThreads.remove(rval);
            mExpiredThreads.push_front(rval);
            rval = NULL;
         }
      }
      
      if(rval == NULL)
      {
         // create new thread and add it to the thread list
         rval = new PooledThread(this, getThreadExpireTime());
         mThreads.push_back(rval);
         
         // lock thread's job lock to prevent it from going idle before
         // its job is assigned
         rval->getJobLock()->lock();
      }
      
      // remove extra threads if applicable
      if(!mIdleThreads.empty())
      {
         // get the number of extra threads
         int extraThreads =
            mThreads.size() - mThreadSemaphore.getMaxPermitCount();
         
         // remove extra idle threads
         if(extraThreads > 0)
         {
            removeIdleThreads((unsigned int)extraThreads);
         }
      }
   }
   mListLock.unlock();
   
   return rval;
}

void ThreadPool::removeIdleThreads(unsigned int count)
{
   // assume list lock is engaged
   for(ThreadList::iterator i = mIdleThreads.begin();
       count > 0 && i != mIdleThreads.end(); count--)
   {
      // interrupt and erase threads
      PooledThread* t = *i;
      t->interrupt();
      i = mIdleThreads.erase(i);
      mThreads.remove(t);
      mExpiredThreads.push_back(t);
   }
}

void ThreadPool::cleanupExpiredThreads()
{
   if(!mExpiredThreads.empty())
   {
      // move threads to cleanup list to avoid potential deadlock
      // issues involved when threads are completing
      mListLock.lock();
      ThreadList cleanup(mExpiredThreads);
      mExpiredThreads.clear();
      mListLock.unlock();
      
      // join and cleanup threads
      for(ThreadList::iterator i = cleanup.begin(); i != cleanup.end(); i++)
      {
         (*i)->join();
         delete (*i);
      }
   }
}

void ThreadPool::runJobOnIdleThread(Runnable& job)
{
   // wait for other jobs to be assigned/terminated
   mJobLock.lock();
   {
      // get an idle thread
      PooledThread* t = getIdleThread();
      
      // set job
      t->setJob(&job);
      
      // unlock thread's job lock now that a job is assigned, so if it was
      // about to become idle or expire, it will pick up its new assignment
      t->getJobLock()->unlock();
      
      // if the thread hasn't started yet, start it
      while(!t->hasStarted())
      {
         if(!t->start(mThreadStackSize))
         {
            // thread might not start if maximum threads has been exceeded
            Thread::yield();
         }
      }
   }
   mJobLock.unlock();
   
   // clean up expired threads
   cleanupExpiredThreads();
}

void ThreadPool::runJobOnIdleThread(RunnableRef& job)
{
   // wait for other jobs to be assigned/terminated
   mJobLock.lock();
   {
      // get an idle thread
      PooledThread* t = getIdleThread();
      
      // set job
      t->setJob(job);
      
      // unlock thread's job lock now that job is assigned
      t->getJobLock()->unlock();
      
      // if the thread hasn't started yet, start it
      while(!t->hasStarted())
      {
         if(!t->start(mThreadStackSize))
         {
            // thread might not start if maximum threads has been exceeded
            // yield, try to start again later
            Thread::yield();
         }
      }
   }
   mJobLock.unlock();
   
   // clean up expired threads
   cleanupExpiredThreads();
}

bool ThreadPool::tryRunJob(Runnable& job)
{
   bool rval;
   
   // try to acquire a thread permit
   if(rval = mThreadSemaphore.tryAcquire())
   {
      // run the job on an idle thread
      runJobOnIdleThread(job);
   }
   
   return rval;
}

bool ThreadPool::tryRunJob(RunnableRef& job)
{
   bool rval;
   
   // try to acquire a thread permit
   if(rval = mThreadSemaphore.tryAcquire())
   {
      // run the job on an idle thread
      runJobOnIdleThread(job);
   }
   
   return rval;
}

void ThreadPool::runJob(Runnable& job)
{
   // acquire a thread permit
   if(mThreadSemaphore.acquire())
   {
      // run the job on an idle thread
      runJobOnIdleThread(job);
   }
}

void ThreadPool::runJob(RunnableRef& job)
{
   // acquire a thread permit
   if(mThreadSemaphore.acquire())
   {
      // run the job on an idle thread
      runJobOnIdleThread(job);
   }
}

void ThreadPool::jobCompleted(PooledThread* t)
{
   // clear the thread's job, there is no need to engage the thread's
   // job lock here because this method is called from the thread itself,
   // and it therefore cannot be idle, waiting to be notified
   t->setJob(NULL);
   
   // lock lists for modification
   mListLock.lock();
   {
      // add the thread to the front of the idle list, so it is more
      // likely to get assigned a job immediately
      mIdleThreads.push_front(t);
   }
   mListLock.unlock();
   
   // release thread permit
   mThreadSemaphore.release();
}

void ThreadPool::interruptAllThreads()
{
   // prevent new jobs from being assigned during interruption
   // ensure lists are not modified during interruption
   mJobLock.lock();
   mListLock.lock();
   {
      // interrupt all threads
      for(ThreadList::iterator i = mThreads.begin(); i != mThreads.end(); i++)
      {
         (*i)->interrupt();
      }
   }
   mJobLock.unlock();
   mListLock.unlock();
}

void ThreadPool::terminateAllThreads()
{
   // prevent new jobs from being assigned
   mJobLock.lock();
   {
      // prepare for list modification
      mListLock.lock();
      {
         // interrupt all the threads
         interruptAllThreads();
         
         // move all threads to the expired list
         mExpiredThreads.splice(mExpiredThreads.end(), mThreads);
      }
      mListLock.unlock();
      
      // clean up expired threads
      cleanupExpiredThreads();
      
      // clear the idle list, no list locking necessary since no jobs
      // are running and no new ones can be assigned while inside of the
      // job lock -- this clear is necessary in case the idle threads
      // list was updating while joining threads in the cleanup code
      mIdleThreads.clear();
   }
   mJobLock.unlock();
}

void ThreadPool::setPoolSize(unsigned int size)
{
   // ensure lists are not modified while removing idle threads
   mListLock.lock();
   {
      // Note: threads are created lazily so if the thread pool size
      // here is greater than the number of threads currently in
      // the pool, those threads will be created as they are needed
      // hence, we do not need to adjust for increases in the pool size
      // only for decreases
      
      // remove threads as necessary
      if(mThreads.size() > size)
      {
         removeIdleThreads(mThreads.size() - size);
      }
      
      // set semaphore permits
      mThreadSemaphore.setMaxPermitCount(size);
   }
   mListLock.unlock();
}

unsigned int ThreadPool::getPoolSize()
{
   return mThreadSemaphore.getMaxPermitCount();
}

void ThreadPool::setThreadStackSize(size_t stackSize)
{
   mThreadStackSize = stackSize;
}

size_t ThreadPool::getThreadStackSize()
{
   return mThreadStackSize;
}

void ThreadPool::setThreadExpireTime(unsigned long long expireTime)
{
   mThreadExpireTime = expireTime;
   
   // ensure lists are not modified while updating expire time
   mListLock.lock();
   {
      // update all existing threads
      for(ThreadList::iterator i = mThreads.begin(); i != mThreads.end(); i++)
      {
         (*i)->setExpireTime(expireTime);
      }
   }
   mListLock.unlock();
}

unsigned long long ThreadPool::getThreadExpireTime()
{
   return mThreadExpireTime;
}

unsigned int ThreadPool::getThreadCount()
{
   return mThreads.size();
}

unsigned int ThreadPool::getRunningThreadCount()
{
   unsigned int rval = 0;
   
   // ensure lists are not modified while getting difference
   mListLock.lock();
   {
      // subtract idle threads from total threads
      rval = mThreads.size() - mIdleThreads.size();
   }
   mListLock.unlock();
   
   return rval;
}

unsigned int ThreadPool::getIdleThreadCount()
{
   return mIdleThreads.size();
}
