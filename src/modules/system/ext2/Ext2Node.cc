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

#include "Ext2Node.h"
#include "Ext2Filesystem.h"
#include <syscallError.h>

Ext2Node::Ext2Node(uintptr_t inode_num, Inode inode, Ext2Filesystem *pFs) :
    m_Inode(inode), m_InodeNumber(inode_num), m_pExt2Fs(pFs), m_pBlocks(0),
    m_nBlocks(LITTLE_TO_HOST32(inode.i_blocks)), m_nSize(LITTLE_TO_HOST32(inode.i_size))
{


    m_pBlocks = new uint32_t[m_nBlocks];
    memset(m_pBlocks, ~0, 4*m_nBlocks);

    for (size_t i = 0; i < 12 && i < m_nBlocks; i++)
    {
        m_pBlocks[i] = LITTLE_TO_HOST32(m_Inode.i_block[i]);
    }
}

Ext2Node::~Ext2Node()
{
}

uint64_t Ext2Node::read(uint64_t location, uint64_t size, uintptr_t buffer)
{
    // Reads get clamped to the filesize.
    if (location >= m_nSize) return 0;
    if ( (location+size) >= m_nSize) size = m_nSize - location;

    if (size == 0) return 0;

    // Special case for symlinks - if we have no blocks but have positive size,
    // We interpret the i_blocks member as data.
    if (m_Inode.i_blocks == 0 && m_nSize > 0)
    {
        memcpy(reinterpret_cast<uint8_t*>(buffer+location),
               reinterpret_cast<uint8_t*>(m_Inode.i_block),
               size);
        return size;
    }

    size_t nBs = m_pExt2Fs->m_BlockSize;

    size_t nBytes = size;
    uint32_t nBlock = location / nBs;
    while (nBytes)
    {
        // If the current location is block-aligned and we have to read at least a
        // block in, we can read directly to the buffer.
        if ( (location % nBs) == 0 && nBytes >= nBs )
        {
            ensureBlockLoaded(nBlock);
            m_pExt2Fs->readBlock(m_pBlocks[nBlock], buffer);
            buffer += nBs;
            location += nBs;
            nBytes -= nBs;
            nBlock++;
        }
        // Else we have to read in a partial block.
        else
        {
            // Create a buffer for the block.
            uint8_t *pBuf = new uint8_t[nBs];
            ensureBlockLoaded(nBlock);
            m_pExt2Fs->readBlock(m_pBlocks[nBlock],
                                 reinterpret_cast<uintptr_t>(pBuf));
            // memcpy the relevant block area.
            uintptr_t start = location % nBs;
            uintptr_t size = (start+nBytes >= nBs) ? nBs-start : nBytes;
            memcpy(reinterpret_cast<uint8_t*>(buffer), &pBuf[start], size);
            delete [] pBuf;
            buffer += size;
            location += size;
            nBytes -= size;
            nBlock++;
        }
    }

    return size;
}

uint64_t Ext2Node::write(uint64_t location, uint64_t size, uintptr_t buffer)
{
    ensureLargeEnough(location+size);

    size_t nBs = m_pExt2Fs->m_BlockSize;

    size_t nBytes = size;
    uint32_t nBlock = location / nBs;
    while (nBytes)
    {
        // If the current location is block-aligned and we have to write at least a
        // block out, we can write directly to the buffer.
        if ( (location % nBs) == 0 && nBytes >= nBs )
        {
            ensureBlockLoaded(nBlock);
            m_pExt2Fs->writeBlock(m_pBlocks[nBlock], buffer);
            buffer += nBs;
            location += nBs;
            nBytes -= nBs;
            nBlock++;
        }
        // Else we have to read in a block, partially edit it and write back.
        else
        {
            // Create a buffer for the block.
            uint8_t *pBuf = new uint8_t[nBs];
            ensureBlockLoaded(nBlock);
            m_pExt2Fs->readBlock(m_pBlocks[nBlock],
                                 reinterpret_cast<uintptr_t>(pBuf));
            // memcpy the relevant block area.
            uintptr_t start = location % nBs;
            uintptr_t size = (start+nBytes >= nBs) ? nBs-start : nBytes;
            memcpy(&pBuf[start], reinterpret_cast<uint8_t*>(buffer), size);
            m_pExt2Fs->writeBlock(m_pBlocks[nBlock],
                                  reinterpret_cast<uintptr_t>(pBuf));
            delete [] pBuf;
            buffer += size;
            location += size;
            nBytes -= size;
            nBlock++;
        }
    }

    return size;
}

void Ext2Node::truncate()
{
    for (size_t i = 0; i < m_nBlocks; i++)
    {
        ensureBlockLoaded(i);
        m_pExt2Fs->releaseBlock(m_pBlocks[i]);
    }

    m_nSize = 0;
    m_nBlocks = 0;
  
    m_Inode.i_size = 0;
    m_Inode.i_blocks = 0;
}

