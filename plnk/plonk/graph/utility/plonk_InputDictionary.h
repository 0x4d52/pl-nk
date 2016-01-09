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

#ifndef PLONK_INPUTDICTIONARY_H
#define PLONK_INPUTDICTIONARY_H

#include "../plonk_GraphForwardDeclarations.h"
#include "../../containers/plonk_DynamicContainer.h"
#include "../../maths/plonk_InlineCommonOps.h"
#include "../utility/plonk_BlockSize.h"
#include "../utility/plonk_SampleRate.h"


/** For storing named Dynamic objects.
 This is used by Channel classes to receive inputs as a dictionary of
 named items. */
class InputDictionary : public Dictionary<Dynamic,int>
{
public:
    typedef WeakPointerContainer<InputDictionary>    Weak;
    typedef Dictionary<Dynamic,int>                  Base;
    
    InputDictionary() throw();
    
    /** Copy constructor. */
    InputDictionary (InputDictionary const& copy) throw();
    
    /** Find the minimum Unit block size in this dictionary. 
     This is not recursive but does search each channel of the units. */
    const BlockSize getMinBlockSize() const throw();
    
    /** Find the maximum Unit block size in this dictionary. 
     This is not recursive but does search each channel of the units. */
    const BlockSize getMaxBlockSize() const throw();
    
    const SampleRate getMinSampleRate() const throw();
    const SampleRate getMaxSampleRate() const throw();
    
    /** Returns this dictionary with all the units containing only a single channel.
     @param index The index of the channel to obtain from all the units. */
    InputDictionary getChannel (const int index) throw();
    
    /** Find the maximum number of channels in this dictionary (i.e., in unit, units and busses). 
     This is not recursive. */
    int getMaxNumChannels() const throw();
    
    void resetExpiredUnits() throw();
    
    PLONK_OBJECTARROWOPERATOR(InputDictionary);
};



#endif // PLONK_UNITDICTIONARY_H