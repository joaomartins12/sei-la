//
// Provides bindings to radss for any platform using genericdig.cpp
//

#include "mss.h"
#include "imssapi.h"
#include "radss.h"

#if defined(__RADNT__)
    #define DEFAULT_OUTPUT RADSS_DSInstallDriver
#elif defined(__RADANDROID__)
    #define DEFAULT_OUTPUT RADSS_SLESInstallDriver
#elif defined(__RADPSP2__)
    #define DEFAULT_OUTPUT RADSS_PSP2InstallDriver
#elif defined(__RADPS4__)
    #define DEFAULT_OUTPUT RADSS_SonyInstallDriver
#elif defined(__RADXENON__) || defined(__RADWINRTAPI__)
    #define DEFAULT_OUTPUT RADSS_XAudio2InstallDriver
#elif defined(__RADWIIU__)
    #define DEFAULT_OUTPUT RADSS_AXInstallDriver
#elif defined(__RADMAC__) || defined(__RADIPHONE__)
    #define DEFAULT_OUTPUT RADSS_CAInstallDriver
#elif defined(__RADLINUX__)
    #define DEFAULT_OUTPUT RADSS_OalInstallDriver
#elif defined(__RADNX__)
    #define DEFAULT_OUTPUT RADSS_NXInstallDriver
#else
    #error No outputs defined for this platform!
#endif

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
HOUTPUTTYPE AIL_get_default_output()
{
    return (HOUTPUTTYPE)DEFAULT_OUTPUT(0, 0);
}

#if defined(__RADMAC__) || defined(__RADIPHONE__)
    DXDEF HOUTPUTTYPE AILEXPORT AIL_output_coreaudio() 
    {
        return (HOUTPUTTYPE)RADSS_CAInstallDriver(0, 0); 
    }
#elif defined(__RADNT__)
    DXDEF HOUTPUTTYPE AILEXPORT AIL_output_directsound(void* device_lpguid, void* window_hwnd)
    {
        return (HOUTPUTTYPE)RADSS_DSInstallDriver((UINTa)device_lpguid, (UINTa)window_hwnd);
    }

    DXDEF HOUTPUTTYPE AILEXPORT AIL_output_waveout()
    {
        return (HOUTPUTTYPE)RADSS_WOInstallDriver(0, 0);
    }
#elif defined(__RADANDROID__)
    DXDEF HOUTPUTTYPE AILEXPORT AIL_output_sles(void* engine_ptr, void* output_mix_ptr)
    {
        return (HOUTPUTTYPE)RADSS_SLESInstallDriver((UINTa)engine_ptr, (UINTa)output_mix_ptr);
    }
#elif defined(__RADLINUX__)
    DXDEF HOUTPUTTYPE AILEXPORT AIL_output_oal(void* shared_context) 
    { 
        return (HOUTPUTTYPE)RADSS_OalInstallDriver((UINTa)shared_context, 0); 
    }
#elif defined(__RADWIIU__)
    DXDEF HOUTPUTTYPE AILEXPORT AIL_output_ax(U32 open_type)
    {
        return (HOUTPUTTYPE)RADSS_AXInstallDriver(open_type, 0);
    }
#endif
