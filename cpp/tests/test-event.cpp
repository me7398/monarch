/*
 * Copyright (c) 2007-2010 Digital Bazaar, Inc. All rights reserved.
 */
#define __STDC_FORMAT_MACROS

#include "monarch/test/Test.h"
#include "monarch/test/TestModule.h"
#include "monarch/rt/DynamicObject.h"
#include "monarch/rt/DynamicObjectIterator.h"
#include "monarch/modest/Kernel.h"
#include "monarch/event/Observable.h"
#include "monarch/event/ObserverDelegate.h"
#include "monarch/event/EventController.h"
#include "monarch/event/EventDaemon.h"
#include "monarch/event/EventWaiter.h"
#include "monarch/rt/Runnable.h"
#include "monarch/util/Timer.h"

#include <cstdio>

using namespace std;
using namespace monarch::test;
using namespace monarch::event;
using namespace monarch::modest;
using namespace monarch::rt;
using namespace monarch::util;

namespace mo_test_event
{

class TestObserver : public Observer
{
public:
   uint64_t idOffset;
   int events;
   int event1;
   int event2;
   int event3;
   int event4;

   ObserverDelegate<TestObserver> delegate1;
   ObserverDelegate<TestObserver> delegate2;
   ObserverDelegate<TestObserver> delegate3;
   ObserverDelegate<TestObserver> delegate4;

   TestObserver(uint64_t idOffset = 0) :
      delegate1(this, &TestObserver::handleEvent1),
      delegate2(this, &TestObserver::handleEvent2),
      delegate3(this, &TestObserver::handleEvent3),
      delegate4(this, &TestObserver::handleEvent4)
   {
      this->idOffset = idOffset;
      events = 0;
      event1 = 0;
      event2 = 0;
      event3 = 0;
      event4 = 0;
   }

   virtual ~TestObserver()
   {
   }

   virtual void eventOccurred(Event& e)
   {
      ++events;
   }

   virtual void handleEvent1(Event& e)
   {
      ++event1;
   }

   virtual void handleEvent2(Event& e)
   {
      ++event2;
   }

   virtual void handleEvent3(Event& e)
   {
      ++event3;
   }

   virtual void handleEvent4(Event& e)
   {
      if(e["id"]->getUInt64() == (3 + idOffset))
      {
         ++event3;
      }
      else if(e["id"]->getUInt64() == (4 + idOffset))
      {
         ++event4;
      }
   }
};

static void runEventTest(TestRunner& tr)
{
   tr.test("Event");

   // create kernel and start engine
   Kernel k;
   k.getEngine()->start();

   // create observable and observer
   Observable observable;
   TestObserver observer;

   // register observer and start observable
   observable.registerObserver(&observer, 1);
   observable.start(&k);

   // create and schedule events
   Event e1;
   Event e2;
   Event e3;
   e1["name"] = "Event1";
   e2["name"] = "Event2";
   e3["name"] = "Event3";
   observable.schedule(e1, 1);
   observable.schedule(e2, 1);
   observable.schedule(e3, 1);

   // wait for a second
   Thread::sleep(1000);

   assert(observer.events == 3);

   // stop observable
   observable.stop();

   // stop kernel engine
   k.getEngine()->stop();

   tr.pass();
}

static void runObserverDelegateTest(TestRunner& tr)
{
   tr.test("ObserverDelegate");

   // create kernel and start engine
   Kernel k;
   k.getEngine()->start();

   // create observable and observers
   Observable observable;
   TestObserver observer;

   // register observers and start observable
   observable.registerObserver(&observer.delegate1, 1);
   observable.registerObserver(&observer.delegate2, 2);
   observable.registerObserver(&observer.delegate3, 3);
   observable.registerObserver(&observer.delegate4, 4);
   observable.addTap(3, 4);
   observable.start(&k);

   // create and schedule events
   Event e1;
   Event e2;
   Event e3;
   Event e4;
   e1["name"] = "Event1";
   e2["name"] = "Event2";
   e3["name"] = "Event3";
   e4["name"] = "Event4";
   observable.schedule(e1, 1);
   observable.schedule(e2, 2);
   observable.schedule(e3, 3);
   observable.schedule(e4, 4);

   // wait for a second
   Thread::sleep(1000);

   assert(observer.event1 == 1);
   assert(observer.event2 == 1);
   assert(observer.event3 == 2);
   assert(observer.event4 == 1);

   // stop observable
   observable.stop();

   // stop kernel engine
   k.getEngine()->stop();

   tr.pass();
}

class TestObserverWithDyno
{
public:
   TestObserverWithDyno()
   {
   }

   virtual ~TestObserverWithDyno()
   {
   }

