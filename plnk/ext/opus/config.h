#ifndef CONFIG_H
#define CONFIG_H

#define CELT_BUILD            1
#define OPUS_BUILD            1
//#define FIXED_POINT           1
#define USE_ALLOCA            1

#if (defined (_WIN32) || defined (_WIN64) || defined (WIN64))
// windows

    #define inline __inline
    #define getpid _getpid

    /* Get rid of the CELT VS compile warnings */
    #pragma warning(disable : 4996)

#elif defined (__APPLE_CPP__) || defined(__APPLE_CC__)
// mac os

    #define HAVE_LRINT        1      

#endif



//#include "version.h"
#endif // CONFIG_H
