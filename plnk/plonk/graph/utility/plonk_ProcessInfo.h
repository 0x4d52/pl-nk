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

#ifndef PLONK_PROCESSINFO_H
#define PLONK_PROCESSINFO_H

#include "../../core/plonk_CoreForwardDeclarations.h"
#include "../../containers/plonk_ContainerForwardDeclarations.h"
#include "../../core/plonk_SmartPointerContainer.h"
#include "../../core/plonk_WeakPointerContainer.h"
#include "../../core/plonk_SenderContainer.h"
#include "../../core/plonk_Receiver.h"
#include "../utility/plonk_TimeStamp.h"


/** Holds information about the Unit graph.
 This keeps track of the current time and whether a subgraph needs to be deleted. 
 The time is incremented on each block processed. 
 
 The "time stamp" is incremented  after each block 
 to ensure that it uniquely identifies the start sample of each 
 block at all block sizes in the graph. 
 @see TimeStamp, UnitBase, ChannelBase */
class ProcessInfo : public SenderContainer<ProcessInfoInternal>
{
public:
    typedef ProcessInfoInternal                 Internal;
    typedef SenderContainer<Internal>           Base;
    typedef ReceiverInternal<ProcessInfo>       Receiver;
    typedef ProcessInfo                         Sender;
    typedef WeakPointerContainer<ProcessInfo>   Weak;        
    
    ProcessInfo (const TimeStamp timeStamp = TimeStamp::getZero(),
                 const bool shouldDelete = false) throw();
    
    explicit ProcessInfo (Internal* internal) throw();
    
    ProcessInfo (ProcessInfo const& copy) throw();
    
    /** Get the current time stamp. */    
    const TimeStamp& getTimeStamp() const throw();
    
    /** Sets the current time stamp.
     Use with caution. */
    void setTimeStamp (const TimeStamp newSampleNumber) throw();
    
    void offsetTimeStamp (const double offset) throw();
    void setShouldDelete() throw();
    void resetShouldDelete() throw();
    bool getShouldDelete() const throw();
        
    PLONK_OBJECTARROWOPERATOR(ProcessInfo);
};


//------------------------------------------------------------------------------


//PLONK_INLINE_LOW const TimeStamp& ProcessInfo::getTimeStamp() const throw()             
//{ 
//    return this->getInternal()->getTimeStamp(); 
//}
//
//PLONK_INLINE_LOW void ProcessInfo::setTimeStamp (const TimeStamp timeStamp) throw()
//{
//    this->getInternal()->setTimeStamp (timeStamp);
//}
//
//PLONK_INLINE_LOW void ProcessInfo::offsetTimeStamp (const double offset) throw()
//{
//    this->getInternal()->offsetTimeStamp (offset);
//}
//
//PLONK_INLINE_LOW void ProcessInfo::setShouldDelete() throw()
//{
//    this->getInternal()->setShouldDelete();
//}
//
//PLONK_INLINE_LOW void ProcessInfo::resetShouldDelete() throw()
//{
//    this->getInternal()->resetShouldDelete();
//}
//
//PLONK_INLINE_LOW bool ProcessInfo::getShouldDelete() const throw()
//{
//    return this->getInternal()->getShouldDelete();
//}


#endif // PLONK_PROCESSINFO_H