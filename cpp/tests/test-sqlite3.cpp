/*
 * Copyright (c) 2007-2010 Digital Bazaar, Inc. All rights reserved.
 */
#include "monarch/test/Test.h"
#include "monarch/test/Tester.h"
#include "monarch/test/TestRunner.h"
#include "monarch/rt/Thread.h"
#include "monarch/sql/Row.h"
#include "monarch/sql/StatementBuilder.h"
#include "monarch/sql/sqlite3/Sqlite3Connection.h"
#include "monarch/sql/sqlite3/Sqlite3ConnectionPool.h"
#include "monarch/sql/sqlite3/Sqlite3DatabaseClient.h"
#include "monarch/util/Timer.h"

#include <cstdio>

using namespace std;
using namespace monarch::rt;
using namespace monarch::sql;
using namespace monarch::sql::sqlite3;
using namespace monarch::test;
using namespace monarch::util;

#define TABLE_TEST_1 "test_1"
#define TABLE_TEST_2 "test_2"
#define TABLE_TEST_3 "test_3"
#define TABLE_TEST_4 "test_4"
#define TABLE_TEST_5 "test_5"

void createSqlite3Table(TestRunner* tr, monarch::sql::Connection* c)
{
   if(tr != NULL)
   {
      tr->test("drop table");
   }
   {
      monarch::sql::Statement* s = c->prepare(
         "DROP TABLE IF EXISTS " TABLE_TEST_1);
      assertNoException();
      int success = s->execute();
      assertNoException();
      assert(success);
   }
   if(tr != NULL)
   {
      tr->passIfNoException();
   }
   else
   {
      assertNoException();
   }
#if 0
   if(tr != NULL)
   {
      tr->test("drop table 2");
   }
   {
      monarch::sql::Statement* s = c->prepare(
         "DROP TABLE IF EXISTS " TABLE_TEST_2);
      assertNoException();
      int success = s->execute();
      assertNoException();
      assert(success);
   }
   if(tr != NULL)
   {
      tr->passIfNoException();
   }
   else
   {
      assertNoException();
   }
#endif
   if(tr != NULL)
   {
      tr->test("create table");
   }
   {
      monarch::sql::Statement* s = c->prepare(
         "CREATE TABLE IF NOT EXISTS " TABLE_TEST_1 " (t TEXT, i INT)");
      assertNoException();
      int success = s->execute();
      assertNoException();
      assert(success);
   }
   if(tr != NULL)
   {
      tr->passIfNoException();
   }
   else
   {
      assertNoException();
   }
#if 0
   if(tr != NULL)
   {
      tr->test("create table 2");
   }
   {
      monarch::sql::Statement* s = c->prepare(
         "CREATE TABLE IF NOT EXISTS " TABLE_TEST_2 " (t TEXT, i INT)");
      assertNoException();
      int success = s->execute();
      assertNoException();
      assert(success);
   }
   if(tr != NULL)
   {
      tr->passIfNoException();
   }
   else
   {
      assertNoException();
   }
#endif
}

