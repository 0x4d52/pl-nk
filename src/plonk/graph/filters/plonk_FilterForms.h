/*
 -------------------------------------------------------------------------------
 This file is part of the Plink, Plonk, Plank libraries
  by Martin Robinson
 
 Copyright University of the West of England, Bristol 2011-12
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
 -------------------------------------------------------------------------------
 */

#ifndef PLONK_FILTERFORMS_H
#define PLONK_FILTERFORMS_H

#include "plonk_FilterForwardDeclarations.h"

template<class SampleType, FilterFormType::Name Form>
struct FilterFormData
{    
    ChannelInternalCore::Data base;
    
    SampleType y1;
};      

template<class SampleType>
struct FilterFormData<SampleType, FilterFormType::P2Z2>
{    
    ChannelInternalCore::Data base;
    
    SampleType y1, y2;
};      

template<class SampleType>
struct FilterFormData<SampleType, FilterFormType::B2>
{    
    ChannelInternalCore::Data base;
    
    SampleType y1, y2;
};      


//------------------------------------------------------------------------------

template<class SampleType, FilterFormType::Name Form>
class FilterFormBase
{
public:    
    static Text getName() throw()
    {
        return FilterFormType::getName (Form);
    }        
};

template<class SampleType, FilterFormType::Name Form>
class FilterForm : public FilterFormBase<SampleType, Form>
{
};

//------------------------------------------------------------------------------

/** One-pole form. 
 Direct Form II implementation. */
template<class SampleType>
class FilterForm<SampleType, FilterFormType::P1>
:   public FilterFormBase<SampleType, FilterFormType::P1>
{
public:
    typedef SampleType                                      SampleDataType;
    typedef UnitBase<SampleType>                            UnitType;
    typedef NumericalArray<SampleType>                      Buffer;
    typedef FilterFormData<SampleType, FilterFormType::P1>  Data;
    
    enum Coeffs
    {
        CoeffA0, 
        CoeffA1,
        CoeffB1, 
        NumCoeffs
    };
        
    static void process (SampleType* const outputSamples,
                         const int outputLength,
                         UnitType& inputUnit, 
                         UnitType& coeffsUnit, 
                         Data& data,
                         ProcessInfo& info, 
                         const int channel) throw()
    {
        const Buffer inputBuffer (inputUnit.process (info, channel));
        const SampleType* const inputSamples = inputBuffer.getArray();
        
        const Buffer a0Buffer (coeffsUnit.process (info, CoeffA0));
        const Buffer a1Buffer (coeffsUnit.process (info, CoeffA1));
        const Buffer b1Buffer (coeffsUnit.process (info, CoeffB1));
        
        const SampleType* const a0Samples = a0Buffer.getArray();
        const SampleType* const a1Samples = a1Buffer.getArray();
        const SampleType* const b1Samples = b1Buffer.getArray();
        
        const int inputLength = inputBuffer.length();
        const int a0Length = a0Buffer.length();
        
        plonk_assert((a0Length == a1Buffer.length()) &&
                     (a0Length == b1Buffer.length()));  // coeff buffers need to be the same length
        
        SampleType y0;
        SampleType y1 = data.y1;
        int i;
        
        if (inputLength == outputLength)
        {
            if (a0Length == inputLength)
            {
                for (i = 0; i < outputLength; ++i)
                {
                    y0 = inputSamples[i] + b1Samples[i] * y1; 
                    outputSamples[i] = a0Samples[i] * y0 + a1Samples[i] * y1;
                    y1 = y0;                    
                }
            }
            else if (a0Length == 1)
            {
                const SampleType a0 = a0Samples[0];
                const SampleType a1 = a1Samples[0];
                const SampleType b1 = b1Samples[0];
                
                for (i = 0; i < outputLength; ++i)
                {                    
                    y0 = inputSamples[i] + b1 * y1; 
                    outputSamples[i] = a0 * y0 + a1 * y1;                    
                    y1 = y0;                    
                }
            }
            else
            {
                double coeffPosition = 0.0;
                const double coeffIncrement = double (a0Length) / double (outputLength);
                
                for (i = 0; i < outputLength; ++i)
                {
                    const int pos (coeffPosition);
                    y0 = inputSamples[i] + b1Samples[pos] * y1; 
                    outputSamples[i] = a0Samples[pos] * y0 + a1Samples[pos] * y1;
                    y1 = y0;
                    coeffPosition += coeffIncrement;
                }
            }
        }
        else
        {
            const SampleType a0 = a0Samples[0];
            const SampleType a1 = a1Samples[0];
            const SampleType b1 = b1Samples[0];
            
            double inputPosition = 0.0;
            const double inputIncrement = double (inputLength) / double (outputLength);
            
            for (i = 0; i < outputLength; ++i)
            {
                y0 = inputSamples[int (inputPosition)] + b1 * y1; 
                outputSamples[i] = a0 * y0 + a1 * y1;
                y1 = y0;
                inputPosition += inputIncrement;
            }            
        }
        
        data.y1 = zap (y1);
    }
};

