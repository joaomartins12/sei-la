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

#include "stdafx.h"
//#include <NiCamera.h>
//#include <NiGeometry.h>
//#include <NiNode.h>

#include "NiOcclusionCuller.h"
#include "PlanarOccluder.h"

//---------------------------------------------------------------------------
NiOcclusionCuller::NiOcclusionCuller()
{
}
//---------------------------------------------------------------------------
NiOcclusionCuller::~NiOcclusionCuller()
{
    RemoveAllOcclusionGeometry();
}
//---------------------------------------------------------------------------
void NiOcclusionCuller::Update(const NiCamera* pkCamera)
{
    // Determine which side of the occlusion planes the camera is on for
    // fast dismissal during occlusion testing.
    unsigned int uiNumOccluders = m_kOccluders.GetEffectiveSize();
    for (unsigned int ui = 0; ui < uiNumOccluders; ui++)
    {
        Occluder* pkRecord = m_kOccluders.GetAt(ui);
        pkRecord->Update((NiCamera*)pkCamera);
    }
}
//---------------------------------------------------------------------------
bool NiOcclusionCuller::IsCulling(NiCamera* pkCamera, NiAVObject* pkObject)
{
    // Cull by occlusion objects first. Then proceed with normal processing
    // if object is not culled.

    unsigned int uiNumOccluders = m_kOccluders.GetEffectiveSize();
    // Test each anti-portal
    for (unsigned int ui = 0; ui < uiNumOccluders; ui++)
    {
		Occluder* pkRecord = m_kOccluders.GetAt(ui);
        if (pkRecord->IsOccluded(pkCamera, pkObject->GetWorldBound()))
            return true;
    } 

	return false;
}
//---------------------------------------------------------------------------
void NiOcclusionCuller::RecursiveFindOcclusionGeometry(NiNode* pkRootNode)
{
    if (!pkRootNode)
        return;

//     unsigned int uiSceneRootCount;
//     if (pkEntity->GetElementCount(m_kSceneRootPointerName, uiSceneRootCount))
//     {
//         for (unsigned int ui = 0; ui < uiSceneRootCount; ui++)
//         {
//             NiObject* pkObject;
//             NIVERIFY(pkEntity->GetPropertyData(
//                 m_kSceneRootPointerName, pkObject, ui));
// 
//             NiAVObject* pkSceneRoot = NiDynamicCast(NiAVObject, pkObject);
//             if (pkSceneRoot)
//             {
//                 FindOcclusionGeometry(pkSceneRoot, false);
//             }
//         }
//     }
}
//---------------------------------------------------------------------------
void NiOcclusionCuller::FindOcclusionGeometry(NiNode* pkEntityScene)
{
//     for (unsigned int ui = 0; ui < pkEntityScene->GetEntityCount(); ui++)
//         RecursiveFindOcclusionGeometry(pkEntityScene->GetEntityAt(ui));
}
//---------------------------------------------------------------------------
void NiOcclusionCuller::AddOcclusionGeometry(NiNode* pkEntityScene)
{
	FindOcclusionGeometry( pkEntityScene, false );
}
//---------------------------------------------------------------------------
void NiOcclusionCuller::ShowOccluder(bool bVisible)
{
	unsigned int uiNumOccluders = m_kOccluders.GetEffectiveSize();
	// Test each anti-portal
	for (unsigned int ui = 0; ui < uiNumOccluders; ui++)
	{
		Occluder* pkRecord = m_kOccluders.GetAt(ui);
		pkRecord->ShowObject(bVisible);
	} 
}
//---------------------------------------------------------------------------
void NiOcclusionCuller::FindOcclusionGeometry(NiAVObject* pkAVObject, bool bMarkedParent)
{
    if (NiIsKindOf(NiGeometry, pkAVObject))
    {
		NiFixedString const& obName = pkAVObject->GetName();

        if (bMarkedParent || (obName.Exists() && strstr(obName, "PlanarOccluder")))
        {
            Occluder* pkRecord = NiNew PlanarOccluder((NiGeometry*)pkAVObject);
            m_kOccluders.AddFirstEmpty(pkRecord);
		
			NiNode* pNiNode = nsCSGBFUNC::GetRootNiNode(pkAVObject);
			if( pNiNode )
				pNiNode->SetAppCulled(true);
				
            //NiGeometry* pkGeom = (NiGeometry*)pkAVObject;
            //pkGeom->GetModelData()->SetKeepFlags(NiGeometryData::KEEP_ALL);
        }
    }
    else if (NiIsKindOf(NiNode, pkAVObject))
    {
        bool bMarked = false;
        if (strstr(pkAVObject->GetName(), "Occluder"))
            bMarked = true;

        NiNode* pkNode = (NiNode*)pkAVObject;

        unsigned int uiChildren = pkNode->GetArrayCount();
        for (unsigned int ui = 0; ui < uiChildren; ui++)
        {
            NiAVObject* pkChild = pkNode->GetAt(ui);
            if (pkChild)
                FindOcclusionGeometry(pkChild, bMarked);
        }
    }
}
//---------------------------------------------------------------------------
void NiOcclusionCuller::RemoveAllOcclusionGeometry()
{
    for (unsigned int ui = 0; ui < m_kOccluders.GetSize(); ui++)
        NiDelete m_kOccluders.GetAt(ui);
    m_kOccluders.RemoveAll();
}
//---------------------------------------------------------------------------