void executeSqlite3Statements(TestRunner* tr, monarch::sql::Connection* c)
{
   if(tr != NULL)
   {
      tr->test("insert test 1");
   }
   {
      monarch::sql::Statement* s = c->prepare(
         "INSERT INTO " TABLE_TEST_1 " (t, i) VALUES ('test!', 1234)");
      assertNoException();
      int success = s->execute();
      assertNoException();
      assert(success);
   }
   if(tr != NULL)
   {
      tr->passIfNoException();
   }
   else
   {
      assertNoException();
   }

   if(tr != NULL)
   {
      tr->test("insert test 2");
   }
   {
      monarch::sql::Statement* s = c->prepare(
         "INSERT INTO " TABLE_TEST_1 " (t, i) VALUES ('!tset', 4321)");
      assertNoException();
      int success = s->execute();
      assertNoException();
      assert(success);
   }
   if(tr != NULL)
   {
      tr->passIfNoException();
   }
   else
   {
      assertNoException();
   }

   if(tr != NULL)
   {
      tr->test("insert positional parameters");
   }
   {
      monarch::sql::Statement* s = c->prepare(
         "INSERT INTO " TABLE_TEST_1 " (t, i) VALUES (?, ?)");
      assertNoException();
      s->setText(1, "boundpositional");
      s->setInt32(2, 2222);
      int success = s->execute();
      assertNoException();
      assert(success);
   }
   if(tr != NULL)
   {
      tr->passIfNoException();
   }
   else
   {
      assertNoException();
   }

   if(tr != NULL)
   {
      tr->test("insert named parameters");
   }
   {
      monarch::sql::Statement* s = c->prepare(
         "INSERT INTO " TABLE_TEST_1 " (t, i) VALUES (:first, :second)");
      assertNoException();
      s->setText(":first", "boundnamed");
      s->setInt32(":second", 2223);
      int success = s->execute();
      assertNoException();
      assert(success);
   }
   if(tr != NULL)
   {
      tr->passIfNoException();
   }
   else
   {
      assertNoException();
   }

   if(tr != NULL)
   {
      tr->test("select");
   }
   {
      monarch::sql::Statement* s = c->prepare("SELECT * FROM " TABLE_TEST_1);
      assertNoException();
      int success = s->execute();
      assertNoException();
      assert(success);

      Row* row;
      string t;
      int i;
      while((row = s->fetch()) != NULL)
      {
         row->getText("t", t);
         assertNoException();
         row->getInt32("i", i);
         assertNoException();

         if(strcmp(t.c_str(), "test!") == 0)
         {
            assert(i == 1234);
         }
         else if(strcmp(t.c_str(), "!tset") == 0)
         {
            assert(i == 4321);
         }
         else if(strcmp(t.c_str(), "boundpositional") == 0)
         {
            assert(i == 2222);
         }
         else if(strcmp(t.c_str(), "boundnamed") == 0)
         {
            assert(i == 2223);
         }
         else
         {
            // bad row data
            assert(false);
         }
      }
   }
   if(tr != NULL)
   {
      tr->passIfNoException();
   }
   else
   {
      assertNoException();
   }
#if 0
   if(tr != NULL)
   {
      tr->test("insert test 1");
   }
   {
      monarch::sql::Statement* s = c->prepare(
         "INSERT INTO " TABLE_TEST_2 " (t, i) VALUES ('test!', 1234)");
      assertNoException();
      int success = s->execute();
      assertNoException();
      assert(success);
   }
   if(tr != NULL)
   {
      tr->passIfNoException();
   }
   else
   {
      assertNoException();
   }

   if(tr != NULL)
   {
      tr->test("insert test 2");
   }
   {
      monarch::sql::Statement* s = c->prepare(
         "INSERT INTO " TABLE_TEST_2 " (t, i) VALUES ('!tset', 4321)");
      assertNoException();
      int success = s->execute();
      assertNoException();
      assert(success);
   }
   if(tr != NULL)
   {
      tr->passIfNoException();
   }
   else
   {
      assertNoException();
   }

   if(tr != NULL)
   {
      tr->test("insert positional parameters");
   }
   {
      monarch::sql::Statement* s = c->prepare(
         "INSERT INTO " TABLE_TEST_2 " (t, i) VALUES (?, ?)");
      assertNoException();
      s->setText(1, "boundpositional");
      s->setInt32(2, 2222);
      int success = s->execute();
      assertNoException();
      assert(success);
   }
   if(tr != NULL)
   {
      tr->passIfNoException();
   }
   else
   {
      assertNoException();
   }

   if(tr != NULL)
   {
      tr->test("insert named parameters");
   }
   {
      monarch::sql::Statement* s = c->prepare(
         "INSERT INTO " TABLE_TEST_2 " (t, i) VALUES (:first, :second)");
      assertNoException();
      s->setText(":first", "boundnamed");
      s->setInt32(":second", 2223);
      int success = s->execute();
      assertNoException();
      assert(success);
   }
   if(tr != NULL)
   {
      tr->passIfNoException();
   }
   else
   {
      assertNoException();
   }

   if(tr != NULL)
   {
      tr->test("select");
   }
   {
      monarch::sql::Statement* s = c->prepare("SELECT * FROM " TABLE_TEST_2);
      assertNoException();
      int success = s->execute();
      assertNoException();
      assert(success);

      Row* row;
      string t;
      int i;
      while((row = s->fetch()) != NULL)
      {
         row->getText("t", t);
         assertNoException();
         row->getInt32("i", i);
         assertNoException();

         if(strcmp(t.c_str(), "test!") == 0)
         {
            assert(i == 1234);
         }
         else if(strcmp(t.c_str(), "!tset") == 0)
         {
            assert(i == 4321);
         }
         else if(strcmp(t.c_str(), "boundpositional") == 0)
         {
            assert(i == 2222);
         }
         else if(strcmp(t.c_str(), "boundnamed") == 0)
         {
            assert(i == 2223);
         }
         else
         {
            // bad row data
            assert(false);
         }
      }
   }
   if(tr != NULL)
   {
      tr->passIfNoException();
   }
   else
   {
      assertNoException();
   }
#endif
}

void runSqlite3ConnectionTest(TestRunner& tr)
{
   tr.test("Sqlite3 Connection");

   Sqlite3Connection c;
   c.connect("sqlite3::memory:");
   c.close();
   assertNoException();

   tr.pass();
}

void runSqlite3PrepareManyTest(TestRunner& tr)
{
   // testing create and cleanup of many statements
   // originially used to help find a memory corruption bug
   tr.test("Sqlite3 Prepare Many");
   {
      Sqlite3Connection c;
      c.connect("sqlite3::memory:");
      int n = 100;
      while(n--)
      {
         c.prepare("SELECT 1");
         c.cleanupPreparedStatements();
      }
      c.close();
      assertNoException();
   }
   tr.pass();
}

void runSqlite3StatementTest(TestRunner& tr)
{
   tr.group("Sqlite3 Statement");

   // clear any exceptions
   Exception::clear();

   Sqlite3Connection c;
   c.connect("sqlite3::memory:");

   // create table
   createSqlite3Table(&tr, &c);

   // execute statements
   executeSqlite3Statements(&tr, &c);

   tr.test("connection close");
   {
      c.close();
   }
   tr.passIfNoException();

   tr.ungroup();
}

