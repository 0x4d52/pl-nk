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

#ifndef PLONK_NEURALNETWORK_H
#define PLONK_NEURALNETWORK_H

#include "../../core/plonk_CoreForwardDeclarations.h"
#include "../../containers/plonk_ContainerForwardDeclarations.h"

#include "../../core/plonk_SmartPointer.h"
#include "../../core/plonk_WeakPointer.h"

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
    
    NeuralNetworkInternal (json_t* json) throw()
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
   
private:
    inline int getNumInputs() const throw() { return pl_NeuralNetworkF_GetNumInputs (const_cast<PlankNeuralNetworkF*> (&network)); }
    inline int getNumOutputs() const throw() { return pl_NeuralNetworkF_GetNumOutputs (const_cast<PlankNeuralNetworkF*> (&network)); }
    inline const VectorType& getOutput() const throw() { return networkOutputs; }
    
    inline const VectorType& propogate (VectorType const& inputs) throw()
    {
        plonk_assert (inputs.length() == this->getNumInputs());
        ResultCode result = pl_NeuralNetworkF_Propogate (&network, inputs.getArray());
        plonk_assert (result == PlankResult_OK);

#ifndef PLONK_DEBUG
        (void)result;
#endif
        
        return networkOutputs;
    }
    
    inline void propogate (VectorType& outputs, VectorType const& inputs) throw()
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

    inline void backProp (VectorType const& inputs, VectorType const& targets) throw()
    {
        plonk_assert (inputs.length() == this->getNumInputs());
        plonk_assert (targets.length() == this->getNumOutputs());
        
        ResultCode result = pl_NeuralNetworkF_BackProp (&network, inputs.getArray(), targets.getArray());
        plonk_assert (result == PlankResult_OK);
        
#ifndef PLONK_DEBUG
        (void)result;
#endif
    }
    
    inline void setActFunc (ActFunc const& function) throw()
    {
        ResultCode result = pl_NeuralNetworkF_SetActFunc (&network, function);
        plonk_assert (result == PlankResult_OK);
        
#ifndef PLONK_DEBUG
        (void)result;
#endif
    }
    
    inline void toJSON (json_t* json) throw()
    {
        ResultCode result = pl_NeuralNetworkF_ToJSON (&network, json);
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
        Pattern() throw() { }
        Pattern (VectorType const& input, VectorType const& target) throw()
        :   i (input), t (target)
        {
        }
        
        friend class NeuralNetworkBase;
        
    private:
        VectorType i;
        VectorType t;
    };
    
    typedef ObjectArray<Pattern> Patterns;
    
    NeuralNetworkBase (IntArray const& layers, const float range = 0.1f) throw()
    :   Base (new Internal (layers, range))
    {
    }
    
    NeuralNetworkBase (json_t* json) throw()
    :   Base (new Internal (json))
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
    
    inline const VectorType& propogate (VectorType const& inputs) throw()
    {
        return this->getInternal()->propogate (inputs);
    }
    
    inline void propogate (VectorType& outputs, VectorType const& inputs) throw()
    {
        return this->getInternal()->propogate (outputs, inputs);
    }

    inline void backProp (VectorType const& inputs, VectorType const& targets) throw()
    {
        return this->getInternal()->backProp (inputs, targets);
    }
    
    inline void train (Patterns const& patterns, const int numEpochs) throw()
    {
        const int numPatterns = patterns.length();
        const Pattern* patternArray = patterns.getArray();
        
        for (int i = 0; i < numEpochs; ++i)
        {
            for (int j = 0; j < numPatterns; ++j)
                this->backProp (patternArray[j].i, patternArray[j].t);
        }
    }
    
    inline void setActFunc (ActFunc const& function) throw()
    {
        return this->getInternal()->setActFunc (function);
    }
    
    inline void toJSON (json_t* json) throw()
    {
        this->getInternal()->toJSON (json);
    }
    
    PLONK_OBJECTARROWOPERATOR(NeuralNetworkBase);
};

typedef NeuralNetworkBase<float> FloatNeuralNetwork;


#endif // PLONK_NEURALNETWORK_H
