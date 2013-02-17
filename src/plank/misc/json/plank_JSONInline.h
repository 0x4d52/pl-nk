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

#if PLANK_INLINING_FUNCTIONS

#include "../../../../ext/jansson/jansson.h"

typedef struct PlankJSON
{
    json_t* json;
} PlankJSON;

static inline PlankResult pl_JSON_Init (PlankJSONRef p)
{
    if (!p) return PlankResult_MemoryError;
    p->json = (json_t*)PLANK_NULL;
    return PlankResult_OK;
}

static inline PlankB pl_JSON_IsObject (PlankJSONRef p)
{
    return p ? json_is_object (p->json) : PLANK_FALSE;
}

static inline PlankB pl_JSON_IsArray (PlankJSONRef p)
{
    return p ? json_is_array (p->json) : PLANK_FALSE;
}

static inline PlankB pl_JSON_IsString (PlankJSONRef p)
{
    return p ? json_is_string (p->json) : PLANK_FALSE;
}

static inline PlankB pl_JSON_IsInt(PlankJSONRef p)
{
    return p ? json_is_integer (p->json) : PLANK_FALSE;
}

static inline PlankB pl_JSON_IsFloat (PlankJSONRef p)
{
    return p ? json_is_real (p->json) : PLANK_FALSE;
}

static inline PlankB pl_JSON_IsDouble (PlankJSONRef p)
{
    return p ? json_is_real (p->json) : PLANK_FALSE;
}

static inline PlankB pl_JSON_IsBool (PlankJSONRef p)
{
    return p ? json_is_boolean (p->json) : PLANK_FALSE;
}

static inline PlankB pl_JSON_IsNull (PlankJSONRef p)
{
    return p ? json_is_null (p->json) : PLANK_FALSE;
}

static inline PlankResult pl_JSON_SetObject (PlankJSONRef p)
{
    if (!p) return PlankResult_MemoryError;
    if (p->json) pl_JSON_DecrementRefCount (p);
    p->json = json_object();
    return p->json ? PlankResult_OK : PlankResult_JSONError;
}

static inline PlankResult pl_JSON_SetArray (PlankJSONRef p)
{
    if (!p) return PlankResult_MemoryError;
    if (p->json) pl_JSON_DecrementRefCount (p);
    p->json = json_array();
    return p->json ? PlankResult_OK : PlankResult_JSONError;
}

static inline PlankResult pl_JSON_SetString (PlankJSONRef p, const char* string)
{
    if (!p) return PlankResult_MemoryError;
    if (p->json) pl_JSON_DecrementRefCount (p);
    p->json = json_string (string);
    return p->json ? PlankResult_OK : PlankResult_JSONError;
}

static inline PlankResult pl_JSON_SetInt(PlankJSONRef p, const int value)
{
    if (!p) return PlankResult_MemoryError;
    if (p->json) pl_JSON_DecrementRefCount (p);
    p->json = json_integer (value);
    return p->json ? PlankResult_OK : PlankResult_JSONError;
}

static inline PlankResult pl_JSON_SetFloat (PlankJSONRef p, const float value)
{
    if (!p) return PlankResult_MemoryError;
    if (p->json) pl_JSON_DecrementRefCount (p);
    p->json = json_real ((double)value);
    return p->json ? PlankResult_OK : PlankResult_JSONError;
}

static inline PlankResult pl_JSON_SetDouble (PlankJSONRef p, const double value)
{
    if (!p) return PlankResult_MemoryError;
    if (p->json) pl_JSON_DecrementRefCount (p);
    p->json = json_real (value);
    return p->json ? PlankResult_OK : PlankResult_JSONError;
}

static inline PlankResult pl_JSON_SetBool (PlankJSONRef p, const PlankB state)
{
    if (!p) return PlankResult_MemoryError;
    if (p->json) pl_JSON_DecrementRefCount (p);
    p->json = json_boolean (state);
    return p->json ? PlankResult_OK : PlankResult_JSONError;
}

static inline PlankResult pl_JSON_SetNull (PlankJSONRef p)
{
    if (!p) return PlankResult_MemoryError;
    if (p->json) pl_JSON_DecrementRefCount (p);
    p->json = json_null();
    return p->json ? PlankResult_OK : PlankResult_JSONError;
}

