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

#include "../../core/plonk_StandardHeader.h"

BEGIN_PLONK_NAMESPACE

#include "../../core/plonk_Headers.h"

TimeStamp::TimeStamp (int initialTime) throw()
:   time (initialTime),
    fraction (0.0)
{
}

TimeStamp::TimeStamp (LongLong initialTime) throw()
:   time (initialTime),
    fraction (0.0)
{
}

TimeStamp::TimeStamp (LongLong initialTime, double initialFraction) throw()
:   time (initialTime),
    fraction (initialFraction)
{
    plonk_assert (fractionIsValid(this->fraction));
}

TimeStamp::TimeStamp (TimeStamp const& other) throw()
:   time (other.time),
    fraction (other.fraction)
{
}

TimeStamp& TimeStamp::operator+= (TimeStamp const& other) throw()
{
    this->time += other.time;
    return operator+= (other.fraction);
}

TimeStamp TimeStamp::operator+ (const double offset) const throw()
{
    TimeStamp copy = *this;
    copy += offset;
    return copy;
}

TimeStamp TimeStamp::operator+ (TimeStamp const& other) const throw()
{
    if (this->isInfinite())
    {
        return *this;
    }
    else if (other.isInfinite())
    {
        return other;
    }
    else
    {
        TimeStamp result (this->time + other.time, this->fraction);
        result += other.fraction;
        return result;
    }
}

TimeStamp& TimeStamp::operator-= (const double offset) throw()
{
    return operator+= (-offset);
}

TimeStamp& TimeStamp::operator-= (TimeStamp const& other) throw()
{
    this->time -= other.time;
    return operator+= (-other.fraction);
}

TimeStamp TimeStamp::operator- (const double offset) const throw()
{
    TimeStamp copy = *this;
    copy += -offset;
    return copy;    
}

TimeStamp TimeStamp::operator- (TimeStamp const& other) const throw()
{
    if (this->isInfinite())
    {
        return *this;
    }
    else if (other.isInfinite())
    {
        return other;
    }
    else
    {
        TimeStamp result (this->time - other.time, this->fraction);
        result += -other.fraction;
        return result;
    }
}

void TimeStamp::set (LongLong newTime, double newFraction) throw()
{
    plonk_assert (newTime >= 0);
    plonk_assert (newFraction >= 0.0);
    plonk_assert (newFraction < 1.0);
    
    time = newTime;
    fraction = newFraction;
}

double TimeStamp::toSamples(const double sampleRate) const throw()
{
    plonk_assert (sampleRate > 0.0);
    const double seconds = this->toSeconds();
    return seconds * sampleRate;
}

bool TimeStamp::operator== (TimeStamp const& other) const throw()
{    
    plonk_assert (fractionIsValid (this->fraction));
    plonk_assert (fractionIsValid (other.fraction));
    
    return (time == other.time) && (fraction == other.fraction);
}

bool TimeStamp::operator!= (TimeStamp const& other) const throw()
{
    plonk_assert (fractionIsValid (this->fraction));
    plonk_assert (fractionIsValid (other.fraction));

    return (time != other.time) || (fraction != other.fraction);
}

bool TimeStamp::operator< (TimeStamp const& other) const throw()
{    
    plonk_assert (fractionIsValid (this->fraction));
    plonk_assert (fractionIsValid (other.fraction));

    if (other.isInfinite())
    {
        if (this->isInfinite())
            return false;
        else
            return true;
    }
    else if (time < other.time)
        return true;
    else if ((time == other.time) && (fraction < other.fraction))
        return true;
    else
        return false;
}

bool TimeStamp::operator<= (TimeStamp const& other) const throw()
{
    plonk_assert (fractionIsValid (this->fraction));
    plonk_assert (fractionIsValid (other.fraction));
    
    if (other.isInfinite())
    {
        if (this->isInfinite())
            return false;
        else
            return true;
    }
    else if (time < other.time)
        return true;
    else if ((time == other.time) && (fraction <= other.fraction))
        return true;
    else
        return false;
}

bool TimeStamp::operator> (TimeStamp const& other) const throw()
{
    plonk_assert (fractionIsValid (this->fraction));
    plonk_assert (fractionIsValid (other.fraction));
    
    if (this->isInfinite())
    {
        if (other.isInfinite())
            return false;
        else
            return true;
    }
    else if (time > other.time)
        return true;
    else if ((time == other.time) && (fraction > other.fraction))
        return true;
    else
        return false;    
}

bool TimeStamp::fractionIsValid (const double f) throw()
{
    if ((f >= 0.0) && (f < 1.0)) 
        return true;
    else if (pl_IsInfD (f))
        return true;
    else
        return false;
}

TimeStamp TimeStamp::fromSeconds (const double seconds) throw()
{
    TimeStamp result;
    result += seconds * getTicks();
    return result;
}

TimeStamp TimeStamp::fromSamples (const double samples, const double sampleRate) throw()
{
    return fromSeconds (samples / sampleRate);    
}

const TimeStamp& TimeStamp::getMaximum() throw()
{
    static const TimeStamp* timeMax = new TimeStamp (TypeUtility<LongLong>::getTypePeak(), 0.0);
    return *timeMax;
}

//

/*PLONK_INLINE_LOW*/ bool TimeStamp::isFinite() const throw()
{
    return ! isInfinite();
}

/*PLONK_INLINE_LOW*/ bool TimeStamp::isInfinite() const throw()
{
    return pl_IsInfD (fraction) != 0;
}

/*PLONK_INLINE_LOW*/ bool TimeStamp::operator>= (TimeStamp const& other) const throw()
{
    plonk_assert (fractionIsValid (this->fraction));
    plonk_assert (fractionIsValid (other.fraction));
    
    if (this->isInfinite())
    {
        if (other.isInfinite())
            return false;
        else
            return true;
    }
    else return ((time > other.time) || ((time == other.time) && (fraction >= other.fraction)));
}

/*PLONK_INLINE_LOW*/ double TimeStamp::getTicks() throw()
{
    static double v = 1000000.0; // microseconds
    return v;
}

/*PLONK_INLINE_LOW*/ double TimeStamp::getReciprocalTicks() throw()
{
    static double v = 1.0 / 1000000.0; // 1/microseconds
    return v;
}

/*PLONK_INLINE_LOW*/ const TimeStamp& TimeStamp::getZero() throw()
{
    static const TimeStamp* timeZero = new TimeStamp (0, 0.0);
    return *timeZero;
}

/*PLONK_INLINE_LOW*/ const TimeStamp& TimeStamp::getSentinel() throw()
{
    static const TimeStamp* sentinel = new TimeStamp (-1, 0.0);
    return *sentinel;
}

/*PLONK_INLINE_LOW*/ TimeStamp& TimeStamp::operator= (TimeStamp const& other) throw()
{
    if (&other != this)
    {
        this->time = other.time;
        this->fraction = other.fraction;
    }
    
    return *this;
}

/*PLONK_INLINE_LOW*/ TimeStamp& TimeStamp::operator+= (const double offset) throw()
{
    if (pl_IsInfD (offset))
    {
        this->time = 0;
        this->fraction = offset;
    }
    else
    {
        const double correctedOffset = offset + this->fraction;
        const LongLong timeOffset = LongLong (correctedOffset);
        this->time += timeOffset;
        this->fraction = correctedOffset - timeOffset; // carry the fraction
        
        if (this->fraction < 0.0)
        {
            this->time--;
            this->fraction += 1.0;
        }
        
        plonk_assert (fractionIsValid (this->fraction));
    }
    
    return *this;
}

END_PLONK_NAMESPACE
