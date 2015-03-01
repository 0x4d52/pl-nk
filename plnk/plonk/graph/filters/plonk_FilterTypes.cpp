/*
 -------------------------------------------------------------------------------
 This file is part of the Plink, Plonk, Plank libraries
  by Martin Robinson
 
 http://code.google.com/p/pl-nk/
 
 Copyright University of the West of England, Bristol 2011-14
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


FilterFormType::Name FilterFormType::fromInt (const int value) throw()
{
    static const FilterFormType::Name names[] = {
        FilterFormType::Unknown,
        FilterFormType::P1,
        FilterFormType::P1a,
        FilterFormType::P1b,
        FilterFormType::P1c,
        FilterFormType::P2,
        FilterFormType::Z1,
        FilterFormType::Z2,
        FilterFormType::P1Z1,
        FilterFormType::P2Z2,
        FilterFormType::B2
    };
    
    if (value < 0 || value >= FilterFormType::NumNames)
    {
        plonk_assertfalse;
        return FilterFormType::Unknown;
    }
    else
    {
        return names[value];
    }
}

Text FilterFormType::getName (const int index) throw()
{
    static const char* names[] = {
        "Unknown",
        "One-Pole",
        "One-Pole [simplified form]",
        "One-Pole [DC removal form]",
        "One-Pole [asymmetic]",
        "Two-Pole",
        "One-Zero",
        "Two-Zero",
        "One-Pole One-Zero", 
        "Two-Pole Two-Zero",
        "Butterworth 2nd Order"
    };
    
    if (index < 0 || index >= FilterFormType::NumNames)
        return "Unknown";
    else
        return names[index];                
}

//------------------------------------------------------------------------------

FilterShapeType::Name FilterShapeType::fromInt (const int value) throw()
{
    static const FilterShapeType::Name names[] = {
        FilterShapeType::Unknown,
        FilterShapeType::LPF,
        FilterShapeType::HPF,
        FilterShapeType::BPF,
        FilterShapeType::BRF,
        FilterShapeType::LowShelf,
        FilterShapeType::HighShelf,
        FilterShapeType::Notch,
        FilterShapeType::Allpass,
        FilterShapeType::DC,
        FilterShapeType::Other
    };
    
    if (value < 0 || value >= FilterShapeType::NumNames)
    {
        plonk_assertfalse;
        return FilterShapeType::Unknown;
    }
    else
    {
        return names[value];
    }
}

Text FilterShapeType::getName (const int index) throw()
{
    static const char* names[] = {
        "Unknown",
        "LPF",
        "HPF",
        "BPF",
        "BRF",
        "LowShelf",
        "HighShelf",
        "Notch",
        "Allpass",
        "DC",
        "Other"
    };
    
    if (index < 0 || index >= FilterShapeType::NumNames)
        return "Unknown";
    else
        return names[index];                
}




END_PLONK_NAMESPACE