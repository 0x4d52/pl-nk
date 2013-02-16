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
#include "plank_NeuralLayer.h"
#include "../../maths/vectors/plank_Vectors.h"
//#include "../../random/plank_RNG.h"



PlankResult pl_NeuralLayerF_InitNumNodesAndPrevious (PlankNeuralLayerFRef p, PlankNeuralNetworkFRef network, const int numNodes, const int numPreviousNodes)
{
    return pl_NeuralLayerF_InitNumNodesPreviousWithRange (p, network, numNodes, numPreviousNodes, 0.1f);
}

PlankResult pl_NeuralLayerF_InitNumNodesPreviousWithRange (PlankNeuralLayerFRef p, PlankNeuralNetworkFRef network, const int numNodes, const int numPreviousNodes, const float range)
{
    PlankResult result;
    int numNodesChecked, numPreviousNodesChecked, i;
    PlankNeuralNodeF* nodeArray;
    
    result = PlankResult_OK;
    
    if (p == PLANK_NULL)
    {
        result = PlankResult_MemoryError;
        goto exit;
    }
    
    pl_MemoryZero (p, sizeof (PlankNeuralLayerF));
    
    numNodesChecked = pl_MaxI (1, numNodes);
    numPreviousNodesChecked = pl_MaxI (1, numPreviousNodes);
    
    if ((result = pl_DynamicArray_InitWithItemSizeAndSize (&p->nodes, sizeof (PlankNeuralNodeF), numNodesChecked, PLANK_FALSE)) != PlankResult_OK)
        goto exit;
    
    nodeArray = (PlankNeuralNodeF*)pl_DynamicArray_GetArray (&p->nodes);
    
    for (i = 0; i < numNodesChecked; ++i)
    {
        if ((result = pl_NeuralNodeF_InitWithNumWeightsAndRange (&nodeArray[i], network, numPreviousNodesChecked, range)) != PlankResult_OK)
            goto exit;
    }

    if ((result = pl_DynamicArray_InitWithItemSizeAndSize (&p->outputVector, sizeof (PlankF), numNodesChecked, PLANK_TRUE)) != PlankResult_OK) goto exit;    
    if ((result = pl_DynamicArray_InitWithItemSizeAndSize (&p->inputVector, sizeof (PlankF), numPreviousNodesChecked, PLANK_TRUE)) != PlankResult_OK) goto exit;
    if ((result = pl_DynamicArray_InitWithItemSizeAndSize (&p->adjustVector, sizeof (PlankF), numPreviousNodesChecked, PLANK_TRUE)) != PlankResult_OK) goto exit;

exit:
    return result;
}

PlankResult pl_NeuralLayerF_DeInit (PlankNeuralLayerFRef p)
{
    PlankResult result;
    int numNodes, i;
    PlankNeuralNodeF* nodeArray;

    result = PlankResult_OK;

    if (p == PLANK_NULL)
    {
        result = PlankResult_MemoryError;
        goto exit;
    }
    
    numNodes = pl_DynamicArray_GetSize (&p->nodes);
    nodeArray = (PlankNeuralNodeF*)pl_DynamicArray_GetArray (&p->nodes);

    for (i = 0; i < numNodes; ++i)
        pl_NeuralNodeF_DeInit (&nodeArray[i]);
    
    pl_DynamicArray_DeInit (&p->nodes);
    pl_DynamicArray_DeInit (&p->outputVector);
    pl_DynamicArray_DeInit (&p->inputVector);
    pl_DynamicArray_DeInit (&p->adjustVector);
    
    pl_MemoryZero (p, sizeof (PlankNeuralLayerF));
    
exit:
    return result;
}

PlankResult pl_NeuralLayerF_Reset (PlankNeuralLayerFRef p, const float amount)
{
    PlankResult result;
    int numNodes, i;
    PlankNeuralNodeF* nodeArray;
    
    result = PlankResult_OK;
    numNodes = pl_DynamicArray_GetSize (&p->nodes);
    nodeArray = (PlankNeuralNodeF*)pl_DynamicArray_GetArray (&p->nodes);
    
    for (i = 0; i < numNodes; ++i)
    {
        if ((result = pl_NeuralNodeF_Reset (&nodeArray[i], amount)) != PlankResult_OK)
            goto exit;
    }
    
exit:
    return result;
}

PlankResult pl_NeuralLayerF_Randomise (PlankNeuralLayerFRef p, const float amount)
{
    PlankResult result;
    int numNodes, i;
    PlankNeuralNodeF* nodeArray;
    
    result = PlankResult_OK;
    numNodes = pl_DynamicArray_GetSize (&p->nodes);
    nodeArray = (PlankNeuralNodeF*)pl_DynamicArray_GetArray (&p->nodes);

    for (i = 0; i < numNodes; ++i)
    {
        if ((result = pl_NeuralNodeF_Randomise (&nodeArray[i], amount)) != PlankResult_OK)
            goto exit;
    }
    
exit:
    return result;
}

PlankResult pl_NeuralLayerF_SetNode (PlankNeuralLayerFRef p, const int nodeIndex, const float* weights, const float threshold)
{
    PlankResult result;
    int numNodes;
    PlankNeuralNodeF* nodeArray;
    
    result = PlankResult_OK;
    numNodes = pl_DynamicArray_GetSize (&p->nodes);

    if ((nodeIndex < 0) || (nodeIndex >= numNodes))
    {
        result = PlankResult_IndexOutOfRange;
        goto exit;
    }
    
    nodeArray = (PlankNeuralNodeF*)pl_DynamicArray_GetArray (&p->nodes);
    result = pl_NeuralNodeF_Set (&nodeArray[nodeIndex], weights, threshold);
    
exit:
    return result;
}

