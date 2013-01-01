/*
 -------------------------------------------------------------------------------
 This file is part of the Plink, Plonk, Plank libraries
  by Martin Robinson
 
 http://code.google.com/p/pl-nk/
 
 Copyright University of the West of England, Bristol 2011-13
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

#ifndef PLONK_IOINFO_H
#define PLONK_IOINFO_H

#include "plonk_InfoHeaders.h"

class ChannelCount
{
public:
    enum Count
    {
        InvalidChannelCount = -2,
        VariableChannelCount = -1,
    };
    
    static int fromInt (const int index) throw();
    static Text getName (const int index) throw();
    static int numOutputInfos (const int index) throw();
};

//------------------------------------------------------------------------------

class IOInfo
{
public:
    
    IOInfo (const IOKey::Name name = IOKey::Invalid, 
            const Measure::Name measure = Measure::Unknown,
            const double defaultValue = IOInfo::NoDefault,
            const IOLimit limit = IOLimit::createNone()) throw();
    
    const IOKey::Name getNameKey() const throw();
    const Measure::Name getMeasureKey() const throw();
    bool hasDefaultValue() const throw();
    double getDefaultValue() const throw();
    const IOLimit& getLimit() const throw();
    
    bool hasMeasure() const throw();
    
    const Text getName() const throw();
    const Text getMeasureName() const throw();
    const Text getMeasureSymbol() const throw();
    
    static double NoDefault;
    static double True;
    static double False;
    
private:
    IOKey::Name name;
    Measure::Name measure;
    double defaultValue;
    IOLimit limit;
};




#endif // PLONK_IOINFO_H