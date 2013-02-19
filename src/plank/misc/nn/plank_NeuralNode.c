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

#include "../../core/plank_StandardHeader.h"
#include "plank_NeuralNode.h"
#include "../../maths/vectors/plank_Vectors.h"
#include "../../random/plank_RNG.h"

float pl_NeuralNodeF_PropogateScalar (PlankNeuralNodeFRef p, const float* inputVector);
PlankResult pl_NeuralNodeF_BackPropVector (PlankNeuralNodeFRef p, const float* inputVector, const float error, const float actFuncOffset, const float learnRate, float* adjustVector);
float pl_NeuralNodeF_PropogateVector (PlankNeuralNodeFRef p, const float* inputVector);
PlankResult pl_NeuralNodeF_BackPropScalar (PlankNeuralNodeFRef p, const float* inputVector, const float error, const float actFuncOffset, const float learnRate, float* adjustVector);

PlankResult pl_NeuralNodeF_Init (PlankNeuralNodeFRef p, PlankNeuralNetworkFRef network)
{
    return pl_NeuralNodeF_InitWithNumWeightsAndRange (p, network, 1, 0.1f);
}

PlankResult pl_NeuralNodeF_InitWithNumWeights (PlankNeuralNodeFRef p, PlankNeuralNetworkFRef network, const int numWeights)
{
    return pl_NeuralNodeF_InitWithNumWeightsAndRange (p, network, numWeights, 0.1f);
}

PlankResult pl_NeuralNodeF_InitWithNumWeightsAndRange (PlankNeuralNodeFRef p, PlankNeuralNetworkFRef network, const int numWeights, const float range)
{
    PlankResult result = PlankResult_OK;
    
    if (p == PLANK_NULL)
    {
        result = PlankResult_MemoryError;
        goto exit;
    }
    
    pl_MemoryZero (p, sizeof (PlankNeuralNodeF));
    
    p->network = network;
        
    if ((result = pl_DynamicArray_InitWithItemSizeAndSize (&p->weightVector, sizeof (PlankF), pl_MaxI (1, numWeights), PLANK_TRUE)) != PlankResult_OK)
        goto exit;
        
    if ((result = pl_NeuralNodeF_Randomise (p, range)) != PlankResult_OK)
        goto exit;
    
    if (numWeights < 8)
    {
        p->propogate = pl_NeuralNodeF_PropogateScalar;
        p->backProp = pl_NeuralNodeF_BackPropScalar;
    }
    else
    {
        p->propogate = pl_NeuralNodeF_PropogateVector;
        p->backProp = pl_NeuralNodeF_BackPropVector;
    }

exit:
    return result;
}

PlankResult pl_NeuralNodeF_DeInit (PlankNeuralNodeFRef p)
{
    PlankResult result = PlankResult_OK;
    
    if (p == PLANK_NULL)
    {
        result = PlankResult_MemoryError;
        goto exit;
    }
    
    result = pl_DynamicArray_DeInit (&p->weightVector);
    pl_MemoryZero (p, sizeof (PlankNeuralNodeF));
    
exit:
    return result;
}

int pl_NeuralNodeF_GetNumWeights (PlankNeuralNodeFRef p)
{
    return pl_DynamicArray_GetSize (&p->weightVector);
}

PlankResult pl_NeuralNodeF_Set (PlankNeuralNodeFRef p, const float* weights, const float threshold)
{
    PlankResult result;
    result = PlankResult_OK;
    
    if ((result = pl_NeuralNodeF_SetWeights (p, weights)) != PlankResult_OK) goto exit;
    if ((result = pl_NeuralNodeF_SetThreshold (p, threshold)) != PlankResult_OK) goto exit;

exit:
    return result;    
}

PlankResult pl_NeuralNodeF_SetThreshold (PlankNeuralNodeFRef p, const float threshold)
{
    p->threshold = threshold;
    return PlankResult_OK;
}

PlankResult pl_NeuralNodeF_SetWeights (PlankNeuralNodeFRef p, const float* weights)
{
    PlankResult result;
    result = PlankResult_OK;

    pl_VectorMoveF_NN ((PlankF*)pl_DynamicArray_GetArray (&p->weightVector),
                       weights,
                       pl_DynamicArray_GetSize (&p->weightVector));
    
    return result;
}

PlankResult pl_NeuralNodeF_SetWeight (PlankNeuralNodeFRef p, const int index, const float weight)
{
    PlankResult result;
    float* weightVectorPtr;
    int size;
    
    result = PlankResult_OK;
    size = pl_DynamicArray_GetSize (&p->weightVector);
    
    if ((index < 0) || (index >= size))
    {
        result = PlankResult_IndexOutOfRange;
        goto exit;
    }
    
    weightVectorPtr = (float*)pl_DynamicArray_GetArray (&p->weightVector);
    weightVectorPtr[index] = weight;
  
exit:
    return result;
}

