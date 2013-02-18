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

static inline PlankResult pl_JSON_InitObject (PlankJSONRef p)
{
    PlankResult result;
    if ((result = pl_JSON_Init (p)) != PlankResult_OK) return result;
    return pl_JSON_SetObject (p);
}

static inline PlankResult pl_JSON_InitArray (PlankJSONRef p)
{
    PlankResult result;
    if ((result = pl_JSON_Init (p)) != PlankResult_OK) return result;
    return pl_JSON_SetArray (p);
}

static inline PlankResult pl_JSON_InitString (PlankJSONRef p, const char* string)
{
    PlankResult result;
    if ((result = pl_JSON_Init (p)) != PlankResult_OK) return result;
    return pl_JSON_SetString (p, string);
}

static inline PlankResult pl_JSON_InitInt(PlankJSONRef p, const int value)
{
    PlankResult result;
    if ((result = pl_JSON_Init (p)) != PlankResult_OK) return result;
    return pl_JSON_SetInt (p, value);
}

static inline PlankResult pl_JSON_InitFloat (PlankJSONRef p, const float value)
{
    PlankResult result;
    if ((result = pl_JSON_Init (p)) != PlankResult_OK) return result;
    return pl_JSON_SetFloat (p, value);
}

static inline PlankResult pl_JSON_InitDouble (PlankJSONRef p, const double value)
{
    PlankResult result;
    if ((result = pl_JSON_Init (p)) != PlankResult_OK) return result;
    return pl_JSON_SetDouble (p, value);
}

static inline PlankResult pl_JSON_InitBool (PlankJSONRef p, const PlankB state)
{
    PlankResult result;
    if ((result = pl_JSON_Init (p)) != PlankResult_OK) return result;
    return pl_JSON_SetBool (p, state);
}

