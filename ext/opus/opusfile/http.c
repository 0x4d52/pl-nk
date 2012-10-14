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
#include "internal.h"
#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include <string.h>

/*RFCs referenced in this file:
  RFC 1738: Uniform Resource Locators (URL)
  RFC 1945: Hypertext Transfer Protocol -- HTTP/1.0
  RFC 2068: Hypertext Transfer Protocol -- HTTP/1.1
  RFC 2145: Use and Interpretation of HTTP Version Numbers
  RFC 2246: The TLS Protocol Version 1.0
  RFC 2616: Hypertext Transfer Protocol -- HTTP/1.1
  RFC 2617: HTTP Authentication: Basic and Digest Access Authentication
  RFC 2817: Upgrading to TLS Within HTTP/1.1
  RFC 2818: HTTP Over TLS
  RFC 3492: Punycode: A Bootstring encoding of Unicode for Internationalized
   Domain Names in Applications (IDNA)
  RFC 3986: Uniform Resource Identifier (URI): Generic Syntax
  RFC 3987: Internationalized Resource Identifiers (IRIs)
  RFC 5894: Internationalized Domain Names for Applications (IDNA):
   Background, Explanation, and Rationale
  RFC 6066: Transport Layer Security (TLS) Extensions: Extension Definitions*/

typedef struct OpusParsedURL   OpusParsedURL;
typedef struct OpusStringBuf   OpusStringBuf;
typedef struct OpusHTTPConn    OpusHTTPConn;
typedef struct OpusHTTPStream  OpusHTTPStream;

static char *op_string_range_dup(const char *_start,const char *_end){
  size_t  len;
  char   *ret;
  OP_ASSERT(_start<=_end);
  len=_end-_start;
  ret=(char *)_ogg_malloc(sizeof(*ret)*(len+1));
  if(OP_LIKELY(ret!=NULL)){
    memcpy(ret,_start,sizeof(*ret)*(len));
    ret[len]='\0';
  }
  return ret;
}

static char *op_string_dup(const char *_s){
  return op_string_range_dup(_s,_s+strlen(_s));
}

static char *op_string_tolower(char *_s){
  int i;
  for(i=0;_s[i]!='\0';i++){
    int c;
    c=_s[i];
    if(c>='A'&&c<='Z')c+='a'-'A';
    _s[i]=(char)c;
  }
  return _s;
}

/*URI character classes (from RFC 3986).*/
#define OP_URL_ALPHA \
 "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz"
#define OP_URL_DIGIT       "01234567890"
#define OP_URL_HEXDIGIT    "01234567890ABCDEFabcdef"
/*Not a character class, but the characters allowed in <scheme>.*/
#define OP_URL_SCHEME      OP_URL_ALPHA OP_URL_DIGIT "+-."
#define OP_URL_GEN_DELIMS  "#/:?@[]"
#define OP_URL_SUB_DELIMS  "!$&'()*+,;="
#define OP_URL_RESERVED    OP_URL_GEN_DELIMS OP_URL_SUB_DELIMS
#define OP_URL_UNRESERVED  OP_URL_ALPHA OP_URL_DIGIT "-._~"
/*Not a character class, but the characters allowed in <pct-encoded>.*/
#define OP_URL_PCT_ENCODED "%"
/*Not a character class or production rule, but for convenience.*/
#define OP_URL_PCHAR_BASE \
 OP_URL_UNRESERVED OP_URL_PCT_ENCODED OP_URL_SUB_DELIMS
#define OP_URL_PCHAR       OP_URL_PCHAR_BASE ":@"
/*Not a character class, but the characters allowed in <userinfo> and
   <IP-literal>.*/
#define OP_URL_PCHAR_NA    OP_URL_PCHAR_BASE ":"
/*Not a character class, but the characters allowed in <segment-nz-nc>.*/
#define OP_URL_PCHAR_NC    OP_URL_PCHAR_BASE "@"
/*Not a character clsss, but the characters allowed in <path>.*/
#define OP_URL_PATH        OP_URL_PCHAR "/"
/*Not a character class, but the characters allowed in <query> / <fragment>.*/
#define OP_URL_QUERY_FRAG  OP_URL_PCHAR "/?"

/*Check the <% HEXDIG HEXDIG> escapes of a URL for validity.
  Return: 0 if valid, or a negative value on failure.*/
static int op_validate_url_escapes(const char *_s){
  int i;
  for(i=0;_s[i];i++){
    if(_s[i]=='%'){
      if(OP_UNLIKELY(!isxdigit(_s[i+1]))
       ||OP_UNLIKELY(!isxdigit(_s[i+2]))
       /*RFC 3986 says %00 "should be rejected if the application is not
          expecting to receive raw data within a component."*/
       ||OP_UNLIKELY(_s[i+1]=='0'&&_s[i+2]=='0')){
        return OP_FALSE;
      }
      i+=2;
    }
  }
  return 0;
}

/*Convert a hex digit to its actual value.
  _c: The hex digit to convert.
      Presumed to be valid ('0'...'9', 'A'...'F', or 'a'...'f').
  Return: The value of the digit, in the range [0,15].*/
static int op_hex_value(int _c){
  return _c>='a'?_c-'a'+10:_c>='A'?_c-'A'+10:_c-'0';
}

/*Unescape all the <% HEXDIG HEXDIG> sequences in a string in-place.
  This does no validity checking.*/
static char *op_unescape_url_component(char *_s){
  int i;
  int j;
  for(i=j=0;_s[i];i++,j++){
    if(_s[i]=='%'){
      _s[i]=(char)(op_hex_value(_s[i+1])<<4|op_hex_value(_s[i+2]));
      i+=2;
    }
  }
  return _s;
}

/*Parse a file: URL.
  This code is not meant to be fast: strspn() with large sets is likely to be
   slow, but it is very convenient.
  It is meant to be RFC 1738-compliant (as updated by RFC 3986).*/
static const char *op_parse_file_url(const char *_src){
  const char *scheme_end;
  const char *path;
  const char *path_end;
  scheme_end=_src+strspn(_src,OP_URL_SCHEME);
  if(OP_UNLIKELY(*scheme_end!=':')
   ||scheme_end-_src!=4||op_strncasecmp(_src,"file",4)!=0){
    /*Unsupported protocol.*/
    return NULL;
  }
  /*Make sure all escape sequences are valid to simplify unescaping later.*/
  if(OP_UNLIKELY(op_validate_url_escapes(scheme_end+1)<0))return NULL;
  if(scheme_end[1]=='/'&&scheme_end[2]=='/'){
    const char *host;
    /*file: URLs can have a host!
      Yeah, I was surprised, too, but that's what RFC 1738 says.
      It also says, "The file URL scheme is unusual in that it does not specify
       an Internet protocol or access method for such files; as such, its
       utility in network protocols between hosts is limited," which is a mild
       understatement.*/
    host=scheme_end+3;
    /*The empty host is what we expect.*/
    if(OP_LIKELY(*host!='/'))path=host;
    else{
      const char *host_end;
      char        host_buf[28];
      /*RFC 1738 says localhost "is interpreted as `the machine from which the
         URL is being interpreted,'" so let's check for it.*/
      host_end=host+strspn(host,OP_URL_PCHAR_BASE);
      /*No <port> allowed.
        This also rejects IP-Literals.*/
      if(*host_end!='/')return NULL;
      /*An escaped "localhost" can take at most 27 characters.*/
      if(OP_UNLIKELY(host_end-host>27))return NULL;
      memcpy(host_buf,host,sizeof(*host_buf)*(host_end-host));
      host_buf[host_end-host]='\0';
      op_unescape_url_component(host_buf);
      op_string_tolower(host_buf);
      /*Some other host: give up.*/
      if(OP_UNLIKELY(strcmp(host_buf,"localhost")!=0))return NULL;
      path=host_end;
    }
  }
  else path=scheme_end+1;
  path_end=path+strspn(path,OP_URL_PATH);
  /*This will reject a <query> or <fragment> component, too.
    I don't know what to do with queries, but a temporal fragment would at
     least make sense.
    RFC 1738 pretty clearly defines a <searchpart> that's equivalent to the
     RFC 3986 <query> component for other schemes, but not the file: scheme,
     so I'm going to just reject it.*/
  if(*path_end!='\0')return NULL;
  return path;
}

#if defined(OP_ENABLE_HTTP)
# include <sys/ioctl.h>
# include <sys/types.h>
# include <sys/socket.h>
# include <sys/timeb.h>
# include <arpa/inet.h>
# include <netinet/in.h>
# include <netinet/tcp.h>
# include <fcntl.h>
# include <netdb.h>
# include <poll.h>
# include <unistd.h>
# include <openssl/ssl.h>

/*The maximum number of simultaneous connections.
  RFC 2616 says this SHOULD NOT be more than 2, but everyone on the modern web
   ignores that (e.g., IE 8 bumped theirs up from 2 to 6, Firefox uses 15).
  If it makes you feel better, we'll only ever actively read from one of these
   at a time.
  The others are kept around mainly to avoid slow-starting a new connection
   when seeking, and time out rapidly.*/
# define OP_NCONNS_MAX (4)

/*The number of redirections at which we give up.
  The value here is the current default in Firefox.
  RFC 2068 mandated a maximum of 5, but RFC 2616 relaxed that to "a client
   SHOULD detect infinite redirection loops."
  Fortunately, 20 is less than infinity.*/
# define OP_REDIRECT_LIMIT (20)

/*The maximum size of a response message (before the body).
  Responses larger than this will be discarded.
  The buffer for this is currently stack-allocated, which will have to change
   if you want to make it much larger.*/
# define OP_RESPONSE_SIZE_MAX (1024)

/*The number of milliseconds we will allow a connection to sit idle before we
   refuse to resurrect it.
  Apache as of 2.2 has reduced its default timeout to 5 seconds (from 15), so
   that's what we'll use here.*/
# define OP_CONNECTION_IDLE_TIMEOUT_MS (5*1000)

/*The number of milliseconds we will wait to send or receive data before giving
   up.*/
# define OP_POLL_TIMEOUT_MS (30*1000)

/*We will always attempt to read ahead at least this much in preference to
   opening a new connection.*/
# define OP_READAHEAD_THRESH_MIN (32*(opus_int32)1024)
/*The amount to read ahead per iteration of the read-ahead loop.
  16 kB is the largest size OpenSSL will return at once.*/
# define OP_READAHEAD_CHUNK_SIZE (16*1024)

/*The amount of data to request after a seek.
  This is a trade-off between read throughput after a seek vs. the the ability
   to quickly perform another seek with the same connection.*/
# define OP_PIPELINE_CHUNK_SIZE     (32*(opus_int32)1024)
/*Subsequent chunks are requested with larger and larger sizes until they pass
   this threshold, after which we just ask for the rest of the resource.*/
# define OP_PIPELINE_CHUNK_SIZE_MAX (1024*(opus_int32)1024)
/*This is the maximum number of requests we'll make with a single connection.
  Many servers will simply disconnect after we attempt some number of requests,
   possibly without sending a Connection: close header, meaning we won't
   discover it until we try to read beyond the end of the current chunk.
  We can reconnect when that happens, but this is slow.
  Instead, we impose a limit ourselves (set to the default for Apache
   installations and thus likely the most common value in use).*/
# define OP_PIPELINE_MAX_REQUESTS   (100)
/*This should be the number of requests, starting from a chunk size of
   OP_PIPELINE_CHUNK_SIZE and doubling each time, until we exceed
   OP_PIPELINE_CHUNK_SIZE_MAX and just request the rest of the file.
  We won't reuse a connection when seeking unless it has at least this many
   requests left, to reduce the chances we'll have to open a new connection
   while reading forward afterwards.*/
# define OP_PIPELINE_MIN_REQUESTS   (7)

/*Is this an https URL?
  For now we can simply check the last letter of the scheme.*/
# define OP_URL_IS_SSL(_url) ((_url)->scheme[4]=='s')

/*Does this URL use the default port for its scheme?*/
# define OP_URL_IS_DEFAULT_PORT(_url) \
 (!OP_URL_IS_SSL(_url)&&(_url)->port==80 \
 ||OP_URL_IS_SSL(_url)&&(_url)->port==443)

struct OpusParsedURL{
  /*Either "http" or "https".*/
  char     *scheme;
  /*The user name from the <userinfo> component, or NULL.*/
  char     *user;
  /*The password from the <userinfo> component, or NULL.*/
  char     *pass;
  /*The <host> component.
    This may not be NULL.*/
  char     *host;
  /*The <path> and <query> components.
    This may not be NULL.*/
  char     *path;
  /*The <port> component.
    This is set to the default port if the URL did not contain one.*/
  unsigned  port;
};

/*Parse a URL.
  This code is not meant to be fast: strspn() with large sets is likely to be
   slow, but it is very convenient.
  It is meant to be RFC 3986-compliant.
  We currently do not support IRIs (Internationalized Resource Identifiers,
   RFC 3987).
  Callers should translate them to URIs first.*/
