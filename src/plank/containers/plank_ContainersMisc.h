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

#ifndef PLANK_CONTAINERSMISC_H
#define PLANK_CONTAINERSMISC_H

static inline void pl_SwapP (PlankP* a, PlankP* b)
{
    PlankP temp;
    temp = *a;
    *a = *b;
    *b = temp;
}

/** Storage for a 24-bit unsigned integer. */
typedef struct PlankUI24
{
    PlankUC data[3];
} PlankUI24;

/** Storage for a 24-bit integer. */
typedef struct PlankI24
{
    PlankUC data[3];
} PlankI24;

/** Storage for an 80-bit extended floating-point value in big endian format. 
 This is useful mainly for converting the sample rate value stored in the
 header of an AIFF file. */
typedef struct PlankF80
{
    PlankUC data[10];
} PlankF80;

#define PLANK_INT24_MAX 0x7fffff

/** Storage for a 255 character Pascal string. */
typedef struct PlankPascalString255
{
    PlankUC data[256];
} PlankPascalString255;


/** Convert an unsigned 32-bit int into to an unsigned 24-bit int. 
 @ingroup PlankMiscFunctions */
static inline PlankUI24 pl_ConvertUIToUI24 (const PlankUI i32)
{
#if PLANK_LITTLEENDIAN
    PlankUI24 i24;
    i24 = *(PlankUI24*)&i32;
    return i24;
#elif PLANK_BIGENDIAN
    PlankUI24 i24;
    unsigned char* bits;
    
    bits = (unsigned char*)&i32;
    i24 = *(PlankUI24*)(bits + 1);
    return i24;
#else
        #error Neither PLANK_BIGENDIAN or PLANK_LITTLEENDIAN are set to 1
#endif
}


/** Convert an unsigned 24-bit int into to an unsigned 32-bit int. 
 @ingroup PlankMiscFunctions */
static inline PlankUI pl_ConvertUI24ToUI (const PlankUI24 i24)
{
#if PLANK_LITTLEENDIAN
    PlankUI i32;
    i32 = *(PlankUI*)&i24 & 0xffffff;
    return i32;
#elif PLANK_BIGENDIAN
	PlankUI i32;
    i32 = (*(PlankUI*)&i24 >> 8) & 0xffffff;
    return i32;
#else
    #error Neither PLANK_BIGENDIAN or PLANK_LITTLEENDIAN are set to 1
#endif
}

/** Convert a 32-bit int into to a 24-bit int. 
 @ingroup PlankMiscFunctions */
static inline PlankI24 pl_ConvertIToI24 (const PlankI i32)
{
#if PLANK_LITTLEENDIAN
    PlankI24 i24;
    i24 = *(PlankI24*)&i32;
    return i24;
#elif PLANK_BIGENDIAN
    PlankI24 i24;
    unsigned char* bits;
    
    bits = (unsigned char*)&i32;
    i24 = *(PlankI24*)(bits + 1);
    return i24;    
#else
    #error Neither PLANK_BIGENDIAN or PLANK_LITTLEENDIAN are set to 1
#endif
}

/** Convert a 24-bit int into to a 32-bit int. 
 @ingroup PlankMiscFunctions */
static inline PlankI pl_ConvertI24ToI (const PlankI24 i24)
{
#if PLANK_LITTLEENDIAN
    PlankI i32;
    i32 = *(PlankI*)&i24 & 0xffffff;
	    
    if (i32 & 0x00800000)
        return i32 | 0xff000000;
    else
        return i32;
#elif PLANK_BIGENDIAN
	PlankI i32;
    i32 = (*(PlankI*)&i24 >> 8) & 0xffffff;
	
    if (i32 & 0x00800000)
        return i32 | 0xff000000;
    else
        return i32;
#else
    #error Neither PLANK_BIGENDIAN or PLANK_LITTLEENDIAN are set to 1
#endif
}

/** @defgroup PlankBitwiseFunctions Plank bitwise functions
 @ingroup PlankFunctions
 @{
 */

static inline PlankUC pl_CountOnesUC (PlankUC x)
{
    x -= ((x >> 1) & 0x55);
    x = (((x >> 2) & 0x33) + (x & 0x33));
    x = (((x >> 4) + x) & 0x0f);
    return x & 0x0000000f;		
}

