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

#include "NiBSPNode.h"
#include "NiCamera.h"
#include "NiCullingProcess.h"
#include <NiRTLib.h>

NiImplementRTTI(NiBSPNode,NiNode);

//---------------------------------------------------------------------------
void NiBSPNode::UpdateWorldPlane()
{
    // The model plane has normal N0, constant C0 and is given by
    // Dot(N0,X) = C0.  If Y = s*R*X+T where s is world scale, R is world
    // rotation, and T is world translation for current node, then
    // X = (1/s)*R^t*(Y-T) and
    //     C0 = Dot(N0,X) = Dot(N0,(1/s)*R^t*(Y-T)) = (1/s)*Dot(R*N0,Y-T)
    // so
    //     Dot(R*N0,Y) = s*c0+Dot(R*N0,T)
    // The world plane has
    //     normal N1 = R*N0
    //     constant C1 = s*C0+Dot(R*N0,T) = s*C0+Dot(N1,T)

    NiPoint3 normal = m_kWorld.m_Rotate*m_kModelPlane.GetNormal();
    float fConstant = m_kWorld.m_fScale*m_kModelPlane.GetConstant() +
        normal*m_kWorld.m_Translate;
    m_kWorldPlane.SetNormal(normal);
    m_kWorldPlane.SetConstant(fConstant);
}
//---------------------------------------------------------------------------
void NiBSPNode::UpdateWorldData()
{
    NiNode::UpdateWorldData();
    UpdateWorldPlane();
}
//---------------------------------------------------------------------------
void NiBSPNode::ApplyTransform(const NiMatrix3& mat, const NiPoint3& trn,
    bool bOnLeft)
{
    unsigned int i;
    NiAVObject* pChild;

    if ( bOnLeft )
    {
        // We need to commute the matrices again as described in the NiNode
        // version of ApplyTransform.
        NiMatrix3 M1 = m_kLocal.m_Rotate.TransposeTimes(mat*m_kLocal.m_Rotate);
        NiPoint3 T1 = ((mat*m_kLocal.m_Translate+trn-m_kLocal.m_Translate) *
            m_kLocal.m_Rotate)/m_kLocal.m_fScale;

        NiMatrix3 invM1;
        M1.Inverse(invM1);
        m_kModelPlane.SetNormal(m_kModelPlane.GetNormal()*invM1);
        m_kModelPlane.SetConstant(m_kModelPlane.GetConstant() 
            + m_kModelPlane.GetNormal()*T1);

        for (i = 0; i < m_kChildren.GetSize(); i++)
        {
            pChild = m_kChildren.GetAt(i);
            if ( pChild )
                pChild->ApplyTransform(M1,T1,true);
        }
    }
    else
    {
        NiMatrix3 invMat;
        mat.Inverse(invMat);
        m_kModelPlane.SetNormal(m_kModelPlane.GetNormal()*invMat);
        m_kModelPlane.SetConstant(m_kModelPlane.GetConstant() 
            + m_kModelPlane.GetNormal()*trn);

        for (i = 0; i < m_kChildren.GetSize(); i++)
        {
            pChild = m_kChildren.GetAt(i);
            if ( pChild )
                pChild->ApplyTransform(mat,trn,true);
        }
    }
}
//---------------------------------------------------------------------------
void NiBSPNode::OnVisible(NiCullingProcess& kCuller)
{
    // determine which side of separating plane the camera is on
    const NiCamera* pkCamera = kCuller.GetCamera();
    int iSide = m_kWorldPlane.WhichSide(pkCamera->GetWorldLocation());
    if (iSide == NiPlane::NO_SIDE)
        iSide = m_kWorldPlane.WhichSide(pkCamera->GetWorldDirection());
    
    // draw children in correct order
    NiAVObject* pkLeft = m_kChildren.GetAt(0);
    NiAVObject* pkRight = m_kChildren.GetAt(1);
    
    if (iSide == NiPlane::NEGATIVE_SIDE) 
    {
        if (pkRight)
            pkRight->Cull(kCuller);
        if (pkLeft)
            pkLeft->Cull(kCuller);
    } 
    else 
    {
        if (pkLeft)
            pkLeft->Cull(kCuller);
        if (pkRight)
            pkRight->Cull(kCuller);
    }
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// cloning
//---------------------------------------------------------------------------
NiImplementCreateClone(NiBSPNode);
//---------------------------------------------------------------------------
void NiBSPNode::CopyMembers(NiBSPNode* pDest,
    NiCloningProcess& kCloning)
{
    NiNode::CopyMembers(pDest, kCloning);

    pDest->m_kModelPlane = m_kModelPlane;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// streaming
//---------------------------------------------------------------------------
NiImplementCreateObject(NiBSPNode);
//---------------------------------------------------------------------------
void NiBSPNode::LoadBinary(NiStream& stream)
{
    NiNode::LoadBinary(stream);
    m_kModelPlane.LoadBinary(stream);
}
//---------------------------------------------------------------------------
void NiBSPNode::LinkObject(NiStream& stream)
{
    NiNode::LinkObject(stream);
}
//---------------------------------------------------------------------------
bool NiBSPNode::RegisterStreamables(NiStream& stream)
{
    return NiNode::RegisterStreamables(stream);
}
//---------------------------------------------------------------------------
void NiBSPNode::SaveBinary(NiStream& stream)
{
    NiNode::SaveBinary(stream);
    m_kModelPlane.SaveBinary(stream);
}
//---------------------------------------------------------------------------
bool NiBSPNode::IsEqual(NiObject* pObject)
{
    if ( !NiNode::IsEqual(pObject) )
        return false;

    NiBSPNode* pNode = (NiBSPNode*) pObject;

    if ( m_kModelPlane != pNode->m_kModelPlane )
        return false;

    return true;
}
//---------------------------------------------------------------------------
void NiBSPNode::GetViewerStrings(NiViewerStringsArray* pStrings)
{
    NiNode::GetViewerStrings(pStrings);

    pStrings->Add(NiGetViewerString(NiBSPNode::ms_RTTI.GetName()));

    pStrings->Add(m_kModelPlane.GetViewerString("m_kModelPlane"));
    pStrings->Add(m_kWorldPlane.GetViewerString("m_kWorldPlane"));
}
//---------------------------------------------------------------------------
