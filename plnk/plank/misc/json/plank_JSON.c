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

#include "../../core/plank_StandardHeader.h"
#include "plank_JSON.h"
#include "../base64/plank_Base64.h"
#include "../zip/plank_Zip.h"
#include "../../maths/plank_Maths.h"

#include "../../../ext/jansson/hashtable.c"
#include "../../../ext/jansson/memory.c"
#include "../../../ext/jansson/dump.c"
#include "../../../ext/jansson/pack_unpack.c"
#include "../../../ext/jansson/strconv.c"
#include "../../../ext/jansson/value.c"
#include "../../../ext/jansson/error.c"
#include "../../../ext/jansson/load.c"
#include "../../../ext/jansson/strbuffer.c"
#include "../../../ext/jansson/utf.c"


size_t pl_JSONLoadCallback (void *buffer, size_t buflen, void *data)
{
    PlankResult result;
    int bytesRead;
    PlankFileRef f;
    
    f = (PlankFileRef)data;
    
    result = pl_File_Read (f, buffer, (int)buflen, &bytesRead);
    
    return (size_t)((result == PlankResult_OK) || (result == PlankResult_FileEOF) ? bytesRead : -1);
}

int pl_JSONDumpCallback (const char *buffer, size_t size, void *data)
{
    PlankResult result;
    PlankFileRef f;
    
    f = (PlankFileRef)data;
    
    result = pl_File_Write (f, buffer, (int)size);
    
    return result == PlankResult_OK ? 0 : -1;
}

PlankJSONRef pl_JSON_FromFile (PlankFileRef f)
{
    PlankJSONRef j, e;
    PlankResult result;
    int fileMode;
    json_error_t jerror;
    
    j = 0;
    e = 0;
    result = PlankResult_OK;
    
    if ((result = pl_File_GetMode (f, &fileMode)) != PlankResult_OK) goto exit;
    
    if (!(fileMode & PLANKFILE_READ))
    {
        result = PlankResult_JSONFileError;
        goto exit;
    }
    
    if (!(fileMode & PLANKFILE_BINARY))
    {
        result = PlankResult_JSONFileError;
        goto exit;
    }    
    
    j = (PlankJSONRef)json_load_callback (pl_JSONLoadCallback, f, 0, &jerror);
    
    if (!j)
    {
        j = pl_JSON_Object();
        e = pl_JSON_Object();
        
        pl_JSON_ObjectPutKey (e, "text",     pl_JSON_String (jerror.text));
        pl_JSON_ObjectPutKey (e, "source",   pl_JSON_String (jerror.source));
        pl_JSON_ObjectPutKey (e, "line",     pl_JSON_Int (jerror.line));
        pl_JSON_ObjectPutKey (e, "column",   pl_JSON_Int (jerror.column));
        pl_JSON_ObjectPutKey (e, "position", pl_JSON_Int (jerror.position));
        pl_JSON_ObjectPutKey (j, "error", e);
    }
    
exit:
    return j;
}

PlankResult pl_JSON_WriteToFile (PlankJSONRef p, PlankFileRef f, const int flags)
{
    PlankResult result;
    int fileMode;
    int jerror;

    result = PlankResult_OK;
    
    if (! (pl_JSON_IsArray (p) || pl_JSON_IsObject (p)))
    {
        result = PlankResult_JSONFileError;
        goto exit;
    }

    if ((result = pl_File_GetMode (f, &fileMode)) != PlankResult_OK) goto exit;
    
    if (!(fileMode & PLANKFILE_WRITE))
    {
        result = PlankResult_JSONFileError;
        goto exit;
    }
    
    if (!(fileMode & PLANKFILE_BINARY))
    {
        result = PlankResult_JSONFileError;
        goto exit;
    }
    
    if (!p)
    {
        result = PlankResult_MemoryError;
        goto exit;
    }
    
    jerror = json_dump_callback ((json_t*)p, pl_JSONDumpCallback, f, flags);

    if (jerror != 0)
    {
        result = PlankResult_JSONError;
        goto exit;
    }
    
exit:
    return PlankResult_OK;
}

//

