/*
 -------------------------------------------------------------------------------
 This file is part of the Plink, Plonk, Plank libraries
  by Martin Robinson
 
 http://code.google.com/p/pl-nk/
 
 Copyright University of the West of England, Bristol 2011-12
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

#include "../../core/plonk_StandardHeader.h"

BEGIN_PLONK_NAMESPACE

#include "../../core/plonk_Headers.h"

BlockSize::BlockSize() throw()
:   Base (0)
{
}

BlockSize::BlockSize (SampleRate const& sampleRate) throw()
:   Base (0.0)
{
    // you must have got your arguments mixed up, perhaps you got
    // sample rate and block size arguments the wrong way round?
    (void)sampleRate;
    plonk_assertfalse;
}

BlockSize::BlockSize (const int initValue) throw()
:   Base (initValue)
{
    plonk_assert (this->getValue() > 0);
}

BlockSize::BlockSize (IntVariable const& initValue) throw()
:   Base (initValue)
{
    plonk_assert (this->getValue() > 0);
}

BlockSize::BlockSize (BlockSize const& copy) throw()
:	Base (static_cast<Base const&> (copy))
{
}        

BlockSize BlockSize::getDefault() throw()
{
    static BlockSize blockSize (512);
    return blockSize;
}

BlockSize BlockSize::getControlRateBlockSize() throw()
{
    static BlockSize blockSize (1); // temporary solution
    return blockSize;
}

const BlockSize BlockSize::getFractionOfDefault (IntVariable const& divisor) throw()
{
    return getDefault() / divisor;
}

const BlockSize BlockSize::getMultipleOfDefault (IntVariable const& factor) throw()
{
    return getDefault() * factor;
}

const BlockSize BlockSize::getMultipleOfDefault (DoubleVariable const& factor) throw()
{
    return Variable<double> (getDefault()) * factor;
}

const BlockSize& BlockSize::noPreference() throw()
{
    static const BlockSize sentinel; // 0
    return sentinel;
}

const BlockSize& BlockSize::getBlockSize1() throw()
{
    static BlockSize blockSize (1);
    return blockSize;
}

BlockSize BlockSize::decide (BlockSize const& inputBlockSize, 
                             BlockSize const& preferredBlockSize) throw()
{
    const BlockSize blockSize ((preferredBlockSize.getValue() == 0) ? inputBlockSize : preferredBlockSize);
    return (blockSize.getValue() == 0) ? getDefault() : blockSize;
}

END_PLONK_NAMESPACE
