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

#include "../../core/plank_StandardHeader.h"
#include "plank_Base64.h"

#include <math.h>
#include <stdint.h>
#include <stdlib.h>

typedef struct PlankBase64Tables
{
    char decoding[256];
    char encoding[64];
    int mod[3];
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
    strcpy (tables->encoding, "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/"); 

    for (i = 0; i < 64; ++i)
        tables->decoding[tables->encoding[i]] = i;
    
    tables->mod[0] = 0;
    tables->mod[1] = 2;
    tables->mod[2] = 1;
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

const char* pl_Base64_Encode (PlankBase64Ref p, const void* binary, const PlankL binaryLength)
{
    PlankResult result;
    PlankL stringLength;
    PlankUC* data;
    PlankC* string;
    int i, j;
    PlankUI octetA, octetB, octetC, triple;
    const char* table;
    const int* mod;
    
    stringLength = pl_Base64EncodedLength (binaryLength);
    data = (PlankUC*)binary;
    string = (PlankC*)PLANK_NULL;
    result = pl_DynamicArray_SetSize (&p->buffer, stringLength + 1);
    
    if (result == PlankResult_OK)
    {
        table = pl_Base64Tables()->encoding;
        mod = pl_Base64Tables()->mod;
        string = (PlankC*)pl_DynamicArray_GetArray (&p->buffer);

        for (i = 0, j = 0; i < binaryLength;)
        {
            octetA = i < binaryLength ? data[i++] : 0;
            octetB = i < binaryLength ? data[i++] : 0;
            octetC = i < binaryLength ? data[i++] : 0;

            triple = (octetA << 0x10) + (octetB << 0x08) + octetC;

            string[j++] = table[(triple >> 3 * 6) & 0x3F];
            string[j++] = table[(triple >> 2 * 6) & 0x3F];
            string[j++] = table[(triple >> 1 * 6) & 0x3F];
            string[j++] = table[(triple >> 0 * 6) & 0x3F];
        }

        for (i = 0; i < mod[binaryLength % 3]; i++)
            string[stringLength - 1 - i] = '=';
        
        string[stringLength] = '\0';
    }

    return string;
}

const void* pl_Base64_Decode (PlankBase64Ref p, const char* text, PlankL* binaryLengthOut)
{
    PlankResult result;
    PlankL stringLength, binaryLength;
    PlankUC* data;
    int i, j;
    PlankUI sextetA, sextetB, sextetC, sextetD, triple;
    const char* table;

    result = PlankResult_OK;
    data = (PlankUC*)PLANK_NULL;
    stringLength = strlen (text);
    *binaryLengthOut = 0;
    
    if ((stringLength & 3) != 0)
    {
        result = PlankResult_ItemCountInvalid;
        return 0;
    }
    
    binaryLength = pl_Base64DecodedLength (stringLength);

    if (text[stringLength - 1] == '=') binaryLength--;
    if (text[stringLength - 2] == '=') binaryLength--;

    result = pl_DynamicArray_SetSize (&p->buffer, binaryLength);

    if (result == PlankResult_OK)
    {
        table = pl_Base64Tables()->decoding;
        data = (PlankUC*)pl_DynamicArray_GetArray (&p->buffer);
        
        for (i = 0, j = 0; i < stringLength;)
        {
            sextetA = (text[i] == '=') ? 0 : table[text[i]]; ++i;
            sextetB = (text[i] == '=') ? 0 : table[text[i]]; ++i;
            sextetC = (text[i] == '=') ? 0 : table[text[i]]; ++i;
            sextetD = (text[i] == '=') ? 0 : table[text[i]]; ++i;

            triple = (sextetA << 3 * 6) + (sextetB << 2 * 6) + (sextetC << 1 * 6) + (sextetD << 0 * 6);

            if (j < binaryLength) data[j++] = (triple >> 2 * 8) & 0xFF;
            if (j < binaryLength) data[j++] = (triple >> 1 * 8) & 0xFF;
            if (j < binaryLength) data[j++] = (triple >> 0 * 8) & 0xFF;
        }
    }
    
    *binaryLengthOut = binaryLength;
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

//static char encoding_table[] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H',
//    'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
//    'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X',
//    'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f',
//    'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',
//    'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
//    'w', 'x', 'y', 'z', '0', '1', '2', '3',
//    '4', '5', '6', '7', '8', '9', '+', '/'};
//static char *decoding_table = NULL;
//static int mod_table[] = {0, 2, 1};
//
//
//char *base64_encode(const unsigned char *data,
//                    size_t input_length,
//                    size_t *output_length) {
//    
//    *output_length = (size_t) (4.0 * ceil((double) input_length / 3.0));
//    
//    char *encoded_data = malloc(*output_length);
//    if (encoded_data == NULL) return NULL;
//    
//    for (int i = 0, j = 0; i < input_length;) {
//        
//        uint32_t octet_a = i < input_length ? data[i++] : 0;
//        uint32_t octet_b = i < input_length ? data[i++] : 0;
//        uint32_t octet_c = i < input_length ? data[i++] : 0;
//        
//        uint32_t triple = (octet_a << 0x10) + (octet_b << 0x08) + octet_c;
//        
//        encoded_data[j++] = encoding_table[(triple >> 3 * 6) & 0x3F];
//        encoded_data[j++] = encoding_table[(triple >> 2 * 6) & 0x3F];
//        encoded_data[j++] = encoding_table[(triple >> 1 * 6) & 0x3F];
//        encoded_data[j++] = encoding_table[(triple >> 0 * 6) & 0x3F];
//    }
//    
//    for (int i = 0; i < mod_table[input_length % 3]; i++)
//        encoded_data[*output_length - 1 - i] = '=';
//    
//    return encoded_data;
//}
//
//
//unsigned char *base64_decode(const char *data,
//                             size_t input_length,
//                             size_t *output_length) {
//    
//    if (decoding_table == NULL) build_decoding_table();
//    
//    if (input_length % 4 != 0) return NULL;
//    
//    *output_length = input_length / 4 * 3;
//    if (data[input_length - 1] == '=') (*output_length)--;
//    if (data[input_length - 2] == '=') (*output_length)--;
//    
//    unsigned char *decoded_data = malloc(*output_length);
//    if (decoded_data == NULL) return NULL;
//    
//    for (int i = 0, j = 0; i < input_length;) {
//        
//        uint32_t sextet_a = data[i] == '=' ? 0 & i++ : decoding_table[data[i++]];
//        uint32_t sextet_b = data[i] == '=' ? 0 & i++ : decoding_table[data[i++]];
//        uint32_t sextet_c = data[i] == '=' ? 0 & i++ : decoding_table[data[i++]];
//        uint32_t sextet_d = data[i] == '=' ? 0 & i++ : decoding_table[data[i++]];
//        
//        uint32_t triple = (sextet_a << 3 * 6)
//        + (sextet_b << 2 * 6)
//        + (sextet_c << 1 * 6)
//        + (sextet_d << 0 * 6);
//        
//        if (j < *output_length) decoded_data[j++] = (triple >> 2 * 8) & 0xFF;
//        if (j < *output_length) decoded_data[j++] = (triple >> 1 * 8) & 0xFF;
//        if (j < *output_length) decoded_data[j++] = (triple >> 0 * 8) & 0xFF;
//    }
//    
//    return decoded_data;
//}
//
//
//void build_decoding_table() {
//    
//    decoding_table = malloc(256);
//    
//    for (int i = 0; i < 0x40; i++)
//        decoding_table[encoding_table[i]] = i;
//}
//
//
//void base64_cleanup() {
//    free(decoding_table);
//}
