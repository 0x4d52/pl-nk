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

#ifndef PLONK_BLOCKSIZE_H
#define PLONK_BLOCKSIZE_H

#include "../../containers/variables/plonk_VariableForwardDeclarations.h"
#include "../../containers/variables/plonk_Variable.h"
#include "../../core/plonk_Receiver.h"
#include "../../maths/plonk_InlineCommonOps.h"

class BlockSizeDefault;
class BlockSizeNoPreference;

/** A specialised Variable<int> for handling block sizes. 
 One or more BlockSize objects store the block size for different parts of 
 the graph. For most applications there can be just one, default block size 
 which is used by the enitre graph (plus a global block size of 1 being 
 available for things like constants and other "control rate" channels). 
 All channels register themsleves as a BlockSize::Receiver so they can be 
 notified of block size changes and resize their output buffers accordingly. 
 There are accessors to get the default block size, and a "no preference" 
 block size (which is equal to 0). Generators (e.g., oscillators) should have 
 a preference (e.g., the default block size) but processors should commonly 
 specify no preference (thus, their block size will be decided according to 
 the block size of their inputs). 
 @see SampleRate */
class BlockSize : public IntVariable
{
public:
    typedef VariableInternal<int>           Internal;
    typedef IntVariable                     Base;    
    typedef WeakPointerContainer<BlockSize> Weak;
    
    BlockSize() throw();
    
    /** To catch a potential programming error. */
    BlockSize (SampleRate const& sampleRate) throw();
    
    /** Create BlockSize with an int. */
    BlockSize (const int initValue) throw();
    
    /** Create BlockSize from an IntVariable. */
    BlockSize (IntVariable const& initValue) throw();
    
    /** Copy constructor. */
    BlockSize (BlockSize const& copy) throw();
    
    BlockSize (BlockSizeDefault const& copy) throw();
    BlockSize (BlockSizeNoPreference const& copy) throw();
        
    /** Create a block size converting from another type.
     This is useful for setting up ratios for both SampleRate and
     BlockSize objects where former holds a double type and the latter holds
     an integer type. */
    template<class OtherType>
    BlockSize (OtherType const& initValue) throw()
    :   Base (initValue)
    {
    }
    
    /** Gets the default block size.
     In most situations this will be the hardware block size.
     This defaults to 512, to update it you should do this early in your
     initialisation code (before creating any units).
     @code
        BlockSize::getDefault().setValue(newBlockSize)
     @endcode */
    static BlockSize& getDefault() throw();
    
    /** Gets the default control rate block size.
     This is currently 1. */
    static BlockSize& getControlRateBlockSize() throw();
    
    /** Create a block size that is a fraction of the default block size.
     This will automatically update when/if the default block size is changed. */
    static const BlockSize getFractionOfDefault (IntVariable const& divisor) throw();
    
    /** Create a block size that is a multiple of the default block size.
     This will automatically update when/if the default block size is changed. */
    static const BlockSize getMultipleOfDefault (IntVariable const& factor) throw();    
    PLONK_INLINE_LOW static const BlockSize getMultipleOfDefault (const int factor) throw() { return getMultipleOfDefault (IntVariable (factor)); }
   
    static const BlockSize getMultipleOfDefault (DoubleVariable const& factor) throw();    
    PLONK_INLINE_LOW static const BlockSize getMultipleOfDefault (const double factor) throw() { return getMultipleOfDefault (DoubleVariable (factor)); }

    
    /** A dummy block size used to indicate no preference.
     Usually this will mean a unit being initialised with "no preference" on 
     block size will use the largest block size it finds in its inputs. */
    static const BlockSize& noPreference() throw();
    
    /** Get a block size of 1.
     NB This is currently the same size as the default control rate block
     size but are two separate copies. */
    static const BlockSize& getBlockSize1() throw();
    
    /** Select a block size based on a unit's preference and its inputs.
     Here the @c preferredBlockSize will be return UNLESS it indicates "no
     preference". If the inputs have "no preference" then the default
     block size is returned. */
    static BlockSize decide (BlockSize const& inputBlockSize, 
                             BlockSize const& preferredBlockSize) throw();
    
    PLONK_OBJECTARROWOPERATOR(BlockSize);

private:
    static const BlockSize getMultipleOfDefault (SampleRate const& factor) throw();    
};

typedef NumericalArray<BlockSize> BlockSizes;

//------------------------------------------------------------------------------

class BlockSizeDefault : public BlockSize
{
public:
    PLONK_INLINE_LOW BlockSizeDefault() : BlockSize (BlockSize::getDefault()) { }
};

//------------------------------------------------------------------------------

class BlockSizeNoPreference : public BlockSize
{
public:
    PLONK_INLINE_LOW BlockSizeNoPreference() : BlockSize (BlockSize::getDefault()) { }
};




#endif // PLONK_BLOCKSIZE_H