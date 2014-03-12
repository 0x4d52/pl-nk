/*
 -------------------------------------------------------------------------------
 This file is part of the Plink, Plonk, Plank libraries
  by Martin Robinson
 
 http://code.google.com/p/pl-nk/
 
 Copyright University of the West of England, Bristol 2011-14
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

#ifndef PLONK_WAVETABLE_H
#define PLONK_WAVETABLE_H

#include "../core/plonk_CoreForwardDeclarations.h"
#include "plonk_ContainerForwardDeclarations.h"
#include "plonk_DynamicContainer.h"

#include "../core/plonk_SmartPointer.h"
#include "../core/plonk_WeakPointer.h"
#include "plonk_ObjectArray.h"
#include "plonk_SimpleArray.h"


/** @ingroup PlonkContainerClasses */
template<class SampleType>
class WavetableBase : private NumericalArray<SampleType>
{
public:
    typedef NumericalArray<SampleType> Buffer;
    
    WavetableBase() throw()
    {
    }
    
    WavetableBase (Buffer const& buffer) throw()
    :   Buffer (Buffer::withSize (buffer.length() * 2, false))
    {
        const int bufferLength = buffer.length();
        plonk_assert (bufferLength > 0);
        
        const SampleType* const bufferSamples = buffer.getArray();     
        SampleType* tableSamples = Buffer::getArray();
        
        int i;
        
        // copy twice (could probably get away with copying just the first four samples
        // for most interpolation techniques)
        
        for (i = 0; i < bufferLength; ++i)
            *tableSamples++ = bufferSamples[i];
        
        for (i = 0; i < bufferLength; ++i)
            *tableSamples++ = bufferSamples[i];
    }
        
    /** Copy constructor.
	 Note that a deep copy is not made, the copy will refer to exactly the same data. */
    WavetableBase (WavetableBase const& copy) throw()
    :   Buffer (static_cast<Buffer const&> (copy))
    {
    }
    
    WavetableBase (Dynamic const& other) throw()
    :   Buffer (other.as<WavetableBase>().getInternal())
    {
    }    
    
    /** Assignment operator. */
    WavetableBase& operator= (WavetableBase const& other) throw()
	{
		if (this != &other)
            this->setInternal (other.getInternal());//this->setInternal (other.containerCopy().getInternal());
        
        return *this;
	}
    
    using Buffer::getInternal;
    
    WavetableBase copy() throw()
    {
        Buffer buffer (this->length(), this->getArray(), false, false);
        return WavetableBase (buffer);
    }
        
    int length() const throw()
    {
        return Buffer::length() / 2;
    }
    
    int size() const throw()
    { 
        return Buffer::size();
    }
        
    const SampleType* getArray() const throw()
    {
        return Buffer::getArray();
    }
    
    SampleType operator[] (const int index) const throw()
    {
        if ((index < 0) || (index >= this->length()))
            return SampleType (0);
        else
            return this->getArray()[index];
    }
    
    SampleType atUnchecked (const int index) const throw()
    {
        plonk_assert ((index >= 0) && (index < this->length()));
        return this->getArray()[index];
    }

    void put (const int index, SampleType const& value) throw()
    {
        const int tableLength = this->length();
        plonk_assert ((index >= 0) && (index < tableLength));
        SampleType* const tableSamples = Buffer::getArray();
        tableSamples[index] = value;
        tableSamples[index + tableLength] = value;
    }
    
    WavetableBase& normalise() throw()
    {
        Buffer::normalise();
        return *this;
    }
    
    static WavetableBase sineTable (const int size, const double repeats = 1.0) throw()
	{
		return WavetableBase (Buffer::sineTable (size, repeats));
	}
    	
	/** Creates a Wavetable with a given size (length) containing one or more cosine tables. */
	static WavetableBase cosineTable (const int size, const double repeats = 1.0) throw()
	{
		return WavetableBase (Buffer::cosineTable (size, repeats));
	}
	
	/** Creates a Wavetable with a given size (length) containing one or more cosine window. */
	static WavetableBase cosineWindow (const int size, const double repeats = 1.0) throw()
	{
		return WavetableBase (Buffer::cosineWindow (size, repeats));
	}
	
	/** Creates a Wavetable with a given size (length) containing one or more triangle windows. */
	static WavetableBase triangleWindow (const int size) throw()
	{
		return WavetableBase (Buffer::triangleWindow (size));
	}
	
