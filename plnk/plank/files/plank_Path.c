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

#include "../core/plank_StandardHeader.h"
#include "plank_Path.h"
#include "../maths/plank_Maths.h"
#include "../random/plank_RNG.h"

#if PLANK_MAC
# include <pwd.h>
#elif PLANK_IOS
//
#elif PLANK_WIN
# include <Shlobj.h>
#else
//
#endif


PlankResult pl_Path_Init (PlankPathRef p)
{
    if (p == PLANK_NULL)
        return PlankResult_MemoryError;
    
    p->temp[0] = '\0';
    
    return pl_DynamicArray_InitWithItemSize (&p->buffer, 1);
}

PlankResult pl_Path_InitPath (PlankPathRef p, const char* path)
{
    PlankResult result;
    
    if ((result = pl_Path_Init (p)) != PlankResult_OK) goto exit;
    
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
    
    if ((p == PLANK_NULL) || (full == PLANK_NULL))
    {
        result = PlankResult_MemoryError;
        goto exit;
    }
    
    p->temp[0] = '\0';
    
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

    if ((result = pl_DynamicArray_SetSize (&p->buffer, pos + 1)) != PlankResult_OK) goto exit;
    
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
    
    p->temp[0] = '\0';
    
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
    
    p->temp[0] = '\0';
    
    if ((result = pl_Path_InitParent (&parent, siblingpath)) != PlankResult_OK) goto exit;
    if ((result = pl_DynamicArray_InitWithItemSize (&p->buffer, 1)) != PlankResult_OK) goto earlyExit;
    if ((result = pl_DynamicArray_SetAsText (&p->buffer, pl_Path_GetFullPath (&parent))) != PlankResult_OK) goto earlyExit;
    if ((result = pl_DynamicArray_AppendText (&p->buffer, child)) != PlankResult_OK) goto earlyExit;
    
earlyExit:
    pl_Path_DeInit (&parent);
    
exit:
    return result;
}

#if PLANK_MAC

static const char* pl_Path_MacSystemUserHome()
{
    struct passwd *pw = getpwuid (getuid());
    return pw ? pw->pw_dir : "";
}

static CFURLRef pl_Path_MacSystemCopyAppBundleURL()
{
    CFBundleRef bundle = CFBundleGetMainBundle();
    CFURLRef url = CFBundleCopyBundleURL (bundle);
    return  url;
}

PlankResult pl_Path_InitSystem (PlankPathRef p, const int systemPath, const char* child)
{
    PlankResult result;
    CFURLRef cfurl;
    CFStringRef cfstring;
    const char* parent;
    const char* append;
    char temp[1024];
    
    result = PlankResult_OK;
    cfurl = nil;
    cfstring = nil;
    parent = "";
    append = "";

    if (p == PLANK_NULL)
    {
        result = PlankResult_MemoryError;
        goto exit;
    }
    
    p->temp[0] = '\0';
    
    switch (systemPath)
    {
        case PLANKPATH_SYSTEMUSERHOME: {
            parent = pl_Path_MacSystemUserHome();
            append = "/";
        } break;
        case PLANKPATH_SYSTEMUSERDOCUMENTS: {
            parent = pl_Path_MacSystemUserHome();
            append = "/Documents/";
        } break;
        case PLANKPATH_SYSTEMUSERDESKTOP: {
            parent = pl_Path_MacSystemUserHome();
            append = "/Desktop/";
        } break;
        case PLANKPATH_SYSTEMUSERAPPDATA: {
            parent = pl_Path_MacSystemUserHome();
            append = "/Library/";
        } break;
        case PLANKPATH_SYSTEMAPPDATA: {
            parent = "/Library/";
            append = "";
        } break;
        case PLANKPATH_SYSTEMAPP: {
            cfurl = pl_Path_MacSystemCopyAppBundleURL();
            cfstring = CFURLCopyFileSystemPath (cfurl, kCFURLPOSIXPathStyle);
            parent = CFStringGetCStringPtr (cfstring, kCFStringEncodingUTF8);
            
            if (!parent)
            {
                if (CFStringGetCString (cfstring, temp, 1024, kCFStringEncodingUTF8))
                    parent = temp;
                else
                    parent = "";
            }
            
            append = "/";
        } break;
        case PLANKPATH_SYSTEMTEMP: {
            parent = pl_Path_MacSystemUserHome();
            append = "/Library/Caches/";
        } break;
        default: {
            result = PlankResult_FilePathInvalid;
            goto exit;
        }
    }
    
    if (parent)
    {
        if ((result = pl_DynamicArray_InitWithItemSize (&p->buffer, 1)) != PlankResult_OK) goto exit;
        if ((result = pl_DynamicArray_SetAsText (&p->buffer, parent)) != PlankResult_OK) goto exit;
        if ((result = pl_DynamicArray_AppendText (&p->buffer, append)) != PlankResult_OK) goto exit;
        
        if (child)
        {
            if ((result = pl_DynamicArray_AppendText (&p->buffer, child)) != PlankResult_OK) goto exit;
        }
    }
    else
    {
        result = PlankResult_FilePathInvalid;
        goto exit;
    }
    
exit:
    if (cfurl)
        CFRelease (cfurl);
    
    if (cfstring)
        CFRelease (cfstring);
    
    return result;
}

