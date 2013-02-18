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
#include "plank_NeuralLayer.h"
#include "plank_NeuralNetwork.h"
#include "../../maths/vectors/plank_Vectors.h"

static const float NeuralFE1 = 2.7182818284590452354f;

static PlankF pl_NeuralNetworkFDefaultActFunction (PlankF value)
{
    return 1.f / (1.f + pl_PowF (NeuralFE1, -value));
}

PlankResult pl_NeuralNetworkF_InitWithLayersAndRange (PlankNeuralNetworkFRef p, const int* layers, const int numLayers, const float range)
{
    PlankResult result;
    int i;
    PlankNeuralLayerF* layerArray;

    result = PlankResult_OK;
    
    if (p == PLANK_NULL)
    {
        result = PlankResult_MemoryError;
        goto exit;
    }
    
    pl_MemoryZero (p, sizeof (PlankNeuralNetworkF));
    
    pl_NeuralNetworkF_SetActFunc (p, PLANK_NULL);
    
    if (numLayers < 2)
    {
        result = PlankResult_ItemCountInvalid;
        goto exit;
    }
    
    if ((result = pl_DynamicArray_InitWithItemSizeAndSize (&p->layers, sizeof (PlankNeuralLayerF), numLayers - 1, PLANK_FALSE)) != PlankResult_OK) goto exit;
    layerArray = (PlankNeuralLayerF*)pl_DynamicArray_GetArray (&p->layers);
    
    for (i = 1; i < numLayers; ++i)
    {
        if ((result = pl_NeuralLayerF_InitNumNodesPreviousWithRange (&layerArray[i - 1], p, layers[i], layers[i - 1], range)) != PlankResult_OK)
            goto exit;
    }

    if ((result = pl_DynamicArray_InitWithItemSizeAndSize (&p->errorVector, sizeof (PlankF), layers[numLayers - 1], PLANK_TRUE)) != PlankResult_OK) goto exit;
    
    p->learnRate = 0.25f;
    p->actFuncOffset = 0.01f;
    
exit:
    return result;
}

PlankResult pl_NeuralNetworkF_DeInit (PlankNeuralNetworkFRef p)
{
    PlankResult result;
    int numLayers, i;
    PlankNeuralLayerF* layerArray;

    result = PlankResult_OK;

    if (p == PLANK_NULL)
    {
        result = PlankResult_MemoryError;
        goto exit;
    }
    
    numLayers = pl_DynamicArray_GetSize (&p->layers);
    layerArray = (PlankNeuralLayerF*)pl_DynamicArray_GetArray (&p->layers);
    
    for (i = 0; i < numLayers; ++i)
        pl_NeuralLayerF_DeInit (&layerArray[i]);

    pl_DynamicArray_DeInit (&p->layers);
    pl_DynamicArray_DeInit (&p->errorVector);
    
    pl_MemoryZero (p, sizeof (PlankNeuralNetworkF));
    
exit:
    return result;
}

PlankResult pl_NeuralNetworkF_Reset (PlankNeuralNetworkFRef p, const float amount)
{
    PlankResult result;
    int numLayers, i;
    PlankNeuralLayerF* layerArray;
    
    result = PlankResult_OK;
    numLayers = pl_DynamicArray_GetSize (&p->layers);
    layerArray = (PlankNeuralLayerF*)pl_DynamicArray_GetArray (&p->layers);
    
    for (i = 0; i < numLayers; ++i)
    {
        if ((result = pl_NeuralLayerF_Reset (&layerArray[i], amount)) != PlankResult_OK)
            goto exit;
    }
    
exit:
    return result;
}

PlankResult pl_NeuralNetworkF_Randomise (PlankNeuralNetworkFRef p, const float amount)
{
    PlankResult result;
    int numLayers, i;
    PlankNeuralLayerF* layerArray;
    
    result = PlankResult_OK;
    numLayers = pl_DynamicArray_GetSize (&p->layers);
    layerArray = (PlankNeuralLayerF*)pl_DynamicArray_GetArray (&p->layers);
    
    for (i = 0; i < numLayers; ++i)
    {
        if ((result = pl_NeuralLayerF_Randomise (&layerArray[i], amount)) != PlankResult_OK)
            goto exit;
    }
    
exit:
    return result;
}

float pl_NeuralNetworkF_GetLearnRate (PlankNeuralNetworkFRef p)
{
    return p->learnRate;
}

float pl_NeuralNetworkF_GetActFuncOffset (PlankNeuralNetworkFRef p)
{
    return p->actFuncOffset;
}

