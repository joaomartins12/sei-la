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

#ifndef NIFXLSHADERLIBPCH_H
#define NIFXLSHADERLIBPCH_H

#if defined(NI_USE_PCH)

#include <NiMainPCH.h>

// Manually include DX-agnostic D3D headers
#include "NiD3DDefaultShader.h"
#include "NiD3DDefines.h"
#include "NiD3DError.h"
#include "NiGlobalConstantEntry.h"
#include "NiD3DMacros.h"
#include "NiD3DPass.h"
#include "NiD3DPixelShader.h"
#include "NiD3DRenderStateGroup.h"
#include "NiD3DRendererHeaders.h"
#include "NiD3DShader.h"
#include "NiD3DShaderConstantMap.h"
#include "NiShaderConstantMapEntry.h"
#include "NiD3DShaderDeclaration.h"
#include "NiD3DShaderFactory.h"
#include "NiD3DShaderInterface.h"
#include "NiD3DShaderLibrary.h"
#include "NiD3DShaderLibraryInterface.h"
#include "NiD3DShaderLibraryVersion.h"
#include "NiD3DShaderProgram.h"
#include "NiD3DShaderProgramFactory.h"
#include "NiD3DTextureStage.h"
#include "NiD3DTextureStageGroup.h"
#include "NiD3DUtility.h"
#include "NiD3DUtils.h"
#include "NiD3DVertexShader.h"
#include "NiGeometryBufferData.h"
#include "NiMemoryDefines.h"
#include "NiPackerMacros.h"
#include "NiTPool.h"

#include "NiFXLInclude.h"
#include "NiFXLAttributeTable.h"
#include "NiFXLDefines.h"
#include "NiFXLFactory.h"
#include "NiFXLFile.h"
#include "NiFXLLoader.h"
#include "NiFXLParameter.h"
#include "NiFXLParameterDesc.h"
#include "NiFXLParser.h"
#include "NiFXLShader.h"
#include "NiFXLShaderLib.h"
#include "NiFXLShaderLibrary.h"
#include "NiFXLShaderLibType.h"
#include "NiFXLShaderSDM.h"
#include "NiFXLTechnique.h"
#include "NiFXLTechniqueDesc.h"
#include "NiFXLTechniqueSet.h"

#include <Fxl.h>

#endif //#if defined(NI_USE_PCH)

#endif // #ifndef NIFXLSHADERLIBPCH_H
