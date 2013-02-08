/********************************************************************
 *                                                                  *
 * THIS FILE IS PART OF THE libopusfile SOFTWARE CODEC SOURCE CODE. *
 * USE, DISTRIBUTION AND REPRODUCTION OF THIS LIBRARY SOURCE IS     *
 * GOVERNED BY A BSD-STYLE SOURCE LICENSE INCLUDED WITH THIS SOURCE *
 * IN 'COPYING'. PLEASE READ THESE TERMS BEFORE DISTRIBUTING.       *
 *                                                                  *
 * THE libopusfile SOURCE CODE IS (C) COPYRIGHT 2012                *
 * by the Xiph.Org Foundation and contributors http://www.xiph.org/ *
 *                                                                  *
 ********************************************************************/
#if !defined(_opusfile_internal_h)
# define _opusfile_internal_h (1)

# if !defined(_REENTRANT)
#  define _REENTRANT
# endif
# if !defined(_GNU_SOURCE)
#  define _GNU_SOURCE
# endif
# if !defined(_LARGEFILE_SOURCE)
#  define _LARGEFILE_SOURCE
# endif
# if !defined(_LARGEFILE64_SOURCE)
#  define _LARGEFILE64_SOURCE
# endif
# if !defined(_FILE_OFFSET_BITS)
#  define _FILE_OFFSET_BITS 64
# endif

# include <stdlib.h>
# include "opusfile.h"

typedef struct OggOpusLink OggOpusLink;
# if defined(OP_FIXED_POINT)
typedef opus_int16 op_sample;
# else
typedef float      op_sample;
# endif

# if OP_GNUC_PREREQ(4,2)
/*Disable excessive warnings about the order of operations.*/
#  pragma GCC diagnostic ignored "-Wparentheses"
# elif defined(_MSC_VER)
/*Disable excessive warnings about the order of operations.*/
#  pragma warning(disable:4554)
/*Disable warnings about "deprecated" POSIX functions.*/
#  pragma warning(disable:4996)
# endif

# if OP_GNUC_PREREQ(3,0)
/*Another alternative is
    (__builtin_constant_p(_x)?!!(_x):__builtin_expect(!!(_x),1))
   but that evaluates _x multiple times, which may be bad.*/
#  define OP_LIKELY(_x) (__builtin_expect(!!(_x),1))
#  define OP_UNLIKELY(_x) (__builtin_expect(!!(_x),0))
# else
#  define OP_LIKELY(_x)   (!!(_x))
#  define OP_UNLIKELY(_x) (!!(_x))
# endif

# define OP_INT64_MAX ((ogg_int64_t)0x7FFFFFFFFFFFFFFFLL)
# define OP_INT64_MIN (-OP_INT64_MAX-1)

/*The maximum channel count for any mapping we'll actually decode.*/
# define OP_NCHANNELS_MAX (8)

# if defined(OP_ENABLE_ASSERTIONS)
#  if OP_GNUC_PREREQ(2,5)||__SUNPRO_C>=0x590
__attribute__((noreturn))
#  endif
void op_fatal_impl(const char *_str,const char *_file,int _line);

#  define OP_FATAL(_str) (op_fatal_impl(_str,__FILE__,__LINE__))

