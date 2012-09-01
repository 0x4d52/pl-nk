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

#include "../../core/plink_StandardHeader.h"
#include "plink_Saw.h"

//void plink_SawProcessF_NN (void* ppv, SawProcessStateF* state)
//{
//    PlinkProcessF* pp;
//    int i, N;
//    float sampleDuration, currentValue, peak, peak2peak, factor;
//    float *output;
//    float *freq;
//    
//    pp = (PlinkProcessF*)ppv;
//    
//    sampleDuration = (float)state->base.sampleDuration;    
//    currentValue = state->currentValue;
//    peak = state->peak;
//    peak2peak = state->peak2peak;
//    factor = peak2peak * sampleDuration;
//    
//    N = pp->buffers[0].bufferSize;
//    output = pp->buffers[0].buffer;
//    freq = pp->buffers[1].buffer;
//    
//    for (i = 0; i < N; ++i) 
//    {
//        output[i] = currentValue;
//        currentValue += freq[i] * factor;
//        
//        if (currentValue >= peak)
//            currentValue -= peak2peak;
//        else if (currentValue < -peak)
//            currentValue += peak2peak;
//    }    
//    
//    state->currentValue = currentValue;
//}
//
//void plink_SawProcessF_N1 (void* ppv, SawProcessStateF* state)
//{
//    PlinkProcessF* pp;
//    int i, N;
//    float sampleDuration, currentValue, peak, peak2peak, factor, valueIncrement;
//    float *output;
//    
//    pp = (PlinkProcessF*)ppv;
//    
//    sampleDuration = (float)state->base.sampleDuration;    
//    currentValue = state->currentValue;
//    peak = state->peak;
//    peak2peak = state->peak2peak;
//    factor = peak2peak * sampleDuration;
//    
//    N = pp->buffers[0].bufferSize;
//    output = pp->buffers[0].buffer;
//    valueIncrement = pp->buffers[1].buffer[0] * factor;
//    
//    if (valueIncrement > 0.f)
//    {
//        for (i = 0; i < N; ++i) 
//        {
//            output[i] = currentValue;
//            currentValue += valueIncrement;
//            
//            if (currentValue >= peak)
//                currentValue -= 2.f;
//        }  
//    }
//    else 
//    {
//        for (i = 0; i < N; ++i) 
//        {
//            output[i] = currentValue;
//            currentValue += valueIncrement;
//            
//            if (currentValue < -peak)
//                currentValue += 2.f;
//        }  
//
//    }
//    
//    state->currentValue = currentValue;    
//}
//
//void plink_SawProcessF_Nn (void* ppv, SawProcessStateF* state)
//{
//    PlinkProcessF* pp;
//    int i, N;
//    float sampleDuration, currentValue, peak, peak2peak, factor;
//    float *output;
//    float *freq;
//    double freqPos, freqInc;
//
//    pp = (PlinkProcessF*)ppv;
//    
//    sampleDuration = (float)state->base.sampleDuration;    
//    currentValue = state->currentValue;
//    peak = state->peak;
//    peak2peak = state->peak2peak;
//    factor = peak2peak * sampleDuration;
//    
//    N = pp->buffers[0].bufferSize;
//    output = pp->buffers[0].buffer;
//    freq = pp->buffers[1].buffer;
//    
//    freqPos = 0.0;
//    freqInc = (double)pp->buffers[1].bufferSize / (double)N;
//    
//    for (i = 0; i < N; ++i) 
//    {
//        output[i] = currentValue;
//        currentValue += freq[(int)freqPos] * factor;
//        
//        if (currentValue >= peak)
//            currentValue -= 2.f;
//        else if (currentValue < -peak)
//            currentValue += 2.f;
//        
//        freqPos += freqInc;
//    }    
//    
//    state->currentValue = currentValue;    
//}

