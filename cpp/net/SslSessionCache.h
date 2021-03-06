/*
 * Copyright (c) 2008-2011 Digital Bazaar, Inc. All rights reserved.
 */
#ifndef monarch_net_SslSessionCache_H
#define monarch_net_SslSessionCache_H

#include "monarch/net/SslSession.h"
#include "monarch/rt/SharedLock.h"
#include "monarch/util/StringTools.h"
#include "monarch/util/Url.h"

#include <map>

namespace monarch
{
namespace net
{

/**
 * An SslSessionCache is a thread-safe cache for SslSessions.
 *
 * @author Dave Longley
 */
class SslSessionCache
{
protected:
   /**
    * A mapping of session keys to re-usable SSL sessions.
    */
   typedef std::map<
      const char*, monarch::net::SslSession, monarch::util::StringComparator>
      SessionMap;
   SessionMap mSessions;

   /**
    * Stores the capacity of this cache.
    */
   unsigned int mCapacity;

   /**
    * A lock for modifying the session map.
    */
   monarch::rt::SharedLock mLock;

public:
   /**
    * Creates a new SslSessionCache with the specified capacity.
    *
    * @param capacity the maximum number of sessions to cache.
    */
   SslSessionCache(unsigned int capacity = 50);

   /**
    * Destructs this SslSessionCache.
    */
   virtual ~SslSessionCache();

   /**
    * Stores an SSL session in this cache.
    *
    * @param host the host (including port) for the session.
    * @param session the session to store.
    * @param vHost an associated virtual host, if applicable.
    */
   virtual void storeSession(
      const char* host, SslSession& session, const char* vHost = NULL);

   /**
    * Stores an SSL session in this cache.
    *
    * @param url the url for the session.
    * @param session the session to store.
    * @param vHost an associated virtual host, if applicable.
    */
   virtual void storeSession(
      monarch::util::Url* url, SslSession& session, const char* vHost = NULL);

   /**
    * Gets a stored SSL session from the cache, if one exists.
    *
    * @param host the host for the session.
    * @param vHost an associated virtual host, if applicable.
    *
    * @return the SslSession (set to NULL if none exists).
    */
   virtual SslSession getSession(const char* host, const char* vHost = NULL);

   /**
    * Gets a stored SSL session from the cache, if one exists.
    *
    * @param url the url for the session.
    * @param vHost an associated virtual host, if applicable.
    *
    * @return the SslSession (set to NULL if none exists).
    */
   virtual SslSession getSession(
      monarch::util::Url* url, const char* vHost = NULL);
};

// type definition for a reference counted SslSessionCache
typedef monarch::rt::Collectable<SslSessionCache> SslSessionCacheRef;

} // end namespace net
} // end namespace monarch
#endif
