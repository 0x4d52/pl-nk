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

PlankResult pl_JSON_InitFromFile (PlankJSONRef p, PlankFileRef f)
{
    PlankResult result;
    int fileMode;
    json_error_t jerror;
    
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
    
    if (!p)
    {
        result = PlankResult_MemoryError;
        goto exit;
    }
        
    p->json = json_load_callback (pl_JSONLoadCallback, f, 0, &jerror);
    
exit:
    return result;
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
    
    jerror = json_dump_callback (p->json, pl_JSONDumpCallback, f, JSON_PRESERVE_ORDER | JSON_INDENT (2));

    if (jerror != 0)
    {
        result = PlankResult_JSONError;
        goto exit;
    }
    
exit:
    return PlankResult_OK;
}


