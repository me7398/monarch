/*
 * Copyright (c) 2007-2010 Digital Bazaar, Inc. All rights reserved.
 */
#include "monarch/sql/mysql/MySqlRow.h"

#include "monarch/sql/mysql/MySqlStatement.h"
#include "monarch/sql/mysql/MySqlConnection.h"

#include <cstdio>

using namespace std;
using namespace monarch::rt;
using namespace monarch::sql;
using namespace monarch::sql::mysql;

MySqlRow::MySqlRow(MySqlStatement* s) :
   Row(s),
   mFields(NULL),
   mFieldCount(0),
   mBindings(NULL)
{
}

MySqlRow::~MySqlRow()
{
}

static inline MYSQL_STMT* getStatementHandle(Statement* s)
{
   return static_cast<MySqlStatement*>(s)->getHandle();
}

void MySqlRow::setFields(
   MYSQL_FIELD* fields, unsigned int count, MYSQL_BIND* bindings)
{
   mFields = fields;
   mFieldCount = count;
   mBindings = bindings;
}

bool MySqlRow::getType(unsigned int column, int& type)
{
   bool rval = false;

   if(column >= mFieldCount)
   {
      ExceptionRef e = new Exception(
         "Could not get column type. Invalid column index.",
         "monarch.sql.mysql.MySql");
      e->getDetails()["column"] = column;
      Exception::set(e);
   }
   else
   {
      type = mFields[column].type;
      rval = true;
   }

   return rval;
}

bool MySqlRow::getInt32(unsigned int column, int32_t& i)
{
   mBindings[column].buffer_type = MYSQL_TYPE_LONG;
   mBindings[column].buffer = (char*)&i;
   mBindings[column].buffer_length = 4;
   mBindings[column].length = &mBindings[column].buffer_length;
   mysql_stmt_fetch_column(
      getStatementHandle(mStatement), &mBindings[column], column, 0);

   // FIXME: check exceptions, etc
   return true;
}

bool MySqlRow::getUInt32(unsigned int column, uint32_t& i)
{
   mBindings[column].buffer_type = MYSQL_TYPE_LONG;
   mBindings[column].buffer = (char*)&i;
   mBindings[column].buffer_length = 4;
   mBindings[column].length = &mBindings[column].buffer_length;
   mysql_stmt_fetch_column(
      getStatementHandle(mStatement), &mBindings[column], column, 0);

   // FIXME: check exceptions, etc
   return true;
}

bool MySqlRow::getInt64(unsigned int column, int64_t& i)
{
   mBindings[column].buffer_type = MYSQL_TYPE_LONGLONG;
   mBindings[column].buffer = (char*)&i;
   mBindings[column].buffer_length = 8;
   mBindings[column].length = &mBindings[column].buffer_length;
   mysql_stmt_fetch_column(
      getStatementHandle(mStatement), &mBindings[column], column, 0);

   // FIXME: check exceptions, etc
   return true;
}

bool MySqlRow::getUInt64(unsigned int column, uint64_t& i)
{
   mBindings[column].buffer_type = MYSQL_TYPE_LONGLONG;
   mBindings[column].buffer = (char*)&i;
   mBindings[column].buffer_length = 8;
   mBindings[column].length = &mBindings[column].buffer_length;
   mysql_stmt_fetch_column(
      getStatementHandle(mStatement), &mBindings[column], column, 0);

   // FIXME: check exceptions, etc
   return true;
}

bool MySqlRow::getText(unsigned int column, string& str)
{
   int length = mBindings[column].buffer_length;
   char tmp[length];
   bool rval = getBlob(column, tmp, &length);
   if(rval)
   {
      str.assign(tmp, length);
   }
   return rval;
}

bool MySqlRow::getBlob(unsigned int column, char* buffer, int* length)
{
   bool rval = false;

   // always update length, but only return blob if it is large enough to
   // hold all of the data
   int max = *length;
   *length = mBindings[column].buffer_length;
   if(*length > max)
   {
      ExceptionRef e = new Exception(
         "Blob too large to fit into buffer.",
         "monarch.sql.BufferOverflow");
      Exception::set(e);
   }
   else
   {
      // FIXME: this code (particularly setting the binding column length
      // appears questionable
      my_bool isNull;
      mBindings[column].buffer_type = MYSQL_TYPE_BLOB;
      mBindings[column].buffer = buffer;
      mBindings[column].length = &mBindings[column].buffer_length;
      mBindings[column].is_null = &isNull;
      mysql_stmt_fetch_column(
         getStatementHandle(mStatement), &mBindings[column], column, 0);
      *length = isNull ? 0 : mBindings[column].buffer_length;
      // FIXME: check exceptions, etc
      rval = true;
   }

   return rval;
}

bool MySqlRow::getType(const char* column, int& type)
{
   bool rval = false;

   // get column index for name
   int64_t index = getColumnIndex(column);
   if(index != -1)
   {
      rval = getType(index, type);
   }

   return rval;
}

bool MySqlRow::getInt32(const char* column, int32_t& i)
{
   bool rval = false;

   // get column index for name
   int64_t index = getColumnIndex(column);
   if(index != -1)
   {
      rval = getInt32(index, i);
   }

   return rval;
}

bool MySqlRow::getUInt32(const char* column, uint32_t& i)
{
   bool rval = false;

   // get column index for name
   int64_t index = getColumnIndex(column);
   if(index != -1)
   {
      rval = getUInt32(index, i);
   }

   return rval;
}

bool MySqlRow::getInt64(const char* column, int64_t& i)
{
   bool rval = false;

   // get column index for name
   int64_t index = getColumnIndex(column);
   if(index != -1)
   {
      rval = getInt64(index, i);
   }

   return rval;
}

bool MySqlRow::getUInt64(const char* column, uint64_t& i)
{
   bool rval = false;

   // get column index for name
   int64_t index = getColumnIndex(column);
   if(index != -1)
   {
      rval = getUInt64(index, i);
   }

   return rval;
}

bool MySqlRow::getText(const char* column, std::string& str)
{
   bool rval = false;

   // get column index for name
   int64_t index = getColumnIndex(column);
   if(index != -1)
   {
      rval = getText(index, str);
   }

   return rval;
}

bool MySqlRow::getBlob(const char* column, char* buffer, int* length)
{
   bool rval = false;

   // get column index for name
   int64_t index = getColumnIndex(column);
   if(index != -1)
   {
      rval = getBlob(index, buffer, length);
   }

   return rval;
}

int64_t MySqlRow::getColumnIndex(const char* name)
{
   // use 64-bit signed int to cover all values + error (negative 1)
   int64_t rval = -1;

   for(unsigned int i = 0; i < mFieldCount; ++i)
   {
      if(strcmp(name, mFields[i].name) == 0)
      {
         rval = i;
         break;
      }
   }

   if(rval == -1)
   {
      // set exception
      ExceptionRef e = new Exception(
         "Could not get column value. Invalid column name.",
         "monarch.sql.mysql.MySql");
      e->getDetails()["name"] = name;
      Exception::set(e);
   }

   return rval;
}
