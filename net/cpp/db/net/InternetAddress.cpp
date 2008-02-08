/*
 * Copyright (c) 2007-2008 Digital Bazaar, Inc.  All rights reserved.
 */
#include "db/net/InternetAddress.h"
#include "db/net/SocketDefinitions.h"
#include "db/rt/Exception.h"

using namespace std;
using namespace db::net;
using namespace db::rt;

InternetAddress::InternetAddress(const char* host, unsigned short port) :
   SocketAddress("IPv4", "0.0.0.0", port)
{
   mHost = strdup("");
   
   if(strcmp(host, "") != 0)
   {
      // resolve host
      InternetAddress::setHost(host);
   }
}

InternetAddress::~InternetAddress()
{
   free(mHost);
}

bool InternetAddress::toSockAddr(sockaddr* addr, unsigned int& size)
{
   bool rval = false;
   
   // use sockaddr_in (IPv4)
   if(size >= sizeof(sockaddr_in))
   {
      struct sockaddr_in* sa = (sockaddr_in*)addr;
      size = sizeof(sockaddr_in);
      memset(sa, '\0', size);
      
      // the address family is internet (AF_INET = address family internet)
      sa->sin_family = AF_INET;
      
      // htons = "Host To Network Short" which means order the short in
      // network byte order (big-endian)
      sa->sin_port = htons(getPort());
      
      // converts an address to network byte order
      rval = (inet_pton(AF_INET, getAddress(), &sa->sin_addr) == 1);
   }
   
   return rval;
}

bool InternetAddress::fromSockAddr(const sockaddr* addr, unsigned int size)
{
   bool rval = false;
   
   // use sockaddr_in (IPv4)
   if(size >= sizeof(sockaddr_in))
   {
      struct sockaddr_in* sa = (sockaddr_in*)addr;
      
      // get address
      char dst[INET_ADDRSTRLEN];
      memset(&dst, '\0', size);
      if(inet_ntop(AF_INET, &sa->sin_addr, dst, INET_ADDRSTRLEN) != NULL)
      {
         // set address
         setAddress(dst);
         
         // converting from network byte order to little-endian
         setPort(ntohs(sa->sin_port));
         
         // conversion successful
         rval = true;
      }
   }
   
   return rval;
}

void InternetAddress::setAddress(const char* address)
{
   // set the address
   SocketAddress::setAddress(address);
   
   // clear the host
   free(mHost);
   mHost = strdup("");
}

bool InternetAddress::setHost(const char* host)
{
   bool rval = false;
   
   // create hints address structure
   struct addrinfo hints;
   memset(&hints, '\0', sizeof(hints));
   hints.ai_family = AF_INET;
   
   // create pointer for storing allocated resolved address
   struct addrinfo* res = NULL;
   
   // get address information
   if(getaddrinfo(host, NULL, &hints, &res) != 0)
   {
      int length = 17 + strlen(host);
      char msg[length];
      snprintf(msg, length, "Unknown host '%s'!", host);
      ExceptionRef e = new Exception(msg, "db.net.UnknownHost");
      Exception::setLast(e);
   }
   else
   {
      // copy the first result
      struct sockaddr_in addr;
      memcpy(&addr, res->ai_addr, res->ai_addrlen);
      
      // get the address
      char dst[INET_ADDRSTRLEN];
      memset(&dst, '\0', INET_ADDRSTRLEN);
      inet_ntop(AF_INET, &addr.sin_addr, dst, INET_ADDRSTRLEN);
      free(mAddress);
      mAddress = strdup(dst);
      rval = true;
   }
   
   if(res != NULL)
   {
      // free res if it got allocated
      freeaddrinfo(res);
   }
   
   return rval;
}

const char* InternetAddress::getHost()
{
   if(strcmp(mHost, "") == 0 && strcmp(getAddress(), "") != 0)
   {
      // get a IPv4 address structure
      struct sockaddr_in sa;
      unsigned int size = sizeof(sockaddr_in);
      toSockAddr((sockaddr*)&sa, size);
      
      // NULL specifies that we don't care about getting a "service" name
      // given in sockaddr_in will be returned
      char dst[100];
      memset(&dst, '\0', 100);
      if(getnameinfo((sockaddr*)&sa, size, dst, 100, NULL, 0, 0) == 0)
      {
         // set host name
         free(mHost);
         mHost = strdup(dst);
      }
      else
      {
         // use address
         free(mHost);
         mHost = strdup(getAddress());
      }
   }
   
   // return host
   return mHost;
}

bool InternetAddress::isMulticast()
{
   bool rval = false;
   
   // get a IPv4 address structure
   struct sockaddr_in sa;
   unsigned int size = sizeof(sockaddr_in);
   toSockAddr((sockaddr*)&sa, size);
   
   if(IN_MULTICAST(ntohl(sa.sin_addr.s_addr)) != 0)
   {
      rval = true;
   }
   
   return rval;
}

string& InternetAddress::toString(string& str)
{
   int length = 100 + strlen(getHost()) + strlen(getAddress());
   char temp[length];
   snprintf(temp, length, "InternetAddress [%s:%u,%s:%u]",
      getHost(), getPort(), getAddress(), getPort());
   str.assign(temp);
   return str;
}