static PLANK_INLINE_LOW PlankJSONRef pl_JSON_Binary (const char* key, const void* data, const PlankL length)
{
    PlankJSONRef j;
    PlankBase64 b64;
    const char* string;
    
    pl_Base64_Init (&b64);
    j = pl_JSON_Object();
    string = pl_Base64_Encode (&b64, data, length);
    pl_JSON_ObjectPutKey (j, key, pl_JSON_StringSplit (string, PLANK_JSON_ENCODEDSTRINGLENGTH));
    pl_Base64_DeInit (&b64);
    
    return j;
}

PlankJSONRef pl_JSON_IntBinary (const PlankLL value)
{
    return pl_JSON_Binary (PLANK_JSON_INTBINARY, &value, sizeof (value));
}

PlankJSONRef pl_JSON_FloatBinary (const float value)
{
    return pl_JSON_Binary (PLANK_JSON_FLOATBINARY, &value, sizeof (value));
}

PlankJSONRef pl_JSON_DoubleBinary (const double value)
{
    return pl_JSON_Binary (PLANK_JSON_DOUBLEBINARY, &value, sizeof (value));
}

PlankJSONRef pl_JSON_IntArrayBinary (const PlankLL* values, const PlankL count)
{
    return pl_JSON_Binary (PLANK_JSON_INTARRAYBINARY, values, sizeof (values[0]) * count);
}

PlankJSONRef pl_JSON_FloatArrayBinary (const float* values, const PlankL count)
{
    return pl_JSON_Binary (PLANK_JSON_FLOATARRAYBINARY, values, sizeof (values[0]) * count);
}

PlankJSONRef pl_JSON_DoubleArrayBinary (const double* values, const PlankL count)
{
    return pl_JSON_Binary (PLANK_JSON_DOUBLEARRAYBINARY, values, sizeof (values[0]) * count);
}

static PLANK_INLINE_LOW PlankJSONRef pl_JSON_Compressed (const char* key, const void* data, const PlankL length)
{
    PlankJSONRef j;
    PlankBase64 b64;
    PlankZip z;
    PlankL zlength;
    const void* zdata;
    const char* text;
    
    pl_Base64_Init (&b64);
    pl_Zip_Init (&z);
    
    j = pl_JSON_Object();
    zdata = pl_Zip_DeflateData (&z, PLANK_JSON_COMPRESSIONLEVEL, data, length, &zlength);
    text = pl_Base64_Encode (&b64, zdata, zlength);
    pl_JSON_ObjectPutKey (j, key, pl_JSON_StringSplit (text, PLANK_JSON_ENCODEDSTRINGLENGTH));
    pl_Base64_DeInit (&b64);
    pl_Zip_DeInit (&z);

    return j;
}

PlankJSONRef pl_JSON_IntArrayCompressed (const PlankLL* values, const PlankL count)
{
    return pl_JSON_Compressed (PLANK_JSON_INTARRAYCOMPRESSED, values, sizeof (values[0]) * count);
}

PlankJSONRef pl_JSON_FloatArrayCompressed (const float* values, const PlankL count)
{
    return pl_JSON_Compressed (PLANK_JSON_FLOATARRAYCOMPRESSED, values, sizeof (values[0]) * count);
}

PlankJSONRef pl_JSON_DoubleArrayCompressed (const double* values, const PlankL count)
{
    return pl_JSON_Compressed (PLANK_JSON_DOUBLEARRAYCOMPRESSED, values, sizeof (values[0]) * count);
}

PlankJSONRef pl_JSON_IntArray (const PlankLL* values, const PlankL count)
{
    PlankJSONRef j;
    PlankL i;

    j = pl_JSON_Array();
    
    for (i = 0; i < count; ++i)
        pl_JSON_ArrayAppend (j, pl_JSON_Int (values[i]));
    
    return j;
}

PlankJSONRef pl_JSON_FloatArray (const float* values, const PlankL count)
{
    PlankJSONRef j;
    PlankL i;
    
    j = pl_JSON_Array();
    
    for (i = 0; i < count; ++i)
        pl_JSON_ArrayAppend (j, pl_JSON_Float (values[i]));
    
    return j;
}

