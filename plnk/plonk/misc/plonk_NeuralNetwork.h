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

#ifndef PLONK_NEURALNETWORK_H
#define PLONK_NEURALNETWORK_H

#include "../core/plonk_CoreForwardDeclarations.h"
#include "../containers/plonk_ContainerForwardDeclarations.h"

#include "../core/plonk_SmartPointer.h"
#include "../core/plonk_WeakPointer.h"

#include "plonk_JSON.h"

#define PLANK_NEURALPATTERNF_JSON_TYPE           "plank::NeuralPatternF"
#define PLANK_NEURALPATTERNF_JSON_VERSION        0, 1, 1, 0
#define PLANK_NEURALPATTERNF_JSON_INPUT          "input"
#define PLANK_NEURALPATTERNF_JSON_TARGET         "target"

template<class ValueType> class NeuralNetworkBase;

template<class ValueType>                                               
class NeuralNetworkInternal : public SmartPointer
{
};

template<>
class NeuralNetworkInternal<float> : public SmartPointer
{
public:
    typedef NeuralNetworkBase<float>            NetworkType;
    typedef NumericalArray<float>               VectorType;
    typedef PlankNeuralNetworkFActFunction      ActFunc;

    NeuralNetworkInternal (IntArray const& layers, const float range) throw()
    {
        ResultCode result = pl_NeuralNetworkF_InitWithLayersAndRange (&network, layers.getArray(), layers.length(), range);
        plonk_assert (result == PlankResult_OK);
        
        networkOutputs.referTo (pl_NeuralNetworkF_GetNumOutputs (&network),
                                const_cast<float*> (pl_NeuralNetworkF_GetOutputsPtr (&network)));
         
#ifndef PLONK_DEBUG
        (void)result;
#endif
    }
    
    NeuralNetworkInternal (PlankJSONRef json) throw()
    {
        ResultCode result = pl_NeuralNetworkF_InitFromJSON (&network, json);
        plonk_assert (result == PlankResult_OK);
        
        networkOutputs.referTo (pl_NeuralNetworkF_GetNumOutputs (&network),
                                const_cast<float*> (pl_NeuralNetworkF_GetOutputsPtr (&network)));
        
#ifndef PLONK_DEBUG
        (void)result;
#endif
    }
    
    ~NeuralNetworkInternal()
    {
        ResultCode result = pl_NeuralNetworkF_DeInit (&network);
        plonk_assert (result == PlankResult_OK);

#ifndef PLONK_DEBUG
        (void)result;
#endif
    }
    
    IntArray getStructure() const throw()
    {
        IntArray structure;
        
        structure.add (this->getNumInputs());
        
        const int numLayers = this->getNumLayers() - 1;
        
        for (int i = 0; i < numLayers; ++i)
        {
            PlankNeuralLayerFRef layer = 0;
            
            ResultCode result = pl_NeuralNetworkF_GetLayer (const_cast<PlankNeuralNetworkF*> (&network), i, &layer);

            if (result == 0)
            {
                const int numNodes = pl_NeuralLayerF_GetNumOutputs (layer);
                structure.add (numNodes);
            }
        }
        
        return structure;
    }
   
private:
    PLONK_INLINE_LOW int getNumInputs() const throw() { return pl_NeuralNetworkF_GetNumInputs (const_cast<PlankNeuralNetworkF*> (&network)); }
    PLONK_INLINE_LOW int getNumOutputs() const throw() { return pl_NeuralNetworkF_GetNumOutputs (const_cast<PlankNeuralNetworkF*> (&network)); }
    PLONK_INLINE_LOW int getNumLayers() const throw() { return pl_NeuralNetworkF_GetNumLayers (const_cast<PlankNeuralNetworkF*> (&network)); }

    PLONK_INLINE_LOW const VectorType& getOutput() const throw() { return networkOutputs; }
    
    PLONK_INLINE_LOW const VectorType& propogate (VectorType const& inputs) throw()
    {
        plonk_assert (inputs.length() == this->getNumInputs());
        ResultCode result = pl_NeuralNetworkF_Propogate (&network, inputs.getArray());
        plonk_assert (result == PlankResult_OK);

#ifndef PLONK_DEBUG
        (void)result;
#endif
        
        return networkOutputs;
    }
    
