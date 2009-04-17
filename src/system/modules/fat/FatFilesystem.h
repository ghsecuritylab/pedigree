/*
 * Copyright (c) 2008 James Molloy, Jörg Pfähler, Matthew Iselin
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */
#ifndef FATFILESYSTEM_H
#define FATFILESYSTEM_H

#include <vfs/Filesystem.h>
#include <utilities/List.h>
#include <utilities/Vector.h>
#include <utilities/Tree.h>
#include <process/Mutex.h>
#include <LockGuard.h>
#include "FatFile.h"

#include "fat.h"

// FAT Attributes
#define ATTR_READONLY   0x01
#define ATTR_HIDDEN     0x02
#define ATTR_SYSTEM     0x04
#define ATTR_VOLUME_ID  0x08
#define ATTR_DIRECTORY  0x10
#define ATTR_ARCHIVE    0x20

#define ATTR_LONG_NAME      ( ATTR_READONLY | ATTR_HIDDEN | ATTR_SYSTEM | ATTR_VOLUME_ID )
#define ATTR_LONG_NAME_MASK ( ATTR_READONLY | ATTR_HIDDEN | ATTR_SYSTEM | ATTR_VOLUME_ID | ATTR_DIRECTORY | ATTR_ARCHIVE )

/** This class provides an implementation of the FAT filesystem. */
class FatFilesystem : public Filesystem
{
  friend class FatFile;
  friend class FatDirectory;
  
public:
  FatFilesystem();

  virtual ~FatFilesystem();
  
    
  /** FAT type */
  enum FatType
  {
    FAT12 = 0, FAT16, FAT32
  };


  //
  // Filesystem interface.
  //

  virtual bool initialise(Disk *pDisk);
  static Filesystem *probe(Disk *pDisk);
  virtual File* getRoot();
  virtual String getVolumeLabel();
  virtual uint64_t read(File *pFile, uint64_t location, uint64_t size, uintptr_t buffer);
  virtual uint64_t write(File *pFile, uint64_t location, uint64_t size, uintptr_t buffer);
  virtual void truncate(File *pFile);
  virtual void fileAttributeChanged(File *pFile);
  virtual void cacheDirectoryContents(File *pFile);

protected:

  virtual bool createFile(File* parent, String filename, uint32_t mask);
  virtual bool createDirectory(File* parent, String filename);
  virtual bool createSymlink(File* parent, String filename, String value);
  virtual bool remove(File* parent, File* file);

  FatFilesystem(const FatFilesystem&);
  void operator =(const FatFilesystem&);
  
  /** Reads a cluster from the disk. */
  bool readCluster(uint32_t block, uintptr_t buffer);
  
  /** Writes a cluster to the disk. */
  bool writeCluster(uint32_t block, uintptr_t buffer);
  
  /** Reads a block starting from a specific sector from the disk. */
  bool writeSectorBlock(uint32_t sec, size_t size, uintptr_t buffer);
  
  /** Writes a block starting from a specific sector to the disk. */
  bool readSectorBlock(uint32_t sec, size_t size, uintptr_t buffer);
  
  /** Obtains the first sector given a cluster number */
  uint32_t getSectorNumber(uint32_t cluster);
  
  /** Grabs a cluster entry - bLock determines if this should enforce locking
    * internally or allow the caller to ensure the FAT is locked. */
  uint32_t getClusterEntry(uint32_t cluster, bool bLock = true);
  
  /** Sets a cluster entry - bLock determines if this should enforce locking
    * internally or allow the caller to ensure the FAT is locked. */
  uint32_t setClusterEntry(uint32_t cluster, uint32_t value, bool bLock = true);
  
  /** Converts a string to 8.3 format */
  String convertFilenameTo(String filename);
  
  /** Converts a string from 8.3 format */
  String convertFilenameFrom(String filename);
  
  /** Finds a free cluster - bLock determines if we should enforce locking,
    * defaults to false because findFreeCluster is generally called within a
    * function that has already locked the FAT */
  uint32_t findFreeCluster(bool bLock = false);
  
  /** Updates the size of a file on disk */
  void updateFileSize(File* pFile, int64_t sizeChange);
  
  /** Sets the cluster for a file on disk */
  void setCluster(File* pFile, uint32_t clus);
  
  /** Reads part of a directory into a buffer, returns the allocated buffer (which needs to be freed */
  void* readDirectoryPortion(uint32_t clus);
  
  /** Writes part of a directory from a buffer */
  void writeDirectoryPortion(uint32_t clus, void* p);
  
  /** Creates a file - actual doer for the public createFile */
  File* createFile(File* parent, String filename, uint32_t mask, bool bDirectory);
  
  /** Reads a directory entry from disk */
  Dir* getDirectoryEntry(uint32_t clus, uint32_t offset);
  
  /** Writes a directry entry to disk */
  void writeDirectoryEntry(Dir* dir, uint32_t clus, uint32_t offset);
  
  /** Is a given cluster *VALUE* EOF? */
  inline bool isEof(uint32_t cluster)
  {
    return (cluster >= eofValue());
  }
  
  /** EOF values */
  inline uint32_t eofValue()
  {
    if(m_Type == FAT12)
      return 0x0FF8;
    if(m_Type == FAT16)
      return 0xFFF8;
    if(m_Type == FAT32)
      return 0x0FFFFFF8;
    return 0;
  }
  
  /** Gets a UNIX timestamp from a FAT date/time */
  inline Time getUnixTimestamp(uint16_t time, uint16_t date)
  {
    // cumulative count of days in each month, to add to the current month to get
    // the number of days before this month
    static uint16_t cumulativeDays[] = {0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334, 365};
    
    // struct version of the passed parameters
    Timestamp* sTime = reinterpret_cast<Timestamp*>(&time);
    Date* sDate = reinterpret_cast<Date*>(&date);
    
    // grab the time information
    uint32_t seconds = sTime->secCount * 2;
    uint32_t minutes = sTime->minutes;
    uint32_t hours = sTime->hours;
    
    // grab the date information
    uint32_t day = sDate->day;
    uint32_t cumulDays = cumulativeDays[sDate->month - 1];
    uint32_t years = sDate->years;
    
    // defaults to ten years, as FAT dates start at 1980
    Time ret = 10 * 365 * 24 * 60 * 60;
    
    // add the time
    ret += seconds;
    ret += minutes * 60;
    ret += hours * 60 * 60;
    
    // and finally the date
    ret += day * 24 * 60 * 60;
    ret += cumulDays * 24 * 60 * 60;
    ret += years * 365 * 24 * 60 * 60;
    
    // completed
    return ret;
  }
  
  /** Our raw device. */
  Disk *m_pDisk;

  /** Our superblocks */
  Superblock m_Superblock;
  Superblock16 m_Superblock16;
  Superblock32 m_Superblock32;
  FSInfo32 m_FsInfo;
  
  /** Type of the FAT */
  FatType m_Type;
  
  /** Required information */
  uint64_t m_DataAreaStart; // data area can potentially start above 4 GB
  uint32_t m_RootDirCount;
  
  /** Root directory information */
  union RootDirInfo
  {
    uint32_t sector; // FAT12 and 16 don't use a cluster
    uint32_t cluster; // but FAT32 does...
  } m_RootDir;

  /** Size of a block (in this case, a cluster) */
  uint32_t m_BlockSize;
  
  /** FAT cache */
  uint8_t *m_pFatCache;
  
  /** FAT lock */
  Mutex m_FatLock;
  
  /** Disk lock */
  Mutex m_DiskLock;

  /** Root filesystem node. */
  File *m_pRoot;
};

#endif
