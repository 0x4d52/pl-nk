/*
 -------------------------------------------------------------------------------
 This file is part of the Plink, Plonk, Plank libraries
  by Martin Robinson
 
 http://code.google.com/p/pl-nk/
 
 Copyright University of the West of England, Bristol 2011-12
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
#include "plank_File.h"


PlankResult pl_FileErase (const char* filepath)
{
    int err;
    
    err = remove (filepath);
    
    if (err != 0)
        return PlankResult_FileRemoveFailed;
    
    return PlankResult_OK;
}

PlankFileRef pl_File_CreateAndInit()
{
    PlankFileRef p;
    p = pl_File_Create();
    
    if (p != PLANK_NULL)
    {
        if (pl_File_Init (p) != PlankResult_OK)
            pl_File_Destroy (p);
        else
            return p;
    }
    
    return PLANK_NULL;
}

PlankFileRef pl_File_Create()
{
    PlankMemoryRef m;
    PlankFileRef p;
    
    m = pl_MemoryGlobal();
    p = (PlankFileRef)pl_Memory_AllocateBytes (m, sizeof (PlankFile));
    
    if (p != PLANK_NULL)
        pl_MemoryZero (p, sizeof (PlankFile));
    
    return p;
}

PlankResult pl_File_Init (PlankFileRef p)
{        
    p->file = 0;
    p->mode = 0;
    p->position = 0;
    p->path[0] = '\0';    
    
    return PlankResult_OK;
}

PlankResult pl_File_DeInit (PlankFileRef p)
{
    PlankResult result = PlankResult_OK;
    
    if (p->file == PLANK_NULL)
    {
        result = PlankResult_MemoryError;
        goto exit;
    }
    
    result = pl_File_Close (p);

exit:
    return result;
}

PlankResult pl_File_Destroy (PlankFileRef p)
{
    PlankResult result = PlankResult_OK;
    PlankMemoryRef m = pl_MemoryGlobal();
    
    if (p == PLANK_NULL)
    {
        result = PlankResult_MemoryError;
        goto exit;
    }
    
    if ((result = pl_File_DeInit (p)) != PlankResult_OK) 
        goto exit;
    
    result = pl_Memory_Free (m, p);
            
exit:
    return result;       
}

PlankResult pl_File_Open (PlankFileRef p, const char* filepath, const int mode)
{
    PlankResult result;
    
    if (p->file != 0)
    {
        result = pl_File_Close (p);
        
        if (result != PlankResult_OK)
            return result;
    }
    
    if ((filepath == 0) || (filepath[0] == 0))
        return PlankResult_FilePathInvalid;
    
    strncpy (p->path, filepath, PLANKFILE_FILENAMEMAX);
    
    p->mode = mode & PLANKFILE_MASK; // without the big endian flag
    
    result = PlankResult_OK;
    
    if (p->mode == PLANKFILE_READ)
        p->file = fopen (filepath, "r");
    else if (p->mode == (PLANKFILE_WRITE | PLANKFILE_NEW))
        p->file = fopen (filepath, "w");
    else if (p->mode == (PLANKFILE_WRITE | PLANKFILE_APPEND))
        p->file = fopen (filepath, "a");
    else if (p->mode == (PLANKFILE_READ | PLANKFILE_WRITE))
        p->file = fopen (filepath, "r+");
    else if (p->mode == (PLANKFILE_READ | PLANKFILE_WRITE | PLANKFILE_NEW))
        p->file = fopen (filepath, "w+");
    else if (p->mode == (PLANKFILE_READ | PLANKFILE_WRITE | PLANKFILE_APPEND))
        p->file = fopen (filepath, "a+");
    else if (p->mode == (PLANKFILE_BINARY | PLANKFILE_READ))
        p->file = fopen (filepath, "rb");
    else if (p->mode == (PLANKFILE_BINARY | PLANKFILE_WRITE | PLANKFILE_NEW))
        p->file = fopen (filepath, "wb");
    else if (p->mode == (PLANKFILE_BINARY | PLANKFILE_WRITE | PLANKFILE_APPEND))
        p->file = fopen (filepath, "ab");
    else if (p->mode == (PLANKFILE_BINARY | PLANKFILE_READ | PLANKFILE_WRITE))
        p->file = fopen (filepath, "rb+");
    else if (p->mode == (PLANKFILE_BINARY | PLANKFILE_READ | PLANKFILE_WRITE | PLANKFILE_NEW))
        p->file = fopen (filepath, "wb+");
    else if (p->mode == (PLANKFILE_BINARY | PLANKFILE_READ | PLANKFILE_WRITE | PLANKFILE_APPEND))
        p->file = fopen (filepath, "ab+");        
    else 
        result = PlankResult_FileModeInvalid;
    
    p->mode = mode & PLANKFILE_ALL; // including big endian flag
    
    if (result != PlankResult_OK)
        return result;
    
    if (p->file == 0)
        return PlankResult_FileOpenFailed;
    
    result = pl_File_SetPosition (p, p->position);
    
    if (result != PlankResult_OK)
        return result;
        
    return PlankResult_OK;
}

PlankResult pl_File_OpenTextRead (PlankFileRef p, const char* filepath, const PlankB andWrite)
{
    int mode = PLANKFILE_READ;
    
    if (andWrite)
        mode |= (PLANKFILE_WRITE | PLANKFILE_APPEND);
    
    return pl_File_Open (p, filepath, mode);
}

PlankResult pl_File_OpenTextWrite (PlankFileRef p, const char* filepath, const PlankB andRead, const PlankB replaceExistingFile)
{
    int mode = PLANKFILE_WRITE;
    
    if (andRead)
        mode |= PLANKFILE_READ;
    
    if (replaceExistingFile)
        mode |= PLANKFILE_NEW;
    else
        mode |= PLANKFILE_APPEND;
    
    return pl_File_Open (p, filepath, mode);
}

PlankResult pl_File_OpenBinaryRead (PlankFileRef p, const char* filepath, const PlankB andWrite, const PlankB isBigEndian)
{
    int mode = PLANKFILE_BINARY | PLANKFILE_READ;
    
    if (andWrite)
        mode |= (PLANKFILE_WRITE | PLANKFILE_APPEND);
    
    if (isBigEndian)
        mode |= PLANKFILE_BIGENDIAN;
    
    return pl_File_Open (p, filepath, mode);    
}

PlankResult pl_File_OpenBinaryWrite (PlankFileRef p, const char* filepath, const PlankB andRead, const PlankB replaceExistingFile, const PlankB isBigEndian)
{
    int mode = PLANKFILE_BINARY | PLANKFILE_WRITE;
    
    if (andRead)
        mode |= PLANKFILE_READ;
    
    if (replaceExistingFile)
        mode |= PLANKFILE_NEW;
    else
        mode |= PLANKFILE_APPEND;
    
    if (isBigEndian)
        mode |= PLANKFILE_BIGENDIAN;
    
    return pl_File_Open (p, filepath, mode);    
}

PlankResult pl_File_OpenBinaryNativeEndianRead (PlankFileRef p, const char* filepath, const PlankB andWrite)
{
    int mode = PLANKFILE_BINARY | PLANKFILE_READ;
    
    if (andWrite)
        mode |= (PLANKFILE_WRITE | PLANKFILE_APPEND);
    
#if PLANK_BIGENDIAN
    mode |= PLANKFILE_BIGENDIAN;
#endif
    
    return pl_File_Open (p, filepath, mode);        
}

PlankResult pl_File_OpenBinaryNativeEndianWrite (PlankFileRef p, const char* filepath, const PlankB andRead, const PlankB replaceExistingFile)
{
    int mode = PLANKFILE_BINARY | PLANKFILE_WRITE;
    
    if (andRead)
        mode |= PLANKFILE_READ;
    
    if (replaceExistingFile)
        mode |= PLANKFILE_NEW;
    else
        mode |= PLANKFILE_APPEND;
    
#if PLANK_BIGENDIAN
    mode |= PLANKFILE_BIGENDIAN;
#endif
    
    return pl_File_Open (p, filepath, mode);        
}

PlankB pl_File_IsBigEndian (PlankFileRef p)
{
    return !! (p->mode & PLANKFILE_BIGENDIAN);
}

PlankB pl_File_IsLittleEndian (PlankFileRef p)
{
    return ! (p->mode & PLANKFILE_BIGENDIAN);
}

PlankB pl_File_IsNativeEndian (PlankFileRef p)
{
#if PLANK_BIGENDIAN
    return !! (p->mode & PLANKFILE_BIGENDIAN);
#elif PLANK_LITTLEENDIAN
    return ! (p->mode & PLANKFILE_BIGENDIAN);
#else
    #error Neither PLANK_BIGENDIAN or PLANK_LITTLEENDIAN are set to 1
#endif
}

PlankResult pl_File_Close (PlankFileRef p)
{
    PlankResult result;
    int err;

    if (p->file == 0)
        return PlankResult_FileInvalid;
    
    result = pl_File_GetPosition (p, 0);
    
    if (result != PlankResult_OK)
        return result;
    
    err = fclose (p->file);
    p->file = 0;

    if (err != 0)
        return PlankResult_FileCloseFailed;
    
    return PlankResult_OK;
}

PlankResult pl_File_SetPosition (PlankFileRef p, const PlankLL position)
{
    int err;
    fpos_t temp;

    if (p->file == 0)
        return PlankResult_FileInvalid;

    temp = position;
    err = fsetpos (p->file, &temp);
    
    if (err != 0)
        return PlankResult_FileSeekFailed;
    
    p->position = position;
    
    return PlankResult_OK;
}

PlankResult pl_File_OffsetPosition (PlankFileRef p, const PlankLL offset)
{
    PlankResult result;

    result = pl_File_GetPosition (p, 0);
    
    if (result != PlankResult_OK)
        return result;
    
    return pl_File_SetPosition (p, p->position + offset);
}

PlankResult pl_File_ResetPosition (PlankFileRef p)
{
    return pl_File_SetPosition (p, 0);
}

PlankB pl_File_IsValid (PlankFileRef p)
{
    return p->file != 0;
}

PlankB pl_File_IsEOF (PlankFileRef p)
{
    if (p->file == 0)
        return PLANK_FALSE;

    return feof (p->file);
}

PlankResult pl_File_SetEOF (PlankFileRef p)
{
    int err;
    
    if (p->file == 0)
        return PlankResult_FileInvalid;
    
    err = fseek (p->file, 0L, SEEK_END);
    
    if (err != 0)
        return PlankResult_FileSeekFailed;
        
    return pl_File_GetPosition (p, 0);    
}

PlankResult pl_File_GetPosition (PlankFileRef p, PlankLL* position)
{
    int err;
    fpos_t temp;

    if (p->file == 0)
        return PlankResult_FileInvalid;
    
    err = fgetpos (p->file, &temp);
    
    if (err != 0)
        return PlankResult_FileReadError;
    
    p->position = temp;
    
    if (position != 0)
        *position = temp;
    
    return PlankResult_OK;
}

PlankResult pl_File_SetMode (PlankFileRef p, const int mode)
{
    PlankResult result;
    
    if (p->file == 0)
        return PlankResult_FileInvalid;

    if (p->mode == mode)
        return PlankResult_OK;
    
    result = pl_File_Close (p);
    
    if (result != PlankResult_OK)
        return result;
    
    result = pl_File_Open (p, p->path, mode);
    
    if (result != PlankResult_OK)
        return result;

    return PlankResult_OK;
}

PlankResult pl_File_SetEndian (PlankFileRef p, const PlankB isBigEndian)
{
    if (isBigEndian)
        p->mode |= PLANKFILE_BIGENDIAN;  
    else    
        p->mode &= ~PLANKFILE_BIGENDIAN;
        
    return PlankResult_OK;
}

PlankResult pl_File_Read (PlankFileRef p, void* data, const int maximumBytes, int* bytesRead)
{
    int size, err;
        
    if (p->file == 0)
        return PlankResult_FileInvalid;
        
    if (! (p->mode & PLANKFILE_READ))
        return PlankResult_FileReadError;
    
    size = (int)fread (data, 1, maximumBytes, p->file);
    
    if (bytesRead != PLANK_NULL)
        *bytesRead = size;
    
    if (size != maximumBytes)
    {
        err = ferror (p->file);
        
        if (err != 0)
            return PlankResult_FileReadError;
        
        err = feof (p->file);
        
        if (err != 0)
            return PlankResult_FileEOF;
        
        return PlankResult_UnknownError;
    }
        
    return PlankResult_OK;    
}

PlankResult pl_File_ReadC (PlankFileRef p, char* data)
{
    PlankResult result;
    result = pl_File_Read (p, data, sizeof (char), PLANK_NULL);
    return result;
}

PlankResult pl_File_ReadS (PlankFileRef p, short* data)
{
    PlankResult result;
    
    result = pl_File_Read (p, data, sizeof (short), PLANK_NULL);
    
    if (! pl_File_IsNativeEndian (p))
        pl_SwapEndianS (data);
    
    return result;    
}

PlankResult pl_File_ReadI (PlankFileRef p, int* data)
{
    PlankResult result;
    
    result = pl_File_Read (p, data, sizeof (int), PLANK_NULL);
    
    if (! pl_File_IsNativeEndian (p))
        pl_SwapEndianI (data);
    
    return result;        
}

PlankResult pl_File_ReadI24 (PlankFileRef p, PlankI24* data)
{
    PlankResult result;
    
    result = pl_File_Read (p, data, sizeof (PlankI24), PLANK_NULL);
    
    if (! pl_File_IsNativeEndian (p))
        pl_SwapEndianI24 (data);
    
    return result;            
}

PlankResult pl_File_ReadL (PlankFileRef p, PlankL* data)
{
    PlankResult result;
    
    result = pl_File_Read (p, data, sizeof (PlankL), PLANK_NULL);
    
    if (! pl_File_IsNativeEndian (p))
        pl_SwapEndianL (data);
    
    return result;        
}

PlankResult pl_File_ReadLL (PlankFileRef p, PlankLL* data)
{
    PlankResult result;
    
    result = pl_File_Read (p, data, sizeof (PlankLL), PLANK_NULL);
    
    if (! pl_File_IsNativeEndian (p))
        pl_SwapEndianLL (data);
    
    return result;            
}

PlankResult pl_File_ReadUC (PlankFileRef p, unsigned char* data)
{
    PlankResult result;
    result = pl_File_Read (p, data, sizeof (unsigned char), PLANK_NULL);
    return result;    
}

PlankResult pl_File_ReadUS (PlankFileRef p, unsigned short* data)
{
    PlankResult result;
    
    result = pl_File_Read (p, data, sizeof (unsigned short), PLANK_NULL);
    
    if (! pl_File_IsNativeEndian (p))
        pl_SwapEndianUS (data);
    
    return result;        
}

PlankResult pl_File_ReadUI (PlankFileRef p, unsigned int* data)
{
    PlankResult result;
    
    result = pl_File_Read (p, data, sizeof (unsigned int), PLANK_NULL);
    
    if (! pl_File_IsNativeEndian (p))
        pl_SwapEndianUI (data);
    
    return result;            
}

PlankResult pl_File_ReadUI24 (PlankFileRef p, PlankUI24* data)
{
    PlankResult result;
    
    result = pl_File_Read (p, data, sizeof (PlankUI24), PLANK_NULL);
    
    if (! pl_File_IsNativeEndian (p))
        pl_SwapEndianUI24 (data);
    
    return result;                
}

PlankResult pl_File_ReadUL (PlankFileRef p, PlankUL* data)
{
    PlankResult result;
    
    result = pl_File_Read (p, data, sizeof (PlankUL), PLANK_NULL);
    
    if (! pl_File_IsNativeEndian (p))
        pl_SwapEndianUL (data);
    
    return result;                
}

PlankResult pl_File_ReadULL (PlankFileRef p, PlankULL* data)
{
    PlankResult result;
    
    result = pl_File_Read (p, data, sizeof (PlankULL), PLANK_NULL);
    
    if (! pl_File_IsNativeEndian (p))
        pl_SwapEndianULL (data);
    
    return result;                
}

PlankResult pl_File_ReadF (PlankFileRef p, float* data)
{
    PlankResult result;
    
    result = pl_File_Read (p, data, sizeof (float), PLANK_NULL);
    
    if (! pl_File_IsNativeEndian (p))
        pl_SwapEndianF (data);
    
    return result;                
}

PlankResult pl_File_ReadD (PlankFileRef p, double* data)
{
    PlankResult result;
    
    result = pl_File_Read (p, data, sizeof (double), PLANK_NULL);
    
    if (! pl_File_IsNativeEndian (p))
        pl_SwapEndianD (data);
    
    return result;                
}

PlankResult pl_File_ReadFourCharCode (PlankFileRef p, PlankFourCharCode* data)
{
    PlankResult result;
    
    result = pl_File_Read (p, data, sizeof (PlankFourCharCode), PLANK_NULL);
    
#if PLANK_BIGENDIAN
    pl_SwapEndianI ((PlankI) data);
#endif
    
    return result;    
}

PlankResult pl_File_ReadPascalString255 (PlankFileRef p, PlankPascalString255* string)
{
    PlankResult result = PlankResult_OK;
    PlankUC length;
    
    if ((result = pl_File_ReadUC (p, &length)) != PlankResult_OK) goto exit;
    
    string->data[0] = length;
    
    if ((result = pl_File_Read (p, &string->data[1], length, PLANK_NULL)) != PlankResult_OK) goto exit;
   
exit:
    return result;        
}

PlankResult pl_File_ReadLine (PlankFileRef p, char* text, const int maximumLength)
{
    int err, i;
    char *temp;
    
    if (p->file == 0)
        return PlankResult_FileInvalid;
    
    if (p->mode & PLANKFILE_BINARY)
        return PlankResult_FileReadError;
    
    if (! (p->mode & PLANKFILE_READ))
        return PlankResult_FileReadError;
    
    temp = fgets (text, maximumLength, p->file);
    
    if (temp == 0)
    {
        err = ferror (p->file);
        
        if (err != 0)
            return PlankResult_FileReadError;
        
        err = feof (p->file);
        
        if (err != 0)
            return PlankResult_FileEOF;
        
        return PlankResult_UnknownError;
    }
    
    // strip out new line characters
    for (i = 0; i < maximumLength; i++)
        if ((text[i] == '\r') || (text[i] == '\n'))
            text[i] = '\0';
    
    return PlankResult_OK;
}

PlankResult pl_File_Write (PlankFileRef p, const void* data, const int maximumBytes)
{
    int size;
    
    if (p->file == 0)
        return PlankResult_FileInvalid;
    
    if (! (p->mode & PLANKFILE_WRITE))
        return PlankResult_FileWriteError;
    
    size = (int)fwrite (data, 1, maximumBytes, p->file);

    if (size != maximumBytes)
        return PlankResult_FileWriteError;

    return PlankResult_OK;
}

PlankResult pl_File_WriteC (PlankFileRef p, char data)
{
    PlankResult result;
    result = pl_File_Write (p, &data, sizeof (char));
    return result;    
}

PlankResult pl_File_WriteS (PlankFileRef p, short data)
{
    PlankResult result;
    
    if (! pl_File_IsNativeEndian (p))
        pl_SwapEndianS (&data);
    
    result = pl_File_Write (p, &data, sizeof (short));
    
    return result;        
}

PlankResult pl_File_WriteI (PlankFileRef p, int data)
{
    PlankResult result;
    
    if (! pl_File_IsNativeEndian (p))
        pl_SwapEndianI (&data);

    result = pl_File_Write (p, &data, sizeof (int));
    
    return result;        
}

PlankResult pl_File_WriteI24 (PlankFileRef p, PlankI24 data)
{
    PlankResult result;
    
    if (! pl_File_IsNativeEndian (p))
        pl_SwapEndianI24 (&data);
    
    result = pl_File_Write (p, &data, sizeof (PlankI24));
    
    return result;            
}

PlankResult pl_File_WriteL (PlankFileRef p, PlankL data)
{
    PlankResult result;
    
    if (! pl_File_IsNativeEndian (p))
        pl_SwapEndianL (&data);
    
    result = pl_File_Write (p, &data, sizeof (PlankL));
    
    return result;            
}

PlankResult pl_File_WriteLL (PlankFileRef p, PlankLL data)
{
    PlankResult result;
    
    if (! pl_File_IsNativeEndian (p))
        pl_SwapEndianLL (&data);

    result = pl_File_Write (p, &data, sizeof (PlankLL));
    
    return result;        
}

PlankResult pl_File_WriteUC (PlankFileRef p, unsigned char data)
{
    PlankResult result;
    result = pl_File_Write (p, &data, sizeof (unsigned char));
    return result;        
}

PlankResult pl_File_WriteUS (PlankFileRef p, unsigned short data)
{
    PlankResult result;
    
    if (! pl_File_IsNativeEndian (p))
        pl_SwapEndianUS (&data);
    
    result = pl_File_Write (p, &data, sizeof (unsigned short));
    
    return result;            
}

PlankResult pl_File_WriteUI (PlankFileRef p, unsigned int data)
{
    PlankResult result;
    
    if (! pl_File_IsNativeEndian (p))
        pl_SwapEndianUI (&data);
    
    result = pl_File_Write (p, &data, sizeof (unsigned int));
    
    return result;                
}

PlankResult pl_File_WriteUI24 (PlankFileRef p, PlankUI24 data)
{
    PlankResult result;
    
    if (! pl_File_IsNativeEndian (p))
        pl_SwapEndianUI24 (&data);
    
    result = pl_File_Write (p, &data, sizeof (PlankUI24));
    
    return result;                    
}

PlankResult pl_File_WriteUL (PlankFileRef p, PlankUL data)
{
    PlankResult result;
    
    if (! pl_File_IsNativeEndian (p))
        pl_SwapEndianUL (&data);
    
    result = pl_File_Write (p, &data, sizeof (PlankUL));
    
    return result;                
}

PlankResult pl_File_WriteULL (PlankFileRef p, PlankULL data)
{
    PlankResult result;
    
    if (! pl_File_IsNativeEndian (p))
        pl_SwapEndianULL (&data);
    
    result = pl_File_Write (p, &data, sizeof (PlankULL));
    
    return result;                
}

PlankResult pl_File_WriteF (PlankFileRef p, float data)
{
    PlankResult result;
    
    if (! pl_File_IsNativeEndian (p))
        pl_SwapEndianF (&data);
    
    result = pl_File_Write (p, &data, sizeof (float));
    
    return result;        
}

PlankResult pl_File_WriteD (PlankFileRef p, double data)
{
    PlankResult result;
    
    if (! pl_File_IsNativeEndian (p))
        pl_SwapEndianD (&data);
    
    result = pl_File_Write (p, &data, sizeof (double));
    
    return result;        
}

PlankResult pl_File_WriteFourCharCode (PlankFileRef p, PlankFourCharCode data)
{
    return pl_File_WriteI (p, (PlankI)data);
}

PlankResult pl_File_WriteString (PlankFileRef p, const char* text)
{
    int err;
    
    if (p->file == 0)
        return PlankResult_FileInvalid;
    
    if (p->mode & PLANKFILE_BINARY)
        return PlankResult_FileWriteError;
    
    if (! (p->mode & PLANKFILE_WRITE))
        return PlankResult_FileWriteError;
    
    err = fputs (text, p->file);
    
    if (err < 0)
        return PlankResult_FileWriteError;
    
    return PlankResult_OK;        
}

PlankResult pl_File_WriteLine (PlankFileRef p, const char* text)
{
    PlankResult result;
    
    result = pl_File_WriteString (p, text);
    
    if (result != PlankResult_OK)
        return result;
    
    result = pl_File_WriteString (p, "\r\n");
    
    if (result != PlankResult_OK)
        return result;
    
    return PlankResult_OK;
}

PlankResult pl_File_WritePascalString255 (PlankFileRef p, PlankPascalString255* string)
{
    PlankResult result = PlankResult_OK;
    PlankUC length;
    
    length = string->data[0];
    
    if ((result = pl_File_WriteUC (p, length)) != PlankResult_OK) goto exit;
    
    if (length > 0)
    {
        if ((result = pl_File_Write (p, &string->data[1], length)) != PlankResult_OK) goto exit;
    }
    
exit:
    return result;        
}