void runSqlite3TableTest(TestRunner& tr)
{
   tr.group("Sqlite3 Table");

   // clear any exceptions
   Exception::clear();

   Sqlite3Connection c;
   c.connect("sqlite3::memory:");

   // clean up table if it exists
   tr.test("drop table if exists");
   {
      monarch::sql::Statement* s = c.prepare(
         "DROP TABLE IF EXISTS " TABLE_TEST_1);
      assertNoException();
      int success = s->execute();
      assertNoException();
      assert(success);
   }
   tr.passIfNoException();

   // create a fresh table
   tr.test("create table");
   {
      monarch::sql::Statement* s = c.prepare(
         "CREATE TABLE " TABLE_TEST_1 " (t TEXT, i INT)");
      assertNoException();
      int success = s->execute();
      assertNoException();
      assert(success);
   }
   tr.passIfNoException();

   // drop table
   tr.test("drop table");
   {
      monarch::sql::Statement* s = c.prepare(
         "DROP TABLE " TABLE_TEST_1);
      assertNoException();
      int success = s->execute();
      assertNoException();
      assert(success);
   }
   tr.passIfNoException();

   tr.test("connection close");
   {
      c.close();
   }
   tr.passIfNoException();

   tr.ungroup();
}

void runSqlite3TableMigrationTest(TestRunner& tr)
{
   tr.group("Sqlite3 Table Migration (1)");
   {
      // test table migration algorithm
      // - begin transaction
      // - alter t1 name to t1_old
      // - create new t1
      // - copy t1_old data to t1
      // - drop t1_old
      // - commit

      // clear any exceptions
      Exception::clear();

      Sqlite3Connection c;
      c.connect("sqlite3::memory:");

      tr.test("create test table");
      {
         monarch::sql::Statement* s = c.prepare("CREATE TABLE t1 (t TEXT, i INT)");
         assertNoException();
         int success = s->execute();
         assertNoException();
         assert(success);
      }
      tr.passIfNoException();

      tr.test("begin");
      {
         c.begin();
      }
      tr.passIfNoException();

      tr.test("rename");
      {
         monarch::sql::Statement* s = c.prepare("ALTER TABLE t1 RENAME TO t1_old");
         assertNoException();
         int success = s->execute();
         assertNoException();
         assert(success);
      }
      tr.passIfNoException();

      tr.test("create new table");
      {
         monarch::sql::Statement* s = c.prepare("CREATE TABLE t1 (t TEXT, i INT)");
         assertNoException();
         int success = s->execute();
         assertNoException();
         assert(success);
      }
      tr.passIfNoException();

      tr.test("copy data");
      {
         monarch::sql::Statement* s =
            c.prepare("INSERT INTO t1 SELECT * FROM t1_old");
         assertNoException();
         int success = s->execute();
         assertNoException();
         assert(success);
      }
      tr.passIfNoException();

      tr.test("drop old table");
      {
         monarch::sql::Statement* s = c.prepare("DROP TABLE t1_old");
         assertNoException();
         int success = s->execute();
         assertNoException();
         assert(success);
      }
      tr.passIfNoException();

      tr.test("commit");
      {
         c.commit();
      }
      tr.passIfNoException();

      tr.test("connection close");
      {
         c.close();
      }
      tr.passIfNoException();
   }
   tr.ungroup();

   tr.group("Sqlite3 Table Migration (2)");
   {
      // test table migration algorithm 2
      // - begin transaction
      // - create temp table t1_new (new schema)
      // - copy/migrate t1 data to t1_new
      // - drop t1
      // - create table t1 (new schema)
      // - copy t1_new data to t1
      // - drop t1_new
      // - commit

      // clear any exceptions
      Exception::clear();

      Sqlite3Connection c;
      c.connect("sqlite3::memory:");

      tr.test("create test table");
      {
         monarch::sql::Statement* s = c.prepare("CREATE TABLE t1 (t TEXT, i INT)");
         assertNoException();
         int success = s->execute();
         assertNoException();
         assert(success);
      }
      tr.passIfNoException();

      tr.test("begin");
      {
         c.begin();
      }
      tr.passIfNoException();

      tr.test("create new temp table");
      {
         monarch::sql::Statement* s =
            c.prepare("CREATE TEMPORARY TABLE t1_new (t TEXT, i INT)");
         assertNoException();
         int success = s->execute();
         assertNoException();
         assert(success);
      }
      tr.passIfNoException();

      tr.test("copy data");
      {
         monarch::sql::Statement* s =
            c.prepare("INSERT INTO t1_new SELECT * FROM t1");
         assertNoException();
         int success = s->execute();
         assertNoException();
         assert(success);
      }
      tr.passIfNoException();

      tr.test("drop old table");
      {
         monarch::sql::Statement* s = c.prepare("DROP TABLE t1");
         assertNoException();
         int success = s->execute();
         assertNoException();
         assert(success);
      }
      tr.passIfNoException();

      tr.test("create new table");
      {
         monarch::sql::Statement* s =
            c.prepare("CREATE TABLE t1 (t TEXT, i INT)");
         assertNoException();
         int success = s->execute();
         assertNoException();
         assert(success);
      }
      tr.passIfNoException();

      tr.test("copy data");
      {
         monarch::sql::Statement* s =
            c.prepare("INSERT INTO t1 SELECT * FROM t1_new");
         assertNoException();
         int success = s->execute();
         assertNoException();
         assert(success);
      }
      tr.passIfNoException();

      tr.test("drop temp table");
      {
         monarch::sql::Statement* s = c.prepare("DROP TABLE t1_new");
         assertNoException();
         int success = s->execute();
         assertNoException();
         assert(success);
      }
      tr.passIfNoException();

      tr.test("commit");
      {
         c.commit();
      }
      tr.passIfNoException();

      tr.test("connection close");
      {
         c.close();
      }
      tr.passIfNoException();
   }
   tr.ungroup();

   tr.group("Sqlite3 Table Migration (3)");
   {
      // test table migration algorithm 3
      // - begin transaction
      // - create temp table t1_old with old data
      // - drop t1
      // - create table t1 with new schema
      // - copy/migrate t1_old data to t1
      // - drop t1_old
      // - commit

      // clear any exceptions
      Exception::clear();

      Sqlite3Connection c;
      c.connect("sqlite3::memory:");

      tr.test("create test table");
      {
         monarch::sql::Statement* s = c.prepare("CREATE TABLE t1 (t TEXT, i INT)");
         assertNoException();
         int success = s->execute();
         assertNoException();
         assert(success);
      }
      tr.passIfNoException();

      tr.test("begin");
      {
         c.begin();
      }
      tr.passIfNoException();

      tr.test("create new temp table");
      {
         monarch::sql::Statement* s =
            c.prepare("CREATE TEMPORARY TABLE t1_old AS SELECT * FROM t1");
         assertNoException();
         int success = s->execute();
         assertNoException();
         assert(success);
      }
      tr.passIfNoException();

      tr.test("drop old table");
      {
         monarch::sql::Statement* s = c.prepare("DROP TABLE t1");
         assertNoException();
         int success = s->execute();
         assertNoException();
         assert(success);
      }
      tr.passIfNoException();

      tr.test("create new table");
      {
         monarch::sql::Statement* s =
            c.prepare("CREATE TABLE t1 (t TEXT, i INT)");
         assertNoException();
         int success = s->execute();
         assertNoException();
         assert(success);
      }
      tr.passIfNoException();

      tr.test("copy data");
      {
         monarch::sql::Statement* s =
            c.prepare("INSERT INTO t1 SELECT * FROM t1_old");
         assertNoException();
         int success = s->execute();
         assertNoException();
         assert(success);
      }
      tr.passIfNoException();

      tr.test("drop temp table");
      {
         monarch::sql::Statement* s = c.prepare("DROP TABLE t1_old");
         assertNoException();
         int success = s->execute();
         assertNoException();
         assert(success);
      }
      tr.passIfNoException();

      tr.test("commit");
      {
         c.commit();
      }
      tr.passIfNoException();

      tr.test("connection close");
      {
         c.close();
      }
      tr.passIfNoException();
   }
   tr.ungroup();
}

