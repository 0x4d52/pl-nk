#if (defined (_WIN32) || defined (_WIN64) || defined (WIN64))
// windows

#elif defined (__APPLE_CPP__) || defined(__APPLE_CC__)
// mac os
    #define PA_USE_COREAUDIO 1
    #include "../options/include/pa_mac_core.h"
    #include "../options/os/unix/pa_unix_util.c"
    #include "../options/os/unix/pa_unix_hostapis.c"
    #include "../options/hostapi/coreaudio/pa_mac_core_blocking.c"
    #include "../options/hostapi/coreaudio/pa_mac_core_utilities.c"
    #include "../options/hostapi/coreaudio/pa_mac_core.c"
#endif