#elif PLANK_IOS

static CFURLRef pl_Path_IOSSystemCopyAppBundleURL()
{
    CFBundleRef bundle = CFBundleGetMainBundle();
    CFURLRef url = CFBundleCopyBundleURL (bundle);
    return  url;
}

PlankResult pl_Path_InitSystem (PlankPathRef p, const int systemPath, const char* child)
{
    PlankResult result;
    CFURLRef cfurl;
    CFStringRef cfstring;
    const char* parent;
    const char* append;
    char temp[1024];
    PlankL pos;
    
    result = PlankResult_OK;
    cfurl = nil;
    cfstring = nil;
    parent = "";
    append = "";
    
    if (p == PLANK_NULL)
    {
        result = PlankResult_MemoryError;
        goto exit;
    }
    
    p->temp[0] = '\0';
    
    cfurl = pl_Path_IOSSystemCopyAppBundleURL();
    cfstring = CFURLCopyFileSystemPath (cfurl, kCFURLPOSIXPathStyle);
    
    if (CFStringGetCString (cfstring, temp, 1024, kCFStringEncodingUTF8))
    {
        parent = temp;
    }
    else
    {
        result = PlankResult_FilePathInvalid;
        goto exit;
    }
    
    if (systemPath != PLANKPATH_SYSTEMAPP)
    {
        pos = strlen (temp);
        
        while (temp[--pos] != '/')
            ;
        
        temp[pos] = '\0';
    }
    
    switch (systemPath)
    {
        case PLANKPATH_SYSTEMUSERHOME: {
            append = "/";
        } break;
        case PLANKPATH_SYSTEMUSERDOCUMENTS: {
            append = "/Documents/";
        } break;
        case PLANKPATH_SYSTEMUSERDESKTOP: {
            parent = "";
            append = "";
        } break;
        case PLANKPATH_SYSTEMUSERAPPDATA: {
            append = "/Documents/";
        } break;
        case PLANKPATH_SYSTEMAPPDATA: {
            append = "/Documents/";
        } break;
        case PLANKPATH_SYSTEMAPP: {            
            append = "/";
        } break;
        case PLANKPATH_SYSTEMTEMP: {
            append = "/tmp/";
        } break;            
        default: {
            result = PlankResult_FilePathInvalid;
            goto exit;
        }
    }
    
    if (parent)
    {
        if ((result = pl_DynamicArray_InitWithItemSize (&p->buffer, 1)) != PlankResult_OK) goto exit;
        if ((result = pl_DynamicArray_SetAsText (&p->buffer, parent)) != PlankResult_OK) goto exit;
        if ((result = pl_DynamicArray_AppendText (&p->buffer, append)) != PlankResult_OK) goto exit;
        
        if (child)
        {
            if ((result = pl_DynamicArray_AppendText (&p->buffer, child)) != PlankResult_OK) goto exit;
        }
    }
    else
    {
        result = PlankResult_FilePathInvalid;
        goto exit;
    }
    
exit:
    if (cfurl)
        CFRelease (cfurl);
    
    if (cfstring)
        CFRelease (cfstring);
    
    return result;
}

#elif PLANK_WIN

