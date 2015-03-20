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

#include "../../core/plonk_StandardHeader.h"

BEGIN_PLONK_NAMESPACE

#include "../../core/plonk_Headers.h"


UnitInfo::UnitInfo() throw()
{
}

UnitInfo::UnitInfo (const char* nameToUse, 
                    const char* detailToUse, ...) throw()
:   name (nameToUse),
    detail (detailToUse)
{
    va_list args;
    va_start(args, detailToUse);
    parseOutputs (args);
    parseInputs (args);
    va_end(args); 
}

void UnitInfo::parseInputs (va_list& args) throw()
{
    parseIO (args, inputInfos);
}

void UnitInfo::parseOutputs (va_list& args) throw()
{
    numOutputs = va_arg(args, int);
    plonk_assert (numOutputs >= -1); // -1 means variable and there will be one info

    parseIO (args, outputInfos);
}

void UnitInfo::parseIO (va_list& args, IOInfos& ioInfos) throw()
{
    IOKey::Name nameKey = IOKey::Invalid;

    do
    {
        nameKey = IOKey::fromInt (va_arg(args, int));
        
        if (nameKey != IOKey::End)
        {            
            Measure::Name measureKey = Measure::fromInt (va_arg(args, int));
            
            const IOKey::IOType type = IOKey::getType (nameKey);
            
            if ((type == IOKey::TypeUnit)       || 
                (type == IOKey::TypeNumerical)  ||
                (type == IOKey::TypeBlockSize)  ||
                (type == IOKey::TypeSampleRate) ||
                (type == IOKey::TypeVariable)   ||
                (type == IOKey::TypeBool))
            {
                const double defaultValue = va_arg(args, double);
                
                const IOLimit::LimitType limit = IOLimit::fromInt (va_arg(args, int));
                Measure::Name limitMeasure;
                double minimum, maximum;
                
                switch (limit) 
                {
                    case IOLimit::Minimum:
                        limitMeasure = Measure::fromInt (va_arg(args, int));
                        minimum = va_arg(args, double);
                        ioInfos.add (IOInfo (nameKey, 
                                             measureKey, 
                                             defaultValue,
                                             IOLimit::createMinimum (limitMeasure,
                                                                     minimum)));
                        break;
                    case IOLimit::Maximum:
                        limitMeasure = Measure::fromInt (va_arg(args, int));
                        maximum = va_arg(args, double);
                        ioInfos.add (IOInfo (nameKey, 
                                             measureKey, 
                                             defaultValue,
                                             IOLimit::createMaximum (limitMeasure,
                                                                     maximum)));
                        break;                 
                    case IOLimit::Clipped:
                        limitMeasure = Measure::fromInt (va_arg(args, int));
                        minimum = va_arg(args, double);
                        maximum = va_arg(args, double);
                        ioInfos.add (IOInfo (nameKey, 
                                             measureKey, 
                                             defaultValue,
                                             IOLimit::createClipped (limitMeasure,
                                                                     minimum, 
                                                                     maximum)));
                        break;                        
                    case IOLimit::None:
                    default:
                        ioInfos.add (IOInfo (nameKey, measureKey, defaultValue)); // no limits
                        break;
                }
            }
            else
            {
                ioInfos.add (IOInfo (nameKey, measureKey)); // other type
            }
        }
        
    } while (nameKey != IOKey::End);
}


const Text& UnitInfo::getName() const throw()
{
    return name;
}

const Text& UnitInfo::getDetail() const throw()
{
    return detail;
}

const IOInfos& UnitInfo::getInputInfos() const throw()
{
    return inputInfos;
}

const IOInfos& UnitInfo::getOutputInfos() const throw()
{
    return outputInfos;
}

int UnitInfo::getNumInputs() const throw()
{
    return inputInfos.length();
}

int UnitInfo::getNumOutputs() const throw()
{
    return numOutputs;
}

Text UnitInfo::getNumOutputsAsText() const throw()
{
    return ChannelCount::getName (numOutputs);
}

Text UnitInfo::getNumInputsAsText() const throw()
{
    return Text::fromValue (inputInfos.length());
}

END_PLONK_NAMESPACE