    PLONK_INLINE_LOW void propogate (VectorType& outputs, VectorType const& inputs) throw()
    {
        ResultCode result;
        
        plonk_assert (inputs.length() == this->getNumInputs());
        result = pl_NeuralNetworkF_Propogate (&network, inputs.getArray());
        plonk_assert (result == PlankResult_OK);
        
        outputs.setSize (this->networkOutputs.length(), false);
        VectorType::copyData (outputs.getArray(),
                              this->networkOutputs.getArray(),
                              this->networkOutputs.length());
        
#ifndef PLONK_DEBUG
        (void)result;
#endif
    }

    PLONK_INLINE_LOW void backProp (VectorType const& inputs, VectorType const& targets) throw()
    {
        plonk_assert (inputs.length() == this->getNumInputs());
        plonk_assert (targets.length() == this->getNumOutputs());
        
        ResultCode result = pl_NeuralNetworkF_BackProp (&network, inputs.getArray(), targets.getArray());
        plonk_assert (result == PlankResult_OK);
        
#ifndef PLONK_DEBUG
        (void)result;
#endif
    }
    
    PLONK_INLINE_LOW void setActFunc (ActFunc const& function) throw()
    {
        ResultCode result = pl_NeuralNetworkF_SetActFunc (&network, function);
        plonk_assert (result == PlankResult_OK);
        
#ifndef PLONK_DEBUG
        (void)result;
#endif
    }
    
    PLONK_INLINE_LOW void toJSON (PlankJSONRef json, const bool useBinary) throw()
    {
        ResultCode result = pl_NeuralNetworkF_ToJSON (&network, json, useBinary);
        plonk_assert (result == PlankResult_OK);
        
#ifndef PLONK_DEBUG
        (void)result;
#endif
    }
    
    friend class NeuralNetworkBase<float>;
    
    PlankNeuralNetworkF network;
    VectorType networkOutputs;
};


//------------------------------------------------------------------------------

/** @ingroup PlonkMiscClasses */
template<class ValueType>                                               
class NeuralNetworkBase : public SmartPointerContainer<NeuralNetworkInternal<ValueType> >
{
public:
        
    typedef NeuralNetworkInternal<ValueType>            Internal;
    typedef SmartPointerContainer<Internal>             Base;
    typedef WeakPointerContainer<NeuralNetworkBase>     Weak;
    typedef typename Internal::VectorType               VectorType;
    typedef typename Internal::ActFunc                  ActFunc;
    
    class Pattern
    {
    public:
        typedef ObjectArray<Pattern> Patterns;

        Pattern() throw() { }
        Pattern (VectorType const& input, VectorType const& target) throw()
        :   i (input.deepCopy()), t (target.deepCopy())
        {
        }
        
        Pattern (JSON const& json) throw()
        {            
            if (!json.isObject()) return;
            if (!json.isObjectType (PLANK_NEURALPATTERNF_JSON_TYPE)) return;
            if (json.getVersion() > JSON::versionCode (PLANK_NEURALNETWORKF_JSON_VERSION)) return;
            
            JSON inputJson = json[PLANK_NEURALPATTERNF_JSON_INPUT];
            JSON targetJson = json[PLANK_NEURALPATTERNF_JSON_TARGET];
            
            i = inputJson;
            t = targetJson;
        }
        
        const VectorType& getInput() const throw() { return i; }
        const VectorType& getTarget() const throw() { return t; }
        int getInputLength() const throw() { return i.length(); }
        int getTargetLength() const throw() { return t.length(); }
        
        void toJSON (JSON& json, const bool useBinary = false) const throw()
        {
            JSON jsonPattern = JSON::object();            
            jsonPattern.setType (PLANK_NEURALPATTERNF_JSON_TYPE);
            jsonPattern.setVersionString (PLANK_NEURALPATTERNF_JSON_VERSION);
            jsonPattern.add (PLANK_NEURALPATTERNF_JSON_INPUT, JSON (i, useBinary));
            jsonPattern.add (PLANK_NEURALPATTERNF_JSON_TARGET, JSON (t, useBinary));
            json.add (jsonPattern);
        }
        
