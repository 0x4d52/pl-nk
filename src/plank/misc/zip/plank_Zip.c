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
#include "plank_Zip.h"
#include "../../maths/plank_Maths.h"

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

//#include "../../../../ext/minizip/ioapi.c"
//#include "../../../../ext/minizip/mztools.c"
//#include "../../../../ext/minizip/zip.c"
//#include "../../../../ext/minizip/unzip.c" // must be after zip.c because of the way crypt.h is inlcuded.


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

#define PLANKZIP_CHUNK 16384

PlankResult pl_Zip_DeflateStream (PlankZipRef p, const int amount,  PlankFileRef outputZipFile, PlankFileRef inputFile)
{
    PlankResult result;
    int ret, flush, outputMode, inputMode, bytesRead;
    unsigned have;
    z_stream strm;
    unsigned char in[PLANKZIP_CHUNK];
    unsigned char out[PLANKZIP_CHUNK];

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
            ret = deflate (&strm, flush);    /* no bad return value */
            
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




///* zpipe.c: example of proper use of zlib's inflate() and deflate()
// Not copyrighted -- provided to the public domain
// Version 1.4  11 December 2005  Mark Adler */
//
///* Version history:
// 1.0  30 Oct 2004  First version
// 1.1   8 Nov 2004  Add void casting for unused return values
// Use switch statement for inflate() return values
// 1.2   9 Nov 2004  Add assertions to document zlib guarantees
// 1.3   6 Apr 2005  Remove incorrect assertion in inf()
// 1.4  11 Dec 2005  Add hack to avoid MSDOS end-of-line conversions
// Avoid some compiler warnings for input and output buffers
// */
//
//#include <stdio.h>
//#include <string.h>
//#include <assert.h>
//#include "zlib.h"
//
//#if defined(MSDOS) || defined(OS2) || defined(WIN32) || defined(__CYGWIN__)
//#  include <fcntl.h>
//#  include <io.h>
//#  define SET_BINARY_MODE(file) setmode(fileno(file), O_BINARY)
//#else
//#  define SET_BINARY_MODE(file)
//#endif
//
//#define CHUNK 16384
//
///* Compress from file source to file dest until EOF on source.
// def() returns Z_OK on success, Z_MEM_ERROR if memory could not be
// allocated for processing, Z_STREAM_ERROR if an invalid compression
// level is supplied, Z_VERSION_ERROR if the version of zlib.h and the
// version of the library linked do not match, or Z_ERRNO if there is
// an error reading or writing the files. */
//int def(FILE *source, FILE *dest, int level)
//{
//    int ret, flush;
//    unsigned have;
//    z_stream strm;
//    unsigned char in[CHUNK];
//    unsigned char out[CHUNK];
//    
//    /* allocate deflate state */
//    strm.zalloc = Z_NULL;
//    strm.zfree = Z_NULL;
//    strm.opaque = Z_NULL;
//    ret = deflateInit(&strm, level);
//    if (ret != Z_OK)
//        return ret;
//    
//    /* compress until end of file */
//    do {
//        strm.avail_in = fread(in, 1, CHUNK, source);
//        if (ferror(source)) {
//            (void)deflateEnd(&strm);
//            return Z_ERRNO;
//        }
//        flush = feof(source) ? Z_FINISH : Z_NO_FLUSH;
//        strm.next_in = in;
//        
//        /* run deflate() on input until output buffer not full, finish
//         compression if all of source has been read in */
//        do {
//            strm.avail_out = CHUNK;
//            strm.next_out = out;
//            ret = deflate(&strm, flush);    /* no bad return value */
//            assert(ret != Z_STREAM_ERROR);  /* state not clobbered */
//            have = CHUNK - strm.avail_out;
//            if (fwrite(out, 1, have, dest) != have || ferror(dest)) {
//                (void)deflateEnd(&strm);
//                return Z_ERRNO;
//            }
//        } while (strm.avail_out == 0);
//        assert(strm.avail_in == 0);     /* all input will be used */
//        
//        /* done when last data in file processed */
//    } while (flush != Z_FINISH);
//    assert(ret == Z_STREAM_END);        /* stream will be complete */
//    
//    /* clean up and return */
//    (void)deflateEnd(&strm);
//    return Z_OK;
//}
//
///* Decompress from file source to file dest until stream ends or EOF.
// inf() returns Z_OK on success, Z_MEM_ERROR if memory could not be
// allocated for processing, Z_DATA_ERROR if the deflate data is
// invalid or incomplete, Z_VERSION_ERROR if the version of zlib.h and
// the version of the library linked do not match, or Z_ERRNO if there
// is an error reading or writing the files. */
//int inf(FILE *source, FILE *dest)
//{
//    int ret;
//    unsigned have;
//    z_stream strm;
//    unsigned char in[CHUNK];
//    unsigned char out[CHUNK];
//    
//    /* allocate inflate state */
//    strm.zalloc = Z_NULL;
//    strm.zfree = Z_NULL;
//    strm.opaque = Z_NULL;
//    strm.avail_in = 0;
//    strm.next_in = Z_NULL;
//    ret = inflateInit(&strm);
//    if (ret != Z_OK)
//        return ret;
//    
//    /* decompress until deflate stream ends or end of file */
//    do {
//        strm.avail_in = fread(in, 1, CHUNK, source);
//        if (ferror(source)) {
//            (void)inflateEnd(&strm);
//            return Z_ERRNO;
//        }
//        if (strm.avail_in == 0)
//            break;
//        strm.next_in = in;
//        
//        /* run inflate() on input until output buffer not full */
//        do {
//            strm.avail_out = CHUNK;
//            strm.next_out = out;
//            ret = inflate(&strm, Z_NO_FLUSH);
//            assert(ret != Z_STREAM_ERROR);  /* state not clobbered */
//            switch (ret) {
//                case Z_NEED_DICT:
//                    ret = Z_DATA_ERROR;     /* and fall through */
//                case Z_DATA_ERROR:
//                case Z_MEM_ERROR:
//                    (void)inflateEnd(&strm);
//                    return ret;
//            }
//            have = CHUNK - strm.avail_out;
//            if (fwrite(out, 1, have, dest) != have || ferror(dest)) {
//                (void)inflateEnd(&strm);
//                return Z_ERRNO;
//            }
//        } while (strm.avail_out == 0);
//        
//        /* done when inflate() says it's done */
//    } while (ret != Z_STREAM_END);
//    
//    /* clean up and return */
//    (void)inflateEnd(&strm);
//    return ret == Z_STREAM_END ? Z_OK : Z_DATA_ERROR;
//}
//
///* report a zlib or i/o error */
//void zerr(int ret)
//{
//    fputs("zpipe: ", stderr);
//    switch (ret) {
//        case Z_ERRNO:
//            if (ferror(stdin))
//                fputs("error reading stdin\n", stderr);
//            if (ferror(stdout))
//                fputs("error writing stdout\n", stderr);
//            break;
//        case Z_STREAM_ERROR:
//            fputs("invalid compression level\n", stderr);
//            break;
//        case Z_DATA_ERROR:
//            fputs("invalid or incomplete deflate data\n", stderr);
//            break;
//        case Z_MEM_ERROR:
//            fputs("out of memory\n", stderr);
//            break;
//        case Z_VERSION_ERROR:
//            fputs("zlib version mismatch!\n", stderr);
//    }
//}
//
///* compress or decompress from stdin to stdout */
//int main(int argc, char **argv)
//{
//    int ret;
//    
//    /* avoid end-of-line conversions */
//    SET_BINARY_MODE(stdin);
//    SET_BINARY_MODE(stdout);
//    
//    /* do compression if no arguments */
//    if (argc == 1) {
//        ret = def(stdin, stdout, Z_DEFAULT_COMPRESSION);
//        if (ret != Z_OK)
//            zerr(ret);
//        return ret;
//    }
//    
//    /* do decompression if -d specified */
//    else if (argc == 2 && strcmp(argv[1], "-d") == 0) {
//        ret = inf(stdin, stdout);
//        if (ret != Z_OK)
//            zerr(ret);
//        return ret;
//    }
//    
//    /* otherwise, report usage */
//    else {
//        fputs("zpipe usage: zpipe [-d] < source > dest\n", stderr);
//        return 1;
//    }
//}
