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

#include "NiMakePropertyUniqueCommand.h"

//---------------------------------------------------------------------------
void NiMakePropertyUniqueCommand::AddReference()
{
    this->IncRefCount();
}
//---------------------------------------------------------------------------
void NiMakePropertyUniqueCommand::RemoveReference()
{
    this->DecRefCount();
}
//---------------------------------------------------------------------------
NiFixedString NiMakePropertyUniqueCommand::GetName()
{
    return m_kCommandName;
}
//---------------------------------------------------------------------------
void NiMakePropertyUniqueCommand::DoCommand(NiEntityErrorInterface* pkErrors,
    bool bUndoable)
{
    NIVERIFY(m_spEntityPropertyInterface->MakePropertyUnique(
        m_kPropertyName, m_bMadeUnique));
}
//---------------------------------------------------------------------------
void NiMakePropertyUniqueCommand::UndoCommand(
    NiEntityErrorInterface* pkErrors)
{
    if (m_bMadeUnique)
    {
        NIVERIFY(m_spEntityPropertyInterface->ResetProperty(
            m_kPropertyName));
    }
}
//---------------------------------------------------------------------------
