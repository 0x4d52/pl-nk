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

#include "../core/plank_StandardHeader.h"
#include "plank_Path.h"

PlankResult pl_Path_Init (PlankPathRef p)
{
    if (p == PLANK_NULL)
        return PlankResult_MemoryError;
        
    return pl_DynamicArray_InitWithItemSize (&p->buffer, 1);
}

PlankResult pl_Path_InitPath (PlankPathRef p, const char* path)
{
    PlankResult result;
    
    if ((result = pl_Path_Init(p)) != PlankResult_OK) goto exit;
    
    result = pl_Path_Set (p, path);
    
exit:
    return result;
}

PlankResult pl_Path_InitParent (PlankPathRef p, PlankPathRef full)
{
    PlankResult result;
    PlankL length, pos;
    const char* fullpath;
    char* parentPath;
    
    result = PlankResult_OK;
    
    if ((p == PLANK_NULL) || (fullpath == PLANK_NULL))
    {
        result = PlankResult_MemoryError;
        goto exit;
    }
    
    length = pl_Path_GetFullPathLength (full);
    fullpath = pl_Path_GetFullPath (full);
    
    if ((result = pl_DynamicArray_InitWithItemSize (&p->buffer, 1)) != PlankResult_OK) goto exit;
    if ((result = pl_DynamicArray_SetAsText (&p->buffer, fullpath)) != PlankResult_OK) goto exit;
    
    parentPath = pl_DynamicArray_GetArray (&p->buffer);
    pos = length - 1;
    
    if (pos > 0)
    {
        if (fullpath[pos] == '/')
            --pos;
    
        do {
            --pos;
        } while ((pos > 0) && (fullpath[pos] != '/'));
    
        ++pos;        
    }
    else
    {
        pos = 0;
    }
    
    parentPath[pos]  = '\0';

    if ((result = pl_DynamicArray_SetSize(&p->buffer, pos + 1)) != PlankResult_OK) goto exit;
    
exit:
    return result;
}

PlankResult pl_Path_InitChild (PlankPathRef p, PlankPathRef parentpath, const char* child)
{
    PlankResult result;
    
    result = PlankResult_OK;
    
    if ((p == PLANK_NULL) || (parentpath == PLANK_NULL))
    {
        result = PlankResult_MemoryError;
        goto exit;
    }
    
    if (!pl_Path_IsDirectory (parentpath))
    {
        result = PlankResult_FilePathInvalid;
        goto exit;
    }
    
    if ((result = pl_DynamicArray_InitWithItemSize (&p->buffer, 1)) != PlankResult_OK) goto exit;
    if ((result = pl_DynamicArray_SetAsText (&p->buffer, pl_Path_GetFullPath (parentpath))) != PlankResult_OK) goto exit;
    if ((result = pl_DynamicArray_AppendText (&p->buffer, child)) != PlankResult_OK) goto exit;

exit:
    return result;
}

PlankResult pl_Path_InitSibling (PlankPathRef p, PlankPathRef siblingpath, const char* child)
{
    PlankResult result;
    PlankPath parent;
    
    if ((p == PLANK_NULL) || (siblingpath == PLANK_NULL))
    {
        result = PlankResult_MemoryError;
        goto exit;
    }
    
    if ((result = pl_Path_InitParent (&parent, siblingpath)) != PlankResult_OK) goto exit;
    if ((result = pl_DynamicArray_InitWithItemSize (&p->buffer, 1)) != PlankResult_OK) goto earlyExit;
    if ((result = pl_DynamicArray_SetAsText (&p->buffer, pl_Path_GetFullPath (&parent))) != PlankResult_OK) goto earlyExit;
    if ((result = pl_DynamicArray_AppendText (&p->buffer, child)) != PlankResult_OK) goto earlyExit;
    
earlyExit:
    pl_Path_DeInit (&parent);
    
exit:
    return result;
}

PlankResult pl_Path_DeInit (PlankPathRef p)
{
    if (p == PLANK_NULL)
        return PlankResult_MemoryError;
    
    return pl_DynamicArray_DeInit (&p->buffer);
}

PlankResult pl_Path_Set (PlankPathRef p, const char* path)
{
    return pl_DynamicArray_SetAsText (&p->buffer, path);
}

const char* pl_Path_GetFullPath (PlankPathRef p)
{
    return (const char*)pl_DynamicArray_GetArray (&p->buffer);
}

const char* pl_Path_GetFilename (PlankPathRef p)
{
    PlankL pos;
    const char* fullpath;
    
    pos = pl_Path_GetFullPathLength (p);
    
    if (pos == 0)
        return "";
    
    fullpath = pl_Path_GetFullPath (p);
    
    do {
        --pos;
    } while ((pos > 0) && (fullpath[pos] != '/'));
    
    return fullpath + pos + 1;
}

const char* pl_Path_GetLastPath (PlankPathRef p)
{
    PlankL pos, end;
    const char* fullpath;
    
    pos = pl_Path_GetFullPathLength (p);
    end = pos - 1;
    
    if (pos == 0)
        return "";
    
    fullpath = pl_Path_GetFullPath (p);
    
    do {
        --pos;
    } while (((pos > 0) && (fullpath[pos] != '/')) || (pos >= end));
    
    return fullpath + pos + 1;
}

const char* pl_Path_GetFileExtension (PlankPathRef p)
{
    PlankL pos;
    const char* fullpath;
    
    pos = pl_Path_GetFullPathLength (p);
    
    if (pos == 0)
        return "";
    
    fullpath = pl_Path_GetFullPath (p);
    
    do {
        --pos;
    } while ((pos > 0) && (fullpath[pos] != '.') && (fullpath[pos] != '/'));
    
    return (fullpath[pos] == '.') ? fullpath + pos + 1 : "";
}

PlankL pl_Path_GetFullPathLength (PlankPathRef p)
{
    return pl_DynamicArray_GetSize (&p->buffer) - 1;

}

PlankB pl_Path_IsDirectory (PlankPathRef p)
{
    PlankL length;
    const char* fullpath;
    
    length = pl_Path_GetFullPathLength (p);
    fullpath = pl_Path_GetFullPath (p);
        
    return length < 1 ? PLANK_FALSE : fullpath[length - 1] == '/' ? PLANK_TRUE : PLANK_FALSE;
}

PlankB pl_Path_IsFile (PlankPathRef p)
{
    PlankL length;
    const char* fullpath;
    
    length = pl_Path_GetFullPathLength (p);
    fullpath = pl_Path_GetFullPath (p);
    
    return length < 1 ? PLANK_FALSE : fullpath[length - 1] != '/' ? PLANK_TRUE : PLANK_FALSE;
}







