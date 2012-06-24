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

//#import "AudioHost.h"
//
//@implementation AudioHost
//
//// override constructGraph to return your own unit graph, 
//// inputs can be accessed via Busses
//
//- (Unit)constructGraph
//{        
//    Unit input = BusRead::ar (Bus ("0"));   // busses 0 and 1 are left and right
//    Unit mod = Sine::ar (500);              // 500Hz sine
//    return input * mod;                     // ringmod
//}
//
//@end

#include "AudioHost.h"

AudioHost::AudioHost()
{
}

Unit AudioHost::constructGraph()
{
//    Unit input = BusRead::ar (Bus ("0"));   // busses 0 and 1 are left and right
//    Unit mod = Sine::ar (500);              // 500Hz sine
//    return input * mod;                     // ringmod
    
//    return Sine::ar (1000, 0.25);
    
    Unit input = BusRead::ar (Bus ("0"));   // busses 0 and 1 are left and right
    Unit mod = Sine::ar (500);              // 500Hz sine
    Unit ringmod = input * mod;             // ringmod
    Unit fmod = Sine::ar (0.2).linexp (500, 7500); 
    Unit filter = RLPF::ar (ringmod, fmod, 5);
    return filter;
}


