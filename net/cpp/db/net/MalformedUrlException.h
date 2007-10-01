/*
 * Copyright (c) 2007 Digital Bazaar, Inc.  All rights reserved.
 */
#ifndef db_net_MalformedUrlException_H
#define db_net_MalformedUrlException_H

#include "db/rt/Exception.h"

namespace db
{
namespace net
{

/**
 * A MalformedUrlException is raised when a URL is malformed.
 *
 * @author Dave Longley
 */
class MalformedUrlException : public db::rt::Exception
{
public:
   /**
    * Creates a new MalformedUrlException.
    *
    * A message, type, and code may be optionally specified.
    *
    * @param message the message for this Exception.
    * @param type the type for this Exception.
    * @param code the code for this Exception.
    */
   MalformedUrlException(
      const char* message = "",
      const char* type = "db.net.MalformedUrl", int code = 0);
   
   /**
    * Destructs this MalformedUrlException.
    */
   virtual ~MalformedUrlException();
};

} // end namespace net
} // end namespace db
#endif