class Sqlite3ThreadTest : public Runnable
{
public:
   Sqlite3Connection* connection;

   virtual void run()
   {
      connection = new Sqlite3Connection();
      connection->connect("sqlite3::memory:");
      //connection->connect("sqlite3:///tmp/sqlite3cptest.db");
   }
};

void runSqlite3ThreadTest(TestRunner& tr)
{
   tr.group("Sqlite3 multithread");

   // create sqlite3 connection in another thread
   Sqlite3ThreadTest runnable;
   Thread t(&runnable);
   t.start();
   t.join();

   // use sqlite3 connection in this thread
   tr.test("connection created in separate thread");
   {
      monarch::sql::Connection* c = runnable.connection;

      // create table
      createSqlite3Table(NULL, c);

      // execute statements
      executeSqlite3Statements(NULL, c);

      // close connection
      c->close();
      delete c;
   }
   tr.passIfNoException();

   tr.ungroup();
}

void runSqlite3ReuseTest(TestRunner& tr)
{
   tr.group("Reuse");

   // clear any exceptions
   Exception::clear();

   // create sqlite3 connection pool
   Sqlite3ConnectionPool cp("sqlite3::memory:", 1);
   assertNoException();

   tr.test("create table");
   {
      // create table
      monarch::sql::Connection* c = cp.getConnection();
      assert(c != NULL);
      monarch::sql::Statement* s = c->prepare(
         "CREATE TABLE IF NOT EXISTS " TABLE_TEST_1 " (t TEXT, i INT)");
      assertNoException();
      int success = s->execute();
      assertNoException();
      assert(success);
      c->close();
   }
   tr.passIfNoException();

   tr.test("insert row");
   {
      // create table
      monarch::sql::Connection* c = cp.getConnection();
      assert(c != NULL);
      monarch::sql::Statement* s = c->prepare(
         "INSERT INTO " TABLE_TEST_1 " (t, i) VALUES ('test!', 1234)");
      assertNoException();
      int success = s->execute();
      assertNoException();
      assert(success);
      c->close();
   }
   tr.passIfNoException();

   tr.test("select single row");
   {
      // select single row
      monarch::sql::Connection* c = cp.getConnection();
      assert(c != NULL);
      monarch::sql::Statement* s = c->prepare(
         "SELECT * FROM " TABLE_TEST_1 " WHERE i=:i LIMIT 1");
      assertNoException();
      s->setInt32(":i", 1234);
      int success = s->execute();
      assertNoException();
      assert(success);

      Row* row = s->fetch();
      assert(row != NULL);
      string t;
      int i;

      row->getText("t", t);
      assertNoException();
      row->getInt32("i", i);
      assertNoException();

      assertStrCmp(t.c_str(), "test!");
      assert(i == 1234);

      c->close();
   }
   tr.passIfNoException();

   tr.test("select single row again");
   {
      // select single row
      monarch::sql::Connection* c = cp.getConnection();
      assert(c != NULL);
      monarch::sql::Statement* s = c->prepare(
         "SELECT * FROM " TABLE_TEST_1 " WHERE i=:i LIMIT 1");
      assertNoException();
      s->setInt32(":i", 1234);
      int success = s->execute();
      assertNoException();
      assert(success);

      Row* row = s->fetch();
      assert(row != NULL);
      string t;
      int i;

      row->getText("t", t);
      assertNoException();
      row->getInt32("i", i);
      assertNoException();

      assertStrCmp(t.c_str(), "test!");
      assert(i == 1234);

      c->close();
   }
   tr.passIfNoException();

   tr.ungroup();
}