PlankResult pl_Path_WinSystemCSIDL (const int csidl, char* path, int max)
{
    HRESULT hr = SHGetFolderPath (0, csidl, 0, SHGFP_TYPE_CURRENT, path);
	(void)max;
    return (hr == S_OK) ? PlankResult_OK : PlankResult_FilePathInvalid;
}

void pl_Path_WinReplaceBackslash (char* path)
{
    if (path)
    {
        while (*path != '\0')
        {
            if (*path == '\\')
                *path = '/';
            
            ++path;
        }
    }
}

PlankResult pl_Path_InitSystem (PlankPathRef p, const int systemPath, const char* child)
{
    PlankResult result;
    const char* parent;
    const char* append;
    char temp[2048]; // must be > MAX_PATH i.e., 260 chars
    
    result = PlankResult_OK;
    parent = "";
    append = "";
    temp[0] = '\0';
    
    if (p == PLANK_NULL)
    {
        result = PlankResult_MemoryError;
        goto exit;
    }
    
    p->temp[0] = '\0';
    
    switch (systemPath)
    {
        case PLANKPATH_SYSTEMUSERHOME: {
            if ((result = pl_Path_WinSystemCSIDL (CSIDL_PROFILE, temp, MAX_PATH)) != PlankResult_OK) goto exit;
            parent = temp;
            append = "/";
        } break;
        case PLANKPATH_SYSTEMUSERDOCUMENTS: {
            if ((result = pl_Path_WinSystemCSIDL (CSIDL_PERSONAL, temp, MAX_PATH)) != PlankResult_OK) goto exit;
            parent = temp;
            append = "/";
        } break;
        case PLANKPATH_SYSTEMUSERDESKTOP: {
            if ((result = pl_Path_WinSystemCSIDL (CSIDL_DESKTOP, temp, MAX_PATH)) != PlankResult_OK) goto exit;
            parent = temp;
            append = "/";
        } break;
        case PLANKPATH_SYSTEMUSERAPPDATA: {
            if ((result = pl_Path_WinSystemCSIDL (CSIDL_APPDATA, temp, MAX_PATH)) != PlankResult_OK) goto exit;
            parent = temp;
            append = "/";
        } break;
        case PLANKPATH_SYSTEMAPPDATA: {
            if ((result = pl_Path_WinSystemCSIDL (CSIDL_COMMON_APPDATA, temp, MAX_PATH)) != PlankResult_OK) goto exit;
            parent = temp;
            append = "/";
        } break;
        case PLANKPATH_SYSTEMAPP: {            

            GetModuleFileName (GetModuleHandleA (0), temp, 2048);

            // need to get up to the last sep
            
            parent = temp;
            append = "/";
        } break;
        case PLANKPATH_SYSTEMTEMP: {
            GetTempPath (2048, temp);
            parent = temp;
            append = "";
        } break;
        default: {
            result = PlankResult_FilePathInvalid;
            goto exit;
        }
    }
    
    if (parent)
    {
//        pl_Path_WinReplaceBackslash ((char*)parent); // fix below
        
        if ((result = pl_DynamicArray_InitWithItemSize (&p->buffer, 1)) != PlankResult_OK) goto exit;
        if ((result = pl_DynamicArray_SetAsText (&p->buffer, parent)) != PlankResult_OK) goto exit;
        
        pl_Path_WinReplaceBackslash ((char*)pl_DynamicArray_GetArray (&p->buffer));
        
        if ((result = pl_DynamicArray_AppendText (&p->buffer, append)) != PlankResult_OK) goto exit;
        
        if (child)
        {
            if ((result = pl_DynamicArray_AppendText (&p->buffer, child)) != PlankResult_OK) goto exit;
        }
    }
    else
    {
        result = PlankResult_FilePathInvalid;
        goto exit;
    }
    
exit:    
    return result;
}
#else
PlankResult pl_Path_InitSystem (PlankPathRef p, const int systemPath, const char* child)
{
    PlankResult result;
    
    result = PlankResult_UnknownError;
    
exit:
    return result;
}
#endif

//PlankResult pl_Path_InitTemp (PlankPathRef p)
//{
//    PlankRNGRef rng;
//    char name[256];
//
//    rng = pl_RNGGlobal();
//    
//    snprintf (name, 256, "temp_%08x", (PlankUI)(((size_t)p + pl_RNG_Next (rng)) & 0xFFFFFFFF));
//    return pl_Path_InitSystem (p, PLANKPATH_SYSTEMTEMP, name);
//}

