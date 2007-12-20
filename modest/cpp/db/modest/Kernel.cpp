/*
 * Copyright (c) 2007 Digital Bazaar, Inc.  All rights reserved.
 */
#include "db/modest/Kernel.h"
#include "db/modest/OperationImpl.h"

using namespace db::modest;
using namespace db::rt;

Kernel::Kernel()
{
   // create engine and module library
   mEngine = new Engine();
   mModuleLibrary = new ModuleLibrary(this);
   mVersion = "1.0";
}

Kernel::~Kernel()
{
   delete mEngine;
   delete mModuleLibrary;
}

void Kernel::runOperation(Operation& op)
{
   // queue operation with engine
   getEngine()->queue(op);
}

Engine* Kernel::getEngine()
{
   return mEngine;
}

ModuleLibrary* Kernel::getModuleLibrary()
{
   return mModuleLibrary;
}

const char* Kernel::getVersion()
{
   return mVersion;
}

inline Kernel* createModestKernel()
{
   return new db::modest::Kernel();
}

inline void freeModestKernel(Kernel* k)
{
   delete k;
}