void runSqlite3DatabaseClientTest(TestRunner& tr)
{
   tr.group("DatabaseClient");

   // create sqlite3 connection pool
   Sqlite3ConnectionPool* cp = new Sqlite3ConnectionPool("sqlite3::memory:", 1);
   ConnectionPoolRef pool(cp);
   assertNoException();

   // create database client
   DatabaseClientRef dbc = new Sqlite3DatabaseClient();
   dbc->setDebugLogging(true);
   dbc->setReadConnectionPool(pool);
   dbc->setWriteConnectionPool(pool);

   tr.test("initialize");
   {
      dbc->initialize();
   }
   tr.passIfNoException();

   tr.test("define table");
   {
      SchemaObject schema;
      schema["table"] = TABLE_TEST_1;

      DatabaseClient::addSchemaColumn(schema,
         "foo_id", "INTEGER PRIMARY KEY", "fooId", UInt64);
      DatabaseClient::addSchemaColumn(schema,
         "foo_string", "TEXT", "fooString", String);
      DatabaseClient::addSchemaColumn(schema,
         "foo_flag", "INTEGER", "fooFlag", Boolean);
      DatabaseClient::addSchemaColumn(schema,
         "foo_int32", "INTEGER", "fooInt32", Int32);

      dbc->define(schema);
   }
   tr.passIfNoException();

   tr.test("create table");
   {
      dbc->create(TABLE_TEST_1, false);
   }
   tr.passIfNoException();

   tr.test("create table if not exists");
   {
      dbc->create(TABLE_TEST_1, true);
   }
   tr.passIfNoException();

   tr.test("insert");
   {
      DynamicObject row;
      row["fooString"] = "foobar";
      row["fooFlag"] = true;
      row["fooInt32"] = 3;
      SqlExecutableRef se = dbc->insert(TABLE_TEST_1, row);
      dbc->execute(se);
      assertNoException();
      row["fooId"] = se->lastInsertRowId;

      DynamicObject expect;
      expect["fooId"] = 1;
      expect["fooString"] = "foobar";
      expect["fooFlag"] = true;
      expect["fooInt32"] = 3;
      if(expect != row)
      {
         printf("expected:\n");
         dumpDynamicObject(expect);
         printf("got:\n");
         dumpDynamicObject(row);
      }
      assert(expect == row);
   }
   tr.passIfNoException();

   tr.test("insert again");
   {
      DynamicObject row;
      row["fooString"] = "foobar";
      row["fooFlag"] = false;
      row["fooInt32"] = 3;
      SqlExecutableRef se = dbc->insert(TABLE_TEST_1, row);
      dbc->execute(se);
      assertNoException();
      row["fooId"] = se->lastInsertRowId;

      DynamicObject expect;
      expect["fooId"] = 2;
      expect["fooString"] = "foobar";
      expect["fooFlag"] = false;
      expect["fooInt32"] = 3;
      if(expect != row)
      {
         printf("expected:\n");
         dumpDynamicObject(expect);
         printf("got:\n");
         dumpDynamicObject(row);
      }
      assert(expect == row);
   }
   tr.passIfNoException();

   tr.test("select one");
   {
      DynamicObject where;
      where["fooId"] = 1;
      SqlExecutableRef se = dbc->selectOne(TABLE_TEST_1, &where);
      dbc->execute(se);
      assertNoException();

      DynamicObject expect;
      expect["fooId"] = 1;
      expect["fooString"] = "foobar";
      expect["fooFlag"] = true;
      expect["fooInt32"] = 3;
      if(expect != se->result)
      {
         printf("expected:\n");
         dumpDynamicObject(expect);
         printf("got:\n");
         dumpDynamicObject(se->result);
      }
      assert(expect == se->result);
   }
   tr.passIfNoException();

   tr.test("select one specific member");
   {
      DynamicObject where;
      where["fooId"] = 1;
      DynamicObject members;
      members["fooString"];
      SqlExecutableRef se = dbc->selectOne(TABLE_TEST_1, &where, &members);
      dbc->execute(se);
      assertNoException();

      DynamicObject expect;
      expect["fooString"] = "foobar";
      if(expect != se->result)
      {
         printf("expected:\n");
         dumpDynamicObject(expect);
         printf("got:\n");
         dumpDynamicObject(se->result);
      }
      assert(expect == se->result);
   }
   tr.passIfNoException();

   tr.test("select");
   {
      DynamicObject where;
      where["fooInt32"] = 3;
      SqlExecutableRef se = dbc->select(TABLE_TEST_1, &where, NULL, 5);
      dbc->execute(se);
      assertNoException();

      DynamicObject expect;
      expect->setType(Array);
      DynamicObject& first = expect->append();
      first["fooId"] = 1;
      first["fooString"] = "foobar";
      first["fooFlag"] = true;
      first["fooInt32"] = 3;
      DynamicObject& second = expect->append();
      second["fooId"] = 2;
      second["fooString"] = "foobar";
      second["fooFlag"] = false;
      second["fooInt32"] = 3;
      if(expect != se->result)
      {
         printf("expected:\n");
         dumpDynamicObject(expect);
         printf("got:\n");
         dumpDynamicObject(se->result);
      }
      assert(expect == se->result);
   }
   tr.passIfNoException();

   tr.test("update");
   {
      DynamicObject row;
      row["fooString"] = "foobar2";
      DynamicObject where;
      where["fooId"] = 2;
      SqlExecutableRef se = dbc->update(TABLE_TEST_1, row, &where);
      dbc->execute(se);
      assert(se->rowsAffected = 1);
   }
   tr.passIfNoException();

   tr.test("update w/limit");
   {
      DynamicObject row;
      row["fooString"] = "bar";
      DynamicObject where;
      where["fooId"] = 2;
      SqlExecutableRef se = dbc->update(TABLE_TEST_1, row, &where, 1);
      dbc->execute(se);
      assert(se->rowsAffected = 1);
   }
   tr.passIfNoException();

   tr.test("select updated one");
   {
      DynamicObject where;
      where["fooString"] = "bar";
      SqlExecutableRef se = dbc->selectOne(TABLE_TEST_1, &where);
      dbc->execute(se);
      assertNoException();

      DynamicObject expect;
      expect["fooId"] = 2;
      expect["fooString"] = "bar";
      expect["fooFlag"] = false;
      expect["fooInt32"] = 3;
      if(expect != se->result)
      {
         printf("expected:\n");
         dumpDynamicObject(expect);
         printf("got:\n");
         dumpDynamicObject(se->result);
      }
      assert(expect == se->result);
   }
   tr.passIfNoException();

   tr.test("select updated");
   {
      DynamicObject where;
      where["fooString"] = "bar";
      SqlExecutableRef se = dbc->select(TABLE_TEST_1, &where);
      dbc->execute(se);
      assertNoException();

      DynamicObject expect;
      expect[0]["fooId"] = 2;
      expect[0]["fooString"] = "bar";
      expect[0]["fooFlag"] = false;
      expect[0]["fooInt32"] = 3;
      if(expect != se->result)
      {
         printf("expected:\n");
         dumpDynamicObject(expect);
         printf("got:\n");
         dumpDynamicObject(se->result);
      }
      assert(expect == se->result);
   }
   tr.passIfNoException();

   tr.test("select IN()");
   {
      DynamicObject where;
      where["fooString"]->append() = "bar";
      where["fooString"]->append() = "foobar";
      SqlExecutableRef se = dbc->select(TABLE_TEST_1, &where);
      dbc->execute(se);
      assertNoException();

      DynamicObject expect;
      expect->setType(Array);
      DynamicObject& first = expect->append();
      first["fooId"] = 1;
      first["fooString"] = "foobar";
      first["fooFlag"] = true;
      first["fooInt32"] = 3;
      DynamicObject& second = expect->append();
      second["fooId"] = 2;
      second["fooString"] = "bar";
      second["fooFlag"] = false;
      second["fooInt32"] = 3;
      if(expect != se->result)
      {
         printf("expected:\n");
         dumpDynamicObject(expect);
         printf("got:\n");
         dumpDynamicObject(se->result);
      }
      assert(expect == se->result);
   }
   tr.passIfNoException();

   tr.test("remove");
   {
      DynamicObject where;
      where["fooId"] = 1;
      SqlExecutableRef se = dbc->remove(TABLE_TEST_1, &where);
      dbc->execute(se);
      assert(se->rowsAffected == 1);
   }
   tr.passIfNoException();

   tr.test("select again");
   {
      SqlExecutableRef se = dbc->select(TABLE_TEST_1);
      dbc->execute(se);
      assertNoException();

      DynamicObject expect;
      expect[0]["fooId"] = 2;
      expect[0]["fooString"] = "bar";
      expect[0]["fooFlag"] = false;
      expect[0]["fooInt32"] = 3;
      if(expect != se->result)
      {
         printf("expected:\n");
         dumpDynamicObject(expect);
         printf("got:\n");
         dumpDynamicObject(se->result);
      }
      assert(expect == se->result);
   }
   tr.passIfNoException();

   tr.ungroup();
}

