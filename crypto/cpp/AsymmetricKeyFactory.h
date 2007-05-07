/*
 * Copyright (c) 2007 Digital Bazaar, Inc.  All rights reserved.
 */
#ifndef AsymmetricKeyFactory_H
#define AsymmetricKeyFactory_H

#include "IOException.h"
#include "PrivateKey.h"
#include "PublicKey.h"

#include <string>

namespace db
{
namespace crypto
{

/**
 * An AsymmetricKeyFactory is used to create and load asymmetric
 * cryptographic keys.
 * 
 * @author Dave Longley
 */
class AsymmetricKeyFactory : public virtual db::rt::Object
{
protected:
   /**
    * A callback function that is called to obtain a password to unlock
    * an encrypted key.
    * 
    * @param b the buffer to populate with a password.
    * @param length the length of the buffer to populate.
    * @param flag a flag that is reserved for future use.
    * @param userData a pointer to some user data.
    * 
    * @return the length of the password.
    */
   static int passwordCallback(char* b, int length, int flag, void* userData);
      
public:
   /**
    * Creates a new AsymmetricKeyFactory.
    */
   AsymmetricKeyFactory();
   
   /**
    * Destructs this AsymmetricKeyFactory.
    */
   virtual ~AsymmetricKeyFactory();
   
   /**
    * Loads a private key from a PEM formatted string. A PEM formatted
    * string is just the base64-encoded version of an ASN.1 DER-encoded key
    * structure that has a header and footer.
    * 
    * @param pem the PEM string to load the key from.
    * @param password the password to use to load the key.
    * 
    * @return the loaded PrivateKey.
    * 
    * @exception IOException thrown if an IO error occurs.
    */
   virtual PrivateKey* loadPrivateKeyFromPem(
      const std::string& pem, const std::string& password)
   throw(db::io::IOException);
   
   /**
    * Writes a private key to a PEM formatted string. A PEM formatted
    * string is just the base64-encoded version of an ASN.1 DER-encoded key
    * structure that has a header and footer.
    * 
    * @param key the PrivateKey to write to a PEM string.
    * @param password the password to use to encrypt the key.
    * 
    * @return the PEM string.
    * 
    * @exception IOException thrown if an IO error occurs.
    */
   std::string writePrivateKeyToPem(
      PrivateKey* key, const std::string& password)
   throw(db::io::IOException);
   
   /**
    * Loads a public key from a PEM formatted string. A PEM formatted
    * string is just the base64-encoded version of an ASN.1 DER-encoded key
    * structure that has a header and footer.
    * 
    * @param pem the PEM string to load the key from.
    * @param password the password to use to load the key.
    * 
    * @return the loaded PublicKey.
    * 
    * @exception IOException thrown if an IO error occurs.
    */
   virtual PublicKey* loadPublicKeyFromPem(const std::string& pem)
   throw(db::io::IOException);
   
   /**
    * Writes a public key to a PEM formatted string. A PEM formatted
    * string is just the base64-encoded version of an ASN.1 DER-encoded key
    * structure that has a header and footer.
    * 
    * @param key the PublicKey to write to a PEM string.
    * @param password the password to use to encrypt the key.
    * 
    * @return the PEM string.
    * 
    * @exception IOException thrown if an IO error occurs.
    */
   std::string writePublicKeyToPem(PublicKey* key) throw(db::io::IOException);
};

} // end namespace crypto
} // end namespace db
#endif
