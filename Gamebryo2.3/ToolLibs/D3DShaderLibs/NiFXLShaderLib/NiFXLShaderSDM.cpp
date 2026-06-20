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
#include "NiFXLShaderLibPCH.h"

//#include "NiD3DXEffectFactory.h"
//#include "NiD3DXEffectLoader.h"
//#include "NiD3DXEffectParameter.h"
//#include "NiFXLShaderLibrary.h"
#include "NiFXLShaderSDM.h"

NiImplementSDMConstructor(NiFXLShader);

//---------------------------------------------------------------------------
void NiFXLShaderSDM::Init()
{
    NiImplementSDMInitCheck();

    NiFXLParser::_SDMInit();
}
//---------------------------------------------------------------------------
void NiFXLShaderSDM::Shutdown()
{
    NiImplementSDMShutdownCheck();

    NiFXLParameter::ReleaseBoneArray();
    NiFXLShaderLibrary::SetDirectoryInfo(0);
    NiFXLLoader::Destroy();
    
    NiFXLParser::_SDMShutdown();
    NiFXLFactory::_SDMShutdown();
}
//---------------------------------------------------------------------------