   virtual void handleEventWithDyno(Event& e, DynamicObject& d)
   {
      //printf("Event occurred w/dyno:\n");
      //dumpDynamicObject(d);
   }
};

static void runObserverDelegateDynoTest(TestRunner& tr)
{
   tr.test("ObserverDelegate Dyno");

   // create kernel and start engine
   Kernel k;
   k.getEngine()->start();

   // create observer
   DynamicObject d;
   d["foo"] = "bar";
   TestObserverWithDyno towd;
   ObserverDelegate<TestObserverWithDyno> od(
      &towd, &TestObserverWithDyno::handleEventWithDyno, d);

   // create observable
   Observable observable;

   // register observer and start observable
   observable.registerObserver(&od, 1);
   observable.start(&k);

   // create and schedule event
   Event e1;
   e1["name"] = "Event1";
   observable.schedule(e1, 1);

   // wait for a second
   Thread::sleep(1000);

   // stop observable
   observable.stop();

   // stop kernel engine
   k.getEngine()->stop();

   tr.pass();
}

static void runEventControllerTest(TestRunner& tr)
{
   tr.test("EventController");

   // create kernel and start engine
   Kernel k;
   k.getEngine()->start();

   // create event controller
   EventController ec;

   // create observer, use ID offset of 1 (event ID 1 is for wildcard event)
   TestObserver observer(1);

   DynamicObject types;
   // string type
   ec.registerObserver(&observer.delegate1, "event1");
   // DynamicObject array of string types
   types[0] = "event2";
   ec.registerObserver(&observer.delegate2, types);
   types[0] = "event3";
   ec.registerObserver(&observer.delegate3, types);
   types[0] = "event4";
   ec.registerObserver(&observer.delegate4, types);

   types[0] = "event1";
   ec.registerObserver(&observer, types);

   // add parent events
   ec.addParent("event2", "event1");
   ec.addParent("event3", "event1");
   ec.addParent("event4", "event3");

   // start event controller
   ec.start(&k);

   // create and schedule events
   Event e1;
   Event e2;
   Event e3;
   Event e4;
   e1["type"] = "event1";
   e2["type"] = "event2";
   e3["type"] = "event3";
   e4["type"] = "event4";
   ec.schedule(e1);
   ec.schedule(e2);
   ec.schedule(e3);
   ec.schedule(e4);

   // wait for a second
   Thread::sleep(1000);

   // check messages
   assert(observer.events == 4);
   assert(observer.event1 == 4);
   assert(observer.event2 == 1);
   assert(observer.event3 == 2);
   assert(observer.event4 == 1);

   // stop event controller
   ec.stop();

   // stop kernel engine
   k.getEngine()->stop();

   tr.pass();
}

#define DBTDONE "monarch.test.done"
class TestEventTrigger : public Runnable
{
public:
   int mSleepMS;
   EventController *mEC;

   TestEventTrigger(EventController* ec)
   {
      mEC = ec;
      mSleepMS = -1;
   }

   virtual ~TestEventTrigger()
   {
   }

