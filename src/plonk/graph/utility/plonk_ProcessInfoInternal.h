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

#ifndef PLONK_PROCESSINFOINTERNAL_H
#define PLONK_PROCESSINFOINTERNAL_H

#include "../../core/plonk_CoreForwardDeclarations.h"
#include "../../containers/plonk_ContainerForwardDeclarations.h"
#include "../../core/plonk_Sender.h"
#include "../utility/plonk_ProcessInfo.h"

class ProcessInfoInternal : public SenderInternal<ProcessInfo>
{
public:
    ProcessInfoInternal (const TimeStamp timeStamp,
                         const bool shouldDelete) throw();
    
    inline const TimeStamp& getTimeStamp() const throw() { return timeStamp; }
    void setTimeStamp (const TimeStamp newTimeStamp) throw();
    void offsetTimeStamp (const double offset) throw();
    void setShouldDelete() throw();
    void resetShouldDelete() throw();
    bool getShouldDelete() const throw() { return shouldDelete; }
    
private:
    TimeStamp timeStamp;
    bool shouldDelete;
    
    ProcessInfoInternal();
};


//------------------------------------------------------------------------------


inline void ProcessInfoInternal::setTimeStamp (const TimeStamp time) throw()
{
    timeStamp = time;
}

inline void ProcessInfoInternal::offsetTimeStamp (const double offset) throw()
{
    timeStamp += offset;
}

inline void ProcessInfoInternal::setShouldDelete() throw()
{
    shouldDelete = true;
}

inline void ProcessInfoInternal::resetShouldDelete() throw()
{
    shouldDelete = false;
}




#endif // PLONK_PROCESSINFOINTERNAL_H