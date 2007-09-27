/*
 * Copyright (c) 2007 Digital Bazaar, Inc.  All rights reserved.
 */
#ifndef db_database_sqlite3_Sqlite3Row_h
#define db_database_sqlite3_Sqlite3Row_h

#include <sqlite3.h>

#include "db/database/Row.h"
#include "db/database/sqlite3/Sqlite3Statement.h"

namespace db
{
namespace database
{
namespace sqlite3
{

// forward declarations
class Sqlite3Connection;
class Sqlite3Statement;

/**
 * An Sqlite3Row is a Statement result Row for an sqlite3 database.
 * 
 * @author David I. Lehn
 * @author Dave Longley
 */
class Sqlite3Row : public db::database::Row
{
protected:
   /**
    * Gets parent statements sqlite3_stmt.
    *
    * @return sqlite3_stmt for this Row
    */
   virtual sqlite3_stmt* getSqlite3Statement();
   
public:
   /**
    * Creates a new Row from the given Statement.
    */
   Sqlite3Row(Sqlite3Statement* s);
   
   /**
    * Destructs this Row.
    */
   virtual ~Sqlite3Row();
   
   /**
    * Gets a column's data type.
    *
    * @param column the column's index.
    * @param type the type ID for the column.
    * 
    * @return a DatabaseException if one occurred, NULL if not.
    */
   virtual DatabaseException* getType(int column, int& type);
   
   /**
    * Gets an integer from a column.
    * 
    * @param column the column's index.
    * @param i the integer to store the integer in.
    * 
    * @return a DatabaseException if one occurred, NULL if not.
    */
   virtual DatabaseException* getInteger(int column, int& i);
   
   /**
    * Gets a text string from a column.
    * 
    * @param column the column's index.
    * @param str the string to store the text in.
    * 
    * @return a DatabaseException if one occurred, NULL if not.
    */
   virtual DatabaseException* getText(int column, std::string& str);
};

} // end namespace sqlite3
} // end namespace database
} // end namespace db
#endif
