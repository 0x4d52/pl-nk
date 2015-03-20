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

#include "../../core/plink_StandardHeader.h"
#include "plink_Table.h"

void plink_TableProcessF_NN (void* ppv, TableProcessStateF* state)
{
    PlinkProcessF* pp;
    int i, N, M;
    unsigned index, mask;
    float fM;
    float factor, currentPosition;
    float frac;
    float *output;
    float *freq;
    float *table;
    float temp;
    
    pp = (PlinkProcessF*)ppv;
    
    currentPosition = state->currentPosition + state->fractionPosition;
    
    N = pp->buffers[0].bufferSize;
    M = pp->buffers[2].bufferSize;
    mask = M - 1;
    fM = (float)M;
    
    output = pp->buffers[0].buffer;
    freq = pp->buffers[1].buffer;
    table = pp->buffers[2].buffer;
    
    factor = (float)state->base.sampleDuration * M;    
    
    pl_VectorMulF_NN1 (output, freq, factor, N);
    
    for (i = 0; i < N; ++i) 
    {
        index = currentPosition;
        frac = currentPosition - index;

        temp = output[i];
        output[i] = pl_LinInterpF (table[index & mask], table[(index + 1) & mask], frac);
        currentPosition += temp;
    }                    
        
    index = currentPosition;
    state->currentPosition = index & mask;
    state->fractionPosition = currentPosition - index;
}

void plink_TableProcessF_N1 (void* ppv, TableProcessStateF* state)
{
    PlinkProcessF* pp;
    int i, N, M;
    unsigned index, mask;
    float fM;
    float currentPosition;
    float frac;
    float *output;
    float freqFactor;
    float *table;
    
    pp = (PlinkProcessF*)ppv;
    
    currentPosition = state->currentPosition + state->fractionPosition;
    
    N = pp->buffers[0].bufferSize;
    M = pp->buffers[2].bufferSize;
    mask = M - 1;
    fM = (float)M;
    
    output = pp->buffers[0].buffer;
    freqFactor = pp->buffers[1].buffer[0] * (float)state->base.sampleDuration * M;
    table = pp->buffers[2].buffer;

    for (i = 0; i < N; ++i) 
    {
        index = currentPosition;
        frac = currentPosition - index;
        
        output[i] = pl_LinInterpF (table[index & mask], table[(index + 1) & mask], frac);
        currentPosition += freqFactor;        
    }                    
        
    index = currentPosition;
    state->currentPosition = index & mask;
    state->fractionPosition = currentPosition - index;
}

void plink_TableProcessF_Nn (void* ppv, TableProcessStateF* state)
{
    PlinkProcessF* pp;
    int i, N, M;
    unsigned index, mask;
    float fM;
    float factor, currentPosition;
    float frac;
    float *output;
    float *freq;
    float *table;
    double freqPos, freqInc;
    
    pp = (PlinkProcessF*)ppv;
    
    currentPosition = state->currentPosition + state->fractionPosition;
    
    N = pp->buffers[0].bufferSize;
    M = pp->buffers[2].bufferSize;
    mask = M - 1;
    fM = (float)M;
    
    output = pp->buffers[0].buffer;
    freq = pp->buffers[1].buffer;
    table = pp->buffers[2].buffer;
    
    factor = (float)state->base.sampleDuration * M;    
    
    freqPos = 0.0;
    freqInc = (double)pp->buffers[1].bufferSize / (double)N;
    
    for (i = 0; i < N; ++i) 
    {
        index = currentPosition;
        frac = currentPosition - index;

        output[i] = pl_LinInterpF (table[index & mask], table[(index + 1) & mask], frac);
        currentPosition += freq[(int)freqPos] * factor;
                
        freqPos += freqInc;
    }                    
    
    index = currentPosition;
    state->currentPosition = index & mask;
    state->fractionPosition = currentPosition - index;
}

void plink_TableProcessF (void* ppv, TableProcessStateF* state) 
{
    PlinkProcessF* pp;
    int N, n;
    
    pp = (PlinkProcessF*)ppv;
    N = pp->buffers[0].bufferSize;
    n = pp->buffers[1].bufferSize;
    
    if (n == N)         
        plink_TableProcessF_NN (ppv, state);
    else if (n == 1)    
        plink_TableProcessF_N1 (ppv, state);
    else                
        plink_TableProcessF_Nn (ppv, state);
}