//------------------------------------------------------------------------------

/** One-pole form, special simplified case for low pass. 
 Direct Form II implementation. */
template<class SampleType>
class FilterForm<SampleType, FilterFormType::P1a>
:   public FilterFormBase<SampleType, FilterFormType::P1a>
{
public:
    typedef SampleType                                          SampleDataType;
    typedef UnitBase<SampleType>                                UnitType;
    typedef NumericalArray<SampleType>                          Buffer;
    typedef FilterFormData<SampleType, FilterFormType::P1a>     Data;

    enum Coeffs
    {
        CoeffB1, 
        NumCoeffs
    };
        
    static void process (SampleType* const outputSamples,
                         const int outputLength,
                         UnitType& inputUnit, 
                         UnitType& coeffsUnit, 
                         Data& data,
                         ProcessInfo& info, 
                         const int channel) throw()
    {
        const Buffer inputBuffer (inputUnit.process (info, channel));
        const SampleType* const inputSamples = inputBuffer.getArray();
        
        const Buffer b1Buffer (coeffsUnit.process (info, CoeffB1));
        const SampleType* const b1Samples = b1Buffer.getArray();
        const int b1Length = b1Buffer.length();
        const int inputLength = inputBuffer.length();
                
        SampleType y0;
        SampleType y1 = data.y1;
        int i;
        
        if (inputLength == outputLength)
        {
            if (b1Length == inputLength)
            {
                for (i = 0; i < outputLength; ++i)
                {                    
                    y0 = inputSamples[i];
                    y1 = y0 + b1Samples[i] * (y1 - y0);
                    outputSamples[i] = y1;
                }
            }
            else if (b1Length == 1)
            {
                const SampleType b1 = b1Samples[0];
                
                for (i = 0; i < outputLength; ++i)
                {                                        
                    y0 = inputSamples[i];
                    y1 = y0 + b1 * (y1 - y0);
                    outputSamples[i] = y1;
                }
            }
            else
            {
                double coeffPosition = 0.0;
                const double coeffIncrement = double (b1Length) / double (outputLength);
                
                for (i = 0; i < outputLength; ++i)
                {
                    y0 = inputSamples[i];
                    y1 = y0 + b1Samples[int (coeffPosition)] * (y1 - y0);
                    outputSamples[i] = y1;
                    coeffPosition += coeffIncrement;
                }
            }
        }
        else
        {
            const SampleType b1 = b1Samples[0];
            
            double inputPosition = 0.0;
            const double inputIncrement = double (inputLength) / double (outputLength);
            
            for (i = 0; i < outputLength; ++i)
            {
                y0 = inputSamples[int (inputPosition)];
                y1 = y0 + b1 * (y1 - y0);
                outputSamples[i] = y1;
                inputPosition += inputIncrement;
            }            
        }
        
        data.y1 = zap (y1);
    }
};

//------------------------------------------------------------------------------

/** Two-pole, two-zero form. 
 Direct Form II implementation. */
