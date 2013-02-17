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

#ifndef PLANK_NEURALLAYER_H
#define PLANK_NEURALLAYER_H

#include "../json/plank_JSON.h"
#include "../../containers/plank_DynamicArray.h"
#include "plank_NeuralNode.h"
#include "plank_NeuralNetwork.h"

#define PLANK_NEURALLAYERF_JSON_TYPE          "plank::NeuralLayerF"
#define PLANK_NEURALLAYERF_JSON_NODES         "nodes"

PLANK_BEGIN_C_LINKAGE

/** A neural layer.
  
 @defgroup PlankNeuralLayerFClass Plank NeuralNetwork class
 @ingroup PlankClasses
 @{
 */

/** An opaque reference to the <i>Plank %NeuralLayerF</i> object. */
typedef struct PlankNeuralLayerF* PlankNeuralLayerFRef;

/** Initialise a <i>Plank %NeuralLayerF</i> object.
 Deafults to have one wieght. pl_NeuralLayerF_InitWithNumWeights() is more useful.
 @param p The <i>Plank %NeuralLayerF</i> object.
 @param numNodes The number of nodes on this layer.
 @param numPreviousNodes The number of nodes on the previous layer (i.e., connected into this one).
 @return PlankResult_OK if successful, otherwise an error code. */
PlankResult pl_NeuralLayerF_InitNumNodesAndPrevious (PlankNeuralLayerFRef p, PlankNeuralNetworkFRef network, const int numNodes, const int numPreviousNodes);
PlankResult pl_NeuralLayerF_InitNumNodesPreviousWithRange (PlankNeuralLayerFRef p, PlankNeuralNetworkFRef network, const int numNodes, const int numPreviousNodes, const float range);

/** Deinitialise a <i>Plank %NeuralLayerF</i> object.
 @param p The <i>Plank %NeuralLayerF</i> object.
 @return PlankResult_OK if successful, otherwise an error code. */
PlankResult pl_NeuralLayerF_DeInit (PlankNeuralLayerFRef p);

PlankResult pl_NeuralLayerF_Reset (PlankNeuralLayerFRef p, const float amount);
PlankResult pl_NeuralLayerF_Randomise (PlankNeuralLayerFRef p, const float amount);
PlankResult pl_NeuralLayerF_SetNode (PlankNeuralLayerFRef p, const int nodeIndex, const float* weights, const float threshold);
PlankResult pl_NeuralLayerF_SetThreshold (PlankNeuralLayerFRef p, const int nodeIndex, const float threshold);
PlankResult pl_NeuralLayerF_SetWeight (PlankNeuralLayerFRef p, const int nodeIndex, const int weightIndex, const float weight);
PlankResult pl_NeuralLayerF_GetNode (PlankNeuralLayerFRef p, const int nodeIndex, float* weights, float* threshold);
PlankResult pl_NeuralLayerF_Propogate (PlankNeuralLayerFRef p, const float* inputs);
PlankResult pl_NeuralLayerF_BackProp (PlankNeuralLayerFRef p, const float* errors, const float actFuncOffset, const float learnRate);
PlankResult pl_NeuralLayerF_GetOutputs (PlankNeuralLayerFRef p, float* outputs);
const float* pl_NeuralLayerF_GetOutputsPtr (PlankNeuralLayerFRef p);
const float* pl_NeuralLayerF_GetAdjustPtr (PlankNeuralLayerFRef p);
int pl_NeuralLayerF_GetNumInputs (PlankNeuralLayerFRef p);
int pl_NeuralLayerF_GetNumOutputs (PlankNeuralLayerFRef p);
PlankResult pl_NeuralLayerF_ToJSON (PlankNeuralLayerFRef p, json_t* j);
PlankResult pl_NeuralLayerF_InitFromJSON (PlankNeuralLayerFRef p, PlankNeuralNetworkFRef network, json_t* j);




PLANK_END_C_LINKAGE

#if !DOXYGEN
typedef struct PlankNeuralLayerF
{
    PlankDynamicArray nodes;
    PlankDynamicArray outputVector;
    PlankDynamicArray inputVector;
    PlankDynamicArray adjustVector;
} PlankNeuralLayerF;
#endif


#endif // PLANK_NEURALLAYER_H


