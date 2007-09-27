/*
 * Copyright (c) 2007 Digital Bazaar, Inc.  All rights reserved.
 */

#include "db/database/sqlite3/Sqlite3Row.h"
#include "db/database/sqlite3/Sqlite3Statement.h"
#include "db/database/sqlite3/Sqlite3Connection.h"

using namespace std;
using namespace db::database;
using namespace db::database::sqlite3;

Sqlite3Row::Sqlite3Row(Sqlite3Statement *s) :
   Row(s)
{
}

Sqlite3Row::~Sqlite3Row()
{
}

sqlite3_stmt* Sqlite3Row::getSqlite3Statement()
{
   return ((Sqlite3Statement*)mStatement)->mSqlite3Statement;
}

DatabaseException* Sqlite3Row::getType(int column, int& type)
{
   // FIXME: check exceptions, etc
   type = sqlite3_column_type(getSqlite3Statement(), column);
   return NULL;
}

DatabaseException* Sqlite3Row::getInteger(int column, int& i)
{
   // FIXME: check exceptions, etc
   i = sqlite3_column_int(getSqlite3Statement(), column);
   return NULL;
}

DatabaseException* Sqlite3Row::getText(int column, string& str)
{
   // FIXME: check exceptions, etc
   str.assign((const char*)sqlite3_column_text(getSqlite3Statement(), column));
   return NULL;
}
