/*
 -------------------------------------------------------------------------------
 This file is part of the Plink, Plonk, Plank libraries
  by Martin Robinson
 
 http://code.google.com/p/pl-nk/
 
 Copyright University of the West of England, Bristol 2011-15
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

#ifndef PLONK_LIMIT_H
#define PLONK_LIMIT_H

#include "plonk_InfoHeaders.h"



class IOLimit
{
public:
    enum LimitType
    {
        None = 0, 
        Minimum,
        Maximum,
        Clipped,
        
        NumLimitTypes
    };
    
    static IOLimit createNone() throw();
    static IOLimit createMinimum (Measure::Name measure, const double minimum) throw();
    static IOLimit createMaximum (Measure::Name measure, const double maximum) throw();
    static IOLimit createClipped (Measure::Name measure, const double minimum, const double maximum) throw();
        
    static IOLimit::LimitType fromInt (const int value) throw();
    static Text getName (const int keyIndex) throw();
    static bool isNone (const IOLimit::LimitType type) throw();
    static bool isClipped (const IOLimit::LimitType type) throw();
    static bool hasMinimum (const IOLimit::LimitType type) throw();
    static bool hasMaximum (const IOLimit::LimitType type) throw();
    
    const Text getName() const throw();
    const Measure::Name getMeasure() const throw();
    bool isNone() const throw();
    bool isClipped() const throw();
    bool hasMinimum() const throw();
    bool hasMaximum() const throw();
    double getMinimum() const throw();
    double getMaximum() const throw();

private:
    IOLimit::LimitType type;
    Measure::Name measure;
    double minimum;
    double maximum;
    
    IOLimit (const IOLimit::LimitType type, 
             const Measure::Name measure,
             const double value1 = 0.0, 
             const double value2 = 0.0) throw();
    
};



#endif // PLONK_LIMIT_H