static inline PlankResult pl_JSON_InitNull (PlankJSONRef p)
{
    PlankResult result;
    if ((result = pl_JSON_Init (p)) != PlankResult_OK) return result;
    return pl_JSON_SetNull (p);
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

static PlankB pl_JSON_IsObjectType (PlankJSONRef p, const char* type)
{
    PlankJSON jtype;
    const char* ptype;
    
    if (!pl_JSON_IsObject (p)) return PLANK_FALSE;
    
    pl_JSON_Init (&jtype);
    pl_JSON_ObjectGetValue (p, PLANK_JSON_TYPE, &jtype);
    
    if (!pl_JSON_IsString (&jtype)) return PLANK_FALSE;
    if (pl_JSON_StringGet (&jtype, &ptype) != PlankResult_OK) return PLANK_FALSE;
    
    return strcmp (ptype, type) == 0;
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

static inline PlankResult pl_JSON_DeInit (PlankJSONRef p)
{
    return pl_JSON_DecrementRefCount (p);
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
    if (!value) return PlankResult_MemoryError;
    if (!json_is_object (p->json)) return PlankResult_JSONError;
    if (value->json) pl_JSON_DecrementRefCount (value);
    value->json = json_object_get (p->json, key);
    return PlankResult_OK;
}

static inline PlankResult pl_JSON_ObjectSetValue (PlankJSONRef p, const char* key, const PlankJSONRef value)
{
    int jerr;
    if (!p) return PlankResult_MemoryError;
    if (!value) return PlankResult_MemoryError;
    if (!json_is_object (p->json)) return PlankResult_JSONError;
    jerr = json_object_set_new (p->json, key, value->json ? value->json : json_null());
    value->json = 0;
    return jerr == 0 ? PlankResult_OK : PlankResult_JSONError;
}

static inline PlankResult pl_JSON_ObjectSetValueFloat (PlankJSONRef p, const char* key, const float value)
{
    PlankJSON jvalue;
    if (pl_JSON_InitFloat (&jvalue, value) != PlankResult_OK) return PlankResult_JSONError;
    return pl_JSON_ObjectSetValue (p, key, &jvalue);
}

static inline PlankResult pl_JSON_ObjectSetValueDouble (PlankJSONRef p, const char* key, const double value)
{
    PlankJSON jvalue;
    if (pl_JSON_InitDouble (&jvalue, value) != PlankResult_OK) return PlankResult_JSONError;
    return pl_JSON_ObjectSetValue (p, key, &jvalue);
}

static inline PlankResult pl_JSON_ObjectSetValueInt (PlankJSONRef p, const char* key, const int value)
{
    PlankJSON jvalue;
    if (pl_JSON_InitInt (&jvalue, value) != PlankResult_OK) return PlankResult_JSONError;
    return pl_JSON_ObjectSetValue (p, key, &jvalue);
}

static inline PlankResult pl_JSON_ObjectSetValueString (PlankJSONRef p, const char* key, const char* value)
{
    PlankJSON jvalue;
    if (pl_JSON_InitString (&jvalue, value) != PlankResult_OK) return PlankResult_JSONError;
    return pl_JSON_ObjectSetValue (p, key, &jvalue);
}

static inline PlankResult pl_JSON_ObjectGetValueFloat (PlankJSONRef p, const char* key, float* value)
{
    PlankJSON jvalue;
    pl_JSON_Init (&jvalue);
    if (pl_JSON_ObjectGetValue (p, key, &jvalue) != PlankResult_OK) return PlankResult_JSONError;
    return pl_JSON_FloatGet (&jvalue, value);
}

static inline PlankResult pl_JSON_ObjectGetValueDouble (PlankJSONRef p, const char* key, double* value)
{
    PlankJSON jvalue;
    pl_JSON_Init (&jvalue);
    if (pl_JSON_ObjectGetValue (p, key, &jvalue) != PlankResult_OK) return PlankResult_JSONError;
    return pl_JSON_DoubleGet (&jvalue, value);
}

static inline PlankResult pl_JSON_ObjectGetValueInt (PlankJSONRef p, const char* key, int* value)
{
    PlankJSON jvalue;
    pl_JSON_Init (&jvalue);
    if (pl_JSON_ObjectGetValue (p, key, &jvalue) != PlankResult_OK) return PlankResult_JSONError;
    return pl_JSON_IntGet (&jvalue, value);
}

static inline PlankResult pl_JSON_ObjectGetValueString (PlankJSONRef p, const char* key, const char** value)
{
    PlankJSON jvalue;
    pl_JSON_Init (&jvalue);
    if (pl_JSON_ObjectGetValue (p, key, &jvalue) != PlankResult_OK) return PlankResult_JSONError;
    return pl_JSON_StringGet (&jvalue, value);
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
    if (!value) return PlankResult_MemoryError;
    if (!json_is_array (p->json)) return PlankResult_JSONError;
    if (value->json) pl_JSON_DecrementRefCount (value);
    value->json = json_array_get (p->json, index);
    return PlankResult_OK;
}

static inline PlankResult pl_JSON_ArrayPut (PlankJSONRef p, const int index, const PlankJSONRef value)
{
    int jerr;
    if (!p) return PlankResult_MemoryError;
    if (!value) return PlankResult_MemoryError;
    if (!json_is_array (p->json)) return PlankResult_JSONError;
    jerr = json_array_set_new (p->json, index, value->json ? value->json : json_null());
    value->json = 0;
    return jerr == 0 ? PlankResult_OK : PlankResult_JSONError;
}

static inline PlankResult pl_JSON_ArrayAppend (PlankJSONRef p, const PlankJSONRef value)
{
    int jerr;
    if (!p) return PlankResult_MemoryError;
    if (!value) return PlankResult_MemoryError;
    if (!json_is_array (p->json)) return PlankResult_JSONError;
    jerr = json_array_append_new (p->json, value->json ? value->json : json_null());
    value->json = 0;
    return jerr == 0 ? PlankResult_OK : PlankResult_JSONError;
}

static PlankResult pl_JSON_ArrayPutFloat (PlankJSONRef p, const int index, const float value)
{
    PlankJSON jvalue;
    if (pl_JSON_InitFloat (&jvalue, value) != PlankResult_OK) return PlankResult_JSONError;
    return pl_JSON_ArrayPut (p, index, &jvalue);
}

static PlankResult pl_JSON_ArrayPutDouble (PlankJSONRef p, const int index, const double value)
{
    PlankJSON jvalue;
    if (pl_JSON_InitDouble (&jvalue, value) != PlankResult_OK) return PlankResult_JSONError;
    return pl_JSON_ArrayPut (p, index, &jvalue);   
}

static PlankResult pl_JSON_ArrayPutInt (PlankJSONRef p, const int index, const int value)
{
    PlankJSON jvalue;
    if (pl_JSON_InitInt (&jvalue, value) != PlankResult_OK) return PlankResult_JSONError;
    return pl_JSON_ArrayPut (p, index, &jvalue);
}

static PlankResult pl_JSON_ArrayPutString (PlankJSONRef p, const int index, const char* value)
{
    PlankJSON jvalue;
    if (pl_JSON_InitString (&jvalue, value) != PlankResult_OK) return PlankResult_JSONError;
    return pl_JSON_ArrayPut (p, index, &jvalue);
}

static PlankResult pl_JSON_ArrayAtFloat (PlankJSONRef p, const int index, float* value)
{
    PlankJSON jvalue;
    pl_JSON_Init (&jvalue);
    if (pl_JSON_ArrayAt (p, index, &jvalue) != PlankResult_OK) return PlankResult_JSONError;
    return pl_JSON_FloatGet (&jvalue, value);
}

static PlankResult pl_JSON_ArrayAtDouble (PlankJSONRef p, const int index, double* value)
{
    PlankJSON jvalue;
    pl_JSON_Init (&jvalue);
    if (pl_JSON_ArrayAt (p, index, &jvalue) != PlankResult_OK) return PlankResult_JSONError;
    return pl_JSON_DoubleGet (&jvalue, value);
}

static PlankResult pl_JSON_ArrayAtInt (PlankJSONRef p, const int index, int* value)
{
    PlankJSON jvalue;
    pl_JSON_Init (&jvalue);
    if (pl_JSON_ArrayAt (p, index, &jvalue) != PlankResult_OK) return PlankResult_JSONError;
    return pl_JSON_IntGet (&jvalue, value);
}

static PlankResult pl_JSON_ArrayAtString (PlankJSONRef p, const int index, const char** value)
{
    PlankJSON jvalue;
    pl_JSON_Init (&jvalue);
    if (pl_JSON_ArrayAt (p, index, &jvalue) != PlankResult_OK) return PlankResult_JSONError;
    return pl_JSON_StringGet (&jvalue, value);
}

static PlankResult pl_JSON_ArrayAppendFloat (PlankJSONRef p, const float value)
{
    PlankJSON jvalue;
    if (pl_JSON_InitFloat (&jvalue, value) != PlankResult_OK) return PlankResult_JSONError;
    return pl_JSON_ArrayAppend (p, &jvalue);
}

static PlankResult pl_JSON_ArrayAppendDouble (PlankJSONRef p, const double value)
{
    PlankJSON jvalue;
    if (pl_JSON_InitDouble (&jvalue, value) != PlankResult_OK) return PlankResult_JSONError;
    return pl_JSON_ArrayAppend (p, &jvalue);
}

static PlankResult pl_JSON_ArrayAppendInt (PlankJSONRef p, const int value)
{
    PlankJSON jvalue;
    if (pl_JSON_InitInt (&jvalue, value) != PlankResult_OK) return PlankResult_JSONError;
    return pl_JSON_ArrayAppend (p, &jvalue);
}

static PlankResult pl_JSON_ArrayAppendString (PlankJSONRef p, const char* value)
{
    PlankJSON jvalue;
    if (pl_JSON_InitString (&jvalue, value) != PlankResult_OK) return PlankResult_JSONError;
    return pl_JSON_ArrayAppend (p, &jvalue);
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


