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

#include "../../core/plank_StandardHeader.h"
#include "plank_Base64.h"

#include <math.h>
#include <stdint.h>
#include <stdlib.h>

typedef struct PlankBase64Tables
{
    const char decoding[256];
    const char encoding[64 + 16];
} PlankBase64Tables;

PlankL pl_Base64EncodedLength (const PlankL inputLength)
{    
    return (inputLength % 3) ? (4 * (inputLength / 3 + 1)) : (4 * (inputLength / 3));
}

PlankL pl_Base64DecodedLength (const PlankL inputLength)
{
    return inputLength / 4 * 3;
}

static void pl_Base64TablesInit (PlankBase64Tables* tables)
{
    int i;
    
    // overflow by 1 on the null is OK as long as mod is after it and done last
    strcpy ((char*)tables->encoding, "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/");

    for (i = 0; i < 64; ++i)
        ((char*)(tables->decoding))[tables->encoding[i]] = i;
}

static const PlankBase64Tables* pl_Base64Tables()
{
    static PlankB firstTime = PLANK_TRUE;
    static PlankBase64Tables tables;
    
    if (firstTime)
    {
        pl_Base64TablesInit (&tables);
        firstTime = PLANK_FALSE;
    }
    
    return &tables;
}

PlankResult pl_Base64_Init (PlankBase64Ref p)
{
    if (p == PLANK_NULL)
        return PlankResult_MemoryError;
        
    return pl_DynamicArray_InitWithItemSize (&p->buffer, 1);
}

PlankResult pl_Base64_DeInit (PlankBase64Ref p)
{
    if (p == PLANK_NULL)
        return PlankResult_MemoryError;
    
    return pl_DynamicArray_DeInit (&p->buffer);
}

PlankResult pl_Base64_EncodeFile (PlankBase64Ref p, PlankFileRef outputTextFile, PlankFileRef inputBinaryFile)
{
    PlankResult result;
    PlankUI octetA, octetB, octetC, triple;
    const char* table;
    int outputMode, inputMode, pad;
    PlankUC data;
    PlankC char0, char1, char2, char3;
    
	(void)p;

    result = PlankResult_OK;
    if ((result = pl_File_GetMode (outputTextFile, &outputMode)) != PlankResult_OK) goto exit;
    if ((result = pl_File_GetMode (inputBinaryFile, &inputMode)) != PlankResult_OK) goto exit;
    
    if (!((outputMode & PLANKFILE_WRITE) && (outputMode & ~PLANKFILE_BINARY)))
    {
        result = PlankResult_FileWriteError;
        goto exit;
    }
    
    if (!((inputMode & PLANKFILE_READ) && (inputMode & PLANKFILE_BINARY)))
    {
        result = PlankResult_FileReadError;
        goto exit;
    }
    
    table = pl_Base64Tables()->encoding;
    
    while (!pl_File_IsEOF (inputBinaryFile))
    {
        pad = 0;
                
        if ((result = pl_File_ReadUC (inputBinaryFile, &data)) == PlankResult_OK)
        {
            octetA = data;
        }
        else goto exit;

        if ((result = pl_File_ReadUC (inputBinaryFile, &data)) == PlankResult_OK)
        {
            octetB = data;
        }
        else if (result == PlankResult_FileEOF)
        {
            octetB = 0;
            octetC = 0;
            pad = 2;
            goto encode;
        }
        else goto exit;
        
        if ((result = pl_File_ReadUC (inputBinaryFile, &data)) == PlankResult_OK)
        {
            octetC = data;
        }
        else if (result == PlankResult_FileEOF)
        {
            octetC = 0;
            pad = 1;
            goto encode;
        }
        else goto exit;
        
    encode:
        triple = (octetA << 0x10) + (octetB << 0x08) + octetC;

        char0 = table[(triple >> 3 * 6) & 0x3F];
        char1 = table[(triple >> 2 * 6) & 0x3F];
        char2 = (pad == 2) ? '=' : table[(triple >> 1 * 6) & 0x3F];
        char3 = (pad >  0) ? '=' : table[(triple >> 0 * 6) & 0x3F];
        
        if ((result = pl_File_WriteC (outputTextFile, char0)) != PlankResult_OK) goto exit;
        if ((result = pl_File_WriteC (outputTextFile, char1)) != PlankResult_OK) goto exit;
        if ((result = pl_File_WriteC (outputTextFile, char2)) != PlankResult_OK) goto exit;
        if ((result = pl_File_WriteC (outputTextFile, char3)) != PlankResult_OK) goto exit;
    }
    
exit:
    return result;
}