PlankResult pl_NeuralNetworkF_SetLearnRate (PlankNeuralNetworkFRef p, const float amount)
{
    if (amount <= 0.f) return PlankResult_UnknownError;
    
    p->learnRate = amount;
    
    return PlankResult_OK;
}

PlankResult pl_NeuralNetworkF_SetActFuncOffset (PlankNeuralNetworkFRef p, const float amount)
{
    p->actFuncOffset = amount;
    return PlankResult_OK;
}

int pl_NeuralNetworkF_GetNumLayers (PlankNeuralNetworkFRef p)
{
    return pl_DynamicArray_GetSize (&p->layers) + 1;
}

int pl_NeuralNetworkF_GetNumLayersExludingInput (PlankNeuralNetworkFRef p)
{
    return pl_DynamicArray_GetSize (&p->layers);
}

int pl_NeuralNetworkF_GetNumInputs (PlankNeuralNetworkFRef p)
{
    PlankNeuralLayerF* layerArray;
    layerArray = (PlankNeuralLayerF*)pl_DynamicArray_GetArray (&p->layers);
    return pl_NeuralLayerF_GetNumInputs (&layerArray[0]);
}

int pl_NeuralNetworkF_GetNumOutputs (PlankNeuralNetworkFRef p)
{
    int numLayers;
    PlankNeuralLayerF* layerArray;
    
    numLayers = pl_DynamicArray_GetSize (&p->layers);
    layerArray = (PlankNeuralLayerF*)pl_DynamicArray_GetArray (&p->layers);
    
    return pl_NeuralLayerF_GetNumOutputs (&layerArray[numLayers - 1]);
}

PlankResult pl_NeuralNetworkF_Set (PlankNeuralNetworkFRef p, const int layerIndex, const int nodeIndex, const float* weights, const float threshold)
{
    PlankResult result;
    int numLayers;
    PlankNeuralLayerF* layerArray;
    
    result = PlankResult_OK;
    numLayers = pl_DynamicArray_GetSize (&p->layers);
    
    if ((layerIndex < 0) || (layerIndex >= numLayers))
    {
        result = PlankResult_IndexOutOfRange;
        goto exit;
    }
    
    layerArray = (PlankNeuralLayerF*)pl_DynamicArray_GetArray (&p->layers);
    result = pl_NeuralLayerF_SetNode (&layerArray[layerIndex], nodeIndex, weights, threshold);
    
exit:
    return result;
}

PlankResult pl_NeuralNetworkF_SetThreshold (PlankNeuralNetworkFRef p, const int layerIndex, const int nodeIndex, const float threshold)
{
    PlankResult result;
    int numLayers;
    PlankNeuralLayerF* layerArray;
    
    result = PlankResult_OK;
    numLayers = pl_DynamicArray_GetSize (&p->layers);
    
    if ((layerIndex < 0) || (layerIndex >= numLayers))
    {
        result = PlankResult_IndexOutOfRange;
        goto exit;
    }
    
    layerArray = (PlankNeuralLayerF*)pl_DynamicArray_GetArray (&p->layers);
    result = pl_NeuralLayerF_SetThreshold (&layerArray[layerIndex], nodeIndex, threshold);
    
exit:
    return result;
}

PlankResult pl_NeuralNetworkF_SetWeight (PlankNeuralNetworkFRef p, const int layerIndex, const int nodeIndex, const int weightIndex, const float weight)
{
    PlankResult result;
    int numLayers;
    PlankNeuralLayerF* layerArray;
    
    result = PlankResult_OK;
    numLayers = pl_DynamicArray_GetSize (&p->layers);
    
    if ((layerIndex < 0) || (layerIndex >= numLayers))
    {
        result = PlankResult_IndexOutOfRange;
        goto exit;
    }
    
    layerArray = (PlankNeuralLayerF*)pl_DynamicArray_GetArray (&p->layers);
    result = pl_NeuralLayerF_SetWeight (&layerArray[layerIndex], nodeIndex, weightIndex, weight);
    
exit:
    return result;
}

PlankResult pl_NeuralNetworkF_Get (PlankNeuralNetworkFRef p, const int layerIndex, const int nodeIndex, float* weights, float* threshold)
{
    PlankResult result;
    int numLayers;
    PlankNeuralLayerF* layerArray;
    
    result = PlankResult_OK;
    numLayers = pl_DynamicArray_GetSize (&p->layers);
    
    if ((layerIndex < 0) || (layerIndex >= numLayers))
    {
        result = PlankResult_IndexOutOfRange;
        goto exit;
    }
    
    layerArray = (PlankNeuralLayerF*)pl_DynamicArray_GetArray (&p->layers);
    result = pl_NeuralLayerF_GetNode(&layerArray[layerIndex], nodeIndex, weights, threshold);
    
exit:
    return result;

}

