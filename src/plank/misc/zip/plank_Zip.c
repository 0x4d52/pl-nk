/*
 -------------------------------------------------------------------------------
 This file is part of the Plink, Plonk, Plank libraries
  by Martin Robinson
 
 http://code.google.com/p/pl-nk/
 
 Copyright University of the West of England, Bristol 2011-14
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
#include "plank_Zip.h"
#include "../../maths/plank_Maths.h"
#include "../../../../ext/zlib/zlib.h"


#include "../../../../ext/zlib/compress.c"
#include "../../../../ext/zlib/inffast.c"
#include "../../../../ext/zlib/adler32.c"
#undef DO1
#undef DO8
#include "../../../../ext/zlib/crc32.c"
#include "../../../../ext/zlib/deflate.c"
#include "../../../../ext/zlib/infback.c"
#undef LOAD
#undef RESTORE
#undef INITBITS
#undef PULLBYTE
#undef NEEDBITS
#undef DROPBITS
#undef BYTEBITS
#include "../../../../ext/zlib/inflate.c"
#include "../../../../ext/zlib/inftrees.c"
#include "../../../../ext/zlib/trees.c"
#include "../../../../ext/zlib/uncompr.c"
#include "../../../../ext/zlib/zutil.c"

#define PLANKZIP_CHUNK 16384

PlankResult pl_Zip_Init (PlankZipRef p)
{
    if (p == PLANK_NULL)
        return PlankResult_MemoryError;
    
    return pl_DynamicArray_InitWithItemSize (&p->buffer, 1);
}

PlankResult pl_Zip_DeInit (PlankZipRef p)
{
    if (p == PLANK_NULL)
        return PlankResult_MemoryError;
    
    return pl_DynamicArray_DeInit (&p->buffer);
}

PlankResult pl_Zip_DeflateStream (PlankZipRef p, const int amount,  PlankFileRef outputZipFile, PlankFileRef inputFile)
{
    PlankResult result;
    int ret, flush, outputMode, inputMode, bytesRead;
    unsigned have;
    z_stream strm;
    unsigned char in[PLANKZIP_CHUNK];
    unsigned char out[PLANKZIP_CHUNK];

	(void)p;

    result = PlankResult_OK;
    if ((result = pl_File_GetMode (inputFile, &inputMode)) != PlankResult_OK) goto earlyExit;
    if ((result = pl_File_GetMode (outputZipFile, &outputMode)) != PlankResult_OK) goto earlyExit;
    
    if (!((outputMode & PLANKFILE_WRITE) && (outputMode & PLANKFILE_BINARY)))
    {
        result = PlankResult_FileWriteError;
        goto earlyExit;
    }
    
    if (!(inputMode & PLANKFILE_READ))
    {
        result = PlankResult_FileReadError;
        goto earlyExit;
    }

    /* allocate deflate state */
    strm.zalloc = Z_NULL;
    strm.zfree = Z_NULL;
    strm.opaque = Z_NULL;
        
    if ((ret = deflateInit (&strm, pl_ClipI (amount, 0, 9))) != Z_OK)
    {
        result = PlankResult_ZipError;
        goto earlyExit;
    }

    do
    {
        result = pl_File_Read (inputFile, in, PLANKZIP_CHUNK, &bytesRead);
        strm.avail_in = bytesRead;
        
        if ((result != PlankResult_OK) && (result != PlankResult_FileEOF)) goto exit;
       
        flush = pl_File_IsEOF (inputFile) ? Z_FINISH : Z_NO_FLUSH;
        strm.next_in = in;

        do {
            strm.avail_out = PLANKZIP_CHUNK;
            strm.next_out = out;
            ret = deflate (&strm, flush);
            
            if (ret == Z_STREAM_ERROR)
            {
                result = PlankResult_ZipError;
                goto exit;
            }
            
            have = PLANKZIP_CHUNK - strm.avail_out;
            
            if ((result = pl_File_Write (outputZipFile, out, have)) != PlankResult_OK) goto exit;
        } while (strm.avail_out == 0);
        
        if (strm.avail_in != 0)
        {
            result = PlankResult_ZipError;
            goto exit;
        }
        
    } while (flush != Z_FINISH);
    
    if (ret != Z_STREAM_END)
    {
        result = PlankResult_ZipError;
        goto exit;
    }
        
