/*
 * Copyright (c) 2007-2011 Digital Bazaar, Inc. All rights reserved.
 */
#define __STDC_FORMAT_MACROS

#include "monarch/net/ConnectionService.h"

#include "monarch/logging/Logging.h"
#include "monarch/net/Server.h"
#include "monarch/net/TcpSocket.h"
#include "monarch/net/Internet6Address.h"
#include "monarch/rt/RunnableDelegate.h"
#include "monarch/util/Timer.h"

using namespace std;
using namespace monarch::modest;
using namespace monarch::net;
using namespace monarch::rt;
using namespace monarch::util;

ConnectionService::ConnectionService(
   Server* server,
   InternetAddress* address,
   ConnectionServicer* servicer,
   SocketDataPresenter* presenter,
   const char* name) :
   PortService(server, address, name),
   mServicer(servicer),
   mDataPresenter(presenter),
   mSocket(NULL),
   mMaxConnections(100),
   mCurrentConnections(0),
   mBacklog(100)
{
}

ConnectionService::~ConnectionService()
{
   // ensure service is stopped
   ConnectionService::stop();
}

bool ConnectionService::canExecuteOperation(Operation& op)
{
   bool rval = false;

   if(op->getUserData() == mSocket)
   {
      // accept OP can execute if server is running
      rval = mServer->isRunning();
   }
   else if(mServer->isRunning())
   {
      // service OP can execute if the server and the connection service
      // have enough permits available
      int32_t permits =
         mServer->getMaxConnectionCount() - mServer->getConnectionCount();
      if(permits > 0)
      {
         permits = getMaxConnectionCount() - getConnectionCount();
         rval = (permits > 0);
      }
   }

   return rval;
}

bool ConnectionService::mustCancelOperation(Operation& op)
{
   bool rval = false;

   // must cancel any OP if the server is not running
   if(!mServer->isRunning())
   {
      rval = true;

      // operation's user data is the socket being serviced, either it is
      // the socket accepting connections or a socket that is servicing one
      Socket* socket = static_cast<Socket*>(op->getUserData());

      // if the socket isn't the accept socket, then it needs to be cleaned up
      if(socket != mSocket)
      {
         // close and clean up the operation's socket
         socket->close();
         delete socket;
         op->setUserData(NULL);
      }
   }

   return rval;
}

void ConnectionService::mutatePreExecutionState(Operation& op)
{
   // increase current connections
   ++mServer->mCurrentConnections;
   ++mCurrentConnections;
}

void ConnectionService::mutatePostExecutionState(Operation& op)
{
   // decrease current connections
   --mCurrentConnections;
   --mServer->mCurrentConnections;
}

void ConnectionService::run()
{
   Socket* s;
   while(!mOperation->isInterrupted())
   {
      // wait for 5 seconds for a connection
      if((s = mSocket->accept(5)) != NULL)
      {
         // create RunnableDelegate to service connection and run it
         // as an Operation
         Operation* op = new Operation(NULL);
         RunnableRef r =
            new RunnableDelegate<ConnectionService, Operation*>(
               this, &ConnectionService::serviceConnection, op);
         *op = Operation(r);
         (*op)->setUserData(s);
         (*op)->addGuard(this);
         (*op)->addStateMutator(this);
         mRunningServicers.add(*op);

         // run operation
         mServer->getOperationRunner()->runOperation(*op);
      }
   }

   // close socket
   mSocket->close();

   // terminate running servicers
   mRunningServicers.terminate();
}

void ConnectionService::serviceConnection(Operation* op)
{
   // start connection service time
   Timer t;
   t.start();

   // ensure the Socket can be wrapped with at least standard data presentation
   bool secure = false;
   Socket* socket = static_cast<Socket*>((*op)->getUserData());
   Socket* wrapper = socket;
   if(mDataPresenter != NULL)
   {
      // the secure flag will be set by the data presenter
      wrapper = mDataPresenter->createPresentationWrapper(socket, secure);
   }

   if(wrapper != NULL)
   {
      // create connection
      Connection* c = new Connection(wrapper, true);
      c->setSecure(secure);

      // get local/remote addresses
      SocketAddress* local = c->getLocalAddress();
      SocketAddress* remote = c->getRemoteAddress();

      // log connection
      MO_CAT_DEBUG(MO_NET_CAT, "%s:%i servicing %s connection from %s:%i",
         local->getAddress(),
         local->getPort(),
         secure ? "secure" : "non-secure",
         remote->getAddress(),
         remote->getPort());

      // service connection and get time
      mServicer->serviceConnection(c);
      uint64_t ms = t.getElapsedMilliseconds();

      // log connection
      MO_CAT_DEBUG(MO_NET_CAT,
         "%s:%i serviced %s connection from %s:%i in %" PRIu64 " ms",
         local->getAddress(),
         local->getPort(),
         secure ? "secure" : "non-secure",
         remote->getAddress(),
         remote->getPort(),
         ms);

      // close and clean up connection
      c->close();
      delete c;
   }
   else
   {
      // close socket, data cannot be presented in standard format
      socket->close();
      delete socket;
   }

   // remove op from running servicers and clean up
   mRunningServicers.remove(*op);
   delete op;
}

inline void ConnectionService::setMaxConnectionCount(int32_t count)
{
   mMaxConnections = count;
}

inline int32_t ConnectionService::getMaxConnectionCount()
{
   return mMaxConnections;
}

inline int32_t ConnectionService::getConnectionCount()
{
   return mCurrentConnections;
}

void ConnectionService::setBacklog(int backlog)
{
   mBacklog = backlog;
}

int ConnectionService::getBacklog()
{
   return mBacklog;
}

Operation ConnectionService::initialize()
{
   Operation rval(NULL);

   // no connections yet
   mCurrentConnections = 0;

   // create tcp socket
   mSocket = new TcpSocket();

   // bind socket to the address and start listening
   if(mSocket->bind(getAddress()) && mSocket->listen(getBacklog()))
   {
      // create Operation for running service
      rval = *this;
      rval->setUserData(&mSocket);
      rval->addGuard(this);
   }

   return rval;
}

void ConnectionService::cleanup()
{
   delete mSocket;
   mSocket = NULL;
}
