/*
 -------------------------------------------------------------------------------
 This file is part of the Plink, Plonk, Plank libraries
  by Martin Robinson
 
 https://github.com/0x4d52/pl-nk/
 
 Copyright University of the West of England, Bristol 2011-16
 All rights reserved.
 
 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions are met:
 
 * Redistributions of source code must retain the above copyright
   notice, this list of conditions and the following disclaimer.
 * Redistributions in binary form must reproduce the above copyright
   notice, this list of conditions and the following disclaimer in the
   documentation and/or other materials provided with the distribution.
 * Neither the name of University of the West of England, Bristol nor 
   the names of its contributors may be used to endorse or promote products
   derived from this software without specific prior written permission.
 
 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE 
 DISCLAIMED. IN NO EVENT SHALL UNIVERSITY OF THE WEST OF ENGLAND, BRISTOL BE 
 LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR 
 CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE 
 GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) 
 HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT 
 LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT 
 OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE. 
 
 This software makes use of third party libraries. For more information see:
 doc/license.txt included in the distribution.
 -------------------------------------------------------------------------------
 */

// unlikely to implement now... other more straightforward solutions used...

//#ifndef PLANK_LOCKFREEMEMORY_H
//#define PLANK_LOCKFREEMEMORY_H
//
//#include "../containers/plank_LockFreeStack.h"
//
//PLANK_BEGIN_C_LINKAGE
//
//typedef struct PlankLockFreeMemory* PlankLockFreeMemoryRef; 
//
//PlankLockFreeMemoryRef pl_LockFreeMemory_CreateAndInit();
//PlankLockFreeMemoryRef pl_LockFreeMemory_Create();
//PlankResult pl_LockFreeMemory_Init (PlankLockFreeMemoryRef p);
//PlankResult pl_LockFreeMemory_DeInit (PlankLockFreeMemoryRef p);
//PlankResult pl_LockFreeMemory_Destroy (PlankLockFreeMemoryRef p);
//void* pl_LockFreeMemory_AllocateBytes (PlankLockFreeMemoryRef p, PlankUL numBytes);
//PlankResult pl_LockFreeMemory_Free (PlankLockFreeMemoryRef p, void* ptr);
//
//PLANK_END_C_LINKAGE
//
////------------------------------------------------------------------------------
//
//struct PlankLockFreeMemoryDescriptorQueue;
//struct PlankLockFreeMemoryAnchor;
//struct PlankLockFreeMemoryDescriptor;
//struct PlankLockFreeMemorySizeClass;
//struct PlankLockFreeMemoryActive;
//struct PlankLockFreeMemoryProcessHeap;
//
//typedef struct PlankLockFreeMemoryDescriptorQueue*  PlankLockFreeMemoryDescriptorQueueRef;
//typedef struct PlankLockFreeMemoryAnchor*           PlankLockFreeMemoryAnchorRef;
//typedef struct PlankLockFreeMemoryDescriptor*       PlankLockFreeMemoryDescriptorRef;
//typedef struct PlankLockFreeMemorySizeClass*        PlankLockFreeMemorySizeClassRef;
//typedef struct PlankLockFreeMemoryActive*           PlankLockFreeMemoryActiveRef;
//typedef struct PlankLockFreeMemoryProcessHeap*      PlankLockFreeMemoryProcessHeapRef;
//
//#if !DOXYGEN
//
////------------------------------------------------------------------------------
//
//#define PLANKLOCKFREEMEMORY_TYPE_SIZE           (4)
//#define PLANKLOCKFREEMEMORY_PTR_SIZE            (sizeof(void*))
//#define PLANKLOCKFREEMEMORY_HEADER_SIZE         (PLANKLOCKFREEMEMORY_TYPE_SIZE + PLANKLOCKFREEMEMORY_PTR_SIZE)
//
//#define PLANKLOCKFREEMEMORY_LARGE               (0)
//#define PLANKLOCKFREEMEMORY_SMALL               (1)
//
//#define	PLANKLOCKFREEMEMORY_PAGESIZE            (4096)
//#define PLANKLOCKFREEMEMORY_SUPERBLOCKSIZE      (16 * PLANKLOCKFREEMEMORY_PAGESIZE)
//#define PLANKLOCKFREEMEMORY_DESCSUPERBLOCKBSIZE (1024 * sizeof (descriptor))
//
//#define PLANKLOCKFREEMEMORY_ACTIVE              (0)
//#define PLANKLOCKFREEMEMORY_FULL                (1)
//#define PLANKLOCKFREEMEMORY_PARTIAL             (2)
//#define PLANKLOCKFREEMEMORY_EMPTY               (3)
//
//#define	PLANKLOCKFREEMEMORY_ACTIVEPTRBITS       (58)
//#define	PLANKLOCKFREEMEMORY_ACTIVECREDITBITS    (6)
//
//#define	PLANKLOCKFREEMEMORY_MAXCREDITS          (1L << PLANKLOCKFREEMEMORY_ACTIVECREDITBITS)
//#define PLANKLOCKFREEMEMORY_GRANULARITY         (8)
//
//#define PLANKLOCKFREEMEMORY_NUMSIZECLASSES      (2048 / PLANKLOCKFREEMEMORY_GRANULARITY)
//
////------------------------------------------------------------------------------
//
//typedef struct PlankLockFreeMemoryDescriptorQueue 
//{
//	PlankBits descriptoAvailable:46;
//    PlankBits tag:18;
//} PlankLockFreeMemoryDescriptorQueue;
//
///* Superblock descriptor structure. We bumped avail and count 
// * to 24 bits to support larger superblock sizes. */
//typedef struct PlankLockFreeMemoryAnchor 
//{
//	PlankBits 	 avail:24;
//    PlankBits    count:24;
//    PlankBits    state:2;
//    PlankBits    tag:14;
//} PlankLockFreeMemoryAnchor;
//
//typedef struct PlankLockFreeMemoryDescriptor 
//{
//	PlankLockFreeStackElement           padding;
//	volatile PlankLockFreeMemoryAnchor  anchor;
//	PlankLockFreeMemoryDescriptorRef    next;
//	PlankP                              superBlock;	// pointer to superblock
//	PlankLockFreeMemoryProcessHeapRef   heap;		// pointer to owner procheap
//	PlankUI                             size;		// block size
//	PlankUI                             maxCount;	// superblock size / sz
//} PlankLockFreeMemoryDescriptor;
//
//typedef struct PlankLockFreeMemorySizeClass
//{
//	PlankLockFreeStack  partial;        // initially empty
//	PlankUI             size;           // block size
//	PlankUI             superBlockSize;		// superblock size
//} PlankLockFreeMemorySizeClass;
//
//typedef struct PlankLockFreeMemoryActive
//{
//	PlankBits ptr:58;
//    PlankBits credits:6;
//} PlankLockFreeMemoryActive;
//
//typedef struct PlankLockFreeMemoryProcessHeap 
//{
//	volatile PlankLockFreeMemoryActive          active;		// initially NULL
//	volatile PlankLockFreeMemoryDescriptorRef	partial;	// initially NULL
//	PlankLockFreeMemorySizeClassRef             sizeclass;		// pointer to parent sizeclass
//} PlankLockFreeMemoryProcessHeap;
//
////------------------------------------------------------------------------------
//
//typedef struct PlankLockFreeMemory
//{
//    PlankLockFreeMemorySizeClass sizeClasses[PLANKLOCKFREEMEMORY_NUMSIZECLASSES];
//    PlankLockFreeMemoryProcessHeapRef heaps[PLANKLOCKFREEMEMORY_NUMSIZECLASSES];
//} PlankLockFreeMemory;
//#endif
//
////------------------------------------------------------------------------------
//
//
//#endif // PLANK_LOCKFREEMEMORY_H
