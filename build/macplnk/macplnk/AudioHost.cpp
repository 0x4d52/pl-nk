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

#include "AudioHost.h"

AudioHost::AudioHost()
{
}

Unit AudioHost::constructGraph()
{
//    Unit input = BusRead::ar (Bus ("0"));   // busses 0 and 1 are left and right
//    Unit mod = Sine::ar (500);              // 500Hz sine
//    return input * mod;                     // ringmod
    
//    Floats freq (1000, 1555);
//    return Sine::ar (freq, 0.25);
    
//    return LinearPan::ar (Sine::ar (1000, 0.25), Sine::kr (0.5));
    
//    Unit input = BusRead::ar (Bus ("0"));
//    Unit delay1 = Delay::ar (input, Sine::ar (5).linlin (0.125, 0.12525), 0.5);
//    Unit delay2 = Delay::ar (input, Sine::kr (5).linlin (0.25, 0.2525).ar(), 0.5);
//    Unit delay3 = Delay::ar (input, 0.5, 0.5);
//    return input + delay1 * 0.5 + delay2 * 0.25 + delay3 * 0.125;
    
//    Unit input = BusRead::ar (Bus ("0"));
//    Unit delay = Delay::ar (input, 
//                            Floats (0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8), 
//                            1.0,
//                            Floats (0.5, 0.5, 0.4, 0.4, 0.3, 0.3, 0.2, 0.2));
//    
//    Unit delay2;
//    
//    delay2.add (delay[0]);
//    delay2.add (delay[1]);
//    delay2.add (delay[2]);
//    delay2.add (delay[3]);
//    delay2.add (delay[4]);
//    delay2.add (delay[5]);
//    delay2.add (delay[6]);
//    delay2.add (delay[7]);
//    
//    Units group = delay2.group (2);
//    return Mixer::ar (group);    
    
//    Unit input = BusRead::ar (Bus ("0"));
//    Unit delay = Delay::ar (input, 
//                            Floats (0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8), 
//                            1.0,
//                            Floats (0.5, 0.5, 0.4, 0.4, 0.3, 0.3, 0.2, 0.2));
//    Units group = delay.group (2);
//    return Mixer::ar (group);    

//    Unit input = BusRead::ar (Bus ("0"));
//    Unit delay = Delay::ar (input, 
//                            Floats (0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8), 
//                            1.0,
//                            Floats (0.5, 0.5, 0.4, 0.4, 0.3, 0.3, 0.2, 0.2));
//    return Mixer::ar (delay.group (2));    
    
    Unit input = BusRead::ar (Bus ("0"));
    return CombDecay::ar (input, Floats (0.049, 0.051), 10.0);    

}