template<class SampleType>
class FilterForm<SampleType, FilterFormType::P2Z2>
:   public FilterFormBase<SampleType, FilterFormType::P2Z2>
{
public:
    typedef SampleType                                          SampleDataType;
    typedef UnitBase<SampleType>                                UnitType;
    typedef NumericalArray<SampleType>                          Buffer;
    typedef FilterFormData<SampleType, FilterFormType::P2Z2>    Data;

    enum Coeffs
    {
        CoeffA0, 
        CoeffA1, 
        CoeffA2, 
        CoeffB1, 
        CoeffB2, 
        NumCoeffs
    };
            
    static void process (SampleType* const outputSamples,
                         const int outputLength,
                         UnitType& inputUnit, 
                         UnitType& coeffsUnit, 
                         Data& data,
                         ProcessInfo& info, 
                         const int channel) throw()
    {
        const Buffer inputBuffer (inputUnit.process (info, channel));
        const SampleType* const inputSamples = inputBuffer.getArray();
        
        const Buffer a0Buffer (coeffsUnit.process (info, CoeffA0));
        const Buffer a1Buffer (coeffsUnit.process (info, CoeffA1));
        const Buffer a2Buffer (coeffsUnit.process (info, CoeffA2));
        const Buffer b1Buffer (coeffsUnit.process (info, CoeffB1));
        const Buffer b2Buffer (coeffsUnit.process (info, CoeffB2));
        
        const SampleType* const a0Samples = a0Buffer.getArray();
        const SampleType* const a1Samples = a1Buffer.getArray();
        const SampleType* const a2Samples = a2Buffer.getArray();
        const SampleType* const b1Samples = b1Buffer.getArray();
        const SampleType* const b2Samples = b2Buffer.getArray();
        
        const int inputLength = inputBuffer.length();
        const int a0Length = a0Buffer.length();
        
        plonk_assert((a0Length == a1Buffer.length()) &&
                     (a0Length == a2Buffer.length()) &&
                     (a0Length == b1Buffer.length()) &&
                     (a0Length == b2Buffer.length()));;  // coeff buffers need to be the same length
        
        SampleType y0;
        SampleType y1 = data.y1;
        SampleType y2 = data.y2;
        int i;
        
        if (inputLength == outputLength)
        {
            if (a0Length == inputLength)
            {
                for (i = 0; i < outputLength; ++i)
                {
                    const SampleType inputSample = inputSamples[i];
                    const SampleType a0 = a0Samples[i];
                    const SampleType a1 = a1Samples[i];
                    const SampleType a2 = a2Samples[i];
                    const SampleType b1 = b1Samples[i];
                    const SampleType b2 = b2Samples[i];
                    
                    y0 = inputSample + b1 * y1 + b2 * y2; 
                    outputSamples[i] = a0 * y0 + a1 * y1 + a2 * y2;
                    y2 = y1; 
                    y1 = y0;


//                    y0 = inputSamples[i] + b1Samples[i] * y1 + b2Samples[i] * y2; 
//                    outputSamples[i] = a0Samples[i] * y0 + a1Samples[i] * y1 + a2Samples[i] * y2;
//                    y2 = y1; 
//                    y1 = y0;
                }
            }
            else if (a0Length == 1)
            {
                const SampleType a0 = a0Samples[0];
                const SampleType a1 = a1Samples[0];
                const SampleType a2 = a2Samples[0];
                const SampleType b1 = b1Samples[0];
                const SampleType b2 = b2Samples[0];
                
                for (i = 0; i < outputLength; ++i)
                {
                    y0 = inputSamples[i] + b1 * y1 + b2 * y2; 
                    outputSamples[i] = a0 * y0 + a1 * y1 + a2 * y2;
                    y2 = y1; 
                    y1 = y0;
                }
            }
            else
            {
                double coeffPosition = 0.0;
                const double coeffIncrement = double (a0Length) / double (outputLength);
                
                for (i = 0; i < outputLength; ++i)
                {
                    const int pos (coeffPosition);
                    y0 = inputSamples[i] + b1Samples[pos] * y1 + b2Samples[pos] * y2; 
                    outputSamples[i] = a0Samples[pos] * y0 + a1Samples[pos] * y1 + a2Samples[pos] * y2;
                    y2 = y1; 
                    y1 = y0;
                    coeffPosition += coeffIncrement;
                }
            }
        }
        else
        {
            const SampleType a0 = a0Samples[0];
            const SampleType a1 = a1Samples[0];
            const SampleType a2 = a2Samples[0];
            const SampleType b1 = b1Samples[0];
            const SampleType b2 = b2Samples[0];
            
            double inputPosition = 0.0;
            const double inputIncrement = double (inputLength) / double (outputLength);
            
            for (i = 0; i < outputLength; ++i)
            {
                y0 = inputSamples[int (inputPosition)] + b1 * y1 + b2 * y2; 
                outputSamples[i] = a0 * y0 + a1 * y1 + a2 * y2;
                y2 = y1; 
                y1 = y0;
                inputPosition += inputIncrement;
            }            
        }
        
        data.y1 = zap (y1);
        data.y2 = zap (y2);
    }
};

//------------------------------------------------------------------------------

/** Butterworth 2nd order form. 
 Direct Form II implementation. */