   virtual void run()
   {
      // wait a bit
      if(mSleepMS >= 0)
      {
         Thread::sleep(mSleepMS);
      }

      Event e;
      e["type"] = DBTDONE;
      mEC->schedule(e);
   }
};

static void runEventWaiterTest(TestRunner& tr)
{
   tr.group("EventWaiter");

   // create kernel and start engine
   Kernel k;
   k.getEngine()->start();

   // create event controller
   EventController ec;

   // start event controller
   ec.start(&k);

   tr.test("quick fire");
   {
      // create a waiter, start, and wait
      EventWaiter ew(&ec);
      ew.start(DBTDONE);

      // create a thread to post event
      TestEventTrigger trigger(&ec);
      Thread t(&trigger);
      t.start();

      bool gotev = ew.waitForEvent();
      // pass if we get past with event
      assert(gotev);
      // stop to unreg event
      ew.stop();

      // join thread
      t.join();
   }
   tr.pass();

   tr.test("delay fire");
   {
      // create a waiter, start, and wait
      EventWaiter ew(&ec);
      ew.start(DBTDONE);

      // create a thread to post event
      TestEventTrigger trigger(&ec);
      trigger.mSleepMS = 1000;
      Thread t(&trigger);
      t.start();

      bool gotev = ew.waitForEvent();
      // pass if we get past with event
      assert(gotev);
      // stop to unreg event
      ew.stop();

      // join thread
      t.join();
   }
   tr.pass();

   // stop event controller
   ec.stop();

   // stop kernel engine
   k.getEngine()->stop();

   tr.ungroup();
}

static void runEventFilterTest(TestRunner& tr)
{
   tr.group("EventFilter");

   // create kernel and start engine
   Kernel k;
   k.getEngine()->start();

   // create event controller
   EventController ec;

   // start event controller
   ec.start(&k);

   tr.test("filter");
   {
      const char* evType = "TESTEVENT";
      Event e;
      e["type"] = evType;
      e["moo"] = false;
      e["foo"] = "bar";
      e["apples"] = 10;

      EventFilter f1;
      f1["moo"] = true;
      EventWaiter ew1(&ec);
      ew1.start(evType, &f1);

      EventFilter f2;
      f2["moo"] = true;
      EventWaiter ew2(&ec);
      ew2.start(evType, &f2);

      EventFilter f3;
      f3["moo"] = false;
      EventWaiter ew3(&ec);
      ew3.start(evType, &f3);

      EventFilter f4;
      f4["foo"] = "bar";
      EventWaiter ew4(&ec);
      ew4.start(evType, &f4);

      EventFilter f5;
      f5["foo"] = "bar";
      f5["moo"] = false;
      EventWaiter ew5(&ec);
      ew5.start(evType, &f5);

      EventFilter f6;
      f6["foo"] = "bar";
      f6["moo"] = true;
      EventWaiter ew6(&ec);
      ew6.start(evType, &f6);

      EventFilter f7;
      f7["foo"] = "woof";
      f7["moo"] = false;
      EventWaiter ew7(&ec);
      ew7.start(evType, &f7);

      EventFilter f8;
      f8["apples"] = 10;
      EventWaiter ew8(&ec);
      ew8.start(evType, &f8);

      EventFilter f9;
      f9["foo"] = "bar";
      f9["moo"] = false;
      f9["apples"] = 10;
      EventWaiter ew9(&ec);
      ew9.start(evType, &f9);

      EventFilter f10;
      f10["foo"] = "bar";
      f10["moo"] = false;
      f10["apples"] = 11;
      EventWaiter ew10(&ec);
      ew10.start(evType, &f10);

      // schedule event
      ec.schedule(e);
      Thread::sleep(250);

      // wait for event
      assert(!ew1.waitForEvent(1));
      assert(!ew2.waitForEvent(1));
      assert(ew3.waitForEvent(1));
      assert(ew4.waitForEvent(1));
      assert(ew5.waitForEvent(1));
      assert(!ew6.waitForEvent(1));
      assert(!ew7.waitForEvent(1));
      assert(ew8.waitForEvent(1));
      assert(ew9.waitForEvent(1));
      assert(!ew10.waitForEvent(1));
   }
   tr.pass();

   // stop event controller
   ec.stop();

   // stop kernel engine
   k.getEngine()->stop();

   tr.ungroup();
}

static void runEventDaemonTest(TestRunner& tr)
{
   tr.group("EventDaemon");

   // create kernel and start engine
   Kernel k;
   k.getEngine()->start();

   // create event controller
   EventController ec;

   // start event controller
   ec.start(&k);

   // create event daemon
   EventDaemon ed;

   // start event daemon
   ed.start(&k, &ec);

   const char* evType = "TESTEVENT";

   tr.test("20ms events x100");
   {
      EventWaiter ew(&ec);
      ew.start(evType);

      Event e;
      e["type"] = evType;
      e["details"]["foo"] = "bar";
      ed.add(e, 20, 100);

      int count = 0;
      uint64_t startTime = Timer::startTiming();
      for(; count < 100; ++count)
      {
         ew.waitForEvent();
         ew.popEvent();
      }
      uint64_t time = Timer::getMilliseconds(startTime);
      printf("time=%" PRIu64 "...", time);
   }
   tr.pass();

   tr.test("No events");
   {
      Event e;
      e["type"] = evType;
      e["details"]["foo"] = "bar";
      ed.add(e, 10, -1);
      Thread::sleep(100);
      ed.remove(evType);

      EventWaiter ew(&ec);
      ew.start(evType);
      assert(!ew.waitForEvent(100));
   }
   tr.pass();

   tr.test("10ms events x100, 20ms events x50");
   {
      EventWaiter ew(&ec);
      ew.start(evType);

      Event e;
      e["type"] = evType;
      e["details"]["foo"] = "bar";
      ed.add(e, 10, 100);
      ed.add(e, 20, 50);

      int count = 0;
      uint64_t startTime = Timer::startTiming();
      for(; count < 150; ++count)
      {
         ew.waitForEvent();
         ew.popEvent();
      }
      uint64_t time = Timer::getMilliseconds(startTime);
      printf("time=%" PRIu64 "...", time);
   }
   tr.pass();

   // stop event daemon
   ed.stop();

   // stop event controller
   ec.stop();

   // stop kernel engine
   k.getEngine()->stop();

   tr.ungroup();
}

static void runEventDaemonSharedEventTest(TestRunner& tr)
{
   tr.group("EventDaemon");

   // create kernel and start engine
   Kernel k;
   k.getEngine()->start();

   // create event controller
   EventController ec;

   // start event controller
   ec.start(&k);

   // create event daemon
   EventDaemon ed;

   // start event daemon
   ed.start(&k, &ec);

   const char* evType = "TESTEVENT";

   tr.test("share 10x 200ms events");
   {
      EventWaiter ew1(&ec);
      EventWaiter ew2(&ec);
      ew1.start(evType);
      ew2.start(evType);

      Event e1;
      e1["type"] = evType;
      e1["details"]["foo"] = "bar";
      ed.add(e1, 200, 5, 1);

      Event e2;
      e2["type"] = evType;
      e2["details"]["foo"] = "bar";
      ed.add(e2, 200, 5, 1);

      int count = 0;
      uint64_t startTime = Timer::startTiming();
      for(; count < 10; ++count)
      {
         ew1.waitForEvent();
         ew2.waitForEvent();
         ew1.popEvent();
         ew2.popEvent();
      }
      uint64_t time = Timer::getMilliseconds(startTime);
      printf("time=%" PRIu64 "...", time);
      assert(!ew1.waitForEvent(300));
      assert(count == 10);
   }
   tr.pass();

   tr.test("No events");
   {
      Event e;
      e["type"] = evType;
      e["details"]["foo"] = "bar";
      ed.remove(e, 200, 2);
      Thread::sleep(100);

      EventWaiter ew(&ec);
      ew.start(evType);
      assert(!ew.waitForEvent(200));
   }
   tr.pass();

   // stop event daemon
   ed.stop();

   // stop event controller
   ec.stop();

   // stop kernel engine
   k.getEngine()->stop();

   tr.ungroup();
}

static void runInteractiveEventDaemonTest(TestRunner& tr)
{
   tr.group("EventDaemon");

   // create kernel and start engine
   Kernel k;
   k.getEngine()->start();

   // create event controller
   EventController ec;

   // start event controller
   ec.start(&k);

   // create event daemon
   EventDaemon ed;

   // start event daemon
   ed.start(&k, &ec);

   const char* evType = "TESTEVENT";

   tr.test("200 millisecond event");
   if(false)
   {
      EventWaiter ew(&ec);
      ew.start(evType);

      Event e;
      e["type"] = evType;
      e["details"]["foo"] = "bar";
      ed.add(e, 200, 1);

      uint64_t startTime = Timer::startTiming();
      ew.waitForEvent();
      uint64_t time = Timer::getMilliseconds(startTime);
      printf("EVENT TIME: %" PRIu64 "\n", time);
   }
   tr.pass();

   tr.test("10x 200 millisecond event");
   {
      EventWaiter ew(&ec);
      ew.start(evType);

      Event e;
      e["type"] = evType;
      e["details"]["foo"] = "bar";
      ed.add(e, 200, 10);

      int count = 0;
      uint64_t startTime = Timer::startTiming();
      for(; count < 10; ++count)
      {
         ew.waitForEvent();
         ew.popEvent();
      }
      uint64_t time = Timer::getMilliseconds(startTime);
      printf("EVENT TIME: %" PRIu64 "\n", time);
   }
   tr.pass();

   // stop event daemon
   ed.stop();

   // stop event controller
   ec.stop();

   // stop kernel engine
   k.getEngine()->stop();

   tr.ungroup();
}

class SelfUnregisterObserver : public Observer
{
public:
   EventController* mEventController;