static int op_parse_url_impl(OpusParsedURL *_dst,const char *_src){
  const char  *scheme_end;
  const char  *authority;
  const char  *userinfo_end;
  const char  *user;
  const char  *user_end;
  const char  *pass;
  const char  *hostport;
  const char  *hostport_end;
  const char  *host_end;
  const char  *port;
  opus_int32   port_num;
  const char  *port_end;
  const char  *path;
  const char  *path_end;
  const char  *uri_end;
  scheme_end=_src+strspn(_src,OP_URL_SCHEME);
  if(OP_UNLIKELY(*scheme_end!=':')
   ||OP_UNLIKELY(scheme_end-_src<4)||OP_UNLIKELY(scheme_end-_src>5)
   ||OP_UNLIKELY(op_strncasecmp(_src,"https",scheme_end-_src)!=0)){
    /*Unsupported protocol.*/
    return OP_EIMPL;
  }
  if(OP_UNLIKELY(scheme_end[1]!='/')||OP_UNLIKELY(scheme_end[2]!='/')){
    /*We require an <authority> component.*/
    return OP_EINVAL;
  }
  authority=scheme_end+3;
  /*Make sure all escape sequences are valid to simplify unescaping later.*/
  if(OP_UNLIKELY(op_validate_url_escapes(authority)<0))return OP_EINVAL;
  /*Look for a <userinfo> component.*/
  userinfo_end=authority+strspn(authority,OP_URL_PCHAR_NA);
  if(*userinfo_end=='@'){
    /*Found one.*/
    user=authority;
    /*Look for a password (yes, clear-text passwords are deprecated, I know,
       but what else are people supposed to use? use SSL if you care).*/
    user_end=authority+strspn(authority,OP_URL_PCHAR_BASE);
    if(*user_end==':')pass=user_end+1;
    else pass=NULL;
    hostport=userinfo_end+1;
  }
  else{
    /*We shouldn't have to initialize user_end, but gcc is too dumb to figure
       out that user!=NULL below means we didn't take this else branch.*/
    user=user_end=NULL;
    pass=NULL;
    hostport=authority;
  }
  /*Try to figure out where the <host> component ends.*/
  if(hostport[0]=='['){
    hostport++;
    /*We have an <IP-literal>, which can contain colons.*/
    hostport_end=host_end=hostport+strspn(hostport,OP_URL_PCHAR_NA);
    if(OP_UNLIKELY(*hostport_end++!=']'))return OP_EINVAL;
  }
  /*Currently we don't support IDNA (RFC 5894), because I don't want to deal
     with the policy about which domains should not be internationalized to
     avoid confusing similarities.
    Give this API Punycode (RFC 3492) domain names instead.*/
  else hostport_end=host_end=hostport+strspn(hostport,OP_URL_PCHAR_BASE);
  /*TODO: Validate host.*/
  /*Is there a port number?*/
  port_num=-1;
  if(*hostport_end==':'){
    int i;
    port=hostport_end+1;
    port_end=port+strspn(port,OP_URL_DIGIT);
    path=port_end;
    /*Not part of RFC 3986, but require port numbers in the range 0...65535.*/
    if(OP_LIKELY(port_end-port>0)){
      while(*port=='0')port++;
      if(OP_UNLIKELY(port_end-port>5))return OP_EINVAL;
      port_num=0;
      for(i=0;i<port_end-port;i++)port_num=port_num*10+port[i]-'0';
      if(OP_UNLIKELY(port_num>65535))return OP_EINVAL;
    }
  }
  else path=hostport_end;
  path_end=path+strspn(path,OP_URL_PATH);
  /*If the path is not empty, it must begin with a '/'.*/
  if(OP_LIKELY(path_end>path)&&OP_UNLIKELY(path[0]!='/'))return OP_EINVAL;
  /*Consume the <query> component, if any (right now we don't split this out
     from the <path> component).*/
  if(*path_end=='?')path_end=path_end+strspn(path_end,OP_URL_QUERY_FRAG);
  /*Discard the <fragment> component, if any.
    This doesn't get sent to the server.
    Some day we should add support for Media Fragment URIs
     <http://www.w3.org/TR/media-frags/>.*/
  if(*path_end=='#')uri_end=path_end+1+strspn(path_end+1,OP_URL_QUERY_FRAG);
  else uri_end=path_end;
  /*If there's anything left, this was not a valid URL.*/
  if(OP_UNLIKELY(*uri_end!='\0'))return OP_EINVAL;
  _dst->scheme=op_string_range_dup(_src,scheme_end);
  if(OP_UNLIKELY(_dst->scheme==NULL))return OP_EFAULT;
  op_string_tolower(_dst->scheme);
  if(user!=NULL){
    _dst->user=op_string_range_dup(user,user_end);
    if(OP_UNLIKELY(_dst->user==NULL))return OP_EFAULT;
    op_unescape_url_component(_dst->user);
    /*Unescaping might have created a ':' in the username.
      That's not allowed by RFC 2617's Basic Authentication Scheme.*/
    if(OP_UNLIKELY(strchr(_dst->user,':')!=NULL))return OP_EINVAL;
  }
  else _dst->user=NULL;
  if(pass!=NULL){
    _dst->pass=op_string_range_dup(pass,userinfo_end);
    if(OP_UNLIKELY(_dst->pass==NULL))return OP_EFAULT;
    op_unescape_url_component(_dst->pass);
  }
  else _dst->pass=NULL;
  _dst->host=op_string_range_dup(hostport,host_end);
  if(OP_UNLIKELY(_dst->host==NULL))return OP_EFAULT;
  if(port_num<0){
    if(_src[4]=='s')port_num=443;
    else port_num=80;
  }
  _dst->port=(unsigned)port_num;
  /*RFC 2616 says an empty <abs-path> component is equivalent to "/", and we
     MUST use the latter in the Request-URI.
    Reserve space for the slash here.*/
  if(path==path_end||path[0]=='?')path--;
  _dst->path=op_string_range_dup(path,path_end);
  if(OP_UNLIKELY(_dst->path==NULL))return OP_EFAULT;
  /*And force-set it here.*/
  _dst->path[0]='/';
  return 0;
}

static void op_parsed_url_init(OpusParsedURL *_url){
  memset(_url,0,sizeof(*_url));
}

static void op_parsed_url_clear(OpusParsedURL *_url){
  _ogg_free(_url->scheme);
  _ogg_free(_url->user);
  _ogg_free(_url->pass);
  _ogg_free(_url->host);
  _ogg_free(_url->path);
}

static int op_parse_url(OpusParsedURL *_dst,const char *_src){
  OpusParsedURL url;
  int           ret;
  op_parsed_url_init(&url);
  ret=op_parse_url_impl(&url,_src);
  if(OP_UNLIKELY(ret<0))op_parsed_url_clear(&url);
  else *_dst=*&url;
  return ret;
}

/*A buffer to hold growing strings.
  The main purpose of this is to consolidate allocation checks and simplify
   cleanup on a failed allocation.*/
struct OpusStringBuf{
  char *buf;
  int   nbuf;
  int   cbuf;
};

static void op_sb_init(OpusStringBuf *_sb){
  _sb->buf=NULL;
  _sb->nbuf=0;
  _sb->cbuf=0;
}

static void op_sb_clear(OpusStringBuf *_sb){
  _ogg_free(_sb->buf);
}

static int op_sb_ensure_capacity(OpusStringBuf *_sb,int _capacity){
  char *buf;
  int   cbuf;
  buf=_sb->buf;
  cbuf=_sb->cbuf;
  if(_capacity>=cbuf-1){
    if(OP_UNLIKELY(cbuf>=INT_MAX-1>>1))return OP_EFAULT;
    if(OP_UNLIKELY(_capacity>=INT_MAX-1))return OP_EFAULT;
    cbuf=OP_MAX(2*cbuf+1,_capacity+1);
    buf=_ogg_realloc(buf,sizeof(*buf)*cbuf);
    if(OP_UNLIKELY(buf==NULL))return OP_EFAULT;
    _sb->buf=buf;
    _sb->cbuf=cbuf;
  }
  return 0;
}

static int op_sb_append(OpusStringBuf *_sb,const char *_s,int _len){
  char *buf;
  int   nbuf;
  int   ret;
  nbuf=_sb->nbuf;
  if(OP_UNLIKELY(nbuf>INT_MAX-_len))return OP_EFAULT;
  ret=op_sb_ensure_capacity(_sb,nbuf+_len);
  if(OP_UNLIKELY(ret<0))return ret;
  buf=_sb->buf;
  memcpy(buf+nbuf,_s,sizeof(*buf)*_len);
  nbuf+=_len;
  buf[nbuf]='\0';
  _sb->nbuf=nbuf;
  return 0;
}

static int op_sb_append_string(OpusStringBuf *_sb,const char *_s){
  return op_sb_append(_sb,_s,strlen(_s));
}

static int op_sb_append_port(OpusStringBuf *_sb,unsigned _port){
  char port_buf[7];
  OP_ASSERT(_port<=65535U);
  sprintf(port_buf,":%u",_port);
  return op_sb_append_string(_sb,port_buf);
}

static int op_sb_append_nonnegative_int64(OpusStringBuf *_sb,opus_int64 _i){
  char digit;
  int  nbuf_start;
  int  ret;
  OP_ASSERT(_i>=0);
  nbuf_start=_sb->nbuf;
  ret=0;
  do{
    digit='0'+_i%10;
    ret|=op_sb_append(_sb,&digit,1);
    _i/=10;
  }
  while(_i>0);
  if(OP_LIKELY(ret>=0)){
    char *buf;
    int   nbuf_end;
    buf=_sb->buf;
    nbuf_end=_sb->nbuf-1;
    /*We've added the digits backwards.
      Reverse them.*/
    while(nbuf_start<nbuf_end){
      digit=buf[nbuf_start];
      buf[nbuf_start]=buf[nbuf_end];
      buf[nbuf_end]=digit;
      nbuf_start++;
      nbuf_end--;
    }
  }
  return ret;
}

static struct addrinfo *op_resolve(const char *_host,unsigned _port){
  struct addrinfo *addrs;
  struct addrinfo  hints;
  char             service[6];
  memset(&hints,0,sizeof(hints));
  hints.ai_socktype=SOCK_STREAM;
  OP_ASSERT(_port<=65535U);
  sprintf(service,"%u",_port);
  if(OP_LIKELY(!getaddrinfo(_host,service,&hints,&addrs)))return addrs;
  return NULL;
}

static int op_sock_set_nonblocking(int _fd,int _nonblocking){
  int flags;
  flags=fcntl(_fd,F_GETFL);
  if(OP_UNLIKELY(flags<0))return flags;
  if(_nonblocking)flags|=O_NONBLOCK;
  else flags&=~O_NONBLOCK;
  return fcntl(_fd,F_SETFL,flags);
}

/*Disable/enable write coalescing if we can.
  We always send whole requests at once and always parse the response headers
   before sending another one, so normally write coalescing just causes added
   delay.*/
static void op_sock_set_tcp_nodelay(int _fd,int _nodelay){
# if defined(TCP_NODELAY)&&(defined(IPPROTO_TCP)||defined(SOL_TCP))
#  if defined(IPPROTO_TCP)
#   define OP_SO_LEVEL IPPROTO_TCP
#  else
#   define OP_SO_LEVEL SOL_TCP
#  endif
  int ret;
  ret=setsockopt(_fd,OP_SO_LEVEL,TCP_NODELAY,&_nodelay,sizeof(_nodelay));
  /*It doesn't really matter if this call fails, but it would be interesting
     to hit a case where it does.*/
  OP_ASSERT(!ret);
# endif
}

/*A single physical connection to an HTTP server.
  We may have several of these open at once.*/
struct OpusHTTPConn{
  /*The current position indicator for this connection.*/
  opus_int64    pos;
  /*The position where the current request will end, or -1 if we're reading
     until EOF (an unseekable stream or the initial HTTP/1.0 request).*/
  opus_int64    end_pos;
  /*The position where next request we've sent will start, or -1 if we haven't
     sent the next request yet.*/
  opus_int64    next_pos;
  /*The end of the next request or -1 if we requested the rest of the resource.
    This is only set to a meaningful value if next_pos is not -1.*/
  opus_int64    next_end;
  /*The SSL connection, if this is https.*/
  SSL          *ssl_conn;
  /*The next connection in either the LRU or free list.*/
  OpusHTTPConn *next;
  /*The last time we blocked for reading from this connection.*/
  struct timeb  read_time;
  /*The number of bytes we've read since the last time we blocked.*/
  opus_int64    read_bytes;
  /*The estimated throughput of this connection, in bytes/s.*/
  opus_int64    read_rate;
  /*The socket we're reading from.*/
  int           fd;
  /*The number of remaining requests we are allowed on this connection.*/
  int           nrequests_left;
  /*The chunk size to use for pipelining requests.*/
  opus_int32    chunk_size;
};

static void op_http_conn_init(OpusHTTPConn *_conn){
  _conn->next_pos=-1;
  _conn->ssl_conn=NULL;
  _conn->next=NULL;
  _conn->fd=-1;
}

static void op_http_conn_clear(OpusHTTPConn *_conn){
  if(_conn->ssl_conn!=NULL)SSL_free(_conn->ssl_conn);
  /*SSL frees the BIO for us.*/
  if(_conn->fd>=0)close(_conn->fd);
}

/*The global stream state.*/
struct OpusHTTPStream{
  /*The list of connections.*/
  OpusHTTPConn     conns[OP_NCONNS_MAX];
  /*The context object used as a framework for TLS/SSL functions.*/
  SSL_CTX         *ssl_ctx;
  /*The cached session to reuse for future connections.*/
  SSL_SESSION     *ssl_session;
  /*The LRU list (ordered from MRU to LRU) of currently connected
     connections.*/
  OpusHTTPConn    *lru_head;
  /*The free list.*/
  OpusHTTPConn    *free_head;
  /*The URL to connect to.*/
  OpusParsedURL    url;
  /*Information about the address we connected to.*/
  struct addrinfo  addr_info;
  /*The address we connected to.*/
  union{
    struct sockaddr     s;
    struct sockaddr_in  v4;
    struct sockaddr_in6 v6;
  }                addr;
  /*A buffer used to build HTTP requests.*/
  OpusStringBuf    request;
  /*A buffer used to build proxy CONNECT requests.*/
  OpusStringBuf    proxy_connect;
  /*The Content-Length, if specified, or -1 otherwise.
    This will always be specified for seekable streams.*/
  opus_int64       content_length;
  /*The position indicator used when no connection is active.*/
  opus_int64       pos;
  /*The connection we're currently reading from.
    This can be -1 if no connection is active.*/
  int              cur_conni;
  /*Whether or not the server supports range requests.*/
  int              seekable;
  /*Whether or not the server supports HTTP/1.1 with persistent connections.*/
  int              pipeline;
  /*The offset of the tail of the request.
    Only the offset in the Range: header appears after this, allowing us to
     quickly edit the request to ask for a new range.*/
  int              request_tail;
  /*The estimated time required to open a new connection, in milliseconds.*/
  opus_int32       connect_rate;
};