PlankResult pl_NeuralNetworkF_Propogate (PlankNeuralNetworkFRef p, const float* inputs)
{
    PlankResult result;
    int numLayers, i;
    PlankNeuralLayerF* layerArray;
    const float* layerInputs;
    
    result = PlankResult_OK;
    numLayers = pl_DynamicArray_GetSize (&p->layers);
    layerArray = (PlankNeuralLayerF*)pl_DynamicArray_GetArray (&p->layers);
    layerInputs = inputs;
    
    for (i = 0; i < numLayers; ++i)
	{
        if ((result = pl_NeuralLayerF_Propogate (&layerArray[i], layerInputs)) != PlankResult_OK) goto exit;
        layerInputs = pl_NeuralLayerF_GetOutputsPtr (&layerArray[i]);
	}
	
exit:
	return result;
}

PlankResult pl_NeuralNetworkF_BackProp (PlankNeuralNetworkFRef p, const float* inputs, const float* targets)
{
    PlankResult result;
    int numLayers, numOutputs, i;
    PlankNeuralLayerF* layerArray;
    const float* actualOutputs;
    const float* adjusts;
    float* outputErrors;

    result = PlankResult_OK;
    numLayers = pl_DynamicArray_GetSize (&p->layers);
    layerArray = (PlankNeuralLayerF*)pl_DynamicArray_GetArray (&p->layers);
    numOutputs = pl_DynamicArray_GetSize (&p->errorVector);
    outputErrors = (float*)pl_DynamicArray_GetArray (&p->errorVector);
    
    if ((result = pl_NeuralNetworkF_Propogate (p, inputs)) != PlankResult_OK) goto exit;
    actualOutputs = pl_NeuralLayerF_GetOutputsPtr (&layerArray[numLayers - 1]);
    
    pl_VectorSubF_NNN (outputErrors, targets, actualOutputs, numOutputs);
    		
    adjusts = outputErrors;
    
	for (i = numLayers - 1; i >= 0; --i)
	{
        if ((result = pl_NeuralLayerF_BackProp (&layerArray[i], adjusts, p->actFuncOffset, p->learnRate)) != PlankResult_OK) goto exit;
        adjusts = pl_NeuralLayerF_GetAdjustPtr (&layerArray[i]);        
	}
    
exit:
	return result;
}

PlankResult pl_NeuralNetworkF_GetOutputs (PlankNeuralNetworkFRef p, float* outputs)
{
    PlankResult result;
    int numLayers;
    PlankNeuralLayerF* layerArray;
    
    result = PlankResult_OK;
    numLayers = pl_DynamicArray_GetSize (&p->layers);
    layerArray = (PlankNeuralLayerF*)pl_DynamicArray_GetArray (&p->layers);
    result = pl_NeuralLayerF_GetOutputs (&layerArray[numLayers - 1], outputs);
    
    return result;
}

const float* pl_NeuralNetworkF_GetOutputsPtr (PlankNeuralNetworkFRef p)
{
    int numLayers;
    PlankNeuralLayerF* layerArray;
    
    numLayers = pl_DynamicArray_GetSize (&p->layers);
    layerArray = (PlankNeuralLayerF*)pl_DynamicArray_GetArray (&p->layers);
    
    return pl_NeuralLayerF_GetOutputsPtr (&layerArray[numLayers - 1]);
}

PlankResult pl_NeuralNetworkF_SetActFunc (PlankNeuralNetworkFRef p, PlankNeuralNetworkFActFunction actFunc)
{
    p->actFunc = actFunc ? actFunc : pl_NeuralNetworkFDefaultActFunction;
    return PlankResult_OK;
}

