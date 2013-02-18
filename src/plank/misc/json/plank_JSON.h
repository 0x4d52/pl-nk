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

#ifndef PLANK_JSON_H
#define PLANK_JSON_H

#include "../../files/plank_File.h"

#define PLANK_JSON_TYPE "type"

PLANK_BEGIN_C_LINKAGE

typedef struct PlankJSON* PlankJSONRef;

static PlankResult pl_JSON_Init (PlankJSONRef p);

static PlankResult pl_JSON_InitObject (PlankJSONRef p);
static PlankResult pl_JSON_InitArray (PlankJSONRef p);
static PlankResult pl_JSON_InitString (PlankJSONRef p, const char* string);
static PlankResult pl_JSON_InitInt(PlankJSONRef p, const int value);
static PlankResult pl_JSON_InitFloat (PlankJSONRef p, const float value);
static PlankResult pl_JSON_InitDouble (PlankJSONRef p, const double value);
static PlankResult pl_JSON_InitBool (PlankJSONRef p, const PlankB state);
static PlankResult pl_JSON_InitNull (PlankJSONRef p);

PlankResult pl_JSON_InitFromFile (PlankJSONRef p, PlankFileRef f);
PlankResult pl_JSON_WriteToFile (PlankJSONRef p, PlankFileRef f);

static PlankB pl_JSON_IsObject (PlankJSONRef p);
static PlankB pl_JSON_IsArray (PlankJSONRef p);
static PlankB pl_JSON_IsString (PlankJSONRef p);
static PlankB pl_JSON_IsInt(PlankJSONRef p);
static PlankB pl_JSON_IsFloat (PlankJSONRef p);
static PlankB pl_JSON_IsDouble (PlankJSONRef p);
static PlankB pl_JSON_IsBool (PlankJSONRef p);
static PlankB pl_JSON_IsNull (PlankJSONRef p);

static PlankB pl_JSON_IsObjectType (PlankJSONRef p, const char* type);

static PlankResult pl_JSON_SetObject (PlankJSONRef p);
static PlankResult pl_JSON_SetArray (PlankJSONRef p);
static PlankResult pl_JSON_SetString (PlankJSONRef p, const char* string);
static PlankResult pl_JSON_SetInt(PlankJSONRef p, const int value);
static PlankResult pl_JSON_SetFloat (PlankJSONRef p, const float value);
static PlankResult pl_JSON_SetDouble (PlankJSONRef p, const double value);
static PlankResult pl_JSON_SetBool (PlankJSONRef p, const PlankB state);
static PlankResult pl_JSON_SetNull (PlankJSONRef p);


static PlankResult pl_JSON_DeInit (PlankJSONRef p);
static PlankResult pl_JSON_IncrementRefCount (PlankJSONRef p);
static PlankResult pl_JSON_DecrementRefCount (PlankJSONRef p);

static PlankResult pl_JSON_ObjectGetSize (PlankJSONRef p, int* size); 
static PlankResult pl_JSON_ObjectGetValue (PlankJSONRef p, const char* key, PlankJSONRef value);
static PlankResult pl_JSON_ObjectGetValueFloat (PlankJSONRef p, const char* key, float* value);
static PlankResult pl_JSON_ObjectGetValueDouble (PlankJSONRef p, const char* key, double* value);
static PlankResult pl_JSON_ObjectGetValueInt (PlankJSONRef p, const char* key, int* value);
static PlankResult pl_JSON_ObjectGetValueString (PlankJSONRef p, const char* key, const char** value);
static PlankResult pl_JSON_ObjectSetValue (PlankJSONRef p, const char* key, const PlankJSONRef value);
static PlankResult pl_JSON_ObjectSetValueFloat (PlankJSONRef p, const char* key, const float value);
static PlankResult pl_JSON_ObjectSetValueDouble (PlankJSONRef p, const char* key, const double value);
static PlankResult pl_JSON_ObjectSetValueInt (PlankJSONRef p, const char* key, const int value);
static PlankResult pl_JSON_ObjectSetValueString (PlankJSONRef p, const char* key, const char* value);


static PlankResult pl_JSON_ArrayGetSize (PlankJSONRef p, int* size);
static PlankResult pl_JSON_ArrayAt (PlankJSONRef p, const int index, PlankJSONRef value);
static PlankResult pl_JSON_ArrayPut (PlankJSONRef p, const int index, const PlankJSONRef value);
static PlankResult pl_JSON_ArrayAppend (PlankJSONRef p, const PlankJSONRef value);

static PlankResult pl_JSON_ArrayPutFloat (PlankJSONRef p, const int index, const float value);
static PlankResult pl_JSON_ArrayPutDouble (PlankJSONRef p, const int index, const double value);
static PlankResult pl_JSON_ArrayPutInt (PlankJSONRef p, const int index, const int value);
static PlankResult pl_JSON_ArrayPutString (PlankJSONRef p, const int index, const char* value);
static PlankResult pl_JSON_ArrayAtFloat (PlankJSONRef p, const int index, float* value);
static PlankResult pl_JSON_ArrayAtDouble (PlankJSONRef p, const int index, double* value);
static PlankResult pl_JSON_ArrayAtInt (PlankJSONRef p, const int index, int* value);
static PlankResult pl_JSON_ArrayAtString (PlankJSONRef p, const int index, const char** value);
static PlankResult pl_JSON_ArrayAppendFloat (PlankJSONRef p, const float value);
static PlankResult pl_JSON_ArrayAppendDouble (PlankJSONRef p, const double value);
static PlankResult pl_JSON_ArrayAppendInt (PlankJSONRef p, const int value);
static PlankResult pl_JSON_ArrayAppendString (PlankJSONRef p, const char* value);

static PlankResult pl_JSON_DoubleGet (PlankJSONRef p, double* value);
static PlankResult pl_JSON_FloatGet (PlankJSONRef p, float* value);
static PlankResult pl_JSON_IntGet (PlankJSONRef p, int* value);
static PlankResult pl_JSON_StringGet (PlankJSONRef p, const char** value);

PLANK_END_C_LINKAGE

#define PLANK_INLINING_FUNCTIONS 1
#include "plank_JSONInline.h"
#undef PLANK_INLINING_FUNCTIONS


#endif // PLANK_JSON_H