PlankResult pl_Base64_DecodeFile (PlankBase64Ref p, PlankFileRef outputBinaryFile, PlankFileRef inputTextFile)
{
    PlankResult result;
    PlankUI sextetA, sextetB, sextetC, sextetD, triple, code;
    const char* table;
    int outputMode, inputMode, bytesRead, binaryBytes;
    PlankC quad[4];
    
	(void)p;

    result = PlankResult_OK;
    if ((result = pl_File_GetMode (inputTextFile, &inputMode)) != PlankResult_OK) goto exit;
    if ((result = pl_File_GetMode (outputBinaryFile, &outputMode)) != PlankResult_OK) goto exit;
    
    if (!((outputMode & PLANKFILE_WRITE) && (outputMode & PLANKFILE_BINARY)))
    {
        result = PlankResult_FileWriteError;
        goto exit;
    }
    
    if (!((inputMode & PLANKFILE_READ) && (inputMode & ~PLANKFILE_BINARY)))
    {
        result = PlankResult_FileReadError;
        goto exit;
    }
    
    table = pl_Base64Tables()->decoding;
    
    while (!pl_File_IsEOF (inputTextFile))
    {
        result = pl_File_Read (inputTextFile, quad, 4, &bytesRead);
        
        if ((result == PlankResult_FileEOF) || (bytesRead != 4))
        {
            result = PlankResult_ItemCountInvalid;
            goto exit;
        }
        else if (bytesRead == 0)
        {
            continue;
        }

        sextetA = table[quad[0]];
        sextetB = table[quad[1]];
        code = (quad[2] == '=' ? 0x02 : 0x00) | (quad[3] == '=' ? 0x01 : 0x00);
        
        switch (code)
        {
            case 0x00:
                sextetC = table[quad[2]];
                sextetD = table[quad[3]];
                binaryBytes = 3;
                break;
            case 0x01:
                sextetC = table[quad[2]];
                sextetD = 0;
                binaryBytes = 2;
                break;
            case 0x03:
                sextetC = 0;
                sextetD = 0;
                binaryBytes = 1;
                break;
            default:
                result = PlankResult_UnknownError;
                goto exit;
        }
            
        triple = (sextetA << 3 * 6) + (sextetB << 2 * 6) + (sextetC << 1 * 6) + (sextetD << 0 * 6);

        if (binaryBytes-- > 0)
        {
            result = pl_File_WriteUC (outputBinaryFile, (triple >> 2 * 8) & 0xFF);
            if ((result != PlankResult_OK) && (result != PlankResult_FileEOF)) goto exit;
        }
        
        if (binaryBytes-- > 0)
        {
            result = pl_File_WriteUC (outputBinaryFile, (triple >> 1 * 8) & 0xFF);
            if ((result != PlankResult_OK) && (result != PlankResult_FileEOF)) goto exit;
        }

        if (binaryBytes-- > 0)
        {
            result = pl_File_WriteUC (outputBinaryFile, (triple >> 0 * 8) & 0xFF);
            if ((result != PlankResult_OK) && (result != PlankResult_FileEOF)) goto exit;
        }
    }
    
exit:
    return result;
}

const char* pl_Base64_Encode (PlankBase64Ref p, const void* binary, const PlankL binaryLength)
{
    PlankResult result;
    PlankFile binaryInputStream;
    PlankFile textOutputStream;
    PlankL stringLength;
    char* string;
    void* data;
    int mode;
    
    result = PlankResult_OK;
    string = (char*)PLANK_NULL;
    stringLength = pl_Base64EncodedLength (binaryLength);
    
    if ((result = pl_DynamicArray_SetSize (&p->buffer, stringLength + 1)) != PlankResult_OK) goto exit;    
    if ((result = pl_File_Init (&binaryInputStream)) != PlankResult_OK) goto exit;
    if ((result = pl_File_Init (&textOutputStream)) != PlankResult_OK) goto exit;
    
    mode = PLANKFILE_BINARY | PLANKFILE_READ | PLANKFILE_NATIVEENDIAN;
    if ((result = pl_File_OpenMemory (&binaryInputStream, (void*)binary, (PlankLL)binaryLength, mode)) != PlankResult_OK) goto exit;

    data = (void*)pl_DynamicArray_GetArray (&p->buffer);
    
    mode = PLANKFILE_WRITE;
    if ((result = pl_File_OpenMemory (&textOutputStream, data, (PlankLL)stringLength, mode)) != PlankResult_OK) goto exit;
    if ((result = pl_Base64_EncodeFile (p, &textOutputStream, &binaryInputStream)) != PlankResult_OK) goto exit;
    
    string = (char*)data;
    string[stringLength] = '\0';
    
exit:    
    pl_File_DeInit (&binaryInputStream);
    pl_File_DeInit (&textOutputStream);

    return string;
}

const void* pl_Base64_Decode (PlankBase64Ref p, const char* text, PlankL* binaryLengthOut)
{
    PlankResult result;
    PlankFile textInputStream;
    PlankFile binaryOutputStream;
    PlankLL stringLength;//, binaryLength;
    const void* data;
    int mode;
    
    result = PlankResult_OK;
    data = (const void*)PLANK_NULL;
    stringLength = strlen (text);
    *binaryLengthOut = 0;
        
    if ((result = pl_DynamicArray_SetSize (&p->buffer, 0)) != PlankResult_OK) goto exit;
    if ((result = pl_File_Init (&textInputStream)) != PlankResult_OK) goto exit;
    if ((result = pl_File_Init (&binaryOutputStream)) != PlankResult_OK) goto exit;
    
    mode = PLANKFILE_READ;
    if ((result = pl_File_OpenMemory (&textInputStream, (void*)text, stringLength, mode)) != PlankResult_OK) goto exit;
        
    mode = PLANKFILE_BINARY | PLANKFILE_WRITE | PLANKFILE_NATIVEENDIAN;
    if ((result = pl_File_OpenDynamicArray (&binaryOutputStream, &p->buffer, mode)) != PlankResult_OK) goto exit;
    if ((result = pl_Base64_DecodeFile (p, &binaryOutputStream, &textInputStream)) != PlankResult_OK) goto exit;
    
    data = (const void*)pl_DynamicArray_GetArray (&p->buffer);
    *binaryLengthOut = pl_DynamicArray_GetSize (&p->buffer);
    
exit:
    pl_File_DeInit (&textInputStream);
    pl_File_DeInit (&binaryOutputStream);
    
    return data;
}

PlankResult pl_Base64_SetBufferSize (PlankBase64Ref p, const PlankL size)
{
    return pl_DynamicArray_SetSize (&p->buffer, size);
}

PlankResult pl_Base64_PurgeBuffer (PlankBase64Ref p)
{
    return pl_DynamicArray_Purge (&p->buffer);
}

