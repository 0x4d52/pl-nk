#if (defined (_WIN32) || defined (_WIN64) || defined (WIN64))
// windows
    #include "../options/include/pa_win_waveformat.h"
    #include "../options/os/win/pa_x86_plain_converters.h"
    #include "../options/os/win/pa_win_coinitialize.h"
    #include "../options/os/win/pa_win_waveformat.c"
    #include "../options/os/win/pa_x86_plain_converters.c"
    #include "../options/os/win/pa_win_coinitialize.c"
    #include "../options/os/win/pa_win_util.c"

    #if PA_USE_DS
        #include "../options/include/pa_win_ds.h"
        #include "../options/hostapi/dsound/pa_win_ds_dynlink.h"
        #include "../options/hostapi/dsound/pa_win_ds_dynlink.c"
        #include "../options/hostapi/dsound/pa_win_ds.c"
    #elif PA_USE_WMME
        #include "../options/include/pa_win_wmme.h"
        #include "../options/hostapi/wmme/pa_win_wmme.c"
    #elif PA_USE_WASAPI
        #include "../options/include/pa_win_wasapi.h"
        #include "../options/hostapi/wasapi/pa_win_wasapi.c"
    #elif PA_USE_WDMKS
        #include "../options/include/pa_win_wdmks.h"
        #include "../options/hostapi/wdmks/pa_win_wdmks.c"
    #else
        #error You need to define one of the preprocessor macros, either: PA_USE_DS, PA_USE_WMME, PA_USE_WASAPI or PA_USE_WDMKS
    #endif
    #include "../options/os/win/pa_win_hostapis.c"
#elif defined (__APPLE_CPP__) || defined(__APPLE_CC__)
// mac os
    #define PA_USE_COREAUDIO 1
    #include "../options/include/pa_mac_core.h"
    #include "../options/os/unix/pa_unix_util.c"
    #include "../options/os/unix/pa_unix_hostapis.c"
    #include "../options/hostapi/coreaudio/pa_mac_core_blocking.c"
    #include "../options/hostapi/coreaudio/pa_mac_core_utilities.c"
    #include "../options/hostapi/coreaudio/pa_mac_core.c"
#elif defined (__linux__)
// linux
    #include "../options/os/unix/pa_unix_util.c"
    #include "../options/os/unix/pa_unix_hostapis.c"
    #if PA_USE_OSS
        #include "../options/hostapi/oss/pa_unix_oss.c"
    #elif PA_USE_ALSA
        #include "../options/include/pa_linux_alsa.h"
        #include "../options/hostapi/alsa/pa_linux_alsa.c"
    #elif PA_USE_JACK
        #include "../options/include/pa_jack.h"
        #include "../options/hostapi/jack/pa_jack.c"
    #else
        #error You need to define one of the preprocessor macros, either: PA_USE_OSS, PA_USE_ALSA or PA_USE_JACK
    #endif
#endif