void runSqlite3RollbackTest(TestRunner& tr)
{
   tr.group("rollback");

   // create sqlite3 connection pool
   Sqlite3ConnectionPool* cp = new Sqlite3ConnectionPool("sqlite3::memory:", 1);
   ConnectionPoolRef pool(cp);
   assertNoException();

   // create database client
   DatabaseClientRef dbc = new Sqlite3DatabaseClient();
   dbc->setDebugLogging(true);
   dbc->setReadConnectionPool(pool);
   dbc->setWriteConnectionPool(pool);

   tr.test("initialize");
   {
      dbc->initialize();
   }
   tr.passIfNoException();

   Connection* c = cp->getConnection();

   tr.test("define table");
   {
      SchemaObject schema;
      schema["table"] = TABLE_TEST_1;

      DatabaseClient::addSchemaColumn(schema,
         "foo_id", "INTEGER PRIMARY KEY", "fooId", UInt64);
      DatabaseClient::addSchemaColumn(schema,
         "foo_string", "TEXT", "fooString", String);
      DatabaseClient::addSchemaColumn(schema,
         "foo_flag", "INTEGER", "fooFlag", Boolean);
      DatabaseClient::addSchemaColumn(schema,
         "foo_int32", "INTEGER", "fooInt32", Int32);

      dbc->define(schema);
   }
   tr.passIfNoException();

   tr.test("create table");
   {
      dbc->create(TABLE_TEST_1, false, c);
   }
   tr.passIfNoException();

   tr.test("begin");
   {
      dbc->begin(c);
   }
   tr.passIfNoException();

   tr.test("insert");
   {
      DynamicObject row;
      row["fooString"] = "foobar";
      row["fooFlag"] = true;
      row["fooInt32"] = 3;
      SqlExecutableRef se = dbc->insert(TABLE_TEST_1, row);
      dbc->execute(se, c);
      assertNoException();
      row["fooId"] = se->lastInsertRowId;

      DynamicObject expect;
      expect["fooId"] = 1;
      expect["fooString"] = "foobar";
      expect["fooFlag"] = true;
      expect["fooInt32"] = 3;
      if(expect != row)
      {
         printf("expected:\n");
         dumpDynamicObject(expect);
         printf("got:\n");
         dumpDynamicObject(row);
      }
      assert(expect == row);
   }
   tr.passIfNoException();

   tr.test("insert again");
   {
      DynamicObject row;
      row["fooString"] = "foobar";
      row["fooFlag"] = false;
      row["fooInt32"] = 3;
      SqlExecutableRef se = dbc->insert(TABLE_TEST_1, row);
      dbc->execute(se, c);
      assertNoException();
      row["fooId"] = se->lastInsertRowId;

      DynamicObject expect;
      expect["fooId"] = 2;
      expect["fooString"] = "foobar";
      expect["fooFlag"] = false;
      expect["fooInt32"] = 3;
      if(expect != row)
      {
         printf("expected:\n");
         dumpDynamicObject(expect);
         printf("got:\n");
         dumpDynamicObject(row);
      }
      assert(expect == row);
   }
   tr.passIfNoException();

   tr.test("select bogus");
   {
      DynamicObject where;
      where["fooId"] = 1;
      SqlExecutableRef se = dbc->selectOne(TABLE_TEST_1, &where);
      se->sql.append("BADSQLBLAHBLAH");
      dbc->execute(se, c);
   }
   tr.passIfException();

   tr.test("rollback");
   {
      dbc->end(c, false);
   }
   tr.passIfNoException();

   tr.ungroup();
}