exit:
    (void)deflateEnd (&strm);

earlyExit:
    return result;
}

PlankResult pl_Zip_InflateStream (PlankZipRef p, PlankFileRef outputFile, PlankFileRef inputZipFile)
{
    PlankResult result;
    int ret, outputMode, inputMode, bytesRead;
    unsigned have;
    z_stream strm;
    unsigned char in[PLANKZIP_CHUNK];
    unsigned char out[PLANKZIP_CHUNK];

	(void)p;

    result = PlankResult_OK;
    if ((result = pl_File_GetMode (inputZipFile, &inputMode)) != PlankResult_OK) goto earlyExit;
    if ((result = pl_File_GetMode (outputFile, &outputMode)) != PlankResult_OK) goto earlyExit;
    
    if (!(outputMode & PLANKFILE_WRITE))
    {
        result = PlankResult_FileWriteError;
        goto earlyExit;
    }
    
    if (!((inputMode & PLANKFILE_READ) && (inputMode & PLANKFILE_BINARY)))
    {
        result = PlankResult_FileReadError;
        goto earlyExit;
    }
    
    /* allocate inflate state */
    strm.zalloc     = Z_NULL;
    strm.zfree      = Z_NULL;
    strm.opaque     = Z_NULL;
    strm.avail_in   = 0;
    strm.next_in    = Z_NULL;
        
    if ((ret = inflateInit (&strm)) != Z_OK)
    {
        result = PlankResult_ZipError;
        goto earlyExit;
    }

    do {
        result = pl_File_Read (inputZipFile, in, PLANKZIP_CHUNK, &bytesRead);
        strm.avail_in = bytesRead;
        
        if ((result != PlankResult_OK) && (result != PlankResult_FileEOF)) goto exit;
        
        if (strm.avail_in == 0)
            break;
        
        strm.next_in = in;

        do {
            strm.avail_out = PLANKZIP_CHUNK;
            strm.next_out = out;
            ret = inflate (&strm, Z_NO_FLUSH);
            
            if (ret == Z_STREAM_ERROR)
            {
                result = PlankResult_ZipError;
                goto exit;
            }
            
            switch (ret)
            {
                case Z_NEED_DICT:
                case Z_DATA_ERROR:
                case Z_MEM_ERROR:
                    result = PlankResult_ZipError;
                    goto exit;
            }
            
            have = PLANKZIP_CHUNK - strm.avail_out;
            
            if ((result = pl_File_Write (outputFile, out, have)) != PlankResult_OK) goto exit;            
        } while (strm.avail_out == 0);
    } while (ret != Z_STREAM_END);
    
exit:
    (void)inflateEnd (&strm);
    
earlyExit:
    return result;
}