bool Ext2Node::ensureLargeEnough(size_t size)
{
    if (size > m_nSize)
    {
        m_nSize = size;
        fileAttributeChanged(m_nSize, LITTLE_TO_HOST32(m_Inode.i_atime),
                             LITTLE_TO_HOST32(m_Inode.i_mtime),
                             LITTLE_TO_HOST32(m_Inode.i_ctime));
    }

    while (size > m_nBlocks*m_pExt2Fs->m_BlockSize)
    {
        uint32_t block = m_pExt2Fs->findFreeBlock(m_InodeNumber);
        if (block == 0)
        {
            // We had a problem.
            SYSCALL_ERROR(NoSpaceLeftOnDevice);
            return false;
        }
        if (!addBlock(block)) return false;
        // Load the block and zero it.
        uint8_t *pBuffer = new uint8_t[m_pExt2Fs->m_BlockSize];
        m_pExt2Fs->readBlock(block, reinterpret_cast<uintptr_t>(pBuffer));
        memset(pBuffer, 0, m_pExt2Fs->m_BlockSize);
        m_pExt2Fs->writeBlock(block, reinterpret_cast<uintptr_t>(pBuffer));
        delete [] pBuffer;
    }
    return true;
}

bool Ext2Node::ensureBlockLoaded(size_t nBlock)
{
    if (nBlock >= m_nBlocks)
    {
        FATAL("EXT2: ensureBlockLoaded: Algorithmic error.");
    }
    if (m_pBlocks[nBlock] == ~0U)
        getBlockNumber(nBlock);

    return true;
}

bool Ext2Node::getBlockNumber(size_t nBlock)
{
    size_t nPerBlock = m_pExt2Fs->m_BlockSize/4;

    if (nBlock < 12)
    {
        FATAL("EXT2: Algorithmic error.");
        return false;
    }

    if (nBlock < nPerBlock+12)
    {
        getBlockNumberIndirect(LITTLE_TO_HOST32(m_Inode.i_block[12]), 12, nBlock);
        return true;
    }

    if (nBlock < (nPerBlock*nPerBlock)+nPerBlock+12)
    {
        getBlockNumberBiindirect(LITTLE_TO_HOST32(m_Inode.i_block[13]), nPerBlock+12, nBlock);
        return true;
    }

    getBlockNumberTriindirect(LITTLE_TO_HOST32(m_Inode.i_block[14]),
                              (nPerBlock*nPerBlock)+nPerBlock+12, nBlock);

    return true;
}

bool Ext2Node::getBlockNumberIndirect(uint32_t inode_block, size_t nBlocks, size_t nBlock)
{
    uint32_t *buffer = new uint32_t[m_pExt2Fs->m_BlockSize/4];
    m_pExt2Fs->readBlock(inode_block, reinterpret_cast<uintptr_t>(buffer));

    for (size_t i = 0; i < m_pExt2Fs->m_BlockSize/4 && nBlocks < m_nBlocks; i++)
    {
        m_pBlocks[nBlocks++] = LITTLE_TO_HOST32(buffer[i]);
    }

    delete [] buffer;

    return true;
}

bool Ext2Node::getBlockNumberBiindirect(uint32_t inode_block, size_t nBlocks, size_t nBlock)
{
    size_t nPerBlock = m_pExt2Fs->m_BlockSize/4;

    uint32_t *buffer = new uint32_t[nPerBlock];
    m_pExt2Fs->readBlock(inode_block, reinterpret_cast<uintptr_t>(buffer));

    // What indirect block does nBlock exist on?
    size_t nIndirectBlock = (nBlock-nBlocks) / nPerBlock;

    getBlockNumberIndirect(LITTLE_TO_HOST32(buffer[nIndirectBlock]),
                           nBlocks+nIndirectBlock*nPerBlock, nBlock);

    delete [] buffer;

    return true;
}

bool Ext2Node::getBlockNumberTriindirect(uint32_t inode_block, size_t nBlocks, size_t nBlock)
{
    size_t nPerBlock = m_pExt2Fs->m_BlockSize/4;

    uint32_t *buffer = new uint32_t[nPerBlock];
    m_pExt2Fs->readBlock(inode_block, reinterpret_cast<uintptr_t>(buffer));

    // What biindirect block does nBlock exist on?
    size_t nBiBlock = (nBlock-nBlocks) / (nPerBlock*nPerBlock);

    getBlockNumberBiindirect(LITTLE_TO_HOST32(buffer[nBiBlock]),
                             nBlocks+nBiBlock*nPerBlock*nPerBlock, nBlock);

    delete [] buffer;

    return true;
}