PlankResult pl_Path_InitTemp (PlankPathRef p, const char* prefix, const char* ext)
{
    char name[1024];
    PlankGUID guid;
    
    name[0] = '\0';
    
    if (prefix)
        strncpy (name, prefix, 512);
    
    pl_GUID_InitRandom (&guid);
    pl_GUID_HexString (&guid, PLANK_TRUE, name + strlen (name));
    
    if (ext && ext[0] != '\0')
    {
        strcat  (name, ".");
        strncat (name, ext, 64);
    }
    
    return pl_Path_InitSystem (p, PLANKPATH_SYSTEMTEMP, name);
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

    if ((end == 2) && (fullpath[1] == ':') && (fullpath[2] == '/'))
        return fullpath;
    
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

const char* pl_Path_GetRoot (PlankPathRef p)
{
    PlankL length, i;
    const char* fullpath;
    
    length = pl_MinL (pl_Path_GetFullPathLength (p), 63);
    
    if (length == 0)
        return "";
    
    fullpath = pl_Path_GetFullPath (p);

    if (length > 2)
    {
        if ((fullpath[0] == '/') && (fullpath[1] == '/'))
        {
            fullpath += 2;
            length -= 2;
        }
    }
    
    for (i = 0; i < length; ++i)
    {
        p->temp[i] = fullpath[i];
        
        if (fullpath[i] == '/')
            break;
    }
    
    p->temp[i + 1] = '\0';
    
    return p->temp;
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

PlankResult pl_Path_Resolve (PlankPathRef p)
{
    PlankResult result;
    PlankL length, pos, pppos, nextpos;
    char* fullpath;
    char* temp;
    
    result = PlankResult_OK;
    length = pl_Path_GetFullPathLength (p);
    fullpath = (char*)pl_Path_GetFullPath (p);

    if (length < 3)
        goto exit;
    
    length -= 1; // as last possible './'
    pos = 0;
    
    while ((pos < length) && (fullpath[pos] != '\0'))
    {
        temp = fullpath + pos;
        
        if ((fullpath[pos + 0] == '.') &&
            (fullpath[pos + 1] == '.') &&
            (fullpath[pos + 2] == '/'))
        {
            nextpos = pos + 2;
            
            pos -= 2;
            
            while ((pos >= 0) && (fullpath[pos] != '/'))
                --pos;
            
            pppos = pos + 1;
            temp = fullpath + pppos;
            
            if (pos >= 0)
            {
                while (fullpath[nextpos] != '\0')
                    fullpath[pos++] = fullpath[nextpos++];
                
                fullpath[pos] = '\0';
            }
            
            pos = pppos;
        }
        else if ((fullpath[pos + 0] == '.') &&
                 (fullpath[pos + 1] == '/'))
        {
            nextpos = pos + 1;
            pos -= 1;
            pppos = pos + 1;
            temp = fullpath + pppos;
            
            if (pos >= 0)
            {
                while (fullpath[nextpos] != '\0')
                    fullpath[pos++] = fullpath[nextpos++];
                
                fullpath[pos] = '\0';
            }
            
            pos = pppos;
        }
        else ++pos;
    }
    
exit:
    return result;
}


///---
//
//PlankResult pl_PathIter_InitFullPath (PlankPathIterRef p, const char* fullPath)
//{
//    if (p == PLANK_NULL)
//        return PlankResult_MemoryError;
//        
//    pl_DynamicArray_InitWithItemSizeAndCapacity (&p->fullParentPath, 1, strlen(fullPath) + 1);
//    pl_DynamicArray_SetAsText (&p->fullParentPath, fullPath);
//    p->index = 0;
//    
//    return PlankResult_OK;
//}
//
//PlankResult pl_PathIter_DeInit (PlankPathIterRef p)
//{
//    if (p == PLANK_NULL)
//        return PlankResult_MemoryError;
//    
//    return pl_DynamicArray_DeInit (&p->fullParentPath);
//}
//
//const char* pl_PathIter_GetFullParentPath (PlankPathIterRef p)
//{
//    return (const char*)pl_DynamicArray_GetArray (&p->fullParentPath);
//}
//
//const char* pl_PathIter_GetNextChild (PlankPathIterRef p)
//{
//    
//}
//

