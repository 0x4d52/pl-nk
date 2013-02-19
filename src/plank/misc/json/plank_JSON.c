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
#include "plank_JSON.h"

#include "../../../../ext/jansson/hashtable.c"
#include "../../../../ext/jansson/memory.c"
#include "../../../../ext/jansson/dump.c"
#include "../../../../ext/jansson/pack_unpack.c"
#include "../../../../ext/jansson/strconv.c"
#include "../../../../ext/jansson/value.c"
#include "../../../../ext/jansson/error.c"
#include "../../../../ext/jansson/load.c"
#include "../../../../ext/jansson/strbuffer.c"
#include "../../../../ext/jansson/utf.c"

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
    PlankJSONRef j;
    PlankResult result;
    int fileMode;
    json_error_t jerror;
    
    j = 0;
    result = PlankResult_OK;
    
    if ((result = pl_File_GetMode (f, &fileMode)) != PlankResult_OK) goto exit;
    
    if (!(fileMode & PLANKFILE_READ))
    {
        result = PlankResult_JSONError;
        goto exit;
    }
    
    if (!(fileMode & PLANKFILE_BINARY))
    {
        result = PlankResult_JSONError;
        goto exit;
    }    
    
    j = (PlankJSONRef)json_load_callback (pl_JSONLoadCallback, f, 0, &jerror);
    
exit:
    return j;
}

PlankResult pl_JSON_WriteToFile (PlankJSONRef p, PlankFileRef f)
{
    PlankResult result;
    int fileMode;
    int jerror;

    result = PlankResult_OK;
    
    if (! (pl_JSON_IsArray (p) || pl_JSON_IsObject (p)))
    {
        result = PlankResult_JSONError;
        goto exit;
    }

    if ((result = pl_File_GetMode (f, &fileMode)) != PlankResult_OK) goto exit;
    
    if (!(fileMode & PLANKFILE_WRITE))
    {
        result = PlankResult_JSONError;
        goto exit;
    }
    
    if (!(fileMode & PLANKFILE_BINARY))
    {
        result = PlankResult_JSONError;
        goto exit;
    }
    
    if (!p)
    {
        result = PlankResult_MemoryError;
        goto exit;
    }
    
    jerror = json_dump_callback ((json_t*)p, pl_JSONDumpCallback, f, JSON_PRESERVE_ORDER | JSON_INDENT (2));

    if (jerror != 0)
    {
        result = PlankResult_JSONError;
        goto exit;
    }
    
exit:
    return PlankResult_OK;
}

//

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
        return pl_JSON_IntGet (p);
}

static void pl_JSON_ObjectSetFormat (PlankJSONRef p, const char* format)
{
    pl_JSON_ObjectPutKey (p, PLANK_JSON_FORMAT, pl_JSON_String (format));
}

void pl_JSON_ObjectSetFormatBinary (PlankJSONRef p)
{
    pl_JSON_ObjectSetFormat (p, PLANK_JSON_FORMATBINARY);
}

void pl_JSON_ObjectSetFormatText (PlankJSONRef p)
{
    pl_JSON_ObjectSetFormat (p, PLANK_JSON_FORMATTEXT);
}

void pl_JSON_ObjectSetFormatZip (PlankJSONRef p)
{
    pl_JSON_ObjectSetFormat (p, PLANK_JSON_FORMATTEXT);
}

PlankB pl_JSON_IsFormatBinary (PlankJSONRef p)
{
    return p ? strcmp (pl_JSON_StringGet (p), PLANK_JSON_FORMATBINARY) == 0 : PLANK_FALSE;
}

PlankB pl_JSON_IsFormatText (PlankJSONRef p)
{
    return p ? strcmp (pl_JSON_StringGet (p), PLANK_JSON_FORMATTEXT) == 0 : PLANK_FALSE;
}

PlankB pl_JSON_IsFormatZip (PlankJSONRef p)
{
    return p ? strcmp (pl_JSON_StringGet (p), PLANK_JSON_FORMATZIP) == 0 : PLANK_FALSE;
}

PlankB pl_JSON_IsObjectType (PlankJSONRef p, const char* type)
{
    return pl_JSON_IsObject (p) && (strcmp (pl_JSON_StringGet (pl_JSON_ObjectAtKey (p, PLANK_JSON_TYPE)), type) == 0);
}

PlankB pl_JSON_IsObjectFormatBinary (PlankJSONRef p)
{
    return pl_JSON_IsObject (p) && pl_JSON_IsFormatBinary (pl_JSON_ObjectAtKey (p, PLANK_JSON_FORMAT));
}

PlankB pl_JSON_IsObjectFormatText (PlankJSONRef p)
{
    return pl_JSON_IsObject (p) && pl_JSON_IsFormatText (pl_JSON_ObjectAtKey (p, PLANK_JSON_FORMAT));
}

PlankB pl_JSON_IsObjectFormatZip (PlankJSONRef p)
{
    return pl_JSON_IsObject (p) && pl_JSON_IsFormatZip (pl_JSON_ObjectAtKey (p, PLANK_JSON_FORMAT));
}