class Sqlite3ConnectionPoolTest : public Runnable
{
public:
   Sqlite3ConnectionPool* pool;
   TestRunner* tr;

   virtual void run()
   {
      monarch::sql::Connection* c = pool->getConnection();
      executeSqlite3Statements(NULL, c);
      //executeSqlite3Statements(*tr, c);
      c->close();
   }
};

void runSqlite3ConnectionPoolTest(TestRunner& tr)
{
   tr.group("Sqlite3 ConnectionPool");

   // create sqlite3 connection pool
   Sqlite3ConnectionPool cp("sqlite3:///tmp/sqlite3cptest.db", 1);
   assertNoException();

   // create table
   monarch::sql::Connection* c = cp.getConnection();
   createSqlite3Table(NULL, c);
   c->close();

   // create connection test threads
   int testCount = 200;
   Sqlite3ConnectionPoolTest tests[testCount];
   Thread* threads[testCount];

   // create threads, set pool for tests
   for(int i = 0; i < testCount; i++)
   {
      tests[i].pool = &cp;
      tests[i].tr = &tr;
      threads[i] = new Thread(&tests[i]);
   }

   uint64_t startTime = Timer::startTiming();

   // run connection threads
   for(int i = 0; i < testCount; i++)
   {
      while(!threads[i]->start(131072))
      {
         threads[i - 1]->join();
      }
   }

   // join threads
   for(int i = 0; i < testCount; i++)
   {
      threads[i]->join();
   }

   double seconds = Timer::getSeconds(startTime);

   // clean up threads
   for(int i = 0; i < testCount; i++)
   {
      delete threads[i];
   }

   // print report
   printf("\nNumber of independent connection uses: %d\n", testCount);
   printf("Number of pooled connections created: %d\n",
      cp.getConnectionCount());
   printf("Total time: %g seconds\n", seconds);

   tr.ungroup();
}