static void op_http_stream_init(OpusHTTPStream *_stream){
  OpusHTTPConn **pnext;
  int ci;
  pnext=&_stream->free_head;
  for(ci=0;ci<OP_NCONNS_MAX;ci++){
    op_http_conn_init(_stream->conns+ci);
    *pnext=_stream->conns+ci;
    pnext=&_stream->conns[ci].next;
  }
  _stream->ssl_ctx=NULL;
  _stream->ssl_session=NULL;
  _stream->lru_head=NULL;
  op_parsed_url_init(&_stream->url);
  op_sb_init(&_stream->request);
  op_sb_init(&_stream->proxy_connect);
  _stream->seekable=0;
}

/*Close the connection and move it to the free list.
  _stream:     The stream containing the free list.
  _conn:       The connection to close.
  _penxt:      The linked-list pointer currently pointing to this connection.
  _gracefully: Whether or not to shut down cleanly.*/
static void op_http_conn_close(OpusHTTPStream *_stream,OpusHTTPConn *_conn,
 OpusHTTPConn **_pnext,int _gracefully){
  /*If we don't shut down gracefully, the server MUST NOT re-use our session
     according to RFC 2246, because it can't tell the difference between an
     abrupt close and a truncation attack.
    So we shut down gracefully if we can.
    However, we will not wait if this would block (it's not worth the savings
     from session resumption to do so).
    Clients (that's us) MAY resume a TLS session that ended with an incomplete
     close, according to RFC 2818, so that's no reason to make sure the server
     shut things down gracefully.
    It also says "client implementations MUST treat any premature closes as
     errors and the data received as potentially truncated," but libopusfile
     treats errors and potentially truncated data in unseekable streams just
     like a normal EOF.
    We warn about this in the docs, and give some suggestions if you truly want
     to avoid truncation attacks.*/
  if(_gracefully&&_conn->ssl_conn!=NULL)SSL_shutdown(_conn->ssl_conn);
  op_http_conn_clear(_conn);
  _conn->next_pos=-1;
  _conn->ssl_conn=NULL;
  _conn->fd=-1;
  OP_ASSERT(*_pnext==_conn);
  *_pnext=_conn->next;
  _conn->next=_stream->free_head;
  _stream->free_head=_conn;
}

static void op_http_stream_clear(OpusHTTPStream *_stream){
  while(_stream->lru_head!=NULL){
    op_http_conn_close(_stream,_stream->lru_head,&_stream->lru_head,0);
  }
  if(_stream->ssl_session!=NULL)SSL_SESSION_free(_stream->ssl_session);
  if(_stream->ssl_ctx!=NULL)SSL_CTX_free(_stream->ssl_ctx);
  op_sb_clear(&_stream->proxy_connect);
  op_sb_clear(&_stream->request);
  op_parsed_url_clear(&_stream->url);
}

static int op_http_conn_write_fully(OpusHTTPConn *_conn,
 const char *_buf,int _size){
  struct pollfd  fd;
  SSL           *ssl_conn;
  fd.fd=_conn->fd;
  ssl_conn=_conn->ssl_conn;
  while(_size>0){
    int err;
    if(ssl_conn!=NULL){
      int ret;
      ret=SSL_write(ssl_conn,_buf,_size);
      if(ret>0){
        /*Wrote some data.*/
        _buf+=ret;
        _size-=ret;
        continue;
      }
      /*Connection closed.*/
      else if(ret==0)return OP_FALSE;
      err=SSL_get_error(ssl_conn,ret);
      /*Yes, renegotiations can cause SSL_write() to block for reading.*/
      if(err==SSL_ERROR_WANT_READ)fd.events=POLLIN;
      else if(err==SSL_ERROR_WANT_WRITE)fd.events=POLLOUT;
      else return OP_FALSE;
    }
    else{
      ssize_t ret;
      errno=0;
      ret=write(fd.fd,_buf,_size);
      if(ret>0){
        _buf+=ret;
        _size-=ret;
        continue;
      }
      err=errno;
      if(err!=EAGAIN&&err!=EWOULDBLOCK)return OP_FALSE;
      fd.events=POLLOUT;
    }
    if(poll(&fd,1,OP_POLL_TIMEOUT_MS)<=0)return OP_FALSE;
  }
  return 0;
}

static int op_http_conn_estimate_available(OpusHTTPConn *_conn){
  int available;
  int ret;
  ret=ioctl(_conn->fd,FIONREAD,&available);
  if(ret<0)available=0;
  /*This requires the SSL read_ahead flag to be unset to work.
    We ignore partial records as well as the protocol overhead for any pending
     bytes.
    This means we might return somewhat less than can truly be read without
     blocking (if there's a partial record).
    This is okay, because we're using this value to estimate network transfer
     time, and we _have_ already received those bytes.
    We also might return slightly more (due to protocol overhead), but that's
     small enough that it probably doesn't matter.*/
  if(_conn->ssl_conn!=NULL)available+=SSL_pending(_conn->ssl_conn);
  return available;
}

static opus_int32 op_time_diff_ms(const struct timeb *_end,
 const struct timeb *_start){
  opus_int64 dtime;
  dtime=_end->time-_start->time;
  OP_ASSERT(_end->millitm<1000);
  OP_ASSERT(_start->millitm<1000);
  if(OP_UNLIKELY(dtime>(0x7FFFFFFF-1000)/1000))return 0x7FFFFFFF;
  if(OP_UNLIKELY(dtime<(-0x7FFFFFFF+999)/1000))return -0x7FFFFFFF-1;
  return (opus_int32)dtime*1000+_end->millitm-_start->millitm;
}

/*Update the read rate estimate for this connection.*/
static void op_http_conn_read_rate_update(OpusHTTPConn *_conn){
  struct timeb read_time;
  opus_int32   read_delta_ms;
  opus_int64   read_delta_bytes;
  opus_int64   read_rate;
  int          ret;
  read_delta_bytes=_conn->read_bytes;
  if(read_delta_bytes<=0)return;
  ret=ftime(&read_time);
  OP_ASSERT(!ret);
  read_delta_ms=op_time_diff_ms(&read_time,&_conn->read_time);
  read_rate=_conn->read_rate;
  read_delta_ms=OP_MAX(read_delta_ms,1);
  read_rate+=read_delta_bytes*1000/read_delta_ms-read_rate+4>>3;
  *&_conn->read_time=*&read_time;
  _conn->read_bytes=0;
  _conn->read_rate=read_rate;
}

/*Tries to read from the given connection.
  [out] _buf: Returns the data read.
  _size:      The size of the buffer.
  _blocking:  Whether or not to block until some data is retrieved.*/
static ptrdiff_t op_http_conn_read(OpusHTTPConn *_conn,
 char *_buf,ptrdiff_t _size,int _blocking){
  struct pollfd   fd;
  SSL            *ssl_conn;
  ptrdiff_t       nread;
  ptrdiff_t       nread_unblocked;
  fd.fd=_conn->fd;
  ssl_conn=_conn->ssl_conn;
  nread=nread_unblocked=0;
  do{
    int err;
    if(ssl_conn!=NULL){
      int ret;
      ret=SSL_read(ssl_conn,_buf+nread,_size-nread);
      if(ret>0){
        /*Read some data.
          Keep going to see if there's more.*/
        nread+=ret;
        nread_unblocked+=ret;
        continue;
      }
      /*Connection closed.*/
      else if(ret==0)break;
      /*If we already read some data, return it right now.*/
      if(nread>0)break;
      err=SSL_get_error(ssl_conn,ret);
      if(err==SSL_ERROR_WANT_READ)fd.events=POLLIN;
      /*Yes, renegotiations can cause SSL_read() to block for writing.*/
      else if(err==SSL_ERROR_WANT_WRITE)fd.events=POLLOUT;
      else return 0;
    }
    else{
      ssize_t ret;
      errno=0;
      ret=read(fd.fd,_buf+nread,_size-nread);
      if(ret>0){
        /*Read some data.
          Keep going to see if there's more.*/
        nread+=ret;
        nread_unblocked+=ret;
        continue;
      }
      /*If we already read some data, return it right now.*/
      if(nread>0)break;
      err=errno;
      if(err!=EAGAIN&&err!=EWOULDBLOCK)return 0;
      fd.events=POLLIN;
    }
    _conn->read_bytes+=nread_unblocked;
    op_http_conn_read_rate_update(_conn);
    nread_unblocked=0;
    if(!_blocking)break;
    /*Need to wait to get any data at all.*/
    if(poll(&fd,1,OP_POLL_TIMEOUT_MS)<=0)return 0;
  }
  while(nread<_size);
  _conn->read_bytes+=nread_unblocked;
  return nread;
}

/*Reads the entirety of a response to an HTTP request into a buffer.
  Actual parsing and validation is done later.
  _buf:  The buffer in which to read the response.
         No terminating NUL is appended.
  _size: The number of bytes available in the buffer.
  Return: The number of bytes in the response on success, OP_EREAD if the
           connection was closed before reading any data, or another negative
           value on any other error.*/
static int op_http_conn_read_response(OpusHTTPConn *_conn,
 char *_buf,int _size){
  /*The remaining size of the buffer.*/
  int size;
  /*How many characters we've yet to see from the "\r\n\r\n" terminator.*/
  int state;
  size=_size;
  state=4;
  while(size>=state){
    ptrdiff_t ret;
    int       len;
    ret=op_http_conn_read(_conn,_buf,state,1);
    if(ret<=0)return _size==size?OP_EREAD:OP_FALSE;
    /*We read some data.*/
    _buf+=ret;
    size-=ret;
    len=_size-size;
    /*Make sure the starting characters are "HTTP".
      Otherwise we could wind up waiting forever for a response from
       something that is not an HTTP server.*/
    if(len-ret<4&&op_strncasecmp(_buf-len,"HTTP",OP_MIN(len,4))!=0){
      return OP_FALSE;
    }
    /*How far along on the "\r\n\r\n" terminator are we?*/
    if(*(_buf-1)=='\n'){
      if(len>=2&&*(_buf-2)=='\r'){
        if(len>=4&&*(_buf-3)=='\n'&&*(_buf-4)=='\r')return len;
        state=2;
      }
      else state=4;
    }
    else if(*(_buf-1)=='\r'){
      state=3;
      if(len>=3&&*(_buf-2)=='\n'&&*(_buf-3)=='\r')state=1;
    }
    else state=4;
  }
  /*Not enough space left in the buffer to add the characters we'd need to get
     a valid terminator.*/
  return OP_EIMPL;
}

# define OP_HTTP_DIGIT "01234567890"

/*The Reason-Phrase is not allowed to contain control characters, except
   horizontal tab (HT: \011).*/
# define OP_HTTP_CREASON_PHRASE \
 "\001\002\003\004\005\006\007\010\012\013\014\015\016\017\020\021" \
 "\022\023\024\025\026\027\030\031\032\033\034\035\036\037\177"

# define OP_HTTP_CTLS \
 "\001\002\003\004\005\006\007\010\011\012\013\014\015\016\017\020" \
 "\021\022\023\024\025\026\027\030\031\032\033\034\035\036\037\177"

/*This also includes '\t', but we get that from OP_HTTP_CTLS.*/
# define OP_HTTP_SEPARATORS " \"(),/:;<=>?@[\\]{}"

/*TEXT can also include LWS, but that has structure, so we parse it
   separately.*/
# define OP_HTTP_CTOKEN OP_HTTP_CTLS OP_HTTP_SEPARATORS

/*Return: The amount of linear white space (LWS) at the start of _s.*/
static int op_http_lwsspn(const char *_s){
  int i;
  for(i=0;;){
    if(_s[0]=='\r'&&_s[1]=='\n'&&(_s[2]=='\t'||_s[2]==' '))i+=3;
    else if(_s[i]=='\t'||_s[i]==' ')i++;
    else return i;
  }
}

static char *op_http_parse_status_line(int *_v1_1_compat,
 char **_status_code,char *_response){
  char   *next;
  char   *status_code;
  int     v1_1_compat;
  size_t  d;
  /*RFC 2616 Section 6.1 does not say that the tokens in the Status-Line cannot
     be separated by optional LWS, but since it specifically calls out where
     spaces are to be placed and that CR and LF are not allowed except at the
     end, I am assuming this to be true.*/
  /*We already validated that this starts with "HTTP"*/
  OP_ASSERT(op_strncasecmp(_response,"HTTP",4)==0);
  next=_response+4;
  if(OP_UNLIKELY(*next++!='/'))return NULL;
  d=strspn(next,OP_HTTP_DIGIT);
  /*"Leading zeros MUST be ignored by recipients."*/
  while(*next=='0'){
    next++;
    OP_ASSERT(d>0);
    d--;
  }
  /*We only support version 1.x*/
  if(OP_UNLIKELY(d!=1)||OP_UNLIKELY(*next++!='1'))return NULL;
  if(OP_UNLIKELY(*next++!='.'))return NULL;
  d=strspn(next,OP_HTTP_DIGIT);
  if(OP_UNLIKELY(d<=0))return NULL;
  /*"Leading zeros MUST be ignored by recipients."*/
  while(*next=='0'){
    next++;
    OP_ASSERT(d>0);
    d--;
  }
  /*We don't need to parse the version number.
    Any non-zero digit means it's greater than 1.*/
  v1_1_compat=d>0;
  next+=d;
  if(OP_UNLIKELY(*next++!=' '))return NULL;
  status_code=next;
  d=strspn(next,OP_HTTP_DIGIT);
  if(OP_UNLIKELY(d!=3))return NULL;
  next+=d;
  /*The Reason-Phrase can be empty, but the space must be here.*/
  if(OP_UNLIKELY(*next++!=' '))return NULL;
  next+=strcspn(next,OP_HTTP_CREASON_PHRASE);
  if(OP_UNLIKELY(*next++!='\r'))return NULL;
  if(OP_UNLIKELY(*next++!='\n'))return NULL;
  if(_v1_1_compat!=NULL)*_v1_1_compat=v1_1_compat;
  *_status_code=status_code;
  return next;
}

