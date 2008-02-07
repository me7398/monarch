/*
 * Copyright (c) 2008 Digital Bazaar, Inc.  All rights reserved.
 */
#ifndef db_compress_zip_ZipEntry_H
#define db_compress_zip_ZipEntry_H

#include "db/rt/Collectable.h"
#include "db/io/InputStream.h"
#include "db/io/OutputStream.h"
#include "db/util/Date.h"

namespace db
{
namespace compress
{
namespace zip
{

/**
 * A ZipEntryImpl provides the implementation for a reference-counted ZipEntry.
 * 
 * A ZipEntry is typedef'ed below.
 * 
 * @author Dave Longley
 */
class ZipEntryImpl
{
protected:
   /**
    * The file name for this entry.
    */
   char* mFilename;
   
   /**
    * The file comment for this entry.
    */
   char *mFileComment;
   
   /**
    * The last modification date & time for this entry using
    * the MS-DOS date & time format.
    */
   unsigned int mDosTime;
   
   /**
    * The compressed size for this entry's data.
    */
   unsigned int mCompressedSize;
   
   /**
    * The uncompressed size for this entry's data.
    */
   unsigned int mUncompressedSize;
   
   /**
    * The crc-32 for this entry.
    */
   unsigned int mCrc32;
   
   /**
    * Stores the offset to the local header, relative to the start of the
    * first disk on which the file appears.
    */
   unsigned int mLocalHeaderOffset;
   
public:
   /**
    * Creates a new ZipEntryImpl.
    */
   ZipEntryImpl();
   
   /**
    * Destructs this ZipEntryImpl.
    */
   virtual ~ZipEntryImpl();
   
   /**
    * Gets the size, in bytes, of a local file header for this entry.
    * 
    * @return the size, in bytes, of a local file header for this entry.
    */
   virtual unsigned int getLocalFileHeaderSize();
   
   /**
    * Gets the size, in bytes, of a file header for this entry.
    * 
    * @return the size, in bytes, of a file header for this entry.
    */
   virtual unsigned int getFileHeaderSize();
   
   /**
    * Sets the filename for this entry.
    * 
    * @param filename the filename for this entry.
    */
   virtual void setFilename(const char* filename);
   
   /**
    * Gets the filename for this entry.
    * 
    * @return the filename for this entry.
    */
   virtual const char* getFilename();
   
   /**
    * Sets the file comment for this entry.
    * 
    * @param comment the file comment for this entry.
    */
   virtual void setFileComment(const char* comment);
   
   /**
    * Gets the file comment for this entry.
    * 
    * @return the file comment for this entry.
    */
   virtual const char* getFileComment();
   
   /**
    * Sets the last modification date for the file.
    * 
    * @param date the last modification date for the file.
    */
   virtual void setDate(db::util::Date* date);
   
   /**
    * Sets the last modification date for the file according to a
    * MS-DOS date & time.
    * 
    * @param dosTime the MS-DOS date & time to use.
    */
   virtual void setDosTime(unsigned int dosTime);
   
   /**
    * Gets the last modification date for the file according to a
    * MS-DOS date & time.
    * 
    * @return the MS-DOS date & time to use.
    */
   virtual unsigned int getDosTime();
   
   /**
    * Sets the compressed size for this entry.
    * 
    * @param size the compressed size for this entry.
    */
   virtual void setCompressedSize(unsigned int size);
   
   /**
    * Gets the compressed size for this entry.
    * 
    * @return the compressed size for this entry.
    */
   virtual unsigned int getCompressedSize();
   
   /**
    * Sets the uncompressed size for this entry.
    * 
    * @param size the uncompressed size for this entry.
    */
   virtual void setUncompressedSize(unsigned int size);
   
   /**
    * Gets the uncompressed size for this entry.
    * 
    * @return the uncompressed size for this entry.
    */
   virtual unsigned int getUncompressedSize();
   
   /**
    * Sets the crc-32 for this entry.
    * 
    * @param crc the crc-32 for this entry.
    */
   virtual void setCrc32(unsigned int crc);
   
   /**
    * Gets the crc-32 for this entry.
    * 
    * @return the crc-32 for this entry.
    */
   virtual unsigned int getCrc32();
   
   /**
    * Sets the offset to the local file header.
    * 
    * @param offset the offset to the local file header.
    */
   virtual void setLocalFileHeaderOffset(unsigned int offset);
   
   /**
    * Gets the offset to the local file header.
    * 
    * @return the offset to the local file header.
    */
   virtual unsigned int getLocalFileHeaderOffset();
};

/**
 * A ZipEntry contains information about a single file in a ZIP archive.
 * 
 * @author Dave Longley
 */
class ZipEntry : public db::rt::Collectable<ZipEntryImpl>
{
public:
   /**
    * Creates a new ZipEntry.
    */
   ZipEntry();
   
   /**
    * Creates a new ZipEntry that uses the passed ZipEntryImpl.
    * 
    * @param impl the ZipEntryImpl to use.
    */
   ZipEntry(ZipEntryImpl* impl);
   
   /**
    * Destructs this Operation.
    */
   virtual ~ZipEntry();
};

} // end namespace zip
} // end namespace compress
} // end namespace db
#endif