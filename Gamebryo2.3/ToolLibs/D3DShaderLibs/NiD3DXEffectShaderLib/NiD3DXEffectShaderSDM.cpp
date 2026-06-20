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
//---------------------------------------------------------------------------
// Precompiled Header
#include "NiD3DXEffectShaderLibPCH.h"

#include "NiD3DXEffectFactory.h"
#include "NiD3DXEffectLoader.h"
#include "NiD3DXEffectParameter.h"
#include "NiD3DXEffectShaderLibrary.h"
#include "NiD3DXEffectShaderSDM.h"

NiImplementSDMConstructor(NiD3DXEffectShader);

//---------------------------------------------------------------------------
void NiD3DXEffectShaderSDM::Init()
{
    NiImplementSDMInitCheck();

    NiD3DXEffectParser::_SDMInit();
}
//---------------------------------------------------------------------------
void NiD3DXEffectShaderSDM::Shutdown()
{
    NiImplementSDMShutdownCheck();

    NiD3DXEffectParser::_SDMShutdown();
    NiD3DXEffectParameter::ReleaseBoneArray();
    NiD3DXEffectShaderLibrary::SetDirectoryInfo(0);
    NiD3DXEffectLoader::Destroy();
    
    NiD3DXEffectFactory::_SDMShutdown();
}
//---------------------------------------------------------------------------
