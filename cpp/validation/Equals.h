/*
 * Copyright (c) 2008-2009 Digital Bazaar, Inc. All rights reserved.
 */
#ifndef monarch_validation_Equals_H
#define monarch_validation_Equals_H

#include "monarch/validation/Validator.h"

namespace monarch
{
namespace validation
{

/**
 * Validates the equality of the validation target object with another object.
 *
 * @author David I. Lehn
 */
class Equals : public Validator
{
protected:
   monarch::rt::DynamicObject mObject;

public:
   /**
    * Creates a new validator.
    *
    * @param object object to validate against.
    * @param errorMessage custom error message.
    */
   Equals(monarch::rt::DynamicObject& object, const char* errorMessage = NULL);

   /**
    * Creates a new validator.
    *
    * @param str the string to validate against.
    * @param errorMessage custom error message.
    */
   Equals(const char* str, const char* errorMessage = NULL);

   /**
    * Creates a new validator.
    *
    * @param b the boolean to validate against.
    * @param errorMessage custom error message.
    */
   Equals(bool b, const char* errorMessage = NULL);

   /**
    * Destructs this validator.
    */
   virtual ~Equals();

   /**
    * Checks if an object is valid.
    *
    * @param obj the object to validate.
    * @param context context to use during validation.
    *
    * @return true if obj is valid, false and exception set otherwise.
    */
   virtual bool isValid(
      monarch::rt::DynamicObject& obj,
      ValidatorContext* context);
   using Validator::isValid;
};

} // end namespace validation
} // end namespace monarch
#endif