PlankJSONRef pl_JSON_DoubleArray (const double* values, const PlankL count)
{
    PlankJSONRef j;
    PlankL i;
    
    j = pl_JSON_Array();
    
    for (i = 0; i < count; ++i)
        pl_JSON_ArrayAppend (j, pl_JSON_Double (values[i]));
    
    return j;
}

static PLANK_INLINE_LOW const void* pl_JSON_EncodedGet (PlankJSONRef p, const char* keyType, const PlankL expectedLength, PlankBase64Ref b64)
{
    PlankL binaryLength;
    const char* key;
    const void* data = 0;
    
    key = json_object_iter_key (json_object_iter ((json_t*)p));
    
    if (key && (strcmp (key, keyType) == 0))
    {
        p = pl_JSON_ObjectAtKey (p, key);
        
        if (pl_JSON_IsString (p))
        {
            data = pl_Base64_Decode (b64, pl_JSON_StringGet (p), &binaryLength);
            
            if (binaryLength != expectedLength)
                data = 0;
        }
    }
    
    return data;

}

PlankLL pl_JSON_IntEncodedGet (PlankJSONRef p)
{
    const PlankLL* data;
    PlankLL value;
    PlankBase64 b64;
    
    pl_Base64_Init (&b64);
    data = (const PlankLL*)pl_JSON_EncodedGet (p, PLANK_JSON_INTBINARY, sizeof (data[0]), &b64);
    value = data ? *data : 0;
    pl_Base64_DeInit (&b64);

    return value;
}

float pl_JSON_FloatEncodedGet (PlankJSONRef p)
{
    const float* data;
    float value;
    PlankBase64 b64;

    pl_Base64_Init (&b64);
    data = (const float*)pl_JSON_EncodedGet (p, PLANK_JSON_FLOATBINARY, sizeof (data[0]), &b64);
    value = data ? *data : 0;
    pl_Base64_DeInit (&b64);

    return value;
}

double pl_JSON_DoubleEncodedGet (PlankJSONRef p)
{
    const double* data;
    double value;
    PlankBase64 b64;

    pl_Base64_Init (&b64);
    data = (const double*)pl_JSON_EncodedGet (p, PLANK_JSON_DOUBLEBINARY, sizeof (data[0]), &b64);
    value = data ? *data : 0;
    pl_Base64_DeInit (&b64);

    return value;
}