   SelfUnregisterObserver(EventController* ec) :
      mEventController(ec)
   {
   };

   virtual ~SelfUnregisterObserver() {}

   virtual void eventOccurred(Event& e)
   {
      mEventController->unregisterObserver(this);

      Event e2;
      e2["type"] = "signal";
      mEventController->schedule(e2);
   }
};

static void runObserverSelfUnregister(TestRunner& tr)
{
   tr.test("Observer self-unregister");
   {
      Kernel k;
      k.getEngine()->start();
      EventController ec;
      ec.start(&k);

      EventWaiter ew(&ec);
      ew.start("signal");

      SelfUnregisterObserver observer(&ec);
      ec.registerObserver(&observer, "test");

      Event e;
      e["type"] = "test";
      ec.schedule(e);

      assert(ew.waitForEvent(5000));

      ec.stop();
      k.getEngine()->stop();
   }
   tr.passIfNoException();
}

static bool run(TestRunner& tr)
{
   if(tr.isDefaultEnabled())
   {
      runEventTest(tr);
      runObserverDelegateTest(tr);
      runObserverDelegateDynoTest(tr);
      runEventControllerTest(tr);
      runEventWaiterTest(tr);
      runEventFilterTest(tr);
      runEventDaemonTest(tr);
      runEventDaemonSharedEventTest(tr);
      runObserverSelfUnregister(tr);
   }
   if(tr.isTestEnabled("event-daemon"))
   {
      runInteractiveEventDaemonTest(tr);
   }
   return true;
}

} // end namespace

MO_TEST_MODULE_FN("monarch.tests.event.test", "1.0", mo_test_event::run)
