/*
 * Copyright (c) 2008-2011 Digital Bazaar, Inc. All rights reserved.
 */
#ifndef monarch_validation_ValidatorList_H
#define monarch_validation_ValidatorList_H

#include "monarch/validation/Validator.h"
#include <vector>

namespace monarch
{
namespace validation
{

/**
 * Base class for Validators that use a list of Validators.  Common methods are
 * provided to build up an internal list of sub-validators.
 *
 * See All and Any for examples.
 *
 * @author David I. Lehn
 */
class ValidatorList : public Validator
{
protected:
   /* A list of validators */
   std::vector<Validator*> mValidators;
   std::vector<ValidatorRef> mValidatorRefs;

public:
   /**
    * Creates a new ValidatorList.
    */
   ValidatorList();

   /**
    * Creates a new ValidatorList with a NULL terminated list of Validators.
    *
    * @param validator a Validator
    * @param ... more Validators.
    */
   ValidatorList(Validator* validator, ...);

   /**
    * Destructs this ValidatorList.
    */
   virtual ~ValidatorList();

   /**
    * Adds a validator to the list.
    *
    * @param validator a Validator.
    */
   virtual void addValidator(Validator* validator);

   /**
    * Adds a validator to the list.
    *
    * @param validator a Validator.
    */
   virtual void addValidatorRef(ValidatorRef validator);

   /**
    * Adds validators to the list.
    *
    * @param validator a Validator.
    * @param ap a vararg list.
    */
   virtual void addValidators(Validator* validator, va_list ap);

   /**
    * Adds a NULL terminated list of validators to the list.
    *
    * @param validator a Validator.
    * @param ... more Validators.
    */
   virtual void addValidators(Validator* validator, ...);
};

} // end namespace validation
} // end namespace monarch
#endif
