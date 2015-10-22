/*
 -------------------------------------------------------------------------------
 This file is part of the Plink, Plonk, Plank libraries
  by Martin Robinson
 
 https://github.com/0x4d52/pl-nk/
 
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

#include "../../core/plonk_StandardHeader.h"

BEGIN_PLONK_NAMESPACE

#include "../../core/plonk_Headers.h"


ProcessInfo::ProcessInfo (TimeStamp timeStamp,
                          bool shouldDelete) throw()
:   Base (new ProcessInfoInternal (timeStamp, shouldDelete))
{
}

ProcessInfo::ProcessInfo (Internal* internal) throw()
:  Base (internal)
{
}

ProcessInfo::ProcessInfo (ProcessInfo const& copy) throw()
:	Base (static_cast<Base const&> (copy))
{
}            

const TimeStamp& ProcessInfo::getTimeStamp() const throw()             
{ 
    return this->getInternal()->getTimeStamp(); 
}

void ProcessInfo::setTimeStamp (const TimeStamp timeStamp) throw()
{
    this->getInternal()->setTimeStamp (timeStamp);
}

void ProcessInfo::offsetTimeStamp (const double offset) throw()
{
    this->getInternal()->offsetTimeStamp (offset);
}

void ProcessInfo::setShouldDelete() throw()
{
    this->getInternal()->setShouldDelete();
}

void ProcessInfo::resetShouldDelete() throw()
{
    this->getInternal()->resetShouldDelete();
}

bool ProcessInfo::getShouldDelete() const throw()
{
    return this->getInternal()->getShouldDelete();
}

END_PLONK_NAMESPACE