static inline PlankUC pl_CountLeadingZerosUC (PlankUC x)
{
    x |= (x >> 1);
    x |= (x >> 2);
    x |= (x >> 4);
    return 8 - pl_CountOnesUC (x);
}

static inline PlankUC pl_CountTrailingZerosUC (PlankUC x) 
{
    return 8 - pl_CountLeadingZerosUC (~x & (x - 1));
}

static inline PlankUC pl_CountLeadingOnesUC (PlankUC x) 
{
    return pl_CountLeadingZerosUC (~x);
}

static inline PlankUC pl_CountTrailingOnesUC (PlankUC x) 
{
    return 8 - pl_CountLeadingZerosUC (x & (~x - 1));
}

static inline PlankUC pl_NumBitsRequiredUC (PlankUC x) 
{
    return 8 - pl_CountLeadingZerosUC (x);
}

static inline PlankUC pl_Log2CeilUC (PlankUC x) 
{
    return 8 - pl_CountLeadingZerosUC (x - 1);
}

static inline PlankUC pl_NextPowerOf2UC (PlankUC x) 
{
    return (PlankUC)1 << pl_Log2CeilUC (x);
}

static inline PlankB pl_IsPowerOf2UC (PlankUC x) 
{
    return (x & (x - 1)) == 0;
}

static inline PlankUS pl_CountOnesUS (PlankUS x)
{
    x -= ((x >> 1) & 0x5555);
    x = (((x >> 2) & 0x3333) + (x & 0x3333));
    x = (((x >> 4) + x) & 0x0f0f);
    x += (x >> 8);
    return x & 0x0000001f;		
}

static inline PlankUS pl_CountLeadingZerosUS (PlankUS x)
{
    x |= (x >> 1);
    x |= (x >> 2);
    x |= (x >> 4);
    x |= (x >> 8);
    return 16 - pl_CountOnesUS (x);
}

static inline PlankUS pl_CountTrailingZerosUS (PlankUS x) 
{
    return 16 - pl_CountLeadingZerosUS (~x & (x - 1));
}

static inline PlankUS pl_CountLeadingOnesUS (PlankUS x) 
{
    return pl_CountLeadingZerosUS (~x);
}

static inline PlankUS pl_CountTrailingOnesUS (PlankUS x) 
{
    return 16 - pl_CountLeadingZerosUS (x & (~x - 1));
}

static inline PlankUS pl_NumBitsRequiredUS (PlankUS x) 
{
    return 16 - pl_CountLeadingZerosUS (x);
}

static inline PlankUS pl_Log2CeilUS (PlankUS x) 
{
    return 16 - pl_CountLeadingZerosUS (x - 1);
}

static inline PlankUS pl_NextPowerOf2US (PlankUS x) 
{
    return 1 << pl_Log2CeilUS (x);
}

static inline PlankB pl_IsPowerOf2US (PlankUS x) 
{
    return (x & (x - 1)) == (PlankUS)0;
}

static inline PlankUI pl_CountOnesUI (PlankUI x)
{
    x -= ((x >> 1) & 0x55555555);
    x = (((x >> 2) & 0x33333333) + (x & 0x33333333));
    x = (((x >> 4) + x) & 0x0f0f0f0f);
    x += (x >> 8);
    x += (x >> 16);
    return x & 0x0000003f;		
}

static inline PlankUI pl_CountLeadingZerosUI (PlankUI x)
{
    x |= (x >> 1);
    x |= (x >> 2);
    x |= (x >> 4);
    x |= (x >> 8);
    x |= (x >> 16);
    return 32 - pl_CountOnesUI (x);
}

static inline PlankUI pl_CountTrailingZerosUI (PlankUI x) 
{
    return 32 - pl_CountLeadingZerosUI (~x & (x - 1));
}

static inline PlankUI pl_CountLeadingOnesUI (PlankUI x) 
{
    return pl_CountLeadingZerosUI (~x);
}

static inline PlankUI pl_CountTrailingOnesUI (PlankUI x) 
{
    return 32 - pl_CountLeadingZerosUI (x & (~x - 1));
}

static inline PlankUI pl_NumBitsRequiredUI (PlankUI x) 
{
    return 32 - pl_CountLeadingZerosUI (x);
}

static inline PlankUI pl_Log2CeilUI (PlankUI x) 
{
    return 32 - pl_CountLeadingZerosUI (x - 1);
}

