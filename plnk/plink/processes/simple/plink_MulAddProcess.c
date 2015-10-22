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

#include "../../core/plink_StandardHeader.h"
#include "plink_MulAddProcess.h"

void plink_MulAddProcessF_NNNN (void* ppv, MulAddProcessState* state)
{
    PlinkProcessF* pp;
    int N;
    float *output, *input, *mul, *add;
    
	(void)state;

    pp     = (PlinkProcessF*)ppv;
    N      = pp->buffers[0].bufferSize;
    output = pp->buffers[0].buffer;
    input  = pp->buffers[1].buffer;
    mul    = pp->buffers[2].buffer;
    add    = pp->buffers[3].buffer;
    
    pl_VectorMulAddF_NNNN (output, input, mul, add, N);
}

void plink_MulAddProcessF_NN11 (void* ppv, MulAddProcessState* state)
{
    PlinkProcessF* pp;
    int N;
    float *output, *input; 
    float mul, add;
    
	(void)state;

    pp     = (PlinkProcessF*)ppv;
    N      = pp->buffers[0].bufferSize;
    output = pp->buffers[0].buffer;
    input  = pp->buffers[1].buffer;
    mul    = pp->buffers[2].buffer[0];
    add    = pp->buffers[3].buffer[0];
            
    pl_VectorMulAddF_NN11(output, input, mul, add, N);
}

void plink_MulAddProcessF_NNN1 (void* ppv, MulAddProcessState* state)
{
    PlinkProcessF* pp;
    int N;
    float *output, *input, *mul;
    float add;
    
	(void)state;

    pp     = (PlinkProcessF*)ppv;
    N      = pp->buffers[0].bufferSize;
    output = pp->buffers[0].buffer;
    input  = pp->buffers[1].buffer;
    mul    = pp->buffers[2].buffer;
    add    = pp->buffers[3].buffer[0];
        
    pl_VectorMulAddF_NNN1 (output, input, mul, add, N);
}

void plink_MulAddProcessF_NN1N (void* ppv, MulAddProcessState* state)
{
    PlinkProcessF* pp;
    int N;
    float *output, *input, *add;
    float mul;
    
	(void)state;

    pp     = (PlinkProcessF*)ppv;
    N      = pp->buffers[0].bufferSize;
    output = pp->buffers[0].buffer;
    input  = pp->buffers[1].buffer;
    mul    = pp->buffers[2].buffer[0];
    add    = pp->buffers[3].buffer;
        
    pl_VectorMulAddF_NN1N (output, input, mul, add, N);
}

void plink_MulAddProcessF_Nnnn (void* ppv, MulAddProcessState* state)
{
    PlinkProcessF* pp;
    int i, N;
    float *output, *input, *mul, *add;
    double inputPos, inputInc;
    double mulPos, mulInc;
    double addPos, addInc;
    
	(void)state;

    pp     = (PlinkProcessF*)ppv;
    N      = pp->buffers[0].bufferSize;
    output = pp->buffers[0].buffer;
    input  = pp->buffers[1].buffer;
    mul    = pp->buffers[2].buffer;
    add    = pp->buffers[3].buffer;
    
    inputPos = mulPos = addPos = 0.0;
    inputInc = (double)pp->buffers[1].bufferSize / (double)N;
    mulInc   = (double)pp->buffers[2].bufferSize / (double)N;
    addInc   = (double)pp->buffers[3].bufferSize / (double)N;

    for (i = 0; i < N; ++i) 
    {
        output[i] = input[(int)inputPos] * mul[(int)mulPos] + add[(int)addPos];
        inputPos += inputInc;
        mulPos += mulInc;
        addPos += addInc;
    }
}

void plink_MulAddProcessF (void* ppv, MulAddProcessState* state) 
{
    PlinkProcessF* pp;
    int N, n1, n2, n3;
    
    pp = (PlinkProcessF*)ppv;
    N  = pp->buffers[0].bufferSize;
    n1 = pp->buffers[1].bufferSize;
    n2 = pp->buffers[2].bufferSize;
    n3 = pp->buffers[3].bufferSize;

    if ((n1 == N) && (n2 == N) && (n3 == N))         
        plink_MulAddProcessF_NNNN (ppv, state);
    else if ((n1 == N) && (n2 == 1) && (n3 == 1))    
        plink_MulAddProcessF_NN11 (ppv, state);
    else if ((n1 == N) && (n2 == N) && (n3 == 1))    
        plink_MulAddProcessF_NNN1 (ppv, state);
    else if ((n1 == N) && (n2 == 1) && (n3 == N))    
        plink_MulAddProcessF_NN1N (ppv, state);
    else                
        plink_MulAddProcessF_Nnnn (ppv, state);
}

