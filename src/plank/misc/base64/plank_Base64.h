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

#ifndef PLANK_BASE64_H
#define PLANK_BASE64_H

#include "../../containers/plank_DynamicArray.h"
#include "../../files/plank_File.h"

typedef struct PlankBase64* PlankBase64Ref;

PlankL pl_Base64EncodedLength (const PlankL binaryLength);
PlankL pl_Base64DecodedLength (const PlankL stringLength);

PlankResult pl_Base64_Init (PlankBase64Ref p);
PlankResult pl_Base64_DeInit (PlankBase64Ref p);
PlankResult pl_Base64_EncodeFile (PlankBase64Ref p, PlankFileRef outputTextFile, PlankFileRef inputBinaryFile);
PlankResult pl_Base64_DecodeFile (PlankBase64Ref p, PlankFileRef outputBinaryFile, PlankFileRef inputTextFile);
const char* pl_Base64_Encode (PlankBase64Ref p, const void* binary, const PlankL binaryLength);
const void* pl_Base64_Decode (PlankBase64Ref p, const char* text, PlankL* binaryLength);
PlankResult pl_Base64_SetBufferSize (PlankBase64Ref p, const PlankL size);
PlankResult pl_Base64_PurgeBuffer (PlankBase64Ref p);


#if !DOXYGEN
typedef struct PlankBase64
{
    PlankDynamicArray buffer;
//    PlankFile inputFile;
//    PlankFile outputFile;
} PlankBase64;
#endif



#endif // PLANK_BASE64_H