//void plink_SawProcessF_NN (void* ppv, SawProcessStateF* state)
//{
//    PlinkProcessF* pp;
//    int i, N;
//    float sampleDuration, currentValue, peak2peak, factor;
//    float *output;
//    float *freq;
//    float temp;
//    
//    pp = (PlinkProcessF*)ppv;
//    
//    sampleDuration = (float)state->base.sampleDuration;    
//    currentValue = state->currentValue + 1.f;    // pre-inc for the clip function
//    peak2peak = state->peak2peak;
//    factor = peak2peak * sampleDuration;
//    
//    N = pp->buffers[0].bufferSize;
//    output = pp->buffers[0].buffer;
//    freq = pp->buffers[1].buffer;
//    
//    // need a vector scalar multiply, scalar add
//    pl_VectorMulF_NN1 (output, freq, factor, N);
//    
//    for (i = 0; i < N; ++i) 
//    {
//        temp = output[i];
//        output[i] = currentValue;
//        currentValue = pl_Clip02F (currentValue + temp);        
//    }    
//    
//    pl_VectorAddF_NN1 (output, output, -1.f, N); // post decrement after the clip function
//    state->currentValue = currentValue - 1.f;    // post decrement after the clip function
//}
//
//void plink_SawProcessF_N1 (void* ppv, SawProcessStateF* state)
//{
//    PlinkProcessF* pp;
//    int i, N;
//    float sampleDuration, currentValue, peak2peak, factor, valueIncrement;
//    float *output;
//    
//    pp = (PlinkProcessF*)ppv;
//    
//    sampleDuration = (float)state->base.sampleDuration;    
//    currentValue = state->currentValue + 1.f;                 // pre-inc for the clip function
//    peak2peak = state->peak2peak;
//    factor = peak2peak * sampleDuration;
//    
//    N = pp->buffers[0].bufferSize;
//    output = pp->buffers[0].buffer;
//    valueIncrement = pp->buffers[1].buffer[0] * factor;
//    
//    for (i = 0; i < N; ++i) 
//    {
//        output[i] = currentValue;
//        currentValue = pl_Clip02F (currentValue + valueIncrement);        
//    }    
//    
//    pl_VectorAddF_NN1 (output, output, -1.f, N);              // post decrement after the clip function
//    state->currentValue = currentValue - 1.f;    
//}
//
//void plink_SawProcessF_Nn (void* ppv, SawProcessStateF* state)
//{
//    PlinkProcessF* pp;
//    int i, N;
//    float sampleDuration, currentValue, peak2peak, factor;
//    float *output;
//    float *freq;
//    double freqPos, freqInc;
//    
//    pp = (PlinkProcessF*)ppv;
//    
//    sampleDuration = (float)state->base.sampleDuration;    
//    currentValue = state->currentValue + 1.f;                 // pre-inc for the clip function
//    peak2peak = state->peak2peak;
//    factor = peak2peak * sampleDuration;
//    
//    N = pp->buffers[0].bufferSize;
//    output = pp->buffers[0].buffer;
//    freq = pp->buffers[1].buffer;
//    
//    freqPos = 0.0;
//    freqInc = (double)pp->buffers[1].bufferSize / (double)N;
//    
//    for (i = 0; i < N; ++i) 
//    {
//        output[i] = currentValue;
//        currentValue = pl_Clip02F (currentValue + freq[(int)freqPos] * factor);
//        freqPos += freqInc;
//    }    
//    
//    pl_VectorAddF_NN1 (output, output, -1.f, N);              // post decrement after the clip function
//    state->currentValue = currentValue - 1.f;    
//}

void plink_SawProcessF_NN (void* ppv, SawProcessStateF* state)
{
    PlinkProcessF* pp;
    int i, N;
    float sampleDuration, currentValue, peak2peak, factor;
    float *output;
    float *freq;
    float temp;
    
    pp = (PlinkProcessF*)ppv;
    
    sampleDuration = (float)state->base.sampleDuration;    
    currentValue = state->currentValue + 3.f;    // pre-inc for the clip function
    peak2peak = state->peak2peak;
    factor = peak2peak * sampleDuration;
    
    N = pp->buffers[0].bufferSize;
    output = pp->buffers[0].buffer;
    freq = pp->buffers[1].buffer;
    
    pl_VectorMulF_NN1 (output, freq, factor, N);
    
    for (i = 0; i < N; ++i) 
    {
        temp = output[i];
        output[i] = currentValue;
        currentValue = pl_Wrap24F (currentValue + temp);  
    }    
    
    pl_VectorAddF_NN1 (output, output, -3.f, N); // post decrement after the clip function
    state->currentValue = output[N-1];
}

void plink_SawProcessF_N1 (void* ppv, SawProcessStateF* state)
{
    PlinkProcessF* pp;
    int i, N;
    float sampleDuration, currentValue, peak2peak, factor, valueIncrement;
    float *output;
    
    pp = (PlinkProcessF*)ppv;
    
    sampleDuration = (float)state->base.sampleDuration;    
    currentValue = state->currentValue + 3.f;                 // pre-inc for the clip function
    peak2peak = state->peak2peak;
    factor = peak2peak * sampleDuration;
    
    N = pp->buffers[0].bufferSize;
    output = pp->buffers[0].buffer;
    valueIncrement = pp->buffers[1].buffer[0] * factor;
    
    for (i = 0; i < N; ++i) 
    {
        output[i] = currentValue;
        currentValue = pl_Wrap24F (currentValue + valueIncrement);        
    }    
    
    pl_VectorAddF_NN1 (output, output, -3.f, N);              // post decrement after the clip function
    state->currentValue = output[N-1];    
}

void plink_SawProcessF_Nn (void* ppv, SawProcessStateF* state)
{
    PlinkProcessF* pp;
    int i, N;
    float sampleDuration, currentValue, peak2peak, factor;
    float *output;
    float *freq;
    double freqPos, freqInc;
    
    pp = (PlinkProcessF*)ppv;
    
    sampleDuration = (float)state->base.sampleDuration;    
    currentValue = state->currentValue + 3.f;                 // pre-inc for the clip function
    peak2peak = state->peak2peak;
    factor = peak2peak * sampleDuration;
    
    N = pp->buffers[0].bufferSize;
    output = pp->buffers[0].buffer;
    freq = pp->buffers[1].buffer;
    
    freqPos = 0.0;
    freqInc = (double)pp->buffers[1].bufferSize / (double)N;
    
    for (i = 0; i < N; ++i) 
    {
        output[i] = currentValue;
        currentValue = pl_Wrap24F (currentValue + freq[(int)freqPos] * factor);
        freqPos += freqInc;
    }    
    
    pl_VectorAddF_NN1 (output, output, -3.f, N);              // post decrement after the clip function
    state->currentValue = output[N-1];
}

void plink_SawProcessF (void* ppv, SawProcessStateF* state) 
{
    PlinkProcessF* pp;
    int N, n;
    
    pp = (PlinkProcessF*)ppv;
    N = pp->buffers[0].bufferSize;
    n = pp->buffers[1].bufferSize;
    
    if (n == N)         
        plink_SawProcessF_NN (ppv, state);
    else if (n == 1)    
        plink_SawProcessF_N1 (ppv, state);
    else                
        plink_SawProcessF_Nn (ppv, state);
}