/*Get the next response header.
  [out] _header: The header token, NUL-terminated, with leading and trailing
                  whitespace stripped, and converted to lower case (to simplify
                  case-insensitive comparisons), or NULL if there are no more
                  response headers.
  [out] _cdr:    The remaining contents of the header, excluding the initial
                  colon (':') and the terminating CRLF ("\r\n"),
                  NUL-terminated, and with leading and trailing whitespace
                  stripped, or NULL if there are no more response headers.
  [inout] _s:    On input, this points to the start of the current line of the
                  response headers.
                 On output, it points to the start of the first line following
                  this header, or NULL if there are no more response headers.
  Return: 0 on success, or a negative value on failure.*/
static int op_http_get_next_header(char **_header,char **_cdr,char **_s){
  char   *header;
  char   *header_end;
  char   *cdr;
  char   *cdr_end;
  char   *next;
  size_t  d;
  next=*_s;
  if(next[0]=='\r'&&next[1]=='\n'){
    /*No more headers.*/
    *_header=NULL;
    *_cdr=NULL;
    *_s=NULL;
    return 0;
  }
  header=next+op_http_lwsspn(next);
  d=strcspn(header,OP_HTTP_CTOKEN);
  if(OP_UNLIKELY(d<=0))return OP_FALSE;
  header_end=header+d;
  next=header_end+op_http_lwsspn(header_end);
  if(OP_UNLIKELY(*next++!=':'))return OP_FALSE;
  next+=op_http_lwsspn(next);
  cdr=next;
  do{
    cdr_end=next+strcspn(next,OP_HTTP_CTLS);
    next=cdr_end+op_http_lwsspn(cdr_end);
  }
  while(next>cdr_end);
  if(OP_UNLIKELY(*next++!='\r'))return OP_FALSE;
  if(OP_UNLIKELY(*next++!='\n'))return OP_FALSE;
  *header_end='\0';
  *cdr_end='\0';
  /*Field names are case-insensitive.*/
  op_string_tolower(header);
  *_header=header;
  *_cdr=cdr;
  *_s=next;
  return 0;
}

static opus_int64 op_http_parse_nonnegative_int64(const char **_next,
 const char *_cdr){
  const char *next;
  opus_int64  content_length;
  int         i;
  next=_cdr+strspn(_cdr,OP_HTTP_DIGIT);
  *_next=next;
  if(OP_UNLIKELY(next<=_cdr))return OP_FALSE;
  while(*_cdr=='0')_cdr++;
  if(OP_UNLIKELY(next-_cdr>19))return OP_EIMPL;
  content_length=0;
  for(i=0;i<next-_cdr;i++){
    int digit;
    digit=_cdr[i]-'0';
    /*Check for overflow.*/
    if(OP_UNLIKELY(content_length>(OP_INT64_MAX-9)/10+(digit<=7))){
      return OP_EIMPL;
    }
    content_length=content_length*10+digit;
  }
  return content_length;
}

static opus_int64 op_http_parse_content_length(const char *_cdr){
  const char *next;
  opus_int64  content_length;
  content_length=op_http_parse_nonnegative_int64(&next,_cdr);
  if(OP_UNLIKELY(*next!='\0'))return OP_FALSE;
  return content_length;
}

static int op_http_parse_content_range(opus_int64 *_first,opus_int64 *_last,
 opus_int64 *_length,const char *_cdr){
  opus_int64 first;
  opus_int64 last;
  opus_int64 length;
  size_t     d;
  if(OP_UNLIKELY(op_strncasecmp(_cdr,"bytes",5)!=0))return OP_FALSE;
  _cdr+=5;
  d=op_http_lwsspn(_cdr);
  if(OP_UNLIKELY(d<=0))return OP_FALSE;
  _cdr+=d;
  if(*_cdr!='*'){
    first=op_http_parse_nonnegative_int64(&_cdr,_cdr);
    if(OP_UNLIKELY(first<0))return (int)first;
    _cdr+=op_http_lwsspn(_cdr);
    if(*_cdr++!='-')return OP_FALSE;
    _cdr+=op_http_lwsspn(_cdr);
    last=op_http_parse_nonnegative_int64(&_cdr,_cdr);
    if(OP_UNLIKELY(last<0))return (int)last;
    _cdr+=op_http_lwsspn(_cdr);
  }
  else{
    /*This is for a 416 response (Requested range not satisfiable).*/
    first=last=-1;
    _cdr++;
  }
  if(OP_UNLIKELY(*_cdr++!='/'))return OP_FALSE;
  if(*_cdr!='*'){
    length=op_http_parse_nonnegative_int64(&_cdr,_cdr);
    if(OP_UNLIKELY(length<0))return (int)length;
  }
  else{
    /*The total length is unspecified.*/
    _cdr++;
    length=-1;
  }
  if(OP_UNLIKELY(*_cdr!='\0'))return OP_FALSE;
  if(OP_UNLIKELY(last<first))return OP_FALSE;
  if(length>=0&&OP_UNLIKELY(last>=length))return OP_FALSE;
  *_first=first;
  *_last=last;
  *_length=length;
  return 0;
}

/*Parse the Connection response header and look for a "close" token.
  Return: 1 if a "close" token is found, 0 if it's not found, and a negative
           value on error.*/
static int op_http_parse_connection(char *_cdr){
  size_t d;
  int    ret;
  ret=0;
  for(;;){
    d=strcspn(_cdr,OP_HTTP_CTOKEN);
    if(OP_UNLIKELY(d<=0))return OP_FALSE;
    if(op_strncasecmp(_cdr,"close",(int)d)==0)ret=1;
    /*We're supposed to strip and ignore any headers mentioned in the
       Connection header if this response is from an HTTP/1.0 server (to
       work around forwarding of hop-by-hop headers by old proxies), but the
       only hop-by-hop header we look at is Connection itself.
      Everything else is a well-defined end-to-end header, and going back and
       undoing the things we did based on already-examined headers would be
       hard (since we only scan them once, in a destructive manner).
      Therefore we just ignore all the other tokens.*/
    _cdr+=d;
    d=op_http_lwsspn(_cdr);
    if(d<=0)break;
    _cdr+=d;
  }
  return OP_UNLIKELY(*_cdr!='\0')?OP_FALSE:0;
}

typedef int (*op_ssl_step_func)(SSL *_ssl_conn);

/*Try to run an SSL function to completion (blocking if necessary).*/
static int op_do_ssl_step(SSL *_ssl_conn,int _fd,op_ssl_step_func _step){
  struct pollfd fd;
  fd.fd=_fd;
  for(;;){
    int ret;
    int err;
    ret=(*_step)(_ssl_conn);
    if(ret>=0)return ret;
    err=SSL_get_error(_ssl_conn,ret);
    if(err==SSL_ERROR_WANT_READ)fd.events=POLLIN;
    else if(err==SSL_ERROR_WANT_WRITE)fd.events=POLLOUT;
    else return OP_FALSE;
    if(poll(&fd,1,OP_POLL_TIMEOUT_MS)<=0)return OP_FALSE;
  }
}

/*Implement a BIO type that just indicates every operation should be retried.
  We use this when initializing an SSL connection via a proxy to allow the
   initial handshake to proceed all the way up to the first read attempt, and
   then return.
  This allows the TLS client hello message to be pipelined with the HTTP
   CONNECT request.*/

static int op_bio_retry_write(BIO *_b,const char *_buf,int _num){
  (void)_buf;
  (void)_num;
  BIO_clear_retry_flags(_b);
  BIO_set_retry_write(_b);
  return -1;
}

static int op_bio_retry_read(BIO *_b,char *_buf,int _num){
  (void)_buf;
  (void)_num;
  BIO_clear_retry_flags(_b);
  BIO_set_retry_read(_b);
  return -1;
}

static int op_bio_retry_puts(BIO *_b,const char *_str){
  return op_bio_retry_write(_b,_str,0);
}

static long op_bio_retry_ctrl(BIO *_b,int _cmd,long _num,void *_ptr){
  long ret;
  (void)_b;
  (void)_num;
  (void)_ptr;
  ret=0;
  switch(_cmd){
    case BIO_CTRL_RESET:
    case BIO_C_RESET_READ_REQUEST:{
      BIO_clear_retry_flags(_b);
      /*Fall through.*/
    }
    case BIO_CTRL_EOF:
    case BIO_CTRL_SET:
    case BIO_CTRL_SET_CLOSE:
    case BIO_CTRL_FLUSH:
    case BIO_CTRL_DUP:{
      ret=1;
    }break;
  }
  return ret;
}

static int op_bio_retry_new(BIO *_b){
  _b->init=1;
  _b->num=0;
  _b->ptr=NULL;
  return 1;
}

static int op_bio_retry_free(BIO *_b){
  return _b!=NULL;
}

/*This is not const because OpenSSL doesn't allow it, even though it won't
   write to it.*/
static BIO_METHOD op_bio_retry_method={
  BIO_TYPE_NULL,
  "retry",
  op_bio_retry_write,
  op_bio_retry_read,
  op_bio_retry_puts,
  NULL,
  op_bio_retry_ctrl,
  op_bio_retry_new,
  op_bio_retry_free,
  NULL
};

/*Establish a CONNECT tunnel and pipeline the start of the TLS handshake for
   proxying https URL requests.*/
int op_http_conn_establish_tunnel(OpusHTTPStream *_stream,
 OpusHTTPConn *_conn,int _fd,SSL *_ssl_conn,BIO *_ssl_bio){
  char  response[OP_RESPONSE_SIZE_MAX];
  BIO  *retry_bio;
  char *status_code;
  char *next;
  int   ret;
  _conn->ssl_conn=NULL;
  _conn->fd=_fd;
  OP_ASSERT(_stream->proxy_connect.nbuf>0);
  ret=op_http_conn_write_fully(_conn,
   _stream->proxy_connect.buf,_stream->proxy_connect.nbuf);
  if(OP_UNLIKELY(ret<0))return ret;
  retry_bio=BIO_new(&op_bio_retry_method);
  if(OP_UNLIKELY(retry_bio==NULL))return OP_EFAULT;
  SSL_set_bio(_ssl_conn,retry_bio,_ssl_bio);
  SSL_set_connect_state(_ssl_conn);
  ret=SSL_connect(_ssl_conn);
  /*This shouldn't succeed, since we can't read yet.*/
  OP_ASSERT(ret<0);
  SSL_set_bio(_ssl_conn,_ssl_bio,_ssl_bio);
  /*Only now do we disable write coalescing, to allow the CONNECT
     request and the start of the TLS handshake to be combined.*/
  op_sock_set_tcp_nodelay(_fd,1);
  ret=op_http_conn_read_response(_conn,response,sizeof(response));
  if(OP_UNLIKELY(ret<0))return ret;
  next=op_http_parse_status_line(NULL,&status_code,response);
  /*According to RFC 2817, "Any successful (2xx) response to a
     CONNECT request indicates that the proxy has established a
     connection to the requested host and port.*/
  if(OP_UNLIKELY(next==NULL)||OP_UNLIKELY(status_code[0]!='2'))return OP_FALSE;
  return 0;
}

/*Perform the TLS handshake on a new connection.*/
int op_http_conn_start_tls(OpusHTTPStream *_stream,OpusHTTPConn *_conn,
 int _fd,SSL *_ssl_conn){
  BIO *ssl_bio;
  int  ret;
  ssl_bio=BIO_new_socket(_fd,BIO_NOCLOSE);
  if(OP_LIKELY(ssl_bio==NULL))return OP_FALSE;
# if !defined(OPENSSL_NO_TLSEXT)
  /*Support for RFC 6066 Server Name Indication.*/
  SSL_set_tlsext_host_name(_ssl_conn,_stream->url.host);
# endif
  /*Resume a previous session if available.*/
  if(_stream->ssl_session!=NULL){
    SSL_set_session(_ssl_conn,_stream->ssl_session);
  }
  /*If we're proxying, establish the CONNECT tunnel.*/
  if(_stream->proxy_connect.nbuf>0){
    ret=op_http_conn_establish_tunnel(_stream,_conn,
     _fd,_ssl_conn,ssl_bio);
    if(OP_UNLIKELY(ret<0))return ret;
  }
  else{
    /*Otherwise, just use this socket directly.*/
    op_sock_set_tcp_nodelay(_fd,1);
    SSL_set_bio(_ssl_conn,ssl_bio,ssl_bio);
    SSL_set_connect_state(_ssl_conn);
  }
  ret=op_do_ssl_step(_ssl_conn,_fd,SSL_connect);
  if(OP_UNLIKELY(ret<=0))return OP_FALSE;
  if(_stream->ssl_session==NULL){
    /*Save a session for later resumption.*/
    ret=op_do_ssl_step(_ssl_conn,_fd,SSL_do_handshake);
    if(OP_UNLIKELY(ret<=0))return OP_FALSE;
    _stream->ssl_session=SSL_get1_session(_ssl_conn);
  }
  _conn->ssl_conn=_ssl_conn;
  _conn->fd=_fd;
  _conn->nrequests_left=OP_PIPELINE_MAX_REQUESTS;
  return 0;
}

/*Try to start a connection to the next address in the given list of a given
   type.
  _fd:           The socket to connect with.
  [inout] _addr: A pointer to the list of addresses.
                 This will be advanced to the first one that matches the given
                  address family (possibly the current one).
  _ai_family:    The address family to connect to.
  Return: 1        If the connection was successful.
          0        If the connection is in progress.
          OP_FALSE If the connection failed and there were no more addresses
                    left to try.
                    *_addr will be set to NULL in this case.*/
