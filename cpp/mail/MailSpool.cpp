/*
 * Copyright (c) 2008-2009 Digital Bazaar, Inc. All rights reserved.
 */
#include "monarch/mail/MailSpool.h"

#include "monarch/data/Data.h"
#include "monarch/data/json/JsonReader.h"
#include "monarch/data/json/JsonWriter.h"
#include "monarch/mail/MailTemplateParser.h"
#include "monarch/io/ByteArrayInputStream.h"
#include "monarch/io/FileInputStream.h"
#include "monarch/io/FileOutputStream.h"
#include "monarch/sql/Row.h"
#include "monarch/sql/Statement.h"
#include "monarch/sql/sqlite3/Sqlite3ConnectionPool.h"
#include "monarch/sql/sqlite3/Sqlite3DatabaseClient.h"
#include "monarch/util/Data.h"
#include "monarch/util/Date.h"

using namespace monarch::data::json;
using namespace monarch::io;
using namespace monarch::mail;
using namespace monarch::rt;
using namespace monarch::sql;
using namespace monarch::sql::sqlite3;
using namespace monarch::util;

#define SPOOL_TABLE_SPOOL   "spool"

MailSpool::MailSpool() :
   mDbClient(NULL)
{
}

MailSpool::~MailSpool()
{
}

bool MailSpool::initialize(const char* url)
{
   bool rval = false;

   // create sqlite3 connection pool
   ConnectionPoolRef pool(new Sqlite3ConnectionPool(url, 1));

   // create database client
   mDbClient = new Sqlite3DatabaseClient();
   mDbClient->setDebugLogging(false);
   mDbClient->setReadConnectionPool(pool);
   mDbClient->setWriteConnectionPool(pool);
   rval = mDbClient->initialize();

   // define schemas:

   // spool table
   if(rval)
   {
      SchemaObject schema;
      schema["table"] = SPOOL_TABLE_SPOOL;

      DatabaseClient::addSchemaColumn(schema,
         "id", "INTEGER PRIMARY KEY", "id", UInt64);
      DatabaseClient::addSchemaColumn(schema,
         "date", "TEXT", "date", String);
      DatabaseClient::addSchemaColumn(schema,
         "mail", "TEXT", "mail", String);
      DatabaseClient::addSchemaColumn(schema,
         "reason", "TEXT", "reason", String);

      rval = mDbClient->define(schema);
   }

   // create tables
   if(rval)
   {
      rval = mDbClient->create(SPOOL_TABLE_SPOOL, true);
   }

   if(!rval)
   {
      mDbClient.setNull();
      ExceptionRef e = new Exception(
         "Could not initialize mail spool.",
         "monarch.mail.MailSpool.InitializeError");
      e->getDetails()["url"] = url;
      Exception::push(e);
   }

   return rval;
}

void MailSpool::setDebugLogging(bool on)
{
   mDbClient->setDebugLogging(on);
}

bool MailSpool::spool(Mail* mail, DynamicObject* reason)
{
   bool rval = false;

   // build spool record
   DynamicObject record;
   record["date"] = Date().getUtcDateTime().c_str();
   record["mail"] = mail->toTemplate().c_str();
   if(reason != NULL)
   {
      record["reason"] =
         JsonWriter::writeToString(*reason, true, false).c_str();
   }

   // insert spool record
   SqlExecutableRef se = mDbClient->insert(SPOOL_TABLE_SPOOL, record);
   rval = !se.isNull() && mDbClient->execute(se);
   if(!rval)
   {
      ExceptionRef e = new Exception(
         "Could not spool mail.",
         "monarch.mail.MailSpool.SpoolError");
      Exception::push(e);
   }

   return rval;
}