static inline PlankUI pl_NextPowerOf2UI (PlankUI x) 
{
    return 1LL << pl_Log2CeilUI (x);
}

static inline PlankB pl_IsPowerOf2UI (PlankUI x) 
{
    return (x & (x - 1)) == (PlankUI)0;
}

static inline PlankULL pl_CountOnesULL (PlankULL x)
{
    x -= ((x >> 1) & 0x5555555555555555);
    x = (((x >> 2) & 0x3333333333333333) + (x & 0x3333333333333333));
    x = (((x >> 4) + x) & 0x0f0f0f0f0f0f0f0f);
    x += (x >> 8);
    x += (x >> 16);
    x += (x >> 32);
    return x & 0x0000007f;		
}

static inline PlankULL pl_CountLeadingZerosULL (PlankULL x)
{
    x |= (x >> 1);
    x |= (x >> 2);
    x |= (x >> 4);
    x |= (x >> 8);
    x |= (x >> 16);
    x |= (x >> 32);
    return 64 - pl_CountOnesULL (x);
}

static inline PlankULL pl_CountTrailingZerosULL (PlankULL x) 
{
    return 64 - pl_CountLeadingZerosULL (~x & (x - 1));
}

static inline PlankULL pl_CountLeadingOnesULL (PlankULL x) 
{
    return pl_CountLeadingZerosULL (~x);
}

static inline PlankULL pl_CountTrailingOnesULL (PlankULL x) 
{
    return 64 - pl_CountLeadingZerosULL (x & (~x - 1));
}

static inline PlankULL pl_NumBitsRequiredULL (PlankULL x) 
{
    return 64 - pl_CountLeadingZerosULL (x);
}

static inline PlankULL pl_Log2CeilULL (PlankULL x) 
{
    return 64 - pl_CountLeadingZerosULL (x - 1);
}

static inline PlankULL pl_NextPowerOf2ULL (PlankULL x) 
{
    return 1L << pl_Log2CeilULL (x);
}

static inline PlankB pl_IsPowerOf2ULL (PlankULL x) 
{
    return (x & (x - 1)) == (PlankULL)0;
}

/// @} // End group PlankBitwiseFunctions


/** @defgroup PlankEndianFunctions Plank endian functions
 @ingroup PlankFunctions
 @{
 */

/** Swap the byte order of the unisgned short pointed to by @e bits. */
static inline void pl_SwapEndianUS (unsigned short *bits)
{
    *bits = (*bits >> 8) | (*bits << 8);
}

/** Swap the byte order of the 24-bit int pointed to by @e bits. */
static inline void pl_SwapEndianUI24 (PlankUI24 *bits)
{
    PlankUC temp = bits->data[0];
    bits->data[0] = bits->data[2];
    bits->data[2] = temp;
}

/** Swap the byte order of the unisgned int pointed to by @e bits. */
static inline void pl_SwapEndianUI (unsigned int *bits)
{
    *bits = (*bits >> 24) | ((*bits << 8) & 0x00ff0000) | ((*bits >> 8) & 0x0000ff00) | (*bits << 24);
}

/** Swap the byte order of the unisgned long long pointed to by @e bits. */
static inline void pl_SwapEndianULL (PlankULL *bits)
{    
    *bits = (*bits >> 56) | 
            ((*bits << 40) & 0x00ff000000000000ULL) |
            ((*bits << 24) & 0x0000ff0000000000ULL) |
            ((*bits << 8)  & 0x000000ff00000000ULL) |
            ((*bits >> 8)  & 0x00000000ff000000ULL) |
            ((*bits >> 24) & 0x0000000000ff0000ULL) |
            ((*bits >> 40) & 0x000000000000ff00ULL) |
            (*bits << 56);
}

static inline void pl_SwapEndianF80 (PlankF80 *bits)
{
    PlankF80 temp = *bits;
    
    bits->data[0] = temp.data[9];
    bits->data[1] = temp.data[8];
    bits->data[2] = temp.data[7];
    bits->data[3] = temp.data[6];
    bits->data[4] = temp.data[5];
    bits->data[5] = temp.data[4];
    bits->data[6] = temp.data[3];
    bits->data[7] = temp.data[2];
    bits->data[8] = temp.data[1];
    bits->data[9] = temp.data[0];
}