static int op_sock_connect_next(int _fd,
 struct addrinfo **_addr,int _ai_family){
  struct addrinfo *addr;
  addr=*_addr;
  for(;;){
    /*Move to the next address of the requested type.*/
    for(;addr!=NULL&&addr->ai_family!=_ai_family;addr=addr->ai_next);
    *_addr=addr;
    /*No more: failure.*/
    if(addr==NULL)return OP_FALSE;
    if(connect(_fd,addr->ai_addr,addr->ai_addrlen)>=0)return 1;
    if(OP_LIKELY(errno==EINPROGRESS))return 0;
    addr=addr->ai_next;
  }
}

/*The number of address families to try connecting to simultaneously.*/
# define OP_NPROTOS (2)

static int op_http_connect(OpusHTTPStream *_stream,OpusHTTPConn *_conn,
 struct addrinfo *_addrs,struct timeb *_start_time){
  struct addrinfo *addr;
  struct addrinfo *addrs[OP_NPROTOS];
  struct pollfd    fds[OP_NPROTOS];
  int              ai_family;
  int              nprotos;
  int              ret;
  int              pi;
  int              pj;
  for(pi=0;pi<OP_NPROTOS;pi++)addrs[pi]=NULL;
  addr=_addrs;
  /*Try connecting via both IPv4 and IPv6 simultaneously, and keep the first
     one that succeeds.*/
  for(;addr!=NULL;addr=addr->ai_next){
    /*Give IPv6 a slight edge by putting it first in the list.*/
    if(addr->ai_family==AF_INET6){
      OP_ASSERT(addr->ai_addrlen<=sizeof(struct sockaddr_in6));
      if(addrs[0]==NULL)addrs[0]=addr;
    }
    else if(addr->ai_family==AF_INET){
      OP_ASSERT(addr->ai_addrlen<=sizeof(struct sockaddr_in));
      if(addrs[1]==NULL)addrs[1]=addr;
    }
  }
  /*Consolidate the list of addresses.*/
  for(pi=nprotos=0;pi<OP_NPROTOS;pi++){
    if(addrs[pi]!=NULL){
      addrs[nprotos]=addrs[pi];
      nprotos++;
    }
  }
  /*Pop the connection off the free list and put it on the LRU list.*/
  OP_ASSERT(_stream->free_head==_conn);
  _stream->free_head=_conn->next;
  _conn->next=_stream->lru_head;
  _stream->lru_head=_conn;
  ret=ftime(_start_time);
  OP_ASSERT(!ret);
  *&_conn->read_time=*_start_time;
  _conn->read_bytes=0;
  _conn->read_rate=0;
  /*Try to start a connection to each protocol.*/
  for(pi=0;pi<nprotos;pi++){
    ai_family=addrs[pi]->ai_family;
    fds[pi].fd=socket(ai_family,SOCK_STREAM,addrs[pi]->ai_protocol);
    fds[pi].events=POLLOUT;
    if(OP_LIKELY(fds[pi].fd>=0)){
      if(OP_LIKELY(op_sock_set_nonblocking(fds[pi].fd,1)>=0)){
        ret=op_sock_connect_next(fds[pi].fd,addrs+pi,ai_family);
        if(OP_UNLIKELY(ret>0)){
          /*It succeeded right away (technically possible), so stop.*/
          nprotos=pi+1;
          break;
        }
        /*Otherwise go on to the next protocol, and skip the clean-up below.*/
        else if(ret==0)continue;
        /*Tried all the addresses for this protocol.*/
      }
      /*Clean up the socket.*/
      close(fds[pi].fd);
    }
    /*Remove this protocol from the list.*/
    memmove(addrs+pi,addrs+pi+1,sizeof(*addrs)*(nprotos-pi-1));
    nprotos--;
    pi--;
  }
  /*Wait for one of the connections to finish.*/
  while(pi>=nprotos&&nprotos>0&&poll(fds,nprotos,OP_POLL_TIMEOUT_MS)>0){
    for(pi=0;pi<nprotos;pi++){
      socklen_t errlen;
      int       err;
      /*Still waiting...*/
      if(!fds[pi].revents)continue;
      errlen=sizeof(err);
      /*Some platforms will return the pending error in &err and return 0.
        Otherwise will put it in errno and return -1.*/
      ret=getsockopt(fds[pi].fd,SOL_SOCKET,SO_ERROR,&err,&errlen);
      if(ret<0)err=errno;
      /*Success!*/
      if(err==0||err==EISCONN)break;
      /*Move on to the next address for this protocol.*/
      ai_family=addrs[pi]->ai_family;
      addrs[pi]=addrs[pi]->ai_next;
      ret=op_sock_connect_next(fds[pi].fd,addrs+pi,ai_family);
      /*It succeeded right away, so stop.*/
      if(ret>0)break;
      /*Otherwise go on to the next protocol, and skip the clean-up below.*/
      else if(ret==0)continue;
      /*Tried all the addresses for this protocol.
        Remove it from the list.*/
      close(fds[pi].fd);
      memmove(fds+pi,fds+pi+1,sizeof(*fds)*(nprotos-pi-1));
      memmove(addrs+pi,addrs+pi+1,sizeof(*addrs)*(nprotos-pi-1));
      nprotos--;
      pi--;
    }
  }
  /*Close all the other sockets.*/
  for(pj=0;pj<nprotos;pj++)if(pi!=pj)close(fds[pj].fd);
  /*If none of them succeeded, we're done.*/
  if(pi>=nprotos)return OP_FALSE;
  /*Save this address for future connection attempts.*/
  if(addrs[pi]!=&_stream->addr_info){
    memcpy(&_stream->addr_info,addrs[pi],sizeof(_stream->addr_info));
    _stream->addr_info.ai_addr=&_stream->addr.s;
    _stream->addr_info.ai_next=NULL;
    memcpy(&_stream->addr,addrs[pi]->ai_addr,addrs[pi]->ai_addrlen);
  }
  if(OP_URL_IS_SSL(&_stream->url)){
    SSL *ssl_conn;
    /*Start the SSL connection.*/
    OP_ASSERT(_stream->ssl_ctx!=NULL);
    ssl_conn=SSL_new(_stream->ssl_ctx);
    if(OP_LIKELY(ssl_conn!=NULL)){
      ret=op_http_conn_start_tls(_stream,_conn,fds[pi].fd,ssl_conn);
      if(OP_LIKELY(ret>=0))return ret;
      SSL_free(ssl_conn);
    }
    close(fds[pi].fd);
    _conn->fd=-1;
    return OP_FALSE;
  }
  /*Just a normal non-SSL connection.*/
  _conn->ssl_conn=NULL;
  _conn->fd=fds[pi].fd;
  _conn->nrequests_left=OP_PIPELINE_MAX_REQUESTS;
  /*Disable write coalescing.
    We always send whole requests at once and always parse the response headers
     before sending another one.*/
  op_sock_set_tcp_nodelay(fds[pi].fd,1);
  return 0;
}

# define OP_BASE64_LENGTH(_len) (((_len)+2)/3*4)

static const char BASE64_TABLE[64]={
  'A','B','C','D','E','F','G','H','I','J','K','L','M','N','O','P',
  'Q','R','S','T','U','V','W','X','Y','Z','a','b','c','d','e','f',
  'g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v',
  'w','x','y','z','0','1','2','3','4','5','6','7','8','9','+','/'
};

static char *op_base64_encode(char *_dst,const char *_src,int _len){
  unsigned s0;
  unsigned s1;
  unsigned s2;
  int      ngroups;
  int      i;
  ngroups=_len/3;
  for(i=0;i<ngroups;i++){
    s0=_src[3*i+0];
    s1=_src[3*i+1];
    s2=_src[3*i+2];
    _dst[4*i+0]=BASE64_TABLE[s0>>2];
    _dst[4*i+1]=BASE64_TABLE[s0&3<<4|s1>>4];
    _dst[4*i+2]=BASE64_TABLE[s1&15<<2|s2>>6];
    _dst[4*i+3]=BASE64_TABLE[s2&63];
  }
  _len-=3*i;
  if(_len==1){
    s0=_src[3*i+0];
    _dst[4*i+0]=BASE64_TABLE[s0>>2];
    _dst[4*i+1]=BASE64_TABLE[s0&3<<4];
    _dst[4*i+2]='=';
    _dst[4*i+3]='=';
    i++;
  }
  else if(_len==2){
    s0=_src[3*i+0];
    s1=_src[3*i+1];
    _dst[4*i+0]=BASE64_TABLE[s0>>2];
    _dst[4*i+1]=BASE64_TABLE[s0&3<<4|s1>>4];
    _dst[4*i+2]=BASE64_TABLE[s1&15<<2];
    _dst[4*i+3]='=';
    i++;
  }
  _dst[4*i]='\0';
  return _dst+4*i;
}

/*Construct an HTTP authorization header using RFC 2617's Basic Authentication
   Scheme and append it to the given string buffer.*/
static int op_sb_append_basic_auth_header(OpusStringBuf *_sb,
 const char *_header,const char *_user,const char *_pass){
  int user_len;
  int pass_len;
  int user_pass_len;
  int base64_len;
  int nbuf_total;
  int ret;
  ret=op_sb_append_string(_sb,_header);
  ret|=op_sb_append(_sb,": Basic ",8);
  user_len=strlen(_user);
  pass_len=strlen(_pass);
  if(OP_UNLIKELY(pass_len>INT_MAX-user_len))return OP_EFAULT;
  if(OP_UNLIKELY(user_len+pass_len>(INT_MAX>>2)*3-3))return OP_EFAULT;
  user_pass_len=user_len+1+pass_len;
  base64_len=OP_BASE64_LENGTH(user_pass_len);
  /*Stick "user:pass" at the end of the buffer so we can Base64 encode it
     in-place.*/
  nbuf_total=_sb->nbuf;
  if(OP_UNLIKELY(base64_len>INT_MAX-nbuf_total))return OP_EFAULT;
  nbuf_total+=base64_len;
  ret|=op_sb_ensure_capacity(_sb,nbuf_total);
  if(OP_UNLIKELY(ret<0))return ret;
  _sb->nbuf=nbuf_total-user_pass_len;
  ret=op_sb_append(_sb,_user,user_len);
  OP_ASSERT(!ret);
  ret=op_sb_append(_sb,":",1);
  OP_ASSERT(!ret);
  ret=op_sb_append(_sb,_pass,pass_len);
  OP_ASSERT(!ret);
  op_base64_encode(_sb->buf+nbuf_total-base64_len,
   _sb->buf+nbuf_total-user_pass_len,user_pass_len);
  return op_sb_append(_sb,"\r\n",2);
}

static int op_http_allow_pipelining(const char *_server){
  /*Servers known to do bad things with pipelined requests.
    This list is taken from Gecko's nsHttpConnection::SupportsPipelining() (in
     netwerk/protocol/http/nsHttpConnection.cpp).*/
  static const char *BAD_SERVERS[]={
    "EFAServer/",
    "Microsoft-IIS/4.",
    "Microsoft-IIS/5.",
    "Netscape-Enterprise/3.",
    "Netscape-Enterprise/4.",
    "Netscape-Enterprise/5.",
    "Netscape-Enterprise/6.",
    "WebLogic 3.",
    "WebLogic 4.",
    "WebLogic 5.",
    "WebLogic 6.",
    "Winstone Servlet Engine v0."
  };
# define NBAD_SERVERS ((int)(sizeof(BAD_SERVERS)/sizeof(*BAD_SERVERS)))
  if(*_server>='E'&&*_server<='W'){
    int si;
    for(si=0;si<NBAD_SERVERS;si++){
      if(strncmp(_server,BAD_SERVERS[si],strlen(BAD_SERVERS[si]))==0){
        return 0;
      }
    }
  }
  return 1;
# undef NBAD_SERVERS
}