        static void patternsToJSON (JSON& json, Patterns const& patterns, const bool useBinary = false) throw()
        {
            const int numPatterns = patterns.length();
            const Pattern* patternArray = patterns.getArray();
            
            for (int j = 0; j < numPatterns; ++j)
            {
                patternArray[j].toJSON (json, useBinary);
            }
        }
        
        friend class NeuralNetworkBase;
        
    private:
        VectorType i;
        VectorType t;
    };
    
    typedef typename Pattern::Patterns Patterns;
    
    NeuralNetworkBase() throw()
    :   Base (new Internal (IntArray (1, 1, 1), 0.1f))
    {
    }
    
    NeuralNetworkBase (IntArray const& layers, const float range = 0.1f) throw()
    :   Base (new Internal (layers, range))
    {
    }
    
//    NeuralNetworkBase (PlankJSONRef json) throw()
//    :   Base (new Internal (json))
//    {
//    }

    NeuralNetworkBase (JSON const& json) throw()
    :   Base (new Internal (json.getInternal()))
    {
    }

    explicit NeuralNetworkBase (Internal* internalToUse) throw()
	:	Base (internalToUse)
	{
	} 
    
    /** Get a weakly linked copy of this object. 
     This will return a blank/empty/null object of this type if
     the original has already been deleted. */    
    static NeuralNetworkBase fromWeak (Weak const& weak) throw()
    {
        return weak.fromWeak();
    }    
    
    /** Copy constructor.
	 Note that a deep copy is not made, the copy will refer to exactly the same data. */
    NeuralNetworkBase (NeuralNetworkBase const& copy) throw()
    :   Base (static_cast<Base const&> (copy))
    {
    }
    
//    NeuralNetworkBase (Dynamic const& other) throw()
//    :   Base (other.as<NeuralNetworkBase>().getInternal())
//    {
//    }    
    
    /** Assignment operator. */
    NeuralNetworkBase& operator= (NeuralNetworkBase const& other) throw()
	{
		if (this != &other)
            this->setInternal (other.getInternal());
        
        return *this;
	}
    
    PLONK_INLINE_LOW const VectorType& propogate (VectorType const& inputs) throw()
    {
        return this->getInternal()->propogate (inputs);
    }
    
    PLONK_INLINE_LOW void propogate (VectorType& outputs, VectorType const& inputs) throw()
    {
        return this->getInternal()->propogate (outputs, inputs);
    }

    PLONK_INLINE_LOW void backProp (VectorType const& inputs, VectorType const& targets) throw()
    {
        return this->getInternal()->backProp (inputs, targets);
    }
    
    PLONK_INLINE_LOW void train (Patterns const& patterns, const int numEpochs) throw()
    {
        const int numPatterns = patterns.length();
        const Pattern* patternArray = patterns.getArray();
        
        for (int i = 0; i < numEpochs; ++i)
        {
            for (int j = 0; j < numPatterns; ++j)
                this->backProp (patternArray[j].i, patternArray[j].t);
        }
    }
    
    void reset (const ValueType amount = 0.1f) throw()
    {
        pl_NeuralNetworkF_Reset (&this->getInternal()->network, amount);
    }
    
    void randomise (const ValueType amount = 0.1f) throw()
    {
        pl_NeuralNetworkF_Randomise (&this->getInternal()->network, amount);
    }
    
    PLONK_INLINE_LOW void setActFunc (ActFunc const& function) throw()
    {
        return this->getInternal()->setActFunc (function);
    }
    
//    void toJSON (PlankJSONRef json, const bool useBinary = false) throw()
//    {
//        this->getInternal()->toJSON (json, useBinary);
//    }

    void toJSON (JSON& json, const bool useBinary = false) throw()
    {
        this->getInternal()->toJSON (json.getInternal(), useBinary);
    }
    
    int getNumInputs() const throw()
    {
        return this->getInternal()->getNumInputs();
    }
    
    int getNumOutputs() const throw()
    {
        return this->getInternal()->getNumOutputs();
    }
    
    IntArray getStructure() const throw()
    {
        return this->getInternal()->getStructure();
    }
    
    PLONK_OBJECTARROWOPERATOR(NeuralNetworkBase);
};

typedef NeuralNetworkBase<float> FloatNeuralNetwork;


#endif // PLONK_NEURALNETWORK_H
