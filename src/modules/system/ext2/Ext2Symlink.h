/*
 * Copyright (c) 2008-2014, Pedigree Developers
 *
 * Please see the CONTRIB file in the root of the source tree for a full
 * list of contributors.
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

#ifndef EXT2_SYMLINK_H
#define EXT2_SYMLINK_H

#include "Ext2Node.h"
#include "modules/system/vfs/Symlink.h"
#include "pedigree/kernel/processor/types.h"
#include "pedigree/kernel/utilities/String.h"

class File;
struct Inode;

/** A File is a file, a directory or a symlink. */
class Ext2Symlink : public Symlink, public Ext2Node
{
  private:
    /** Copy constructors are hidden - unused! */
    Ext2Symlink(const Ext2Symlink &file);
    Ext2Symlink &operator=(const Ext2Symlink &);

  public:
    /** Constructor, should be called only by a Filesystem. */
    Ext2Symlink(
        const String &name, uintptr_t inode_num, Inode *inode,
        class Ext2Filesystem *pFs, File *pParent = 0);
    /** Destructor */
    virtual ~Ext2Symlink();

    virtual uint64_t readBytewise(
        uint64_t location, uint64_t size, uintptr_t buffer, bool canBlock);
    virtual uint64_t writeBytewise(
        uint64_t location, uint64_t size, uintptr_t buffer, bool canBlock);

    void truncate();

    /** Updates inode attributes. */
    void fileAttributeChanged();
};

#endif
