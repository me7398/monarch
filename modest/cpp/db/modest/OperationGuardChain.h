/*
 * Copyright (c) 2007 Digital Bazaar, Inc.  All rights reserved.
 */
#ifndef db_modest_OperationGuardChain_H
#define db_modest_OperationGuardChain_H

#include "db/modest/OperationGuard.h"

namespace db
{
namespace modest
{

// forward declare Operation
class Operation;

/**
 * An OperationGuardChain is used to chain two OperationGuards together.
 * 
 * @author Dave Longley
 */
class OperationGuardChain : public OperationGuard
{
protected:
   /**
    * The OperationGuard to run first.
    */
   OperationGuard* mGuard1;
   CollectableOperationGuard mGuardReference1;
   
   /**
    * The OperationGuard to run second.
    */
   OperationGuard* mGuard2;
   CollectableOperationGuard mGuardReference2;
   
public:
   /**
    * Creates a new OperationGuardChain that chains the passed two
    * OperationGuards together in the order specified.
    * 
    * @param g1 the first OperationGuard.
    * @param g2 the second OperationGuard.
    */
   OperationGuardChain(OperationGuard* g1, OperationGuard* g2);
   OperationGuardChain(CollectableOperationGuard& g1, OperationGuard* g2);
   OperationGuardChain(OperationGuard* g1, CollectableOperationGuard& g2);
   OperationGuardChain(
      CollectableOperationGuard& g1, CollectableOperationGuard& g2);
   
   /**
    * Destructs this OperationGuardChain.
    */
   virtual ~OperationGuardChain();
   
   /**
    * This method allows for custom conditions to be checked that allow
    * this Operation to execute after it has been queued, or that require
    * it to wait or be canceled.
    * 
    * It returns true if the passed State meets the conditions of this guard
    * such that an Operation that requires this guard could be executed
    * immediately by an Engine with the given State.
    * 
    * This method will be called before an Operation is executed. It should
    * only return true if the Operation can execute immediately. If the
    * Operation should wait or perhaps be canceled, then this method
    * should return false. If this method returns true, then Operation will
    * be executed immediately, if it returns false, then this guard's
    * "mustCancelOperation()" method will be called.
    * 
    * @param s the ImmutableState to inspect.
    * @param op the Operation this guard is for.
    * 
    * @return true if an Engine with the given State could immediately execute
    *         an Operation with this guard, false if it should wait and be
    *         checked for possible cancelation.
    */
   virtual bool canExecuteOperation(ImmutableState* s, Operation &op);
   
   /**
    * This method allows for custom conditions to be checked that require
    * this Operation to be canceled after it has been queued for execution,
    * but before it is executed.
    * 
    * It returns true if the passed State cannot meet the conditions of this
    * guard such that an Operation that requires this guard must be immediately
    * canceled. 
    * 
    * This method may return false when an Operation may not be able to be
    * executed immediately, but it needn't be canceled either, instead it
    * can be checked again later for execution or cancelation.
    * 
    * It will be called before an Operation is executed, and after
    * "canExecuteOperation()" if that method returns false. This method
    * should only return true if the Operation should not be executed at
    * all and should be canceled instead.
    * 
    * To cancel any Operation after it has been queued or after it has started
    * executing, regardless of the Operation's custom cancel implementation,
    * use Operation.interrupt().
    * 
    * @param s the ImmutableState to inspect.
    * @param op the Operation this guard is for.
    * 
    * @return true if an Engine with the given State must cancel an Operation
    *         with this guard before it executes, false if not.
    */
   virtual bool mustCancelOperation(ImmutableState* s, Operation &op);
};

} // end namespace modest
} // end namespace db
#endif