bool MailSpool::getFirst(Mail* mail)
{
   bool rval = false;

   // get the first mail in the spool
   SqlExecutableRef se = mDbClient->selectOne(SPOOL_TABLE_SPOOL);
   rval = !se.isNull() && mDbClient->execute(se);
   if(rval && se->rowsRetrieved == 0)
   {
      ExceptionRef e = new Exception(
         "Spool is empty.",
         "monarch.mail.MailSpool.Empty");
      Exception::set(e);
   }
   else
   {
      // parse mail data
      char* str = const_cast<char*>(se->result["mail"]->getString());
      int length = se->result["mail"]->length();
      ByteBuffer bb(str, 0, length, length, false);
      ByteArrayInputStream bais(&bb, false);
      MailTemplateParser parser;
      DynamicObject vars;
      rval = parser.parse(mail, vars, false, &bais);
   }

   if(!rval)
   {
      ExceptionRef e = new Exception(
         "Could not get mail from spool.",
         "monarch.mail.MailSpool.GetMailError");
      Exception::push(e);
   }

   return rval;
}

bool MailSpool::unwind(Mail* mail, bool* unwound)
{
   bool rval = false;

   if(unwound != NULL)
   {
      // no mail unwound yet
      *unwound = false;
   }

   Connection* c = mDbClient->getWriteConnection();
   if(c != NULL)
   {
      // begin a transaction
      if(mDbClient->begin(c))
      {
         DynamicObject members(NULL);
         if(mail == NULL)
         {
            // since we are not populating a mail object, only get the mail ID
            members = DynamicObject();
            members["id"];
         }
         SqlExecutableRef se = mDbClient->selectOne(
            SPOOL_TABLE_SPOOL, NULL, members.isNull() ? NULL : &members);
         rval = !se.isNull() && mDbClient->execute(se, c);
         if(rval && se->rowsRetrieved == 1)
         {
            if(mail != NULL)
            {
               // populate the mail
               char* str = const_cast<char*>(se->result["mail"]->getString());
               int length = se->result["mail"]->length();
               ByteBuffer bb(str, 0, length, length, false);
               ByteArrayInputStream bais(&bb, false);
               MailTemplateParser parser;
               DynamicObject vars;
               rval = parser.parse(mail, vars, false, &bais);
            }

            if(rval)
            {
               // delete mail from spool
               DynamicObject where;
               where["id"] = se->result["id"];
               se = mDbClient->remove(SPOOL_TABLE_SPOOL, &where);
               rval = !se.isNull() && mDbClient->execute(se, c);
               if(rval && unwound != NULL)
               {
                  // a mail was unwound from the spool
                  *unwound = true;
               }
            }
         }

         // end transaction
         rval = mDbClient->end(c, rval) && rval;
      }

      c->close();
   }

   if(!rval)
   {
      ExceptionRef e = new Exception(
         "Could not unwind mail spool.",
         "monarch.mail.MailSpool.UnwindError");
      Exception::push(e);
   }

   return rval;
}

uint32_t MailSpool::getCount()
{
   uint32_t rval = 0;

   Connection* c = mDbClient->getReadConnection();
   if(c != NULL)
   {
      Statement* s = c->prepare("SELECT COUNT(*) FROM " SPOOL_TABLE_SPOOL);
      if((s != NULL) && s->execute())
      {
         Row* row = s->fetch();
         if(row != NULL)
         {
            uint64_t count;
            if(row->getUInt64((unsigned int)0, count))
            {
               rval = (uint32_t)count;
            }
            s->fetch();
         }
      }

      c->close();
   }

   return rval;
}

bool MailSpool::clear()
{
   bool rval = false;

   Connection* c = mDbClient->getReadConnection();
   if(c != NULL)
   {
      // drop tables and recreate them in a transaction
      if(mDbClient->begin(c))
      {
         rval =
            mDbClient->drop(SPOOL_TABLE_SPOOL, true, c) &&
            mDbClient->create(SPOOL_TABLE_SPOOL, true, c);
         rval = mDbClient->end(c, rval) && rval;
      }

      c->close();
   }

   return rval;
}
