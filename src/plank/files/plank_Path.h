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

#ifndef PLANK_PATH_H
#define PLANK_PATH_H

#include "../containers/plank_DynamicArray.h"

/** The maximum length of the file path. */
#define PLANKPATH_MAXLENGTH           FILENAME_MAX

#define PLANKPATH_SYSTEMUSERHOME            (1)
#define PLANKPATH_SYSTEMUSERDOCUMENTS       (2)
#define PLANKPATH_SYSTEMUSERDESKTOP         (3)
#define PLANKPATH_SYSTEMUSERAPPDATA         (4)
#define PLANKPATH_SYSTEMAPPDATA             (5)
#define PLANKPATH_SYSTEMAPP                 (7)
#define PLANKPATH_SYSTEMTEMP                (8)

PLANK_BEGIN_C_LINKAGE

typedef struct PlankPath* PlankPathRef;

PlankResult pl_Path_Init (PlankPathRef p);
PlankResult pl_Path_InitPath (PlankPathRef p, const char* path);
PlankResult pl_Path_InitParent (PlankPathRef p, PlankPathRef fullpath);
PlankResult pl_Path_InitChild (PlankPathRef p, PlankPathRef parentpath, const char* child);
PlankResult pl_Path_InitSibling (PlankPathRef p, PlankPathRef siblingpath, const char* child);
PlankResult pl_Path_InitSystem (PlankPathRef p, const int systemPath, const char* child);
PlankResult pl_Path_InitTemp (PlankPathRef p, const char* prefix, const char* ext);
PlankResult pl_Path_DeInit (PlankPathRef p);

PlankResult pl_Path_Set (PlankPathRef p, const char* path);
const char* pl_Path_GetFullPath (PlankPathRef p);
const char* pl_Path_GetFilename (PlankPathRef p);
const char* pl_Path_GetLastPath (PlankPathRef p);
const char* pl_Path_GetFileExtension (PlankPathRef p);
const char* pl_Path_GetRoot (PlankPathRef p);
PlankL pl_Path_GetFullPathLength (PlankPathRef p);

PlankB pl_Path_IsDirectory (PlankPathRef p);
PlankB pl_Path_IsFile (PlankPathRef p);
PlankResult pl_Path_Resolve (PlankPathRef p);

//typedef struct PlankPathIter* PlankPathIterRef;
//PlankResult pl_PathIter_InitFullPath (PlankPathIterRef p, const char* fullPath);
//PlankResult pl_PathIter_DeInit (PlankPathIterRef p);
//const char* pl_PathIter_GetFullParentPath (PlankPathIterRef p);
//const char* pl_PathIter_GetNextChild (PlankPathIterRef p);


PLANK_END_C_LINKAGE

#if !DOXYGEN
typedef struct PlankPath
{
    PlankDynamicArray buffer;
    char temp[64];
} PlankPath;

typedef struct PlankPathIter
{
    PlankDynamicArray fullParentPath;
    int index;
} PlankPathIter;

#endif


#endif // PLANK_PATH_H