template<class SampleType>
class FilterForm<SampleType, FilterFormType::B2>
:   public FilterFormBase<SampleType, FilterFormType::B2>
{
public:
    typedef SampleType                                          SampleDataType;
    typedef UnitBase<SampleType>                                UnitType;
    typedef NumericalArray<SampleType>                          Buffer;
    typedef FilterFormData<SampleType, FilterFormType::B2>      Data;

    enum Coeffs
    {
        CoeffA0, 
        CoeffA1, 
        CoeffA2, 
        CoeffB1, 
        CoeffB2, 
        NumCoeffs
    };
        
    static void process (SampleType* const outputSamples,
                         const int outputLength,
                         UnitType& inputUnit, 
                         UnitType& coeffsUnit, 
                         Data& data,
                         ProcessInfo& info, 
                         const int channel) throw()
    {
        const Buffer inputBuffer (inputUnit.process (info, channel));
        const SampleType* const inputSamples = inputBuffer.getArray();
        
        const Buffer a0Buffer (coeffsUnit.process (info, CoeffA0));
        const Buffer a1Buffer (coeffsUnit.process (info, CoeffA1));
        const Buffer a2Buffer (coeffsUnit.process (info, CoeffA2));
        const Buffer b1Buffer (coeffsUnit.process (info, CoeffB1));
        const Buffer b2Buffer (coeffsUnit.process (info, CoeffB2));
        
        const SampleType* const a0Samples = a0Buffer.getArray();
        const SampleType* const a1Samples = a1Buffer.getArray();
        const SampleType* const a2Samples = a2Buffer.getArray();
        const SampleType* const b1Samples = b1Buffer.getArray();
        const SampleType* const b2Samples = b2Buffer.getArray();
        
        const int inputLength = inputBuffer.length();
        const int a0Length = a0Buffer.length();
        
        plonk_assert((a0Length == a1Buffer.length()) &&
                     (a0Length == a2Buffer.length()) &&
                     (a0Length == b1Buffer.length()) &&
                     (a0Length == b2Buffer.length()));;  // coeff buffers need to be the same length
        
        SampleType y0;
        SampleType y1 = data.y1;
        SampleType y2 = data.y2;
        int i;
        
        if (inputLength == outputLength)
        {
            if (a0Length == inputLength)
            {
                for (i = 0; i < outputLength; ++i)
                {
                    const SampleType inputSample = inputSamples[i];
                    const SampleType a0 = a0Samples[i];
                    const SampleType a1 = a1Samples[i];
                    const SampleType a2 = a2Samples[i];
                    const SampleType b1 = b1Samples[i];
                    const SampleType b2 = b2Samples[i];

                    y0 = inputSample + b1 * y1 + b2 * y2; 
                    outputSamples[i] = a0 * (y0 + a1 * y1 + a2 * y2);
                    y2 = y1; 
                    y1 = y0;

//                    y0 = inputSamples[i] + b1Samples[i] * y1 + b2Samples[i] * y2; 
//                    outputSamples[i] = a0Samples[i] * (y0 + a1Samples[i] * y1 + a2Samples[i] * y2);
//                    y2 = y1; 
//                    y1 = y0;
                }
            }
            else if (a0Length == 1)
            {
                const SampleType a0 = a0Samples[0];
                const SampleType a1 = a1Samples[0];
                const SampleType a2 = a2Samples[0];
                const SampleType b1 = b1Samples[0];
                const SampleType b2 = b2Samples[0];
                
                for (i = 0; i < outputLength; ++i)
                {
                    y0 = inputSamples[i] + b1 * y1 + b2 * y2; 
                    outputSamples[i] = a0 * (y0 + a1 * y1 + a2 * y2);
                    y2 = y1; 
                    y1 = y0;
                }
            }
            else
            {
                double coeffPosition = 0.0;
                const double coeffIncrement = double (a0Length) / double (outputLength);
                
                for (i = 0; i < outputLength; ++i)
                {
                    const int pos (coeffPosition);
                    y0 = inputSamples[i] + b1Samples[pos] * y1 + b2Samples[pos] * y2; 
                    outputSamples[i] = a0Samples[pos] * (y0 + a1Samples[pos] * y1 + a2Samples[pos] * y2);
                    y2 = y1; 
                    y1 = y0;
                    coeffPosition += coeffIncrement;
                }
            }
        }
        else
        {
            const SampleType a0 = a0Samples[0];
            const SampleType a1 = a1Samples[0];
            const SampleType a2 = a2Samples[0];
            const SampleType b1 = b1Samples[0];
            const SampleType b2 = b2Samples[0];
            
            double inputPosition = 0.0;
            const double inputIncrement = double (inputLength) / double (outputLength);
            
            for (i = 0; i < outputLength; ++i)
            {
                y0 = inputSamples[int (inputPosition)] + b1 * y1 + b2 * y2; 
                outputSamples[i] = a0 * (y0 + a1 * y1 + a2 * y2);
                y2 = y1; 
                y1 = y0;
                inputPosition += inputIncrement;
            }            
        }
        
        data.y1 = zap (y1);
        data.y2 = zap (y2);
    }
};





#endif // PLONK_FILTERFORMS_H