PlankResult pl_NeuralNodeF_Get (PlankNeuralNodeFRef p, float* weights, float* threshold)
{
    PlankResult result;
    result = PlankResult_OK;
    
    if ((result = pl_NeuralNodeF_GetWeights (p, weights)) != PlankResult_OK)
        goto exit;

    *threshold = pl_NeuralNodeF_GetThreshold (p);
    
exit:
    return result;
}

float pl_NeuralNodeF_GetThreshold (PlankNeuralNodeFRef p)
{
    return p->threshold;
}

PlankResult pl_NeuralNodeF_GetWeights (PlankNeuralNodeFRef p, float* weights)
{
    PlankResult result;
    result = PlankResult_OK;
    
    pl_VectorMoveF_NN (weights,
                       (const float*)pl_DynamicArray_GetArray (&p->weightVector),
                       pl_DynamicArray_GetSize (&p->weightVector));
    
    return result;
}

const float* pl_NeuralNodeF_GetWeightsPtr (PlankNeuralNodeFRef p)
{
    return (const float*)pl_DynamicArray_GetArray (&p->weightVector);
}

PlankResult pl_NeuralNodeF_Reset (PlankNeuralNodeFRef p, const float amount)
{
    PlankRNGRef r;
    float amount2;
    float* weightVectorPtr;
    int size, i;
    
    weightVectorPtr = (float*)pl_DynamicArray_GetArray (&p->weightVector);
    size = pl_DynamicArray_GetSize (&p->weightVector);
    r = pl_RNGGlobal();
    
    amount2 = amount * 2.f;
    
	for (i = 0; i < size; ++i)
		weightVectorPtr[i] = pl_RNG_NextFloat (r) * amount2 - amount;
    
    p->threshold = pl_RNG_NextFloat (r) * amount2 - amount;
    
	return PlankResult_OK;
}

PlankResult pl_NeuralNodeF_Randomise (PlankNeuralNodeFRef p, const float amount)
{
    PlankRNGRef r;
    float amount2;
    float* weightVectorPtr;
    int size, i;
    
    weightVectorPtr = (float*)pl_DynamicArray_GetArray (&p->weightVector);
    size = pl_DynamicArray_GetSize (&p->weightVector);
    r = pl_RNGGlobal();
    
    amount2 = amount * 2.f;
    
	for (i = 0; i < size; ++i)
		weightVectorPtr[i] += pl_RNG_NextFloat (r) * amount2 - amount;

    p->threshold += pl_RNG_NextFloat (r) * amount2 - amount;
    
	return PlankResult_OK;
}

PlankResult pl_NeuralNodeF_BackPropVector (PlankNeuralNodeFRef p, const float* inputVector, const float error, const float actFuncOffset, const float learnRate, float* adjustVector)
{
	float output, adjust, learn;
	float* weightVectorPtr;
    int size;
    
    output = p->output;
    adjust = error * (actFuncOffset + (output * (1.f - output)));
    learn = adjust * learnRate;

	weightVectorPtr = (float*)pl_DynamicArray_GetArray (&p->weightVector);

	size = pl_DynamicArray_GetSize (&p->weightVector);
        
    pl_VectorMulAddF_NN1N (weightVectorPtr, inputVector, learn, weightVectorPtr, size);
    pl_VectorMulAddF_NN1N (adjustVector, weightVectorPtr, adjust, adjustVector, size);
    	   
	p->threshold += learn;
    
    return PlankResult_OK;
}

float pl_NeuralNodeF_PropogateVector (PlankNeuralNodeFRef p, const float* inputVector)
{
	float input;
    const float* weightVectorPtr;
    int size;
    
    input = 0.f;
    weightVectorPtr = (const float*)pl_DynamicArray_GetArray (&p->weightVector);
    size = pl_DynamicArray_GetSize (&p->weightVector);
    
    pl_VectorAddMulF_1NN (&input, inputVector, weightVectorPtr, size);
    p->output = p->network->actFunc (input + p->threshold);
	
	return p->output;
}

PlankResult pl_NeuralNodeF_BackPropScalar (PlankNeuralNodeFRef p, const float* inputVector, const float error, const float actFuncOffset, const float learnRate, float* adjustVector)
{
	float output, adjust, learn;
	float* weightVectorPtr;
    int size, i;
    
    output = p->output;
    adjust = error * (actFuncOffset + (output * (1.f - output)));
    learn = adjust * learnRate;
    
	weightVectorPtr = (float*)pl_DynamicArray_GetArray (&p->weightVector);
	size = pl_DynamicArray_GetSize (&p->weightVector);
    
    for (i = 0; i < size; ++i)
    {
        weightVectorPtr[i] += inputVector[i] * learn;
        adjustVector[i] += weightVectorPtr[i] * adjust;
    }
    
	p->threshold += learn;
    
    return PlankResult_OK;
}

