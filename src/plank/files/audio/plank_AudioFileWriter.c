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
#include "../plank_File.h"
#include "../plank_IffFileWriter.h"
#include "plank_AudioFileWriter.h"

//PlankAudioFileWriterRef pl_AudioFileWriter_CreateAndInit()
//{
//    PlankAudioFileWriterRef p;
//    p = pl_AudioFileWriter_Create();
//    
//    if (p != PLANK_NULL)
//    {
//        if (pl_AudioFileWriter_Init (p) != PlankResult_OK)
//            pl_AudioFileWriter_Destroy (p);
//        else
//            return p;
//    }
//    
//    return (PlankAudioFileWriterRef)PLANK_NULL;
//}
//
//PlankAudioFileWriterRef pl_AudioFileWriter_Create()
//{
//    return (PlankAudioFileWriterRef)PLANK_NULL;
//}
//
//PlankResult pl_AudioFileWriter_Init (PlankAudioFileWriterRef p)
//{
//	(void)p;
//    return PlankResult_UnknownError;
//}
//
//PlankResult pl_AudioFileWriter_DeInit (PlankAudioFileWriterRef p)
//{
//    PlankResult result = PlankResult_OK;
//    
//    if (p == PLANK_NULL)
//    {
//        result = PlankResult_MemoryError;
//        goto exit;
//    }
//    
//    result = pl_IffFileWriter_DeInit (&p->iff);
//    
//    pl_MemoryZero (p, sizeof (PlankAudioFileWriter));
//
//exit:
//    return result;
//}
//
//PlankResult pl_AudioFileWriter_Destroy (PlankAudioFileWriterRef p)
//{
//    PlankResult result = PlankResult_OK;
//    PlankMemoryRef m = pl_MemoryGlobal();
//    
//    if (p == PLANK_NULL)
//    {
//        result = PlankResult_MemoryError;
//        goto exit;
//    }
//    
//    if ((result = pl_AudioFileWriter_DeInit (p)) != PlankResult_OK)
//        goto exit;
//    
//    result = pl_Memory_Free (m, p);    
//    
//exit:
//    return result;
//}
//
//PlankFileRef pl_AudioFileWriter_GetFile (PlankAudioFileWriterRef p)
//{
//    return pl_IffFileWriter_GetFile (&p->iff); 
//}
//
//PlankResult pl_AudioFileWriter_Open (PlankAudioFileWriterRef p, const char* filepath)
//{
//	(void)p;
//	(void)filepath;
//    return PlankResult_OK;
//}
//
//PlankResult pl_AudioFileWriter_Close (PlankAudioFileWriterRef p)
//{
//	(void)p;
//    return PlankResult_OK;
//}
//

