// EMERGENT GAME TECHNOLOGIES PROPRIETARY INFORMATION
//
// This software is supplied under the terms of a license agreement or
// nondisclosure agreement with Emergent Game Technologies and may not 
// be copied or disclosed except in accordance with the terms of that 
// agreement.
//
//      Copyright (c) 1996-2007 Emergent Game Technologies.
//      All Rights Reserved.
//
// Emergent Game Technologies, Chapel Hill, North Carolina 27517
// http://www.emergent.net

#include "NiStripifyInterface.h"

NiStripifyInterface::Platform_Settings 
NiStripifyInterface::ms_akDefaultSettings[] =
{
    { PLATFORM_WIN32,  ALGORITHM_NG, 24,   STITCH_ENABLED,  0, false },
    { PLATFORM_NGC,    ALGORITHM_NI, 24,   STITCH_ENABLED,  0, false },
    { PLATFORM_PS2,    ALGORITHM_NI, 1024, STITCH_DISABLED, 0, false },
    { PLATFORM_XBOX,   ALGORITHM_NG, 24,   STITCH_ENABLED,  0, false }
};

//---------------------------------------------------------------------------
NiStripifyInterface::NiStripifyInterface(Platform ePlatform)
{
    NIASSERT(ePlatform < PLATFORM_COUNT);

    m_kSettings = ms_akDefaultSettings[ePlatform];

    NIASSERT(m_kSettings.m_ePlatform == ePlatform);
}
//---------------------------------------------------------------------------
void NiStripifyInterface::Stripify_Object(NiNode* pkNode)
{
    switch (m_kSettings.m_eAlgorithm)
    {
    case ALGORITHM_NI:
        NiStripify::SetUse_NextGeneration(false);        
        NiStripify::Stripify(pkNode);
        break;
    case ALGORITHM_NG:
        Stripify_NextGeneration(pkNode);
        break;
    default:
        NIASSERT(0);
        break;
    }
}
//---------------------------------------------------------------------------
#if defined(_STRIPIFY_USE_NVIDIA_)
//---------------------------------------------------------------------------
void NiStripifyInterface::Stripify_NextGeneration(NiNode* pkNode)
{
    NiStripify::SetUse_NextGeneration(true);

    //  Set the nVidia variables...
    SetCacheSize(m_kSettings.m_uiCacheSize);

#if defined(ALLOW_NO_STITCHING)
    if (m_kSettings.m_eStitch == STITCH_ENABLED)
        SetStitchStrips(true);
    else
        SetStitchStrips(false);
#else   //#if defined(ALLOW_NO_STITCHING)
    SetStitchStrips(true);
    #if defined(_STITCH_WARNING_ENABLED_)
        if (m_kSettings.m_eStitch != STITCH_ENABLED)
        {
            NiMessageBox("No Stitching is not currently supported.\n"
                "Defaulting to stitching strips together.", "WARNING");
        }
    #endif  //#if defined(_STITCH_WARNING_ENABLED_)
#endif  //#if defined(ALLOW_NO_STITCHING)

    SetMinStripSize(0);
    SetListsOnly(false);

    NiStripify::Stripify(pkNode);
}
//---------------------------------------------------------------------------
#else   //#if defined(_STRIPIFY_USE_NVIDIA_)
//---------------------------------------------------------------------------
void NiStripifyInterface::Stripify_NextGeneration(NiNode* pkNode)
{
    NiStripify::SetUse_NextGeneration(true);
    NiStripify::Stripify(pkNode);
}
//---------------------------------------------------------------------------
#endif  //#if defined(_STRIPIFY_USE_NVIDIA_)
