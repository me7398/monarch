/*
 * Copyright (c) 2007-2009 Digital Bazaar, Inc. All rights reserved.
 */
#include "db/rt/Exception.h"

#include "db/rt/Thread.h"
#include "db/rt/DynamicObject.h"

#include <cstdlib>

using namespace db::rt;

Exception::Exception(const char* message, const char* type, int code)
{
   mMessage = strdup((message == NULL) ? "" : message);
   mType = strdup((type == NULL) ? "" : type);
   mCode = code;
   mCause = new ExceptionRef(NULL);
   mDetails = new DynamicObject(NULL);
}

Exception::~Exception()
{
   free(mMessage);
   free(mType);
   delete mCause;
   delete mDetails;
}

void Exception::setMessage(const char* message)
{
   free(mMessage);
   mMessage = strdup((message == NULL) ? "" : message);
}

const char* Exception::getMessage()
{
   return mMessage;
}

void Exception::setType(const char* type)
{
   free(mType);
   mType = strdup((type == NULL) ? "" : type);
}

const char* Exception::getType()
{
   return mType;
}

bool Exception::isType(const char* type, bool startsWith)
{
   bool rval = false;
   
   if(startsWith)
   {
      rval = (strncmp(getType(), type, strlen(type)) == 0);
   }
   else
   {
      rval = (strcmp(getType(), type) == 0);
   }
   
   return rval;
}

void Exception::setCode(int code)
{
   mCode = code;
}
int Exception::getCode()
{
   return mCode;
}

void Exception::setCause(ExceptionRef& cause)
{
   *mCause = cause;
}

ExceptionRef& Exception::getCause()
{
   return *mCause;
}

// method to optimize deep searches for exception type base names
static ExceptionRef _getCauseOfType(ExceptionRef& e, const char* type, int n)
{
   ExceptionRef rval(NULL);
   
   // check this exception's type
   if(strncmp(e->getType(), type, n) == 0)
   {
      rval = e;
   }
   // check this exception's cause
   else if(!e->getCause().isNull())
   {
      rval = _getCauseOfType(e->getCause(), type, n);
   }
   
   return rval;
}

ExceptionRef Exception::getCauseOfType(const char* type, bool startsWith)
{
   ExceptionRef rval(NULL);
   
   if(!mCause->isNull())
   {
      if(startsWith)
      {
         // use optimized recursive method that only counts "type" length once
         _getCauseOfType(*mCause, type, strlen(type));
      }
      else
      {
         // check this exception's cause
         if((*mCause)->isType(type, false))
         {
            rval = *mCause;
         }
         // check deeper in the stack/chain
         else
         {
            rval = (*mCause)->getCauseOfType(type, false);
         }
      }
   }
   
   return rval;
}

DynamicObject& Exception::getDetails()
{
   if(mDetails->isNull())
   {
      DynamicObject details;
      details->setType(Map);
      *mDetails = details;
   }
   
   return *mDetails;
}

ExceptionRef& Exception::set(ExceptionRef& e)
{
   // false = do not use previous exception as cause, instead, clear it 
   Thread::setException(e, false);
   return e;
}

ExceptionRef& Exception::push(ExceptionRef& e)
{
   // true = use previous exception as cause, do NOT clear it 
   Thread::setException(e, true);
   return e;
}

ExceptionRef Exception::get()
{
   return Thread::getException();
}

bool Exception::isSet()
{
   return Thread::hasException();
}

void Exception::clear()
{
   Thread::clearException();
}

DynamicObject Exception::getAsDynamicObject()
{
   ExceptionRef e = Exception::get();
   return Exception::convertToDynamicObject(e);
}

DynamicObject Exception::convertToDynamicObject(ExceptionRef& e)
{
   DynamicObject dyno;
   
   dyno["message"] = e->getMessage();
   dyno["type"] = e->getType();
   dyno["code"] = e->getCode();
   
   if(!e->getCause().isNull())
   {
      dyno["cause"] = convertToDynamicObject(e->getCause());
   }
   
   if(!(*e).mDetails->isNull())
   {
      dyno["details"] = e->getDetails();
   }
   
   return dyno;
}

ExceptionRef Exception::convertToException(DynamicObject& dyno)
{
   ExceptionRef e = new Exception(
      dyno["message"]->getString(),
      dyno["type"]->getString(),
      dyno["code"]->getInt32());
   
   if(dyno->hasMember("cause"))
   {
      ExceptionRef cause = convertToException(dyno["cause"]);
      e->setCause(cause);
   }
   
   if(dyno->hasMember("details"))
   {
      *(*e).mDetails = dyno["details"].clone();
   }
   
   return e;
}
