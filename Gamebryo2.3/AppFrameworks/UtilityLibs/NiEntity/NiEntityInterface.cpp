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

#include "NiEntityInterface.h"

NiFixedString NiEntityInterface::IT_ENTITYINTERFACE;

//---------------------------------------------------------------------------
void NiEntityInterface::_SDMInit()
{
    IT_ENTITYINTERFACE = "Entity Interface";
}
//---------------------------------------------------------------------------
void NiEntityInterface::_SDMShutdown()
{
    IT_ENTITYINTERFACE = NULL;
}
//---------------------------------------------------------------------------
NiFixedString NiEntityInterface::GetInterfaceType()
{
    return IT_ENTITYINTERFACE;
}
//---------------------------------------------------------------------------