void runSqlite3StatementBuilderTest(TestRunner& tr)
{
   tr.group("Sqlite3 StatementBuilder");

   /* ObjRelMap: {} of
   *    "objectType": object-type
   *    "members": {} of
   *       "member-name": {} of
   *          "objectType": "_col" OR "_fkey" or object-type
   *          "table": if _col or _fkey, then database table name
   *          "column": if _col or _fkey, then database column name
   *          "memberType": if _col or _fkey the expected member type
   *          FIXME: stuff for _fkey mappings
   *          FIXME: stuff for transformations
   */

   // create sqlite3 connection pool
   Sqlite3ConnectionPool* cp = new Sqlite3ConnectionPool("sqlite3::memory:", 1);
   ConnectionPoolRef pool(cp);
   assertNoException();

   // create database client
   DatabaseClientRef dbc = new Sqlite3DatabaseClient();
   dbc->setDebugLogging(true);
   dbc->setReadConnectionPool(pool);
   dbc->setWriteConnectionPool(pool);
   dbc->initialize();
   assertNoException();

   // define Test object type
   tr.test("set Test OR map");
   {
      ObjRelMap orMap;
      orMap["objectType"] = "Test";

      // define the object's members
      DynamicObject& members = orMap["members"];

      // id column
      {
         DynamicObject& entry = members["id"];
         entry["objectType"] = "_col";
         entry["table"] = TABLE_TEST_1;
         entry["column"] = "id";
         entry["columnType"]->setType(UInt64);
         entry["memberType"]->setType(String);
      }

      // t column
      {
         DynamicObject& entry = members["description"];
         entry["objectType"] = "_col";
         entry["table"] = TABLE_TEST_1;
         entry["column"] = "t";
         entry["columnType"]->setType(String);
         entry["memberType"]->setType(String);
      }

      // i column
      {
         DynamicObject& entry = members["number"];
         entry["objectType"] = "_col";
         entry["table"] = TABLE_TEST_1;
         entry["column"] = "i";
         entry["columnType"]->setType(UInt32);
         entry["memberType"]->setType(UInt32);
      }

      // type w/foreign key
      {
         DynamicObject& entry = members["type"];
         entry["objectType"] = "_fkey";
         entry["table"] = TABLE_TEST_1;
         entry["column"] = "type";
         entry["ftable"] = TABLE_TEST_2;
         entry["fkey"] = "type_id";
         entry["fcolumn"] = "type_value";
         entry["columnType"]->setType(String);
         entry["memberType"]->setType(String);
      }

      dbc->setObjRelMap(orMap);
   }
   tr.passIfNoException();

   // FIXME: define a complex object type with hierarchy and foreign keys
   // FIXME: get the actual data out

   monarch::sql::Connection* c = dbc->getWriteConnection();

   // create test tables
   tr.test("create tables");
   {
      monarch::sql::Statement* s;

      s = c->prepare(
         "CREATE TABLE IF NOT EXISTS " TABLE_TEST_1
         " (id INTEGER UNSIGNED PRIMARY KEY, t TEXT, i INTEGER UNSIGNED,"
         "type_id BIGINT UNSIGNED)");
      assert(s != NULL);
      s->execute();
      assertNoException();

      s = c->prepare(
         "CREATE TABLE IF NOT EXISTS " TABLE_TEST_2
         " (type_id INTEGER UNSIGNED PRIMARY KEY, type_value TEXT)");
      assert(s != NULL);
      s->execute();
      assertNoException();
   }
   tr.passIfNoException();

   tr.test("add Test object");
   {
      DynamicObject testObj;
      testObj["id"] = "123";
      testObj["description"] = "My test object description";
      testObj["number"] = 10;
      testObj["type"] = "type1";

      StatementBuilder sb(dbc);
      sb.add("Test", testObj).execute(c);
   }
   tr.passIfNoException();

   tr.test("update Test object");
   {
      DynamicObject testObj;
      testObj["id"] = "123";
      testObj["description"] = "A different test object description";
      testObj["number"] = 12;
      testObj["type"] = "type2";

      StatementBuilder sb(dbc);
      sb.update("Test", testObj).execute(c);
   }
   tr.passIfNoException();

   tr.test("get full Test object");
   {
      StatementBuilder sb(dbc);
      sb.get("Test").limit(1).execute(c);
   }
   tr.passIfNoException();

   tr.test("get Test object IDs");
   {
      DynamicObject testObj;
      testObj["id"];

      StatementBuilder sb(dbc);
      sb.get("Test", &testObj).execute(c);
   }
   tr.passIfNoException();

   c->close();

   tr.ungroup();
}

class MoSqlite3Tester : public monarch::test::Tester
{
public:
   MoSqlite3Tester()
   {
      setName("sqlite3");
   }

   /**
    * Run automatic unit tests.
    */
   virtual int runAutomaticTests(TestRunner& tr)
   {
      runSqlite3ConnectionTest(tr);
      runSqlite3PrepareManyTest(tr);
      runSqlite3StatementTest(tr);
      runSqlite3TableTest(tr);
      runSqlite3TableMigrationTest(tr);
      runSqlite3ThreadTest(tr);
      runSqlite3ReuseTest(tr);
      runSqlite3DatabaseClientTest(tr);
      runSqlite3RollbackTest(tr);
      return 0;
   }

   /**
    * Runs interactive unit tests.
    */
   virtual int runInteractiveTests(TestRunner& tr)
   {
      //runSqlite3ConnectionPoolTest(tr);
      runSqlite3StatementBuilderTest(tr);
      return 0;
   }
};

monarch::test::Tester* getMoSqlite3Tester() { return new MoSqlite3Tester(); }


MO_TEST_MAIN(MoSqlite3Tester)
