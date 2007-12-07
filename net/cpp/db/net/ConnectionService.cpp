/*
 * Copyright (c) 2007 Digital Bazaar, Inc.  All rights reserved.
 */
#include "db/net/ConnectionService.h"
#include "db/net/Server.h"
#include "db/net/TcpSocket.h"

using namespace std;
using namespace db::modest;
using namespace db::net;
using namespace db::rt;

ConnectionService::ConnectionService(
   Server* server,
   InternetAddress* address,
   ConnectionServicer* servicer,
   SocketDataPresenter* presenter) :
   PortService(server, address),
   mConnectionSemaphore(10000, true)
{
   mServicer = servicer;
   mDataPresenter = presenter;
   mSocket = NULL;
   mConnectionCount = 0;
}

ConnectionService::~ConnectionService()
{
   // ensure service is stopped
   ConnectionService::stop();
}

Operation ConnectionService::initialize()
{
   Operation rval(NULL);
   
   // no connections yet
   mConnectionCount = 0;
   
   // create tcp socket
   mSocket = new TcpSocket();
   
   // bind socket to the address and start listening
   if(mSocket->bind(getAddress()) && mSocket->listen())
   {
      // create Operation for running service
      rval = *this;
      rval->addGuard(this);
   }
   
   return rval;
}

void ConnectionService::cleanup()
{
   if(mSocket != NULL)
   {
      // clean up socket
      delete mSocket;
      mSocket = NULL;
   }
}

bool ConnectionService::canExecuteOperation(ImmutableState* s, Operation& op)
{
   // can execute if server is running
   return mServer->isRunning();
}

bool ConnectionService::mustCancelOperation(ImmutableState* s, Operation& op)
{
   // must cancel if server is no longer running
   return !mServer->isRunning();
}

void ConnectionService::run()
{
   while(!mOperation->isInterrupted())
   {
      // prune running servicers
      mRunningServicers.prune();
      
      // acquire service connection permit
      if(mConnectionSemaphore.acquire() == NULL)
      {
         // acquire server connection permit
         if(mServer->mConnectionSemaphore.acquire() == NULL)
         {
            // wait for 5 seconds for a connection
            Socket* s = mSocket->accept(5);
            if(s != NULL)
            {
               // create Connection from the connected Socket
               createConnection(s);
            }
            else
            {
               // release connection permits
               mServer->mConnectionSemaphore.release();
               mConnectionSemaphore.release();
            }
         }
         else
         {
            // release service connection permit
            mConnectionSemaphore.release();
         }
      }
   }
   
   // close socket
   mSocket->close();
   
   // terminate running servicers
   mRunningServicers.terminate();
}

void ConnectionService::createConnection(Socket* s)
{
   // try to wrap Socket for standard data presentation
   bool secure = false;
   Socket* wrapper = s;
   if(mDataPresenter != NULL)
   {
      wrapper = mDataPresenter->createPresentationWrapper(s, secure);
   }
   
   if(wrapper != NULL)
   {
      // create connection
      Connection* c = new Connection(wrapper, true);
      c->setSecure(secure);
      
      // increase connection count
      mServer->mConnectionCount++;
      mConnectionCount++;
      
      // create ConnectionWorker and Operation to run it
      ConnectionWorker* worker = new ConnectionWorker(this, c);
      CollectableRunnable cr = worker;
      Operation op(cr);
      mRunningServicers.add(op);
      
      // run operation
      mServer->getOperationRunner()->runOperation(op);
   }
   else
   {
      // close socket, data cannot be presented in standard format
      s->close();
      delete s;
      
      // release connection permits
      mServer->mConnectionSemaphore.release();
      mConnectionSemaphore.release();
   }
}

void ConnectionService::serviceConnection(Connection* c)
{
   // service the connection
   mServicer->serviceConnection(c);
   
   // ensure connection is closed
   c->close();
   
   // decrease connection count
   mServer->mConnectionCount--;
   mConnectionCount--;
   
   // release connection permits
   mServer->mConnectionSemaphore.release();
   mConnectionSemaphore.release();
}

void ConnectionService::setMaxConnectionCount(unsigned int count)
{
   mConnectionSemaphore.setMaxPermitCount(count);
}

unsigned int ConnectionService::getMaxConnectionCount()
{
   return mConnectionSemaphore.getMaxPermitCount();
}

unsigned int ConnectionService::getConnectionCount()
{
   return mConnectionCount;
}