bool Ext2Node::addBlock(uint32_t blockValue)
{
    size_t nEntriesPerBlock = m_pExt2Fs->m_BlockSize/4;

    // Calculate whether direct, indirect or tri-indirect addressing is needed.
    if (m_nBlocks < 12)
    {
        // Direct addressing is possible.
        m_Inode.i_block[m_nBlocks] = HOST_TO_LITTLE32(blockValue);
    }
    else if (m_nBlocks < 12 + nEntriesPerBlock)
    {
        // Indirect addressing needed.

        // If this is the first indirect block, we need to reserve a new table block.
        if (m_nBlocks == 12)
        {
            m_Inode.i_block[12] = HOST_TO_LITTLE32(m_pExt2Fs->findFreeBlock(m_InodeNumber));
            if (m_Inode.i_block[12] == 0)
            {
                // We had a problem.
                SYSCALL_ERROR(NoSpaceLeftOnDevice);
                return false;
            }
        }

        // Now we can set the block.
        uint32_t *buffer = new uint32_t[nEntriesPerBlock];
        m_pExt2Fs->readBlock(LITTLE_TO_HOST32(m_Inode.i_block[12]), reinterpret_cast<uintptr_t>(buffer));

        buffer[m_nBlocks-12] = HOST_TO_LITTLE32(blockValue);

        m_pExt2Fs->writeBlock(LITTLE_TO_HOST32(m_Inode.i_block[12]), reinterpret_cast<uintptr_t>(buffer));

        delete [] buffer;
    }
    else if (m_nBlocks < 12 + nEntriesPerBlock + nEntriesPerBlock*nEntriesPerBlock)
    {
        // Bi-indirect addressing required.

        // Index from the start of the bi-indirect block (i.e. ignore the 12 direct entries and one indirect block).
        size_t biIdx = m_nBlocks - 12 - nEntriesPerBlock;
        // Block number inside the bi-indirect table of where to find the indirect block table.
        size_t indirectBlock = biIdx / nEntriesPerBlock;
        // Index inside the indirect block table.
        size_t indirectIdx = biIdx % nEntriesPerBlock;

        // If this is the first bi-indirect block, we need to reserve a bi-indirect table block.
        if (biIdx == 0)
        {
            m_Inode.i_block[13] = HOST_TO_LITTLE32(m_pExt2Fs->findFreeBlock(m_InodeNumber));
            if (m_Inode.i_block[13] == 0)
            {
                // We had a problem.
                SYSCALL_ERROR(NoSpaceLeftOnDevice);
                return false;
            }
        }

        // Now we can safely read the bi-indirect block.
        uint32_t *pBlock = new uint32_t[nEntriesPerBlock];

        m_pExt2Fs->readBlock(LITTLE_TO_HOST32(m_Inode.i_block[13]), reinterpret_cast<uintptr_t>(pBlock));

        // Do we need to start a new indirect block?
        if (indirectIdx == 0)
        {
            pBlock[indirectBlock] = HOST_TO_LITTLE32(m_pExt2Fs->findFreeBlock(m_InodeNumber));
            if (pBlock[indirectBlock] == 0)
            {
                // We had a problem.
                SYSCALL_ERROR(NoSpaceLeftOnDevice);
                return false;
            }

            // Write the block back, as we've modified it.
            m_pExt2Fs->writeBlock(LITTLE_TO_HOST32(m_Inode.i_block[13]), reinterpret_cast<uintptr_t>(pBlock));
        }

        // Cache this as it gets clobbered by the readBlock call (using the same buffer).
        uint32_t nIndirectBlockNum = LITTLE_TO_HOST32(pBlock[indirectBlock]);

        // Grab the indirect block.
        m_pExt2Fs->readBlock(nIndirectBlockNum, reinterpret_cast<uintptr_t>(pBlock));

        // Set the correct entry.
        pBlock[indirectIdx] = HOST_TO_LITTLE32(blockValue);

        // Write back.
        m_pExt2Fs->writeBlock(nIndirectBlockNum, reinterpret_cast<uintptr_t>(pBlock));

        // Done.
        delete [] pBlock;
    }
    else
    {
        // Tri-indirect addressing required.
        FATAL("EXT2: Tri-indirect addressing required, but not implemented.");
        return false;
    }

    uint32_t *pTmp = new uint32_t[m_nBlocks+1];
    memcpy(pTmp, m_pBlocks, m_nBlocks*4);
    delete [] m_pBlocks;
    m_pBlocks = pTmp;
    m_pBlocks[m_nBlocks] = blockValue;

    m_nBlocks++;
    m_Inode.i_blocks = HOST_TO_LITTLE32(m_nBlocks);
    m_pExt2Fs->setInode(m_InodeNumber, m_Inode);

    return true;
}

void Ext2Node::fileAttributeChanged(size_t size, size_t atime, size_t mtime, size_t ctime)
{
    // Reconstruct the inode from the cached fields.
    m_Inode.i_blocks = HOST_TO_LITTLE32(m_nBlocks);
    m_Inode.i_size = HOST_TO_LITTLE32(size); /// \todo 4GB files.
    m_Inode.i_atime = HOST_TO_LITTLE32(atime);
    m_Inode.i_mtime = HOST_TO_LITTLE32(mtime);
    m_Inode.i_ctime = HOST_TO_LITTLE32(ctime);

    m_pExt2Fs->setInode(m_InodeNumber, m_Inode);
}