static PlankResult pl_JSON_EncodedArrayGet (PlankJSONRef p, PlankDynamicArrayRef array, const char* binaryKey, const char* compressedKey, const PlankUL sz)
{
    PlankResult result;
    PlankBase64 b64;
    PlankZip z;
    PlankFile decodedStream;
    PlankFile encodedStream;
    PlankFile zStream;
    PlankDynamicArray zArray;
    PlankJSONRef j;
    PlankL binaryLength, zlength;
    const char* key;
    const char* string;
    const void* data;
    const void* zdata;
    PlankUL itemSize;
    PlankL numStrings, i;
    
    result = PlankResult_OK;
    key = json_object_iter_key (json_object_iter ((json_t*)p));
    
    pl_Base64_Init (&b64);
    pl_Zip_Init (&z);
    pl_File_Init (&decodedStream);
    pl_File_OpenDynamicArray (&decodedStream, array, PLANKFILE_NATIVEENDIAN | PLANKFILE_WRITE | PLANKFILE_BINARY);
    
    itemSize = pl_DynamicArray_GetItemSize (array);
    
    if (itemSize == 0)
        if ((result = pl_DynamicArray_InitWithItemSize (array, sz)) != PlankResult_OK)
            goto exit;
    
    if (key)
    {
        data = 0;
        binaryLength = 0;
        
        if (strcmp (key, compressedKey) == 0)
        {
            p = pl_JSON_ObjectAtKey (p, key);

            if (pl_JSON_IsString (p))
            {
                string = pl_JSON_StringGet (p);
                zdata = pl_Base64_Decode (&b64, string, &zlength);
                
                pl_File_Init (&encodedStream);
                pl_File_OpenMemory (&encodedStream, (void*)zdata, zlength, PLANKFILE_NATIVEENDIAN | PLANKFILE_READ | PLANKFILE_BINARY);
                pl_Zip_InflateStream (&z, &decodedStream, &encodedStream);
                pl_File_DeInit (&encodedStream);
            }
            else if (pl_JSON_IsArray (p))
            {
                numStrings = pl_JSON_ArrayGetSize (p);
                
                pl_DynamicArray_InitWithItemSize (&zArray, 1);
                pl_File_Init (&zStream);
                pl_File_OpenDynamicArray (&zStream, &zArray, PLANKFILE_NATIVEENDIAN | PLANKFILE_WRITE | PLANKFILE_READ | PLANKFILE_BINARY);
                
                for (i = 0; i < numStrings; ++i)
                {
                    j = pl_JSON_ArrayAt (p, i);
                    
                    if (!pl_JSON_IsString (j))
                    {
                        result = PlankResult_JSONError;
                        pl_File_DeInit (&zStream);
                        pl_DynamicArray_DeInit (&zArray);
                        goto exit;
                    }
                    
                    string = pl_JSON_StringGet (j);
                    
                    pl_File_Init (&encodedStream);
                    pl_File_OpenMemory (&encodedStream, (void*)string, strlen (string), PLANKFILE_NATIVEENDIAN | PLANKFILE_READ | PLANKFILE_BINARY);                    
                    pl_Base64_DecodeFile (&b64, &zStream, &encodedStream);
                    pl_File_DeInit (&encodedStream);
                }
                
                pl_File_ResetPosition (&zStream);
                pl_Zip_InflateStream (&z, &decodedStream, &zStream);
                pl_File_DeInit (&zStream);
                pl_DynamicArray_DeInit (&zArray);
            }
        }
        else if (strcmp (key, binaryKey) == 0)
        {
            p = pl_JSON_ObjectAtKey (p, key);
            
            if (pl_JSON_IsString (p))
            {
                string = pl_JSON_StringGet (p);
                
                pl_File_Init (&encodedStream);
                pl_File_OpenMemory (&encodedStream, (void*)string, strlen (string), PLANKFILE_NATIVEENDIAN | PLANKFILE_READ | PLANKFILE_BINARY);
                pl_Base64_DecodeFile (&b64, &decodedStream, &encodedStream);
                pl_File_DeInit (&encodedStream);

            }
            else if (pl_JSON_IsArray (p))
            {
                numStrings = pl_JSON_ArrayGetSize (p);
                
                for (i = 0; i < numStrings; ++i)
                {
                    j = pl_JSON_ArrayAt (p, i);
                    
                    if (!pl_JSON_IsString (j))
                    {
                        result = PlankResult_JSONError;
                        goto exit;
                    }
                    
                    string = pl_JSON_StringGet (j);

                    pl_File_Init (&encodedStream);
                    pl_File_OpenMemory (&encodedStream, (void*)string, strlen (string), PLANKFILE_NATIVEENDIAN | PLANKFILE_READ | PLANKFILE_BINARY);
                    pl_Base64_DecodeFile (&b64, &decodedStream, &encodedStream);
                    pl_File_DeInit (&encodedStream);
                }
            }
        }
    }
    
exit:
    pl_File_DeInit (&decodedStream);
    pl_Zip_DeInit (&z);
    pl_Base64_DeInit (&b64);

    return result;
}

static PlankL pl_JSON_NumericalArrayCheckSize (PlankJSONRef p, PlankDynamicArrayRef array, const PlankL sz)
{
    PlankResult result;
    PlankL itemSize, size;
    
    itemSize = pl_DynamicArray_GetItemSize (array);
    
    if (itemSize == 0)
    {
        if ((result = pl_DynamicArray_InitWithItemSize (array, sz)) != PlankResult_OK)
            return 0;
    }
    else if (itemSize != sz)
    {
        return 0;
    }
    
    size = pl_JSON_ArrayGetSize (p);
    
    if ((result = pl_DynamicArray_SetSize (array, size)) != PlankResult_OK)
        return 0;

    return size;
}

