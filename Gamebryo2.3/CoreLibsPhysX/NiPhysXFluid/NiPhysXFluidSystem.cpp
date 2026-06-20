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
#include "NiPhysXFluidPCH.h"

#include "NiPhysXFluidSystem.h"
#include "NiPhysXFluidDest.h"

NiImplementRTTI(NiPhysXFluidSystem, NiParticleSystem);

//---------------------------------------------------------------------------
NiPhysXFluidSystem::NiPhysXFluidSystem(NxParticleData& kFluidData,
    unsigned short usMaxNumParticles, bool bHasColors, bool bHasRotations,
    bool bWorldSpace, NiPhysXProp* pkProp, bool bPhysXSpace) :
    NiParticleSystem(NiNew NiPhysXFluidData(kFluidData, usMaxNumParticles,
    bHasColors, bHasRotations), bWorldSpace)
{
    m_bPhysXSpace = bPhysXSpace;

    // Set up sources and destinations to get data into and out of PhysX
    if (pkProp)
    {
        AttachToProp(pkProp);
    }
}
//---------------------------------------------------------------------------
NiPhysXFluidSystem::NiPhysXFluidSystem(NiParticleSystem* pkPSys,
    NiPhysXProp* pkProp, bool bPhysXSpace,
    NiTMap<NiAVObject*, NiPhysXActorDescPtr>& kActorMap) : NiParticleSystem()
{
    // Clone to copy all the modifiers, controllers, shaders, etc.
    NiCloningProcess kCloning;
    kCloning.m_eCopyType = NiObjectNET::COPY_EXACT;
    kCloning.m_cAppendChar = '%';

    pkPSys->CopyMembers(this, kCloning);
    pkPSys->ProcessClone(kCloning);
    
    // Set the stuff the clone didn't catch
    m_bPhysXSpace = bPhysXSpace;

    // Replace the model data.
    m_spModelData = NiNew NiPhysXFluidData(
        NiDynamicCast(NiPSysData, m_spModelData));

    //
    // Create a fluid descriptor and add it to the scene
    //
    // First get the scene descriptor.
    NIASSERT(pkProp->GetSnapshot() &&
        NiIsKindOf(NiPhysXFluidPropDesc, pkProp->GetSnapshot()));
    NiPhysXFluidPropDesc* pkPropDesc =
        (NiPhysXFluidPropDesc*)pkProp->GetSnapshot();

    // Create the Fluid descriptor
    NiPhysXFluidDescPtr spFluidDesc = NiNew NiPhysXFluidDesc();
    spFluidDesc->SetName(GetName());
    spFluidDesc->SetParticleSystem(this);
    
    ConvertModifiers(this, pkProp, spFluidDesc, kActorMap);
    
    pkPropDesc->AddFluidDesc(spFluidDesc);

    // Set up the destinations to get data out of PhysX
    if (pkProp)
    {
        AttachToProp(pkProp);
    }
    else
    {
        m_spDestination = 0;
    }
}
//---------------------------------------------------------------------------
NiPhysXFluidSystem::NiPhysXFluidSystem()
{
    m_spDestination = 0;
}
//---------------------------------------------------------------------------
void NiPhysXFluidSystem::SetModelData(NiGeometryData* pkModelData,
    NiPhysXProp* pkProp)
{
    // The model data must be of type NiPhysXFluidData.
    NIASSERT(NiIsKindOf(NiPhysXFluidData, pkModelData));
    NiParticles::SetModelData(pkModelData);

    // Set up destination to get data out of PhysX
    AttachToProp(pkProp);
}
//---------------------------------------------------------------------------
void NiPhysXFluidSystem::AttachToProp(NiPhysXProp* pkProp)
{
    m_spDestination = NiNew NiPhysXFluidDest(this);
    pkProp->AddDestination(m_spDestination);
}
//---------------------------------------------------------------------------
void NiPhysXFluidSystem::DetachFromProp(NiPhysXProp* pkProp)
{
    pkProp->DeleteDestination(m_spDestination);
    m_spDestination = 0;
}
//---------------------------------------------------------------------------
void NiPhysXFluidSystem::ConvertModifiers(NiParticleSystem* pkTarget,
    NiPhysXProp* pkProp, NiPhysXFluidDescPtr spFluidDesc,
    NiTMap<NiAVObject*, NiPhysXActorDescPtr>& kActorMap)
{
    // Cloning misses setting the target on the controllers attached to the
    // particle system object. Go through and set them now.
    NiTimeController* pkController = pkTarget->GetControllers();
    while (pkController)
    {
        pkController->SetTarget(pkTarget);
        pkController = pkController->GetNext();
    }
    
    // We get these from the first emitter we find
    NiColorA kEmitColor(1.0f, 1.0f, 1.0f, 1.0f);
    float fEmitRad = 1.0f;
    float fEmitRadVar = 0.0f;
    
    // Find the excess modifiers and their controllers.
    // - Emitters
    // - NiPSysAgeDeathModifier
    // - NiPSysPositionModifier
    // - NiPSysSpawnModifier
    // - Any force modifier
    NiTObjectArray<NiPhysXFluidEmitterDescPtr> kEmitters;
    NiTPointerList<NiPSysModifierPtr> kVictimModifiers;
    NiTListIterator kListIter = pkTarget->m_kModifierList.GetHeadPos();
    while (kListIter)
    {
        NiPSysModifier* pkModifier =
            pkTarget->m_kModifierList.GetNext(kListIter);
        if (NiIsKindOf(NiPSysEmitter, pkModifier))
        {
            NiPSysEmitter* pkEmitter = (NiPSysEmitter*)pkModifier;

            kEmitColor = pkEmitter->GetInitialColor();
            fEmitRad = pkEmitter->GetInitialRadius();
            fEmitRadVar = pkEmitter->GetRadiusVar();

            // Create a fluid emitter for each volume emitter found
            float fInvScale = 1.0f;
            if (pkProp)
                fInvScale = 1.0f / pkProp->GetScaleFactor();
            ConvertEmitter(
                pkTarget, pkEmitter, fInvScale, kActorMap, kEmitters);

            kVictimModifiers.AddTail(pkModifier);
        }
        else if (NiIsKindOf(NiPSysPositionModifier, pkModifier) ||
            NiIsKindOf(NiPSysAgeDeathModifier, pkModifier) ||
            NiIsKindOf(NiPSysSpawnModifier, pkModifier) ||
            NiIsKindOf(NiPSysAirFieldModifier, pkModifier) ||
            NiIsKindOf(NiPSysBombModifier, pkModifier) ||
            NiIsKindOf(NiPSysDragModifier, pkModifier) ||
            NiIsKindOf(NiPSysFieldModifier, pkModifier) ||
            NiIsKindOf(NiPSysGravityModifier, pkModifier) ||
            NiIsKindOf(NiPSysRadialFieldModifier, pkModifier) ||
            NiIsKindOf(NiPSysVortexFieldModifier, pkModifier) ||
            NiIsKindOf(NiPSysColliderManager, pkModifier))
        {
            kVictimModifiers.AddTail(pkModifier);
        }
    }

    NiTPointerList<NiPSysModifierCtlrPtr> kVictimControllers;
    pkController = pkTarget->GetControllers();
    while (pkController)
    {
        if (NiIsKindOf(NiPSysModifierCtlr, pkController))
        {
            NiPSysModifierCtlr* pkModCtrl = (NiPSysModifierCtlr*)pkController;
            
            const NiFixedString kModName = pkModCtrl->GetModifierName();
            NiTListIterator kIter = kVictimModifiers.GetHeadPos();
            while (kIter)
            {
                NiPSysModifier* pkModifier = kVictimModifiers.GetNext(kIter);
                if (kModName == pkModifier->GetName())
                {
                    kVictimControllers.AddTail(pkModCtrl);
                    break;
                }
            }
        }
        pkController = pkController->GetNext();            
    }
    
    NiTListIterator kCtrlIter = kVictimControllers.GetHeadPos();
    while (kCtrlIter)
    {
        NiPSysModifierCtlr* pkModCtrl = kVictimControllers.GetNext(kCtrlIter);
        
        // Setting the target deletes the controller from the target's
        // list, and then removing all from the list below deletes it.
        pkModCtrl->SetTarget(0);
    }
    kVictimControllers.RemoveAll();
    
    NiTListIterator kModIter = kVictimModifiers.GetHeadPos();
    while (kModIter)
    {
        NiPSysModifier* pkModifier = kVictimModifiers.GetNext(kModIter);
        pkTarget->RemoveModifier(pkModifier);
    }
    kVictimModifiers.RemoveAll();
   
    // Add some modifiers
    // - NiPhysXFluidEmitter
    // - NiPhysXFluidAgeDeathModifier
    // - NiPhysXFluidUpdateModifier
    pkTarget->AddModifier(NiNew NiPhysXFluidEmitter("FluidEmitter", kEmitColor,
        fEmitRad, fEmitRadVar));
    pkTarget->AddModifier(NiNew NiPhysXFluidAgeDeathModifier("FluidAgeDeath"));
    pkTarget->AddModifier(NiNew NiPhysXFluidUpdateModifier("FluidUpdate"));
    
    spFluidDesc->SetEmitters(kEmitters);
}    
//---------------------------------------------------------------------------
void NiPhysXFluidSystem::ConvertEmitter(NiParticleSystem* pkTarget,
    NiPSysEmitter* pkEmitter, float fInvScale,
    NiTMap<NiAVObject*, NiPhysXActorDescPtr>& kActorMap,
    NiTObjectArray<NiPhysXFluidEmitterDescPtr>& kEmitters)
{
    if (NiIsKindOf(NiPSysVolumeEmitter, pkEmitter))
    {
        NiPSysVolumeEmitter* pkVolEmitter = (NiPSysVolumeEmitter*)pkEmitter;
        
        NiPhysXActorDescPtr spActorDesc = 0;
        NiPhysXShapeDescPtr spShapeDesc = 0;
        NxMat34 kPose = NiPhysXTypes::NXMAT34_ID;
        NiAVObject* pkEmitObj = pkVolEmitter->GetEmitterObj();
        if (pkEmitObj)
        {
            NiAVObject* pkEmitParent = pkEmitObj->GetParent();
            if (pkEmitParent && kActorMap.GetAt(pkEmitParent, spActorDesc))
            {
                // Need relative Xform
                NiMatrix3 kRotation = pkEmitObj->GetRotate();
                NiPoint3 kTranslation = pkEmitObj->GetTranslate();
                NxMat34 kActorXform;
                NiPhysXTypes::NiTransformToNxMat34(kRotation, kTranslation,
                    kActorXform);
                kActorXform.t *= fInvScale;
                
                spShapeDesc = spActorDesc->GetActorShapes().GetAt(0);
                NIASSERT(spShapeDesc);
                
                NxMat34 kShapeXform = spShapeDesc->GetLocalPose();
                kShapeXform.getInverse(kShapeXform);
                
                kPose = kShapeXform * kActorXform;
            }
            else
            {
                NiMatrix3 kRotation = pkEmitObj->GetWorldRotate();
                NiPoint3 kTranslation = pkEmitObj->GetWorldTranslate();
                NiPhysXTypes::NiTransformToNxMat34(kRotation, kTranslation,
                    kPose);
                kPose.t *= fInvScale;
            }
        }

        float fEmitSpeed = pkEmitter->GetSpeed() * fInvScale;
        float fEmitLife = pkEmitter->GetLifeSpan();
        
        bool bEllipse;
        float fDimX;
        float fDimY;
        if (NiIsKindOf(NiPSysBoxEmitter, pkVolEmitter))
        {
            fDimX = ((NiPSysBoxEmitter*)pkVolEmitter)->
                GetEmitterWidth() * fInvScale;
            fDimY = ((NiPSysBoxEmitter*)pkVolEmitter)->
                GetEmitterHeight() * fInvScale;
            bEllipse = false;
        }
        else if (NiIsKindOf(NiPSysSphereEmitter, pkVolEmitter))
        {
            fDimX = fDimY = ((NiPSysSphereEmitter*)pkVolEmitter)->
                GetEmitterRadius() * fInvScale;
            bEllipse = true;
        }
        else if (NiIsKindOf(NiPSysCylinderEmitter, pkVolEmitter))
        {
            fDimX = fDimY = ((NiPSysCylinderEmitter*)pkVolEmitter)->
                GetEmitterRadius() * fInvScale;
            bEllipse = true;
        }
        else
        {
            NIASSERT(false && "NiPhysXFluidSystem::ConvertModifiers"
                    && "Unknown type of emitter");
        }
        
        float fBirthRate = 0.0f;
        NiTimeController* pkController = pkTarget->GetControllers();
        while (pkController)
        {
            if (NiIsKindOf(NiPSysEmitterCtlr, pkController))
            {
                NiPSysEmitterCtlr* pkModCtrl =
                    (NiPSysEmitterCtlr*)pkController;
                if (pkModCtrl->GetModifierPointer() == pkEmitter)
                {
                    NiFloatInterpolator* pkInterp =
                        pkModCtrl->GetBirthRateInterpolator();
                    pkInterp->Update(0.0f, pkTarget, fBirthRate);
                }
                break;
            }
            pkController = pkController->GetNext();
        }
        
        NxFluidEmitterDesc kNxEmitDesc;
        kNxEmitDesc.setToDefault();
        kNxEmitDesc.rate = fBirthRate;
        kNxEmitDesc.fluidVelocityMagnitude = fEmitSpeed;
        kNxEmitDesc.particleLifetime = fEmitLife;
        kNxEmitDesc.type = NX_FE_CONSTANT_FLOW_RATE;
        kNxEmitDesc.dimensionX = fDimX * 0.5f;
        kNxEmitDesc.dimensionY = fDimY * 0.5f;
        if (bEllipse)
            kNxEmitDesc.shape = NX_FE_ELLIPSE;
        else
            kNxEmitDesc.shape = NX_FE_RECTANGULAR;
        kNxEmitDesc.relPose = kPose;
        
        NiPhysXFluidEmitterDesc* pkEmitterDesc =
            NiNew NiPhysXFluidEmitterDesc();
        pkEmitterDesc->SetNxFluidEmitterDesc(kNxEmitDesc);
        pkEmitterDesc->SetFrameShape(spShapeDesc);
        spActorDesc = 0;
        spShapeDesc = 0;

        kEmitters.AddFirstEmpty(pkEmitterDesc);
    }
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Streaming
//---------------------------------------------------------------------------
NiImplementCreateObject(NiPhysXFluidSystem);
//---------------------------------------------------------------------------
bool NiPhysXFluidSystem::StreamCanSkip()
{
    return true;
}
//---------------------------------------------------------------------------
void NiPhysXFluidSystem::LoadBinary(NiStream& kStream)
{
    NiParticleSystem::LoadBinary(kStream);

    m_spDestination = (NiPhysXFluidDest*)kStream.ResolveLinkID();

    NiBool bVal;
    NiStreamLoadBinary(kStream, bVal);
    m_bPhysXSpace = ( bVal != 0 );
}
//---------------------------------------------------------------------------
void NiPhysXFluidSystem::LinkObject(NiStream& kStream)
{
    NiParticleSystem::LinkObject(kStream);

}
//---------------------------------------------------------------------------
bool NiPhysXFluidSystem::RegisterStreamables(NiStream& kStream)
{
    if (!NiParticleSystem::RegisterStreamables(kStream))
    {
        return false;
    }

    if (m_spDestination)
        m_spDestination->RegisterStreamables(kStream);

    return true;
}
//---------------------------------------------------------------------------
void NiPhysXFluidSystem::SaveBinary(NiStream& kStream)
{
    NiParticleSystem::SaveBinary(kStream);

    kStream.SaveLinkID(m_spDestination);

    NiStreamSaveBinary(kStream, NiBool(m_bPhysXSpace));
}
//---------------------------------------------------------------------------
bool NiPhysXFluidSystem::IsEqual(NiObject* pkObject)
{
    if (!NiParticleSystem::IsEqual(pkObject))
    {
        return false;
    }

    NiPhysXFluidSystem* pkNewObject = (NiPhysXFluidSystem*)pkObject;
    
    if (m_bPhysXSpace != pkNewObject->m_bPhysXSpace)
        return false;

    return true;
}
//---------------------------------------------------------------------------