/** Swap the byte order of the unisgned long pointed to by @e data. */
static inline void pl_SwapEndianUL (PlankUL *data)
{
#if PLANK_32BIT
    pl_SwapEndianUI ((unsigned int*)data);
#elif PLANK_64BIT
    pl_SwapEndianULL ((PlankULL*)data);
#else
    #error Neither PLANK_32BIT or PLANK_64BIT are set to 1
#endif
}

/** Swap the byte order of the short pointed to by @e data. */
static inline void pl_SwapEndianS (short *data)
{
    pl_SwapEndianUS ((unsigned short*)data);
}

/** Swap the byte order of the int pointed to by @e data. */
static inline void pl_SwapEndianI (int *data)
{
    pl_SwapEndianUI ((unsigned int*)data);
}

/** Swap the byte order of the 24-bit int pointed to by @e data. */
static inline void pl_SwapEndianI24 (PlankI24 *data)
{
    pl_SwapEndianUI24 ((PlankUI24*)data);
}

/** Swap the byte order of the long pointed to by @e data. */
static inline void pl_SwapEndianL (long *data)
{
#if PLANK_32BIT
    pl_SwapEndianUI ((unsigned int*)data);
#elif PLANK_64BIT
    pl_SwapEndianULL ((PlankULL*)data);
#else
    #error Neither PLANK_32BIT or PLANK_64BIT are set to 1
#endif    
}

/** Swap the byte order of the long long pointed to by @e data. */
static inline void pl_SwapEndianLL (PlankLL *data)
{
    pl_SwapEndianULL ((PlankULL*)data);
}

/** Swap the byte order of the float pointed to by @e data. */
static inline void pl_SwapEndianF (float *data)
{
    pl_SwapEndianUI ((unsigned int*)data);
}

/** Swap the byte order of the double pointed to by @e data. */
static inline void pl_SwapEndianD (double *data)
{
    pl_SwapEndianULL ((PlankULL*)data);
}

/// @} // End group PlankEndianFunctions


/** Convert four characters to a int as used by IFF file formats to identify data chunks. 
 @ingroup PlankMiscFunctions */
static inline PlankFourCharCode pl_FourCharCode (const char* data)
{
#if PLANK_BIGENDIAN
    pl_SwapEndianI ((int*)data);
#endif
    return *(PlankFourCharCode*)data;
}

/** Convert a positive 80-bit extended float value to an unsigned integer. */
static inline PlankUI pl_F802I (const PlankF80 extended)
{
    PlankUI mantissa;
    PlankUI last = 0;
    PlankUC exp;
    
#if PLANK_LITTLEENDIAN
    pl_SwapEndianUI ((PlankUI*)(extended.data + 2));
#endif
    
    mantissa = *(PlankUI*)(extended.data + 2);
    exp = 30 - *(extended.data + 1);
    
    while (exp)
    {
        last = mantissa;
        mantissa = mantissa >> 1;
        --exp;
    }
    
    if (last & 0x00000001) 
        ++mantissa;
    
    return mantissa;
}

/** Convert an unsigned integer to an 80-bit extended float value. */
static inline PlankF80 pl_I2F80 (const PlankUI value)
{
    PlankF80 extended;
    PlankI temp = value;
    PlankUI exp;
    PlankUC i;
    
    pl_MemoryZero (&extended, sizeof (PlankF80));
    
    if (value == 0)
        goto exit;
    
    exp = temp;
    exp = exp >> 1;
    
    for (i = 0; i < 32; ++i) 
    { 
        exp = exp >> 1;
        
        if (!exp) 
            break;
    }
    
    if (value == -1)
        extended.data[0] = 191;
    else if (value == 1)
        extended.data[0] = 63;
//    else if (value < 0)
//        extended.data[0] = 192;
    else
        extended.data[0] = 64;
    
    extended.data[1] = i;
    
    for (i = 32; i > 0; --i)
    {
        if (temp & 0x80000000) 
            break;
        
        temp <<= 1; 
    } 
        
    *(PlankUI*)(extended.data + 2) = temp;
    
#if PLANK_LITTLEENDIAN
    pl_SwapEndianUI ((PlankUI*)(extended.data + 2));
#endif 
    
exit:
    return extended;
} 

#endif // PLANK_CONTAINERSMISC_H
