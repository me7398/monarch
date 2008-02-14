/*
 * Copyright (c) 2007 Digital Bazaar, Inc.  All rights reserved.
 */
#include "db/net/Connection.h"
#include "db/util/Math.h"

using namespace db::io;
using namespace db::net;
using namespace db::rt;
using namespace db::util;

ConnectionOutputStream::ConnectionOutputStream(Connection* c)
{
   mConnection = c;
   mBytesWritten = 0;
   mThread = NULL;
}

ConnectionOutputStream::~ConnectionOutputStream()
{
}

bool ConnectionOutputStream::write(const char* b, int length)
{
   bool rval = true;
   
   // set current thread
   if(mThread == NULL)
   {
      mThread = Thread::currentThread();
   }
   
   // set the data offset
   int offset = 0;
   int numBytes = length;
   while(rval && length > 0)
   {
      // throttle the write as appropriate
      BandwidthThrottler* bt = mConnection->getBandwidthThrottler(false);
      if(bt != NULL)
      {
         bt->requestBytes(length, numBytes);
      }
      
      // ensure thread not interrupted
      if(rval = !mThread->isInterrupted())
      {
         // send data through the socket output stream
         if(rval = mConnection->getSocket()->getOutputStream()->write(
            b + offset, numBytes))
         {
            // increment offset and decrement length
            offset += numBytes;
            length -= numBytes;
            
            // update bytes written (reset as necessary)
            if(mBytesWritten > Math::HALF_MAX_LONG_VALUE)
            {
               mBytesWritten = 0;
            }
            
            mBytesWritten += numBytes;
         }
      }
   }
   
   return rval;
}

void ConnectionOutputStream::close()
{
   // close socket output stream
   mConnection->getSocket()->getOutputStream()->close();
}

unsigned long long ConnectionOutputStream::getBytesWritten()
{
   return mBytesWritten;
}
