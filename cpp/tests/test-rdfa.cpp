/*
 * Copyright (c) 2010-2012 Digital Bazaar, Inc. All rights reserved.
 */
#define __STDC_CONSTANT_MACROS

#include "monarch/data/json/JsonWriter.h"
#include "monarch/data/rdfa/RdfaReader.h"
#include "monarch/io/ByteArrayInputStream.h"
#include "monarch/logging/Logging.h"
#include "monarch/rt/DynamicObject.h"
#include "monarch/test/Test.h"
#include "monarch/test/TestModule.h"

#include <cstdio>

using namespace std;
using namespace monarch::data::json;
using namespace monarch::data::rdfa;
using namespace monarch::io;
using namespace monarch::modest;
using namespace monarch::rt;
using namespace monarch::test;

namespace mo_test_data_rdfa
{

static void runRdfaReaderTest(TestRunner& tr)
{
   tr.group("RdfaReader");

   tr.test("no base URI");
   {
      string rdfa =
         "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
         "<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML+RDFa 1.0//EN\" "
         "\"http://www.w3.org/MarkUp/DTD/xhtml-rdfa-1.dtd\">\n"
         "<html xmlns=\"http://www.w3.org/1999/xhtml\"\n"
         "      xmlns:dc=\"http://purl.org/dc/elements/1.1/\">\n"
         "<head><title>Test</title></head>\n"
         "<body><p>\n"
         "<span about=\"#foo\" rel=\"dc:title\" resource=\"#you\" />\n"
         "</p></body>\n"
         "</html>";

      ByteArrayInputStream bais(rdfa.c_str(), rdfa.length());
      RdfaReader reader;
      DynamicObject dyno;
      assertException(
         reader.start(dyno));

      MO_DEBUG("%s", JsonWriter::writeToString(dyno).c_str());
   }
   tr.passIfException();

   tr.test("no base URI (readFromString)");
   {
      string rdfa = "...";

      ByteArrayInputStream bais(rdfa.c_str(), rdfa.length());
      RdfaReader reader;
      DynamicObject dyno;
      assertException(
         RdfaReader::readFromString(dyno, "", 0, ""));

      MO_DEBUG("%s", JsonWriter::writeToString(dyno).c_str());
   }
   tr.passIfException();

   tr.test("error");
   {
      string rdfa =
         "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
         "<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML+RDFa 1.0//EN\" "
         "\"http://www.w3.org/MarkUp/DTD/xhtml-rdfa-1.dtd\">\n"
         "<html xmlns=\"http://www.w3.org/1999/xhtml\"\n"
         "      xmlns:dc=\"http://purl.org/dc/elements/1.1/\">\n"
         "<head><title>Test</title></head>\n"
         "<body><p>\n"
         "<span about=\"#foo\" rel=\"dc:title\" resource=\"#you\" />\n"
         "<p>\n"
         "</p></body>\n"
         "</html>";

      // custom context
      DynamicObject ctx;
      ctx["dc"] = "http://purl.org/dc/terms/";
      ctx["w3"] = "http://www.w3.org/2009/pointers#";

      // Check with low level API
      ByteArrayInputStream bais(rdfa.c_str(), rdfa.length());
      RdfaReader reader;
      reader.setBaseUri("http://example.org/test");
      reader.setContext(ctx);
      DynamicObject dyno;
      assertNoException(
         reader.start(dyno));
      assertException(
         reader.read(&bais));

      // check exception
      DynamicObject ex = Exception::getAsDynamicObject();

      DynamicObject expect;
      expect["message"] = "RDFa parse error.";
      expect["type"] = "monarch.data.rdfa.RdfaReader.ParseError";
      DynamicObject& graph = expect["details"]["graph"];
      graph["@context"]["dc"] =
         "http://purl.org/dc/terms/";
      graph["@context"]["w3"] =
         "http://www.w3.org/2009/pointers#";
      graph["@id"] = "_:c14n0";
      graph["@type"] = "http://www.w3.org/ns/rdfa_processing_graph#Error";
      graph["dc:description"]["@language"] = "en";
      graph["dc:description"]["@value"] =
         "XML parsing error: mismatched tag at line 9, column 6.";
      graph["http://www.w3.org/ns/rdfa_processing_graph#context"]
         ["@id"] = "_:c14n1";
      graph["http://www.w3.org/ns/rdfa_processing_graph#context"]
         ["@type"] = "w3:LineCharPointer";
      graph["http://www.w3.org/ns/rdfa_processing_graph#context"]
         ["w3:charNumber"]["@type"] =
            "http://www.w3.org/2001/XMLSchema#positiveInteger";
      graph["http://www.w3.org/ns/rdfa_processing_graph#context"]
         ["w3:charNumber"]["@value"] = "6";
      graph["http://www.w3.org/ns/rdfa_processing_graph#context"]
         ["w3:lineNumber"]["@type"] =
            "http://www.w3.org/2001/XMLSchema#positiveInteger";
      graph["http://www.w3.org/ns/rdfa_processing_graph#context"]
         ["w3:lineNumber"]["@value"] = "9";
      assertDynoCmp(expect, ex);

      MO_DEBUG("%s", JsonWriter::writeToString(expect).c_str());
   }
   tr.passIfException();

   tr.test("simple");
   {
      string rdfa =
         "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
         "<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML+RDFa 1.0//EN\" "
         "\"http://www.w3.org/MarkUp/DTD/xhtml-rdfa-1.dtd\">\n"
         "<html xmlns=\"http://www.w3.org/1999/xhtml\"\n"
         "      xmlns:dc=\"http://purl.org/dc/elements/1.1/\">\n"
         "<head><title>Test</title></head>\n"
         "<body><p>\n"
         "<span about=\"#foo\" rel=\"dc:title\" resource=\"#you\" />\n"
         "</p></body>\n"
         "</html>";

      // Check with low level API
      ByteArrayInputStream bais(rdfa.c_str(), rdfa.length());
      RdfaReader reader;
      reader.setBaseUri("http://example.org/test");
      DynamicObject dyno;
      assertNoException(
         reader.start(dyno));
      assertNoException(
         reader.read(&bais));
      assertNoException(
         reader.finish());

      // Check with readFromString
      DynamicObject dyno2;
      assertNoException(
         RdfaReader::readFromString(
            dyno2, rdfa.c_str(), rdfa.length(), "http://example.org/test"));

      DynamicObject expect;
      expect["@context"]["dc"] = "http://purl.org/dc/elements/1.1/";
      expect["@id"][0]["@id"] = "http://example.org/test#foo";
      expect["@id"][0]["dc:title"]["@id"] = "http://example.org/test#you";
      assertNamedDynoCmp("expect", expect, "result", dyno);
      assertNamedDynoCmp("expect", expect, "result", dyno2);

      MO_DEBUG("%s", JsonWriter::writeToString(expect).c_str());
   }
   tr.passIfNoException();

   tr.test("single embed");
   {
      string rdfa =
         "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
         "<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML+RDFa 1.0//EN\" "
         "\"http://www.w3.org/MarkUp/DTD/xhtml-rdfa-1.dtd\">\n"
         "<html xmlns=\"http://www.w3.org/1999/xhtml\"\n"
         "      xmlns:dc=\"http://purl.org/dc/elements/1.1/\"\n"
         "      xmlns:ex=\"http://example.org/vocab#\">\n"
         "<head><title>Test</title></head>\n"
         "<body><p>\n"
         "<span about=\"#library\" rel=\"ex:contains\" resource=\"#book\" />\n"
         "<span about=\"#book\" property=\"dc:title\">My Book</span>\n"
         "</p></body>\n"
         "</html>";

      DynamicObject frame;
      frame["@context"]["ex"] = "http://example.org/vocab#";
      frame["ex:contains"]->setType(Map);

      ByteArrayInputStream bais(rdfa.c_str(), rdfa.length());
      RdfaReader reader;
      reader.setBaseUri("http://example.org/test");
      reader.setFrame(frame);
      DynamicObject dyno;
      assertNoException(
         reader.start(dyno));
      assertNoException(
         reader.read(&bais));
      assertNoException(
         reader.finish());

      DynamicObject expect;
      expect["@context"]["dc"] = "http://purl.org/dc/elements/1.1/";
      expect["@context"]["ex"] = "http://example.org/vocab#";
      expect["@id"] = "http://example.org/test#library";
      expect["ex:contains"]["@id"] = "http://example.org/test#book";
      expect["ex:contains"]["dc:title"] = "My Book";
      assertNamedDynoCmp("expect", expect, "result", dyno);

      MO_DEBUG("%s", JsonWriter::writeToString(expect).c_str());
   }
   tr.passIfNoException();

   tr.test("double embed");
   {
      string rdfa =
         "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
         "<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML+RDFa 1.0//EN\" "
         "\"http://www.w3.org/MarkUp/DTD/xhtml-rdfa-1.dtd\">\n"
         "<html xmlns=\"http://www.w3.org/1999/xhtml\"\n"
         "      xmlns:dc=\"http://purl.org/dc/elements/1.1/\"\n"
         "      xmlns:ex=\"http://example.org/vocab#\">\n"
         "<head><title>Test</title></head>\n"
         "<body><p>\n"
         "<span about=\"#library\" typeof=\"ex:Library\" "
            "rel=\"ex:contains\" resource=\"#book\" />\n"
         "<span about=\"#book\" property=\"dc:title\">My Book</span>\n"
         "<span about=\"#book\" property=\"dc:contributor\">Writer</span>\n"
         "<span about=\"#book\" rel=\"ex:contains\" resource=\"#chapter\" />\n"
         "<span about=\"#chapter\" property=\"dc:title\">Chapter One</span>\n"
         "<span about=\"#chapter\" property=\"dc:description\">Fun</span>\n"
         "</p></body>\n"
         "</html>";

      DynamicObject frame;
      frame["@context"]["ex"] = "http://example.org/vocab#";
      frame["@type"] = "ex:Library";
      frame["ex:contains"]["ex:contains"]->setType(Map);

      ByteArrayInputStream bais(rdfa.c_str(), rdfa.length());
      RdfaReader reader;
      reader.setBaseUri("http://example.org/test");
      reader.setFrame(frame);
      DynamicObject dyno;
      assertNoException(
         reader.start(dyno));
      assertNoException(
         reader.read(&bais));
      assertNoException(
         reader.finish());

      DynamicObject expect;
      expect["@context"]["dc"] = "http://purl.org/dc/elements/1.1/";
      expect["@context"]["ex"] = "http://example.org/vocab#";
      expect["@id"] = "http://example.org/test#library";
      expect["@type"] = "ex:Library";
      expect["ex:contains"]["@id"] = "http://example.org/test#book";
      expect["ex:contains"]["dc:contributor"] = "Writer";
      expect["ex:contains"]["dc:title"] = "My Book";
      expect["ex:contains"]["ex:contains"]["@id"] =
         "http://example.org/test#chapter";
      expect["ex:contains"]["ex:contains"]["dc:description"] = "Fun";
      expect["ex:contains"]["ex:contains"]["dc:title"] = "Chapter One";
      assertNamedDynoCmp("expect", expect, "result", dyno);

      MO_DEBUG("%s", JsonWriter::writeToString(expect).c_str());
   }
   tr.passIfNoException();

   tr.test("2-subgraphs");
   {
      string rdfa =
         "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
         "<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML+RDFa 1.0//EN\" "
         "\"http://www.w3.org/MarkUp/DTD/xhtml-rdfa-1.dtd\">\n"
         "<html xmlns=\"http://www.w3.org/1999/xhtml\"\n"
         "      xmlns:foaf=\"http://xmlns.org/foaf/0.1/\">\n"
         "<head><title>Test</title></head>\n"
         "<body><p>\n"
         "<span about=\"#john\" property=\"foaf:name\">John</span>\n"
         "<span about=\"#jane\" property=\"foaf:name\">Jane</span>\n"
         "</p></body>\n"
         "</html>";

      ByteArrayInputStream bais(rdfa.c_str(), rdfa.length());
      RdfaReader reader;
      reader.setBaseUri("http://example.org/test");
      DynamicObject dyno;
      assertNoException(
         reader.start(dyno));
      assertNoException(
         reader.read(&bais));
      assertNoException(
         reader.finish());

      DynamicObject expect;
      expect["@context"]["foaf"] = "http://xmlns.org/foaf/0.1/";
      expect["@id"][0]["@id"] = "http://example.org/test#jane";
      expect["@id"][0]["foaf:name"] = "Jane";
      expect["@id"][1]["@id"] = "http://example.org/test#john";
      expect["@id"][1]["foaf:name"] = "John";
      assertNamedDynoCmp("expect", expect, "result", dyno);

      MO_DEBUG("%s", JsonWriter::writeToString(expect).c_str());
   }
   tr.passIfNoException();

   tr.test("multiple values");
   {
      string rdfa =
         "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
         "<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML+RDFa 1.0//EN\" "
         "\"http://www.w3.org/MarkUp/DTD/xhtml-rdfa-1.dtd\">\n"
         "<html xmlns=\"http://www.w3.org/1999/xhtml\"\n"
         "      xmlns:dc=\"http://purl.org/dc/terms/\"\n"
         "      xmlns:ex=\"http://example.org/vocab#\"\n"
         "      xmlns:foaf=\"http://xmlns.org/foaf/0.1/\">\n"
         "<head></head>\n"
         "<body><p>\n"
         "<span about=\"#book\" property=\"ex:prop\">Prop 1</span>\n"
         "<span about=\"#book\" property=\"ex:prop\">Prop 2</span>\n"
         "</p></body>\n"
         "</html>";

      ByteArrayInputStream bais(rdfa.c_str(), rdfa.length());
      RdfaReader reader;
      reader.setBaseUri("http://example.org/test");
      DynamicObject dyno;
      assertNoException(
         reader.start(dyno));
      assertNoException(
         reader.read(&bais));
      assertNoException(
         reader.finish());

      DynamicObject expect;
      expect["@context"]["ex"] = "http://example.org/vocab#";
      expect["@id"][0]["@id"] = "http://example.org/test#book";
      expect["@id"][0]["ex:prop"][0] = "Prop 1";
      expect["@id"][0]["ex:prop"][1] = "Prop 2";
      assertNamedDynoCmp("expect", expect, "result", dyno);

      MO_DEBUG("%s", JsonWriter::writeToString(expect).c_str());
   }
   tr.passIfNoException();

   tr.test("HTML5+RDFa");
   {
      string rdfa =
         "<!DOCTYPE html>\n"
         "<html xmlns:dc=\"http://purl.org/dc/elements/1.1/\">\n"
         "<head><title>Test</title></head>\n"
         "<body><p>\n"
         "<span about=\"#foo\" rel=\"dc:title\" resource=\"#you\" />\n"
         "</p></body>\n"
         "</html>";

      // Check with low level API
      ByteArrayInputStream bais(rdfa.c_str(), rdfa.length());
      RdfaReader reader;
      reader.setBaseUri("http://example.org/test");
      DynamicObject dyno;
      assertNoException(reader.start(dyno));
      assertNoException(reader.read(&bais));
      assertNoException(reader.finish());

      // Check with readFromString
      DynamicObject dyno2;
      assertNoException(
         RdfaReader::readFromString(
            dyno2, rdfa.c_str(), rdfa.length(), "http://example.org/test"));

      DynamicObject expect;
      expect["@context"]["dc"] = "http://purl.org/dc/elements/1.1/";
      expect["@id"][0]["@id"] = "http://example.org/test#foo";
      expect["@id"][0]["dc:title"]["@id"] = "http://example.org/test#you";
      assertNamedDynoCmp("expect", expect, "result", dyno);
      assertNamedDynoCmp("expect", expect, "result", dyno2);

      MO_DEBUG("%s", JsonWriter::writeToString(expect).c_str());
   }
   tr.passIfNoException();

   tr.test("frame w/types");
   {
      string rdfa =
         "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
         "<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML+RDFa 1.0//EN\" "
         "\"http://www.w3.org/MarkUp/DTD/xhtml-rdfa-1.dtd\">\n"
         "<html xmlns=\"http://www.w3.org/1999/xhtml\"\n"
         "      xmlns:dc=\"http://purl.org/dc/elements/1.1/\"\n"
         "      xmlns:ex=\"http://example.org/vocab#\"\n"
         "      xmlns:foaf=\"http://xmlns.org/foaf/0.1/\">\n"
         "<head><title>Test</title></head>\n"
         "<body><p>\n"
         "<span about=\"#library\" typeof=\"ex:Library\" "
            "rel=\"ex:contains\" resource=\"#book\" />\n"
         "<span about=\"#book\" typeof=\"ex:Book\" "
            "property=\"dc:title\">My Book</span>\n"
         "<span about=\"#book\" property=\"dc:contributor\">Writer</span>\n"
         "<span about=\"#book\" rel=\"ex:contains\" resource=\"#chapter\" />\n"
         "<span about=\"#chapter\" typeof=\"ex:Chapter\" "
            "property=\"dc:title\">Chapter One</span>\n"
         "<span about=\"#chapter\" property=\"dc:description\">Fun</span>\n"
         "<span about=\"#john\" typeof=\"ex:Person\" "
            "property=\"foaf:name\">John</span>\n"
         "<span about=\"#jane\" typeof=\"ex:Person\" "
            "property=\"foaf:name\">Jane</span>\n"
         "<span about=\"#jane\" rel=\"ex:authored\" resource=\"#chapter\" />\n"
         "</p></body>\n"
         "</html>";

      DynamicObject frame;
      frame["@context"]["ex"] = "http://example.org/vocab#";
      frame["@type"] = "ex:Library";
      frame["ex:contains"]["@type"] = "ex:Book";
      frame["ex:contains"]["ex:contains"]["@type"] = "ex:Chapter";

      ByteArrayInputStream bais(rdfa.c_str(), rdfa.length());
      RdfaReader reader;
      reader.setBaseUri("http://example.org/test");
      reader.setFrame(frame);
      DynamicObject dyno;
      assertNoException(
         reader.start(dyno));
      assertNoException(
         reader.read(&bais));
      assertNoException(
         reader.finish());

      DynamicObject expect;
      expect["@context"]["dc"] = "http://purl.org/dc/elements/1.1/";
      expect["@context"]["ex"] = "http://example.org/vocab#";
      expect["@id"] = "http://example.org/test#library";
      expect["@type"] = "ex:Library";
      expect["ex:contains"]["@id"] = "http://example.org/test#book";
      expect["ex:contains"]["@type"] = "ex:Book";
      expect["ex:contains"]["dc:contributor"] = "Writer";
      expect["ex:contains"]["dc:title"] = "My Book";
      expect["ex:contains"]["ex:contains"]["@id"] =
         "http://example.org/test#chapter";
      expect["ex:contains"]["ex:contains"]["@type"] = "ex:Chapter";
      expect["ex:contains"]["ex:contains"]["dc:description"] = "Fun";
      expect["ex:contains"]["ex:contains"]["dc:title"] = "Chapter One";
      assertNamedDynoCmp("expect", expect, "result", dyno);

      MO_DEBUG("%s", JsonWriter::writeToString(expect).c_str());
   }
   tr.passIfNoException();

   tr.test("frame w/types +extra deep-data");
   {
      string rdfa =
         "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
         "<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML+RDFa 1.0//EN\" "
         "\"http://www.w3.org/MarkUp/DTD/xhtml-rdfa-1.dtd\">\n"
         "<html xmlns=\"http://www.w3.org/1999/xhtml\"\n"
         "      xmlns:dc=\"http://purl.org/dc/elements/1.1/\"\n"
         "      xmlns:ex=\"http://example.org/vocab#\"\n"
         "      xmlns:foaf=\"http://xmlns.org/foaf/0.1/\">\n"
         "<head><title>Test</title></head>\n"
         "<body><p>\n"
         "<span about=\"#library\" typeof=\"ex:Library\" "
            "rel=\"ex:contains\" resource=\"#book\" />\n"
         "<span about=\"#book\" typeof=\"ex:Book\" "
            "property=\"dc:title\">My Book</span>\n"
         "<span about=\"#book\" property=\"dc:contributor\">Writer</span>\n"
         "<span about=\"#book\" rel=\"ex:authoredBy\" resource=\"#jane\" />\n"
         "<span about=\"#jane\" typeof=\"ex:Person\" "
            "property=\"foaf:name\">Jane</span>\n"
         "<span about=\"#book\" rel=\"ex:contains\" resource=\"#chapter\" />\n"
         "<span about=\"#chapter\" typeof=\"ex:Chapter\" "
            "property=\"dc:title\">Chapter One</span>\n"
         "<span about=\"#chapter\" property=\"dc:description\">Fun</span>\n"
         "<span about=\"#john\" typeof=\"ex:Person\" "
            "property=\"foaf:name\">John</span>\n"
         "<span about=\"#jane\" rel=\"ex:authored\" resource=\"#chapter\" />\n"
         "</p></body>\n"
         "</html>";

      DynamicObject frame;
      frame["@context"]["ex"] = "http://example.org/vocab#";
      frame["@type"] = "ex:Library";
      frame["ex:contains"]["@type"] = "ex:Book";
      frame["ex:contains"]["ex:authoredBy"]->setType(Map);
      frame["ex:contains"]["ex:contains"]["@type"] = "ex:Chapter";

      ByteArrayInputStream bais(rdfa.c_str(), rdfa.length());
      RdfaReader reader;
      reader.setBaseUri("http://example.org/test");
      reader.setFrame(frame);
      DynamicObject dyno;
      assertNoException(
         reader.start(dyno));
      assertNoException(
         reader.read(&bais));
      assertNoException(
         reader.finish());

      DynamicObject expect;
      expect["@context"]["dc"] = "http://purl.org/dc/elements/1.1/";
      expect["@context"]["ex"] = "http://example.org/vocab#";
      expect["@context"]["foaf"] = "http://xmlns.org/foaf/0.1/";
      expect["@id"] = "http://example.org/test#library";
      expect["@type"] = "ex:Library";
      expect["ex:contains"]["@id"] = "http://example.org/test#book";
      expect["ex:contains"]["@type"] = "ex:Book";
      expect["ex:contains"]["ex:authoredBy"]["@id"] =
         "http://example.org/test#jane";
      expect["ex:contains"]["ex:authoredBy"]["@type"] = "ex:Person";
      expect["ex:contains"]["ex:authoredBy"]["ex:authored"]["@id"] =
         "http://example.org/test#chapter";
      expect["ex:contains"]["ex:authoredBy"]["foaf:name"] = "Jane";
      expect["ex:contains"]["dc:contributor"] = "Writer";
      expect["ex:contains"]["dc:title"] = "My Book";
      expect["ex:contains"]["ex:contains"]["@id"] =
         "http://example.org/test#chapter";
      expect["ex:contains"]["ex:contains"]["@type"] = "ex:Chapter";
      expect["ex:contains"]["ex:contains"]["dc:description"] = "Fun";
      expect["ex:contains"]["ex:contains"]["dc:title"] = "Chapter One";
      assertNamedDynoCmp("expect", expect, "result", dyno);

      MO_DEBUG("%s", JsonWriter::writeToString(expect).c_str());
   }
   tr.passIfNoException();

   tr.test("frame w/types +extra shallow-data");
   {
      string rdfa =
         "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
         "<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML+RDFa 1.0//EN\" "
         "\"http://www.w3.org/MarkUp/DTD/xhtml-rdfa-1.dtd\">\n"
         "<html xmlns=\"http://www.w3.org/1999/xhtml\"\n"
         "      xmlns:dc=\"http://purl.org/dc/elements/1.1/\"\n"
         "      xmlns:ex=\"http://example.org/vocab#\"\n"
         "      xmlns:foaf=\"http://xmlns.org/foaf/0.1/\">\n"
         "<head><title>Test</title></head>\n"
         "<body><p>\n"
         "<span about=\"#library\" typeof=\"ex:Library\" "
            "rel=\"ex:contains\" resource=\"#book\" />\n"
         "<span about=\"#book\" typeof=\"ex:Book\" "
            "property=\"dc:title\">My Book</span>\n"
         "<span about=\"#book\" property=\"dc:contributor\">Writer</span>\n"
         "<span about=\"#book\" rel=\"ex:authoredBy\" resource=\"#jane\" />\n"
         "<span about=\"#jane\" typeof=\"ex:Person\" "
            "property=\"foaf:name\">Jane</span>\n"
         "<span about=\"#book\" rel=\"ex:contains\" resource=\"#chapter\" />\n"
         "<span about=\"#chapter\" typeof=\"ex:Chapter\" "
            "property=\"dc:title\">Chapter One</span>\n"
         "<span about=\"#chapter\" property=\"dc:description\">Fun</span>\n"
         "<span about=\"#john\" typeof=\"ex:Person\" "
            "property=\"foaf:name\">John</span>\n"
         "<span about=\"#jane\" rel=\"ex:authored\" resource=\"#chapter\" />\n"
         "</p></body>\n"
         "</html>";

      DynamicObject frame;
      frame["@context"]["ex"] = "http://example.org/vocab#";
      frame["@type"] = "ex:Library";
      frame["ex:contains"]["@type"] = "ex:Book";
      frame["ex:contains"]["ex:authoredBy"]["@embed"] = false;
      frame["ex:contains"]["ex:contains"]["@type"] = "ex:Chapter";

      ByteArrayInputStream bais(rdfa.c_str(), rdfa.length());
      RdfaReader reader;
      reader.setBaseUri("http://example.org/test");
      reader.setFrame(frame);
      DynamicObject dyno;
      assertNoException(
         reader.start(dyno));
      assertNoException(
         reader.read(&bais));
      assertNoException(
         reader.finish());

      DynamicObject expect;
      expect["@context"]["dc"] = "http://purl.org/dc/elements/1.1/";
      expect["@context"]["ex"] = "http://example.org/vocab#";
      expect["@id"] = "http://example.org/test#library";
      expect["@type"] = "ex:Library";
      expect["ex:contains"]["@id"] = "http://example.org/test#book";
      expect["ex:contains"]["@type"] = "ex:Book";
      expect["ex:contains"]["ex:authoredBy"]["@id"] =
         "http://example.org/test#jane";
      expect["ex:contains"]["dc:contributor"] = "Writer";
      expect["ex:contains"]["dc:title"] = "My Book";
      expect["ex:contains"]["ex:contains"]["@id"] =
         "http://example.org/test#chapter";
      expect["ex:contains"]["ex:contains"]["@type"] = "ex:Chapter";
      expect["ex:contains"]["ex:contains"]["dc:description"] = "Fun";
      expect["ex:contains"]["ex:contains"]["dc:title"] = "Chapter One";
      assertNamedDynoCmp("expect", expect, "result", dyno);

      MO_DEBUG("%s", JsonWriter::writeToString(expect).c_str());
   }
   tr.passIfNoException();

   tr.test("frame w/types +extra shallow-data in array");
   {
      string rdfa =
         "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
         "<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML+RDFa 1.0//EN\" "
         "\"http://www.w3.org/MarkUp/DTD/xhtml-rdfa-1.dtd\">\n"
         "<html xmlns=\"http://www.w3.org/1999/xhtml\"\n"
         "      xmlns:dc=\"http://purl.org/dc/elements/1.1/\"\n"
         "      xmlns:ex=\"http://example.org/vocab#\"\n"
         "      xmlns:foaf=\"http://xmlns.org/foaf/0.1/\">\n"
         "<head><title>Test</title></head>\n"
         "<body><p>\n"
         "<span about=\"#library\" typeof=\"ex:Library\" "
            "rel=\"ex:contains\" resource=\"#book\" />\n"
         "<span about=\"#book\" typeof=\"ex:Book\" "
            "property=\"dc:title\">My Book</span>\n"
         "<span about=\"#book\" property=\"dc:contributor\">Writer</span>\n"
         "<span about=\"#book\" rel=\"ex:authoredBy\" resource=\"#jane\" />\n"
         "<span about=\"#jane\" typeof=\"ex:Person\" "
            "property=\"foaf:name\">Jane</span>\n"
         "<span about=\"#book\" rel=\"ex:contains\" resource=\"#chapter\" />\n"
         "<span about=\"#chapter\" typeof=\"ex:Chapter\" "
            "property=\"dc:title\">Chapter One</span>\n"
         "<span about=\"#chapter\" property=\"dc:description\">Fun</span>\n"
         "<span about=\"#john\" typeof=\"ex:Person\" "
            "property=\"foaf:name\">John</span>\n"
         "<span about=\"#jane\" rel=\"ex:authored\" resource=\"#chapter\" />\n"
         "</p></body>\n"
         "</html>";

      DynamicObject frame;
      frame["@context"]["ex"] = "http://example.org/vocab#";
      frame["@type"] = "ex:Library";
      frame["ex:contains"]["@type"] = "ex:Book";
      frame["ex:contains"]["ex:authoredBy"][0]["@embed"] = false;
      frame["ex:contains"]["ex:contains"]["@type"] = "ex:Chapter";

      ByteArrayInputStream bais(rdfa.c_str(), rdfa.length());
      RdfaReader reader;
      reader.setBaseUri("http://example.org/test");
      reader.setFrame(frame);
      DynamicObject dyno;
      assertNoException(
         reader.start(dyno));
      assertNoException(
         reader.read(&bais));
      assertNoException(
         reader.finish());

      DynamicObject expect;
      expect["@context"]["dc"] = "http://purl.org/dc/elements/1.1/";
      expect["@context"]["ex"] = "http://example.org/vocab#";
      expect["@id"] = "http://example.org/test#library";
      expect["@type"] = "ex:Library";
      expect["ex:contains"]["@id"] = "http://example.org/test#book";
      expect["ex:contains"]["@type"] = "ex:Book";
      expect["ex:contains"]["ex:authoredBy"][0]["@id"] =
         "http://example.org/test#jane";
      expect["ex:contains"]["dc:contributor"] = "Writer";
      expect["ex:contains"]["dc:title"] = "My Book";
      expect["ex:contains"]["ex:contains"]["@id"] =
         "http://example.org/test#chapter";
      expect["ex:contains"]["ex:contains"]["@type"] = "ex:Chapter";
      expect["ex:contains"]["ex:contains"]["dc:description"] = "Fun";
      expect["ex:contains"]["ex:contains"]["dc:title"] = "Chapter One";
      assertNamedDynoCmp("expect", expect, "result", dyno);

      MO_DEBUG("%s", JsonWriter::writeToString(expect).c_str());
   }
   tr.passIfNoException();

   tr.test("frame w/types +explicit");
   {
      string rdfa =
         "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
         "<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML+RDFa 1.0//EN\" "
         "\"http://www.w3.org/MarkUp/DTD/xhtml-rdfa-1.dtd\">\n"
         "<html xmlns=\"http://www.w3.org/1999/xhtml\"\n"
         "      xmlns:dc=\"http://purl.org/dc/elements/1.1/\"\n"
         "      xmlns:ex=\"http://example.org/vocab#\"\n"
         "      xmlns:foaf=\"http://xmlns.org/foaf/0.1/\">\n"
         "<head><title>Test</title></head>\n"
         "<body><p>\n"
         "<span about=\"#library\" typeof=\"ex:Library\" "
            "rel=\"ex:contains\" resource=\"#book\" />\n"
         "<span about=\"#book\" typeof=\"ex:Book\" "
            "property=\"dc:title\">My Book</span>\n"
         "<span about=\"#book\" property=\"dc:contributor\">Writer</span>\n"
         "<span about=\"#book\" rel=\"ex:contains\" resource=\"#chapter\" />\n"
         "<span about=\"#chapter\" typeof=\"ex:Chapter\" "
            "property=\"dc:title\">Chapter One</span>\n"
         "<span about=\"#chapter\" property=\"dc:description\">Fun</span>\n"
         "<span about=\"#john\" typeof=\"ex:Person\" "
            "property=\"foaf:name\">John</span>\n"
         "<span about=\"#jane\" typeof=\"ex:Person\" "
            "property=\"foaf:name\">Jane</span>\n"
         "<span about=\"#jane\" rel=\"ex:authored\" resource=\"#chapter\" />\n"
         "</p></body>\n"
         "</html>";

      DynamicObject frame;
      frame["@context"]["ex"] = "http://example.org/vocab#";
      frame["@context"]["dc"] = "http://purl.org/dc/elements/1.1/";
      frame["@explicit"] = true;
      frame["@type"] = "ex:Library";
      frame["ex:contains"]["@explicit"] = true;
      frame["ex:contains"]["@type"] = "ex:Book";
      frame["ex:contains"]["dc:contributor"]->setType(Map);
      frame["ex:contains"]["ex:contains"]["@explicit"] = true;
      frame["ex:contains"]["ex:contains"]["@type"] = "ex:Chapter";
      frame["ex:contains"]["ex:contains"]["dc:title"]->setType(Map);

      ByteArrayInputStream bais(rdfa.c_str(), rdfa.length());
      RdfaReader reader;
      reader.setBaseUri("http://example.org/test");
      reader.setFrame(frame);
      DynamicObject dyno;
      assertNoException(
         reader.start(dyno));
      assertNoException(
         reader.read(&bais));
      assertNoException(
         reader.finish());

      DynamicObject expect;
      expect["@context"]["dc"] = "http://purl.org/dc/elements/1.1/";
      expect["@context"]["ex"] = "http://example.org/vocab#";
      expect["@id"] = "http://example.org/test#library";
      expect["@type"] = "ex:Library";
      expect["ex:contains"]["@id"] = "http://example.org/test#book";
      expect["ex:contains"]["@type"] = "ex:Book";
      expect["ex:contains"]["dc:contributor"] = "Writer";
      expect["ex:contains"]["ex:contains"]["@id"] =
         "http://example.org/test#chapter";
      expect["ex:contains"]["ex:contains"]["@type"] = "ex:Chapter";
      expect["ex:contains"]["ex:contains"]["dc:title"] = "Chapter One";
      assertNamedDynoCmp("expect", expect, "result", dyno);

      MO_DEBUG("%s", JsonWriter::writeToString(expect).c_str());
   }
   tr.passIfNoException();

   tr.test("frame w/types +explicit +useArrays");
   {
      string rdfa =
         "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
         "<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML+RDFa 1.0//EN\" "
         "\"http://www.w3.org/MarkUp/DTD/xhtml-rdfa-1.dtd\">\n"
         "<html xmlns=\"http://www.w3.org/1999/xhtml\"\n"
         "      xmlns:dc=\"http://purl.org/dc/elements/1.1/\"\n"
         "      xmlns:ex=\"http://example.org/vocab#\"\n"
         "      xmlns:foaf=\"http://xmlns.org/foaf/0.1/\">\n"
         "<head><title>Test</title></head>\n"
         "<body><p>\n"
         "<span about=\"#library\" typeof=\"ex:Library\" "
            "rel=\"ex:contains\" resource=\"#book\" />\n"
         "<span about=\"#book\" typeof=\"ex:Book\" "
            "property=\"dc:title\">My Book</span>\n"
         "<span about=\"#book\" property=\"dc:contributor\">Writer</span>\n"
         "<span about=\"#book\" rel=\"ex:contains\" resource=\"#chapter\" />\n"
         "<span about=\"#chapter\" typeof=\"ex:Chapter\" "
            "property=\"dc:title\">Chapter One</span>\n"
         "<span about=\"#chapter\" property=\"dc:description\">Fun</span>\n"
         "<span about=\"#john\" typeof=\"ex:Person\" "
            "property=\"foaf:name\">John</span>\n"
         "<span about=\"#jane\" typeof=\"ex:Person\" "
            "property=\"foaf:name\">Jane</span>\n"
         "<span about=\"#jane\" rel=\"ex:authored\" resource=\"#chapter\" />\n"
         "</p></body>\n"
         "</html>";

      DynamicObject frame;
      frame["@context"]["ex"] = "http://example.org/vocab#";
      frame["@context"]["dc"] = "http://purl.org/dc/elements/1.1/";
      frame["@explicit"] = true;
      frame["@type"] = "ex:Library";
      frame["ex:contains"]["@explicit"] = true;
      frame["ex:contains"]["@type"] = "ex:Book";
      frame["ex:contains"]["dc:contributor"]->setType(Array);
      frame["ex:contains"]["ex:contains"]["@explicit"] = true;
      frame["ex:contains"]["ex:contains"]["@type"] = "ex:Chapter";
      frame["ex:contains"]["ex:contains"]["dc:title"]->setType(Map);

      ByteArrayInputStream bais(rdfa.c_str(), rdfa.length());
      RdfaReader reader;
      reader.setBaseUri("http://example.org/test");
      DynamicObject options;
      options["defaults"]["explicitOn"] = true;
      reader.setFrame(frame, &options);
      DynamicObject dyno;
      assertNoException(
         reader.start(dyno));
      assertNoException(
         reader.read(&bais));
      assertNoException(
         reader.finish());

      DynamicObject expect;
      expect["@context"]["dc"] = "http://purl.org/dc/elements/1.1/";
      expect["@context"]["ex"] = "http://example.org/vocab#";
      expect["@id"] = "http://example.org/test#library";
      expect["@type"] = "ex:Library";
      expect["ex:contains"]["@id"] = "http://example.org/test#book";
      expect["ex:contains"]["@type"] = "ex:Book";
      expect["ex:contains"]["dc:contributor"][0] = "Writer";
      expect["ex:contains"]["ex:contains"]["@id"] =
         "http://example.org/test#chapter";
      expect["ex:contains"]["ex:contains"]["@type"] = "ex:Chapter";
      expect["ex:contains"]["ex:contains"]["dc:title"] = "Chapter One";
      assertNamedDynoCmp("expect", expect, "result", dyno);

      MO_DEBUG("%s", JsonWriter::writeToString(expect).c_str());
   }
   tr.passIfNoException();

   tr.test("frame w/types same as triple order");
   {
      string rdfa =
         "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
         "<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML+RDFa 1.0//EN\" "
         "\"http://www.w3.org/MarkUp/DTD/xhtml-rdfa-1.dtd\">\n"
         "<html xmlns=\"http://www.w3.org/1999/xhtml\"\n"
         "      xmlns:dc=\"http://purl.org/dc/elements/1.1/\"\n"
         "      xmlns:ex=\"http://example.org/vocab#\"\n"
         "      xmlns:foaf=\"http://xmlns.org/foaf/0.1/\">\n"
         "<head><title>Test</title></head>\n"
         "<body><p>\n"
         "<span about=\"#a\" typeof=\"ex:A\" "
            "rel=\"ex:knows\" resource=\"#b\" />\n"
         "<span about=\"#a\" rel=\"ex:knows\" resource=\"#aa\" />\n"
         "<span about=\"#b\" typeof=\"ex:B\" "
            "rel=\"ex:contains\" resource=\"#a\" />\n"
         "<span about=\"#b\" rel=\"ex:contains\" resource=\"#aa\" />\n"
         "<span about=\"#aa\" typeof=\"ex:A\" "
            "property=\"dc:title\">Embedded</span>\n"
         "<span about=\"#john\" typeof=\"ex:Person\" "
            "property=\"foaf:name\">John</span>\n"
         "<span about=\"#jane\" typeof=\"ex:Person\" "
            "property=\"foaf:name\">Jane</span>\n"
         "<span about=\"#jane\" rel=\"ex:authored\" resource=\"#chapter\" />\n"
         "</p></body>\n"
         "</html>";

      DynamicObject frame;
      frame["@context"]["ex"] = "http://example.org/vocab#";
      frame["@context"]["dc"] = "http://purl.org/dc/elements/1.1/";
      frame["@type"] = "ex:A";
      frame["ex:knows"]->setType(Array);

      ByteArrayInputStream bais(rdfa.c_str(), rdfa.length());
      RdfaReader reader;
      reader.setBaseUri("http://example.org/test");
      reader.setFrame(frame);
      DynamicObject dyno;
      assertNoException(
         reader.start(dyno));
      assertNoException(
         reader.read(&bais));
      assertNoException(
         reader.finish());

      DynamicObject expect;
      expect["@context"]["dc"] = "http://purl.org/dc/elements/1.1/";
      expect["@context"]["ex"] = "http://example.org/vocab#";
      expect["@id"] = "http://example.org/test#a";
      expect["@type"] = "ex:A";
      expect["ex:knows"][0]["@id"] = "http://example.org/test#aa";
      expect["ex:knows"][0]["@type"] = "ex:A";
      expect["ex:knows"][0]["dc:title"] = "Embedded";
      expect["ex:knows"][1]["@id"] = "http://example.org/test#b";
      expect["ex:knows"][1]["@type"] = "ex:B";
      expect["ex:knows"][1]["ex:contains"][0]["@id"] =
         "http://example.org/test#a";
      expect["ex:knows"][1]["ex:contains"][1]["@id"] =
         "http://example.org/test#aa";
      assertNamedDynoCmp("expect", expect, "result", dyno);

      MO_DEBUG("%s", JsonWriter::writeToString(expect).c_str());
   }
   tr.passIfNoException();

   tr.test("frame w/types same as triple order +1 type");
   {
      string rdfa =
         "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
         "<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML+RDFa 1.0//EN\" "
         "\"http://www.w3.org/MarkUp/DTD/xhtml-rdfa-1.dtd\">\n"
         "<html xmlns=\"http://www.w3.org/1999/xhtml\"\n"
         "      xmlns:dc=\"http://purl.org/dc/elements/1.1/\"\n"
         "      xmlns:ex=\"http://example.org/vocab#\"\n"
         "      xmlns:foaf=\"http://xmlns.org/foaf/0.1/\">\n"
         "<head><title>Test</title></head>\n"
         "<body><p>\n"
         "<span about=\"#a\" typeof=\"ex:A\" "
            "rel=\"ex:knows\" resource=\"#b\" />\n"
         "<span about=\"#a\" rel=\"ex:knows\" resource=\"#aa\" />\n"
         "<span about=\"#b\" typeof=\"ex:B\" "
            "rel=\"ex:contains\" resource=\"#a\" />\n"
         "<span about=\"#b\" rel=\"ex:contains\" resource=\"#aa\" />\n"
         "<span about=\"#aa\" typeof=\"ex:A\" "
            "property=\"dc:title\">Embedded</span>\n"
         "<span about=\"#john\" typeof=\"ex:Person\" "
            "property=\"foaf:name\">John</span>\n"
         "<span about=\"#jane\" typeof=\"ex:Person\" "
            "property=\"foaf:name\">Jane</span>\n"
         "<span about=\"#jane\" rel=\"ex:authored\" resource=\"#chapter\" />\n"
         "</p></body>\n"
         "</html>";

      DynamicObject frame;
      frame["@context"]["ex"] = "http://example.org/vocab#";
      frame["@type"] = "ex:A";
      frame["ex:knows"][0]["@type"] = "ex:A";

      ByteArrayInputStream bais(rdfa.c_str(), rdfa.length());
      RdfaReader reader;
      reader.setBaseUri("http://example.org/test");
      DynamicObject options;
      options["defaults"]["explicitOn"] = true;
      reader.setFrame(frame, &options);
      DynamicObject dyno;
      assertNoException(
         reader.start(dyno));
      assertNoException(
         reader.read(&bais));
      assertNoException(
         reader.finish());

      DynamicObject expect;
      expect["@context"]["ex"] = "http://example.org/vocab#";
      expect["@id"] = "http://example.org/test#a";
      expect["@type"] = "ex:A";
      expect["ex:knows"][0]["@id"] = "http://example.org/test#aa";
      expect["ex:knows"][0]["@type"] = "ex:A";
      assertNamedDynoCmp("expect", expect, "result", dyno);

      MO_DEBUG("%s", JsonWriter::writeToString(expect).c_str());
   }
   tr.passIfNoException();

   tr.test("array frame w/types same as triple order +1 type");
   {
      string rdfa =
         "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
         "<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML+RDFa 1.0//EN\" "
         "\"http://www.w3.org/MarkUp/DTD/xhtml-rdfa-1.dtd\">\n"
         "<html xmlns=\"http://www.w3.org/1999/xhtml\"\n"
         "      xmlns:dc=\"http://purl.org/dc/elements/1.1/\"\n"
         "      xmlns:ex=\"http://example.org/vocab#\"\n"
         "      xmlns:foaf=\"http://xmlns.org/foaf/0.1/\">\n"
         "<head><title>Test</title></head>\n"
         "<body><p>\n"
         "<span about=\"#a\" typeof=\"ex:A\" "
            "rel=\"ex:knows\" resource=\"#b\" />\n"
         "<span about=\"#a\" rel=\"ex:knows\" resource=\"#aa\" />\n"
         "<span about=\"#b\" typeof=\"ex:B\" "
            "rel=\"ex:contains\" resource=\"#a\" />\n"
         "<span about=\"#b\" rel=\"ex:contains\" resource=\"#aa\" />\n"
         "<span about=\"#aa\" typeof=\"ex:A\" "
            "property=\"dc:title\">Embedded</span>\n"
         "<span about=\"#john\" typeof=\"ex:Person\" "
            "property=\"foaf:name\">John</span>\n"
         "<span about=\"#jane\" typeof=\"ex:Person\" "
            "property=\"foaf:name\">Jane</span>\n"
         "<span about=\"#jane\" rel=\"ex:authored\" resource=\"#chapter\" />\n"
         "</p></body>\n"
         "</html>";

      DynamicObject frame;
      frame[0]["@context"]["ex"] = "http://example.org/vocab#";
      frame[0]["@type"] = "ex:A";
      frame[0]["ex:knows"][0]["@type"] = "ex:A";
      frame[0]["ex:knows"][0]["@embed"] = false;

      ByteArrayInputStream bais(rdfa.c_str(), rdfa.length());
      RdfaReader reader;
      reader.setBaseUri("http://example.org/test");
      DynamicObject options;
      options["defaults"]["explicitOn"] = true;
      reader.setFrame(frame, &options);
      DynamicObject dyno;
      assertNoException(
         reader.start(dyno));
      assertNoException(
         reader.read(&bais));
      assertNoException(
         reader.finish());

      DynamicObject expect;
      expect["@context"]["ex"] = "http://example.org/vocab#";
      expect["@id"][0]["@id"] = "http://example.org/test#a";
      expect["@id"][0]["@type"] = "ex:A";
      expect["@id"][0]["ex:knows"][0]["@id"] = "http://example.org/test#aa";
      expect["@id"][1]["@id"] = "http://example.org/test#aa";
      expect["@id"][1]["@type"] = "ex:A";
      expect["@id"][1]["ex:knows"]->setType(Array);
      assertNamedDynoCmp("expect", expect, "result", dyno);

      MO_DEBUG("%s", JsonWriter::writeToString(expect).c_str());
   }
   tr.passIfNoException();

   tr.test("frame w/types same as triple order +2 types");
   {
      string rdfa =
         "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
         "<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML+RDFa 1.0//EN\" "
         "\"http://www.w3.org/MarkUp/DTD/xhtml-rdfa-1.dtd\">\n"
         "<html xmlns=\"http://www.w3.org/1999/xhtml\"\n"
         "      xmlns:dc=\"http://purl.org/dc/elements/1.1/\"\n"
         "      xmlns:ex=\"http://example.org/vocab#\"\n"
         "      xmlns:foaf=\"http://xmlns.org/foaf/0.1/\">\n"
         "<head><title>Test</title></head>\n"
         "<body><p>\n"
         "<span about=\"#a\" typeof=\"ex:A\" "
            "rel=\"ex:knows\" resource=\"#b\" />\n"
         "<span about=\"#a\" rel=\"ex:knows\" resource=\"#aa\" />\n"
         "<span about=\"#b\" typeof=\"ex:B\" "
            "rel=\"ex:contains\" resource=\"#a\" />\n"
         "<span about=\"#b\" rel=\"ex:contains\" resource=\"#aa\" />\n"
         "<span about=\"#aa\" typeof=\"ex:A\" "
            "property=\"dc:title\">Embedded</span>\n"
         "<span about=\"#john\" typeof=\"ex:Person\" "
            "property=\"foaf:name\">John</span>\n"
         "<span about=\"#jane\" typeof=\"ex:Person\" "
            "property=\"foaf:name\">Jane</span>\n"
         "<span about=\"#jane\" rel=\"ex:authored\" resource=\"#chapter\" />\n"
         "</p></body>\n"
         "</html>";

      DynamicObject frame;
      frame["@context"]["ex"] = "http://example.org/vocab#";
      frame["@type"] = "ex:A";
      frame["ex:knows"][0]["@type"][0] = "ex:A";
      frame["ex:knows"][0]["@type"][1] = "ex:B";

      ByteArrayInputStream bais(rdfa.c_str(), rdfa.length());
      RdfaReader reader;
      reader.setBaseUri("http://example.org/test");
      reader.setFrame(frame);
      DynamicObject dyno;
      assertNoException(
         reader.start(dyno));
      assertNoException(
         reader.read(&bais));
      assertNoException(
         reader.finish());

      DynamicObject expect;
      expect["@context"]["dc"] = "http://purl.org/dc/elements/1.1/";
      expect["@context"]["ex"] = "http://example.org/vocab#";
      expect["@id"] = "http://example.org/test#a";
      expect["@type"] = "ex:A";
      expect["ex:knows"][0]["@id"] = "http://example.org/test#aa";
      expect["ex:knows"][0]["@type"] = "ex:A";
      expect["ex:knows"][0]["dc:title"] = "Embedded";
      expect["ex:knows"][1]["@id"] = "http://example.org/test#b";
      expect["ex:knows"][1]["@type"] = "ex:B";
      expect["ex:knows"][1]["ex:contains"][0]["@id"] =
         "http://example.org/test#a";
      expect["ex:knows"][1]["ex:contains"][1]["@id"] =
         "http://example.org/test#aa";
      assertNamedDynoCmp("expect", expect, "result", dyno);

      MO_DEBUG("%s", JsonWriter::writeToString(expect).c_str());
   }
   tr.passIfNoException();

   tr.test("frame w/types reversed from triple order");
   {
      string rdfa =
         "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
         "<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML+RDFa 1.0//EN\" "
         "\"http://www.w3.org/MarkUp/DTD/xhtml-rdfa-1.dtd\">\n"
         "<html xmlns=\"http://www.w3.org/1999/xhtml\"\n"
         "      xmlns:dc=\"http://purl.org/dc/elements/1.1/\"\n"
         "      xmlns:ex=\"http://example.org/vocab#\"\n"
         "      xmlns:foaf=\"http://xmlns.org/foaf/0.1/\">\n"
         "<head><title>Test</title></head>\n"
         "<body><p>\n"
         "<span about=\"#a\" typeof=\"ex:A\" "
            "rel=\"ex:knows\" resource=\"#b\" />\n"
         "<span about=\"#a\" rel=\"ex:knows\" resource=\"#aa\" />\n"
         "<span about=\"#b\" typeof=\"ex:B\" "
            "rel=\"ex:contains\" resource=\"#a\" />\n"
         "<span about=\"#b\" rel=\"ex:contains\" resource=\"#aa\" />\n"
         "<span about=\"#aa\" typeof=\"ex:A\" "
            "property=\"dc:title\">Embedded</span>\n"
         "<span about=\"#john\" typeof=\"ex:Person\" "
            "property=\"foaf:name\">John</span>\n"
         "<span about=\"#jane\" typeof=\"ex:Person\" "
            "property=\"foaf:name\">Jane</span>\n"
         "<span about=\"#jane\" rel=\"ex:authored\" resource=\"#chapter\" />\n"
         "</p></body>\n"
         "</html>";

      DynamicObject frame;
      frame["@context"]["ex"] = "http://example.org/vocab#";
      frame["@type"] = "ex:B";
      frame["ex:contains"][0]["@type"] = "ex:A";

      ByteArrayInputStream bais(rdfa.c_str(), rdfa.length());
      RdfaReader reader;
      reader.setBaseUri("http://example.org/test");
      reader.setFrame(frame);
      DynamicObject dyno;
      assertNoException(
         reader.start(dyno));
      assertNoException(
         reader.read(&bais));
      assertNoException(
         reader.finish());

      DynamicObject expect;
      expect["@context"]["dc"] = "http://purl.org/dc/elements/1.1/";
      expect["@context"]["ex"] = "http://example.org/vocab#";
      expect["@id"] = "http://example.org/test#b";
      expect["@type"] = "ex:B";
      expect["ex:contains"][0]["@id"] = "http://example.org/test#a";
      expect["ex:contains"][0]["@type"] = "ex:A";
      expect["ex:contains"][0]["ex:knows"][0]["@id"] =
         "http://example.org/test#aa";
      expect["ex:contains"][0]["ex:knows"][1]["@id"] =
         "http://example.org/test#b";
      expect["ex:contains"][1]["@id"] = "http://example.org/test#aa";
      expect["ex:contains"][1]["@type"] = "ex:A";
      expect["ex:contains"][1]["dc:title"] = "Embedded";
      assertNamedDynoCmp("expect", expect, "result", dyno);

      MO_DEBUG("%s", JsonWriter::writeToString(expect).c_str());
   }
   tr.passIfNoException();

   tr.test("datatypes");
   {
      string rdfa =
         "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
         "<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML+RDFa 1.0//EN\" "
         "\"http://www.w3.org/MarkUp/DTD/xhtml-rdfa-1.dtd\">\n"
         "<html xmlns=\"http://www.w3.org/1999/xhtml\"\n"
         "      xmlns:xsd=\"http://www.w3.org/2001/XMLSchema#\"\n"
         "      xmlns:ex=\"http://example.org/vocab#\">\n"
         "<head><title>Test</title></head>\n"
         "<body><p>\n"
         "<span about=\"#a\""
         "      property=\"ex:prop\" datatype=\"xsd:anyURI\">ex:link</span>\n"
         "</p></body>\n"
         "</html>";

      ByteArrayInputStream bais(rdfa.c_str(), rdfa.length());
      RdfaReader reader;
      reader.setBaseUri("http://example.org/test");
      DynamicObject dyno;
      assertNoException(
         reader.start(dyno));
      assertNoException(
         reader.read(&bais));
      assertNoException(
         reader.finish());

      DynamicObject expect;
      expect["@context"]["ex"] = "http://example.org/vocab#";
      expect["@id"][0]["@id"] = "http://example.org/test#a";
      expect["@id"][0]["ex:prop"]["@value"] = "ex:link";
      expect["@id"][0]["ex:prop"]["@type"] =
         "http://www.w3.org/2001/XMLSchema#anyURI";
      assertNamedDynoCmp("expect", expect, "result", dyno);

      MO_DEBUG("%s", JsonWriter::writeToString(expect).c_str());
   }
   tr.passIfNoException();

   tr.ungroup();
}

static bool run(TestRunner& tr)
{
   if(tr.isDefaultEnabled() || tr.isTestEnabled("rdfa"))
   {
      runRdfaReaderTest(tr);
   }
   /*if(tr.isTestEnabled("rdfa-reader-speed"))
   {
      runRdfaReaderSpeedTest(tr);
   }*/
   return true;
}

} // end namespace

MO_TEST_MODULE_FN("monarch.tests.data.rdfa.test", "1.0", mo_test_data_rdfa::run)
