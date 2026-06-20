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

#include "NiCloningProcess.h"
#include "NiGeometry.h"
#include "NiMatrix3.h"
#include "NiProcessor.h"
#include "NiShaderFactory.h"
#include "NiSingleShaderMaterial.h"
#include "NiMaterialLibrary.h"
#include <NiSystem.h>

NiImplementRTTI(NiGeometry, NiAVObject);

bool NiGeometry::ms_bStreamingAppliesDefaultMaterial = true;

//---------------------------------------------------------------------------
NiGeometry::NiGeometry(NiGeometryData* pkModelData)
    : m_spModelData(pkModelData),
    m_kMaterials(0),
    m_uiActiveMaterial(NO_MATERIAL),
    m_spShaderDecl(NULL),
    m_bDefaultMaterialDirtyFlag(true)
{
}
//---------------------------------------------------------------------------
NiGeometry::NiGeometry()
    : m_spModelData(0),
    m_kMaterials(0),
    m_uiActiveMaterial(NO_MATERIAL),
    m_spShaderDecl(NULL),
    m_bDefaultMaterialDirtyFlag(true)
{
    /* */
}
//---------------------------------------------------------------------------
NiGeometry::~NiGeometry()
{
    unsigned int uiMaterialCount = m_kMaterials.GetSize();
    for (unsigned int i = 0; i < uiMaterialCount; i++)
        m_kMaterials.GetAt(i).Reinitialize();
    m_kMaterials.RemoveAll();

    m_spModelData = 0;
}
//---------------------------------------------------------------------------
void NiGeometry::OnVisible(NiCullingProcess& kCuller)
{
    kCuller.Append(*this);
}
//---------------------------------------------------------------------------
void NiGeometry::RenderImmediate(NiRenderer* pkRenderer)
{
    NIASSERT(m_spPropertyState);
    pkRenderer->SetPropertyState(m_spPropertyState);
    pkRenderer->SetEffectState(m_spEffectState);

    NiTimeController::OnPreDisplayIterate(GetControllers());
}
//---------------------------------------------------------------------------
bool NiGeometry::ApplyMaterial(NiMaterial* pkMaterial, 
    unsigned int uiMaterialExtraData)
{
    // Ensure material is not already applied to geometry
    unsigned int uiNumMaterials = m_kMaterials.GetSize();
    for (unsigned int i = 0; i < uiNumMaterials; i++)
    {
        const NiMaterialInstance& kMatInst = m_kMaterials.GetAt(i);
        if (kMatInst.GetMaterial() == pkMaterial)
            return false;
    }

    if (!pkMaterial)
        return false;

    NiMaterialInstance kMaterialInstance(pkMaterial, uiMaterialExtraData);
    kMaterialInstance.SetMaterialNeedsUpdate(true);

    m_kMaterials.Add(kMaterialInstance);

    return true;
}
//---------------------------------------------------------------------------
bool NiGeometry::ApplyMaterial(const NiFixedString& kName, 
    unsigned int uiMaterialExtraData)
{
    NiMaterial* pkMaterial = NiMaterial::GetMaterial(kName);
    if (!pkMaterial)
    {
        pkMaterial = NiMaterialLibrary::CreateMaterial(kName);
    }

    return ApplyMaterial(pkMaterial, uiMaterialExtraData);
}
//---------------------------------------------------------------------------
bool NiGeometry::ApplyAndSetActiveMaterial(NiMaterial* pkMaterial, 
    unsigned int uiMaterialExtraData)
{
    // Ensure material is not already applied to geometry
    unsigned int uiNumMaterials = m_kMaterials.GetSize();
    for (unsigned int i = 0; i < uiNumMaterials; i++)
    {
        const NiMaterialInstance& kMatInst = m_kMaterials.GetAt(i);
        if (kMatInst.GetMaterial() == pkMaterial)
        {
            m_uiActiveMaterial = i;
            return true;
        }
    }

    if (!pkMaterial)
    {
        m_uiActiveMaterial = NO_MATERIAL;
        return true;
    }

    NiMaterialInstance kMaterialInstance(pkMaterial, uiMaterialExtraData);
    kMaterialInstance.SetMaterialNeedsUpdate(true);

    m_uiActiveMaterial = m_kMaterials.GetSize();
    m_kMaterials.Add(kMaterialInstance);

    return true;
}
//---------------------------------------------------------------------------
bool NiGeometry::ApplyAndSetActiveMaterial(const NiFixedString& kName, 
    unsigned int uiMaterialExtraData)
{
    NiMaterial* pkMaterial = NiMaterial::GetMaterial(kName);
    if (!pkMaterial)
    {
        pkMaterial = NiMaterialLibrary::CreateMaterial(kName);
    }

    return ApplyAndSetActiveMaterial(pkMaterial, uiMaterialExtraData);
}
//---------------------------------------------------------------------------
bool NiGeometry::RemoveMaterial(const NiFixedString& kName)
{
    return RemoveMaterial(NiMaterial::GetMaterial(kName));
}
//---------------------------------------------------------------------------
bool NiGeometry::RemoveMaterial(NiMaterial* pkMaterial)
{
    unsigned int uiNumMaterials = m_kMaterials.GetSize();
    unsigned int uiIndex = UINT_MAX;
    for (unsigned int i = 0; i < uiNumMaterials; i++)
    {
        const NiMaterialInstance& kMatInst = m_kMaterials.GetAt(i);
        if (kMatInst.GetMaterial() == pkMaterial)
        {
            uiIndex = i;
            break;
        }
    }
    if (uiIndex == UINT_MAX)
        return false;

#ifdef _DEBUG
    const NiMaterial* pkOldActiveMaterial = GetActiveMaterial();
#endif

    if (uiIndex == m_uiActiveMaterial)
    {
        // Removing active material
        // Set no new active material
        m_uiActiveMaterial = NO_MATERIAL;
#ifdef _DEBUG
        pkOldActiveMaterial = NULL;
#endif
    }
    else if (m_uiActiveMaterial == m_kMaterials.GetSize())
    {
        // Active material will be moved; update the active material to 
        // reflect this
        m_uiActiveMaterial = uiIndex;
    }

    m_kMaterials.GetAt(uiIndex).Reinitialize();
    m_kMaterials.RemoveAt(uiIndex);

    NIASSERT(pkOldActiveMaterial == GetActiveMaterial());

    return true;
}
//---------------------------------------------------------------------------
bool NiGeometry::IsMaterialApplied(const NiMaterial* pkMaterial) const
{
    if (pkMaterial == NULL)
        return false;

    unsigned int uiNumMaterials = m_kMaterials.GetSize();
    for (unsigned int i = 0; i < uiNumMaterials; i++)
    {
        const NiMaterialInstance& kMatInst = m_kMaterials.GetAt(i);
        if (kMatInst.GetMaterial() == pkMaterial)
            return true;
    }

    return false;
}
//---------------------------------------------------------------------------
unsigned int NiGeometry::GetMaterialCount()
{
    return m_kMaterials.GetSize();
}
//---------------------------------------------------------------------------
const NiMaterialInstance* NiGeometry::GetMaterialInstance(
    unsigned int uiInstance)
{
    return &(m_kMaterials.GetAt(uiInstance));
}
//---------------------------------------------------------------------------
void NiGeometry::SetMaterialNeedsUpdate(bool bNeedsUpdate)
{
    unsigned int uiNumMaterials = m_kMaterials.GetSize();
    for (unsigned int i = 0; i < uiNumMaterials; i++)
    {
        NiMaterialInstance& kMatInst = m_kMaterials.GetAt(i);
        kMatInst.SetMaterialNeedsUpdate(bNeedsUpdate);
    }
}
//---------------------------------------------------------------------------
void NiGeometry::SetDefaultMaterialNeedsUpdateFlag(bool bFlag)
{
    m_bDefaultMaterialDirtyFlag = bFlag;
}
//---------------------------------------------------------------------------
bool NiGeometry::ComputeCachedShader(unsigned int uiMaterial)
{
    NiMaterialInstance& kInst = m_kMaterials.GetAt(uiMaterial);
    return kInst.GetCurrentShader(this, GetSkinInstance(), GetPropertyState(), 
        GetEffectState()) != NULL;   
}
//---------------------------------------------------------------------------
void NiGeometry::ClearCachedShader(unsigned int uiMaterial)
{
    NiMaterialInstance& kInst = m_kMaterials.GetAt(uiMaterial);
    return kInst.ClearCachedShader();
}
//---------------------------------------------------------------------------
NiShader* NiGeometry::GetShader() const
{
    if (m_uiActiveMaterial >= m_kMaterials.GetSize())
        return NULL;

    const NiMaterialInstance& kInst = m_kMaterials.GetAt(m_uiActiveMaterial);

    return kInst.GetCachedShader(this, GetSkinInstance(), GetPropertyState(), 
        GetEffectState());
}
//---------------------------------------------------------------------------
NiShader* NiGeometry::GetShaderFromMaterial()
{
    if (m_uiActiveMaterial >= m_kMaterials.GetSize())
        return NULL;

    NiMaterialInstance& kInst = m_kMaterials.GetAt(m_uiActiveMaterial);

    return kInst.GetCurrentShader(this, GetSkinInstance(), GetPropertyState(), 
        GetEffectState());
}
//---------------------------------------------------------------------------
void NiGeometry::SetShader(NiShader* pkShader)
{
    NiSingleShaderMaterial* pkMaterial = 
        NiSingleShaderMaterial::Create(pkShader);

    // Apply pkMaterial even if it's NULL
    ApplyAndSetActiveMaterial(pkMaterial);
}
//---------------------------------------------------------------------------
void NiGeometry::SetModelData(NiGeometryData* pkModelData)
{
    NIASSERT(pkModelData);
    if (!pkModelData)
        return;

    if (m_spCollisionObject)
    {
        m_spCollisionObject->RecreateWorldData();
        m_spCollisionObject->UpdateWorldData();
    }

    m_spModelData = pkModelData;
}
//---------------------------------------------------------------------------
void NiGeometry::CalculateNormals()
{
    m_spModelData->CalculateNormals();
}
//---------------------------------------------------------------------------
void NiGeometry::UpdateWorldBound()
{
    if (m_spSkinInstance != NULL)
    {
        // In the case of skinned characters, the model data's bound is not
        // accurate because the bones will modify it, so it should not get
        // read nor written.
        NiBound kBound;
        m_spSkinInstance->UpdateModelBound(kBound);
        m_kWorldBound.Update(kBound, m_kWorld);
    }
    else
    {
        NiBound& kBound = m_spModelData->GetBound();
        m_kWorldBound.Update(kBound, m_kWorld);
    }
}
//---------------------------------------------------------------------------
void NiGeometry::UpdatePropertiesDownward(NiPropertyState* pkParentState)
{
    m_spPropertyState = PushLocalProperties(pkParentState, true);
    SetMaterialNeedsUpdate(true);
}
//---------------------------------------------------------------------------
void NiGeometry::UpdateEffectsDownward(NiDynamicEffectState* pkParentState)
{
    if (pkParentState)
    {
        m_spEffectState = pkParentState;
    }
    else
    {
        // DO NOT change the cached effect state - there is no point in
        // ever having this be NULL - all NULL's are the same as-is
        m_spEffectState = NULL;   
    }

    SetMaterialNeedsUpdate(true);
}
//---------------------------------------------------------------------------
void NiGeometry::ApplyTransform(const NiMatrix3& kMat, const NiPoint3& kTrn, 
    bool bOnLeft)
{
    // The model data is transformed on the spot rather than hanging onto
    // kMat and kTrn and using them in the UpdateWorldData each click.

    unsigned short usVertices = m_spModelData->GetActiveVertexCount();
    NiPoint3* pkVertex = m_spModelData->GetVertices();
    NiPoint3* pkNormal = m_spModelData->GetNormals();
    unsigned short i;

    if (bOnLeft)
    {
        // We need to commute the matrices again as described in the NiNode
        // version of ApplyTransform.
        NiMatrix3 kM1 = m_kLocal.m_Rotate.TransposeTimes(kMat 
            * m_kLocal.m_Rotate);
        NiPoint3 kT1 = ((kMat * m_kLocal.m_Translate + kTrn 
            - m_kLocal.m_Translate) * m_kLocal.m_Rotate) / m_kLocal.m_fScale;

        if (pkVertex)
        {
            for (i = 0; i < usVertices; i++)
                pkVertex[i] = kT1 + kM1 * pkVertex[i];
        }
        if (pkNormal)
        {
            NiMatrix3 kInvM1;
            kM1.Inverse(kInvM1);
            for (i = 0; i < usVertices; i++)
                pkNormal[i] = pkNormal[i]*kInvM1;
        }
    }
    else
    {
        if (pkVertex)
        {
            for (i = 0; i < usVertices; i++)
                pkVertex[i] = kTrn + kMat * pkVertex[i];
        }
        if (pkNormal)
        {
            NiMatrix3 kInvMat;
            kMat.Inverse(kInvMat);
            for (i = 0; i < usVertices; i++)
                pkNormal[i] = pkNormal[i] * kInvMat;
        }
    }

    m_spModelData->GetBound().ComputeFromData(usVertices, pkVertex);
}
//---------------------------------------------------------------------------
void NiGeometry::SetSelectiveUpdateFlags(bool& bSelectiveUpdate, 
    bool bSelectiveUpdateTransforms, bool& bRigid)
{
    // Sets the selective update flags the same as NiAVObject does,
    // except checks to see if geometry is skinned.  If it is, then
    // bSelectiveUpdateTransforms and bSelectiveUpdate are set to true,
    // and bRigid is returned as false.

    NiAVObject::SetSelectiveUpdateFlags(bSelectiveUpdate, 
        bSelectiveUpdateTransforms, bRigid);

    if (GetSkinInstance() != NULL)
    {
        SetSelectiveUpdateTransforms(true);
        bSelectiveUpdate = true;
        SetSelectiveUpdate(bSelectiveUpdate);
        bRigid = false;
    }
}
//---------------------------------------------------------------------------
void NiGeometry::CalculateConsistency(bool bTool)
{
    NiGeometryData::Consistency eConsistency = NiGeometryData::STATIC;

    // Check for controllers that change vertex data
    NiTimeController* pkController = GetControllers();
    while (pkController)
    {
        if (pkController->IsVertexController())
        {
            eConsistency = NiGeometryData::VOLATILE;
            break;
        }

        pkController = pkController->GetNext();
    }

    // Check for software skinning
    NiSkinInstance* pkSkin = GetSkinInstance();
    if (pkSkin)
    {
        if (bTool)
        {
            // If called from a tool, we only have whether or not the object
            // contains partitions to determine whether or not the object
            // will be skinned in hardware.
            if (!pkSkin->GetSkinPartition())
            {
                eConsistency = NiGeometryData::VOLATILE;
            }
        }
        else
        {
            // If called from the game itself, the hardware can also be 
            // queried to determine whether the object will be skinned
            // in hardware.
            if (pkSkin->GetSkinPartition())
            {
                NiRenderer* pkRenderer = NiRenderer::GetRenderer();
                NIASSERT(pkRenderer);
                if (pkRenderer && (pkRenderer->GetFlags() & 
                    NiRenderer::CAPS_HARDWARESKINNING) == 0)
                {
                    eConsistency = NiGeometryData::VOLATILE;
                }
            }
            else
            {
                eConsistency = NiGeometryData::VOLATILE;
            }
        }
    }

    m_spModelData->SetConsistency(eConsistency);
}
//---------------------------------------------------------------------------
void NiGeometry::PurgeRendererData(NiRenderer* pkRenderer)
{
    NiAVObject::PurgeRendererData(pkRenderer);

    pkRenderer->PurgeGeometryData(m_spModelData);

    unsigned int uiCount = GetMaterialCount();
    for (unsigned int ui = 0; ui < uiCount; ui++)
    {
        ClearCachedShader(ui);
    }

    if (m_spSkinInstance)
    {
        pkRenderer->PurgeSkinInstance(m_spSkinInstance);
        pkRenderer->PurgeSkinPartitionRendererData(
            m_spSkinInstance->GetSkinPartition());
    }
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// cloning
//---------------------------------------------------------------------------
void NiGeometry::CopyMembers(NiGeometry* pkDest,
    NiCloningProcess& kCloning)
{
    pkDest->SetModelData(m_spModelData);
    pkDest->SetDefaultMaterialNeedsUpdateFlag(m_bDefaultMaterialDirtyFlag);
    NiAVObject::CopyMembers(pkDest, kCloning);
}
//---------------------------------------------------------------------------
void NiGeometry::ProcessClone(
    NiCloningProcess& kCloning)
{
    NiAVObject::ProcessClone(kCloning);

    NiObject* pkClone = NULL;
    bool bCloned = kCloning.m_pkCloneMap->GetAt(this, pkClone);
    NIASSERT(bCloned && pkClone->IsKindOf(GetRTTI()));
    NiGeometry* pkGeom = (NiGeometry*) pkClone;

    if (m_spSkinInstance != NULL)
    {
        bCloned = kCloning.m_pkCloneMap->GetAt(m_spSkinInstance, pkClone);
        if (bCloned)
        {
            pkGeom->m_spSkinInstance = (NiSkinInstance*) pkClone;
        }
        else
        {
            pkGeom->m_spSkinInstance = (NiSkinInstance*)
                m_spSkinInstance->CreateClone(kCloning);
            m_spSkinInstance->ProcessClone(kCloning);
        }
    }

    for (unsigned int i = 0; i < GetMaterialCount(); i++)
    {
        const NiMaterialInstance* pkInst = GetMaterialInstance(i);
        pkGeom->ApplyMaterial(pkInst->GetMaterial(), 
            pkInst->GetMaterialExtraData());
    }

    pkGeom->SetActiveMaterial(GetActiveMaterial());
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// streaming
//---------------------------------------------------------------------------
void NiGeometry::PostLinkObject(NiStream& kStream)
{
    NiAVObject::PostLinkObject(kStream);

}
//---------------------------------------------------------------------------
void NiGeometry::LoadBinary(NiStream& kStream)
{
    NiAVObject::LoadBinary(kStream);

    kStream.ReadLinkID();   // m_spModelData
    kStream.ReadLinkID();   // m_spSkinInstance

    if (kStream.GetFileVersion() >= NiStream::GetVersion(20, 2, 0, 5))
    {
        unsigned int uiNumMaterials = 0;
        NiStreamLoadBinary(kStream, uiNumMaterials);

        for (unsigned int i = 0; i < uiNumMaterials; i++)
        {
            NiFixedString kName;
            kStream.LoadFixedString(kName);

            unsigned int uiMaterialExtraData;
            NiStreamLoadBinary(kStream, uiMaterialExtraData);

            bool bSuccess = ApplyMaterial(kName, uiMaterialExtraData);
            if (!bSuccess)
            {
                NILOG("Material %s not found during "
                    "file loading.\n", (const char*)kName);
            }
        }

        NiStreamLoadBinary(kStream, m_uiActiveMaterial);

        if (uiNumMaterials == 0)
            m_uiActiveMaterial = NO_MATERIAL;
    }
    else
    {
        NiBool bShader = false;

        NiStreamLoadBinary(kStream, bShader);
        if (bShader)
        {
            // Just load the name and implementation here.
            NiFixedString kShaderName;
            if (kStream.GetFileVersion() < NiStream::GetVersion(20, 1, 0, 1))
            {
                kStream.LoadCStringAsFixedString(kShaderName);
            }
            else
            {
                kStream.LoadFixedString(kShaderName);
            }
            unsigned int uiImplementation;
            NiStreamLoadBinary(kStream, uiImplementation);

            NiSingleShaderMaterial* pkMaterial = 
                NiSingleShaderMaterial::Create(kShaderName);

            ApplyAndSetActiveMaterial(pkMaterial, uiImplementation);
        }
    }

    if (kStream.GetFileVersion() >= NiStream::GetVersion(20, 2, 0, 7))
    {
        NiBool bDefaultMaterialDirtyFlag = false;
        NiStreamLoadBinary(kStream, bDefaultMaterialDirtyFlag);
        m_bDefaultMaterialDirtyFlag = NIBOOL_IS_TRUE(
            bDefaultMaterialDirtyFlag);
    }

    // If there are no materials applied, then we attach the default material.
    if (ms_bStreamingAppliesDefaultMaterial && GetMaterialCount() == 0)
    {
        NiRenderer* pkRenderer = NiRenderer::GetRenderer();
        // If we have a renderer, attach the renderer default material and
        // mark as dirty.
        if (pkRenderer)
        {
            pkRenderer->ApplyDefaultMaterial(this);
            m_bDefaultMaterialDirtyFlag = true;
        }
    }
}
//---------------------------------------------------------------------------
void NiGeometry::LinkObject(NiStream& kStream)
{
    NiAVObject::LinkObject(kStream);

    // link the shared data
    m_spModelData = (NiGeometryData*) kStream.GetObjectFromLinkID();

    m_spSkinInstance = (NiSkinInstance*) 
        kStream.GetObjectFromLinkID();
}
//---------------------------------------------------------------------------
bool NiGeometry::RegisterStreamables(NiStream& kStream)
{
    if (!NiAVObject::RegisterStreamables(kStream))
        return false;

    m_spModelData->RegisterStreamables(kStream);

    if (m_spSkinInstance != NULL)
    {
        m_spSkinInstance->RegisterStreamables(kStream);
    }

    unsigned int uiNumMaterials = m_kMaterials.GetSize();
    for (unsigned int i = 0; i < uiNumMaterials; i++)
    {
        NiMaterial* pkMaterial = m_kMaterials.GetAt(i).GetMaterial();
        if (pkMaterial)
        {
            kStream.RegisterFixedString(pkMaterial->GetName());
        }
    }

    return true;
}
//---------------------------------------------------------------------------
void NiGeometry::SaveBinary(NiStream& kStream)
{
    NiAVObject::SaveBinary(kStream);

    kStream.SaveLinkID(m_spModelData);
    kStream.SaveLinkID(m_spSkinInstance);

    unsigned int uiNumMaterials = m_kMaterials.GetSize();
    NiStreamSaveBinary(kStream, uiNumMaterials);
    for (unsigned int i = 0; i < uiNumMaterials; i++)
    {
        const NiMaterial* pkMaterial = m_kMaterials.GetAt(i).GetMaterial();
        kStream.SaveFixedString(pkMaterial->GetName());

        unsigned int uiExtraData = 
            m_kMaterials.GetAt(i).GetMaterialExtraData();
        NiStreamSaveBinary(kStream, uiExtraData);
    }

    NiStreamSaveBinary(kStream, m_uiActiveMaterial);

    NiBool bDefaultMaterialDirtyFlag = m_bDefaultMaterialDirtyFlag;
    NiStreamSaveBinary(kStream, bDefaultMaterialDirtyFlag);
}
//---------------------------------------------------------------------------
bool NiGeometry::IsEqual(NiObject* pkObject)
{
    if (!NiAVObject::IsEqual(pkObject))
        return false;

    NiGeometry* pkGeom = (NiGeometry*) pkObject;

    if ((m_spSkinInstance == NULL) != (pkGeom->m_spSkinInstance == NULL))
        return false;

    if (m_spSkinInstance != NULL && 
        !m_spSkinInstance->IsEqual(pkGeom->m_spSkinInstance))
    {
        return false;
    }

    if (m_bDefaultMaterialDirtyFlag != pkGeom->m_bDefaultMaterialDirtyFlag)
        return false;
    
    return m_spModelData->IsEqual(pkGeom->m_spModelData);
}
//---------------------------------------------------------------------------
void NiGeometry::GetViewerStrings(NiViewerStringsArray* pkStrings)
{
    NiAVObject::GetViewerStrings(pkStrings);

    pkStrings->Add(NiGetViewerString(NiGeometry::ms_RTTI.GetName()));
}
//---------------------------------------------------------------------------