const void* pl_Zip_DeflateData (PlankZipRef p, const int amount, const void* dataInput, const PlankL dataInputLength, PlankL* dataOutputLength)
{
    PlankResult result;
    PlankFile inputStream;
    PlankFile outputStream;
    const void* dataOutput;
    int mode;
    
    result = PlankResult_OK;
    dataOutput = (const void*)PLANK_NULL;
    *dataOutputLength = 0;
    
    if ((result = pl_DynamicArray_SetSize (&p->buffer, 0)) != PlankResult_OK) goto exit;
    if ((result = pl_File_Init (&inputStream)) != PlankResult_OK) goto exit;
    if ((result = pl_File_Init (&outputStream)) != PlankResult_OK) goto exit;
    
    mode = PLANKFILE_BINARY | PLANKFILE_READ | PLANKFILE_NATIVEENDIAN;
    if ((result = pl_File_OpenMemory (&inputStream, (void*)dataInput, dataInputLength, mode)) != PlankResult_OK) goto exit;
    
    mode = PLANKFILE_BINARY | PLANKFILE_WRITE | PLANKFILE_NATIVEENDIAN;
    if ((result = pl_File_OpenDynamicArray (&outputStream, &p->buffer, mode)) != PlankResult_OK) goto exit;
    if ((result = pl_Zip_DeflateStream (p, amount, &outputStream, &inputStream)) != PlankResult_OK) goto exit;
    
    dataOutput = (const void*)pl_DynamicArray_GetArray (&p->buffer);
    *dataOutputLength = pl_DynamicArray_GetSize (&p->buffer);
    
exit:
    pl_File_DeInit (&inputStream);
    pl_File_DeInit (&outputStream);
    
    return dataOutput;
}

const void* pl_Zip_InflateData (PlankZipRef p, const void* dataInput, const PlankL dataInputLength, PlankL* dataOutputLength)
{
    PlankResult result;
    PlankFile inputStream;
    PlankFile outputStream;
    const void* dataOutput;
    int mode;
    
    result = PlankResult_OK;
    dataOutput = (const void*)PLANK_NULL;
    *dataOutputLength = 0;
    
    if ((result = pl_DynamicArray_SetSize (&p->buffer, 0)) != PlankResult_OK) goto exit;
    if ((result = pl_File_Init (&inputStream)) != PlankResult_OK) goto exit;
    if ((result = pl_File_Init (&outputStream)) != PlankResult_OK) goto exit;
    
    mode = PLANKFILE_BINARY | PLANKFILE_READ | PLANKFILE_NATIVEENDIAN;
    if ((result = pl_File_OpenMemory (&inputStream, (void*)dataInput, dataInputLength, mode)) != PlankResult_OK) goto exit;
    
    mode = PLANKFILE_BINARY | PLANKFILE_WRITE | PLANKFILE_NATIVEENDIAN;
    if ((result = pl_File_OpenDynamicArray (&outputStream, &p->buffer, mode)) != PlankResult_OK) goto exit;
    if ((result = pl_Zip_InflateStream (p, &outputStream, &inputStream)) != PlankResult_OK) goto exit;
    
    dataOutput = (const void*)pl_DynamicArray_GetArray (&p->buffer);
    *dataOutputLength = pl_DynamicArray_GetSize (&p->buffer);
    
exit:
    pl_File_DeInit (&inputStream);
    pl_File_DeInit (&outputStream);
    
    return dataOutput;
}

PlankResult pl_Zip_SetBufferSize (PlankZipRef p, const PlankL size)
{
    return pl_DynamicArray_SetSize (&p->buffer, size);
}

PlankResult pl_Zip_PurgeBuffer (PlankZipRef p)
{
    return pl_DynamicArray_Purge (&p->buffer);
}

#undef deflateInit_
#undef deflate
#undef deflateEnd
#undef inflateInit_
#undef inflate
#undef inflateEnd
#undef inflatePrime
#undef inflateGetHeader
#undef adler32_combine
#undef crc32_combine
#undef deflateInit2_
#undef deflateSetDictionary
#undef deflateCopy
#undef deflateReset
#undef deflateParams
#undef deflateBound
#undef deflatePrime
#undef inflateInit2_
#undef inflateSetDictionary
#undef inflateSync
#undef inflateSyncPoint
#undef inflateCopy
#undef inflateReset
#undef inflateBack
#undef inflateBackEnd
#undef compress
#undef compress2
#undef compressBound
#undef uncompress
#undef adler32
#undef crc32
#undef get_crc_table
#undef zError
#undef alloc_func
#undef free_func
#undef in_func
#undef out_func
#undef Byte
#undef uInt
#undef uLong
#undef Bytef
#undef charf
#undef intf
#undef uIntf
#undef uLongf
#undef voidpf
#undef voidp