static inline PlankResult pl_JSON_IncrementRefCount (PlankJSONRef p)
{
    if (!p) return PlankResult_MemoryError;
    if (!p->json) return PlankResult_JSONError;
    json_incref (p->json);
    return PlankResult_OK;
}

static inline PlankResult pl_JSON_DecrementRefCount (PlankJSONRef p)
{
    if (!p) return PlankResult_MemoryError;
    if (!p->json) return PlankResult_JSONError;
    json_decref (p->json);
    return PlankResult_OK;
}

static inline PlankResult pl_JSON_ObjectGetSize (PlankJSONRef p, int* size)
{
    if (!p) return PlankResult_MemoryError;
    if (!json_is_object (p->json)) return PlankResult_JSONError;
    *size = json_object_size (p->json);
    return PlankResult_OK;
}

static inline PlankResult pl_JSON_ObjectGetValue (PlankJSONRef p, const char* key, PlankJSONRef value)
{
    if (!p) return PlankResult_MemoryError;
    if (!json_is_object (p->json)) return PlankResult_JSONError;
    if (value->json) pl_JSON_DecrementRefCount (value);
    value->json = json_object_get (p->json, key);
    return PlankResult_OK;
}

static inline PlankResult pl_JSON_ObjectSetValue (PlankJSONRef p, const char* key, const PlankJSONRef value)
{
    int jerr;
    if (!p) return PlankResult_MemoryError;
    if (!json_is_object (p->json)) return PlankResult_JSONError;    
    jerr = json_object_set_new (p->json, key, value->json ? value->json : json_null());    
    return jerr == 0 ? PlankResult_OK : PlankResult_JSONError;
}

static inline PlankResult pl_JSON_ArrayGetSize (PlankJSONRef p, int* size)
{
    if (!p) return PlankResult_MemoryError;
    if (!json_is_array (p->json)) return PlankResult_JSONError;
    *size = json_array_size (p->json);
    return PlankResult_OK;
}

static inline PlankResult pl_JSON_ArrayAt (PlankJSONRef p, const int index, PlankJSONRef value)
{
    if (!p) return PlankResult_MemoryError;
    if (!json_is_array (p->json)) return PlankResult_JSONError;
    if (value->json) pl_JSON_DecrementRefCount (value);
    value->json = json_array_get (p->json, index);
    return PlankResult_OK;
}

static inline PlankResult pl_JSON_ArrayPut (PlankJSONRef p, const int index, const PlankJSONRef value)
{
    int jerr;
    if (!p) return PlankResult_MemoryError;
    if (!json_is_array (p->json)) return PlankResult_JSONError;
    jerr = json_array_set_new (p->json, index, value->json ? value->json : json_null());
    return jerr == 0 ? PlankResult_OK : PlankResult_JSONError;
}

static inline PlankResult pl_JSON_ArrayAppend (PlankJSONRef p, const PlankJSONRef value)
{
    int jerr;
    if (!p) return PlankResult_MemoryError;
    if (!json_is_array (p->json)) return PlankResult_JSONError;
    jerr = json_array_append_new (p->json, value->json ? value->json : json_null());
    return jerr == 0 ? PlankResult_OK : PlankResult_JSONError;
}

static inline PlankResult pl_JSON_DoubleGet (PlankJSONRef p, double* value)
{
    if (!p) return PlankResult_MemoryError;
    if (!json_is_real (p->json)) return PlankResult_JSONError;
    *value = json_real_value (p->json);
    return PlankResult_OK;
}

static inline PlankResult pl_JSON_FloatGet (PlankJSONRef p, float* value)
{
    if (!p) return PlankResult_MemoryError;
    if (!json_is_real (p->json)) return PlankResult_JSONError;
    *value = (float)json_real_value (p->json);
    return PlankResult_OK;
}

static inline PlankResult pl_JSON_IntGet (PlankJSONRef p, int* value)
{
    if (!p) return PlankResult_MemoryError;
    if (!json_is_integer (p->json)) return PlankResult_JSONError;
    *value = (int)json_integer_value (p->json);
    return PlankResult_OK;
}

static inline PlankResult pl_JSON_StringGet (PlankJSONRef p, const char** value)
{
    if (!p) return PlankResult_MemoryError;
    if (!json_is_string (p->json)) return PlankResult_JSONError;
    *value = json_string_value (p->json);
    return PlankResult_OK;
}



#endif // PLANK_INLINING_FUNCTIONS


