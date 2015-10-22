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

#ifndef PLANK_NEURALNETWORK_H
#define PLANK_NEURALNETWORK_H

#include "../../containers/plank_DynamicArray.h"
#include "plank_NeuralCommon.h"
#include "plank_NeuralNode.h"
#include "plank_NeuralLayer.h"
#include "../json/plank_JSON.h"

#define PLANK_NEURALNETWORKF_JSON_TYPE           "plank::NeuralNetworkF"
#define PLANK_NEURALNETWORKF_JSON_VERSION        0, 1, 1, 0
#define PLANK_NEURALNETWORKF_JSON_LEARNRATE      "learnRate"
#define PLANK_NEURALNETWORKF_JSON_ACTFUNCOFFSET  "actFuncOffset"
#define PLANK_NEURALNETWORKF_JSON_STRUCTURE      "structure"
#define PLANK_NEURALNETWORKF_JSON_LAYERS         "layers"

PLANK_BEGIN_C_LINKAGE

/** A neural network.
  
 @defgroup PlankNeuralNetworkFClass Plank NeuralNetwork class
 @ingroup PlankClasses
 @{
 */

#if DOXYGEN
/** An opaque reference to the <i>Plank %NeuralNetworkF</i> object. */
typedef struct PlankNeuralNetworkF* PlankNeuralNetworkFRef;
#endif 

typedef PlankF (*PlankNeuralNetworkFActFunction)(PlankF);

/** Initialise a <i>Plank %NeuralNetworkF</i> object.
 Deafults to have one wieght. pl_NeuralNetworkF_InitWithNumWeights() is more useful.
 @param p The <i>Plank %NeuralNetworkF</i> object.
 @param layers An integer array containing the count of the nodes on each layer.
 @param numLayer The number of items in the @c layers array.
 @param range The random range of the initial weights for the network.
 @return PlankResult_OK if successful, otherwise an error code. */
PlankResult pl_NeuralNetworkF_InitWithLayersAndRange (PlankNeuralNetworkFRef p, const int* layers, const int numLayers, const float range);

/** Deinitialise a <i>Plank %NeuralNetworkF</i> object.
 @param p The <i>Plank %NeuralNetworkF</i> object.
 @return PlankResult_OK if successful, otherwise an error code. */
PlankResult pl_NeuralNetworkF_DeInit (PlankNeuralNetworkFRef p);

PlankResult pl_NeuralNetworkF_Reset (PlankNeuralNetworkFRef p, const float amount);
PlankResult pl_NeuralNetworkF_Randomise (PlankNeuralNetworkFRef p, const float amount);
float pl_NeuralNetworkF_GetLearnRate (PlankNeuralNetworkFRef p);
float pl_NeuralNetworkF_GetActFuncOffset (PlankNeuralNetworkFRef p);
PlankResult pl_NeuralNetworkF_SetLearnRate (PlankNeuralNetworkFRef p, const float amount);
PlankResult pl_NeuralNetworkF_SetActFuncOffset (PlankNeuralNetworkFRef p, const float amount);
int pl_NeuralNetworkF_GetNumLayers (PlankNeuralNetworkFRef p);
int pl_NeuralNetworkF_GetNumLayersExludingInput (PlankNeuralNetworkFRef p);
int pl_NeuralNetworkF_GetNumInputs (PlankNeuralNetworkFRef p);
int pl_NeuralNetworkF_GetNumOutputs (PlankNeuralNetworkFRef p);
PlankResult pl_NeuralNetworkF_GetLayer (PlankNeuralNetworkFRef p, const int layerIndex, PlankNeuralLayerFRef* layer);

PlankResult pl_NeuralNetworkF_Set (PlankNeuralNetworkFRef p, const int layerIndex, const int nodeIndex, const float* weights, const float threshold);
PlankResult pl_NeuralNetworkF_SetThreshold (PlankNeuralNetworkFRef p, const int layerIndex, const int nodeIndex, const float threshold);
PlankResult pl_NeuralNetworkF_SetWeight (PlankNeuralNetworkFRef p, const int layerIndex, const int nodeIndex, const int weightIndex, const float weight);
PlankResult pl_NeuralNetworkF_Get (PlankNeuralNetworkFRef p, const int layerIndex, const int nodeIndex, float* weights, float* threshold);
PlankResult pl_NeuralNetworkF_Propogate (PlankNeuralNetworkFRef p, const float* inputs);
PlankResult pl_NeuralNetworkF_BackProp (PlankNeuralNetworkFRef p, const float* inputs, const float* targets);
PlankResult pl_NeuralNetworkF_GetOutputs (PlankNeuralNetworkFRef p, float* outputs);
const float* pl_NeuralNetworkF_GetOutputsPtr (PlankNeuralNetworkFRef p);
PlankResult pl_NeuralNetworkF_SetActFunc (PlankNeuralNetworkFRef p, PlankNeuralNetworkFActFunction actFunc);

PlankResult pl_NeuralNetworkF_ToJSON (PlankNeuralNetworkFRef p, PlankJSONRef j, const PlankB useBinary);
PlankResult pl_NeuralNetworkF_InitFromJSON (PlankNeuralNetworkFRef p, PlankJSONRef j);




PLANK_END_C_LINKAGE

#if !DOXYGEN
typedef struct PlankNeuralNetworkF
{
    float learnRate, actFuncOffset;
	PlankDynamicArray layers;
	PlankDynamicArray errorVector;
    PlankNeuralNetworkFActFunction actFunc;
} PlankNeuralNetworkF;
#endif


#endif // PLANK_NEURALNETWORK_H
