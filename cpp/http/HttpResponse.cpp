/*
 * Copyright (c) 2007-2009 Digital Bazaar, Inc. All rights reserved.
 */
#include "monarch/http/HttpResponse.h"

#include "monarch/http/HttpRequest.h"

using namespace monarch::io;
using namespace monarch::http;

HttpResponse::HttpResponse(HttpRequest* request)
{
   mRequest = request;
}

HttpResponse::~HttpResponse()
{
}

inline bool HttpResponse::sendHeader()
{
   return getConnection()->sendHeader(getHeader());
}

inline bool HttpResponse::receiveHeader()
{
   return getConnection()->receiveHeader(getHeader());
}

inline bool HttpResponse::sendBody(InputStream* is, HttpTrailer* trailer)
{
   return getConnection()->sendBody(getHeader(), is, trailer);
}

inline OutputStream* HttpResponse::getBodyOutputStream(HttpTrailer* trailer)
{
   return getConnection()->getBodyOutputStream(getHeader(), trailer);
}

inline bool HttpResponse::receiveBody(OutputStream* os, HttpTrailer* trailer)
{
   return getConnection()->receiveBody(getHeader(), os, trailer);
}

inline InputStream* HttpResponse::getBodyInputStream(HttpTrailer* trailer)
{
   return getConnection()->getBodyInputStream(getHeader(), trailer);
}

inline HttpResponseHeader* HttpResponse::getHeader()
{
   return &mHeader;
}

inline HttpConnection* HttpResponse::getConnection()
{
   return mRequest->getConnection();
}

inline HttpRequest* HttpResponse::getRequest()
{
   return mRequest;
}