PlankResult pl_JSON_IntArrayGet (PlankJSONRef p, PlankDynamicArrayRef array)
{
    PlankResult result;
    PlankL size, i, sz;
    PlankLL* values;
    
    result = PlankResult_OK;
    sz = sizeof (values[0]);
    
    if (pl_JSON_IsObject (p))
    {
        result = pl_JSON_EncodedArrayGet (p, array, PLANK_JSON_INTARRAYBINARY, PLANK_JSON_INTARRAYCOMPRESSED, sz);
    }
    else if (pl_JSON_IsArray (p))
    {
        size = pl_JSON_NumericalArrayCheckSize (p, array, sz);
        
        if (size)
        {
            values = (PlankLL*)pl_DynamicArray_GetArray (array);
            
            for (i = 0; i < size; ++i)
                values[i] = pl_JSON_IntGet (pl_JSON_ArrayAt (p, i));
        }
        else result = PlankResult_JSONError;
    }
    else result = PlankResult_JSONError;
    
//exit:
    return result;
}

PlankResult pl_JSON_FloatArrayGet (PlankJSONRef p, PlankDynamicArrayRef array)
{
    PlankResult result;
    PlankL size, i, sz;
    float* values;
    
    result = PlankResult_OK;
    sz = sizeof (values[0]);
    
    if (pl_JSON_IsObject (p))
    {
        result = pl_JSON_EncodedArrayGet (p, array, PLANK_JSON_FLOATARRAYBINARY, PLANK_JSON_FLOATARRAYCOMPRESSED, sz);
    }
    else if (pl_JSON_IsArray (p))
    {
        size = pl_JSON_NumericalArrayCheckSize (p, array, sz);
        
        if (size)
        {
            values = (float*)pl_DynamicArray_GetArray (array);
            
            for (i = 0; i < size; ++i)
                values[i] = pl_JSON_FloatGet (pl_JSON_ArrayAt (p, i));
        }
        else result = PlankResult_JSONError;
    }
    else result = PlankResult_JSONError;
    
//exit:
    return result;
}

PlankResult pl_JSON_DoubleArrayGet (PlankJSONRef p, PlankDynamicArrayRef array)
{    
    PlankResult result;
    PlankL size, i, sz;
    double* values;
    
    result = PlankResult_OK;
    sz = sizeof (values[0]);
    
    if (pl_JSON_IsObject (p))
    {
        result = pl_JSON_EncodedArrayGet (p, array, PLANK_JSON_DOUBLEARRAYBINARY, PLANK_JSON_DOUBLEARRAYCOMPRESSED, sz);
    }
    else if (pl_JSON_IsArray (p))
    {
        size = pl_JSON_NumericalArrayCheckSize (p, array, sz);
        
        if (size)
        {
            values = (double*)pl_DynamicArray_GetArray (array);
            
            for (i = 0; i < size; ++i)
                values[i] = pl_JSON_DoubleGet (pl_JSON_ArrayAt (p, i));
        }
        else result = PlankResult_JSONError;
    }
    else result = PlankResult_JSONError;
    
//exit:
    return result;
}

PlankJSONRef pl_JSON_StringSplit (const char* string, const PlankL count)
{
    PlankJSONRef j;
    PlankL pos, length, end;
    PlankC temp;
    
    length = strlen (string);

    if (length > count)
    {
        pos = 0;
        j = pl_JSON_Array();
        
        while (pos < length)
        {
            end = pl_MinL (pos + count, length);
            temp = string[end];
            ((char*)string)[end] = '\0';
            pl_JSON_ArrayAppend (j, pl_JSON_String (string + pos));
            ((char*)string)[end] = temp;
            pos = end;
        }
    }
    else
    {
        j = pl_JSON_String (string);
    }
    
    return j;
}

static PLANK_INLINE_LOW PlankB pl_JSON_IsEncoded (PlankJSONRef p, const char* binaryKey, const char* compressedKey)
{
    const char* key;
    key = json_object_iter_key (json_object_iter ((json_t*)p));
    
    if (!key)
        return PLANK_FALSE;
    else if (binaryKey && ((strcmp (key, binaryKey) == 0)))
        return PLANK_TRUE;
    else if (compressedKey && ((strcmp (key, compressedKey) == 0)))
        return PLANK_TRUE;
    else
        return PLANK_FALSE;
}

PlankB pl_JSON_IsIntEncoded (PlankJSONRef p)
{
    return pl_JSON_IsEncoded (p, PLANK_JSON_INTBINARY, 0);
}

