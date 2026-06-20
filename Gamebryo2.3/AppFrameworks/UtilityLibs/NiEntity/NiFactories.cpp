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
#include "NiEntityPCH.h"

#include "NiTFactory.h"
#include "NiEntityStreaming.h"
#include "NiExternalAssetHandler.h"
#include "NiEntityPropertyInterface.h"
#include "NiFactories.h"

#include "NiExternalAssetKFHandler.h"
#include "NiExternalAssetNIFHandler.h"
#include "NiExternalAssetKFMHandler.h"

#include "NiActorComponent.h"
#include "NiCameraComponent.h"
#include "NiGeneralComponent.h"
#include "NiInheritedTransformationComponent.h"
#include "NiLightComponent.h"
#include "NiSceneGraphComponent.h"
#include "NiShadowGeneratorComponent.h"
#include "NiTransformationComponent.h"
#include "NiGeneralEntity.h"

#include <NiMain.h>

// Declare streaming handlers
NiFactoryDeclareStreaming(NiEntityStreamingAscii);

// Declare asset handlers
NiFactoryDeclareAssetHandler(NiExternalAssetKFHandler);
NiFactoryDeclareAssetHandler(NiExternalAssetNIFHandler);
NiFactoryDeclareAssetHandler(NiExternalAssetKFMHandler);

// Declare classes
NiFactoryDeclarePropIntf(NiActorComponent);
NiFactoryDeclarePropIntf(NiCameraComponent);
NiFactoryDeclarePropIntf(NiGeneralComponent);
NiFactoryDeclarePropIntf(NiInheritedTransformationComponent);
NiFactoryDeclarePropIntf(NiLightComponent);
NiFactoryDeclarePropIntf(NiSceneGraphComponent);
NiFactoryDeclarePropIntf(NiShadowGeneratorComponent);
NiFactoryDeclarePropIntf(NiTransformationComponent);
NiFactoryDeclarePropIntf(NiGeneralEntity);

NiTFactory<NiEntityStreaming*>* NiFactories::ms_pkStreamingFactory = NULL;
NiTFactory<NiExternalAssetHandler*>* NiFactories::ms_pkAssetFactory = NULL;
NiTFactory<NiEntityPropertyInterface*>* NiFactories::ms_pkEntityCompFactory =
    NULL;

//---------------------------------------------------------------------------
NiTFactory<NiEntityStreaming*>* NiFactories::GetStreamingFactory()
{
    return ms_pkStreamingFactory;
}
//---------------------------------------------------------------------------
NiTFactory<NiExternalAssetHandler*>* NiFactories::GetAssetFactory()
{
    return ms_pkAssetFactory;
}
//---------------------------------------------------------------------------
NiTFactory<NiEntityPropertyInterface*>* NiFactories::GetEntityCompFactory()
{
    return ms_pkEntityCompFactory;
}
//---------------------------------------------------------------------------
void NiFactories::_SDMInit()
{
    // Create streaming Factory and creators....
    ms_pkStreamingFactory = NiNew NiTFactory<NiEntityStreaming*>;
    
    // Create Asset Factory and creators    
    ms_pkAssetFactory = NiNew NiTFactory<NiExternalAssetHandler*>;
    
    // Create entity and component factory and creators
    ms_pkEntityCompFactory = NiNew NiTFactory<NiEntityPropertyInterface*>;
}
//---------------------------------------------------------------------------
void NiFactories::_SDMRegister()
{
    NiFactoryRegisterStreaming(NiEntityStreamingAscii);

    NiFactoryRegisterAssetHandler(NiExternalAssetKFHandler);
    NiFactoryRegisterAssetHandler(NiExternalAssetNIFHandler);
    NiFactoryRegisterAssetHandler(NiExternalAssetKFMHandler);

    NiFactoryRegisterPropIntf(NiActorComponent);
    NiFactoryRegisterPropIntf(NiCameraComponent);
    NiFactoryRegisterPropIntf(NiGeneralComponent);
    NiFactoryRegisterPropIntf(NiInheritedTransformationComponent);
    NiFactoryRegisterPropIntf(NiLightComponent);
    NiFactoryRegisterPropIntf(NiSceneGraphComponent);
    NiFactoryRegisterPropIntf(NiShadowGeneratorComponent);
    NiFactoryRegisterPropIntf(NiTransformationComponent);
    NiFactoryRegisterPropIntf(NiGeneralEntity);
}
//---------------------------------------------------------------------------
void NiFactories::_SDMShutdown()
{
    NiDelete ms_pkStreamingFactory;
    NiDelete ms_pkAssetFactory;
    NiDelete ms_pkEntityCompFactory;
}
//---------------------------------------------------------------------------