	/** Creates a Wavetable with a given size (length) containing one or more Bartlett windows. */
	static WavetableBase bartlettWindow (const int size) throw()
	{
		return WavetableBase (Buffer::bartlettWindow (size));
	}
	
	/** Creates a Wavetable with a given size (length) containing one or more von Hann (Hanning) windows. */
	static WavetableBase hannWindow (const int size) throw()
	{
		return WavetableBase (Buffer::hannWindow (size));
	}
	
	/** Creates a Wavetable with a given size (length) containing one or more Hamming windows. */
	static WavetableBase hammingWindow (const int size) throw()
	{
		return WavetableBase (Buffer::hammingWindow (size));
	}
	
	/** Creates a Wavetable with a given size (length) containing one or more Blackman windows. */
	static WavetableBase blackmanWindow (const int size, const double alpha = 0.16) throw()
    {
		return WavetableBase (Buffer::blackmanWindow (size, alpha));
    }
    
    static const WavetableBase& sine512() throw()
    {
        static const WavetableBase table (Buffer::sineTable (512, 1.0));
        return table;
    }    
        
    static const WavetableBase& sine8192() throw()
    {
        static const WavetableBase table (Buffer::sineTable (8192, 1.0));        
        return table;        
    }
    
    static const WavetableBase& cosine512() throw()
    {
        static const WavetableBase table (Buffer::cosineTable (512, 1.0));
        return table;
    }
    
    static const WavetableBase& cosine8192() throw()
    {
        static const WavetableBase table (Buffer::cosineTable (8192, 1.0));
        return table;
    }
    
    static WavetableBase harmonic (const int size, Buffer const& weights) throw()
	{
		return WavetableBase (Buffer::harmonicTable (size, weights));
	}

    static WavetableBase harmonic8192 (Buffer const& weights) throw()
	{
		return WavetableBase (Buffer::harmonicTable (8192, weights));
	}
    
    static const WavetableBase& harmonicSaw() throw()
	{
		static const WavetableBase table (WavetableBase::harmonic8192 (Buffer::series (21, 1, 1).reciprocal()).normalise());
        return table;
	}

    static WavetableBase harmonicSaw (const int size, const int numHarmonics = 21) throw()
	{
		return WavetableBase::harmonic (size, Buffer::series (numHarmonics, 1, 1)
                                        .reciprocal()).normalise();
	}
    
    static const WavetableBase& harmonicSquare() throw()
	{
		static const WavetableBase table (WavetableBase::harmonic8192 ((Buffer::series (21, 1, 1).reciprocal() * Buffer (SampleType (1), SampleType (0)))).normalise());
        return table;
	}
    
    static WavetableBase harmonicSquare (const int size, const int numHarmonics = 21) throw()
	{
        plonk_assert (numHarmonics >= 2);
		return WavetableBase::harmonic (size, (Buffer::series (numHarmonics, 1, 1).reciprocal()  * Buffer (SampleType (1), SampleType (0)))
                                        .normalise());
	}
    
    static const WavetableBase& harmonicTri() throw()
	{
		static const WavetableBase table (WavetableBase::harmonic8192 ((Buffer::series (21, 1, 1).reciprocal() * Buffer (SampleType (1), SampleType (0), SampleType (-1), SampleType (0)))
                                                                       .squared()).normalise());
        return table;
	}
    
    static WavetableBase harmonicTri (const int size, const int numHarmonics = 21) throw()
	{
        plonk_assert (numHarmonics >= 4);
		return WavetableBase::harmonic (size, (Buffer::series (numHarmonics, 1, 1).reciprocal()  * Buffer (SampleType (1), SampleType (0), SampleType (-1), SampleType (0)))
                                        .squared())
                                        .normalise();
	}

    
    PLONK_OBJECTARROWOPERATOR(WavetableBase);
    
    void print (const char *prefix = 0, const bool oneLine = false) const throw()
	{
#if !PLONK_ANDROID
        int i;
        
		if (oneLine)
		{
			if (prefix) std::cout << prefix << ": ";
                
                for (i = 0; i < this->length(); i++)
			{
				std::cout << this->at (i) << " ";
			}
                
            std::cout << std::endl;
        }
        else
        {
            for (i = 0; i < this->length(); i++)
            {
                if (prefix) std::cout << prefix;
                    
                std::cout << "[" << i << "] = " << this->at (i) << std::endl;
            }
        }
#endif
    }

};

#endif // PLONK_WAVETABLE_H
