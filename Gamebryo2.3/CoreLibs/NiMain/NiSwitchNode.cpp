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
#include "NiMainPCH.h"

#include "NiBool.h"
#include "NiMainMetrics.h"
#include "NiSwitchNode.h"

NiImplementRTTI(NiSwitchNode, NiNode);

//---------------------------------------------------------------------------
NiSwitchNode::NiSwitchNode() :
    m_uFlags(0),
    m_iIndex(-1),
    m_fSavedTime(0.0f),
    m_uiRevID(1),
    m_kChildRevID(1)
{
    UpdateOnlyActiveChild(true);
    SetUpdateControllers(true);
}
//---------------------------------------------------------------------------
void NiSwitchNode::AttachChild(NiAVObject* pkChild, bool bFirstAvail)
{
    NiNode::AttachChild(pkChild, bFirstAvail);

    m_uiRevID = 1;
    m_kChildRevID.Add(0);
}
//---------------------------------------------------------------------------
NiAVObjectPtr NiSwitchNode::DetachChild(NiAVObject* pkChild)
{
    m_uiRevID = 1;

    for (unsigned int i = 0; i < m_kChildren.GetSize(); i++)
    {
        if (pkChild == m_kChildren.GetAt(i))
        {
            m_kChildRevID.RemoveAt(i);
            break;
        }
    }

    NiAVObjectPtr spObject = NiNode::DetachChild(pkChild);

    // Set the Index to -1 if we removed the current child
    if ((m_iIndex > -1) && ((m_iIndex >= (int)m_kChildren.GetSize()) ||
        (m_kChildren.GetAt(m_iIndex) == NULL)))
    {
        m_iIndex = -1;
    }

    return spObject;
}
//---------------------------------------------------------------------------
NiAVObjectPtr NiSwitchNode::DetachChildAt(unsigned int i)
{
    m_uiRevID = 1;

    if (i < m_kChildren.GetSize())
        m_kChildRevID.RemoveAt(i);

    NiAVObjectPtr spObject = NiNode::DetachChildAt(i);

    // Set the Index to -1 if we removed the current child
    if ((m_iIndex > -1) && ((m_iIndex >= (int)m_kChildren.GetSize()) ||
        (m_kChildren.GetAt(m_iIndex) == NULL)))
    {
        m_iIndex = -1;
    }

    return spObject;
}
//---------------------------------------------------------------------------
NiAVObjectPtr NiSwitchNode::SetAt(unsigned int i, NiAVObject* pkChild)
{
    m_uiRevID = 1;
    m_kChildRevID.SetAtGrow(i, 0);

    return NiNode::SetAt(i, pkChild);
}
//---------------------------------------------------------------------------
void NiSwitchNode::UpdateDownwardPass(float fTime, bool bUpdateControllers)
{
    // NOTE: When changing UpdateDownwardPass, UpdateSelectedDownwardPass,
    // or UpdateRigidDownwardPass, remember to make equivalent changes
    // the all of these functions.

    SetUpdateControllers(bUpdateControllers);

    // Store the update time. This might be needed in Display().
    // Additionally any derived class can use it.
    m_fSavedTime = fTime;

    if (GetUpdateOnlyActiveChild())
    {
        NIMETRICS_MAIN_INCREMENTUPDATES();

        // Used in the Display() routine to check if the active child needs
        // to be updated.
        m_uiRevID++;

        if (bUpdateControllers)
            UpdateObjectControllers(fTime);
        
        UpdateWorldData();
        
        if (m_iIndex < 0)
            return;

        NiAVObject* pkChild = m_kChildren.GetAt(m_iIndex);
        if (pkChild != NULL)
        {
            pkChild->UpdateDownwardPass(fTime, bUpdateControllers);
            m_kChildRevID.SetAt(m_iIndex, m_uiRevID);
            m_kWorldBound = pkChild->GetWorldBound();
        }        
    }
    else
    {
        NiNode::UpdateDownwardPass(fTime, bUpdateControllers);
    }
}
//---------------------------------------------------------------------------
void NiSwitchNode::UpdateSelectedDownwardPass(float fTime)
{
    // NOTE: When changing UpdateDownwardPass, UpdateSelectedDownwardPass,
    // or UpdateRigidDownwardPass, remember to make equivalent changes
    // the all of these functions.

    // Store the update time. This might be needed in Display().
    // Additionally any derived class can use it.
    m_fSavedTime = fTime;

    if (GetUpdateOnlyActiveChild())
    {
        NIMETRICS_MAIN_INCREMENTUPDATES();

        // Used in the Display() routine to check if the active child needs
        // to be updated.
        m_uiRevID++;

        UpdateObjectControllers(fTime,
            GetSelectiveUpdatePropertyControllers());
        
        UpdateWorldData();

        if (m_iIndex < 0)
            return;
        
        NiAVObject* pkChild = m_kChildren.GetAt(m_iIndex);
        if (pkChild != NULL)
        {
            pkChild->DoSelectedUpdate(fTime);
            m_kChildRevID.SetAt(m_iIndex, m_uiRevID);
            m_kWorldBound = pkChild->GetWorldBound();
        }        
    }
    else
    {
        NiNode::UpdateSelectedDownwardPass(fTime);
    }
}
//---------------------------------------------------------------------------
void NiSwitchNode::UpdateRigidDownwardPass(float fTime)
{
    // NOTE: When changing UpdateDownwardPass, UpdateSelectedDownwardPass,
    // or UpdateRigidDownwardPass, remember to make equivalent changes
    // the all of these functions.

    // Store the update time. This might be needed in Display().
    // Additionally any derived class can use it.
    m_fSavedTime = fTime;

    if (GetUpdateOnlyActiveChild())
    {
        NIMETRICS_MAIN_INCREMENTUPDATES();

        // Used in the Display() routine to check if the active child needs
        // to be updated.
        m_uiRevID++;

        UpdateObjectControllers(fTime, 
            GetSelectiveUpdatePropertyControllers());
        
        UpdateWorldData();
        
        if (m_iIndex < 0)
            return;

        NiAVObject* pkChild = m_kChildren.GetAt(m_iIndex);
        if (pkChild != NULL)
        {
            if (pkChild->GetSelectiveUpdate())
                pkChild->UpdateRigidDownwardPass(fTime);

            m_kChildRevID.SetAt(m_iIndex, m_uiRevID);
            m_kWorldBound = pkChild->GetWorldBound();
        }        
    }
    else
    {
        NiNode::UpdateRigidDownwardPass(fTime);
    }
}
//---------------------------------------------------------------------------
void NiSwitchNode::OnVisible(NiCullingProcess& kCuller)
{
    if (m_iIndex >= 0)
    {
        // If the active child was not updated by the Update pass that
        // visited the switch node, it needs to be updated now. To avoid
        // unnecessary updating, a revision ID is checked to insure we only
        // update when necessary.
        NiAVObject* pkChild = m_kChildren.GetAt(m_iIndex);
        if (pkChild)
        {
            if (m_kChildRevID.GetAt(m_iIndex) != m_uiRevID)
            {
                m_kChildRevID.SetAt(m_iIndex, m_uiRevID);
                pkChild->UpdateDownwardPass(m_fSavedTime,
                    GetUpdateControllers());
            }
            
            pkChild->Cull(kCuller);
        }
    }
}
//---------------------------------------------------------------------------
void NiSwitchNode::UpdateControllers(float fTime)
{
    if (GetUpdateOnlyActiveChild())
    {
        if (m_iIndex < 0)
            return;

        UpdateObjectControllers(fTime);

        NiAVObject* pkChild = m_kChildren.GetAt(m_iIndex);
        if (pkChild)
            pkChild->UpdateControllers(fTime);
    }
    else
    {
        NiNode::UpdateControllers(fTime);
    }
}
//---------------------------------------------------------------------------
void NiSwitchNode::UpdateWorldBound()
{
    if (GetUpdateOnlyActiveChild())
    {
        NiAVObject* pkChild;

        if (m_iIndex >= 0 && (pkChild = m_kChildren.GetAt(m_iIndex)))
        {
            m_kWorldBound = pkChild->GetWorldBound();
        }
        else
        {
            m_kWorldBound.SetRadius(0.0f);
        }
    }
    else
    {
        NiNode::UpdateWorldBound();
    }
}
//---------------------------------------------------------------------------
void NiSwitchNode::UpdateNodeBound()
{
    // Recursively sets m_kBound for all nodes in the subtree

    for (unsigned int i = 0; i < m_kChildren.GetSize(); i++)
    {
        NiAVObject* pkChild = m_kChildren.GetAt(i);
        if (pkChild)
        {
            // Update the child if it hasn't been updated
            if (m_kChildRevID.GetAt(i) != m_uiRevID)
            {
                m_kChildRevID.SetAt(i, m_uiRevID);
                pkChild->UpdateDownwardPass(m_fSavedTime,
                    GetUpdateControllers());
            }
            pkChild->UpdateNodeBound();
        }
    }

    NiTransform kWorldInverse;
    m_kWorld.Invert(kWorldInverse);
    m_kBound.Update(m_kWorldBound, kWorldInverse);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
// cloning
//---------------------------------------------------------------------------
NiImplementCreateClone(NiSwitchNode);
//---------------------------------------------------------------------------
void NiSwitchNode::CopyMembers(NiSwitchNode* pkDest,
    NiCloningProcess& kCloning)
{
    NiNode::CopyMembers(pkDest, kCloning);

    pkDest->m_uFlags = m_uFlags;

    pkDest->m_iIndex = m_iIndex;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// streaming
//---------------------------------------------------------------------------
NiImplementCreateObject(NiSwitchNode);
//---------------------------------------------------------------------------
void NiSwitchNode::LoadBinary(NiStream& kStream)
{
    NiNode::LoadBinary(kStream);

    NiStreamLoadBinary(kStream, m_uFlags);
    NiStreamLoadBinary(kStream, m_iIndex);

    // Variable should not be streamed in; return to initialized value.
    SetUpdateControllers(true);
}
//---------------------------------------------------------------------------
void NiSwitchNode::LinkObject(NiStream& kStream)
{
    NiNode::LinkObject(kStream);
}
//---------------------------------------------------------------------------
bool NiSwitchNode::RegisterStreamables(NiStream& kStream)
{
    return NiNode::RegisterStreamables(kStream);
}
//---------------------------------------------------------------------------
void NiSwitchNode::SaveBinary(NiStream& kStream)
{
    NiNode::SaveBinary(kStream);
    NiStreamSaveBinary(kStream, m_uFlags);
    NiStreamSaveBinary(kStream, m_iIndex);
}
//---------------------------------------------------------------------------
bool NiSwitchNode::IsEqual(NiObject* pkObject)
{
    if (!NiNode::IsEqual(pkObject))
        return false;

    NiSwitchNode* pkSwitch = (NiSwitchNode*) pkObject;

    if (m_iIndex != pkSwitch->m_iIndex ||
        GetUpdateOnlyActiveChild() != pkSwitch->GetUpdateOnlyActiveChild())
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
void NiSwitchNode::GetViewerStrings(NiViewerStringsArray* pkStrings)
{
    NiNode::GetViewerStrings(pkStrings);

    pkStrings->Add(NiGetViewerString(NiSwitchNode::ms_RTTI.GetName()));

    pkStrings->Add(NiGetViewerString("m_iIndex", m_iIndex));
    pkStrings->Add(NiGetViewerString("m_bUpdateOnlyActive",
        GetUpdateOnlyActiveChild()));
}
//---------------------------------------------------------------------------