#  define OP_ASSERT(_cond) \
  do{ \
    if(OP_UNLIKELY(!(_cond)))OP_FATAL("assertion failed: " #_cond); \
  } \
  while(0)

# else
#  define OP_FATAL(_str) abort()
#  define OP_ASSERT(_cond)
# endif

# define OP_MIN(_a,_b)        ((_a)<(_b)?(_a):(_b))
# define OP_MAX(_a,_b)        ((_a)>(_b)?(_a):(_b))
# define OP_CLAMP(_lo,_x,_hi) (OP_MAX(_lo,OP_MIN(_x,_hi)))

/*Initial state.*/
# define  OP_NOTOPEN   (0)
/*We've found the first Opus stream in the first link.*/
# define  OP_PARTOPEN  (1)
# define  OP_OPENED    (2)
/*We've found the first Opus stream in the current link.*/
# define  OP_STREAMSET (3)
/*We've initialized the decoder for the chosen Opus stream in the current
   link.*/
# define  OP_INITSET   (4)

/*Information cached for a single link in a chained Ogg Opus file.
  We choose the first Opus stream encountered in each link to play back (and
   require at least one).*/
struct OggOpusLink{
  /*The byte offset of the first header page in this link.*/
  opus_int64   offset;
  /*The byte offset of the first data page from the chosen Opus stream in this
     link (after the headers).*/
  opus_int64   data_offset;
  /*The byte offset of the last page from the chosen Opus stream in this link.
    This is used when seeking to ensure we find a page before the last one, so
     that end-trimming calculations work properly.
    This is only valid for seekable sources.*/
  opus_int64   end_offset;
  /*The granule position of the last sample.
    This is only valid for seekable sources.*/
  ogg_int64_t  pcm_end;
  /*The granule position before the first sample.*/
  ogg_int64_t  pcm_start;
  /*The serial number.*/
  ogg_uint32_t serialno;
  /*The contents of the info header.*/
  OpusHead     head;
  /*The contents of the comment header.*/
  OpusTags     tags;
};

struct OggOpusFile{
  /*The callbacks used to access the data source.*/
  OpusFileCallbacks  callbacks;
  /*A FILE *, memory bufer, etc.*/
  void              *source;
  /*Whether or not we can seek with this data source.*/
  int                seekable;
  /*The number of links in this chained Ogg Opus file.*/
  int                nlinks;
  /*The cached information from each link in a chained Ogg Opus file.
    If source isn't seekable (e.g., it's a pipe), only the current link
     appears.*/
  OggOpusLink       *links;
  /*The number of serial numbers from a single link.*/
  int                nserialnos;
  /*The capacity of the list of serial numbers from a single link.*/
  int                cserialnos;
  /*Storage for the list of serial numbers from a single link.*/
  ogg_uint32_t      *serialnos;
  /*This is the current offset of the data processed by the ogg_sync_state.
    After a seek, this should be set to the target offset so that we can track
     the byte offsets of subsequent pages.
    After a call to op_get_next_page(), this will point to the first byte after
     that page.*/
  opus_int64         offset;
  /*The total size of this data source, or -1 if it's unseekable.*/
  opus_int64         end;
  /*Used to locate pages in the data source.*/
  ogg_sync_state     oy;
  /*One of OP_NOTOPEN, OP_PARTOPEN, OP_OPENED, OP_STREAMSET, OP_INITSET.*/
  int                ready_state;
  /*The current link being played back.*/
  int                cur_link;
  /*The number of decoded samples to discard from the start of decoding.*/
  opus_int32         cur_discard_count;
  /*The granule position of the previous packet (current packet start time).*/
  ogg_int64_t        prev_packet_gp;
  /*The number of bytes read since the last bitrate query, including framing.*/
  opus_int64         bytes_tracked;
  /*The number of samples decoded since the last bitrate query.*/
  ogg_int64_t        samples_tracked;
  /*Takes physical pages and welds them into a logical stream of packets.*/
  ogg_stream_state   os;
  /*Re-timestamped packets from a single page.
    Buffering these relies on the undocumented libogg behavior that ogg_packet
     pointers remain valid until the next page is submitted to the
     ogg_stream_state they came from.*/
  ogg_packet         op[255];
  /*The index of the next packet to return.*/
  int                op_pos;
  /*The total number of packets available.*/
  int                op_count;
  /*Central working state for the packet-to-PCM decoder.*/
  OpusMSDecoder     *od;
  /*The stream count used to initialize the decoder.*/
  int                od_stream_count;
  /*The coupled stream count used to initialize the decoder.*/
  int                od_coupled_count;
  /*The channel count used to initialize the decoder.*/
  int                od_channel_count;
  /*The channel mapping used to initialize the decoder.*/
  unsigned char      od_mapping[OP_NCHANNELS_MAX];
  /*The buffered data for one decoded packet.*/
  op_sample         *od_buffer;
  /*The current position in the decoded buffer.*/
  int                od_buffer_pos;
  /*The number of valid samples in the decoded buffer.*/
  int                od_buffer_size;
  /*Internal state for dithering float->short output.*/
#if !defined(OP_FIXED_POINT)
  float              dither_a[OP_NCHANNELS_MAX*4];
  float              dither_b[OP_NCHANNELS_MAX*4];
  int                dither_mute;
  opus_uint32        dither_seed;
#endif
};

int op_strncasecmp(const char *_a,const char *_b,int _n);

//#ifndef OP_ENABLE_HTTP
//# define OP_ENABLE_HTTP (1)
//#endif

#endif
