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
    p->stream = 0;
    p->mode = 0;
    p->position = 0;
    p->path[0] = '\0';    
    
    // initialise to default functions
    pl_File_SetFunction (p, 0, 0, 0, 0, 0, 0, 0);
    
    return PlankResult_OK;
}

PlankResult pl_File_DeInit (PlankFileRef p)
{
    PlankResult result = PlankResult_OK;
    
    if (p->stream != PLANK_NULL)
        result = pl_File_Close (p);    

exit:
    return result;
}

PlankResult pl_File_Destroy (PlankFileRef p)
{
    PlankResult result;
    PlankMemoryRef m;
    
    result = PlankResult_OK;
    m = pl_MemoryGlobal();
    
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

PlankResult pl_File_SetFunction (PlankFileRef p, 
                                 PlankFileOpenFunction openFunction, 
                                 PlankFileCloseFunction closeFunction, 
                                 PlankFileGetStatusFunction statusFunction,
                                 PlankFileReadFunction readFunction,
                                 PlankFileWriteFunction writeFunction,
                                 PlankFileSetPositionFunction setPositionFunction,
                                 PlankFileGetPositionFunction getPositionFunction)
{
    if (openFunction != PLANK_NULL) p->openFunction = openFunction; 
    else p->openFunction = pl_FileDefaultOpenCallback;
    
    if (closeFunction != PLANK_NULL) p->closeFunction = closeFunction; 
    else p->closeFunction = pl_FileDefaultCloseCallback;
    
    if (statusFunction != PLANK_NULL) p->statusFunction = statusFunction; 
    else p->statusFunction = pl_FileDefaultGetStatusCallback;
    
    if (readFunction != PLANK_NULL) p->readFunction = readFunction; 
    else p->readFunction = pl_FileDefaultReadCallback;
    
    if (writeFunction != PLANK_NULL) p->writeFunction = writeFunction; 
    else p->writeFunction = pl_FileDefaultWriteCallback;
    
    if (setPositionFunction != PLANK_NULL) p->setPositionFunction = setPositionFunction; 
    else p->setPositionFunction = pl_FileDefaultSetPositionCallback;
    
    if (getPositionFunction != PLANK_NULL) p->getPositionFunction = getPositionFunction; 
    else p->getPositionFunction = pl_FileDefaultGetPositionCallback;
    
    return PlankResult_OK;
}

PlankResult pl_File_Open (PlankFileRef p, const char* filepath, const int mode)
{
    PlankResult result;
    
    if (p->stream != 0)
    {
        result = pl_File_Close (p);
        
        if (result != PlankResult_OK)
            return result;
    }
    
    if ((filepath == 0) || (filepath[0] == 0))
        return PlankResult_FilePathInvalid;
    
    strncpy (p->path, filepath, PLANKFILE_FILENAMEMAX);
    
    p->mode = mode & PLANKFILE_MASK;    // without the big endian flag
    result = (p->openFunction) (p);
    p->mode = mode & PLANKFILE_ALL;     // including big endian flag
    
    if (result != PlankResult_OK)
        return result;
    
    if (p->stream == 0)
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

PlankB pl_File_IsPositionable (PlankFileRef p)
{
    int isPositionable;
    (p->statusFunction) (p, PLANKFILE_STATUS_ISPOSITIONABLE, &isPositionable);
    return isPositionable ? PLANK_TRUE : PLANK_FALSE;
}

PlankResult pl_File_Close (PlankFileRef p)
{
    if (p->stream == 0)
        return PlankResult_FileInvalid;
    
    return (p->closeFunction) (p);
}

PlankResult pl_File_SetPosition (PlankFileRef p, const PlankLL position)
{
    int isPositionable;
    
    if (p->stream == 0)
        return PlankResult_FileInvalid;
    
    (p->statusFunction) (p, PLANKFILE_STATUS_ISPOSITIONABLE, &isPositionable);
    
    if (!isPositionable)
        return PlankResult_FileSeekFailed;
    
    return (p->setPositionFunction) (p, position, SEEK_SET);
}

PlankResult pl_File_OffsetPosition (PlankFileRef p, const PlankLL offset)
{
    int isPositionable;

    if (p->stream == 0)
        return PlankResult_FileInvalid;
    
    (p->statusFunction) (p, PLANKFILE_STATUS_ISPOSITIONABLE, &isPositionable);
    
    if (!isPositionable)
        return PlankResult_FileSeekFailed;
    
    return (p->setPositionFunction) (p, offset, SEEK_CUR);
}

PlankResult pl_File_ResetPosition (PlankFileRef p)
{
    return pl_File_SetPosition (p, 0);
}

PlankB pl_File_IsValid (PlankFileRef p)
{
    return p->stream != 0;
}

PlankB pl_File_IsEOF (PlankFileRef p)
{
    int status;
    
    if (p->stream == 0)
        return PLANK_FALSE;

    status = PLANK_FALSE;
    (p->statusFunction) (p, PLANKFILE_STATUS_EOF, &status);
    
    return status != 0;
}

PlankResult pl_File_SetEOF (PlankFileRef p)
{
    int isPositionable;

    if (p->stream == 0)
        return PlankResult_FileInvalid;
    
    (p->statusFunction) (p, PLANKFILE_STATUS_ISPOSITIONABLE, &isPositionable);
    
    if (!isPositionable)
        return PlankResult_FileSeekFailed;
    
    return (p->setPositionFunction) (p, 0L, SEEK_END);  
}

PlankResult pl_File_GetPosition (PlankFileRef p, PlankLL* position)
{
    if (p->stream == 0)
        return PlankResult_FileInvalid;
    
    return (p->getPositionFunction) (p, position);
}

PlankResult pl_File_SetMode (PlankFileRef p, const int mode)
{
    PlankResult result;
    
    if (p->stream == 0)
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

PlankResult pl_File_Read (PlankFileRef p, PlankP data, const int maximumBytes, int* bytesRead)
{
    if (p->stream == 0)
        return PlankResult_FileInvalid;
        
    if (! (p->mode & PLANKFILE_READ))
        return PlankResult_FileReadError;
        
    return (p->readFunction) (p, data, maximumBytes, bytesRead);    
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

/*
PlankResult pl_File_ReadL (PlankFileRef p, PlankL* data)
{
    PlankResult result;
    
    result = pl_File_Read (p, data, sizeof (PlankL), (int*)PLANK_NULL);
    
    if (! pl_File_IsNativeEndian (p))
        pl_SwapEndianL (data);
    
    return result;        
}
*/

PlankResult pl_File_ReadLL (PlankFileRef p, PlankLL* data)
{
    PlankResult result;
    
    result = pl_File_Read (p, data, sizeof (PlankLL), (int*)PLANK_NULL);
    
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
    PlankResult result;
    int i;
    char temp;
    
    text[0] = '\0';

    if (p->stream == 0)
        return PlankResult_FileInvalid;
    
    if (p->mode & PLANKFILE_BINARY)
        return PlankResult_FileReadError;
    
    if (! (p->mode & PLANKFILE_READ))
        return PlankResult_FileReadError;
    
    result = PlankResult_OK;
    
    do 
    {
        result = pl_File_ReadC (p, &temp);
    } while ((result == PlankResult_OK) && ((temp != '\r') || (temp != '\n')));

    if (result == PlankResult_OK)
        text[0] = temp;

    i = 1;
    
    while ((i < (maximumLength - 1)) && (result == PlankResult_OK))
    {
        result = pl_File_ReadC (p, &temp);
        
        if ((result != PlankResult_OK) || (temp == '\r') || (temp == '\n') || (temp == '\0'))
        {
            text[i] = '\0';
            break;
        }
        
        text[i] = temp;
        i++;
    }
    
    return result;
}

PlankResult pl_File_Write (PlankFileRef p, const void* data, const int maximumBytes)
{    
    if (p->stream == 0)
        return PlankResult_FileInvalid;
    
    if (! (p->mode & PLANKFILE_WRITE))
        return PlankResult_FileWriteError;
    
    return (p->writeFunction) (p, data, maximumBytes);
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

/*
PlankResult pl_File_WriteL (PlankFileRef p, PlankL data)
{
    PlankResult result;
    
    if (! pl_File_IsNativeEndian (p))
        pl_SwapEndianL (&data);
    
    result = pl_File_Write (p, &data, sizeof (PlankL));
    
    return result;            
}
*/

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
    if (p->stream == 0)
        return PlankResult_FileInvalid;
    
    if (p->mode & PLANKFILE_BINARY)
        return PlankResult_FileWriteError;
    
    if (! (p->mode & PLANKFILE_WRITE))
        return PlankResult_FileWriteError;
    
    return (p->writeFunction) (p, text, strlen (text));
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
        if ((result = pl_File_Write (p, &string->data[1], length)) != PlankResult_OK) goto exit;
    
exit:
    return result;        
}

// callbacks

PlankResult pl_FileDefaultOpenCallback (PlankFileRef p)
{    
    if (p->mode == PLANKFILE_READ)
        p->stream = fopen (p->path, "r");
    else if (p->mode == (PLANKFILE_WRITE | PLANKFILE_NEW))
        p->stream = fopen (p->path, "w");
    else if (p->mode == (PLANKFILE_WRITE | PLANKFILE_APPEND))
        p->stream = fopen (p->path, "a");
    else if (p->mode == (PLANKFILE_READ | PLANKFILE_WRITE))
        p->stream = fopen (p->path, "r+");
    else if (p->mode == (PLANKFILE_READ | PLANKFILE_WRITE | PLANKFILE_NEW))
        p->stream = fopen (p->path, "w+");
    else if (p->mode == (PLANKFILE_READ | PLANKFILE_WRITE | PLANKFILE_APPEND))
        p->stream = fopen (p->path, "a+");
    else if (p->mode == (PLANKFILE_BINARY | PLANKFILE_READ))
        p->stream = fopen (p->path, "rb");
    else if (p->mode == (PLANKFILE_BINARY | PLANKFILE_WRITE | PLANKFILE_NEW))
        p->stream = fopen (p->path, "wb");
    else if (p->mode == (PLANKFILE_BINARY | PLANKFILE_WRITE | PLANKFILE_APPEND))
        p->stream = fopen (p->path, "ab");
    else if (p->mode == (PLANKFILE_BINARY | PLANKFILE_READ | PLANKFILE_WRITE))
        p->stream = fopen (p->path, "rb+");
    else if (p->mode == (PLANKFILE_BINARY | PLANKFILE_READ | PLANKFILE_WRITE | PLANKFILE_NEW))
        p->stream = fopen (p->path, "wb+");
    else if (p->mode == (PLANKFILE_BINARY | PLANKFILE_READ | PLANKFILE_WRITE | PLANKFILE_APPEND))
        p->stream = fopen (p->path, "ab+");        
    else 
        return PlankResult_FileModeInvalid;
    
    return PlankResult_OK;
}

PlankResult pl_FileDefaultCloseCallback (PlankFileRef p)
{
    int err;
    
    err = fclose ((FILE*)p->stream);
    p->stream = 0;
    
    if (err != 0)
        return PlankResult_FileCloseFailed;
    
    return PlankResult_OK;
}

PlankResult pl_FileDefaultGetStatusCallback (PlankFileRef p, int type, int* status)
{
    switch (type) 
    {
        case PLANKFILE_STATUS_EOF:              *status = feof ((FILE*)p->stream); break;
        case PLANKFILE_STATUS_ISPOSITIONABLE:   *status = PLANK_TRUE; break;
            
        default: return PlankResult_UnknownError;
    }
    
    return PlankResult_OK;
}

PlankResult pl_FileDefaultReadCallback (PlankFileRef p, PlankP ptr, int maximumBytes, int* bytesRead)
{
    int size, err;
    size = fread (ptr, 1, (size_t)maximumBytes, (FILE*)p->stream);
    
    if (bytesRead != PLANK_NULL)
        *bytesRead = size;
        
    if (size != maximumBytes)
    {
        err = ferror ((FILE*)p->stream);
        
        if (err != 0)
            return PlankResult_FileReadError;
        
        err = feof ((FILE*)p->stream);
        
        if (err != 0)
            return PlankResult_FileEOF;
        
        return PlankResult_UnknownError;
    }
    
    return PlankResult_OK;
}

PlankResult pl_FileDefaultWriteCallback (PlankFileRef p, const void* data, const int maximumBytes)
{
    int size;
    
    size = (int)fwrite (data, 1, maximumBytes, p->stream);
    
    if (size != maximumBytes)
        return PlankResult_FileWriteError;
    
    return PlankResult_OK;
}

PlankResult pl_FileDefaultSetPositionCallback (PlankFileRef p, PlankLL offset, int code)
{
#if PLANK_WIN
    int err;

    err = _fseeki64 ((FILE*)p->stream, temp, code);
    
    if (err != 0)
        return PlankResult_FileSeekFailed;
    
    return PlankResult_OK;
#else
    int err;
    off_t temp;

    temp = offset;
    err = fseeko ((FILE*)p->stream, temp, code);
    
    if (err != 0)
        return PlankResult_FileSeekFailed;

    return PlankResult_OK;
#endif
}

PlankResult pl_FileDefaultGetPositionCallback (PlankFileRef p, PlankLL* position)
{
    int err;
    fpos_t temp;
    err = fgetpos ((FILE*)p->stream, &temp);
    
    if (err != 0)
        return PlankResult_FileReadError;
        
    p->position = temp;
    
    if (position != 0)
        *position = temp;

    return PlankResult_OK;    
}

