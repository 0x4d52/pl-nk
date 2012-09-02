/*
 -------------------------------------------------------------------------------
 This file is part of the Plink, Plonk, Plank libraries
  by Martin Robinson
 
 http://code.google.com/p/pl-nk/
 
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
 
 This software makes use of third party libraries. For more information see:
 doc/license.txt included in the distribution.
 -------------------------------------------------------------------------------
 */

#ifndef PLONK_TIMESTAMP_H
#define PLONK_TIMESTAMP_H

/** Measures time.
 This uses 128 bits (64 in an @c int and 64 in a @c double) to store, measure and
 timestamp blocks and samples. It is used mainly to idenfiy the start time of
 blocks whcih channels use to determine if they need to render yet. 
 @see ProcessInfo */
class TimeStamp
{
public:
    TimeStamp (int initialTime) throw();
    TimeStamp (LongLong initialTime) throw();
    TimeStamp (LongLong initialTime = 0, double initialFraction = 0.0) throw();
    
    TimeStamp (TimeStamp const& other) throw();
    TimeStamp& operator= (TimeStamp const& other) throw();
    
    bool isFinite() const throw();
    bool isInfinite() const throw();
    
    TimeStamp& operator+= (const double offset) throw();
    TimeStamp& operator+= (TimeStamp const& other) throw();
    TimeStamp operator+ (const double offset) const throw();
    TimeStamp operator+ (TimeStamp const& other) const throw();
    
    TimeStamp& operator-= (const double offset) throw();
    TimeStamp& operator-= (TimeStamp const& other) throw();
    TimeStamp operator- (const double offset) const throw();
    TimeStamp operator- (TimeStamp const& other) const throw();
    
    
    void set (LongLong newTime, double newFraction = 0.0) throw();
    inline LongLong getTime() const throw() { return time; }
    inline double getFraction() const throw() { return fraction; }
    inline int getFractionInTicks() const throw() { return int (fraction * getTicks()); }
    
    inline double getValue() const throw() 
    { 
        plonk_assert (((double (time) + 1.0) - (double (time))) == 1.0); // lost precision
        return double (time) + fraction; 
    }
    
    inline operator double () const throw() { return this->getValue(); }
    inline double toSeconds() const throw() { return this->getValue() * getReciprocalTicks(); }
    double toSamples (const double sampleRate) const throw();
    
    bool operator== (TimeStamp const& other) const throw();
    bool operator!= (TimeStamp const& other) const throw();
    
    bool operator<  (TimeStamp const& other) const throw();
    bool operator<= (TimeStamp const& other) const throw();
    bool operator>  (TimeStamp const& other) const throw();
    bool operator>= (TimeStamp const& other) const throw();
    
    static double getTicks() throw();
    static double getReciprocalTicks() throw();
    
    static TimeStamp fromSeconds (const double seconds) throw();
    static TimeStamp fromSamples (const double samples, const double sampleRate) throw();
    static const TimeStamp& getZero() throw();
    static const TimeStamp& getMaximum() throw();

private:
    LongLong time;
    double fraction;
    
    static bool fractionIsValid (const double f) throw();
};

//------------------------------------------------------------------------------

inline bool TimeStamp::isFinite() const throw()
{
    return ! isInfinite();
}

inline bool TimeStamp::isInfinite() const throw()
{
    return pl_IsInfD (fraction) != 0;
}

inline bool TimeStamp::operator>= (TimeStamp const& other) const throw()
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

inline double TimeStamp::getTicks() throw()
{
    return 1000000.0; // microseconds
}

inline double TimeStamp::getReciprocalTicks() throw()
{
    return 1.0 / 1000000.0; // 1/microseconds
}

inline const TimeStamp& TimeStamp::getZero() throw()
{
    static const TimeStamp* timeZero = new TimeStamp (0, 0.0);
    return *timeZero;
}

inline TimeStamp& TimeStamp::operator= (TimeStamp const& other) throw()
{
    if (&other != this)
    {
        this->time = other.time;
        this->fraction = other.fraction;
    }
    
    return *this;
}

inline TimeStamp& TimeStamp::operator+= (const double offset) throw()
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


#endif // PLONK_TIMESTAMP_H