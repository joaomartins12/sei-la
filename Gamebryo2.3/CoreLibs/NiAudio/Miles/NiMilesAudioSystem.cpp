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

// Precompiled Header
#include "NiMilesAudioPCH.h"

#include <NiSystem.h>
#include <NiVersion.h>
#include "NiMilesListener.h"
#include "NiMilesAudioSystem.h"
#include "NiMilesSource.h"
#include <NiBool.h>
#include <NiMaterialProperty.h>
#include <NiNode.h>
#include <NiTriShape.h>
#include <Mss.h>

#ifdef _XENON
#include <xaudio.h>
#endif
#ifdef _PS3
#include <cell/audio.h>
#include <sysutil/sysutil_sysparam.h>
#include <sdk_version.h>
#endif

//---------------------------------------------------------------------------
// The following copyright notice may not be removed.
static char EmergentCopyright[] NI_UNUSED =
"Copyright 2007 Emergent Game Technologies";
//---------------------------------------------------------------------------
static char acGamebryoVersion[] NI_UNUSED =
    GAMEBRYO_MODULE_VERSION_STRING("NiAudio");
//---------------------------------------------------------------------------

NiImplementRTTI(NiMilesAudioSystem, NiAudioSystem);

//---------------------------------------------------------------------------
NiMilesAudioSystem* NiMilesAudioSystem::GetAudioSystem()
{
    return (NiMilesAudioSystem*)ms_pAudioSystem;
}
//---------------------------------------------------------------------------
NiMilesAudioSystem::NiMilesAudioSystem() :
    m_uFlags(0),
    m_kWnd(0),
    m_pDIG(0)
{
    m_fUnitsPerMeter = 1.0f;     // set units to meters
    m_spListener = NiNew NiMilesListener;
    NIASSERT(m_spListener);
    NIASSERT(m_pSources);
}
//---------------------------------------------------------------------------
NiMilesAudioSystem::~NiMilesAudioSystem()
{
    Shutdown();
}
//---------------------------------------------------------------------------
void NiMilesAudioSystem::SetHWnd(NiWindowRef kWnd)
{
    m_kWnd = kWnd;
}
//---------------------------------------------------------------------------
NiWindowRef NiMilesAudioSystem::GetHWnd()
{
    return m_kWnd;
}
//---------------------------------------------------------------------------
bool NiMilesAudioSystem::Startup(const char* pcDirectoryname)
{
#if defined(_XENON)
    Register_RIB(XMADec);
    Register_RIB(MP3Dec);

    XAUDIOENGINEINIT EngineInit = { 0 };
    EngineInit.pEffectTable = &XAudioDefaultEffectTable;
    XAudioInitialize(&EngineInit);

#elif defined(_PS3)
    Register_RIB(MP3Dec);

    int iReturn = cellAudioInit();
    if (iReturn != CELL_OK)
    {
        return false;
    }

    int iNumSpeakers;
    CellAudioOutConfiguration a_config;
    int i;
    int dev;
    CellAudioOutDeviceInfo a_info;

    a_config.channel = 2;

    dev = cellAudioOutGetNumberOfDevice(CELL_AUDIO_OUT_PRIMARY);
    for (i = 0; i < dev; i++)
    {
        iReturn = cellAudioOutGetDeviceInfo(CELL_AUDIO_OUT_PRIMARY, i, &a_info);
        if (iReturn == CELL_OK)
        {
            if (a_info.portType == CELL_AUDIO_OUT_PORT_HDMI)
            {
                int j;
                for (j = 0; j < 16; j++)
                {
#if (CELL_SDK_VERSION >= 0x090002)
                    if ((a_info.availableModes[j].type ==
                        CELL_AUDIO_OUT_CODING_TYPE_LPCM) &&
                        (a_info.availableModes[j].fs &
                            CELL_AUDIO_OUT_FS_48KHZ))
#else
                    if ((a_info.availableModes[j].type ==
                        CELL_AUDIO_CODING_TYPE_LPCM) &&
                        (a_info.availableModes[j].fs & CELL_AUDIO_FS_48KHZ))
#endif
                    {
                        if (a_config.channel <
                            a_info.availableModes[j].channel)
                        {
                            a_config.channel =
                                a_info.availableModes[j].channel;
                        }
                    }
                }
            }
        }
    }

#if (CELL_SDK_VERSION >= 0x090002)
    a_config.downMixer = CELL_AUDIO_OUT_DOWNMIXER_NONE;
#else
    a_config.downMixer = CELL_AUDIO_DOWNMIXER_NONE;
#endif

    iReturn = cellAudioOutConfigure(CELL_AUDIO_OUT_PRIMARY, &a_config, NULL, 0);
    if (iReturn != CELL_OK)
    {
        iNumSpeakers = 2;
    }

    iNumSpeakers = a_config.channel;

    AIL_startup();

    m_pDIG = AIL_open_digital_driver(48000, 16, iNumSpeakers, 0);

#else
    AIL_set_redist_directory(pcDirectoryname);
#endif

#ifndef _PS3
    if (!AIL_quick_startup(1, 0, 44100, 16, MSS_MC_USE_SYSTEM_CONFIG))
        return false;

    AIL_quick_handles((HDIGDRIVER*)&m_pDIG, NULL, NULL);
#endif

    return true;
}
//---------------------------------------------------------------------------
void NiMilesAudioSystem::Shutdown()
{
    if (m_pSources)
    {
        NiTListIterator pos = m_pSources->GetHeadPos();
        while (pos)
        {
            NiMilesSource* pSource = (NiMilesSource*)m_pSources->GetNext(pos);
            pSource->Unload();
        }

        m_pSources->RemoveAll();
    }

    if (m_spListener)
        NiSmartPointerCast(NiMilesListener, m_spListener)->Release();

    if (m_pDIG)
    {
        AIL_close_digital_driver((HDIGDRIVER)m_pDIG);
        m_pDIG = 0;
    }

    AIL_quick_shutdown();
    AIL_shutdown();

#ifdef _XENON
    XAudioShutDown();
#endif

#ifdef _PS3
    cellAudioQuit();
#endif
}
//---------------------------------------------------------------------------
NiMilesListener* NiMilesAudioSystem::GetListener()
{
    return (NiMilesListener*)NiAudioSystem::GetListener();
}
//---------------------------------------------------------------------------
NiAudioSource* NiMilesAudioSystem::CreateSource(
    unsigned int uiType /* = NiAudioSource::TYPE_DEFAULT*/)
{
    return (NiAudioSource*)NiNew NiMilesSource(uiType);
}
//---------------------------------------------------------------------------
NiAudioSystem::SpeakerType NiMilesAudioSystem::GetSpeakerType()
{
    return NiAudioSystem::TYPE_3D_2_SPEAKER;
}
//---------------------------------------------------------------------------
bool NiMilesAudioSystem::SetSpeakerType(unsigned int uiType)
{
    return false;
}
//---------------------------------------------------------------------------
bool NiMilesAudioSystem::SetBestSpeakerTypeAvailable()
{
    return false;
}
//---------------------------------------------------------------------------
char* NiMilesAudioSystem::GetLastError()
{
    char* ptemp = AIL_last_error();
    return ptemp;
}
//---------------------------------------------------------------------------
NiMilesSource* NiMilesAudioSystem::GetFirstSource(NiTListIterator& iter)
{
    return (NiMilesSource*)NiAudioSystem::GetFirstSource(iter);
}
//---------------------------------------------------------------------------
NiMilesSource* NiMilesAudioSystem::GetNextSource(NiTListIterator& iter)
{
    return (NiMilesSource*)NiAudioSystem::GetNextSource(iter);
}
//---------------------------------------------------------------------------
NiMilesSource* NiMilesAudioSystem::FindDuplicateSource(
    NiAudioSource* pkOriginal)
{
    NiTListIterator iter;
    NiAudioSource* pkSource = GetFirstSource(iter);

    while (pkSource)
    {
        if ((pkOriginal != pkSource) && pkSource->GetAllowSharing() &&
            (pkSource->GetType() == pkOriginal->GetType()) &&
            pkSource->GetFilename() &&
            (!strcmp(pkOriginal->GetFilename(), pkSource->GetFilename())))
        {
            return (NiMilesSource*)pkSource;
        }

        pkSource = GetNextSource(iter);
    }

    return NULL;
}
//---------------------------------------------------------------------------
bool NiMilesAudioSystem::IsUniqueSource(NiMilesSource* pkOriginal)
{
    NiTListIterator iter;
    NiMilesSource* pkSource = (NiMilesSource*)GetFirstSource(iter);

    while (pkSource)
    {
        if ((pkOriginal != pkSource) &&
            pkOriginal->GetFileImage() == pkSource->GetFileImage())
        {
            return false;
        }

        pkSource = (NiMilesSource*)GetNextSource(iter);
    }

    return true;
}
//---------------------------------------------------------------------------
bool NiMilesAudioSystem::SetUnitsPerMeter(float fUnits)
{
    if (!NiAudioSystem::SetUnitsPerMeter(fUnits))
        return false;

    AIL_set_3D_distance_factor((HDIGDRIVER)m_pDIG, 1.0f / fUnits);

    return true;
}
//---------------------------------------------------------------------------
void NiMilesAudioSystem::SetDriverProperty(char* cPreferenceName,
    void* PreferenceValue)
{
    AIL_output_filter_driver_property((HDIGDRIVER)m_pDIG, cPreferenceName,
        0, PreferenceValue, 0);
}
//---------------------------------------------------------------------------
void NiMilesAudioSystem::GetDriverProperty(char* cPreferenceName,
    void* PreferenceValue)
{
    AIL_output_filter_driver_property((HDIGDRIVER)m_pDIG, cPreferenceName,
        PreferenceValue, 0, 0);
}
//---------------------------------------------------------------------------
bool NiMilesAudioSystem::GetReverbAvailable()
{
    if (!m_pDIG)
        return false;

    // Miles SDK newer signature:
    // AIL_room_type(HDIGDRIVER, S32 room/speaker index)
    return (AIL_room_type((HDIGDRIVER)m_pDIG, 0) != -1);
}
//---------------------------------------------------------------------------
bool NiMilesAudioSystem::SetCurrentRoomReverb(unsigned int dwPreset)
{
    if (!m_pDIG)
        return false;

    if (!GetReverbAvailable())
        return false;

    // Miles SDK newer signature:
    // AIL_set_room_type(HDIGDRIVER, S32 room_type, F32 change_time)
    // 0.0f = immediate.
    AIL_set_room_type((HDIGDRIVER)m_pDIG, (S32)dwPreset, 0.0f);

    return true;
}
//---------------------------------------------------------------------------
unsigned int NiMilesAudioSystem::GetCurrentRoomReverb()
{
    if (!m_pDIG)
        return 0;

    // Miles SDK newer signature:
    // AIL_room_type(HDIGDRIVER, S32 room/speaker index)
    return (unsigned int)AIL_room_type((HDIGDRIVER)m_pDIG, 0);
}
//---------------------------------------------------------------------------
void* NiMilesAudioSystem::GetDigitalDriver()
{
    return m_pDIG;
}
//---------------------------------------------------------------------------
// Streaming
//---------------------------------------------------------------------------
NiObject* NiMilesAudioSystem::CreateObject()
{
    return NiMilesAudioSystem::GetAudioSystem();
}
//---------------------------------------------------------------------------
void NiMilesAudioSystem::LoadBinary(NiStream& stream)
{
    NiObject::LoadBinary(stream);

    float fVal1;
    NiStreamLoadBinary(stream, fVal1);
    SetUnitsPerMeter(fVal1);

    NiStreamLoadBinary(stream, m_uFlags);
}
//---------------------------------------------------------------------------
void NiMilesAudioSystem::LinkObject(NiStream& stream)
{
    NiObject::LinkObject(stream);
}
//---------------------------------------------------------------------------
bool NiMilesAudioSystem::RegisterStreamables(NiStream& stream)
{
    if (!NiObject::RegisterStreamables(stream))
        return false;

    return true;
}
//---------------------------------------------------------------------------
void NiMilesAudioSystem::SaveBinary(NiStream& stream)
{
    NiObject::SaveBinary(stream);
    NiStreamSaveBinary(stream, m_fUnitsPerMeter);
    NiStreamSaveBinary(stream, m_uFlags);
}
//---------------------------------------------------------------------------
bool NiMilesAudioSystem::IsEqual(NiObject* pObject)
{
    if (!NiAudioSystem::IsEqual(pObject))
        return false;

    NiMilesAudioSystem* pSS = (NiMilesAudioSystem*)pObject;
    return true;
}
//---------------------------------------------------------------------------
const unsigned int NiMilesAudioSystem::GetNumberActiveSamples()
{
    return AIL_active_sample_count((HDIGDRIVER)m_pDIG);
}
//---------------------------------------------------------------------------