float pl_NeuralNodeF_PropogateScalar (PlankNeuralNodeFRef p, const float* inputVector)
{
	float input;
    const float* weightVectorPtr;
    int size, i;
    
    input = 0.f;
    weightVectorPtr = (const float*)pl_DynamicArray_GetArray (&p->weightVector);
    size = pl_DynamicArray_GetSize (&p->weightVector);
    
    for (i = 0; i < size; ++i)
        input += inputVector[i] * weightVectorPtr[i];
        
    p->output = p->network->actFunc (input + p->threshold);
	
	return p->output;
}

PlankResult pl_NeuralNodeF_BackProp (PlankNeuralNodeFRef p, const float* inputVector, const float error, const float actFuncOffset, const float learnRate, float* adjustVector)
{
    return p->backProp (p, inputVector, error, actFuncOffset, learnRate, adjustVector);
}

float pl_NeuralNodeF_Propogate (PlankNeuralNodeFRef p, const float* inputVector)
{
    return p->propogate (p, inputVector);
}

float pl_NeuralNodeF_GetOutput (PlankNeuralNodeFRef p)
{
    return p->output;
}

PlankResult pl_NeuralNodeF_ToJSON (PlankNeuralNodeFRef p, PlankJSONRef j)
{
    PlankResult result;
    int i, numWeights;
    const float* weightArray;
    PlankJSONRef jnode;
    PlankJSONRef jweights;
    
    result = PlankResult_OK;
    
    jnode = pl_JSON_Object();
    jweights = pl_JSON_Array();
    
    pl_JSON_ObjectSetType (jnode, PLANK_NEURALNODEF_JSON_TYPE);
    pl_JSON_ObjectSetVersionString (jnode, PLANK_NEURALNODEF_JSON_VERSION);
    pl_JSON_ObjectSetFormatText (jnode);
    
    pl_JSON_ObjectPutKey (jnode, PLANK_NEURALNODEF_JSON_THRESHOLD, pl_JSON_Float (p->threshold));

    numWeights = pl_DynamicArray_GetSize (&p->weightVector);
    weightArray = (const float*)pl_DynamicArray_GetArray (&p->weightVector);
    
    for (i = 0; i < numWeights; ++i)
	{
        pl_JSON_ArrayAppend (jweights, pl_JSON_Float (weightArray[i]));
    }
    
    pl_JSON_ObjectPutKey (jnode, PLANK_NEURALNODEF_JSON_WEIGHTS, jweights);
    pl_JSON_ArrayAppend (j, jnode);

exit:
    return result;
}

PlankResult pl_NeuralNodeF_InitFromJSON (PlankNeuralNodeFRef p, PlankNeuralNetworkFRef network, PlankJSONRef j)
{
    PlankResult result;
    PlankJSONRef jweights;
    int numWeights, i;
    float* weights;
    
    result = PlankResult_OK;
    
    if (p == PLANK_NULL)
    {
        result = PlankResult_MemoryError;
        goto exit;
    }
    
    pl_MemoryZero (p, sizeof (PlankNeuralNodeF));
    p->network = network;
    
    if (!pl_JSON_IsObjectType (j, PLANK_NEURALNODEF_JSON_TYPE))
    {
        result = PlankResult_JSONError;
        goto exit;
    }
    
    if (pl_JSON_ObjectGetVersion (j) > pl_JSON_VersionCode (PLANK_NEURALNODEF_JSON_VERSION))
    {
        result = PlankResult_JSONError;
        goto exit;
    }
    
    if (pl_JSON_IsObjectFormatText (j))
    {
        if ((jweights = pl_JSON_ObjectAtKey (j, PLANK_NEURALNODEF_JSON_WEIGHTS)) == 0)
        {
            result = PlankResult_JSONError;
            goto exit;
        }
        
        if (!pl_JSON_IsArray (jweights))
        {
            result = PlankResult_JSONError;
            goto exit;
        }
        
        numWeights = (int)pl_JSON_ArrayGetSize (jweights);
            
        if (numWeights < 1)
        {
            result = PlankResult_JSONError;
            goto exit;
        }
        
        if ((result = pl_DynamicArray_InitWithItemSizeAndSize (&p->weightVector, sizeof (PlankF), numWeights, PLANK_TRUE)) != PlankResult_OK)
            goto exit;
        
        weights = (float*)pl_DynamicArray_GetArray (&p->weightVector);
        
        for (i = 0; i < numWeights; ++i)
            weights[i] = pl_JSON_FloatGet (pl_JSON_ArrayAt (jweights, i));
        
        p->threshold = pl_JSON_FloatGet (pl_JSON_ObjectAtKey (j, PLANK_NEURALNODEF_JSON_THRESHOLD));
    }
    
    if (numWeights < 8)
    {
        p->propogate = pl_NeuralNodeF_PropogateScalar;
        p->backProp = pl_NeuralNodeF_BackPropScalar;
    }
    else
    {
        p->propogate = pl_NeuralNodeF_PropogateVector;
        p->backProp = pl_NeuralNodeF_BackPropVector;
    }
    
exit:
    return result;
}