PlankB pl_JSON_IsFloatEncoded (PlankJSONRef p)
{
    return pl_JSON_IsEncoded (p, PLANK_JSON_FLOATBINARY, 0);
}

PlankB pl_JSON_IsDoubleEncoded (PlankJSONRef p)
{
    return pl_JSON_IsEncoded (p, PLANK_JSON_DOUBLEBINARY, 0);
}

PlankB pl_JSON_IsIntArrayEncoded (PlankJSONRef p)
{
    return pl_JSON_IsEncoded (p, PLANK_JSON_INTARRAYBINARY, PLANK_JSON_INTARRAYCOMPRESSED);
}

PlankB pl_JSON_IsFloatArrayEncoded (PlankJSONRef p)
{
    return pl_JSON_IsEncoded (p, PLANK_JSON_FLOATARRAYBINARY, PLANK_JSON_FLOATARRAYCOMPRESSED);
}

PlankB pl_JSON_IsDoubleArrayEncoded (PlankJSONRef p)
{
    return pl_JSON_IsEncoded (p, PLANK_JSON_DOUBLEARRAYBINARY, PLANK_JSON_DOUBLEARRAYCOMPRESSED);
}

PlankB pl_JSON_IsError (PlankJSONRef p)
{
    return pl_JSON_IsObject (p) && (pl_JSON_ObjectGetSize (p) == 1) && (pl_JSON_ObjectAtKey (p, "error") != 0);
}

PlankJSONRef pl_JSON_VersionString (const PlankUC ex, const PlankUC major, const PlankUC minor, const PlankUC micro)
{
    char string[64];
    snprintf (string, 64, "%d.%d.%d.%d", ex, major, minor, micro);
    return pl_JSON_String (string);
}

PlankUI pl_JSON_VersionCode (const PlankUC ex, const PlankUC major, const PlankUC minor, const PlankUC micro)
{
    PlankUI code;
    code = (ex << 24) | (major << 16) | (minor << 8) | micro;
    return code;
}

static PlankUI pl_JSON_VersionCodeFromString (PlankJSONRef p)
{
    int ex, major, minor, micro;
            
    ex = 0;
    major = 0;
    minor = 0;
    micro = 0;
    
    sscanf (pl_JSON_StringGet (p), "%d.%d.%d.%d", &ex, &major, &minor, &micro);
    
    return pl_JSON_VersionCode ((PlankUC)ex, (PlankUC)major, (PlankUC)minor, (PlankUC)micro);
}

void pl_JSON_ObjectSetType (PlankJSONRef p, const char* type)
{
    pl_JSON_ObjectPutKey (p, PLANK_JSON_TYPE, pl_JSON_String (type));
}

void pl_JSON_ObjectSetVersionString (PlankJSONRef p, const PlankUC ex, const PlankUC major, const PlankUC minor, const PlankUC micro)
{
    pl_JSON_ObjectPutKey (p, PLANK_JSON_VERSION, pl_JSON_VersionString (ex, major, minor, micro));
}

void pl_JSON_ObjectSetVersionCode (PlankJSONRef p, const PlankUC ex, const PlankUC major, const PlankUC minor, const PlankUC micro)
{
    pl_JSON_ObjectPutKey (p, PLANK_JSON_VERSION, pl_JSON_Int ((int)pl_JSON_VersionCode (ex, major, minor, micro)));
}

PlankUI pl_JSON_ObjectGetVersion (PlankJSONRef p)
{
    return pl_JSON_IsObject (p) ? pl_JSON_VersionGet (pl_JSON_ObjectAtKey (p, PLANK_JSON_VERSION)) : 0;
}

PlankUI pl_JSON_VersionGet (PlankJSONRef p)
{
    if (pl_JSON_IsString (p))
        return pl_JSON_VersionCodeFromString (p);
    else
        return (PlankUI)pl_JSON_IntGet (p);
}

PlankB pl_JSON_IsObjectType (PlankJSONRef p, const char* type)
{
    return pl_JSON_IsObject (p) && (strcmp (pl_JSON_StringGet (pl_JSON_ObjectAtKey (p, PLANK_JSON_TYPE)), type) == 0);
}



