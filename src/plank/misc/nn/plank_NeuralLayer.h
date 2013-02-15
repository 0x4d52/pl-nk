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

#include "../../containers/plank_DynamicArray.h"

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
PlankResult pl_NeuralLayerF_InitNumNodesAndPrevious (PlankNeuralLayerFRef p, const int numNodes, const int numPreviousNodes);
PlankResult pl_NeuralLayerF_InitNumNodesPreviousWithRange (PlankNeuralLayerFRef p, const int numNodes, const int numPreviousNodes, const float range);

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

//class NeuralLayerBaseInternal : public SmartPointer
//{
//public:
//	virtual int getNumNodes() const = 0;
//	virtual int getNumInputs() const = 0;
//	virtual int getNumOutputs() const = 0;
//	
//	virtual void init(const float weightMaximum) = 0;
//	virtual void randomise(const float amount) = 0;
//	virtual void set(const int node, NumericalArray<float> const& weightVector, const float threshold) = 0;
//	virtual void setThreshold(const int node, const float threshold) = 0;
//	virtual void setWeight(const int node, const int weightIndex, const float weight) = 0;
//	virtual void get(const int node, NumericalArray<float> *weightVector, float& threshold) const = 0;
//	
//    //	virtual void write(TextFileWriter const& file) const = 0;
//	
//	virtual NumericalArray<float>& propogate(NumericalArray<float> const& inputVector) = 0;
//	virtual NumericalArray<float>& backProp(NumericalArray<float>& errorVector, const float actFuncOffset, const float learnRate) = 0;
//};
//
//class NeuralLayerSimpleInternal : public NeuralLayerBaseInternal
//{
//public:
//	NeuralLayerSimpleInternal(const int numNodes, const int numNodesOnPreviousLayer) throw();
//	
//	inline int getNumNodes() const throw() { return nodes.size(); }
//	inline int getNumInputs() const throw() { return inputVector.size(); }
//	inline int getNumOutputs() const throw() { return outputVector.size(); }
//	
//	void init(const float weightMaximum) throw();
//	void randomise(const float amount) throw();
//	void set(const int node, NumericalArray<float> const& weightVector, const float threshold) throw();
//	void setThreshold(const int node, const float threshold) throw();
//	void setWeight(const int node, const int weightIndex, const float weight) throw();
//	void get(const int node, NumericalArray<float> *weightVector, float& threshold) const throw();
//	
//    //	void write(TextFileWriter const& file) const throw();
//	
//	NumericalArray<float>& propogate(NumericalArray<float> const& inputVector) throw();
//	NumericalArray<float>& backProp(NumericalArray<float>& errorVector, const float actFuncOffset, const float learnRate) throw();
//	
//private:
//	NeuralNodeArray nodes;
//	NumericalArray<float> outputVector;
//	NumericalArray<float> inputVector;
//	NumericalArray<float> adjustVector;
//};
//
//class NeuralLayer : public SmartPointerContainer<NeuralLayerBaseInternal>
//{
//public:
//	NeuralLayer(const int null = 0) throw()
//	:	SmartPointerContainer<NeuralLayerBaseInternal>(0)
//	{ (void)null;}
//	
//	NeuralLayer(const int numNodes, const int numNodesOnPreviousLayer) throw()
//	:	SmartPointerContainer<NeuralLayerBaseInternal>
//    (new NeuralLayerSimpleInternal(numNodes, numNodesOnPreviousLayer))
//	{
//	}
//	
//	inline int getNumNodes() const throw()
//	{
//		if(getInternal() != 0)
//		{
//			return getInternal()->getNumNodes();
//		}
//		else return 0;
//	}
//	
//	inline int getNumInputs() const throw()
//	{
//		if(getInternal() != 0)
//		{
//			return getInternal()->getNumInputs();
//		}
//		else return 0;
//	}
//	
//	inline int getNumOutputs() const throw()
//	{
//		if(getInternal() != 0)
//		{
//			return getInternal()->getNumOutputs();
//		}
//		else return 0;
//	}
//	
//	inline void init(const float weightMaximum) throw()
//	{
//		if(getInternal() != 0)
//		{
//			getInternal()->init(weightMaximum);
//		}
//	}
//	
//	inline void randomise(const float amount) throw()
//	{
//		if(getInternal() != 0)
//		{
//			getInternal()->randomise(amount);
//		}
//	}
//	
//	inline void set(const int node, NumericalArray<float> const& weightVector, const float threshold) throw()
//	{
//		if(getInternal() != 0)
//		{
//			getInternal()->set(node, weightVector, threshold);
//		}
//	}
//	
//	void setThreshold(const int node, const float threshold) throw()
//	{
//		if(getInternal() != 0)
//		{
//			getInternal()->setThreshold(node, threshold);
//		}
//	}
//	
//	void setWeight(const int node, const int weightIndex, const float weight) throw()
//	{
//		if(getInternal() != 0)
//		{
//			getInternal()->setWeight(node, weightIndex, weight);
//		}
//	}
//	
//	inline void get(const int node, NumericalArray<float> *weightVector, float& threshold) const throw()
//	{
//		if(getInternal() != 0)
//		{
//			getInternal()->get(node, weightVector, threshold);
//		}
//	}
//	
//    //	inline void write(TextFileWriter const& file) const throw()
//    //	{
//    //		if(getInternal() != 0)
//    //		{
//    //			getInternal()->write(file);
//    //		}
//    //	}
//	
//	inline NumericalArray<float> propogate(NumericalArray<float> const& inputVector) throw()
//	{
//		if(getInternal() != 0)
//		{
//			return getInternal()->propogate(inputVector);
//		}
//		else return NumericalArraySpec(0, false);
//            }
//	
//	inline NumericalArray<float> backProp(NumericalArray<float>& errorVector, const float actFuncOffset, const float learnRate) throw()
//	{
//		if(getInternal() != 0)
//		{
//			return getInternal()->backProp(errorVector, actFuncOffset, learnRate);
//		}
//		else return NumericalArraySpec(0, false);
//            }
//	
//};
//
//class NeuralLayerArray : public ObjectArray<NeuralLayer>
//{
//public:
//	NeuralLayerArray(const int size)
//	:	ObjectArray<NeuralLayer> (size, false)
//	{
//	}
//    
//	ObjectArrayAssignmentDefinition(NeuralLayerArray, NeuralLayer)
//};