PlankResult pl_NeuralLayerF_SetThreshold (PlankNeuralLayerFRef p, const int nodeIndex, const float threshold)
{
    PlankResult result;
    int numNodes;
    PlankNeuralNodeF* nodeArray;
    
    result = PlankResult_OK;
    numNodes = pl_DynamicArray_GetSize (&p->nodes);
    
    if ((nodeIndex < 0) || (nodeIndex >= numNodes))
    {
        result = PlankResult_IndexOutOfRange;
        goto exit;
    }
    
    nodeArray = (PlankNeuralNodeF*)pl_DynamicArray_GetArray (&p->nodes);
    result = pl_NeuralNodeF_SetThreshold (&nodeArray[nodeIndex], threshold);
    
exit:
    return result;
}

PlankResult pl_NeuralLayerF_SetWeight (PlankNeuralLayerFRef p, const int nodeIndex, const int weightIndex, const float weight)
{
    PlankResult result;
    int numNodes;
    PlankNeuralNodeF* nodeArray;
    
    result = PlankResult_OK;
    numNodes = pl_DynamicArray_GetSize (&p->nodes);
    
    if ((nodeIndex < 0) || (nodeIndex >= numNodes))
    {
        result = PlankResult_IndexOutOfRange;
        goto exit;
    }
    
    nodeArray = (PlankNeuralNodeF*)pl_DynamicArray_GetArray (&p->nodes);
    result = pl_NeuralNodeF_SetWeight (&nodeArray[nodeIndex], weightIndex, weight);
    
exit:
    return result;
}

PlankResult pl_NeuralLayerF_GetNode (PlankNeuralLayerFRef p, const int nodeIndex, float* weights, float* threshold)
{
    PlankResult result;
    int numNodes;
    PlankNeuralNodeF* nodeArray;
    
    result = PlankResult_OK;
    numNodes = pl_DynamicArray_GetSize (&p->nodes);
    
    if ((nodeIndex < 0) || (nodeIndex >= numNodes))
    {
        result = PlankResult_IndexOutOfRange;
        goto exit;
    }
    
    nodeArray = (PlankNeuralNodeF*)pl_DynamicArray_GetArray (&p->nodes);
    result = pl_NeuralNodeF_Get (&nodeArray[nodeIndex], weights, threshold);
    
exit:
    return result;
}

PlankResult pl_NeuralLayerF_Propogate (PlankNeuralLayerFRef p, const float* inputs)
{
    PlankResult result;
    int numNodes, numInputs, i;
    PlankNeuralNodeF* nodeArray;
    float* inputVectorPtr;
    float* outputVectorPtr;
    
    result = PlankResult_OK;
    numNodes = pl_DynamicArray_GetSize (&p->nodes);
    numInputs = pl_DynamicArray_GetSize (&p->inputVector);
    
    inputVectorPtr = (float*)pl_DynamicArray_GetArray (&p->inputVector);
    outputVectorPtr = (float*)pl_DynamicArray_GetArray (&p->outputVector);
    nodeArray = (PlankNeuralNodeF*)pl_DynamicArray_GetArray (&p->nodes);

    pl_VectorMoveF_NN (inputVectorPtr, inputs, numInputs);

    for (i = 0; i < numNodes; ++i)
        outputVectorPtr[i] = pl_NeuralNodeF_Propogate (&nodeArray[i], inputVectorPtr);
        
exit:
    return result;
}

PlankResult pl_NeuralLayerF_BackProp (PlankNeuralLayerFRef p, const float* errors, const float actFuncOffset, const float learnRate)
{
    PlankResult result;
    int numNodes, i;
    PlankNeuralNodeF* nodeArray;
    const float* inputVectorPtr;
    float* adjustVectorPtr;
   
    result = PlankResult_OK;
    numNodes = pl_DynamicArray_GetSize (&p->nodes);
    
    pl_DynamicArray_Zero (&p->adjustVector);
    
    inputVectorPtr = (const float*)pl_DynamicArray_GetArray (&p->inputVector);
    adjustVectorPtr = (float*)pl_DynamicArray_GetArray (&p->adjustVector);
    nodeArray = (PlankNeuralNodeF*)pl_DynamicArray_GetArray (&p->nodes);

    for (i = 0; i < numNodes; ++i)
        pl_NeuralNodeF_BackProp (&nodeArray[i], inputVectorPtr, errors[i], actFuncOffset, learnRate, adjustVectorPtr);
    
    return result;
}

PlankResult pl_NeuralLayerF_GetOutputs (PlankNeuralLayerFRef p, float* outputs)
{
    PlankResult result;
    result = PlankResult_OK;
    
    pl_VectorMoveF_NN (outputs,
                       (const float*)pl_DynamicArray_GetArray (&p->outputVector),
                       pl_DynamicArray_GetSize (&p->outputVector));
    
    return result;

}

const float* pl_NeuralLayerF_GetOutputsPtr (PlankNeuralLayerFRef p)
{
    return (const float*)pl_DynamicArray_GetArray (&p->outputVector);
}

const float* pl_NeuralLayerF_GetAdjustPtr (PlankNeuralLayerFRef p)
{
    return (const float*)pl_DynamicArray_GetArray (&p->adjustVector);
}

int pl_NeuralLayerF_GetNumInputs (PlankNeuralLayerFRef p)
{
    return pl_DynamicArray_GetSize (&p->inputVector);
}

int pl_NeuralLayerF_GetNumOutputs (PlankNeuralLayerFRef p)
{
    return pl_DynamicArray_GetSize (&p->outputVector);
}

