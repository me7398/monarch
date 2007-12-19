/*
 * Copyright (c) 2007 Digital Bazaar, Inc.  All rights reserved.
 */
#include "db/sql/sqlite3/Sqlite3Connection.h"
#include "db/sql/sqlite3/Sqlite3Statement.h"

#include <iostream>

using namespace std;
using namespace db::sql;
using namespace db::sql::sqlite3;
using namespace db::net;
using namespace db::rt;

Sqlite3Connection::Sqlite3Connection()
{
   // initialize handle to NULL
   mHandle = NULL;
}

Sqlite3Connection::~Sqlite3Connection()
{
   // ensure connection is closed
   Sqlite3Connection::close();
}

SqlException* Sqlite3Connection::connect(const char* url)
{
   return Connection::connect(url);
}

SqlException* Sqlite3Connection::connect(Url* url)
{
   SqlException* rval = NULL;
   
   if(strncmp(url->getScheme().c_str(), "sqlite3", 7) != 0)
   {
      string msg;
      string urlStr;
      msg.append(
         "Could not connect to sqlite3 database, "
         "url scheme doesn't start with 'sqlite3', url='");
      msg.append(url->toString(urlStr));
      msg.append(1, '\'');
      
      Exception::setLast(new SqlException(msg.c_str()));
   }
   else
   {
      // get database name
      const char* db;
      if(strcmp(url->getScheme().c_str(), "sqlite3::memory:") == 0)
      {
         db = url->getScheme().c_str();
      }
      else
      {
         db = url->getPath().c_str();
      }
      
      // FIXME: we want to add read/write/create params to the URL
      // so connections can be read-only/write/etc (use query in URL)
      // handle username/password
      int ec = sqlite3_open(db, &mHandle);
      if(ec != SQLITE_OK)
      {
         // create exception, close connection
         rval = new Sqlite3Exception(this);
         Exception::setLast(rval);
         Sqlite3Connection::close();
      }
   }
   
   return rval;
}

Statement* Sqlite3Connection::prepare(const char* sql)
{
   Exception* e = Exception::getLast();
   
   // create statement
   Statement* rval = new Sqlite3Statement(this, sql);
   if(Exception::getLast() != e)
   {
      // delete statement if exception was thrown while creating statement
      delete rval;
      rval = NULL;
   }
   
   return rval;
}

void Sqlite3Connection::close()
{
   if(mHandle != NULL)
   {
      sqlite3_close(mHandle);
      mHandle = NULL;
   }
}

SqlException* Sqlite3Connection::commit()
{
   // FIXME:
   cout << "FIXME: commit" << endl;
   return NULL;
}

SqlException* Sqlite3Connection::rollback()
{
   // FIXME:
   cout << "FIXME: rollback" << endl;
   return NULL;
}
