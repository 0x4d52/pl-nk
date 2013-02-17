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

#ifndef PLANK_NEURALNODE_H
#define PLANK_NEURALNODE_H

#include "../json/plank_JSON.h"
#include "../../containers/plank_DynamicArray.h"
#include "plank_NeuralLayer.h"
#include "plank_NeuralNetwork.h"

#define PLANK_NEURALNODEF_JSON_TYPE          "plank::NeuralNodeF"
#define PLANK_NEURALNODEF_JSON_WEIGHTS       "weights"
#define PLANK_NEURALNODEF_JSON_THRESHOLD     "threshold"


PLANK_BEGIN_C_LINKAGE

/** A neural node.
  
 @defgroup PlankNeuralNodeFClass Plank NeuralNode class
 @ingroup PlankClasses
 @{
 */

/** An opaque reference to the <i>Plank %NeuralNodeF</i> object. */
typedef struct PlankNeuralNodeF* PlankNeuralNodeFRef;


/** Initialise a <i>Plank %NeuralNodeF</i> object.
 Deafults to have one wieght. pl_NeuralNodeF_InitWithNumWeights() is more useful.
 @param p The <i>Plank %NeuralNodeF</i> object.
 @return PlankResult_OK if successful, otherwise an error code. */
PlankResult pl_NeuralNodeF_Init (PlankNeuralNodeFRef p, PlankNeuralNetworkFRef network);

/** Initialise a <i>Plank %NeuralNodeF</i> object.
 @param p The <i>Plank %NeuralNodeF</i> object.
 @param numWeights The number of weights.
 @return PlankResult_OK if successful, otherwise an error code. */
PlankResult pl_NeuralNodeF_InitWithNumWeights (PlankNeuralNodeFRef p, PlankNeuralNetworkFRef network, const int numWeights);
PlankResult pl_NeuralNodeF_InitWithNumWeightsAndRange (PlankNeuralNodeFRef p, PlankNeuralNetworkFRef network, const int numWeights, const float range);

/** Deinitialise a <i>Plank %NeuralNodeF</i> object.
 @param p The <i>Plank %NeuralNodeF</i> object.
 @return PlankResult_OK if successful, otherwise an error code. */
PlankResult pl_NeuralNodeF_DeInit (PlankNeuralNodeFRef p);


int pl_NeuralNodeF_GetNumWeights (PlankNeuralNodeFRef p);
PlankResult pl_NeuralNodeF_Set (PlankNeuralNodeFRef p, const float* weights, const float threshold);
PlankResult pl_NeuralNodeF_SetThreshold (PlankNeuralNodeFRef p, const float threshold);
PlankResult pl_NeuralNodeF_SetWeights (PlankNeuralNodeFRef p, const float* weights);
PlankResult pl_NeuralNodeF_SetWeight (PlankNeuralNodeFRef p, const int index, const float weight);
PlankResult pl_NeuralNodeF_Get (PlankNeuralNodeFRef p, float* weights, float* threshold);
float pl_NeuralNodeF_GetThreshold (PlankNeuralNodeFRef p);
PlankResult pl_NeuralNodeF_GetWeights (PlankNeuralNodeFRef p, float* weights);
const float* pl_NeuralNodeF_GetWeightsPtr (PlankNeuralNodeFRef p);
PlankResult pl_NeuralNodeF_Reset (PlankNeuralNodeFRef p, const float amount);
PlankResult pl_NeuralNodeF_Randomise (PlankNeuralNodeFRef p, const float amount);
float pl_NeuralNodeF_Propogate (PlankNeuralNodeFRef p, const float* inputVector);
PlankResult pl_NeuralNodeF_BackProp (PlankNeuralNodeFRef p, const float* inputVector, const float error, const float actFuncOffset, const float learnRate, float* adjustVector);
float pl_NeuralNodeF_GetOutput (PlankNeuralNodeFRef p);

PlankResult pl_NeuralNodeF_ToJSON (PlankNeuralNodeFRef p, json_t* j);
PlankResult pl_NeuralNodeF_InitFromJSON (PlankNeuralNodeFRef p, PlankNeuralNetworkFRef network, json_t* j);


PLANK_END_C_LINKAGE

#if !DOXYGEN

typedef PlankResult (*PlankNeuralNodeFBackPropFunction)(PlankNeuralNodeFRef, const float*, const float, const float, const float, float*);
typedef float (*PlankNeuralNodeFPropogateFunction)(PlankNeuralNodeFRef, const float*);

typedef struct PlankNeuralNodeF
{
    PlankNeuralNetworkFRef network;
    PlankF threshold, output;
    PlankDynamicArray weightVector;
    PlankNeuralNodeFPropogateFunction propogate;
    PlankNeuralNodeFBackPropFunction backProp;
} PlankNeuralNodeF;
#endif


#endif // PLANK_NEURALNODE_H

