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

#include "NiPhysXPCH.h"

#include "NiPhysX.h"

#include <NxPhysics.h>

NiImplementRTTI(NiPhysXClothDesc, NiObject);

//---------------------------------------------------------------------------
NiPhysXClothDesc::NiPhysXClothDesc() : m_kClothName(NULL), m_kAttachments(0,5)
{
    NxClothDesc kClothDesc;
    kClothDesc.setToDefault();
    
    m_kGlobalPose = kClothDesc.globalPose;
    m_fThickness = kClothDesc.thickness;
    m_fDensity = kClothDesc.density;
    m_fBendingStiffness = kClothDesc.bendingStiffness;
    m_fStretchingStiffness = kClothDesc.stretchingStiffness;
    m_fDampingCoefficient = kClothDesc.dampingCoefficient;
    m_fFriction = kClothDesc.friction;
    m_fPressure = kClothDesc.pressure;
    m_fTearFactor = kClothDesc.tearFactor;
    m_fCollisionResponseCoefficient = kClothDesc.collisionResponseCoefficient;
    m_fAttachmentResponseCoefficient =
        kClothDesc.attachmentResponseCoefficient;
    m_fAttachmentTearFactor = kClothDesc.attachmentTearFactor;
    m_uiSolverIterations = kClothDesc.solverIterations;
    m_kExternalAcceleration = kClothDesc.externalAcceleration;
    m_fWakeUpCounter = kClothDesc.wakeUpCounter;
    m_fSleepLinearVelocity = kClothDesc.sleepLinearVelocity;
    m_uiCollisionGroup = kClothDesc.collisionGroup;
    m_auiCollisionBits[0] = kClothDesc.groupsMask.bits0;
    m_auiCollisionBits[1] = kClothDesc.groupsMask.bits1;
    m_auiCollisionBits[2] = kClothDesc.groupsMask.bits2;
    m_auiCollisionBits[3] = kClothDesc.groupsMask.bits3;
    m_uiFlags = kClothDesc.flags;

    m_spClothMesh = 0;
    
    m_pkCloth = 0;
    m_pkParentActor = 0;
    
    m_spDest = 0;
}
//---------------------------------------------------------------------------
NiPhysXClothDesc::~NiPhysXClothDesc()
{
    m_spClothMesh = 0;
    
    for (unsigned int ui = m_kAttachments.GetSize(); ui > 0; ui--)
        RemoveAttachmentAt(ui-1);
    
    m_pkParentActor = 0;
    m_spDest = 0;
}
//---------------------------------------------------------------------------
void NiPhysXClothDesc::SetParameters(NiFixedString& kClothName,
    NiPhysXMeshDesc* pkMeshDesc, const NxMat34& kPose, const NxReal fThickness,
    const NxReal fDensity, const NxReal fBendingStiffness,
    const NxReal fStretchingStiffness, const NxReal fDampingCoefficient,
    const NxReal fFriction, const NxReal fPressure, const NxReal fTearFactor,
    const NxReal fCollisionResponseCoefficient,
    const NxReal fAttachmentResponseCoefficient,
    const NxReal fAttachmentTearFactor, const NxU32 fSolverIterations,
    const NxVec3 fExternalAcceleration, const NxReal fWakeUpCounter,
    const NxReal fSleepLinearVelocity, const NxCollisionGroup uiCollisionGroup,
    const NxGroupsMask kCollisionMask, const NxU32 uiFlags)
{
    m_kClothName = kClothName;
    m_spClothMesh = pkMeshDesc;
    m_kGlobalPose = kPose;
    m_fThickness = fThickness;
    m_fDensity = fDensity;
    m_fBendingStiffness = fBendingStiffness;
    m_fStretchingStiffness = fStretchingStiffness;
    m_fDampingCoefficient = fDampingCoefficient;
    m_fFriction = fFriction;
    m_fPressure = fPressure;
    m_fTearFactor = fTearFactor;
    m_fCollisionResponseCoefficient = fCollisionResponseCoefficient;
    m_fAttachmentResponseCoefficient = fAttachmentResponseCoefficient;
    m_fAttachmentTearFactor = fAttachmentTearFactor;
    m_uiSolverIterations = fSolverIterations;
    m_kExternalAcceleration = fExternalAcceleration;
    m_fWakeUpCounter = fWakeUpCounter;
    m_fSleepLinearVelocity = fSleepLinearVelocity;
    m_uiCollisionGroup = uiCollisionGroup;
    m_auiCollisionBits[0] = kCollisionMask.bits0;
    m_auiCollisionBits[1] = kCollisionMask.bits1;
    m_auiCollisionBits[2] = kCollisionMask.bits2;
    m_auiCollisionBits[3] = kCollisionMask.bits3;
    m_uiFlags = uiFlags;
}
//---------------------------------------------------------------------------
bool NiPhysXClothDesc::ToClothDesc(NxClothDesc& kClothDesc,
    const NxMat34& kXform, const bool bKeepMeshes, const bool bCheckPlatform)
{
    kClothDesc.name = m_kClothName;    
    kClothDesc.globalPose.multiply(kXform, m_kGlobalPose);
    kClothDesc.thickness = m_fThickness;
    kClothDesc.density = m_fDensity;
    kClothDesc.bendingStiffness = m_fBendingStiffness;
    kClothDesc.stretchingStiffness = m_fStretchingStiffness;
    kClothDesc.dampingCoefficient = m_fDampingCoefficient;
    kClothDesc.friction = m_fFriction;
    kClothDesc.pressure = m_fPressure;
    kClothDesc.tearFactor = m_fTearFactor;
    kClothDesc.collisionResponseCoefficient = m_fCollisionResponseCoefficient;
    kClothDesc.attachmentResponseCoefficient =
        m_fAttachmentResponseCoefficient;
    kClothDesc.attachmentTearFactor = m_fAttachmentTearFactor;
    kClothDesc.solverIterations = m_uiSolverIterations;
    kClothDesc.externalAcceleration = kXform.M * m_kExternalAcceleration;
    kClothDesc.wakeUpCounter = m_fWakeUpCounter;
    kClothDesc.sleepLinearVelocity = m_fSleepLinearVelocity;
    kClothDesc.collisionGroup = m_uiCollisionGroup;
    kClothDesc.groupsMask.bits0 = m_auiCollisionBits[0];
    kClothDesc.groupsMask.bits1 = m_auiCollisionBits[1];
    kClothDesc.groupsMask.bits2 = m_auiCollisionBits[2];
    kClothDesc.groupsMask.bits3 = m_auiCollisionBits[3];
    kClothDesc.flags = m_uiFlags;

    m_spClothMesh->ToClothDesc(kClothDesc, bKeepMeshes, bCheckPlatform);
    
    if (kClothDesc.clothMesh)
        return true;
    else
        return false;
}
//---------------------------------------------------------------------------
bool NiPhysXClothDesc::IsValid()
{
    bool bResult;
    
    // Do this with keep meshes true. If the mesh already exists, won't be
    // an issue at all. If it doesn't, we might create it but we won't
    // throw the data away. That makes this a suitable function to call
    // from an exporter. If the mesh is to be loaded from file, it will have
    // to have been saved already.
    NxClothDesc kClothDesc;
    if (!ToClothDesc(kClothDesc, NiPhysXTypes::NXMAT34_ID, true, false))
        return false;

    bResult = kClothDesc.isValid();
    
    return bResult;
}
//---------------------------------------------------------------------------
NxCloth* NiPhysXClothDesc::CreateCloth(NxScene* pkScene,
    NxCompartment* pkCompartment, const NxMat34& kXform,
    const bool bKeepMeshes)
{
    NxClothDesc kClothDesc;
    if (!ToClothDesc(kClothDesc, kXform, bKeepMeshes))
        return 0;
    
    kClothDesc.compartment = pkCompartment;
            
    m_pkCloth = pkScene->createCloth(kClothDesc);
    if (!m_pkCloth)
    {
#ifdef _DEBUG
        NiOutputDebugString(
            "NiPhysXClothDesc::CreateCloth: Cloth creation failed.\n");
        NiOutputDebugString(
            "NiPhysXClothDesc::CreateCloth: Look for a message from the "
            "PhysX SDK as to why this might have occurred.\n");
#endif
        return 0;
    }

    // Create attachments
    unsigned int uiCount = m_kAttachments.GetSize();
    for (unsigned int ui = 0; ui < uiCount; ui++)
    {
        ClothAttachment *pkAttachment = m_kAttachments.GetAt(ui);
        if (!pkAttachment)
            continue;
            
        if (pkAttachment->m_spShape)
        {
            NxShape* pkShape = pkAttachment->m_spShape->GetShape();
            
            NxMat34 kWorldPose = pkShape->getGlobalPose();
            NxMat34 kWorldInv;
            kWorldPose.getInverse(kWorldInv);
            
            NIASSERT(pkShape != 0);
            if (pkAttachment->m_uiVertexCount)
            {
                for (unsigned int uj = 0;
                    uj < pkAttachment->m_uiVertexCount; uj++)
                {
                    NxVec3 kPt = kWorldInv * pkAttachment->m_pkPositions[uj];
                    m_pkCloth->attachVertexToShape(
                        pkAttachment->m_puiVertexIDs[uj], pkShape, kPt,
                        pkAttachment->m_puiFlags[uj]);
                }
            }
            else
            {
                m_pkCloth->attachToShape(pkShape,
                    pkAttachment->m_puiFlags[0]);
            }
        }
        else
        {
            if (pkAttachment->m_uiVertexCount)
            {
                for (unsigned int uj = 0;
                    uj < pkAttachment->m_uiVertexCount; uj++)
                {
                    m_pkCloth->attachVertexToGlobalPosition(
                        pkAttachment->m_puiVertexIDs[uj],
                        pkAttachment->m_pkPositions[uj]);
                }
            }
            else
            {
                m_pkCloth->attachToCollidingShapes(
                    pkAttachment->m_puiFlags[0]);
            }
        }
    }
    
    unsigned short usSize;
    const unsigned short* pusVertexMap;
    m_spClothMesh->GetVertexMap(usSize, pusVertexMap);
    if (m_spDest)
    {
        m_spDest->SetCloth(m_pkCloth, pusVertexMap);
        if (m_pkParentActor)
        {
            m_spDest->SetParentActor(m_pkParentActor->GetActor());
        }
    }

    return m_pkCloth;
}
//---------------------------------------------------------------------------
void NiPhysXClothDesc::RemoveCloth(NxScene* pkScene)
{
    // De-activate any destination.
    if (m_spDest)
    {
        m_spDest->SetCloth(0, 0);
        m_spDest->SetParentActor(0);
    }

    if (m_pkCloth)
    {
        pkScene->releaseCloth(*m_pkCloth);
        m_pkCloth = 0;
    }
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Cloning Functions
//---------------------------------------------------------------------------
NiImplementCreateClone(NiPhysXClothDesc);
//---------------------------------------------------------------------------
void NiPhysXClothDesc::CopyMembers(NiPhysXClothDesc* pkDest,
    NiCloningProcess& kCloning)
{
    NiObject::CopyMembers(pkDest, kCloning);

    // copy object names
    if (kCloning.m_eCopyType == NiObjectNET::COPY_EXACT)
    {
        pkDest->m_kClothName = m_kClothName;
    }
    else if (kCloning.m_eCopyType == NiObjectNET::COPY_UNIQUE)
    {
        if (m_kClothName)
        {
            int iQuantity = strlen(m_kClothName) + 2;
            char* pcNamePlus = NiAlloc(char, iQuantity);
            NIASSERT(pcNamePlus);
            NiStrcpy(pcNamePlus, iQuantity, m_kClothName);
            pcNamePlus[iQuantity - 2] = kCloning.m_cAppendChar;
            pcNamePlus[iQuantity - 1] = 0;
            pkDest->m_kClothName = pcNamePlus;
            NiFree(pcNamePlus);
        }
    }

    // Cloth mesh data is shared
    pkDest->m_spClothMesh = m_spClothMesh;
    
    pkDest->m_kGlobalPose = m_kGlobalPose;
    pkDest->m_fThickness = m_fThickness;
    pkDest->m_fDensity = m_fDensity;
    pkDest->m_fBendingStiffness = m_fBendingStiffness;
    pkDest->m_fStretchingStiffness = m_fStretchingStiffness;
    pkDest->m_fDampingCoefficient = m_fDampingCoefficient;
    pkDest->m_fFriction = m_fFriction;
    pkDest->m_fPressure = m_fPressure;
    pkDest->m_fTearFactor = m_fTearFactor;
    pkDest->m_fCollisionResponseCoefficient = m_fCollisionResponseCoefficient;
    pkDest->m_fAttachmentResponseCoefficient =
        m_fAttachmentResponseCoefficient;
    pkDest->m_fAttachmentTearFactor = m_fAttachmentTearFactor;
    pkDest->m_uiSolverIterations = m_uiSolverIterations;
    pkDest->m_kExternalAcceleration = m_kExternalAcceleration;
    pkDest->m_fWakeUpCounter = m_fWakeUpCounter;
    pkDest->m_fSleepLinearVelocity = m_fSleepLinearVelocity;
    pkDest->m_uiCollisionGroup = m_uiCollisionGroup;
    pkDest->m_auiCollisionBits[0] = m_auiCollisionBits[0];
    pkDest->m_auiCollisionBits[1] = m_auiCollisionBits[1];
    pkDest->m_auiCollisionBits[2] = m_auiCollisionBits[2];
    pkDest->m_auiCollisionBits[3] = m_auiCollisionBits[3];
    pkDest->m_uiFlags = m_uiFlags;
    
    // Clone attachments
    unsigned int uiCount = m_kAttachments.GetSize();
    for (unsigned int ui = 0; ui < uiCount; ui++)
    {
        ClothAttachment* pkAttachment = m_kAttachments.GetAt(ui);
        if (!pkAttachment)
            continue;
        ClothAttachment* pkCopy = NiNew ClothAttachment;
        pkCopy->m_spShape = pkAttachment->m_spShape;
        pkCopy->m_uiVertexCount = pkAttachment->m_uiVertexCount;
        if (pkAttachment->m_uiVertexCount)
        {
            pkCopy->m_puiVertexIDs =
                NiAlloc(unsigned int, pkCopy->m_uiVertexCount);
            pkCopy->m_pkPositions =
                NiAlloc(NxVec3, pkCopy->m_uiVertexCount);
            pkCopy->m_puiFlags =
                NiAlloc(unsigned int, pkCopy->m_uiVertexCount);
            for (unsigned int uj = 0; uj < pkAttachment->m_uiVertexCount; uj++)
            {
                pkCopy->m_puiVertexIDs[uj] = pkAttachment->m_puiVertexIDs[uj];
                pkCopy->m_pkPositions[uj] = pkAttachment->m_pkPositions[uj];
                pkCopy->m_puiFlags[uj] = pkAttachment->m_puiFlags[uj];
            }
        }
        else
        {
            pkCopy->m_puiFlags = NiAlloc(unsigned int, 1);
            pkCopy->m_puiFlags[0] = pkAttachment->m_puiFlags[0];
        }
        pkDest->m_kAttachments.Add(pkCopy);
    }

    pkDest->m_pkCloth = 0;
    pkDest->m_pkParentActor = m_pkParentActor;
    pkDest->m_spDest = m_spDest;
}
//---------------------------------------------------------------------------
void NiPhysXClothDesc::ProcessClone(NiCloningProcess& kCloning)
{
    NiObject::ProcessClone(kCloning);

    bool bCloned;
    NiObject* pkCloneObj = 0;
    bCloned = kCloning.m_pkCloneMap->GetAt(this, pkCloneObj);
    if (!bCloned)
        return;
    NiPhysXClothDesc* pkClone = (NiPhysXClothDesc*)pkCloneObj;

    // Check for clones of other objects.
    if (m_spClothMesh)
    {
        NiObject* pkClonedMeshObj = 0;
        bCloned = kCloning.m_pkCloneMap->
            GetAt(m_spClothMesh, pkClonedMeshObj);        
        if (bCloned)
        {
            pkClone->m_spClothMesh = (NiPhysXMeshDesc*)pkClonedMeshObj;
        }
    }

    unsigned int uiCount = pkClone->m_kAttachments.GetSize();
    for (unsigned int ui = 0; ui < uiCount; ui++)
    {
        ClothAttachment* pkAttachment = pkClone->m_kAttachments.GetAt(ui);
        if (!pkAttachment->m_spShape)
            continue;
        bCloned = kCloning.m_pkCloneMap->
            GetAt(pkAttachment->m_spShape, pkCloneObj);        
        if (bCloned)
        {
            pkAttachment->m_spShape = (NiPhysXShapeDesc*)pkCloneObj;
        }
    }

    if (m_pkParentActor)
    {
        NiObject* pkClonedParentObj = 0;
        bCloned = kCloning.m_pkCloneMap->
            GetAt(m_pkParentActor, pkClonedParentObj);        
        if (bCloned)
        {
            pkClone->m_pkParentActor = (NiPhysXActorDesc*)pkClonedParentObj;
        }
    }
    
    if (m_spDest)
    {
        NiObject* pkClonedDestObj = 0;
        bCloned = kCloning.m_pkCloneMap->GetAt(m_spDest, pkClonedDestObj);
        if (bCloned)
        {
            pkClone->m_spDest = (NiPhysXClothDest*)pkClonedDestObj;
        }
    }
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Streaming Functions
//---------------------------------------------------------------------------
bool NiPhysXClothDesc::RegisterStreamables(NiStream& kStream)
{
    if (!NiObject::RegisterStreamables(kStream))
        return false;

    kStream.RegisterFixedString(m_kClothName);

    if (m_spClothMesh)
        m_spClothMesh->RegisterStreamables(kStream);
        
    unsigned int uiCount = m_kAttachments.GetSize();
    for (unsigned int ui = 0; ui < uiCount; ui++)
    {
        ClothAttachment* pkAttachment = m_kAttachments.GetAt(ui);
        if (pkAttachment && pkAttachment->m_spShape)
            pkAttachment->m_spShape->RegisterStreamables(kStream);
    }

    if (m_pkParentActor)
        m_pkParentActor->RegisterStreamables(kStream);

    if (m_spDest)
        m_spDest->RegisterStreamables(kStream);

    return true;
}
//---------------------------------------------------------------------------
void NiPhysXClothDesc::SaveBinary(NiStream& kStream)
{
    NiObject::SaveBinary(kStream);
    
    kStream.SaveFixedString(m_kClothName);

    kStream.SaveLinkID(m_spClothMesh);
    
    NiPhysXTypes::NxMat34SaveBinary(kStream, m_kGlobalPose);
    NiStreamSaveBinary(kStream, m_fThickness);
    NiStreamSaveBinary(kStream, m_fDensity);
    NiStreamSaveBinary(kStream, m_fBendingStiffness);
    NiStreamSaveBinary(kStream, m_fStretchingStiffness);
    NiStreamSaveBinary(kStream, m_fDampingCoefficient);
    NiStreamSaveBinary(kStream, m_fFriction);
    NiStreamSaveBinary(kStream, m_fPressure);
    NiStreamSaveBinary(kStream, m_fTearFactor);
    NiStreamSaveBinary(kStream, m_fCollisionResponseCoefficient);
    NiStreamSaveBinary(kStream, m_fAttachmentResponseCoefficient);
    NiStreamSaveBinary(kStream, m_fAttachmentTearFactor);
    NiStreamSaveBinary(kStream, m_uiSolverIterations);
    NiPhysXTypes::NxVec3SaveBinary(kStream, m_kExternalAcceleration);
    NiStreamSaveBinary(kStream, m_fWakeUpCounter);
    NiStreamSaveBinary(kStream, m_fSleepLinearVelocity);
    NiStreamSaveBinary(kStream, m_uiCollisionGroup);
    NiStreamSaveBinary(kStream, m_auiCollisionBits[0]);
    NiStreamSaveBinary(kStream, m_auiCollisionBits[1]);
    NiStreamSaveBinary(kStream, m_auiCollisionBits[2]);
    NiStreamSaveBinary(kStream, m_auiCollisionBits[3]);
    NiStreamSaveBinary(kStream, m_uiFlags);

    unsigned int uiCount = m_kAttachments.GetSize();
    NiStreamSaveBinary(kStream, m_kAttachments.GetEffectiveSize());
    for (unsigned int ui = 0; ui < uiCount; ui++)
    {
        ClothAttachment* pkAttachment = m_kAttachments.GetAt(ui);
        if (!pkAttachment)
            continue;
        kStream.SaveLinkID(pkAttachment->m_spShape);
        NiStreamSaveBinary(kStream, pkAttachment->m_uiVertexCount);
        for (unsigned int uj = 0; uj < pkAttachment->m_uiVertexCount; uj++)
        {
            NiStreamSaveBinary(kStream, pkAttachment->m_puiVertexIDs[uj]);
            NiPhysXTypes::NxVec3SaveBinary(kStream,
                pkAttachment->m_pkPositions[uj]);
            NiStreamSaveBinary(kStream, pkAttachment->m_puiFlags[uj]);
        }
        if (pkAttachment->m_uiVertexCount == 0)
            NiStreamSaveBinary(kStream, pkAttachment->m_puiFlags[0]);
    }

    kStream.SaveLinkID(m_pkParentActor);
    kStream.SaveLinkID(m_spDest);
}
//---------------------------------------------------------------------------
NiImplementCreateObject(NiPhysXClothDesc);
//---------------------------------------------------------------------------
void NiPhysXClothDesc::LoadBinary(NiStream& kStream)
{
    NiObject::LoadBinary(kStream);

    kStream.LoadFixedString(m_kClothName);

    m_spClothMesh = (NiPhysXMeshDesc*)kStream.ResolveLinkID();
    
    NiPhysXTypes::NxMat34LoadBinary(kStream, m_kGlobalPose);
    NiStreamLoadBinary(kStream, m_fThickness);
    NiStreamLoadBinary(kStream, m_fDensity);
    NiStreamLoadBinary(kStream, m_fBendingStiffness);
    NiStreamLoadBinary(kStream, m_fStretchingStiffness);
    NiStreamLoadBinary(kStream, m_fDampingCoefficient);
    NiStreamLoadBinary(kStream, m_fFriction);
    NiStreamLoadBinary(kStream, m_fPressure);
    NiStreamLoadBinary(kStream, m_fTearFactor);
    NiStreamLoadBinary(kStream, m_fCollisionResponseCoefficient);
    NiStreamLoadBinary(kStream, m_fAttachmentResponseCoefficient);
    NiStreamLoadBinary(kStream, m_fAttachmentTearFactor);
    NiStreamLoadBinary(kStream, m_uiSolverIterations);
    NiPhysXTypes::NxVec3LoadBinary(kStream, m_kExternalAcceleration);
    NiStreamLoadBinary(kStream, m_fWakeUpCounter);
    NiStreamLoadBinary(kStream, m_fSleepLinearVelocity);
    NiStreamLoadBinary(kStream, m_uiCollisionGroup);
    NiStreamLoadBinary(kStream, m_auiCollisionBits[0]);
    NiStreamLoadBinary(kStream, m_auiCollisionBits[1]);
    NiStreamLoadBinary(kStream, m_auiCollisionBits[2]);
    NiStreamLoadBinary(kStream, m_auiCollisionBits[3]);
    NiStreamLoadBinary(kStream, m_uiFlags);

    unsigned int uiCount;
    NiStreamLoadBinary(kStream, uiCount);
    m_kAttachments.SetSize(uiCount);
    for (unsigned int ui = 0; ui < uiCount; ui++)
    {
        ClothAttachment* pkAttachment = NiNew ClothAttachment;
        pkAttachment->m_spShape = (NiPhysXShapeDesc*)kStream.ResolveLinkID();
        NiStreamLoadBinary(kStream, pkAttachment->m_uiVertexCount);
        if (pkAttachment->m_uiVertexCount == 0)
        {
            pkAttachment->m_puiFlags = NiAlloc(unsigned int, 1);
            NiStreamLoadBinary(kStream, pkAttachment->m_puiFlags[0]);
        }
        else
        {
            pkAttachment->m_puiVertexIDs =
                NiAlloc(unsigned int, pkAttachment->m_uiVertexCount);
            pkAttachment->m_pkPositions =
                NiAlloc(NxVec3, pkAttachment->m_uiVertexCount);
            pkAttachment->m_puiFlags =
                NiAlloc(unsigned int, pkAttachment->m_uiVertexCount);
            for (unsigned int uj = 0; uj < pkAttachment->m_uiVertexCount; uj++)
            {
                NiStreamLoadBinary(kStream, pkAttachment->m_puiVertexIDs[uj]);
                NiPhysXTypes::NxVec3LoadBinary(kStream,
                    pkAttachment->m_pkPositions[uj]);
                NiStreamLoadBinary(kStream, pkAttachment->m_puiFlags[uj]);
            }
        }

        m_kAttachments.SetAt(ui, pkAttachment);
    }

    m_pkParentActor = (NiPhysXActorDesc*)kStream.ResolveLinkID();
    m_spDest = (NiPhysXClothDest*)kStream.ResolveLinkID();
}
//---------------------------------------------------------------------------
void NiPhysXClothDesc::LinkObject(NiStream& kStream)
{
    NiObject::LinkObject(kStream);
}
//---------------------------------------------------------------------------
bool NiPhysXClothDesc::IsEqual(NiObject* pkObject)
{
    NIASSERT(NiIsKindOf(NiPhysXClothDesc, pkObject));
    if(!NiObject::IsEqual(pkObject))
        return false;
        
    NiPhysXClothDesc* pkNewObject = (NiPhysXClothDesc*)pkObject;
    
    if (m_kClothName != pkNewObject->m_kClothName)
        return false;
    
    if (m_spClothMesh)
    {
        if (!pkNewObject->m_spClothMesh ||
            !m_spClothMesh->IsEqual(pkNewObject->m_spClothMesh))
        return false;
    }
    else
    {
        if (pkNewObject->m_spClothMesh)
            return false;
    }

    NxVec3 kDiffT = pkNewObject->m_kGlobalPose.t - m_kGlobalPose.t;
    if (kDiffT.magnitudeSquared() > 1.0e-8f)
        return false;
    NxMat33 kDiffM = pkNewObject->m_kGlobalPose.M - m_kGlobalPose.M;
    if (NiAbs(kDiffM.determinant() > 0.0001f))
        return false;
    
    NxVec3 kDiffA =
        pkNewObject->m_kExternalAcceleration - m_kExternalAcceleration;
    if (kDiffA.magnitudeSquared() > 1.0e-8f)
        return false;

    if (pkNewObject->m_fThickness != m_fThickness ||
        pkNewObject->m_fDensity != m_fDensity ||
        pkNewObject->m_fBendingStiffness != m_fBendingStiffness ||
        pkNewObject->m_fStretchingStiffness != m_fStretchingStiffness ||
        pkNewObject->m_fDampingCoefficient != m_fDampingCoefficient ||
        pkNewObject->m_fFriction != m_fFriction ||
        pkNewObject->m_fPressure != m_fPressure ||
        pkNewObject->m_fTearFactor != m_fTearFactor ||
        pkNewObject->m_fCollisionResponseCoefficient
            != m_fCollisionResponseCoefficient ||
        pkNewObject->m_fAttachmentResponseCoefficient !=
            m_fAttachmentResponseCoefficient ||
        pkNewObject->m_fAttachmentTearFactor != m_fAttachmentTearFactor ||
        pkNewObject->m_uiSolverIterations != m_uiSolverIterations ||
        pkNewObject->m_fWakeUpCounter != m_fWakeUpCounter ||
        pkNewObject->m_fSleepLinearVelocity != m_fSleepLinearVelocity ||
        pkNewObject->m_uiCollisionGroup != m_uiCollisionGroup ||
        pkNewObject->m_auiCollisionBits[0] != m_auiCollisionBits[0] ||
        pkNewObject->m_auiCollisionBits[1] != m_auiCollisionBits[1] ||
        pkNewObject->m_auiCollisionBits[2] != m_auiCollisionBits[2] ||
        pkNewObject->m_auiCollisionBits[3] != m_auiCollisionBits[3] ||
        pkNewObject->m_uiFlags != m_uiFlags)
        return false;
        
    unsigned int uiSizeThis = m_kAttachments.GetSize(); 
    unsigned int uiSizeThat = pkNewObject->m_kAttachments.GetSize();
    unsigned int uiThis = 0;
    unsigned int uiThat = 0;
    bool bDone = false;
    do
    {
        ClothAttachment* pkAttachmentThis = NULL;
        while (uiThis < uiSizeThis &&
            !(pkAttachmentThis = m_kAttachments.GetAt(uiThis)))
        {
            uiThis++;
        }

        ClothAttachment* pkAttachmentThat = NULL;
        while (uiThat < uiSizeThat &&
            !(pkAttachmentThat = pkNewObject->m_kAttachments.GetAt(uiThat)))
        {
            uiThat++;
        }
        if (uiThis == uiSizeThis && uiThat == uiSizeThat)
            bDone = true; // All is good
        else if (uiThis == uiSizeThis && uiThat != uiSizeThat)
            return false; // More systems in That than in This
        else if (uiThis != uiSizeThis && uiThat == uiSizeThat)
            return false; // More systems in This than in That
        else
        {
            if (pkAttachmentThis->m_spShape)
            {
                if (!pkAttachmentThat->m_spShape ||
                    !pkAttachmentThis->m_spShape->
                        IsEqual(pkAttachmentThat->m_spShape))
                {
                    return false;
                }
            }
            else if (pkAttachmentThat->m_spShape)
            {
                return false;
            }
            
            if (pkAttachmentThis->m_uiVertexCount !=
                pkAttachmentThat->m_uiVertexCount)
                return false;
            
            for (unsigned int ui = 0; ui < pkAttachmentThis->m_uiVertexCount;
                ui++)
            {
                NxVec3 kDiffP = pkAttachmentThat->m_pkPositions[ui]
                     - pkAttachmentThis->m_pkPositions[ui];
                if (kDiffP.magnitudeSquared() > 1.0e-8f)
                    return false;
                
                if (pkAttachmentThis->m_puiVertexIDs[ui] !=
                    pkAttachmentThat->m_puiVertexIDs[ui] ||
                    pkAttachmentThis->m_puiFlags[ui] !=
                    pkAttachmentThat->m_puiFlags[ui])
                {
                    return false;
                }
            }
            
            if (pkAttachmentThis->m_uiVertexCount == 0)
            {
                if (pkAttachmentThis->m_puiFlags[0] !=
                    pkAttachmentThat->m_puiFlags[0])
                {
                    return false;
                }
            }
        }    
        uiThis++;
        uiThat++;
    } while (!bDone);

    if (m_pkParentActor)
    {
        if (!pkNewObject->m_pkParentActor ||
            !m_pkParentActor->IsEqual(pkNewObject->m_pkParentActor))
        return false;
    }
    else
    {
        if (pkNewObject->m_pkParentActor)
            return false;
    }
    
    if (m_spDest)
    {
        if (!pkNewObject->m_spDest ||
            !m_spDest->IsEqual(pkNewObject->m_spDest))
        return false;
    }
    else
    {
        if (pkNewObject->m_spDest)
            return false;
    }
    
    return true;
}
//---------------------------------------------------------------------------
NiPhysXClothDesc::ClothAttachment::ClothAttachment()
{
    m_spShape = 0;
    m_uiVertexCount = 0;
    m_puiVertexIDs = 0;    
    m_pkPositions = 0;
    m_puiFlags = 0;
}
//---------------------------------------------------------------------------
NiPhysXClothDesc::ClothAttachment::~ClothAttachment()
{
    m_spShape = 0;
    NiFree(m_puiVertexIDs);
    NiFree(m_pkPositions);
    NiFree(m_puiFlags);
}
//---------------------------------------------------------------------------

