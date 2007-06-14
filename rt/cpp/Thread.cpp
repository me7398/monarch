/*
 * Copyright (c) 2007 Digital Bazaar, Inc.  All rights reserved.
 */
#include "Thread.h"

using namespace std;
using namespace db::rt;

// initialize current thread key parameters
pthread_once_t Thread::CURRENT_THREAD_KEY_INIT = PTHREAD_ONCE_INIT;
pthread_key_t Thread::CURRENT_THREAD_KEY;

Thread::Thread(Runnable* runnable, std::string name)
{
   // initialize POSIX thread attributes
   pthread_attr_init(&mPThreadAttributes);
   
   // make thread joinable
   pthread_attr_setdetachstate(&mPThreadAttributes, PTHREAD_CREATE_JOINABLE);
   
   // make thread cancelable upon joins/waits/etc
   pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
   pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, NULL);
   
   // store runnable
   mRunnable = runnable;
   
   // set name
   mName = name;
   
   // thread is not alive yet
   mAlive = false;
   
   // thread is not interrupted yet
   mInterrupted = false;
   
   // thread is not started yet
   mStarted = false;
}

Thread::~Thread()
{
   // destroy the POSIX thread attributes
   pthread_attr_destroy(&mPThreadAttributes);
}

void Thread::createCurrentThreadKey()
{
   // create the thread key for obtaining the current thread
   pthread_key_create(&CURRENT_THREAD_KEY, NULL);
}

void* Thread::execute(void* thread)
{
   // run the passed thread's run() method
   Thread* t = (Thread*)thread;
   t->run();
   
   // exit thread
   pthread_exit(NULL);
   return NULL;
}

bool Thread::start()
{
   bool rval = false;
   
   // create the POSIX thread
   int rc = pthread_create(
      &mPThread, &mPThreadAttributes, execute, (void*)this);
      
   // if the thread was created successfully, return true
   if(rc == 0)
   {
      // thread has started
      mStarted = true;
      rval = true;
   }
   
   return rval;
}

bool Thread::isAlive()
{
   return mAlive;
}

void Thread::interrupt()
{
   // set interrupted flag
   mInterrupted = true;
   
   // cancel thread
   pthread_cancel(mPThread);
}

bool Thread::isInterrupted()
{
   return mInterrupted;
}

bool Thread::hasStarted()
{
   return mStarted;
}

void Thread::join()
{
   // join thread, wait for it to detach/terminate indefinitely
   int status;
   pthread_join(mPThread, (void **)&status);
}

void Thread::detach()
{
   // detach thread
   pthread_detach(mPThread);
}

void Thread::run()
{
   // create the current thread key if it hasn't been created yet
   pthread_once(&CURRENT_THREAD_KEY_INIT, Thread::createCurrentThreadKey);
   
   // set thread specific data to "this" pointer
   pthread_setspecific(CURRENT_THREAD_KEY, this);
   
   // thread is alive
   mAlive = true;
   
   // if a Runnable if available, use it
   if(mRunnable != NULL)
   {
      mRunnable->run();
   }
   
   // thread is no longer alive
   mAlive = false;
}

void Thread::setName(string name)
{
   mName = name;
}

const string& Thread::getName()
{
   return mName;
}

Thread* Thread::currentThread()
{
   // get a pointer to the current thread
   return (Thread*)pthread_getspecific(CURRENT_THREAD_KEY);
}

bool Thread::interrupted()
{
   bool rval = false;
   
   // get the current thread
   Thread* thread = Thread::currentThread();
   
   if(thread != NULL)
   {
      rval = thread->isInterrupted();

      // clear interrupted flag
      thread->mInterrupted = false;
   }
   
   return rval;
}

void Thread::sleep(unsigned long time) throw(InterruptedException)
{
   // create a lock object
   Object lock;
   
   lock.lock();
   {
      // wait on the lock object for the specified time
      lock.wait(time);
   }
   lock.unlock();
}

void Thread::yield()
{
   pthread_yield();
}