static int op_http_stream_open(OpusHTTPStream *_stream,const char *_url,
 int _flags,const char *_proxy_host,unsigned _proxy_port,
 const char *_proxy_user,const char *_proxy_pass){
  struct addrinfo *addrs;
  const char      *last_host;
  unsigned         last_port;
  int              nredirs;
  int              ret;
  if(_proxy_host!=NULL&&OP_UNLIKELY(_proxy_port>65535U))return OP_EINVAL;
  last_host=NULL;
  /*We shouldn't have to initialize last_port, but gcc is too dumb to figure
     out that last_host!=NULL implies we've already taken one trip through the
     loop.*/
  last_port=0;
  ret=op_parse_url(&_stream->url,_url);
  if(OP_UNLIKELY(ret<0))return ret;
  for(nredirs=0;nredirs<OP_REDIRECT_LIMIT;nredirs++){
    struct timeb  start_time;
    struct timeb  end_time;
    char          response[OP_RESPONSE_SIZE_MAX];
    char         *next;
    char         *status_code;
    const char   *host;
    unsigned      port;
    int           minor_version_pos;
    int           v1_1_compat;
    if(_proxy_host==NULL){
      host=_stream->url.host;
      port=_stream->url.port;
    }
    else{
      host=_proxy_host;
      port=_proxy_port;
    }
    /*If connecting to the same place as last time, don't re-resolve it.*/
    addrs=NULL;
    if(last_host!=NULL){
      if(strcmp(last_host,host)==0&&last_port==port)addrs=&_stream->addr_info;
      else if(_stream->ssl_session!=NULL){
        /*Forget any cached SSL session from the last host.*/
        SSL_SESSION_free(_stream->ssl_session);
        _stream->ssl_session=NULL;
      }
      if(last_host!=_proxy_host)_ogg_free((void *)last_host);
    }
    last_host=host;
    last_port=port;
    /*Initialize the SSL library if necessary.*/
    if(OP_URL_IS_SSL(&_stream->url)&&_stream->ssl_ctx==NULL){
      SSL_CTX *ssl_ctx;
# if !defined(OPENSSL_NO_LOCKING)
      /*The documentation says SSL_library_init() is not reentrant.
        We don't want to add our own depenencies on a threading library, and it
         appears that it's safe to call OpenSSL's locking functions before the
         library is initialized, so that's what we'll do (really OpenSSL should
         do this for us).
        This doesn't guarantee that _other_ threads in the application aren't
         calling SSL_library_init() at the same time, but there's not much we
         can do about that.*/
      CRYPTO_w_lock(CRYPTO_LOCK_SSL);
# endif
      SSL_library_init();
      /*Needed to get SHA2 algorithms with old OpenSSL versions.*/
      OpenSSL_add_ssl_algorithms();
# if !defined(OPENSSL_NO_LOCKING)
      CRYPTO_w_unlock(CRYPTO_LOCK_SSL);
# endif
      ssl_ctx=SSL_CTX_new(SSLv23_client_method());
      if(ssl_ctx==NULL)return OP_EFAULT;
      if(!(_flags&OP_SSL_SKIP_CERTIFICATE_CHECK)){
        SSL_CTX_set_verify(ssl_ctx,SSL_VERIFY_PEER,NULL);
      }
      _stream->ssl_ctx=ssl_ctx;
      if(_proxy_host!=NULL){
        /*We need to establish a CONNECT tunnel to handle https proxying.
          Build the request we'll send to do so.*/
        ret=op_sb_append(&_stream->proxy_connect,"CONNECT ",8);
        ret|=op_sb_append_string(&_stream->proxy_connect,_stream->url.host);
        ret|=op_sb_append_port(&_stream->proxy_connect,_stream->url.port);
        /*CONNECT requires at least HTTP 1.1.*/
        ret|=op_sb_append(&_stream->proxy_connect," HTTP/1.1\r\n",11);
        ret|=op_sb_append(&_stream->proxy_connect,"Host: ",6);
        ret|=op_sb_append_string(&_stream->proxy_connect,_stream->url.host);
        /*The example in RFC 2817 Section 5.2 specifies an explicit port even
           when connecting to the default port.
          Given that the proxy doesn't know whether we're trying to connect to
           an http or an https URL except by the port number, this seems like a
           good idea.*/
        ret|=op_sb_append_port(&_stream->proxy_connect,_stream->url.port);
        ret|=op_sb_append(&_stream->proxy_connect,"\r\n",2);
        ret|=op_sb_append(&_stream->proxy_connect,"User-Agent: .\r\n",15);
        if(_proxy_user!=NULL&&_proxy_pass!=NULL){
          ret|=op_sb_append_basic_auth_header(&_stream->proxy_connect,
           "Proxy-Authorization",_proxy_user,_proxy_pass);
        }
        /*For backwards compatibility.*/
        ret|=op_sb_append(&_stream->proxy_connect,
         "Proxy-Connection: keep-alive\r\n",30);
        ret|=op_sb_append(&_stream->proxy_connect,"\r\n",2);
        if(OP_UNLIKELY(ret<0))return ret;
      }
    }
    /*Actually make the connection.*/
    if(addrs!=&_stream->addr_info){
      addrs=op_resolve(host,port);
      if(OP_UNLIKELY(addrs==NULL))return OP_FALSE;
    }
    ret=op_http_connect(_stream,_stream->conns+0,addrs,&start_time);
    if(addrs!=&_stream->addr_info)freeaddrinfo(addrs);
    if(OP_UNLIKELY(ret<0))return ret;
    /*Build the request to send.*/
    _stream->request.nbuf=0;
    ret=op_sb_append(&_stream->request,"GET ",4);
    ret|=op_sb_append_string(&_stream->request,
     _proxy_host!=NULL?_url:_stream->url.path);
    /*Send HTTP/1.0 by default for maximum compatibility (so we don't have to
       re-try if HTTP/1.1 fails, though it shouldn't, even for a 1.0 server).
      This means we aren't conditionally compliant with RFC 2145, because we
       violate the requirement that "An HTTP client SHOULD send a request
       version equal to the highest version for which the client is at least
       conditionally compliant...".
      According to RFC 2145, that means we can't claim any compliance with any
       IETF HTTP specification.*/
    ret|=op_sb_append(&_stream->request," HTTP/1.0\r\n",11);
    /*Remember where this is so we can upgrade to HTTP/1.1 if the server
       supports it.*/
    minor_version_pos=_stream->request.nbuf-3;
    ret|=op_sb_append(&_stream->request,"Host: ",6);
    ret|=op_sb_append_string(&_stream->request,_stream->url.host);
    if(!OP_URL_IS_DEFAULT_PORT(&_stream->url)){
      ret|=op_sb_append_port(&_stream->request,_stream->url.port);
    }
    ret|=op_sb_append(&_stream->request,"\r\n",2);
    /*User-Agents have been a bad idea, so send as little as possible.
      RFC 2616 requires at least one token in the User-Agent, which must have
       at least one character.*/
    ret|=op_sb_append(&_stream->request,"User-Agent: .\r\n",15);
    if(_proxy_host!=NULL&&!OP_URL_IS_SSL(&_stream->url)
     &&_proxy_user!=NULL&&_proxy_pass!=NULL){
      ret|=op_sb_append_basic_auth_header(&_stream->request,
       "Proxy-Authorization",_proxy_user,_proxy_pass);
    }
    if(_stream->url.user!=NULL&&_stream->url.pass!=NULL){
      ret|=op_sb_append_basic_auth_header(&_stream->request,
       "Authorization",_stream->url.user,_stream->url.pass);
    }
    /*Always send a Referer [sic] header.
      It's common to refuse to serve a resource unless one is present.
      We just use the relative "/" URI to suggest we came from the same domain,
       as this is the most common check.
      This might violate RFC 2616's mandate that the field "MUST NOT be sent if
       the Request-URI was obtained from a source that does not have its own
       URI, such as input from the user keyboard," but we don't really have any
       way to know.*/
    /*TODO: Should we update this on redirects?*/
    ret|=op_sb_append(&_stream->request,"Referer: /\r\n",12);
    /*Always send a Range request header to find out if we're seekable.
      This requires an HTTP/1.1 server to succeed, but we'll still get what we
       want with an HTTP/1.0 server that ignores this request header.*/
    ret|=op_sb_append(&_stream->request,"Range: bytes=0-\r\n",17);
    /*Remember where this is so we can append offsets to it later.*/
    _stream->request_tail=_stream->request.nbuf-4;
    ret|=op_sb_append(&_stream->request,"\r\n",2);
    if(OP_UNLIKELY(ret<0))return ret;
    ret=op_http_conn_write_fully(_stream->conns+0,
     _stream->request.buf,_stream->request.nbuf);
    if(OP_UNLIKELY(ret<0))return ret;
    ret=op_http_conn_read_response(_stream->conns+0,
     response,sizeof(response)/sizeof(*response));
    if(OP_UNLIKELY(ret<0))return ret;
    ret=ftime(&end_time);
    OP_ASSERT(!ret);
    next=op_http_parse_status_line(&v1_1_compat,&status_code,response);
    if(OP_UNLIKELY(next==NULL))return OP_FALSE;
    if(status_code[0]=='2'){
      opus_int64 content_length;
      opus_int64 range_length;
      int        pipeline;
      /*We only understand 20x codes.*/
      if(status_code[1]!='0')return OP_FALSE;
      content_length=-1;
      range_length=-1;
      /*Pipelining is disabled by default.*/
      pipeline=0;
      for(;;){
        char *header;
        char *cdr;
        ret=op_http_get_next_header(&header,&cdr,&next);
        if(OP_UNLIKELY(ret<0))return ret;
        if(header==NULL)break;
        if(strcmp(header,"content-length")==0){
          /*Two Content-Length headers?*/
          if(OP_UNLIKELY(content_length>=0))return OP_FALSE;
          content_length=op_http_parse_content_length(cdr);
          if(OP_UNLIKELY(content_length<0))return (int)content_length;
          /*Make sure the Content-Length and Content-Range headers match.*/
          if(range_length>=0&&OP_UNLIKELY(content_length!=range_length)){
            return OP_FALSE;
          }
        }
        else if(strcmp(header,"content-range")==0){
          opus_int64 range_first;
          opus_int64 range_last;
          /*Two Content-Range headers?*/
          if(OP_UNLIKELY(range_length>=0))return OP_FALSE;
          ret=op_http_parse_content_range(&range_first,&range_last,
           &range_length,cdr);
          if(OP_UNLIKELY(ret<0))return ret;
          /*"A response with satus code 206 (Partial Content) MUST NOTE
             include a Content-Range field with a byte-range-resp-spec of
             '*'."*/
          if(status_code[2]=='6'
           &&(OP_UNLIKELY(range_first<0)||OP_UNLIKELY(range_last<0))){
            return OP_FALSE;
          }
          /*We asked for the entire resource.*/
          if(range_length>=0){
            /*Quit if we didn't get it.*/
            if(range_last>=0&&OP_UNLIKELY(range_last!=range_length-1)){
              return OP_FALSE;
            }
          }
          /*If there was no length, use the end of the range.*/
          else if(range_last>=0)range_length=range_last+1;
          /*Make sure the Content-Length and Content-Range headers match.*/
          if(content_length>=0&&OP_UNLIKELY(content_length!=range_length)){
            return OP_FALSE;
          }
        }
        else if(strcmp(header,"connection")==0){
          /*According to RFC 2616, if an HTTP/1.1 application does not support
             pipelining, it "MUST include the 'close' connection option in
             every message."
            Therefore, if we receive one in the initial response, disable
             pipelining entirely.
            The server still might support it (e.g., we might just have hit the
             request limit for a temporary child process), but if it doesn't
             and we assume it does, every time we cross a chunk boundary we'll
             error out and reconnect, adding lots of latency.*/
          ret=op_http_parse_connection(cdr);
          if(OP_UNLIKELY(ret<0))return ret;
          pipeline-=ret;
        }
        else if(strcmp(header,"server")){
          /*If we got a Server response header, and it wasn't from a known-bad
             server, enable pipelining, as long as it's at least HTTP/1.1.
            According to RFC 2145, the server is supposed to respond with the
             highest minor version number it supports unless it is known or
             suspected that we incorrectly implement the HTTP specification.
            So it should send back at least HTTP/1.1, despite our HTTP/1.0
             request.*/
          pipeline+=v1_1_compat&&op_http_allow_pipelining(cdr);
        }
      }
      switch(status_code[2]){
        /*200 OK*/
        case '0':break;
        /*203 Non-Authoritative Information*/
        case '3':break;
        /*204 No Content*/
        case '4':{
          if(content_length>=0&&OP_UNLIKELY(content_length!=0)){
            return OP_FALSE;
          }
        }break;
        /*206 Partial Content*/
        case '6':{
          /*No Content-Range header.*/
          if(OP_UNLIKELY(range_length<0))return OP_FALSE;
          content_length=range_length;
          /*The server supports range requests for this resource.
            We can seek.*/
          _stream->seekable=1;
        }break;
        /*201 Created: the response "SHOULD include an entity containing a list
           of resource characteristics and location(s)," but not an Opus file.
          202 Accepted: the response "SHOULD include an indication of request's
           current status and either a pointer to a status monitor or some
           estimate of when the user can expect the request to be fulfilled,"
           but not an Opus file.
          205 Reset Content: this "MUST NOT include an entity," meaning no Opus
           file.
          207...209 are not yet defined, so we don't know how to handle them.*/
        default:return OP_FALSE;
      }
      _stream->content_length=content_length;
      _stream->pipeline=pipeline>0;
      /*Pipelining requires HTTP/1.1 persistent connections.*/
      if(pipeline)_stream->request.buf[minor_version_pos]='1';
      _stream->conns[0].pos=0;
      _stream->conns[0].end_pos=_stream->seekable?content_length:-1;
      _stream->conns[0].chunk_size=-1;
      _stream->cur_conni=0;
      _stream->connect_rate=op_time_diff_ms(&end_time,&start_time);
      _stream->connect_rate=OP_MAX(_stream->connect_rate,1);
      /*The URL has been successfully opened.*/
      return 0;
    }
    /*Shouldn't get 1xx; 4xx and 5xx are both failures (and we don't retry).
      Everything else is undefined.*/
    else if(status_code[0]!='3')return OP_FALSE;
    /*We have some form of redirect request.*/
    /*We only understand 30x codes.*/
    if(status_code[1]!='0')return OP_FALSE;
    switch(status_code[2]){
      /*300 Multiple Choices: "If the server has a preferred choice of
         representation, it SHOULD include the specific URI for that
         representation in the Location field," otherwise we'll fail.*/
      case '0':
      /*301 Moved Permanently*/
      case '1':
      /*302 Found*/
      case '2':
      /*307 Temporary Redirect*/
      case '7':break;
      /*305 Use Proxy: "The Location field gives the URI of the proxy."
        TODO: This shouldn't actually be that hard to do.*/
      case '5':return OP_EIMPL;
      /*303 See Other: "The new URI is not a substitute reference for the
         originally requested resource."
        304 Not Modified: "The 304 response MUST NOT contain a message-body."
        306 (Unused)
        308...309 are not yet defined, so we don't know how to handle them.*/
      default:return OP_FALSE;
    }
    _url=NULL;
    for(;;){
      char *header;
      char *cdr;
      ret=op_http_get_next_header(&header,&cdr,&next);
      if(OP_UNLIKELY(ret<0))return ret;
      if(header==NULL)break;
      if(strcmp(header,"location")==0&&OP_LIKELY(_url==NULL))_url=cdr;
    }
    if(OP_UNLIKELY(_url==NULL))return OP_FALSE;
    /*Don't free last_host if it came from the last URL.*/
    if(last_host!=_proxy_host)_stream->url.host=NULL;
    op_parsed_url_clear(&_stream->url);
    ret=op_parse_url(&_stream->url,_url);
    if(OP_UNLIKELY(ret<0)){
      if(ret==OP_EINVAL)ret=OP_FALSE;
      if(last_host!=_proxy_host)_ogg_free((void *)last_host);
      return ret;
    }
    op_http_conn_close(_stream,_stream->conns+0,&_stream->lru_head,1);
  }
  /*Redirection limit reached.*/
  return OP_FALSE;
}

