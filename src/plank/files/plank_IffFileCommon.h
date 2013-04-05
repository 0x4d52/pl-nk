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

#ifndef PLANK_IFFFILECOMMON_H
#define PLANK_IFFFILECOMMON_H

#include "plank_File.h"

#define PLANKIFFFILE_CURRENTCHUNKPOSITION   -1
#define PLANKIFFFILE_ANYCHUNKID             -1
#define PLANKIFFFILE_ID_FCC                  1
#define PLANKIFFFILE_ID_GUID                 2

#define PLANKIFFFILE_W64_RIFF_ID          "riff-912E-11CF-A5D6-28DB04C10000"
#define PLANKIFFFILE_W64_LIST_ID          "list-912F-11CF-A5D6-28DB04C10000"
#define PLANKIFFFILE_W64_JUNK_ID          "junk-ACF3-11D3-8CD1-00C04f8EDB8A"


typedef union PlankIffID
{
    PlankFourCharCode fcc;
    PlankGUID guid;
} PlankIffID;

typedef struct PlankIffFileHeaderInfo
{
    PlankIffID mainID;
    PlankIffID formatID;
    PlankIffID junkID;
    
    PlankLL mainLength;

    PlankUC lengthSize;
    PlankUC idType;
    PlankUC headerLength;
    PlankUC reserved3;
    PlankUC mainEndOffset;
    PlankUC initMainLength;
    PlankUC alignment;
    PlankUC reserved7;
    
} PlankIffFileHeaderInfo;

typedef struct PlankIffFile* PlankIffFileRef;
typedef struct PlankIffFile
{
    PlankFile file;
    PlankIffFileHeaderInfo headerInfo;
} PlankIffFile;



#endif // PLANK_IFFFILECOMMON_H
