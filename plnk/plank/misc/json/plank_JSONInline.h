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

#if PLANK_INLINING_FUNCTIONS

#include "../../../ext/jansson/jansson.h"

typedef struct PlankJSON
{
    json_t json;
} PlankJSON;


static PLANK_INLINE_LOW PlankJSONRef pl_JSON_Object()
{
    return (PlankJSONRef)json_object();
}

static PLANK_INLINE_LOW PlankJSONRef pl_JSON_Array()
{
    return (PlankJSONRef)json_array();
}

static PLANK_INLINE_LOW PlankJSONRef pl_JSON_String (const char* string)
{
    return (PlankJSONRef)json_string (string);
}

static PLANK_INLINE_LOW PlankJSONRef pl_JSON_Int (const PlankLL value)
{
    return (PlankJSONRef)json_integer ((PlankL)value);
}

static PLANK_INLINE_LOW PlankJSONRef pl_JSON_Float (const float value)
{
    return (PlankJSONRef)json_real ((double)value);
}

static PLANK_INLINE_LOW PlankJSONRef pl_JSON_Double (const double value)
{
    return (PlankJSONRef)json_real (value);
}

static PLANK_INLINE_LOW PlankJSONRef pl_JSON_Bool (const PlankB state)
{
    return (PlankJSONRef)json_boolean (state);
}

static PLANK_INLINE_LOW PlankJSONRef pl_JSON_Null()
{
    return (PlankJSONRef)json_null();
}

static PLANK_INLINE_LOW PlankB pl_JSON_IsObject (PlankJSONRef p)
{
    return json_is_object ((json_t*)p);
}

static PLANK_INLINE_LOW PlankB pl_JSON_IsArray (PlankJSONRef p)
{
    return json_is_array ((json_t*)p);
}

static PLANK_INLINE_LOW PlankB pl_JSON_IsString (PlankJSONRef p)
{
    return json_is_string ((json_t*)p);
}

static PLANK_INLINE_LOW PlankB pl_JSON_IsInt (PlankJSONRef p)
{
    return json_is_integer ((json_t*)p) ? PLANK_TRUE : pl_JSON_IsIntEncoded (p);
}

static PLANK_INLINE_LOW PlankB pl_JSON_IsFloat (PlankJSONRef p)
{
    return json_is_real ((json_t*)p) ? PLANK_TRUE : pl_JSON_IsFloatEncoded (p);
}

static PLANK_INLINE_LOW PlankB pl_JSON_IsDouble (PlankJSONRef p)
{
    return json_is_real ((json_t*)p) ? PLANK_TRUE : pl_JSON_IsDoubleEncoded (p);
}

static PLANK_INLINE_LOW PlankB pl_JSON_IsBool (PlankJSONRef p)
{
    return json_is_boolean ((json_t*)p);
}

static PLANK_INLINE_LOW PlankB pl_JSON_IsNull (PlankJSONRef p)
{
    return json_is_null ((json_t*)p);
}

static PLANK_INLINE_LOW PlankJSONRef pl_JSON_IncrementRefCount (PlankJSONRef p)
{
    return (PlankJSONRef)json_incref ((json_t*)p);
}

static PLANK_INLINE_LOW void pl_JSON_DecrementRefCount (PlankJSONRef p)
{
    json_decref ((json_t*)p);
}

static PLANK_INLINE_LOW PlankL pl_JSON_ObjectGetSize (PlankJSONRef p)
{
    return (PlankL)json_object_size ((json_t*)p);
}

static PLANK_INLINE_LOW PlankJSONRef pl_JSON_ObjectAtKey (PlankJSONRef p, const char* key)
{
    return (PlankJSONRef)json_object_get ((json_t*)p, key);
}

static PLANK_INLINE_LOW void pl_JSON_ObjectPutKey (PlankJSONRef p, const char* key, const PlankJSONRef value)
{
    json_object_set_new ((json_t*)p, key, (json_t*)value);
}

static PLANK_INLINE_LOW PlankL pl_JSON_ArrayGetSize (PlankJSONRef p)
{
    return (PlankL)json_array_size ((json_t*)p);
}

static PLANK_INLINE_LOW PlankJSONRef pl_JSON_ArrayAt (PlankJSONRef p, const PlankL index)
{
    return (PlankJSONRef)json_array_get ((json_t*)p, (size_t)index);
}

static PLANK_INLINE_LOW void pl_JSON_ArrayPut (PlankJSONRef p, const PlankL index, const PlankJSONRef value)
{
    json_array_set_new ((json_t*)p, (size_t)index, (json_t*)value);
}

static PLANK_INLINE_LOW void pl_JSON_ArrayAppend (PlankJSONRef p, const PlankJSONRef value)
{
    json_array_append_new ((json_t*)p, (json_t*)value);
}

static PLANK_INLINE_LOW double pl_JSON_DoubleGet (PlankJSONRef p)
{
    double value = 0.0;
    
    if (json_is_real ((json_t*)p))
    {
        value = json_real_value ((json_t*)p);
    }
    else if (pl_JSON_ObjectGetSize (p) == 1)
    {
        value = pl_JSON_DoubleEncodedGet (p);
    }
    
    return value;
}

static PLANK_INLINE_LOW float pl_JSON_FloatGet (PlankJSONRef p)
{
    float value = 0.f;
    
    if (json_is_real ((json_t*)p))
    {
        value = (float)json_real_value ((json_t*)p);
    }
    else if (pl_JSON_ObjectGetSize (p) == 1)
    {
        value = pl_JSON_FloatEncodedGet (p);
    }
    
    return value;
}

static PLANK_INLINE_LOW PlankLL pl_JSON_IntGet (PlankJSONRef p)
{
    PlankLL value = 0;
    
    if (json_is_integer ((json_t*)p))
    {
        value = (PlankLL)json_integer_value ((json_t*)p);
    }
    else if (pl_JSON_ObjectGetSize (p) == 1)
    {
        value = pl_JSON_IntEncodedGet (p);
    }
    
    return value;
}

static PLANK_INLINE_LOW const char* pl_JSON_StringGet (PlankJSONRef p)
{
    return json_string_value ((json_t*)p);
}




#endif // PLANK_INLINING_FUNCTIONS


