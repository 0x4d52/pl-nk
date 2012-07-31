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

class MyThread : public Threading::Thread
{
public:
    MyThread (UnitVariable const& srcref)
    :   src (srcref)
    {
    }
    
    ResultCode run()
    {
        while (!getShouldExit())
        {
            Threading::sleep (rand (0.001, 2.0));
            
            Unit unit = Sine::ar (exprand (500, 2000), 0.1);
            src.swapValues (unit);
        }
        
        return 0;
    }
    
private:
    UnitVariable src;
};

AudioHost::AudioHost()
{
//    thread1 = new MyThread (src);
//    thread2 = new MyThread (src);
}

AudioHost::~AudioHost()
{
//    thread1->setShouldExitAndWait();
//    delete thread1;
//    thread2->setShouldExitAndWait();
//    delete thread2;
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
//    return delay.mix();   

    
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
    
//    Unit input = BusRead::ar (Bus ("0"));
//    return CombDecay::ar (input, Floats (0.03, 0.0299), 10.0); 

//    Unit input = BusRead::ar (Bus ("0"));
//    return AllpassDecay::ar (input, Floats (0.03, 0.0299), 10.0); 

//    Unit input = BusRead::ar (Bus ("0"));
//    return CombFB::ar (input, Floats (0.03, 0.0299), 0.5);; 

//    Unit input = BusRead::ar (Bus ("0"));
//    return AllpassFFFB::ar (input, Floats (0.03, 0.0299), 0.5); 

    
//    Unit input = BusRead::ar (Bus ("0"));
//    return Delay::ar (input, Floats (0.25, 0.5), 1.0);
    
//    Unit input = BusRead::ar (Bus ("0"));
//    const int size = 50;
//    Floats times = Floats::rand (size, 0.001, 0.05);
//    times.print("times");
//    const float maximumTime = times.findMaximum();
//    printf ("maxtime: %f\n", maximumTime);
//    Unit combs = CombDecay::ar (input, times, 5, maximumTime);
//    return Mixer::ar (combs.group (2)) * (2.f / size);
//    return combs.mix() * (2.f / size);
    
//    Unit input = BusRead::ar (Bus ("0"));
    
//    Unit input = Sine::ar (50, Sine::ar (0.5).linlin (0, 0.2));
//    const float time = 0.05;
//    Unit output = CombDecay::ar (input, Floats (time, time), 5, time);
//    output.add (input);
//    return output;
    
//    Unit input = Sine::ar (50, Sine::ar (0.5).linlin (0, 0.2));
//    const float time = 0.05;
//    Unit output = Delay::ar (input, Floats (time, time), time);
////    output.add (input);
//    return output;
    
//    Unit input = BusRead::ar (Bus ("0"));
//    Unit delay = Delay::ar (input, 
//                            Floats (0.2, 0.4, 0.6, 0.8), 
//                            1.0);
//    return delay; 

//    Unit input = BusRead::ar (Bus ("0"));    
//    Unit combs = CombDecay::ar(input, Floats::rand (16, 0.0001, 0.05), 0.05);
//    
//    Unit output = Mixer::ar (combs.group (2));
//    
//    for (int i = 0; i < 40; ++i)
//        output = AllpassDecay::ar (output, Floats::rand (2, 0.0001, 0.05), 10, 0.05);
//    
//    return output;

//    Unit input = BusRead::ar (Bus ("0"));    
//    Unit combs = CombDecay::ar(input, Floats::rand (16, 0.0001, 0.05), 0.05);
//    
//    Unit output = Mixer::ar (combs.group (2));
//    
//    for (int i = 0; i < 20; ++i)
//        output = AllpassDecay::ar (output, Floats::rand (2, 0.0001, 0.001), i * 0.5, 0.05);
//    
//    for (int i = 20; i >= 0; --i)
//        output = AllpassDecay::ar (output, Floats::rand (2, 0.005, 0.05), i * 0.5, 0.05);
//
//    return output;
    
//    Unit input = BusRead::ar (Bus ("0"));
//    return CombLPF::ar (input, Floats (0.0101, 0.00999), 0.95, 1200.0, 1.0);
    
//    Unit input = BusRead::ar (Bus ("0"));
//    const int size = 12;
//    Unit combs = CombLPF::ar (input, 
//                              Floats::exprand (size, 0.01, 0.5), 
//                              Floats::exprand (size, 0.5, 0.999), 
//                              Floats::exprand (size, 100, 10000), 
//                              1.0);
//    return Mixer::ar (combs.group (2));

    
//    Unit output = BusRead::ar (Bus ("0"));
//    for (int i = 0; i < 6; ++i)
//        output = CombLPF::ar (output, 
//                              Floats::exprand (2, 0.01, 0.5), 
//                              Floats::exprand (2, 0.5, 0.999), 
//                              Floats::exprand (2, 100, 10000), 
//                              1.0);
//    return output;
    
//    Unit input = BusRead::ar (Bus ("0"));
//    return CombLPFP1::ar (input, 0.25, 0.95, 10000.0, 1.0);

//    Unit input = BusRead::ar (Bus ("0"));
//    return CombLPF::ar (input, 0.25, 0.95, 10000.0, 1.0);

//    Unit input = BusRead::ar (Bus ("0"));
//    return CombHPF::ar (input, 0.25, 0.95, 1000, 1.0);


//    Unit input = BusRead::ar (Bus ("0"));
//    return AllpassDecay::ar (input, Floats::rand (2, 0.05), 10.0); 
    
//    return WhiteNoise::ar (0.1);
    
    
//    int val = 0x2000;
//    int count = Bits<int>::countTrailingZeroes (val);
//    printf ("count (%d) = %d\n", val, count);
    
//    return WhiteNoise::ar (Floats (0.1, 0.1));
    
//    return RLPF::ar(WhiteNoise::ar(), Sine::ar(0.1).linexp(50, 10000), Sine::ar(0.9999).linexp(1, 10), 0.2);
    
//    return WhiteNoise::ar (Floats::series (50, 0.000001, 0.000001)).mix() * 0;

//    return WhiteNoise::ar (Floats (0.1, 0.1)) * Saw::ar(-1);

//    Dynamic dyn;
//    AtomicDynamicPointer adyn = &dyn;
//    dyn = Sine::ar (1000, 0.1);
//    return adyn.getObject().as<Unit>();

//    Unit source = Sine::ar (1000, 0.1);
//    static PatchSource<Unit> patch (source);    
//    Unit unit = Patch::ar (patch);
//    return unit;
    
//    return Sine::ar (Floats(500, 2000), 0.25);
//    return Sine::ar (Floats(1000, 1000), 0.25);
    
//    AtomicIntVariable intvar;
//    
//    Unit src1 = Sine::ar (1000);
//    Unit src2 = WhiteNoise::ar();
//    
//    UnitVariable var1;
//    UnitVariable var2;
//    
//    var1.setValue (src1);
//    var2.setValue (src2);
//    
//    var1.swapValues (var2);
//    
//    return var1.getValue() * 0.1;
    
//    Unit src = Sine::ar (1000, 0.1);
//    Unit patch = Patch::ar (src);
//    return src;
    
//    Unit patch = Patch::ar (src);
//    thread1->start();
//    thread2->start();
//    return patch;
    
//    Units units;
//    return Mixer::ar (units);
    
//    return Sine::ar (1000, 0.1) * AtomicVariableUnit<float>::ar (gate);

    
    return Patch::ar (src, false);
    
//    const int NUM_SINES = 60;
//    return Sine::ar (Float1D::exprand(NUM_SINES, 100, 1000)).mix() * (0.25f / NUM_SINES);
}

void AudioHost::setGate (const bool state) throw() 
{ 
    gate.setValue (state ? 1.f : 0.f); 
    
    if (state)
    {
        Unit sine = Sine::ar (exprand (880, 1760), 0.25);
        //        Unit gateUnit = AtomicVariableUnit<float>::ar (gate);
        Unit gateUnit = AtomicVariableUnit<float>::kr (gate);

        Unit env = Envelope::ar (Breakpoints::adsr (1.0, 1.0, 0.7, 2.0), gateUnit);
        Unit newSource = sine * env;
        src.swapValues (newSource);
    }
}





