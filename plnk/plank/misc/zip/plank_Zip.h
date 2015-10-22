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

#ifndef PLANK_ZIP_H
#define PLANK_ZIP_H

#include "../../containers/plank_DynamicArray.h"
#include "../../files/plank_File.h"

PLANK_BEGIN_C_LINKAGE

typedef struct PlankZip* PlankZipRef;

PlankResult pl_Zip_Init (PlankZipRef p);
PlankResult pl_Zip_DeInit (PlankZipRef p);
PlankResult pl_Zip_DeflateStream (PlankZipRef p, const int amount, PlankFileRef outputZipFile, PlankFileRef inputFile);
PlankResult pl_Zip_InflateStream (PlankZipRef p, PlankFileRef outputFile, PlankFileRef inputZipFile);
const void* pl_Zip_DeflateData (PlankZipRef p, const int amount, const void* dataInput, const PlankL dataInputLength, PlankL* dataOutputLength);
const void* pl_Zip_InflateData (PlankZipRef p, const void* dataInput, const PlankL dataInputLength, PlankL* dataOutputLength);
PlankResult pl_Zip_SetBufferSize (PlankZipRef p, const PlankL size);
PlankResult pl_Zip_PurgeBuffer (PlankZipRef p);

PLANK_END_C_LINKAGE

#if !DOXYGEN
typedef struct PlankZip
{
    PlankDynamicArray buffer;
} PlankZip;
#endif



#endif // PLANK_ZIP_H