static int op_http_conn_send_request(OpusHTTPStream *_stream,
 OpusHTTPConn *_conn,opus_int64 _pos,opus_int32 _chunk_size,
 int _try_not_to_block){
  opus_int64 next_end;
  int        ret;
  /*We shouldn't have another request outstanding.*/
  OP_ASSERT(_conn->next_pos<0);
  /*Build the request to send.*/
  OP_ASSERT(_stream->request.nbuf>=_stream->request_tail);
  _stream->request.nbuf=_stream->request_tail;
  ret=op_sb_append_nonnegative_int64(&_stream->request,_pos);
  ret|=op_sb_append(&_stream->request,"-",1);
  if(_chunk_size>0&&_pos<=OP_INT64_MAX-_chunk_size
   &&_pos+_chunk_size<_stream->content_length){
    /*We shouldn't be pipelining requests with non-HTTP/1.1 servers.*/
    OP_ASSERT(_stream->pipeline);
    next_end=_pos+_chunk_size;
    ret|=op_sb_append_nonnegative_int64(&_stream->request,next_end-1);
    /*Use a larger chunk size for our next request.*/
    _chunk_size<<=1;
    /*But after a while, just request the rest of the resource.*/
    if(_chunk_size>OP_PIPELINE_CHUNK_SIZE_MAX)_chunk_size=-1;
  }
  else{
    next_end=-1;
    _chunk_size=-1;
  }
  ret|=op_sb_append(&_stream->request,"\r\n\r\n",4);
  if(OP_UNLIKELY(ret<0))return ret;
  /*If we don't want to block, check to see if there's enough space in the send
     queue.
    There's still a chance we might block, even if there is enough space, but
     it's a much slimmer one.
    Blocking at all is pretty unlikely, as we won't have any requests queued
     when _try_not_to_block is set, so if FIONSPACE isn't available (e.g., on
     Linux), just skip the test.*/
  if(_try_not_to_block){
# if defined(FIONSPACE)
    int available;
    ret=ioctl(_conn->fd,FIONSPACE,&available);
    if(ret<0||available<_stream->request.nbuf)return 1;
# endif
  }
  ret=op_http_conn_write_fully(_conn,
   _stream->request.buf,_stream->request.nbuf);
  if(OP_UNLIKELY(ret<0))return ret;
  _conn->next_pos=_pos;
  _conn->next_end=next_end;
  /*Save the chunk size to use for the next request.*/
  _conn->chunk_size=_chunk_size;
  _conn->nrequests_left--;
  return ret;
}

/*Handles the response to all requests after the first one.
  Return: 1 if the connection was closed or timed out, 0 on success, or a
           negative value on any other error.*/
static int op_http_conn_handle_response(OpusHTTPStream *_stream,
 OpusHTTPConn *_conn){
  char        response[OP_RESPONSE_SIZE_MAX];
  char       *next;
  char       *status_code;
  opus_int64  range_length;
  opus_int64  next_pos;
  opus_int64  next_end;
  int         ret;
  ret=op_http_conn_read_response(_conn,
   response,sizeof(response)/sizeof(*response));
  /*If the server just closed the connection on us, we may have just hit a
     connection re-use limit, so we might want to retry.*/
  if(OP_UNLIKELY(ret<0))return ret==OP_EREAD?1:ret;
  next=op_http_parse_status_line(NULL,&status_code,response);
  if(OP_UNLIKELY(next==NULL))return OP_FALSE;
  /*We _need_ a 206 Partial Content response.
    Nothing else will do.*/
  if(strncmp(status_code,"206",3)!=0){
    /*But on a 408 Request Timeout, we might want to re-try.*/
    return strncmp(status_code,"408",3)==0?1:OP_FALSE;
  }
  next_pos=_conn->next_pos;
  next_end=_conn->next_end;
  range_length=-1;
  for(;;){
    char *header;
    char *cdr;
    ret=op_http_get_next_header(&header,&cdr,&next);
    if(OP_UNLIKELY(ret<0))return ret;
    if(header==NULL)break;
    if(strcmp(header,"content-range")==0){
      opus_int64 range_first;
      opus_int64 range_last;
      /*Two Content-Range headers?*/
      if(OP_UNLIKELY(range_length>=0))return OP_FALSE;
      ret=op_http_parse_content_range(&range_first,&range_last,
       &range_length,cdr);
      if(OP_UNLIKELY(ret<0))return ret;
      /*"A response with satus code 206 (Partial Content) MUST NOT
         include a Content-Range field with a byte-range-resp-spec of
         '*'."*/
      if(OP_UNLIKELY(range_first<0)||OP_UNLIKELY(range_last<0))return OP_FALSE;
      /*We also don't want range_last to overflow.*/
      if(OP_UNLIKELY(range_last>=OP_INT64_MAX))return OP_FALSE;
      range_last++;
      /*Quit if we didn't get the offset we asked for.*/
      if(range_first!=next_pos)return OP_FALSE;
      if(next_end<0){
        /*We asked for the rest of the resource.*/
        if(range_length>=0){
          /*Quit if we didn't get it.*/
          if(OP_UNLIKELY(range_last!=range_length))return OP_FALSE;
        }
        /*If there was no length, use the end of the range.*/
        else range_length=range_last;
        next_end=range_last;
      }
      else{
        if(range_last!=next_end)return OP_FALSE;
        /*If there was no length, use the larger of the content length or the
           end of this chunk.*/
        if(range_length<0){
          range_length=OP_MAX(range_last,_stream->content_length);
        }
      }
    }
    else if(strcmp(header,"content-length")==0){
      opus_int64 content_length;
      /*Validate the Content-Length header, if present, against the request we
         made.*/
      content_length=op_http_parse_content_length(cdr);
      if(OP_UNLIKELY(content_length<0))return (int)content_length;
      if(next_end<0){
        /*If we haven't seen the Content-Range header yet and we asked for the
            rest of the resource, set next_end, so we can make sure they match
            when we do find the Content-Range header.*/
        if(OP_UNLIKELY(next_pos>OP_INT64_MAX-content_length))return OP_FALSE;
        next_end=next_pos+content_length;
      }
      /*Otherwise, make sure they match now.*/
      else if(OP_UNLIKELY(next_end-next_pos!=content_length))return OP_FALSE;
    }
    else if(strcmp(header,"connection")==0){
      ret=op_http_parse_connection(cdr);
      if(OP_UNLIKELY(ret<0))return ret;
      /*If the server told us it was going to close the connection, don't make
         any more requests.*/
      if(OP_UNLIKELY(ret>0))_conn->nrequests_left=0;
    }
  }
  /*No Content-Range header.*/
  if(OP_UNLIKELY(range_length<0))return OP_FALSE;
  /*Update the content_length if necessary.*/
  _stream->content_length=range_length;
  _conn->pos=next_pos;
  _conn->end_pos=next_end;
  _conn->next_pos=-1;
  return 0;
}

/*Open a new connection that will start reading at byte offset _pos.
  _pos:        The byte offset to start readiny from.
  _chunk_size: The number of bytes to ask for in the initial request, or -1 to
                request the rest of the resource.
               This may be more bytes than remain, in which case it will be
                converted into a request for the rest.*/
static int op_http_conn_open_pos(OpusHTTPStream *_stream,
 OpusHTTPConn *_conn,opus_int64 _pos,opus_int32 _chunk_size){
  struct timeb  start_time;
  struct timeb  end_time;
  opus_int32    connect_rate;
  opus_int32    connect_time;
  int           ret;
  ret=op_http_connect(_stream,_conn,&_stream->addr_info,&start_time);
  if(OP_UNLIKELY(ret<0))return ret;
  ret=op_http_conn_send_request(_stream,_conn,_pos,_chunk_size,0);
  if(OP_UNLIKELY(ret<0))return ret;
  ret=op_http_conn_handle_response(_stream,_conn);
  if(OP_UNLIKELY(ret!=0))return OP_FALSE;
  ret=ftime(&end_time);
  OP_ASSERT(!ret);
  _stream->cur_conni=_conn-_stream->conns;
  OP_ASSERT(_stream->cur_conni>=0&&_stream->cur_conni<OP_NCONNS_MAX);
  /*The connection has been successfully opened.
    Update the connection time estimate.*/
  connect_time=op_time_diff_ms(&end_time,&start_time);
  connect_rate=_stream->connect_rate;
  connect_rate+=OP_MAX(connect_time,1)-connect_rate+8>>4;
  _stream->connect_rate=connect_rate;
  return 0;
}

/*Read data from the current response body.
  If we're pipelining and we get close to the end of this response, queue
   another request.
  If we've reached the end of this response body, parse the next response and
   keep going.
  [out] _buf: Returns the data read.
  _size:      The size of the buffer.
  _blocking:  Whether or not to block until some data is retrieved.*/
static ptrdiff_t op_http_conn_read_body(OpusHTTPStream *_stream,
 OpusHTTPConn *_conn,char *_buf,ptrdiff_t _size,int _blocking){
  opus_int64 pos;
  opus_int64 end_pos;
  opus_int64 next_pos;
  opus_int64 content_length;
  ptrdiff_t  nread;
  int        pipeline;
  int        ret;
  /*Currently this function can only be called on the LRU head.
    Otherwise, we'd need a _pnext pointer if we needed to close the connection,
     and re-opening it would re-organize the lists.*/
  OP_ASSERT(_stream->lru_head==_conn);
  /*If we try an empty read, we won't be able to tell if we hit an error.*/
  OP_ASSERT(_size>0);
  pos=_conn->pos;
  end_pos=_conn->end_pos;
  next_pos=_conn->next_pos;
  pipeline=_stream->pipeline;
  content_length=_stream->content_length;
  if(end_pos>=0){
    /*Have we reached the end of the current response body?*/
    if(pos>=end_pos){
      OP_ASSERT(content_length>=0);
      /*If this was the end of the stream, we're done.
        Also return early if a non-blocking read was requested (regardless of
         whether we might be able to parse the next response without
         blocking).*/
      if(content_length<=end_pos||!_blocking)return 0;
      /*Otherwise, start on the next response.*/
      if(next_pos<0){
        /*We haven't issued another request yet.*/
        if(!pipeline||_conn->nrequests_left<=0){
          /*There are two ways to get here: either the server told us it was
             going to close the connection after the last request, or we
             thought we were reading the whole resource, but it grew while we
             were reading it.
            The only way the latter could have happened is if content_length
             changed while seeking.
            Open a new request to read the rest.*/
          OP_ASSERT(_stream->seekable);
          /*Try to open a new connection to read another chunk.*/
          op_http_conn_close(_stream,_conn,&_stream->lru_head,1);
          /*If we're not pipelining, we should be requesting the rest.*/
          OP_ASSERT(pipeline||_conn->chunk_size==-1);
          ret=op_http_conn_open_pos(_stream,_conn,end_pos,_conn->chunk_size);
          if(OP_UNLIKELY(ret<0))return 0;
        }
        else{
          /*Issue the request now (better late than never).*/
          ret=op_http_conn_send_request(_stream,_conn,pos,_conn->chunk_size,0);
          if(OP_UNLIKELY(ret<0))return 0;
          next_pos=_conn->next_pos;
          OP_ASSERT(next_pos>=0);
        }
      }
      if(next_pos>=0){
        /*We shouldn't be trying to read past the current request body if we're
           seeking somewhere else.*/
        OP_ASSERT(next_pos==end_pos);
        ret=op_http_conn_handle_response(_stream,_conn);
        if(OP_UNLIKELY(ret<0))return 0;
        if(OP_UNLIKELY(ret>0)&&pipeline){
          opus_int64 next_end;
          next_end=_conn->next_end;
          /*Our request timed out or the server closed the connection.
            Try re-connecting.*/
          op_http_conn_close(_stream,_conn,&_stream->lru_head,1);
          /*Unless there's a bug, we should be able to convert
             (next_pos,next_end) into valid (_pos,_chunk_size) parameters.*/
          OP_ASSERT(next_end<0
           ||next_end-next_pos>=0&&next_end-next_pos<=0x7FFFFFFF);
          ret=op_http_conn_open_pos(_stream,_conn,next_pos,
           next_end<0?-1:(opus_int32)(next_end-next_pos));
          if(OP_UNLIKELY(ret<0))return 0;
        }
        else if(OP_UNLIKELY(ret!=0))return OP_FALSE;
      }
      pos=_conn->pos;
      end_pos=_conn->end_pos;
      content_length=_stream->content_length;
    }
    OP_ASSERT(end_pos>pos);
    _size=OP_MIN(_size,end_pos-pos);
  }
  nread=op_http_conn_read(_conn,_buf,_size,_blocking);
  pos+=nread;
  _conn->pos=pos;
  OP_ASSERT(end_pos<0||content_length>=0);
  /*TODO: If nrequests_left<=0, we can't make a new request, and there will be
     a big pause after we hit the end of the chunk while we open a new
     connection.
    It would be nice to be able to start that process now, but we have no way
     to do it in the background without blocking (even if we could start it, we
     have no guarantee the application will return control to us in a
     sufficiently timely manner to allow us to complete it, and this is
     uncommon enough that it's not worth using threads just for this).*/
  if(end_pos>=0&&end_pos<content_length&&next_pos<0
   &&pipeline&&OP_LIKELY(_conn->nrequests_left>0)){
    opus_int64 request_thresh;
    opus_int32 chunk_size;
    /*Are we getting close to the end of the current response body?
      If so, we should request more data.*/
    request_thresh=_stream->connect_rate*_conn->read_rate>>12;
    /*But don't commit ourselves too quickly.*/
    chunk_size=_conn->chunk_size;
    if(chunk_size>=0)request_thresh=OP_MIN(chunk_size>>2,request_thresh);
    if(end_pos-pos<=request_thresh){
      ret=op_http_conn_send_request(_stream,_conn,end_pos,_conn->chunk_size,1);
      if(OP_UNLIKELY(ret<0))return 0;
    }
  }
  return nread;
}

