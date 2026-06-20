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

#ifndef NIFXLTECHNIQUEDESC_H
#define NIFXLTECHNIQUEDESC_H

#include "NiFXLShaderLibType.h"
#include <NiGeometryData.h>

class NIFXLSHADER_ENTRY NiFXLTechniqueDesc : public NiMemObject
{
public:
    static NiFXLTechniqueDesc* GetDesc();

    const char* m_pcName;
    const char* m_pcDesc;

    unsigned int m_uiImplementation;
    unsigned int m_uiPSVersion;
    unsigned int m_uiVSVersion;
    unsigned int m_uiUserVersion;

    unsigned int m_uiBonesPerPartition;
    NiGeometryData::DataFlags m_eNBTMethod;

    bool m_bUsesNiRenderState;
    bool m_bUsesNiLightState;
protected:
    static NiFXLTechniqueDesc ms_kTechniqueDesc;
};

#endif //NIFXLTECHNIQUEDESC_H