PlankResult pl_NeuralNetworkF_ToJSON (PlankNeuralNetworkFRef p, PlankJSONRef j)
{
    PlankResult result;
    int numLayers, i;
    PlankNeuralLayerF* layerArray;
    PlankJSON jnetwork;
    PlankJSON jlayers;
    
    result = PlankResult_OK;
    
    pl_JSON_InitObject (&jnetwork);
    pl_JSON_InitArray (&jlayers);
    
    if ((result = pl_JSON_ObjectSetValueString (&jnetwork, PLANK_JSON_TYPE, PLANK_NEURALNETWORKF_JSON_TYPE)) != PlankResult_OK) goto exit;
    if ((result = pl_JSON_ObjectSetValueFloat (&jnetwork, PLANK_NEURALNETWORKF_JSON_LEARNRATE, p->learnRate)) != PlankResult_OK) goto exit;
    if ((result = pl_JSON_ObjectSetValueFloat (&jnetwork, PLANK_NEURALNETWORKF_JSON_ACTFUNCOFFSET, p->actFuncOffset)) != PlankResult_OK) goto exit;
    
    numLayers = pl_DynamicArray_GetSize (&p->layers);
    layerArray = (PlankNeuralLayerF*)pl_DynamicArray_GetArray (&p->layers);
    
    for (i = 0; i < numLayers; ++i)
	{
        if ((result = pl_NeuralLayerF_ToJSON (&layerArray[i], &jlayers)) != PlankResult_OK)
            goto exit;
    }
    
    if ((result = pl_JSON_ObjectSetValue (&jnetwork, PLANK_NEURALNETWORKF_JSON_LAYERS, &jlayers)) != PlankResult_OK) goto exit;
    if ((result = pl_JSON_ArrayAppend (j, &jnetwork)) != PlankResult_OK) goto exit;
    
exit:
    return result;
}

PlankResult pl_NeuralNetworkF_InitFromJSON (PlankNeuralNetworkFRef p, PlankJSONRef j)
{
    PlankResult result;
    PlankJSON jlayers, jlayer;
    int i, numLayers, numOutputs;
    PlankNeuralLayerF* layerArray;

    result = PlankResult_OK;

    if (p == PLANK_NULL)
    {
        result = PlankResult_MemoryError;
        goto exit;
    }
    
    pl_MemoryZero (p, sizeof (PlankNeuralNetworkF));
    pl_NeuralNetworkF_SetActFunc (p, PLANK_NULL);
    
    if (!pl_JSON_IsObjectType (j, PLANK_NEURALNETWORKF_JSON_TYPE))
    {
        result = PlankResult_JSONError;
        goto exit;
    }
    
    pl_JSON_Init (&jlayers);

    if ((result = pl_JSON_ObjectGetValue (j, PLANK_NEURALNETWORKF_JSON_LAYERS, &jlayers)) != PlankResult_OK) goto exit;

    if (!pl_JSON_IsArray (&jlayers))
    {
        result = PlankResult_JSONError;
        goto exit;
    }

    if ((result = pl_JSON_ArrayGetSize (&jlayers, &numLayers)) != PlankResult_OK) goto exit;
    ++numLayers;
    
    if (numLayers < 2)
    {
        result = PlankResult_ItemCountInvalid;
        goto exit;
    }
    
    if ((result = pl_DynamicArray_InitWithItemSizeAndSize (&p->layers, sizeof (PlankNeuralLayerF), numLayers - 1, PLANK_FALSE)) != PlankResult_OK) goto exit;
    layerArray = (PlankNeuralLayerF*)pl_DynamicArray_GetArray (&p->layers);
    
    for (i = 1; i < numLayers; ++i)
    {
        pl_JSON_Init (&jlayer);

        if ((result = pl_JSON_ArrayAt (&jlayers, i - 1, &jlayer)) != PlankResult_OK)
            goto exit;

        if ((result = pl_NeuralLayerF_InitFromJSON (&layerArray[i - 1], p, &jlayer)) != PlankResult_OK)
            goto exit;
    }
    
    numOutputs = pl_NeuralLayerF_GetNumOutputs (&layerArray[numLayers - 1]);
        
    if ((result = pl_DynamicArray_InitWithItemSizeAndSize (&p->errorVector, sizeof (PlankF), numOutputs, PLANK_TRUE)) != PlankResult_OK) goto exit;
    
    if ((result = pl_JSON_ObjectGetValueFloat (j, PLANK_NEURALNETWORKF_JSON_LEARNRATE, &p->learnRate)) != PlankResult_OK) goto exit;
    p->learnRate = p->learnRate > 0.f ? p->learnRate : 0.25f;
    
    if ((result = pl_JSON_ObjectGetValueFloat (j, PLANK_NEURALNETWORKF_JSON_ACTFUNCOFFSET, &p->actFuncOffset)) != PlankResult_OK) goto exit;
    p->actFuncOffset = p->actFuncOffset > 0.f ? p->actFuncOffset : 0.01f;
        
exit:
    return result;
}