static size_t op_http_stream_read(void *_ptr,size_t _size,size_t _nmemb,
 void *_stream){
  OpusHTTPStream *stream;
  ptrdiff_t       nread;
  ptrdiff_t       total;
  opus_int64      size;
  opus_int64      pos;
  int             ci;
  stream=(OpusHTTPStream *)_stream;
  total=_size*_nmemb;
  /*Check for overflow/empty read.*/
  if(total==0||total/_size!=_nmemb||total>OP_INT64_MAX)return 0;
  ci=stream->cur_conni;
  /*No current connection => EOF.*/
  if(ci<0)return 0;
  pos=stream->conns[ci].pos;
  size=stream->content_length;
  /*Check for EOF.*/
  if(size>=0){
    if(pos>=size)return 0;
    /*Check for a short read.*/
    if(total>size-pos){
      _nmemb=(size-pos)/_size;
      total=_size*_nmemb;
    }
  }
  if(_size==1){
    nread=op_http_conn_read_body(stream,stream->conns+ci,_ptr,total,1);
  }
  else{
    ptrdiff_t n;
    nread=0;
    /*libopusfile doesn't read multi-byte items, but our abstract stream API
       requires it for stdio compatibility.
      Implement it for completeness' sake by reading individual items one at a
       time.*/
    do{
      ptrdiff_t nread_item;
      nread_item=0;
      do{
        /*Block on the first item, or if we've gotten a partial item.*/
        n=op_http_conn_read_body(stream,stream->conns+ci,
         _ptr,_size-nread_item,nread==0||nread_item>0);
        nread_item+=n;
      }
      while(n>0&&nread_item<(ptrdiff_t)_size);
      /*We can still fail to read a whole item if we encounter an error, or if
         we hit EOF and didn't know the stream length.
        TODO: The former is okay, the latter is not, but I don't know how to
         fix it without buffering arbitrarily large amounts of data.*/
      if(nread_item>=(ptrdiff_t)_size)nread++;
      total-=_size;
    }
    while(n>0&&total>0);
  }
  if(OP_UNLIKELY(nread<=0)){
    /*We either hit an error or EOF.
      Either way, we're done with this connection.*/
    op_http_conn_close(stream,stream->conns+ci,&stream->lru_head,1);
    stream->cur_conni=-1;
    stream->pos=pos;
  }
  return nread;
}

/*Discard data until we reach the _target position.
  _just_read_ahead: Whether or not this is a plain fast-forward.
                    If 0, we need to issue a new request for a chunk at _target
                     and discard all the data from our current request(s).
                    Otherwise, we should be able to reach _target without
                     issuing any new requests.
  _target:          The stream position to which to read ahead.*/
static int op_http_conn_read_ahead(OpusHTTPStream *_stream,
 OpusHTTPConn *_conn,int _just_read_ahead,opus_int64 _target){
  static char dummy_buf[OP_READAHEAD_CHUNK_SIZE];
  opus_int64 pos;
  opus_int64 end_pos;
  opus_int64 next_pos;
  opus_int64 next_end;
  ptrdiff_t  nread;
  int        ret;
  pos=_conn->pos;
  end_pos=_conn->end_pos;
  next_pos=_conn->next_pos;
  next_end=_conn->next_end;
  if(!_just_read_ahead){
    /*We need to issue a new pipelined request.
      This is the only case where we allow more than one outstanding request
       at a time, so we need to reset next_pos (we'll restore it below if we
       did have an outstanding request).*/
    OP_ASSERT(_stream->pipeline);
    _conn->next_pos=-1;
    ret=op_http_conn_send_request(_stream,_conn,_target,
     OP_PIPELINE_CHUNK_SIZE,0);
    if(OP_UNLIKELY(ret<0))return ret;
  }
  /*We can reach the target position by reading forward in the current chunk.*/
  if(_just_read_ahead&&(end_pos<0||_target<end_pos))end_pos=_target;
  else if(next_pos>=0){
    opus_int64 next_next_pos;
    opus_int64 next_next_end;
    /*We already have a request outstanding.
      Finish off the current chunk.*/
    while(pos<end_pos){
      nread=op_http_conn_read(_conn,dummy_buf,
       OP_MIN(end_pos-pos,OP_READAHEAD_CHUNK_SIZE),1);
      /*We failed to read ahead.*/
      if(nread<=0)return OP_FALSE;
      pos+=nread;
    }
    OP_ASSERT(pos==end_pos);
    if(_just_read_ahead){
      next_next_pos=next_next_end=-1;
      end_pos=_target;
    }
    else{
      OP_ASSERT(_conn->next_pos==_target);
      next_next_pos=_target;
      next_next_end=_conn->next_end;
      _conn->next_pos=next_pos;
      _conn->next_end=next_end;
      end_pos=next_end;
    }
    ret=op_http_conn_handle_response(_stream,_conn);
    if(OP_UNLIKELY(ret!=0))return OP_FALSE;
    _conn->next_pos=next_next_pos;
    _conn->next_end=next_next_end;
  }
  while(pos<end_pos){
    nread=op_http_conn_read(_conn,dummy_buf,
     OP_MIN(end_pos-pos,OP_READAHEAD_CHUNK_SIZE),1);
    /*We failed to read ahead.*/
    if(nread<=0)return OP_FALSE;
    pos+=nread;
  }
  OP_ASSERT(pos==end_pos);
  if(!_just_read_ahead){
    ret=op_http_conn_handle_response(_stream,_conn);
    if(OP_UNLIKELY(ret!=0))return OP_FALSE;
  }
  else _conn->pos=end_pos;
  OP_ASSERT(_conn->pos==_target);
  return 0;
}

static int op_http_stream_seek(void *_stream,opus_int64 _offset,int _whence){
  struct timeb     seek_time;
  OpusHTTPStream  *stream;
  OpusHTTPConn    *conn;
  OpusHTTPConn   **pnext;
  OpusHTTPConn    *close_conn;
  OpusHTTPConn   **close_pnext;
  opus_int64       content_length;
  opus_int64       pos;
  int              pipeline;
  int              ci;
  int              ret;
  stream=(OpusHTTPStream *)_stream;
  if(!stream->seekable)return -1;
  content_length=stream->content_length;
  /*If we're seekable, we should have gotten a Content-Length.*/
  OP_ASSERT(content_length>=0);
  ci=stream->cur_conni;
  pos=ci<0?content_length:stream->conns[ci].pos;
  switch(_whence){
    case SEEK_SET:{
      /*Check for overflow:*/
      if(_offset<0)return -1;
      pos=_offset;
    }break;
    case SEEK_CUR:{
      /*Check for overflow:*/
      if(_offset<-pos||_offset>OP_INT64_MAX-pos)return -1;
      pos+=_offset;
    }break;
    case SEEK_END:{
      /*Check for overflow:*/
      if(_offset>content_length||_offset<content_length-OP_INT64_MAX)return -1;
      pos=content_length-_offset;
    }break;
    default:return -1;
  }
  /*Mark when we deactivated the active connection.*/
  if(ci>=0){
    op_http_conn_read_rate_update(stream->conns+ci);
    *&seek_time=*&stream->conns[ci].read_time;
  }
  else{
    ret=ftime(&seek_time);
    OP_ASSERT(!ret);
  }
  /*If we seeked past the end of the stream, just disable the active
     connection.*/
  if(pos>=content_length){
    stream->cur_conni=-1;
    stream->pos=pos;
    return 0;
  }
  close_pnext=NULL;
  close_conn=NULL;
  pnext=&stream->lru_head;
  conn=stream->lru_head;
  pipeline=stream->pipeline;
  while(conn!=NULL){
    opus_int64 conn_pos;
    opus_int64 end_pos;
    opus_int64 read_ahead_thresh;
    int        available;
    int        just_read_ahead;
    /*If this connection has been dormant too long or has made too many
       requests, close it.
      This is to prevent us from hitting server limits/firewall timeouts.*/
    if(op_time_diff_ms(&seek_time,&conn->read_time)>
     OP_CONNECTION_IDLE_TIMEOUT_MS
     ||conn->nrequests_left<OP_PIPELINE_MIN_REQUESTS){
      op_http_conn_close(stream,conn,pnext,1);
      conn=*pnext;
      continue;
    }
    /*Dividing by 2048 instead of 1000 scales this by nearly 1/2, biasing away
       from connection re-use (and roughly compensating for the lag required to
       reopen the TCP window of a connection that's been idle).
      There's no overflow checking here, because it's vanishingly unlikely, and
       all it would do is cause us to make poor decisions.*/
    read_ahead_thresh=OP_MAX(OP_READAHEAD_THRESH_MIN,
     stream->connect_rate*conn->read_rate>>11);
    available=op_http_conn_estimate_available(conn);
    conn_pos=conn->pos;
    end_pos=conn->end_pos;
    if(conn->next_pos>=0){
      OP_ASSERT(end_pos>=0);
      OP_ASSERT(conn->next_pos==end_pos);
      end_pos=conn->next_end;
    }
    OP_ASSERT(end_pos<0||conn_pos<=end_pos);
    /*Can we quickly read ahead without issuing a new request?*/
    just_read_ahead=conn_pos<=pos&&pos-conn_pos-available<=read_ahead_thresh
     &&(end_pos<0||pos<end_pos);
    if(just_read_ahead||pipeline&&end_pos>=0
     &&end_pos-conn_pos-available<=read_ahead_thresh){
      /*Found a suitable connection to re-use.
        We always attempt to re-use the first suitable connection we find, even
         if another one might require less read-ahead, under the assumption
         more recently used connetions have TCP windows that are open wider.
        This seems to give a slight performance boost over picking the one with
         the shortest estimated read-ahead time.*/
      ret=op_http_conn_read_ahead(stream,conn,just_read_ahead,pos);
      if(OP_UNLIKELY(ret<0)){
        /*The connection might have become stale, so close it and keep going.*/
        op_http_conn_close(stream,conn,pnext,1);
        conn=*pnext;
        continue;
      }
      /*Sucessfully resurrected this connection.*/
      *pnext=conn->next;
      conn->next=stream->lru_head;
      stream->lru_head=conn;
      stream->cur_conni=conn-stream->conns;
      return 0;
    }
    close_pnext=pnext;
    close_conn=conn;
    pnext=&conn->next;
    conn=conn->next;
  }
  /*No suitable connections.
    Open a new one.*/
  if(stream->free_head==NULL){
    /*All connections in use.
      Expire one of them (we should have already picked which one when scanning
       the list).*/
    OP_ASSERT(close_conn!=NULL);
    OP_ASSERT(close_pnext!=NULL);
    op_http_conn_close(stream,close_conn,close_pnext,1);
  }
  OP_ASSERT(stream->free_head!=NULL);
  conn=stream->free_head;
  /*If we can pipeline, only request a chunk of data.
    If we're seeking now, there's a good chance we will want to seek again
     soon, and this avoids committing this connection to reading the rest of
     the stream.
    Particularly with SSL or proxies, issuing a new request on the same
     connection can be substantially faster than opening a new one.
    This also limits the amount of data the server will blast at us on this
     connection if we later seek elsewhere and start reading from a different
     connection.*/
  ret=op_http_conn_open_pos(stream,conn,pos,
   pipeline?OP_PIPELINE_CHUNK_SIZE:-1);
  if(OP_UNLIKELY(ret<0)){
    op_http_conn_close(stream,conn,&stream->lru_head,1);
    return -1;
  }
  return 0;
}

static opus_int64 op_http_stream_tell(void *_stream){
  OpusHTTPStream *stream;
  int             ci;
  stream=(OpusHTTPStream *)_stream;
  ci=stream->cur_conni;
  return ci<0?stream->pos:stream->conns[ci].pos;
}

static int op_http_stream_close(void *_stream){
  OpusHTTPStream *stream;
  stream=(OpusHTTPStream *)_stream;
  if(OP_LIKELY(stream!=NULL)){
    op_http_stream_clear(stream);
    _ogg_free(stream);
  }
  return 0;
}

static const OpusFileCallbacks OP_HTTP_CALLBACKS={
  op_http_stream_read,
  op_http_stream_seek,
  op_http_stream_tell,
  op_http_stream_close
};
#endif

void *op_url_stream_create_with_proxy(OpusFileCallbacks *_cb,const char *_url,
 int _flags,const char *_proxy_host,unsigned _proxy_port,
 const char *_proxy_user,const char *_proxy_pass){
  const char *path;
  /*Check to see if this is a valid file: URL.*/
  path=op_parse_file_url(_url);
  if(path!=NULL){
    char *unescaped_path;
    void *ret;
    unescaped_path=op_string_dup(path);
    if(OP_UNLIKELY(unescaped_path==NULL))return NULL;
    ret=op_fopen(_cb,op_unescape_url_component(unescaped_path),"rb");
    _ogg_free(unescaped_path);
    return ret;
  }
#if defined(OP_ENABLE_HTTP)
  /*If not, try http/https.*/
  else{
    OpusHTTPStream *stream;
    int             ret;
    stream=(OpusHTTPStream *)_ogg_malloc(sizeof(*stream));
    if(OP_UNLIKELY(stream==NULL))return NULL;
    op_http_stream_init(stream);
    ret=op_http_stream_open(stream,_url,_flags,
     _proxy_host,_proxy_port,_proxy_user,_proxy_pass);
    if(OP_UNLIKELY(ret<0)){
      op_http_stream_clear(stream);
      _ogg_free(stream);
      return NULL;
    }
    *_cb=*&OP_HTTP_CALLBACKS;
    return stream;
  }
#else
  (void)_flags;
  (void)_proxy_host;
  (void)_proxy_port;
  (void)_proxy_user;
  (void)_proxy_pass;
  return NULL;
#endif
}

void *op_url_stream_create(OpusFileCallbacks *_cb,const char *_url,int _flags){
  return op_url_stream_create_with_proxy(_cb,_url,_flags,NULL,0,NULL,NULL);
}
