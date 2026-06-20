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

#include "MaxImmerse.h"

#include "NiMAXPSysConverter.h"
#include "NiMAXCustAttribConverter.h"
#include "NiMAXHierConverter.h"
#include "NiMAXMaterialConverter.h"
#include "NiMAXMeshConverter.h"
#include "NiUtils.h"
#include "NiMAXAnimationConverter.h"

#define PDRAGMOD_CLASS_ID Class_ID(705380396, 1574462936)

static float sfTimeFactor = 1.0f / 4800.0f;

NiTPointerList<NiMAXPSysConverter::ParticleLinkUpInfo *>
    NiMAXPSysConverter::ms_kLinkUpList;
NiTPointerList<NiMAXPSysConverter::ParticleRemapInfo *>
    NiMAXPSysConverter::ms_kRemapList;
NiTPointerList<NiMAXPSysConverter::ParticleStaticBoundInfo *>
    NiMAXPSysConverter::ms_kStaticBoundList;

unsigned int NiMAXPSysConverter::ms_uiTotalCount = 0;

extern const char* gpcMultiMtlName;

#define PSYS_INTERVAL_TIME 0.00001f
#define ALWAYS_UPDATE_BOUNDS 1
#define STATIC_BOUNDS        2
//---------------------------------------------------------------------------
NiMAXPSysConverter::NiMAXPSysConverter(TimeValue kAnimStart, 
    TimeValue kAnimEnd)
{
    m_kAnimStart = kAnimStart;
    m_kAnimEnd = kAnimEnd;
}
//---------------------------------------------------------------------------
bool NiMAXPSysConverter::IsPSys(INode* pkMaxNode)
{
    Class_ID eID = pkMaxNode->GetObjectRef()->ClassID();

    return ((eID == Class_ID(RAIN_CLASS_ID, 0) ||
        eID == Class_ID(SNOW_CLASS_ID, 0) ||
        eID == PCLOUD_CLASS_ID ||
        eID == SUPRSPRAY_CLASS_ID ||
        eID == BLIZZARD_CLASS_ID ||
        eID == PArray_CLASS_ID));
}
//---------------------------------------------------------------------------
// When using instanced geometry we need to remove the root transform
// of the subtree. To do so we create a new root and copy all the
// data to it.
static int ReplaceRoot(NiNodePtr* pspRoot)
{
    CHECK_MEMORY();
    NiNode* pkNewRoot;
    if (NiIsExactKindOf(NiBillboardNode, (*pspRoot)))
    {
        NiBillboardNode* pkBB;

        pkNewRoot = pkBB = NiNew NiBillboardNode;
        pkBB->SetMode(NiBillboardNode::ALWAYS_FACE_CAMERA);
    }
    else
    {
        pkNewRoot = NiNew NiNode;
    }
    if (pkNewRoot == NULL)
        return(W3D_STAT_NO_MEMORY);

    pkNewRoot->SetName("NewRoot");

    NiAVObject* pkChild;
    for (unsigned int i = 0; i < (*pspRoot)->GetArrayCount(); i++)
    {
        pkChild = (*pspRoot)->GetAt(i);
        if (pkChild)
            pkNewRoot->AttachChild((NiAVObject*) pkChild->Clone());
    }

    NiPropertyList* pkPropList = &((*pspRoot)->GetPropertyList());
    NiTListIterator kIter = pkPropList->GetHeadPos();
    while (kIter)
    {
        NiProperty* pkProperty = pkPropList->GetNext(kIter);

        if(pkProperty)
            pkNewRoot->AttachProperty(pkProperty);
   }

    *pspRoot = pkNewRoot;

    // don't cleanup or remove pOldRoot because it is in the 
    // Node Hash table and messing w/ it could cause trouble
    // elsewhere. It will get removed when the hash table is 
    // freed

    return(W3D_STAT_OK);
}
//---------------------------------------------------------------------------
NiMAXPSysConverter::NiPSysDesc::NiPSysDesc()
{
    CHECK_MEMORY();
    m_spSpeedInterp = NULL;
    m_fSpeed = 0.0f;
    m_fSpeedVar = 0.0f;
    m_fDirDeclination = 0.0f;
    m_fDirDeclinationVar = 0.0f;
    m_fDirPlanarAngle = 0.0f;
    m_fDirPlanarAngleVar = 0.0f;

    m_fSize = 0.0f;
    m_fSizeVar = 0.0f;

    m_fEmitStart = 0.0f;
    m_fEmitStop = 0.0f;

    m_spBirthRateInterp = NULL;
    m_fBirthRate = 0.0f;
    m_fLifeSpan = 0.0f;
    m_fLifeSpanVar = 0.0f;
    m_bUseBirthRate = false;
    m_bSpawnOnDeath = false;

    m_eEmitterType = EMITTER_VOLUME;
    m_pkMAXEmitter = 0;

    m_eInitVelocityType = NiPSysMeshEmitter::NI_VELOCITY_USE_NORMALS; 
    m_eEmissionType = NiPSysMeshEmitter::NI_EMIT_FROM_VERTICES;
    m_kEmitAxis = NiPoint3::UNIT_X;

    m_fEmitterWidth = 0.0f;
    m_fEmitterHeight = 0.0f;
    m_fEmitterDepth = 0.0f;

    
    m_bDieOnCollide = false;
    m_bSpawnOnCollide = false;
    m_usNumGenerations = 0;
    m_fPercentageSpawned = 0.0f;
    m_usMultiplier = 0;
    m_fSpeedChaos = 0.0f;
    m_fDirChaos = 0.0f;

    m_usRendererTotal = 0;
    m_usViewportTotal = 0;

    m_fGrowFor = 0.0f;
    m_fFadeFor = 0.0f;
   
    m_uiNumColorKeys = 0;
    m_bColorAges = false;

    m_usNumMeshes = 0;
    m_pspMeshes = NULL;

    m_fSpinTime = 0.0f;
    m_fSpinTimeVar = 0.0f;
    m_bRandomSpinAxis = false;
    m_kSpinAxis = NiPoint3::ZERO;
    m_fSpinAxisVar = 0.0f;

    m_fSpinPhase = 0.0f;
    m_fSpinPhaseVar = 0.0f;

    m_usNumFrags = 0;
    m_pkPos = 0;
    m_pkVel = 0;
    m_pspFrag = NULL;

    m_spColliderManager = NULL;
    m_spEmitter = NULL;

    m_bLocalCoords = false; 
    m_iBoundsUpdateType = 1;
    m_kBoundPosition = NiPoint3(0.0f,0.0f,0.0f);
    m_fBoundRadius = 1.0f;
    m_iBoundsDynamicSkip = 0;
    m_pkPSysRoot = NULL;
    m_eDeclinationInterpCycle = NiTimeController::CLAMP;
    m_eSpeedInterpCycle = NiTimeController::CLAMP;
    m_ePlanarAngleInterpCycle = NiTimeController::CLAMP;
    m_eSizeInterpCycle = NiTimeController::CLAMP;
    m_eLifeSpanInterpCycle = NiTimeController::CLAMP;
    m_eBirthRateInterpCycle = NiTimeController::CLAMP;

    m_fNiPSysRotationSpeed = 0.0f;
    m_eRotationSpeedInterpCycle = NiTimeController::CLAMP;
    m_fNiPSysRotationVariation = 0.0f;
    m_eRotationVariationInterpCycle = NiTimeController::CLAMP;
    m_bNiPSysBidirectionalRotation = false;
    m_eDeclinationVarInterpCycle = NiTimeController::CLAMP;
    m_ePlanarAngleVarInterpCycle = NiTimeController::CLAMP;
    m_eSpinPhaseInterpCycle = NiTimeController::CLAMP;
    m_eSpinPhaseVarInterpCycle = NiTimeController::CLAMP;
}
//---------------------------------------------------------------------------
NiMAXPSysConverter::NiPSysDesc::~NiPSysDesc()
{
    CHECK_MEMORY();
    unsigned short i;
    for (i = 0; i < m_usNumMeshes; i++)
    {
        m_pspMeshes[i] = 0;
    }
    NiDelete [] m_pspMeshes;

    for (i = 0; i < m_usNumFrags; i++)
    {
        m_pspFrag[i] = 0;
    }
    NiDelete[] m_pkPos;
    NiDelete[] m_pkVel;
    NiFree(m_pspFrag);
}
//---------------------------------------------------------------------------
NiMAXPSysConverter::ParticleLinkUpInfo::ParticleLinkUpInfo()
{
    m_spSystem = NULL;
    m_spModifier = NULL;
    m_spCollider = NULL;
    m_pkMAXNode = NULL;
}

//---------------------------------------------------------------------------
NiMAXPSysConverter::ParticleRemapInfo::ParticleRemapInfo()
{
    m_spSystem = NULL;
    m_pkMAXNode = NULL;
}
//---------------------------------------------------------------------------
NiMAXPSysConverter::ParticleStaticBoundInfo::ParticleStaticBoundInfo()
{
    m_spSystem = NULL;
    m_spOldParent = NULL;
}
//---------------------------------------------------------------------------
int NiMAXPSysConverter::ExtractSuperSpray(Interface* pkIntf, 
    SimpleParticle* pkPart, NiPSysDesc* pkDesc, ScaleInfo* pkParentScale)
{
    CHECK_MEMORY();
    CommonParticle* pkCommon = (CommonParticle*) pkPart;
    IParamBlock* pkPBlock = pkPart->pblock;
    Class_ID eID = pkPart->ClassID();

    // Basic Parameters Rollout
    float fDirDeclination, fDirDeclinationVar;
    pkPBlock->GetValue(PBBLIZ_OFFAXIS, 0, fDirDeclination, FOREVER);
    pkPBlock->GetValue(PBBLIZ_AXISSPREAD, 0, fDirDeclinationVar, FOREVER);
    float fDirPlanarAngle, fDirPlanarAngleVar;
    pkPBlock->GetValue(PBBLIZ_OFFPLANE, 0, fDirPlanarAngle, FOREVER);
    pkPBlock->GetValue(PBBLIZ_PLANESPREAD, 0, fDirPlanarAngleVar, FOREVER);
    float fWidth;
    pkPBlock->GetValue(PBBLIZ_EMITRWID, 0, fWidth, FOREVER);
    int iShowEmitter, iViewportDisplayType;
    pkPBlock->GetValue(PBBLIZ_EMITRHID, 0, iShowEmitter, FOREVER);
    pkPBlock->GetValue(PBBLIZ_VIEWPORTSHOWS, 0, iViewportDisplayType, 
        FOREVER);
    float fPercentageShown;
    pkPBlock->GetValue(PBBLIZ_DISPLAYPORTION, 0, fPercentageShown, FOREVER);
                        
    // Particle Generation Rollout
    int iLimitUsingTotal, iBirthRate, iTotalParticles;
    pkPBlock->GetValue(PBBLIZ_BIRTHMETHOD, 0, iLimitUsingTotal, FOREVER);
    pkPBlock->GetValue(PBBLIZ_PBIRTHRATE, 0, iBirthRate, FOREVER);
    
    pkPBlock->GetValue(PBBLIZ_PTOTALNUMBER, 0, iTotalParticles, FOREVER);
    float fSpeed, fSpeedVar;
    pkPBlock->GetValue(PBBLIZ_SPEED, 0, fSpeed, FOREVER);
    pkPBlock->GetValue(PBBLIZ_SPEEDVAR, 0, fSpeedVar, FOREVER);
    TimeValue kEmitStart, kEmitStop;
    pkPBlock->GetValue(PBBLIZ_EMITSTART, 0, kEmitStart, FOREVER);
    pkPBlock->GetValue(PBBLIZ_EMITSTOP, 0, kEmitStop, FOREVER);
    TimeValue kDisplayUntil, kLifeSpan, kLifeSpanVar;
    pkPBlock->GetValue(PBBLIZ_DISPUNTIL, 0, kDisplayUntil, FOREVER);
    pkPBlock->GetValue(PBBLIZ_LIFE, 0, kLifeSpan, FOREVER);
    pkPBlock->GetValue(PBBLIZ_LIFEVAR, 0, kLifeSpanVar, FOREVER);
    int iSubFrameTime, iSubFrameTrans, iSubFrameRot;
    pkPBlock->GetValue(PBBLIZ_SUBFRAMETIME, 0, iSubFrameTime, FOREVER);
    pkPBlock->GetValue(PBBLIZ_SUBFRAMEMOVE, 0, iSubFrameTrans, FOREVER);
    pkPBlock->GetValue(PBBLIZ_SUBFRAMEROT, 0, iSubFrameRot, FOREVER);
    float fSize, fSizeVar;
    pkPBlock->GetValue(PBBLIZ_SIZE, 0, fSize, FOREVER);
    pkPBlock->GetValue(PBBLIZ_SIZEVAR, 0, fSizeVar, FOREVER);
    TimeValue kGrowFor, kFadeFor;
    pkPBlock->GetValue(PBBLIZ_GROWTIME, 0, kGrowFor, FOREVER);
    pkPBlock->GetValue(PBBLIZ_FADETIME, 0, kFadeFor, FOREVER);
    int iRandSeed;
    pkPBlock->GetValue(PBBLIZ_RNDSEED, 0, iRandSeed, FOREVER);

    // Particle Type Rollout
    int iParticleType, iStandardParticleType;
    pkPBlock->GetValue(PBBLIZ_PARTICLECLASS, 0, iParticleType, FOREVER);
    pkPBlock->GetValue(PBBLIZ_PARTICLETYPE, 0, iStandardParticleType, 
        FOREVER);
    float fMetaTension, fMetaTensionVar;
    pkPBlock->GetValue(PBBLIZ_METATENSION, 0, fMetaTension, FOREVER);
    pkPBlock->GetValue(PBBLIZ_METATENSIONVAR, 0, fMetaTensionVar, FOREVER);
    float fMetaCourseness, fMetaViewportCourseness;
    pkPBlock->GetValue(PBBLIZ_METACOURSE, 0, fMetaCourseness, FOREVER);
    pkPBlock->GetValue(PBBLIZ_METACOURSEV, 0, fMetaViewportCourseness, 
        FOREVER);
    int iMetaAutoCoarseness;
    pkPBlock->GetValue(PBBLIZ_METAAUTOCOARSE, 0, iMetaAutoCoarseness, 
        FOREVER);
    int iInstancedSubtree;
    pkPBlock->GetValue(PBBLIZ_USESUBTREE, 0, iInstancedSubtree, FOREVER);
    int iInstancedOffsetType;
    pkPBlock->GetValue(PBBLIZ_ANIMATIONOFFSET, 0, iInstancedOffsetType, 
        FOREVER);
    float fInstancedOffsetAmount;
    pkPBlock->GetValue(PBBLIZ_OFFSETAMOUNT, 0, fInstancedOffsetAmount, 
        FOREVER);
    int iMaterialMappingType;
    pkPBlock->GetValue(PBBLIZ_MAPPINGTYPE, 0, iMaterialMappingType, FOREVER);
    TimeValue kMappingTime;
    pkPBlock->GetValue(PBBLIZ_MAPPINGTIME, 0, kMappingTime, FOREVER);
    float fMappingDistance;
    pkPBlock->GetValue(PBBLIZ_MAPPINGDIST, 0, fMappingDistance, FOREVER);

    // Rotation And Collision Rollout
    TimeValue kSpinTime;
    pkPBlock->GetValue(PBBLIZ_SPINTIME, 0, kSpinTime, FOREVER);
    float fSpinTimeVar;
    pkPBlock->GetValue(PBBLIZ_SPINTIMEVAR, 0, fSpinTimeVar, FOREVER);
    float fSpinPhase, fSpinPhaseVar;
    pkPBlock->GetValue(PBBLIZ_SPINPHASE, 0, fSpinPhase, FOREVER);
    pkPBlock->GetValue(PBBLIZ_SPINPHASEVAR, 0, fSpinPhaseVar, FOREVER);
    int iSpinAxisType;
    pkPBlock->GetValue(PBBLIZ_SPINAXISTYPE, 0, iSpinAxisType, FOREVER);
    float fSpinStretch;
    pkPBlock->GetValue(PBBLIZ_STRETCH, 0, fSpinStretch, FOREVER);
    NiPoint3 kSpinAxis;
    pkPBlock->GetValue(PBBLIZ_SPINAXISX, 0, kSpinAxis.x, FOREVER);
    pkPBlock->GetValue(PBBLIZ_SPINAXISY, 0, kSpinAxis.y, FOREVER);
    pkPBlock->GetValue(PBBLIZ_SPINAXISZ, 0, kSpinAxis.z, FOREVER);
    float fSpinAxisVar;
    pkPBlock->GetValue(PBBLIZ_SPINAXISVAR, 0, fSpinAxisVar, FOREVER);
    int iParticleCollisionOn;
    pkPBlock->GetValue(PBBLIZ_IPCOLLIDE_ON, 0, iParticleCollisionOn, 
        FOREVER);
    int iParticleCollisionSteps;
    pkPBlock->GetValue(PBBLIZ_IPCOLLIDE_STEPS, 0, iParticleCollisionSteps, 
        FOREVER);
    float fParticleCollisionBounce, fParticleCollisionBounceVar;
    pkPBlock->GetValue(PBBLIZ_IPCOLLIDE_BOUNCE, 0, fParticleCollisionBounce, 
        FOREVER);
    pkPBlock->GetValue(PBBLIZ_IPCOLLIDE_BOUNCEVAR, 0, 
        fParticleCollisionBounceVar, FOREVER);

    // Object Motion Inheritance Rollout
    float fOMIInfluence, fOMIMultiplier, fOMIMultiplierVar;
    pkPBlock->GetValue(PBBLIZ_EMITVINFL, 0, fOMIInfluence, FOREVER);
    pkPBlock->GetValue(PBBLIZ_EMITVMULT, 0, fOMIMultiplier, FOREVER);
    pkPBlock->GetValue(PBBLIZ_EMITVMULTVAR, 0, fOMIMultiplierVar, FOREVER);

    // Bubble Motion Rollout
    float fBubbleAmp, fBubbleAmpVar;
    pkPBlock->GetValue(PBBLIZ_BUBLAMP, 0, fBubbleAmp, FOREVER);
    pkPBlock->GetValue(PBBLIZ_BUBLAMPVAR, 0, fBubbleAmpVar, FOREVER);
    TimeValue kBubblePeriod;
    pkPBlock->GetValue(PBBLIZ_BUBLPER, 0, kBubblePeriod, FOREVER);
    float fBubblePeriodVar;
    pkPBlock->GetValue(PBBLIZ_BUBLPERVAR, 0, fBubblePeriodVar, FOREVER);
    float fBubblePhase, fBubblePhaseVar;
    pkPBlock->GetValue(PBBLIZ_BUBLPHAS, 0, fBubblePhase, FOREVER);
    pkPBlock->GetValue(PBBLIZ_BUBLPHASVAR, 0, fBubblePhaseVar, FOREVER);

    // Particle Spawn Rollout
    int iSpawnType;
    pkPBlock->GetValue(PBBLIZ_SPAWNTYPE, 0, iSpawnType, FOREVER);
    TimeValue kPersist;
    pkPBlock->GetValue(PBBLIZ_SSSPAWNDIEAFTER, 0, kPersist, FOREVER);
    float fPersistVar;
    pkPBlock->GetValue(PBBLIZ_SSSPAWNDIEAFTERVAR, 0, fPersistVar, FOREVER);
    int iNumSpawns;
    pkPBlock->GetValue(PBBLIZ_SPAWNGENS, 0, iNumSpawns, FOREVER);
    int iSpawnPercent;
    pkPBlock->GetValue(PBBLIZ_SPAWNPERCENT, 0, iSpawnPercent, FOREVER);
    int iSpawnMultiplier;
    pkPBlock->GetValue(PBBLIZ_SPAWNCOUNT, 0, iSpawnMultiplier, FOREVER);
    float fSpawnMultiplierVar;
    pkPBlock->GetValue(PBBLIZ_SPAWNMULTVAR, 0, fSpawnMultiplierVar, FOREVER);
    float fSpawnDirChaos;
    pkPBlock->GetValue(PBBLIZ_SPAWNDIRCHAOS, 0, fSpawnDirChaos, FOREVER);
    float fSpawnSpeedChaos;
    pkPBlock->GetValue(PBBLIZ_SPAWNSPEEDCHAOS, 0, fSpawnSpeedChaos, FOREVER);
    int iSpawnSpeedSign;
    pkPBlock->GetValue(PBBLIZ_SPAWNSPEEDSIGN, 0, iSpawnSpeedSign, FOREVER);
    int iSpawnInheritSpeed;
    pkPBlock->GetValue(PBBLIZ_SPAWNINHERITV, 0, iSpawnInheritSpeed, FOREVER);
    int iSpawnFixedSpeed;
    pkPBlock->GetValue(PBBLIZ_SPAWNSPEEDFIXED, 0, iSpawnFixedSpeed, FOREVER);
    float fSpawnScaleChaos;
    pkPBlock->GetValue(PBBLIZ_SPAWNSCALECHAOS, 0, fSpawnScaleChaos, FOREVER);
    int iSpawnScaleSign;
    pkPBlock->GetValue(PBBLIZ_SPAWNSCALESIGN, 0, iSpawnScaleSign, FOREVER);
    int iSpawnFixedScale;
    pkPBlock->GetValue(PBBLIZ_SPAWNSCALEFIXED, 0, iSpawnFixedScale, FOREVER);
    TimeValue kSpawnLifeValue;
    pkPBlock->GetValue(PBBLIZ_SPAWNLIFEVLUE, 0, kSpawnLifeValue, FOREVER);

    // Unknown
    int iCustomMtl, iPCNotDraft;
    pkPBlock->GetValue(PBBLIZ_CUSTOMMTL, 0, iCustomMtl, FOREVER);
    pkPBlock->GetValue(PBBLIZ_SSNOTDRAFT, 0, iPCNotDraft, FOREVER);

    // Set up parameters
    pkDesc->m_fSpeed = fSpeed * GetFrameRate();
    // we must account for the fact that NiParticle calculates speed by
    // taking Speed + SpeedVar * (UnitRandom - 0.5);
    pkDesc->m_fSpeedVar = fSpeedVar * pkDesc->m_fSpeed * 2.0f;

    pkDesc->m_fDirDeclination = fDirDeclination;
    pkDesc->m_fDirDeclinationVar = fDirDeclinationVar;
    pkDesc->m_fDirPlanarAngle = fDirPlanarAngle;
    pkDesc->m_fDirPlanarAngleVar = fDirPlanarAngleVar;

    pkDesc->m_fSize = fSize;
    // account for SizeVar being in units (not %) in engine
    pkDesc->m_fSizeVar = fSizeVar * pkDesc->m_fSize;

    pkDesc->m_fEmitStart = kEmitStart * sfTimeFactor;
    pkDesc->m_fEmitStop = kEmitStop * sfTimeFactor;

    if (pkDesc->m_bNiPSysUseViewport)
        pkDesc->m_fBirthRate = fPercentageShown * iBirthRate * GetFrameRate();
    else 
        pkDesc->m_fBirthRate = (float)iBirthRate * GetFrameRate();

    pkDesc->m_fLifeSpan = kLifeSpan * sfTimeFactor;
    // we must account for the fact that NiParticle calculates lifespan by
    // taking Lifespan + LifeSpanVar * (UnitRandom - 0.5);
    pkDesc->m_fLifeSpanVar = kLifeSpanVar * sfTimeFactor * 2.0f;
    pkDesc->m_bUseBirthRate = true;
    pkDesc->m_bSpawnOnDeath = (iSpawnType == 3);

    if (pkDesc->m_fEmitStart == pkDesc->m_fEmitStop)
    {
        pkDesc->m_fEmitStop = pkDesc->m_fEmitStart + PSYS_INTERVAL_TIME;
        if (iLimitUsingTotal == 0)
            iTotalParticles = iBirthRate;

        iLimitUsingTotal = 1;
    }

    if (iLimitUsingTotal != 0)
    {
        float fLifetime = pkDesc->m_fEmitStop - pkDesc->m_fEmitStart;

        if (pkDesc->m_bNiPSysUseViewport)
        {
            pkDesc->m_fBirthRate = fPercentageShown * iTotalParticles /  
                fLifetime;
        }
        else
        {
            pkDesc->m_fBirthRate = iTotalParticles /  fLifetime;
        }
    }

    if (IsTrue(NiMAXOptions::GetValue(NI_USEAREASUPERSPRAY)))
    {
        pkDesc->m_fEmitterWidth = fWidth;
        pkDesc->m_fEmitterHeight = fWidth;
    }
    else
    {
        pkDesc->m_fEmitterWidth = 0.0f;
        pkDesc->m_fEmitterHeight = 0.0f;
    }

    pkDesc->m_fEmitterDepth = 0.0f;

    pkDesc->m_bDieOnCollide = (iSpawnType == 1 || iSpawnType == 2);
    pkDesc->m_bSpawnOnCollide = (iSpawnType == 2);
    pkDesc->m_usNumGenerations = (iSpawnType == 0 ? 0 : iNumSpawns);
    pkDesc->m_fPercentageSpawned = (float)iSpawnPercent * 0.01f;
    pkDesc->m_usMultiplier = iSpawnMultiplier;
    pkDesc->m_fMultiplierVar = fSpawnMultiplierVar;
    pkDesc->m_fSpeedChaos = fSpawnSpeedChaos * 0.01f;
    switch (iSpawnSpeedSign)
    {
        case 0:
            pkDesc->m_fSpeedChaos *= -1.0f;
            break;
        case 1:
        case 2:
            break;
    };
    pkDesc->m_fDirChaos = fSpawnDirChaos;

    pkDesc->m_usViewportTotal = (unsigned short)
        (fPercentageShown * iTotalParticles);
    pkDesc->m_usRendererTotal = (unsigned short)
        (iTotalParticles);

    pkDesc->m_fGrowFor = kGrowFor * sfTimeFactor;
    pkDesc->m_fFadeFor = kFadeFor * sfTimeFactor;

    pkDesc->m_fSpinTime = kSpinTime * sfTimeFactor;
    pkDesc->m_fSpinTimeVar = fSpinTimeVar;
    pkDesc->m_bRandomSpinAxis = (iSpinAxisType != 2);
    pkDesc->m_kSpinAxis = kSpinAxis;
    pkDesc->m_fSpinAxisVar = fSpinAxisVar;
    pkDesc->m_fSpinPhase = fSpinPhase;
    pkDesc->m_fSpinPhaseVar = fSpinPhaseVar;

    if (iParticleType == 2 && pkCommon->custnode)
    {
        // Instanced particles
        NiMAXHierConverter kHier(m_kAnimStart, m_kAnimEnd);

        unsigned int uiNumMutations = pkCommon->nlist.Count();

        pkDesc->m_usNumMeshes = uiNumMutations + 1;

        pkDesc->m_pspMeshes = NiNew NiAVObjectPtr[uiNumMutations + 1];

        NiNodePtr spResult;
        for (unsigned int i = 0; i < uiNumMutations; i++)
        {
            int iStatus = kHier.BuildNode(pkIntf, pkCommon->nlist[i], 
                &spResult, pkParentScale, false, false);
            ReplaceRoot(&spResult);
            pkDesc->m_pspMeshes[i + 1] = spResult;
        }

        int iStatus = kHier.BuildNode(pkIntf, pkCommon->custnode, &spResult, 
            pkParentScale, false, false);
        ReplaceRoot(&spResult);
        pkDesc->m_pspMeshes[0] = spResult;
    }

    // Extract animations
    if (iLimitUsingTotal == 0)
    {
        float fBRScalar = 1.0f;
        if (pkDesc->m_bNiPSysUseViewport)
            fBRScalar = fPercentageShown * GetFrameRate();
        else 
            fBRScalar = (float) GetFrameRate();

        pkDesc->m_spBirthRateInterp = BuildFloatInterp(pkPBlock, 
            PBBLIZ_PBIRTHRATE, pkDesc->m_eBirthRateInterpCycle, fBRScalar);
    }
    pkDesc->m_spLifeSpanInterp = BuildFloatInterp(pkPBlock, PBBLIZ_LIFE, 
        pkDesc->m_eLifeSpanInterpCycle, sfTimeFactor);
    pkDesc->m_spSpeedInterp = BuildFloatInterp(pkPBlock, PBBLIZ_SPEED, 
        pkDesc->m_eSpeedInterpCycle, (float) GetFrameRate());
    pkDesc->m_spSizeInterp = BuildFloatInterp(pkPBlock, PBBLIZ_SIZE,
        pkDesc->m_eSizeInterpCycle);
    pkDesc->m_spPlanarAngleInterp = BuildFloatInterp(pkPBlock, 
        PBBLIZ_OFFPLANE, pkDesc->m_ePlanarAngleInterpCycle);
    pkDesc->m_spPlanarAngleVarInterp = BuildFloatInterp(pkPBlock, 
        PBBLIZ_PLANESPREAD, pkDesc->m_ePlanarAngleVarInterpCycle);
    pkDesc->m_spDeclinationInterp = BuildFloatInterp(pkPBlock, 
        PBBLIZ_OFFAXIS, pkDesc->m_eDeclinationInterpCycle);
    pkDesc->m_spDeclinationVarInterp = BuildFloatInterp(pkPBlock,
        PBBLIZ_AXISSPREAD, pkDesc->m_eDeclinationVarInterpCycle);
    pkDesc->m_spSpinPhaseInterp = BuildFloatInterp(pkPBlock, PBBLIZ_SPINPHASE,
        pkDesc->m_eSpinPhaseInterpCycle);
    pkDesc->m_spSpinPhaseVarInterp = BuildFloatInterp(pkPBlock, 
        PBBLIZ_SPINPHASEVAR, pkDesc->m_eSpinPhaseVarInterpCycle);

    return(W3D_STAT_OK);
}
//---------------------------------------------------------------------------
int NiMAXPSysConverter::ExtractBlizzard(Interface* pkIntf, 
    SimpleParticle* pkPart, NiPSysDesc* pkDesc, ScaleInfo* pkParentScale)
{
    CHECK_MEMORY();
    CommonParticle* pkCommon = (CommonParticle*) pkPart;
    IParamBlock* pkPBlock = pkPart->pblock;
    Class_ID eID = pkPart->ClassID();

    // Basic Parameters Rollout
    float fTumble, fTumbleRate;
    pkPBlock->GetValue(PBBLIZ_TUMBLE, 0, fTumble, FOREVER);
    pkPBlock->GetValue(PBBLIZ_TUMBLERATE, 0, fTumbleRate, FOREVER);
    float fWidth, fHeight;
    pkPBlock->GetValue(PBBLIZ_EMITRWID, 0, fWidth, FOREVER);
    pkPBlock->GetValue(PBBLIZ_EMITRLENGTH, 0, fHeight, FOREVER);
    int iShowEmitter, iViewportDisplayType;
    pkPBlock->GetValue(PBBLIZ_EMITRHID, 0, iShowEmitter, FOREVER);
    pkPBlock->GetValue(PBBLIZ_VIEWPORTSHOWS, 0, iViewportDisplayType, 
        FOREVER);
    float fPercentageShown;
    pkPBlock->GetValue(PBBLIZ_DISPLAYPORTION, 0, fPercentageShown, FOREVER);
                        
    // Particle Generation Rollout
    int iLimitUsingTotal, iBirthRate, iTotalParticles;
    pkPBlock->GetValue(PBBLIZ_BIRTHMETHOD, 0, iLimitUsingTotal, FOREVER);
    pkPBlock->GetValue(PBBLIZ_PBIRTHRATE, 0, iBirthRate, FOREVER);
    pkPBlock->GetValue(PBBLIZ_PTOTALNUMBER, 0, iTotalParticles, FOREVER);
    float fSpeed, fSpeedVar;
    pkPBlock->GetValue(PBBLIZ_SPEED, 0, fSpeed, FOREVER);
    pkPBlock->GetValue(PBBLIZ_SPEEDVAR, 0, fSpeedVar, FOREVER);
    TimeValue kEmitStart, kEmitStop;
    pkPBlock->GetValue(PBBLIZ_EMITSTART, 0, kEmitStart, FOREVER);
    pkPBlock->GetValue(PBBLIZ_EMITSTOP, 0, kEmitStop, FOREVER);
    TimeValue kDisplayUntil, kLifeSpan, kLifeSpanVar;
    pkPBlock->GetValue(PBBLIZ_DISPUNTIL, 0, kDisplayUntil, FOREVER);
    pkPBlock->GetValue(PBBLIZ_LIFE, 0, kLifeSpan, FOREVER);
    pkPBlock->GetValue(PBBLIZ_LIFEVAR, 0, kLifeSpanVar, FOREVER);
    int iSubFrameTime, iSubFrameTrans, iSubFrameRot;
    pkPBlock->GetValue(PBBLIZ_SUBFRAMETIME, 0, iSubFrameTime, FOREVER);
    pkPBlock->GetValue(PBBLIZ_SUBFRAMEMOVE, 0, iSubFrameTrans, FOREVER);
    pkPBlock->GetValue(PBBLIZ_SUBFRAMEROT2, 0, iSubFrameRot, FOREVER);
    float fSize, fSizeVar;
    pkPBlock->GetValue(PBBLIZ_SIZE, 0, fSize, FOREVER);
    pkPBlock->GetValue(PBBLIZ_SIZEVAR, 0, fSizeVar, FOREVER);
    TimeValue kGrowFor, kFadeFor;
    pkPBlock->GetValue(PBBLIZ_GROWTIME, 0, kGrowFor, FOREVER);
    pkPBlock->GetValue(PBBLIZ_FADETIME, 0, kFadeFor, FOREVER);
    int iRandSeed;
    pkPBlock->GetValue(PBBLIZ_RNDSEED, 0, iRandSeed, FOREVER);

    // Particle Type Rollout
    int iParticleType, iStandardParticleType;
    pkPBlock->GetValue(PBBLIZ_PARTICLECLASS, 0, iParticleType, FOREVER);
    pkPBlock->GetValue(PBBLIZ_PARTICLETYPE, 0, iStandardParticleType, 
        FOREVER);
    float fMetaTension, fMetaTensionVar;
    pkPBlock->GetValue(PBBLIZ_METATENSION, 0, fMetaTension, FOREVER);
    pkPBlock->GetValue(PBBLIZ_METATENSIONVAR, 0, fMetaTensionVar, FOREVER);
    float fMetaCourseness, fMetaViewportCourseness;
    pkPBlock->GetValue(PBBLIZ_METACOURSE, 0, fMetaCourseness, FOREVER);
    pkPBlock->GetValue(PBBLIZ_METACOURSEVB, 0, fMetaViewportCourseness, 
        FOREVER);
    int iMetaAutoCoarseness;
    pkPBlock->GetValue(PBBLIZ_METAAUTOCOARSE, 0, iMetaAutoCoarseness, 
        FOREVER);
    int iInstancedSubtree;
    pkPBlock->GetValue(PBBLIZ_USESUBTREE, 0, iInstancedSubtree, FOREVER);
    int iInstancedOffsetType;
    pkPBlock->GetValue(PBBLIZ_ANIMATIONOFFSET, 0, iInstancedOffsetType, 
        FOREVER);
    float fInstancedOffsetAmount;
    pkPBlock->GetValue(PBBLIZ_OFFSETAMOUNT, 0, fInstancedOffsetAmount, 
        FOREVER);
    int iEmitterFitPlanar;
    pkPBlock->GetValue(PBBLIZ_EMITMAP, 0, iEmitterFitPlanar, FOREVER);
    int iMaterialMappingType;
    pkPBlock->GetValue(PBBLIZ_MAPPINGTYPE, 0, iMaterialMappingType, FOREVER);
    TimeValue kMappingTime;
    pkPBlock->GetValue(PBBLIZ_MAPPINGTIME, 0, kMappingTime, FOREVER);
    float fMappingDistance;
    pkPBlock->GetValue(PBBLIZ_MAPPINGDIST, 0, fMappingDistance, FOREVER);

    // Rotation And Collision Rollout
    TimeValue kSpinTime;
    pkPBlock->GetValue(PBBLIZ_SPINTIME, 0, kSpinTime, FOREVER);
    float fSpinTimeVar;
    pkPBlock->GetValue(PBBLIZ_SPINTIMEVAR, 0, fSpinTimeVar, FOREVER);
    float fSpinPhase, fSpinPhaseVar;
    pkPBlock->GetValue(PBBLIZ_SPINPHASE, 0, fSpinPhase, FOREVER);
    pkPBlock->GetValue(PBBLIZ_SPINPHASEVAR, 0, fSpinPhaseVar, FOREVER);
    int iSpinAxisType;
    pkPBlock->GetValue(PBBLIZ_SPINAXISTYPE, 0, iSpinAxisType, FOREVER);
    float fSpinStretch;
    pkPBlock->GetValue(PBBLIZ_STRETCH, 0, fSpinStretch, FOREVER);
    NiPoint3 kSpinAxis;
    pkPBlock->GetValue(PBBLIZ_SPINAXISX, 0, kSpinAxis.x, FOREVER);
    pkPBlock->GetValue(PBBLIZ_SPINAXISY, 0, kSpinAxis.y, FOREVER);
    pkPBlock->GetValue(PBBLIZ_SPINAXISZ, 0, kSpinAxis.z, FOREVER);
    float fSpinAxisVar;
    pkPBlock->GetValue(PBBLIZ_SPINAXISVAR, 0, fSpinAxisVar, FOREVER);
    int iParticleCollisionOn;
    pkPBlock->GetValue(PBBLIZ_BLIPCOLLIDE_ON, 0, iParticleCollisionOn, 
        FOREVER);
    int iParticleCollisionSteps;
    pkPBlock->GetValue(PBBLIZ_BLIPCOLLIDE_STEPS, 0, iParticleCollisionSteps, 
        FOREVER);
    float fParticleCollisionBounce, fParticleCollisionBounceVar;
    pkPBlock->GetValue(PBBLIZ_BLIPCOLLIDE_BOUNCE, 0, fParticleCollisionBounce,
        FOREVER);
    pkPBlock->GetValue(PBBLIZ_BLIPCOLLIDE_BOUNCEVAR, 0, 
        fParticleCollisionBounceVar, FOREVER);

    // Object Motion Inheritance Rollout
    float fOMIInfluence, fOMIMultiplier, fOMIMultiplierVar;
    pkPBlock->GetValue(PBBLIZ_EMITVINFL, 0, fOMIInfluence, FOREVER);
    pkPBlock->GetValue(PBBLIZ_EMITVMULT, 0, fOMIMultiplier, FOREVER);
    pkPBlock->GetValue(PBBLIZ_EMITVMULTVAR, 0, fOMIMultiplierVar, FOREVER);

    // Particle Spawn Rollout
    int iSpawnType;
    pkPBlock->GetValue(PBBLIZ_SPAWNTYPE, 0, iSpawnType, FOREVER);
    TimeValue kPersist;
    pkPBlock->GetValue(PBBLIZ_BLSPAWNDIEAFTER, 0, kPersist, FOREVER);
    float fPersistVar;
    pkPBlock->GetValue(PBBLIZ_BLSPAWNDIEAFTERVAR, 0, fPersistVar, FOREVER);
    int iNumSpawns;
    pkPBlock->GetValue(PBBLIZ_SPAWNGENS, 0, iNumSpawns, FOREVER);
    int iSpawnPercent;
    pkPBlock->GetValue(PBBLIZ_SPAWNPERCENT2, 0, iSpawnPercent, FOREVER);
    int iSpawnMultiplier;
    pkPBlock->GetValue(PBBLIZ_SPAWNCOUNT, 0, iSpawnMultiplier, FOREVER);
    float fSpawnMultiplierVar;
    pkPBlock->GetValue(PBBLIZ_SPAWNMULTVAR2, 0, fSpawnMultiplierVar, FOREVER);
    float fSpawnDirChaos;
    pkPBlock->GetValue(PBBLIZ_SPAWNDIRCHAOS, 0, fSpawnDirChaos, FOREVER);
    float fSpawnSpeedChaos;
    pkPBlock->GetValue(PBBLIZ_SPAWNSPEEDCHAOS, 0, fSpawnSpeedChaos, FOREVER);
    int iSpawnSpeedSign;
    pkPBlock->GetValue(PBBLIZ_SPAWNSPEEDSIGN, 0, iSpawnSpeedSign, FOREVER);
    int iSpawnInheritSpeed;
    pkPBlock->GetValue(PBBLIZ_SPAWNINHERITV, 0, iSpawnInheritSpeed, FOREVER);
    int iSpawnFixedSpeed;
    pkPBlock->GetValue(PBBLIZ_SPAWNSPEEDFIXED, 0, iSpawnFixedSpeed, FOREVER);
    float fSpawnScaleChaos;
    pkPBlock->GetValue(PBBLIZ_SPAWNSCALECHAOS, 0, fSpawnScaleChaos, FOREVER);
    int iSpawnScaleSign;
    pkPBlock->GetValue(PBBLIZ_SPAWNSCALESIGN, 0, iSpawnScaleSign, FOREVER);
    int iSpawnFixedScale;
    pkPBlock->GetValue(PBBLIZ_SPAWNSCALEFIXED, 0, iSpawnFixedScale, FOREVER);
    TimeValue kSpawnLifeValue;
    pkPBlock->GetValue(PBBLIZ_SPAWNLIFEVLUE, 0, kSpawnLifeValue, FOREVER);

    // Unknown
    int iCustomMtl, iBLNotDraft;
    pkPBlock->GetValue(PBBLIZ_CUSTOMMTL2, 0, iCustomMtl, FOREVER);
    pkPBlock->GetValue(PBBLIZ_BLNOTDRAFT, 0, iBLNotDraft, FOREVER);

    // Set up parameters
    pkDesc->m_fSpeed = fSpeed * GetFrameRate();
    // we must account for the fact that NiParticle calculates speed by
    // taking Speed + SpeedVar * (UnitRandom - 0.5);
    pkDesc->m_fSpeedVar = fSpeedVar * pkDesc->m_fSpeed * 2.0f;

    pkDesc->m_fDirDeclination = NI_PI;
    pkDesc->m_fDirDeclinationVar = 0.0f;
    pkDesc->m_fDirPlanarAngle = 0.0f;
    pkDesc->m_fDirPlanarAngleVar = 0.0f;

    pkDesc->m_fSize = fSize;
    pkDesc->m_fSizeVar = fSizeVar * pkDesc->m_fSize;

    pkDesc->m_fEmitStart = kEmitStart * sfTimeFactor;
    pkDesc->m_fEmitStop = kEmitStop * sfTimeFactor;

    if (pkDesc->m_bNiPSysUseViewport)
        pkDesc->m_fBirthRate = fPercentageShown * iBirthRate * GetFrameRate();
    else 
        pkDesc->m_fBirthRate = (float) iBirthRate * GetFrameRate();

    pkDesc->m_fLifeSpan = kLifeSpan * sfTimeFactor;
    // we must account for the fact that NiParticle calculates lifespan by
    // taking Lifespan + LifeSpanVar * (UnitRandom - 0.5);
    pkDesc->m_fLifeSpanVar = kLifeSpanVar * sfTimeFactor * 2.0f;
    pkDesc->m_bUseBirthRate = true;
    pkDesc->m_bSpawnOnDeath = (iSpawnType == 3);

    if (pkDesc->m_fEmitStart == pkDesc->m_fEmitStop)
    {
        pkDesc->m_fEmitStop = pkDesc->m_fEmitStart + PSYS_INTERVAL_TIME;
        if (iLimitUsingTotal == 0)
            iTotalParticles = iBirthRate;

        iLimitUsingTotal = 1;
    }

    if (iLimitUsingTotal != 0)
    {
        float fLifetime = pkDesc->m_fEmitStop - pkDesc->m_fEmitStart;

        if (pkDesc->m_bNiPSysUseViewport)
        {
            pkDesc->m_fBirthRate = fPercentageShown * iTotalParticles / 
                fLifetime;
        }
        else
        {
            pkDesc->m_fBirthRate = iTotalParticles / fLifetime;
        }
    }

    pkDesc->m_fEmitterWidth = fWidth;
    pkDesc->m_fEmitterHeight = fHeight;
    pkDesc->m_fEmitterDepth = 0.0f;

    pkDesc->m_bDieOnCollide = (iSpawnType == 1 || iSpawnType == 2);
    pkDesc->m_bSpawnOnCollide = (iSpawnType == 2);
    pkDesc->m_usNumGenerations = (iSpawnType == 0 ? 0 : iNumSpawns);
    pkDesc->m_fPercentageSpawned = (float)iSpawnPercent * 0.01f;
    pkDesc->m_usMultiplier = iSpawnMultiplier;
    pkDesc->m_fMultiplierVar = fSpawnMultiplierVar;
    pkDesc->m_fSpeedChaos = fSpawnSpeedChaos * 0.01f;
    switch (iSpawnSpeedSign)
    {
        case 0:
            pkDesc->m_fSpeedChaos *= -1.0f;
            break;
        case 1:
        case 2:
            break;
    };
    pkDesc->m_fDirChaos = fSpawnDirChaos;

    pkDesc->m_usViewportTotal = (unsigned short)
        (fPercentageShown * iTotalParticles);
    pkDesc->m_usRendererTotal = (unsigned short)
        (iTotalParticles);

    pkDesc->m_fGrowFor = kGrowFor * sfTimeFactor;
    pkDesc->m_fFadeFor = kFadeFor * sfTimeFactor;

    pkDesc->m_fSpinTime = kSpinTime * sfTimeFactor;
    pkDesc->m_fSpinTimeVar = fSpinTimeVar;
    pkDesc->m_bRandomSpinAxis = (iSpinAxisType != 1);
    pkDesc->m_kSpinAxis = kSpinAxis;
    pkDesc->m_fSpinAxisVar = fSpinAxisVar;
    pkDesc->m_fSpinPhase = fSpinPhase;
    pkDesc->m_fSpinPhaseVar = fSpinPhaseVar;

    if (iParticleType == 2 && pkCommon->custnode)
    {
        // Instanced particles
        NiMAXHierConverter kHier(m_kAnimStart, m_kAnimEnd);

        unsigned int uiNumMutations = pkCommon->nlist.Count();

        pkDesc->m_usNumMeshes = uiNumMutations + 1;

        pkDesc->m_pspMeshes = NiNew NiAVObjectPtr[uiNumMutations + 1];

        NiNodePtr spResult;
        for (unsigned int i = 0; i < uiNumMutations; i++)
        {
            int iStatus = kHier.BuildNode(pkIntf, pkCommon->nlist[i], 
                &spResult, pkParentScale, false, false);
            ReplaceRoot(&spResult);
            pkDesc->m_pspMeshes[i + 1] = spResult;
        }

        int iStatus = kHier.BuildNode(pkIntf, pkCommon->custnode, 
            &spResult, pkParentScale, false, false);
        ReplaceRoot(&spResult);
        pkDesc->m_pspMeshes[0] = spResult;
    }
    CHECK_MEMORY();

    // Extract animations
    if (iLimitUsingTotal == 0)
    {
        float fBRScalar = 1.0f;
        if (pkDesc->m_bNiPSysUseViewport)
            fBRScalar = fPercentageShown * GetFrameRate();
        else 
            fBRScalar = (float) GetFrameRate();

        pkDesc->m_spBirthRateInterp = BuildFloatInterp(pkPBlock,
            PBBLIZ_PBIRTHRATE, pkDesc->m_eBirthRateInterpCycle, fBRScalar);
    }
    pkDesc->m_spLifeSpanInterp = BuildFloatInterp(pkPBlock, PBBLIZ_LIFE, 
        pkDesc->m_eLifeSpanInterpCycle, (float) sfTimeFactor);
    pkDesc->m_spSpeedInterp = BuildFloatInterp(pkPBlock, PBBLIZ_SPEED, 
        pkDesc->m_eSpeedInterpCycle, (float) GetFrameRate());
    pkDesc->m_spSizeInterp = BuildFloatInterp(pkPBlock, PBBLIZ_SIZE, 
        pkDesc->m_eSizeInterpCycle);
    pkDesc->m_spPlanarAngleInterp = BuildFloatInterp(pkPBlock, 
        PBBLIZ_OFFPLANE,pkDesc->m_ePlanarAngleInterpCycle);
    pkDesc->m_spPlanarAngleVarInterp = BuildFloatInterp(pkPBlock, 
        PBBLIZ_PLANESPREAD, pkDesc->m_ePlanarAngleVarInterpCycle);
    pkDesc->m_spDeclinationInterp = BuildFloatInterp(pkPBlock, 
        PBBLIZ_OFFAXIS, pkDesc->m_eDeclinationInterpCycle);
    pkDesc->m_spDeclinationVarInterp = BuildFloatInterp(pkPBlock,
        PBBLIZ_AXISSPREAD, pkDesc->m_eDeclinationVarInterpCycle);
    pkDesc->m_spSpinPhaseInterp = BuildFloatInterp(pkPBlock, PBBLIZ_SPINPHASE,
        pkDesc->m_eSpinPhaseInterpCycle);
    pkDesc->m_spSpinPhaseVarInterp = BuildFloatInterp(pkPBlock, 
        PBBLIZ_SPINPHASEVAR, pkDesc->m_eSpinPhaseVarInterpCycle);

    return(W3D_STAT_OK);
}
//---------------------------------------------------------------------------
int NiMAXPSysConverter::ExtractPArray(INode* pkMaxNode, NiNode* pkNode,
    Interface* pkIntf, SimpleParticle* pkPart, NiPSysDesc* pkDesc, 
    ScaleInfo* pkParentScale)
{
    CHECK_MEMORY();
    PArrayParticle* pkPArray = (PArrayParticle*) pkPart;
    IParamBlock* pkPBlock = pkPart->pblock;
    
    // Emitter
    pkDesc->m_pkMAXEmitter = pkPArray->distnode;

    // Basic Parameters Rollout
    int iParticleFormation;
    pkPBlock->GetValue(PBPARR_DISTRIBUTION, 0, iParticleFormation, FOREVER);
    int iDistinctPoints;
    pkPBlock->GetValue(PBPARR_EMITTERCOUNT, 0, iDistinctPoints, FOREVER);
    int iUseSelectedSubobjects;
    pkPBlock->GetValue(PBPARR_USESELECTED, 0, iUseSelectedSubobjects, 
        FOREVER);
    float fWidth;
    pkPBlock->GetValue(PBPARR_EMITRWID, 0, fWidth, FOREVER);
    int iShowEmitter, iViewportDisplayType;
    pkPBlock->GetValue(PBPARR_EMITRHID, 0, iShowEmitter, FOREVER);
    pkPBlock->GetValue(PBPARR_VIEWPORTSHOWS, 0, iViewportDisplayType, 
        FOREVER);
    float fPercentageShown;
    pkPBlock->GetValue(PBPARR_DISPLAYPORTION, 0, fPercentageShown, FOREVER);

    // Particle Generation Rollout
    int iLimitUsingTotal, iBirthRate, iTotalParticles;
    pkPBlock->GetValue(PBPARR_BIRTHMETHOD, 0, iLimitUsingTotal, FOREVER);
    pkPBlock->GetValue(PBPARR_PBIRTHRATE, 0, iBirthRate, FOREVER);
    pkPBlock->GetValue(PBPARR_PTOTALNUMBER, 0, iTotalParticles, FOREVER);
    float fSpeed, fSpeedVar;
    pkPBlock->GetValue(PBPARR_SPEED, 0, fSpeed, FOREVER);
    pkPBlock->GetValue(PBPARR_SPEEDVAR, 0, fSpeedVar, FOREVER);
    float fDivergence;
    pkPBlock->GetValue(PBPARR_ANGLEDIV, 0, fDivergence, FOREVER);
    TimeValue kEmitStart, kEmitStop;
    pkPBlock->GetValue(PBPARR_EMITSTART, 0, kEmitStart, FOREVER);
    pkPBlock->GetValue(PBPARR_EMITSTOP, 0, kEmitStop, FOREVER);
    TimeValue kDisplayUntil, kLifeSpan, kLifeSpanVar;
    pkPBlock->GetValue(PBPARR_DISPUNTIL, 0, kDisplayUntil, FOREVER);
    pkPBlock->GetValue(PBPARR_LIFE, 0, kLifeSpan, FOREVER);
    pkPBlock->GetValue(PBPARR_LIFEVAR, 0, kLifeSpanVar, FOREVER);
    int iSubFrameTime, iSubFrameTrans, iSubFrameRot;
    pkPBlock->GetValue(PBPARR_SUBFRAMETIME, 0, iSubFrameTime, FOREVER);
    pkPBlock->GetValue(PBPARR_SUBFRAMEMOVE, 0, iSubFrameTrans, FOREVER);
    pkPBlock->GetValue(PBPARR_SUBFRAMEROT, 0, iSubFrameRot, FOREVER);
    float fSize, fSizeVar;
    pkPBlock->GetValue(PBPARR_SIZE, 0, fSize, FOREVER);
    pkPBlock->GetValue(PBPARR_SIZEVAR, 0, fSizeVar, FOREVER);
    TimeValue kGrowFor, kFadeFor;
    pkPBlock->GetValue(PBPARR_GROWTIME, 0, kGrowFor, FOREVER);
    pkPBlock->GetValue(PBPARR_FADETIME, 0, kFadeFor, FOREVER);
    int iRandSeed;
    pkPBlock->GetValue(PBPARR_RNDSEED, 0, iRandSeed, FOREVER);

    // Particle Type Rollout
    int iParticleType, iStandardParticleType;
    pkPBlock->GetValue(PBPARR_PARTICLECLASS, 0, iParticleType, FOREVER);
    pkPBlock->GetValue(PBPARR_PARTICLETYPE, 0, iStandardParticleType, 
        FOREVER);
    float fMetaTension, fMetaTensionVar;
    pkPBlock->GetValue(PBPARR_METATENSION, 0, fMetaTension, FOREVER);
    pkPBlock->GetValue(PBPARR_METATENSIONVAR, 0, fMetaTensionVar, FOREVER);
    float fMetaCourseness, fMetaViewportCourseness;
    pkPBlock->GetValue(PBPARR_METACOURSE, 0, fMetaCourseness, FOREVER);
    pkPBlock->GetValue(PBPARR_METACOURSEV, 0, fMetaViewportCourseness, 
        FOREVER);
    int iMetaAutoCoarseness;
    pkPBlock->GetValue(PBPARR_METAAUTOCOARSE, 0, iMetaAutoCoarseness, 
        FOREVER);
    float fFragThickness;
    pkPBlock->GetValue(PBPARR_FRAGTHICKNESS, 0, fFragThickness, FOREVER);
    int iFragMethod, iFragCount;
    pkPBlock->GetValue(PBPARR_FRAGMETHOD, 0, iFragMethod, FOREVER);
    pkPBlock->GetValue(PBPARR_FRAGCOUNT, 0, iFragCount, FOREVER);
    float fFragSmoothingAngle;
    pkPBlock->GetValue(PBPARR_SMOOTHANG, 0, fFragSmoothingAngle, FOREVER);
    int iInstancedSubtree;
    pkPBlock->GetValue(PBPARR_USESUBTREE, 0, iInstancedSubtree, FOREVER);
    int iInstancedOffsetType;
    pkPBlock->GetValue(PBPARR_ANIMATIONOFFSET, 0, iInstancedOffsetType, 
        FOREVER);
    float fInstancedOffsetAmount;
    pkPBlock->GetValue(PBPARR_OFFSETAMOUNT, 0, fInstancedOffsetAmount, 
        FOREVER);
    int iMaterialMappingType;
    pkPBlock->GetValue(PBPARR_MAPPINGTYPE, 0, iMaterialMappingType, FOREVER);
    TimeValue kMappingTime;
    pkPBlock->GetValue(PBPARR_MAPPINGTIME, 0, kMappingTime, FOREVER);
    float fMappingDistance;
    pkPBlock->GetValue(PBPARR_MAPPINGDIST, 0, fMappingDistance, FOREVER);
    
    int iOutsideMat, iEdgeMat, iBacksideMat;
    pkPBlock->GetValue(PBPARR_FMAT, 0, iOutsideMat, FOREVER);
    pkPBlock->GetValue(PBPARR_EMAT, 0, iEdgeMat, FOREVER);
    pkPBlock->GetValue(PBPARR_BMAT, 0, iBacksideMat, FOREVER);

    // Rotation And Collision Rollout
    TimeValue kSpinTime;
    pkPBlock->GetValue(PBPARR_SPINTIME, 0, kSpinTime, FOREVER);
    float fSpinTimeVar;
    pkPBlock->GetValue(PBPARR_SPINTIMEVAR, 0, fSpinTimeVar, FOREVER);
    float fSpinPhase, fSpinPhaseVar;
    pkPBlock->GetValue(PBPARR_SPINPHASE, 0, fSpinPhase, FOREVER);
    pkPBlock->GetValue(PBPARR_SPINPHASEVAR, 0, fSpinPhaseVar, FOREVER);
    int iSpinAxisType;
    pkPBlock->GetValue(PBPARR_SPINAXISTYPE, 0, iSpinAxisType, FOREVER);
    float fSpinStretch;
    pkPBlock->GetValue(PBPARR_STRETCH, 0, fSpinStretch, FOREVER);
    NiPoint3 kSpinAxis;
    pkPBlock->GetValue(PBPARR_SPINAXISX, 0, kSpinAxis.x, FOREVER);
    pkPBlock->GetValue(PBPARR_SPINAXISY, 0, kSpinAxis.y, FOREVER);
    pkPBlock->GetValue(PBPARR_SPINAXISZ, 0, kSpinAxis.z, FOREVER);
    float fSpinAxisVar;
    pkPBlock->GetValue(PBPARR_SPINAXISVAR, 0, fSpinAxisVar, FOREVER);
    int iParticleCollisionOn;
    pkPBlock->GetValue(PBPARR_PAIPCOLLIDE_ON, 0, iParticleCollisionOn, 
        FOREVER);
    int iParticleCollisionSteps;
    pkPBlock->GetValue(PBPARR_PAIPCOLLIDE_STEPS, 0, iParticleCollisionSteps, 
        FOREVER);
    float fParticleCollisionBounce, fParticleCollisionBounceVar;
    pkPBlock->GetValue(PBPARR_PAIPCOLLIDE_BOUNCE, 0, 
        fParticleCollisionBounce, FOREVER);
    pkPBlock->GetValue(PBPARR_PAIPCOLLIDE_BOUNCEVAR, 0, 
        fParticleCollisionBounceVar, FOREVER);

    // Object Motion Inheritance Rollout
    float fOMIInfluence, fOMIMultiplier, fOMIMultiplierVar;
    pkPBlock->GetValue(PBPARR_EMITVINFL, 0, fOMIInfluence, FOREVER);
    pkPBlock->GetValue(PBPARR_EMITVMULT, 0, fOMIMultiplier, FOREVER);
    pkPBlock->GetValue(PBPARR_EMITVMULTVAR, 0, fOMIMultiplierVar, FOREVER);

    // Bubble Motion Rollout
    float fBubbleAmp, fBubbleAmpVar;
    pkPBlock->GetValue(PBPARR_BUBLAMP, 0, fBubbleAmp, FOREVER);
    pkPBlock->GetValue(PBPARR_BUBLAMPVAR, 0, fBubbleAmpVar, FOREVER);
    TimeValue kBubblePeriod;
    pkPBlock->GetValue(PBPARR_BUBLPER, 0, kBubblePeriod, FOREVER);
    float fBubblePeriodVar;
    pkPBlock->GetValue(PBPARR_BUBLPERVAR, 0, fBubblePeriodVar, FOREVER);
    float fBubblePhase, fBubblePhaseVar;
    pkPBlock->GetValue(PBPARR_BUBLPHAS, 0, fBubblePhase, FOREVER);
    pkPBlock->GetValue(PBPARR_BUBLPHASVAR, 0, fBubblePhaseVar, FOREVER);

    // Particle Spawn Rollout
    int iSpawnType;
    pkPBlock->GetValue(PBPARR_SPAWNTYPE, 0, iSpawnType, FOREVER);
    TimeValue kPersist;
    pkPBlock->GetValue(PBPARR_PASPAWNDIEAFTER, 0, kPersist, FOREVER);
    float fPersistVar;
    pkPBlock->GetValue(PBPARR_PASPAWNDIEAFTERVAR, 0, fPersistVar, FOREVER);
    int iNumSpawns;
    pkPBlock->GetValue(PBPARR_SPAWNGENS, 0, iNumSpawns, FOREVER);
    int iSpawnPercent;
    pkPBlock->GetValue(PBPARR_SPAWNPERCENT, 0, iSpawnPercent, FOREVER);
    int iSpawnMultiplier;
    pkPBlock->GetValue(PBPARR_SPAWNCOUNT, 0, iSpawnMultiplier, FOREVER);
    float fSpawnMultiplierVar;
    pkPBlock->GetValue(PBPARR_SPAWNMULTVAR, 0, fSpawnMultiplierVar, FOREVER);
    float fSpawnDirChaos;
    pkPBlock->GetValue(PBPARR_SPAWNDIRCHAOS, 0, fSpawnDirChaos, FOREVER);
    float fSpawnSpeedChaos;
    pkPBlock->GetValue(PBPARR_SPAWNSPEEDCHAOS, 0, fSpawnSpeedChaos, FOREVER);
    int iSpawnSpeedSign;
    pkPBlock->GetValue(PBPARR_SPAWNSPEEDSIGN, 0, iSpawnSpeedSign, FOREVER);
    int iSpawnInheritSpeed;
    pkPBlock->GetValue(PBPARR_SPAWNINHERITV, 0, iSpawnInheritSpeed, FOREVER);
    int iSpawnFixedSpeed;
    pkPBlock->GetValue(PBPARR_SPAWNSPEEDFIXED, 0, iSpawnFixedSpeed, FOREVER);
    float fSpawnScaleChaos;
    pkPBlock->GetValue(PBPARR_SPAWNSCALECHAOS, 0, fSpawnScaleChaos, FOREVER);
    int iSpawnScaleSign;
    pkPBlock->GetValue(PBPARR_SPAWNSCALESIGN, 0, iSpawnScaleSign, FOREVER);
    int iSpawnFixedScale;
    pkPBlock->GetValue(PBPARR_SPAWNSCALEFIXED, 0, iSpawnFixedScale, FOREVER);
    TimeValue kSpawnLifeValue;
    pkPBlock->GetValue(PBPARR_SPAWNLIFEVLUE, 0, kSpawnLifeValue, FOREVER);

    // Unknown
    int iCustomMtl, iPANotDraft;
    pkPBlock->GetValue(PBPARR_CUSTOMMATERIAL, 0, iCustomMtl, FOREVER);
    pkPBlock->GetValue(PBPARR_PANOTDRAFT, 0, iPANotDraft, FOREVER);

    // Set up emitter info
    if (pkDesc->m_pkMAXEmitter)
    {
        pkDesc->m_eEmitterType = EMITTER_OBJECT;
        switch (iParticleFormation)
        {
            default:
            case 0:
                pkDesc->m_eEmissionType = 
                    NiPSysMeshEmitter::NI_EMIT_FROM_FACE_SURFACE;
                break;
            case 1:
                pkDesc->m_eEmissionType = 
                    NiPSysMeshEmitter::NI_EMIT_FROM_EDGE_SURFACE;
                break;
            case 2:
                pkDesc->m_eEmissionType = 
                    NiPSysMeshEmitter::NI_EMIT_FROM_VERTICES;
                break;
            case 3:
                pkDesc->m_eEmissionType = 
                    NiPSysMeshEmitter::NI_EMIT_FROM_VERTICES;
                break;
            case 4:
                pkDesc->m_eEmissionType = 
                    NiPSysMeshEmitter::NI_EMIT_FROM_FACE_CENTER;
                break;
        }
    }

    // Set up parameters
    pkDesc->m_fSpeed = fSpeed * GetFrameRate();
    // we must account for the fact that NiParticle calculates speed by
    // taking Speed + SpeedVar * (UnitRandom - 0.5);
    pkDesc->m_fSpeedVar = fSpeedVar * pkDesc->m_fSpeed * 2.0f;

    pkDesc->m_fDirDeclination = 0.0f;
    pkDesc->m_fDirDeclinationVar = fDivergence * 0.5f;
    pkDesc->m_fDirPlanarAngle = 0.0f;
    pkDesc->m_fDirPlanarAngleVar = fDivergence * 0.5f;

    pkDesc->m_fSize = fSize;
    pkDesc->m_fSizeVar = fSizeVar * pkDesc->m_fSize;

    pkDesc->m_fEmitStart = kEmitStart * sfTimeFactor;
    pkDesc->m_fEmitStop = kEmitStop * sfTimeFactor;

    if (pkDesc->m_bNiPSysUseViewport)
        pkDesc->m_fBirthRate = fPercentageShown * iBirthRate * GetFrameRate();
    else 
        pkDesc->m_fBirthRate = (float) iBirthRate * (float) GetFrameRate();

    pkDesc->m_fLifeSpan = kLifeSpan * sfTimeFactor;
    // we must account for the fact that NiParticle calculates lifespan by
    // taking Lifespan + LifeSpanVar * (UnitRandom - 0.5);
    pkDesc->m_fLifeSpanVar = kLifeSpanVar * sfTimeFactor * 2.0f;
    pkDesc->m_bUseBirthRate = true;
    pkDesc->m_bSpawnOnDeath = (iSpawnType == 3);

    if (pkDesc->m_fEmitStart == pkDesc->m_fEmitStop)
    {
        pkDesc->m_fEmitStop = pkDesc->m_fEmitStart + PSYS_INTERVAL_TIME;
        if (iLimitUsingTotal == 0)
            iTotalParticles = iBirthRate;
        iLimitUsingTotal = 1;
    }

    if (iLimitUsingTotal != 0)
    {
        float fLifetime = pkDesc->m_fEmitStop - pkDesc->m_fEmitStart;

        if (pkDesc->m_bNiPSysUseViewport)
        {
            pkDesc->m_fBirthRate = fPercentageShown * iTotalParticles / 
                fLifetime;
        }
        else
        {
            pkDesc->m_fBirthRate = (float) iTotalParticles / fLifetime;
        }
    }
    pkDesc->m_fEmitterWidth = 0.0f;
    pkDesc->m_fEmitterHeight = 0.0f;
    pkDesc->m_fEmitterDepth = 0.0f;

    pkDesc->m_bDieOnCollide = (iSpawnType == 1 || iSpawnType == 2);
    pkDesc->m_bSpawnOnCollide = (iSpawnType == 2);
    pkDesc->m_usNumGenerations = (iSpawnType == 0 ? 0 : iNumSpawns);
    pkDesc->m_fPercentageSpawned = (float)iSpawnPercent * 0.01f;
    pkDesc->m_usMultiplier = iSpawnMultiplier;
    pkDesc->m_fMultiplierVar = fSpawnMultiplierVar;
    pkDesc->m_fSpeedChaos = fSpawnSpeedChaos * 0.01f;
    switch (iSpawnSpeedSign)
    {
        case 0:
            pkDesc->m_fSpeedChaos *= -1.0f;
            break;
        case 1:
        case 2:
            break;
    };
    pkDesc->m_fDirChaos = fSpawnDirChaos;

    pkDesc->m_usViewportTotal = (unsigned short)
        (fPercentageShown * iTotalParticles);
    pkDesc->m_usRendererTotal = (unsigned short)
        (iTotalParticles);

    pkDesc->m_fGrowFor = kGrowFor * sfTimeFactor;
    pkDesc->m_fFadeFor = kFadeFor * sfTimeFactor;

    pkDesc->m_fSpinTime = kSpinTime * sfTimeFactor;
    pkDesc->m_fSpinTimeVar = fSpinTimeVar;
    pkDesc->m_bRandomSpinAxis = (iSpinAxisType != 2);
    pkDesc->m_kSpinAxis = kSpinAxis;
    pkDesc->m_fSpinAxisVar = fSpinAxisVar;
    pkDesc->m_fSpinPhase = fSpinPhase;
    pkDesc->m_fSpinPhaseVar = fSpinPhaseVar;

    if (iParticleType == 3 && pkPArray->custnode)
    {
        // Instanced particles
        NiMAXHierConverter kHier(m_kAnimStart, m_kAnimEnd);

        unsigned int uiNumMutations = pkPArray->nlist.Count();

        pkDesc->m_usNumMeshes = uiNumMutations + 1;

        pkDesc->m_pspMeshes = NiNew NiAVObjectPtr[uiNumMutations + 1];

        NiNodePtr spResult;
        for (unsigned int i = 0; i < uiNumMutations; i++)
        {
            int iStatus = kHier.BuildNode(pkIntf, pkPArray->nlist[i], 
                &spResult, pkParentScale, false, false);
            ReplaceRoot(&spResult);
            pkDesc->m_pspMeshes[i + 1] = spResult;
        }

        int iStatus = kHier.BuildNode(pkIntf, pkPArray->custnode, &spResult, 
            pkParentScale, false, false);
        ReplaceRoot(&spResult);
        pkDesc->m_pspMeshes[0] = spResult;
    }
    else if (iParticleType == 2)
    {
        // Object fragments
        pkPArray->UpdateParticles(kEmitStart, NULL);
        unsigned int uiNumFrags = pkPArray->parts.Count();
        if (uiNumFrags)
        {
            NiMAXMaterialConverter kMtl(m_kAnimStart, m_kAnimEnd);
            pkDesc->m_usViewportTotal = uiNumFrags;
            pkDesc->m_usRendererTotal = uiNumFrags;
            pkDesc->m_usNumFrags = uiNumFrags;
            pkDesc->m_pkPos = NiNew NiPoint3[uiNumFrags];
            pkDesc->m_pkVel = NiNew NiPoint3[uiNumFrags];
            pkDesc->m_pspFrag = NiNew NiTriShapePtr[uiNumFrags];
        }
    }

    // Extract animations
    if (iLimitUsingTotal == 0)
    {
        float fBRScalar = 1.0f;
        if (pkDesc->m_bNiPSysUseViewport)
            fBRScalar = fPercentageShown * GetFrameRate();
        else 
            fBRScalar = (float) GetFrameRate();

        pkDesc->m_spBirthRateInterp = BuildFloatInterp(pkPBlock, 
            PBPARR_PBIRTHRATE, pkDesc->m_eBirthRateInterpCycle, fBRScalar);
    }
    pkDesc->m_spLifeSpanInterp = BuildFloatInterp(pkPBlock, PBPARR_LIFE,
        pkDesc->m_eLifeSpanInterpCycle, sfTimeFactor);
    pkDesc->m_spSpeedInterp = BuildFloatInterp(pkPBlock, PBPARR_SPEED,
        pkDesc->m_eSpeedInterpCycle, (float) GetFrameRate());
    pkDesc->m_spSizeInterp = BuildFloatInterp(pkPBlock, PBPARR_SIZE,
        pkDesc->m_eSizeInterpCycle);
    pkDesc->m_spSpinPhaseInterp = BuildFloatInterp(pkPBlock, PBPARR_SPINPHASE,
        pkDesc->m_eSpinPhaseInterpCycle);
    pkDesc->m_spSpinPhaseVarInterp = BuildFloatInterp(pkPBlock, 
        PBPARR_SPINPHASEVAR, pkDesc->m_eSpinPhaseVarInterpCycle);

    CHECK_MEMORY();

    return(W3D_STAT_OK);
}
//---------------------------------------------------------------------------
int NiMAXPSysConverter::ExtractPCloud(INode* pkMaxNode, Interface* pkIntf, 
    SimpleParticle* pkPart, NiPSysDesc* pkDesc, ScaleInfo* pkParentScale)
{
    CHECK_MEMORY();
    PCloudParticle* pkPCld = (PCloudParticle*)pkPart;
    IParamBlock* pkPBlock = pkPart->pblock;

    // Basic Parameters Rollout
    int iEmitterType;
    pkPBlock->GetValue(PB_PCLD_CREATEIN, 0, iEmitterType, FOREVER);
    switch (iEmitterType)
    {
        default:
        case 0: // Box
            pkDesc->m_eEmitterType = EMITTER_BOX;
            break;
        case 1: // Sphere
            pkDesc->m_eEmitterType = EMITTER_SPHERE;
            break;
        case 2: // Cylinder
            pkDesc->m_eEmitterType = EMITTER_CYLINDER;
            break;
        case 3: // Generic mesh
            pkDesc->m_eEmitterType = EMITTER_OBJECT;
            pkDesc->m_eEmissionType = 
                NiPSysMeshEmitter::NI_EMIT_FROM_FACE_SURFACE;
            pkDesc->m_pkMAXEmitter = pkPCld->distnode;
            break;
    }
    float fWidth, fHeight, fDepth;
    pkPBlock->GetValue(PB_PCLD_EMITRHEIGHT, 0, fWidth, FOREVER);
    pkPBlock->GetValue(PB_PCLD_EMITRWID, 0, fHeight, FOREVER);
    pkPBlock->GetValue(PB_PCLD_EMITRDEPTH, 0, fDepth, FOREVER);
    int iShowEmitter, iViewportDisplayType;
    pkPBlock->GetValue(PB_PCLD_EMITRHID, 0, iShowEmitter, FOREVER);
    pkPBlock->GetValue(PB_PCLD_VIEWPORTSHOWS, 0, iViewportDisplayType, 
        FOREVER);
    float fPercentageShown;
    pkPBlock->GetValue(PB_PCLD_DISPLAYPORTION, 0, fPercentageShown, FOREVER);

    // Particle Generation Rollout
    int iLimitUsingTotal, iBirthRate, iTotalParticles;
    pkPBlock->GetValue(PB_PCLD_BIRTHMETHOD, 0, iLimitUsingTotal, FOREVER);
    pkPBlock->GetValue(PB_PCLD_PBIRTHRATE, 0, iBirthRate, FOREVER);
    pkPBlock->GetValue(PB_PCLD_PTOTALNUMBER, 0, iTotalParticles, FOREVER);
    float fSpeed, fSpeedVar;
    pkPBlock->GetValue(PB_PCLD_SPEED, 0, fSpeed, FOREVER);
    pkPBlock->GetValue(PB_PCLD_SPEEDVAR, 0, fSpeedVar, FOREVER);
    int iSpeedDirectionType;
    pkPBlock->GetValue(PB_PCLD_SPEEDDIR, 0, iSpeedDirectionType, FOREVER);
    NiPoint3 kSpeedDirection;
    pkPBlock->GetValue(PB_PCLD_DIRX, 0, kSpeedDirection.x, FOREVER);
    pkPBlock->GetValue(PB_PCLD_DIRY, 0, kSpeedDirection.y, FOREVER);
    pkPBlock->GetValue(PB_PCLD_DIRZ, 0, kSpeedDirection.z, FOREVER);
    float fSpeedDirectionVar;
    pkPBlock->GetValue(PB_PCLD_DIRVAR, 0, fSpeedDirectionVar, FOREVER);
    TimeValue kEmitStart, kEmitStop;
    pkPBlock->GetValue(PB_PCLD_EMITSTART, 0, kEmitStart, FOREVER);
    pkPBlock->GetValue(PB_PCLD_EMITSTOP, 0, kEmitStop, FOREVER);
    TimeValue kDisplayUntil, kLifeSpan, kLifeSpanVar;
    pkPBlock->GetValue(PB_PCLD_DISPUNTIL, 0, kDisplayUntil, FOREVER);
    pkPBlock->GetValue(PB_PCLD_LIFE, 0, kLifeSpan, FOREVER);
    pkPBlock->GetValue(PB_PCLD_LIFEVAR, 0, kLifeSpanVar, FOREVER);
    float fSize, fSizeVar;
    pkPBlock->GetValue(PB_PCLD_SIZE, 0, fSize, FOREVER);
    pkPBlock->GetValue(PB_PCLD_SIZEVAR, 0, fSizeVar, FOREVER);
    TimeValue kGrowFor, kFadeFor;
    pkPBlock->GetValue(PB_PCLD_GROWTIME, 0, kGrowFor, FOREVER);
    pkPBlock->GetValue(PB_PCLD_FADETIME, 0, kFadeFor, FOREVER);
    int iRandSeed;
    pkPBlock->GetValue(PB_PCLD_RNDSEED, 0, iRandSeed, FOREVER);

    // Particle Type Rollout
    int iParticleType, iStandardParticleType;
    pkPBlock->GetValue(PB_PCLD_PARTICLECLASS, 0, iParticleType, FOREVER);
    pkPBlock->GetValue(PB_PCLD_PARTICLETYPE, 0, iStandardParticleType, 
        FOREVER);
    float fMetaTension, fMetaTensionVar;
    pkPBlock->GetValue(PB_PCLD_METATENSION, 0, fMetaTension, FOREVER);
    pkPBlock->GetValue(PB_PCLD_METATENSIONVAR, 0, fMetaTensionVar, FOREVER);
    float fMetaCourseness, fMetaViewportCourseness;
    pkPBlock->GetValue(PB_PCLD_METACOURSE, 0, fMetaCourseness, FOREVER);
    pkPBlock->GetValue(PB_PCLD_METACOURSEV, 0, fMetaViewportCourseness, 
        FOREVER);
    int iMetaAutoCoarseness;
    pkPBlock->GetValue(PB_PCLD_METAAUTOCOARSE, 0, iMetaAutoCoarseness, 
        FOREVER);
    int iInstancedSubtree;
    pkPBlock->GetValue(PB_PCLD_USESUBTREE, 0, iInstancedSubtree, FOREVER);
    int iInstancedOffsetType;
    pkPBlock->GetValue(PB_PCLD_ANIMATIONOFFSET, 0, iInstancedOffsetType, 
        FOREVER);
    float fInstancedOffsetAmount;
    pkPBlock->GetValue(PB_PCLD_OFFSETAMOUNT, 0, fInstancedOffsetAmount, 
        FOREVER);
    int iMaterialMappingType;
    pkPBlock->GetValue(PB_PCLD_MAPPINGTYPE, 0, iMaterialMappingType, FOREVER);
    TimeValue kMappingTime;
    pkPBlock->GetValue(PB_PCLD_MAPPINGTIME, 0, kMappingTime, FOREVER);
    float fMappingDistance;
    pkPBlock->GetValue(PB_PCLD_MAPPINGDIST, 0, fMappingDistance, FOREVER);

    // Rotation And Collision Rollout
    TimeValue kSpinTime;
    pkPBlock->GetValue(PB_PCLD_SPINTIME, 0, kSpinTime, FOREVER);
    float fSpinTimeVar;
    pkPBlock->GetValue(PB_PCLD_SPINTIMEVAR, 0, fSpinTimeVar, FOREVER);
    float fSpinPhase, fSpinPhaseVar;
    pkPBlock->GetValue(PB_PCLD_SPINPHASE, 0, fSpinPhase, FOREVER);
    pkPBlock->GetValue(PB_PCLD_SPINPHASEVAR, 0, fSpinPhaseVar, FOREVER);
    int iSpinAxisType;
    pkPBlock->GetValue(PB_PCLD_SPINAXISTYPE, 0, iSpinAxisType, FOREVER);
    float fSpinStretch;
    pkPBlock->GetValue(PB_PCLD_STRETCH, 0, fSpinStretch, FOREVER);
    NiPoint3 kSpinAxis;
    pkPBlock->GetValue(PB_PCLD_SPINAXISX, 0, kSpinAxis.x, FOREVER);
    pkPBlock->GetValue(PB_PCLD_SPINAXISY, 0, kSpinAxis.y, FOREVER);
    pkPBlock->GetValue(PB_PCLD_SPINAXISZ, 0, kSpinAxis.z, FOREVER);
    float fSpinAxisVar;
    pkPBlock->GetValue(PB_PCLD_SPINAXISVAR, 0, fSpinAxisVar, FOREVER);
    int iParticleCollisionOn;
    pkPBlock->GetValue(PB_PCLD_PCIPCOLLIDE_ON, 0, iParticleCollisionOn, 
        FOREVER);
    int iParticleCollisionSteps;
    pkPBlock->GetValue(PB_PCLD_PCIPCOLLIDE_STEPS, 0, 
        iParticleCollisionSteps, FOREVER);
    float fParticleCollisionBounce, fParticleCollisionBounceVar;
    pkPBlock->GetValue(PB_PCLD_PCIPCOLLIDE_BOUNCE, 0, 
        fParticleCollisionBounce, FOREVER);
    pkPBlock->GetValue(PB_PCLD_PCIPCOLLIDE_BOUNCEVAR, 0, 
        fParticleCollisionBounceVar, FOREVER);

    // Object Motion Inheritance Rollout
    float fOMIInfluence, fOMIMultiplier, fOMIMultiplierVar;
    pkPBlock->GetValue(PB_PCLD_EMITVINFL, 0, fOMIInfluence, FOREVER);
    pkPBlock->GetValue(PB_PCLD_EMITVMULT, 0, fOMIMultiplier, FOREVER);
    pkPBlock->GetValue(PB_PCLD_EMITVMULTVAR, 0, fOMIMultiplierVar, FOREVER);

    // Bubble Motion Rollout
    float fBubbleAmp, fBubbleAmpVar;
    pkPBlock->GetValue(PB_PCLD_BUBLAMP, 0, fBubbleAmp, FOREVER);
    pkPBlock->GetValue(PB_PCLD_BUBLAMPVAR, 0, fBubbleAmpVar, FOREVER);
    TimeValue kBubblePeriod;
    pkPBlock->GetValue(PB_PCLD_BUBLPER, 0, kBubblePeriod, FOREVER);
    float fBubblePeriodVar;
    pkPBlock->GetValue(PB_PCLD_BUBLPERVAR, 0, fBubblePeriodVar, FOREVER);
    float fBubblePhase, fBubblePhaseVar;
    pkPBlock->GetValue(PB_PCLD_BUBLPHAS, 0, fBubblePhase, FOREVER);
    pkPBlock->GetValue(PB_PCLD_BUBLPHASVAR, 0, fBubblePhaseVar, FOREVER);

    // Particle Spawn Rollout
    int iSpawnType;
    pkPBlock->GetValue(PB_PCLD_SPAWNTYPE, 0, iSpawnType, FOREVER);
    TimeValue kPersist;
    pkPBlock->GetValue(PB_PCLD_SPAWNDIEAFTER, 0, kPersist, FOREVER);
    float fPersistVar;
    pkPBlock->GetValue(PB_PCLD_SPAWNDIEAFTERVAR, 0, fPersistVar, FOREVER);
    int iNumSpawns;
    pkPBlock->GetValue(PB_PCLD_SPAWNGENS, 0, iNumSpawns, FOREVER);
    int iSpawnPercent;
    pkPBlock->GetValue(PB_PCLD_SPAWNPERCENT, 0, iSpawnPercent, FOREVER);
    int iSpawnMultiplier;
    pkPBlock->GetValue(PB_PCLD_SPAWNCOUNT, 0, iSpawnMultiplier, FOREVER);
    float fSpawnMultiplierVar;
    pkPBlock->GetValue(PB_PCLD_SPAWNMULTVAR, 0, fSpawnMultiplierVar, FOREVER);
    float fSpawnDirChaos;
    pkPBlock->GetValue(PB_PCLD_SPAWNDIRCHAOS, 0, fSpawnDirChaos, FOREVER);
    float fSpawnSpeedChaos;
    pkPBlock->GetValue(PB_PCLD_SPAWNSPEEDCHAOS, 0, fSpawnSpeedChaos, FOREVER);
    int iSpawnSpeedSign;
    pkPBlock->GetValue(PB_PCLD_SPAWNSPEEDSIGN, 0, iSpawnSpeedSign, FOREVER);
    int iSpawnInheritSpeed;
    pkPBlock->GetValue(PB_PCLD_SPAWNINHERITV, 0, iSpawnInheritSpeed, FOREVER);
    int iSpawnFixedSpeed;
    pkPBlock->GetValue(PB_PCLD_SPAWNSPEEDFIXED, 0, iSpawnFixedSpeed, FOREVER);
    float fSpawnScaleChaos;
    pkPBlock->GetValue(PB_PCLD_SPAWNSCALECHAOS, 0, fSpawnScaleChaos, FOREVER);
    int iSpawnScaleSign;
    pkPBlock->GetValue(PB_PCLD_SPAWNSCALESIGN, 0, iSpawnScaleSign, FOREVER);
    int iSpawnFixedScale;
    pkPBlock->GetValue(PB_PCLD_SPAWNSCALEFIXED, 0, iSpawnFixedScale, FOREVER);
    TimeValue kSpawnLifeValue;
    pkPBlock->GetValue(PB_PCLD_SPAWNLIFEVLUE, 0, kSpawnLifeValue, FOREVER);

    // Unknown
    int iCustomMtl, iPCNotDraft;
    pkPBlock->GetValue(PB_PCLD_CUSTOMMTL, 0, iCustomMtl, FOREVER);
    pkPBlock->GetValue(PB_PCLD_PCNOTDRAFT, 0, iPCNotDraft, FOREVER);

    // Set up parameters
    pkDesc->m_fSpeed = fSpeed * GetFrameRate();
    // we must account for the fact that NiParticle calculates speed by
    // taking Speed + SpeedVar * (UnitRandom - 0.5);
    pkDesc->m_fSpeedVar = fSpeedVar * pkDesc->m_fSpeed * 2.0f;

    if (iSpeedDirectionType != 1)
    {
        // Random direction or "reference object" - random
        pkDesc->m_fDirDeclination = 0.0f;
        pkDesc->m_fDirDeclinationVar = NI_PI;
        pkDesc->m_fDirPlanarAngle = 0.0f;
        pkDesc->m_fDirPlanarAngleVar = NI_TWO_PI;
    }
    else
    {
        // Specified direction
        pkDesc->m_fDirDeclination = NiACos(kSpeedDirection.z /
            kSpeedDirection.Length());
        pkDesc->m_fDirDeclinationVar = NI_PI * fSpeedDirectionVar;
        if (kSpeedDirection.x != 0.0f)
        {
            pkDesc->m_fDirPlanarAngle = NiATan(kSpeedDirection.y / 
                kSpeedDirection.x);
            if (kSpeedDirection.y < 0.0f)
            {
                pkDesc->m_fDirPlanarAngle += NI_PI;
            }
        }
        else
        {
            if (kSpeedDirection.y > 0.0f)
            {
                pkDesc->m_fDirPlanarAngle = NI_HALF_PI;
            }
            else if (kSpeedDirection.y < 0.0f)
            {
                pkDesc->m_fDirPlanarAngle = 3.0f * NI_HALF_PI;
            }
            else
            {
                pkDesc->m_fDirPlanarAngle = 0.0f;
            }
        }
        pkDesc->m_fDirPlanarAngleVar = NI_TWO_PI * fSpeedDirectionVar;
    }

    pkDesc->m_fSize = fSize;
    pkDesc->m_fSizeVar = fSizeVar * pkDesc->m_fSize;

    pkDesc->m_fEmitStart = kEmitStart * sfTimeFactor;
    pkDesc->m_fEmitStop = kEmitStop * sfTimeFactor;

    pkDesc->m_fLifeSpan = kLifeSpan * sfTimeFactor;
    // we must account for the fact that NiParticle calculates lifespan by
    // taking Lifespan + LifeSpanVar * (UnitRandom - 0.5);
    pkDesc->m_fLifeSpanVar = kLifeSpanVar * sfTimeFactor * 2.0f;
    pkDesc->m_bUseBirthRate = true;
    pkDesc->m_bSpawnOnDeath = (iSpawnType == 3);

    if (pkDesc->m_fEmitStart == pkDesc->m_fEmitStop)
    {
        pkDesc->m_fEmitStop = pkDesc->m_fEmitStart + PSYS_INTERVAL_TIME;
        if (iLimitUsingTotal == 0)
            iTotalParticles = iBirthRate;

        iLimitUsingTotal = 1;

    }

    if (iLimitUsingTotal == 0)
    {
        if (pkDesc->m_bNiPSysUseViewport)
        {
            pkDesc->m_fBirthRate = fPercentageShown * iBirthRate * 
                GetFrameRate();
        }
        else 
        {
            pkDesc->m_fBirthRate = (float) iBirthRate * (float)
                GetFrameRate();
        }
    }
    else
    {
        float fTimeSpan = pkDesc->m_fEmitStop - pkDesc->m_fEmitStart;

        if (pkDesc->m_bNiPSysUseViewport)
        {
            pkDesc->m_fBirthRate = fPercentageShown * iTotalParticles  / 
                fTimeSpan;
        }
        else
        {
            pkDesc->m_fBirthRate = iTotalParticles  / fTimeSpan;
        }
    }
    
    pkDesc->m_fEmitterWidth = fWidth;
    pkDesc->m_fEmitterHeight = fHeight;
    pkDesc->m_fEmitterDepth = fDepth;

    pkDesc->m_bDieOnCollide = (iSpawnType == 1 || iSpawnType == 2);
    pkDesc->m_bSpawnOnCollide = (iSpawnType == 2);
    pkDesc->m_usNumGenerations = (iSpawnType == 0 ? 0 : iNumSpawns);
    pkDesc->m_fPercentageSpawned = (float)iSpawnPercent * 0.01f;
    pkDesc->m_usMultiplier = iSpawnMultiplier;
    pkDesc->m_fMultiplierVar = fSpawnMultiplierVar;
    pkDesc->m_fSpeedChaos = fSpawnSpeedChaos * 0.01f;
    switch (iSpawnSpeedSign)
    {
        case 0:
            pkDesc->m_fSpeedChaos *= -1.0f;
            break;
        case 1:
        case 2:
            break;
    };
    pkDesc->m_fDirChaos = fSpawnDirChaos;

    pkDesc->m_usViewportTotal = (unsigned short)
        (fPercentageShown * iTotalParticles);
    pkDesc->m_usRendererTotal = (unsigned short)
        (iTotalParticles);

    pkDesc->m_fGrowFor = kGrowFor * sfTimeFactor;
    pkDesc->m_fFadeFor = kFadeFor * sfTimeFactor;

    pkDesc->m_fSpinTime = kSpinTime * sfTimeFactor;
    pkDesc->m_fSpinTimeVar = fSpinTimeVar;
    pkDesc->m_bRandomSpinAxis = (iSpinAxisType != 2);
    pkDesc->m_kSpinAxis = kSpinAxis;
    pkDesc->m_fSpinAxisVar = fSpinAxisVar;
    pkDesc->m_fSpinPhase = fSpinPhase;
    pkDesc->m_fSpinPhaseVar = fSpinPhaseVar;

    if (iParticleType == 2 && pkPCld->custnode)
    {
        // Instanced particles
        NiMAXHierConverter kHier(m_kAnimStart, m_kAnimEnd);

        unsigned int uiNumMutations = pkPCld->nlist.Count();

        pkDesc->m_usNumMeshes = uiNumMutations + 1;

        pkDesc->m_pspMeshes = NiNew NiAVObjectPtr[uiNumMutations + 1];

        NiNodePtr spResult;
        for (unsigned int i = 0; i < uiNumMutations; i++)
        {
            int iStatus = kHier.BuildNode(pkIntf, pkPCld->nlist[i], 
                &spResult, pkParentScale, false, false);
            ReplaceRoot(&spResult);
            pkDesc->m_pspMeshes[i + 1] = spResult;
        }

        int iStatus = kHier.BuildNode(pkIntf, pkPCld->custnode, &spResult, 
            pkParentScale, false, false);
        ReplaceRoot(&spResult);
        pkDesc->m_pspMeshes[0] = spResult;
    }

    // Extract animations
    if (iLimitUsingTotal == 0)
    {
        float fBRScalar = 1.0f;
        if (pkDesc->m_bNiPSysUseViewport)
            fBRScalar = fPercentageShown * GetFrameRate();
        else 
            fBRScalar = (float) GetFrameRate();

        pkDesc->m_spBirthRateInterp = BuildFloatInterp(pkPBlock, 
            PB_PCLD_PBIRTHRATE, pkDesc->m_eBirthRateInterpCycle, fBRScalar);
    }
    pkDesc->m_spLifeSpanInterp = BuildFloatInterp(pkPBlock, PB_PCLD_LIFE,
        pkDesc->m_eLifeSpanInterpCycle, sfTimeFactor);
    pkDesc->m_spSpeedInterp = BuildFloatInterp(pkPBlock, PB_PCLD_SPEED, 
        pkDesc->m_eSpeedInterpCycle, (float) GetFrameRate());
    pkDesc->m_spSizeInterp = BuildFloatInterp(pkPBlock, PB_PCLD_SIZE,
        pkDesc->m_eSizeInterpCycle);
    pkDesc->m_spSpinPhaseInterp = BuildFloatInterp(pkPBlock, 
        PB_PCLD_SPINPHASE, pkDesc->m_eSpinPhaseInterpCycle);
    pkDesc->m_spSpinPhaseVarInterp = BuildFloatInterp(pkPBlock, 
        PB_PCLD_SPINPHASEVAR, pkDesc->m_eSpinPhaseVarInterpCycle);

    CHECK_MEMORY();

    return(W3D_STAT_OK);
}
//---------------------------------------------------------------------------
int NiMAXPSysConverter::ExtractOldPSys(INode* pkMaxNode, 
    SimpleParticle* pkPart, NiPSysDesc* pkDesc)
{
    CHECK_MEMORY();
    Class_ID eID = pkPart->ClassID();
    IParamBlock* pkPBlock = pkPart->pblock;

    // Particles section
    int iViewportNum, iRenderNum;
    pkPBlock->GetValue(PB_VPTPARTICLES, m_kAnimStart, iViewportNum, FOREVER);
    pkPBlock->GetValue(PB_RNDPARTICLES, m_kAnimStart, iRenderNum, FOREVER);
    float fSize;
    pkPBlock->GetValue(PB_DROPSIZE, m_kAnimStart, fSize, FOREVER);
    float fSpeed, fSpeedVar;
    pkPBlock->GetValue(PB_SPEED, m_kAnimStart, fSpeed, FOREVER);
    pkPBlock->GetValue(PB_VARIATION, m_kAnimStart, fSpeedVar, FOREVER);
    float fTumbleAmount = 0.0f, fTumbleRate = 0.0f;
    if (eID == Class_ID(SNOW_CLASS_ID, 0))
    {
        pkPBlock->GetValue(PB_TUMBLE, m_kAnimStart, fTumbleAmount, FOREVER);
        pkPBlock->GetValue(PB_SCALE, m_kAnimStart, fTumbleRate, FOREVER);
    }

    // Render
    int iDisplayType;
    pkPBlock->GetValue(PB_DISPTYPE, m_kAnimStart, iDisplayType, FOREVER);

    // Timing
    TimeValue kEmitStart, kLifeSpan;
    pkPBlock->GetValue(PB_STARTTIME, m_kAnimStart, kEmitStart, FOREVER);
    pkPBlock->GetValue(PB_LIFETIME, m_kAnimStart, kLifeSpan, FOREVER);
    float fBirthRate;
    pkPBlock->GetValue(PB_BIRTHRATE, m_kAnimStart, fBirthRate, FOREVER);
    
    int iMaxSustainable;
    pkPBlock->GetValue(PB_CONSTANT, m_kAnimStart, iMaxSustainable, FOREVER);

    // Emitter
    float fWidth, fHeight;
    pkPBlock->GetValue(PB_EMITTERWIDTH, m_kAnimStart, fWidth, FOREVER);
    pkPBlock->GetValue(PB_EMITTERHEIGHT, m_kAnimStart, fHeight, FOREVER);
    int iHideEmitter;
    pkPBlock->GetValue(PB_HIDEEMITTER, m_kAnimStart, iHideEmitter, FOREVER);

    // Unknown
    int iRender;
    pkPBlock->GetValue(PB_RENDER, m_kAnimStart, iRender, FOREVER);

    // Set up parameters
    pkDesc->m_fSpeed = 0.4f * fSpeed * GetFrameRate();
    pkDesc->m_fSpeedVar = 0.4f * fSpeedVar * GetFrameRate();

    pkDesc->m_fDirDeclination = NI_PI;
    pkDesc->m_fDirDeclinationVar = NiATan(fSpeedVar / (2.0f * fSpeed));
    pkDesc->m_fDirPlanarAngle = 0.0f;
    pkDesc->m_fDirPlanarAngleVar = 0.0f;

    pkDesc->m_fSize = fSize;

    pkDesc->m_fEmitStart = kEmitStart * sfTimeFactor;
    pkDesc->m_fEmitStop = m_kAnimEnd * sfTimeFactor;
 
    pkDesc->m_fBirthRate = fBirthRate / sfTimeFactor;
    pkDesc->m_fLifeSpan = kLifeSpan * sfTimeFactor;
    pkDesc->m_bUseBirthRate = true;
    pkDesc->m_fEmitterWidth = fWidth;
    pkDesc->m_fEmitterHeight = fHeight;
    pkDesc->m_fEmitterDepth = 0.0f;

    pkDesc->m_usViewportTotal = iViewportNum;
    pkDesc->m_usRendererTotal = iRenderNum;

    if (iMaxSustainable  != 0 && pkDesc->m_bNiPSysUseViewport)
    {
        pkDesc->m_fBirthRate = iViewportNum  / pkDesc->m_fLifeSpan;
    }
    else if (iMaxSustainable  != 0)
    {
        pkDesc->m_fBirthRate = iRenderNum  / pkDesc->m_fLifeSpan;
    }
    else
    {
        pkDesc->m_spBirthRateInterp = BuildFloatInterp(pkPBlock, PB_BIRTHRATE,
            pkDesc->m_eBirthRateInterpCycle, 160.0f * (float)GetFrameRate());
    }
    
    pkDesc->m_fSpinTime = fTumbleAmount * fTumbleRate;
    pkDesc->m_bRandomSpinAxis = true;

    if (eID == Class_ID(SNOW_CLASS_ID, 0) && iRender < 2)
    {
        NiTriShape* pkResult;

        // Build flake
        BuildFlake(&pkResult, fSize);

        pkDesc->m_usNumMeshes = 1;
        pkDesc->m_pspMeshes = NiNew NiAVObjectPtr[1];
        pkDesc->m_pspMeshes[0] = pkResult;
    }

    // Extract animations
    pkDesc->m_spLifeSpanInterp = BuildFloatInterp(pkPBlock, PB_LIFETIME,
        pkDesc->m_eLifeSpanInterpCycle, sfTimeFactor);
    pkDesc->m_spSpeedInterp = BuildFloatInterp(pkPBlock, PB_SPEED,
        pkDesc->m_eSpeedInterpCycle, 0.4f *  GetFrameRate());
    pkDesc->m_spSizeInterp = BuildFloatInterp(pkPBlock, PB_DROPSIZE,
        pkDesc->m_eSizeInterpCycle);

    CHECK_MEMORY();
    return(W3D_STAT_OK);
}
//---------------------------------------------------------------------------
int NiMAXPSysConverter::BuildFlake(NiTriShape** ppkResult, float fSize)
{
     CHECK_MEMORY();
   NiPoint3* pkVerts = NiNew NiPoint3[8];
    if (pkVerts == NULL)
        return(W3D_STAT_NO_MEMORY);

    NiPoint3* pkNorms = NiNew NiPoint3[8];
    if (pkNorms == NULL)
        return(W3D_STAT_NO_MEMORY);

    NiPoint2* pkUVs = NiNew NiPoint2[8];
    if (pkUVs == NULL)
        return(W3D_STAT_NO_MEMORY);

    unsigned short* pkIndices = NiAlloc(unsigned short, 12);
    if (pkIndices == NULL)
        return(W3D_STAT_NO_MEMORY);

    pkVerts[0] = pkVerts[4] = NiPoint3(-fSize, 0.0f, -fSize);
    pkVerts[1] = pkVerts[5] = NiPoint3(fSize, 0.0f, -fSize);
    pkVerts[2] = pkVerts[6] = NiPoint3(fSize, 0.0f, fSize);
    pkVerts[3] = pkVerts[7] = NiPoint3(-fSize, 0.0f, fSize);

    for (unsigned int i = 0; i < 4; i++)
    {
        pkNorms[i] = NiPoint3(0.0f, 1.0f, 0.0f);
        pkNorms[i+4] = NiPoint3(0.0f, -1.0f, 0.0f);
    }

    pkUVs[0] = pkUVs[4] = NiPoint2(0.0f, 1.0f);
    pkUVs[1] = pkUVs[5] = NiPoint2(1.0f, 1.0f);
    pkUVs[2] = pkUVs[6] = NiPoint2(1.0f, 0.0f);
    pkUVs[3] = pkUVs[7] = NiPoint2(0.0f, 0.0f);

    pkIndices[0] = 0;
    pkIndices[1] = 1;
    pkIndices[2] = 2;

    pkIndices[3] = 0;
    pkIndices[4] = 2;
    pkIndices[5] = 3;

    pkIndices[6] = 4;
    pkIndices[7] = 6;
    pkIndices[8] = 5;

    pkIndices[9] = 4;
    pkIndices[10] = 7;
    pkIndices[11] = 6;

    *ppkResult = NiNew NiTriShape(8, pkVerts, pkNorms, NULL, pkUVs, 1,
        NiGeometryData::NBT_METHOD_NONE, 4, pkIndices);
    if (*ppkResult == NULL)
        return(W3D_STAT_NO_MEMORY);
    else
        return(W3D_STAT_OK);
}

//---------------------------------------------------------------------------
void NiMAXPSysConverter::MergeBirthRateTrackWithActiveKeyTrack(
    unsigned int& uiNumKeys, NiStepBoolKey*& pkBoolKeys, 
    unsigned int uiNumBirthRateKeys, NiFloatKey* pkBRKeys, 
    NiFloatKey::KeyType eBirthRateKeyType)
{

    if (uiNumBirthRateKeys == 1)
        return;

    unsigned int uiNumEmitterActiveKeys;
    NiStepBoolKey* pkEmitterActiveKeys;

    SetEmitterControllerStartAndStop(uiNumEmitterActiveKeys, 
        pkEmitterActiveKeys, uiNumBirthRateKeys, pkBRKeys,
        eBirthRateKeyType);

    unsigned int uiFinalKeyCount = uiNumKeys + uiNumEmitterActiveKeys;
    NiStepBoolKey* pkFinalBoolKeys = NiNew NiStepBoolKey[uiFinalKeyCount];
    unsigned int uiIndex = 0;
    unsigned int uiTrack1Index = 0;
    bool bOn = false;
    unsigned int ui = 0;

    for (; ui < uiNumKeys; ui++)
    {
        pkFinalBoolKeys[uiIndex].SetTime(pkBoolKeys[ui].GetTime());
        pkFinalBoolKeys[uiIndex].SetBool(pkBoolKeys[ui].GetBool());
        uiIndex++;
        uiTrack1Index++;

        if (pkBoolKeys[ui].GetBool() == true)
            break;    
    }

    for (unsigned int uiTrack2Index = 0; uiTrack2Index < 
        uiNumEmitterActiveKeys; uiTrack2Index++)
    {
        if (pkEmitterActiveKeys[uiTrack2Index].GetTime() < 
            pkFinalBoolKeys[uiIndex-1].GetTime())
        {
            continue;
        }
        else if(pkEmitterActiveKeys[uiTrack2Index].GetTime() == 
            pkFinalBoolKeys[uiIndex-1].GetTime())
        {
            uiIndex--;
        }

        if (pkEmitterActiveKeys[uiTrack2Index].GetTime() >=
            pkBoolKeys[uiTrack1Index].GetTime())
        {
            break;
        }

        pkFinalBoolKeys[uiIndex].SetTime(
            pkEmitterActiveKeys[uiTrack2Index].GetTime());
        pkFinalBoolKeys[uiIndex].SetBool(
            pkEmitterActiveKeys[uiTrack2Index].GetBool());
        uiIndex++;
    }

    for (ui = uiTrack1Index; ui < uiNumKeys; ui++)
    {
        pkFinalBoolKeys[uiIndex].SetTime(pkBoolKeys[ui].GetTime());
        pkFinalBoolKeys[uiIndex].SetBool(pkBoolKeys[ui].GetBool());
        uiIndex++;
    }

    NiDelete [] pkBoolKeys;
    NiDelete [] pkEmitterActiveKeys;
    uiNumKeys = uiIndex;
    pkBoolKeys = pkFinalBoolKeys;
}
//---------------------------------------------------------------------------
NiPSysEmitter* NiMAXPSysConverter::BuildEmitterInfo(NiPSysDesc* pkDesc, 
    NiParticleSystem* pkPSystem)
{
    NiAVObject* pkMAXEmitterObj = NULL;
    char acName[256];
    
    NiPSysEmitter* pkEmitter = NULL;
    bool bCreateLinkup = true;

    switch (pkDesc->m_eEmitterType)
    {
        default:
        case EMITTER_VOLUME:
            NiSprintf(acName, 256, "NiPSysBoxEmitter:%d", ms_uiTotalCount);
            pkEmitter = NiNew NiPSysBoxEmitter(
                acName, 
                pkMAXEmitterObj, 
                pkDesc->m_fEmitterWidth, 
                pkDesc->m_fEmitterHeight,
                pkDesc->m_fEmitterDepth);
            break;
        case EMITTER_OBJECT:
            NiSprintf(acName, 256, "NiPSysMeshEmitter:%d", ms_uiTotalCount);
            pkEmitter = NiNew NiPSysMeshEmitter(
                acName, NULL, pkDesc->m_eEmissionType, 
                pkDesc->m_eInitVelocityType,
                NiPoint3::UNIT_X);
            break;
        case EMITTER_SPHERE:
            NiSprintf(acName, 256, "NiPSysSphereEmitter:%d", ms_uiTotalCount);
            pkEmitter = NiNew NiPSysSphereEmitter(
                acName, 
                pkMAXEmitterObj, 
                pkDesc->m_fEmitterHeight);
            break;
        case EMITTER_BOX:
            NiSprintf(acName, 256, "NiPSysBoxEmitter:%d", ms_uiTotalCount);
            pkEmitter = NiNew NiPSysBoxEmitter(
                acName, 
                pkMAXEmitterObj, 
                pkDesc->m_fEmitterWidth, 
                pkDesc->m_fEmitterHeight,
                pkDesc->m_fEmitterDepth);
            break;
        case EMITTER_CYLINDER:
            NiSprintf(acName, 256, "NiPSysCylinderEmitter:%d", 
                ms_uiTotalCount);
            pkEmitter = NiNew NiPSysCylinderEmitter(
                acName, 
                pkMAXEmitterObj, 
                pkDesc->m_fEmitterHeight / 2.0f, 
                pkDesc->m_fEmitterDepth);
            break;
    };

    NIASSERT(pkEmitter);
    pkEmitter->SetSpeed(pkDesc->m_fSpeed);
    pkEmitter->SetSpeedVar(pkDesc->m_fSpeedVar);
    pkEmitter->SetDeclination(pkDesc->m_fDirDeclination);
    pkEmitter->SetDeclinationVar(pkDesc->m_fDirDeclinationVar);
    pkEmitter->SetPlanarAngle(pkDesc->m_fDirPlanarAngle);
    pkEmitter->SetPlanarAngleVar(pkDesc->m_fDirPlanarAngleVar);
    pkEmitter->SetLifeSpan(pkDesc->m_fLifeSpan);
    pkEmitter->SetLifeSpanVar(pkDesc->m_fLifeSpanVar);
    pkEmitter->SetInitialRadius(pkDesc->m_fSize);
    pkEmitter->SetRadiusVar(pkDesc->m_fSizeVar);

    // Put particle control and emitter on a list to resolve emitter pointer 
    //   after all the nodes are converted
    if (bCreateLinkup)
    {
        ParticleLinkUpInfo* pkParticleLinkUpInfo = NiNew ParticleLinkUpInfo;
        pkParticleLinkUpInfo->m_pkMAXNode = pkDesc->m_pkMAXEmitter;
        pkParticleLinkUpInfo->m_spSystem = pkPSystem;
        pkParticleLinkUpInfo->m_spModifier = pkEmitter;
        ms_kLinkUpList.AddHead(pkParticleLinkUpInfo);
    }

    pkPSystem->AddModifier(pkEmitter);
    ms_uiTotalCount++;
    pkDesc->m_spEmitter = pkEmitter;

    unsigned int uiNumBirthRateKeys = 1;
    NiFloatKey* pkBRKeys = NULL;
    NiAnimationKey::KeyType eType;

    if (pkDesc->m_spBirthRateInterp)
    {
        unsigned char ucSize;
        NiFloatKey* pkOldBRKeys = (NiFloatKey*) 
            pkDesc->m_spBirthRateInterp->GetAnim(
            uiNumBirthRateKeys, eType, ucSize);

        NiAnimationKey::ArrayFunction pArrayFunction = 
            NiAnimationKey::GetArrayFunction(NiAnimationKey::FLOATKEY, eType);
        pkBRKeys = (NiFloatKey*) pArrayFunction(uiNumBirthRateKeys);

        NiAnimationKey::CopyFunction pCopyFunc = 
            NiAnimationKey::GetCopyFunction(NiAnimationKey::FLOATKEY, eType);
        for (unsigned int ui = 0; ui < uiNumBirthRateKeys; ui++)
        {
            NiFloatKey* pkNewKey = pkBRKeys->GetKeyAt(ui, ucSize);
            NiFloatKey* pkOldKey = pkOldBRKeys->GetKeyAt(ui, ucSize);
            pCopyFunc(pkNewKey, pkOldKey);
        }
    }
    else
    {
        NiLinFloatKey* pkBRTempKeys = NiNew NiLinFloatKey[1];
        pkBRTempKeys[0].SetTime(pkDesc->m_fEmitStart);
        pkBRTempKeys[0].SetValue(pkDesc->m_fBirthRate);
        uiNumBirthRateKeys = 1;
        eType = NiAnimationKey::LINKEY;
        pkBRKeys = pkBRTempKeys;
    }

    unsigned int uiNumKeys = 0;
    
    NiStepBoolKey* pkBoolKeys = BuildActiveKeys(pkDesc->m_fNiPSysLoopStart, 
        pkDesc->m_fNiPSysLoopEnd, pkDesc->m_fEmitStart, pkDesc->m_fEmitStop, 
        uiNumKeys);  

    MergeBirthRateTrackWithActiveKeyTrack(uiNumKeys, pkBoolKeys, 
        uiNumBirthRateKeys, pkBRKeys, eType);

    NiBoolInterpolator* pkEmitInterp = NiNew NiBoolInterpolator();
    NiFloatInterpolator* pkBirthRateInterp = NiNew NiFloatInterpolator();
    pkEmitInterp->SetKeys(pkBoolKeys, NiAnimationKey::STEPKEY, uiNumKeys);
    pkBirthRateInterp->SetKeys(pkBRKeys, uiNumBirthRateKeys, eType);

    NiPSysEmitterCtlr* pkEmitController = 
        NiNew NiPSysEmitterCtlr(pkEmitter->GetName());
    pkEmitController->SetBirthRateInterpolator(pkBirthRateInterp);
    pkEmitController->SetEmitterActiveInterpolator(pkEmitInterp);
    pkEmitController->ResetTimeExtrema();

    pkEmitInterp->Collapse();
    pkBirthRateInterp->Collapse();
    
    if (pkDesc->m_bNiPSysLoop)
        pkEmitController->SetCycleType(NiTimeController::LOOP);
    else
        pkEmitController->SetCycleType(NiTimeController::CLAMP);
    pkEmitController->SetTarget(pkPSystem);
    
    if (pkDesc->m_spSpeedInterp)
    {
        NiPSysEmitterSpeedCtlr* pkEmitSpeedController = 
            NiNew NiPSysEmitterSpeedCtlr(pkEmitter->GetName());
        pkEmitSpeedController->SetInterpolator(pkDesc->m_spSpeedInterp);
        pkEmitSpeedController->ResetTimeExtrema();
        pkEmitSpeedController->SetCycleType(pkDesc->m_eSpeedInterpCycle);
        pkEmitSpeedController->SetTarget(pkPSystem);
    }
    if (pkDesc->m_spDeclinationInterp)
    {
        NiPSysEmitterDeclinationCtlr* pkEmitDeclinationController = 
            NiNew NiPSysEmitterDeclinationCtlr(pkEmitter->GetName());
        pkEmitDeclinationController->SetInterpolator(
            pkDesc->m_spDeclinationInterp);
        pkEmitDeclinationController->ResetTimeExtrema();
        pkEmitDeclinationController->SetCycleType(
            pkDesc->m_eDeclinationInterpCycle);
        pkEmitDeclinationController->SetTarget(pkPSystem);
    }
    if (pkDesc->m_spDeclinationVarInterp)
    {
        NiPSysEmitterDeclinationVarCtlr* pkEmitDeclinationVarController = 
            NiNew NiPSysEmitterDeclinationVarCtlr(pkEmitter->GetName());
        pkEmitDeclinationVarController->SetInterpolator(
            pkDesc->m_spDeclinationVarInterp);
        pkEmitDeclinationVarController->ResetTimeExtrema();
        pkEmitDeclinationVarController->SetCycleType(
            pkDesc->m_eDeclinationVarInterpCycle);
        pkEmitDeclinationVarController->SetTarget(pkPSystem);
    }
    if (pkDesc->m_spPlanarAngleInterp)
    {
        NiPSysEmitterPlanarAngleCtlr* pkEmitPlanarAngleController = 
            NiNew NiPSysEmitterPlanarAngleCtlr(pkEmitter->GetName());
        pkEmitPlanarAngleController->SetInterpolator(
            pkDesc->m_spPlanarAngleInterp);
        pkEmitPlanarAngleController->ResetTimeExtrema();
        pkEmitPlanarAngleController->SetCycleType(
            pkDesc->m_ePlanarAngleInterpCycle);
        pkEmitPlanarAngleController->SetTarget(pkPSystem);
    }
    if (pkDesc->m_spPlanarAngleVarInterp)
    {
        NiPSysEmitterPlanarAngleVarCtlr* pkEmitPlanarAngleVarController = 
            NiNew NiPSysEmitterPlanarAngleVarCtlr(pkEmitter->GetName());
        pkEmitPlanarAngleVarController->SetInterpolator(
            pkDesc->m_spPlanarAngleVarInterp);
        pkEmitPlanarAngleVarController->ResetTimeExtrema();
        pkEmitPlanarAngleVarController->SetCycleType(
            pkDesc->m_ePlanarAngleVarInterpCycle);
        pkEmitPlanarAngleVarController->SetTarget(pkPSystem);
    }
    if (pkDesc->m_spSizeInterp)
    {
        NiPSysEmitterInitialRadiusCtlr* pkEmitSizeController = 
            NiNew NiPSysEmitterInitialRadiusCtlr(pkEmitter->GetName());
        pkEmitSizeController->SetInterpolator(pkDesc->m_spSizeInterp);
        pkEmitSizeController->ResetTimeExtrema();
        pkEmitSizeController->SetCycleType(pkDesc->m_eSizeInterpCycle);
        pkEmitSizeController->SetTarget(pkPSystem);
    }
    if (pkDesc->m_spLifeSpanInterp)
    {
        NiPSysEmitterLifeSpanCtlr* pkEmitLifeSpanController = 
            NiNew NiPSysEmitterLifeSpanCtlr(pkEmitter->GetName());
        pkEmitLifeSpanController->SetInterpolator(
            pkDesc->m_spLifeSpanInterp);
        pkEmitLifeSpanController->ResetTimeExtrema();
        pkEmitLifeSpanController->SetCycleType(
            pkDesc->m_eLifeSpanInterpCycle);
        pkEmitLifeSpanController->SetTarget(pkPSystem);
    }


    return pkEmitter;
}
//---------------------------------------------------------------------------
NiPSysSpawnModifier* NiMAXPSysConverter::BuildSpawnInfo(NiPSysDesc* pkDesc,
    NiParticleSystem* pkPSystem)
{
    unsigned short usMinNumToSpawn = pkDesc->m_usMultiplier - (unsigned short)
        (pkDesc->m_fMultiplierVar * pkDesc->m_usMultiplier);
    if (usMinNumToSpawn == 0)
    {
        usMinNumToSpawn = 1;
    }

    char acName[256];
    NiSprintf(acName, 256, "NiPSysSpawnModifier:%d", ms_uiTotalCount);
    NiPSysSpawnModifier* pkSpawnModifier = NiNew NiPSysSpawnModifier(
        acName, pkDesc->m_usNumGenerations, pkDesc->m_fPercentageSpawned,
        usMinNumToSpawn, pkDesc->m_usMultiplier, pkDesc->m_fSpeedChaos,
        pkDesc->m_fDirChaos, pkDesc->m_fLifeSpan, pkDesc->m_fLifeSpanVar);

    pkDesc->m_pkSpawnModifier = pkSpawnModifier;

    pkPSystem->AddModifier(pkSpawnModifier);
    ms_uiTotalCount++;

    return pkSpawnModifier;
}
//---------------------------------------------------------------------------
bool NiMAXPSysConverter::BuildAgeBasedDeathInfo(NiPSysDesc* pkDesc, 
    NiParticleSystem* pkPSystem, NiPSysSpawnModifier* pkSpawnModifier)
{
    char acName[256];
    NiSprintf(acName, 256, "NiPSysAgeDeath:%d", ms_uiTotalCount);
    NiPSysAgeDeathModifier* pkModifier = NiNew NiPSysAgeDeathModifier(acName,
        pkDesc->m_bSpawnOnDeath, pkSpawnModifier);

    pkPSystem->AddModifier(pkModifier);
    ms_uiTotalCount++;

    return true;
}
//---------------------------------------------------------------------------
bool NiMAXPSysConverter::BuildGrowFadeInfo(NiPSysDesc* pkDesc, 
    NiParticleSystem* pkPSystem, NiPSysSpawnModifier* pkSpawnModifier)
{
    if (pkDesc->m_fGrowFor > 0.0f || pkDesc->m_fFadeFor > 0.0f)
    {
        char acName[256];
        NiSprintf(acName, 256, "NiPSysGrowFadeModifier:%d", ms_uiTotalCount);
    
        NiPSysGrowFadeModifier* pkGrowFadeMod = 
            NiNew NiPSysGrowFadeModifier(acName, pkDesc->m_fGrowFor,
            0, pkDesc->m_fFadeFor, pkDesc->m_usNumGenerations);

        pkPSystem->AddModifier(pkGrowFadeMod);
        ms_uiTotalCount++;
    }
    return true;
}
//---------------------------------------------------------------------------
bool NiMAXPSysConverter::BuildColorModInfo(NiPSysDesc* pkDesc, 
    NiParticleSystem* pkPSystem, NiPSysSpawnModifier* pkSpawnModifier)
{
    if (pkDesc->m_bColorAges)
    {
        char acName[256];
        NiSprintf(acName, 256, "NiPSysColorModifier:%d", ms_uiTotalCount);
    
        NiPSysColorModifier* pkColorMod = NiNew 
            NiPSysColorModifier(acName);

        NiLinColorKey* pKeys = pkDesc->m_pkColors;
        NIASSERT(pKeys);

        NiColorData* pkColorData = NiNew NiColorData;
        pkColorData->ReplaceAnim(pKeys, pkDesc->m_uiNumColorKeys,
            NiAnimationKey::LINKEY);

        pkColorMod->SetColorData(pkColorData);
        pkPSystem->AddModifier(pkColorMod);
        ms_uiTotalCount++;
    }

    return true;
}
//---------------------------------------------------------------------------
void RecursivelyUnhide(NiAVObject* pkObj)
{
    if (!pkObj)
        return;

    pkObj->SetAppCulled(false);
    if (NiIsKindOf(NiNode, pkObj))
    {
        NiNode* pkNode = (NiNode*) pkObj;
        for (unsigned int ui = 0; ui < pkNode->GetArrayCount(); ui++)
            RecursivelyUnhide(pkNode->GetAt(ui));
    }
}
//---------------------------------------------------------------------------
bool NiMAXPSysConverter::BuildMeshModInfo(NiPSysDesc* pkDesc, 
    NiParticleSystem* pkPSystem, NiPSysSpawnModifier* pkSpawnModifier)
{
    if (pkDesc->m_usNumMeshes > 0)
    {
        char acName[256];
        NiSprintf(acName, 256, "NiPSysMeshUpdateModifier:%d", 
            ms_uiTotalCount);
    
        
        NiPSysMeshUpdateModifier* pkMeshMod = 
            NiNew NiPSysMeshUpdateModifier(acName);

        for (unsigned int i = 0; i < pkDesc->m_usNumMeshes; i++)
        {

            pkMeshMod->SetMeshAt(i, pkDesc->m_pspMeshes[i]);
            if (pkDesc->m_pspMeshes[i])
                RecursivelyUnhide(pkDesc->m_pspMeshes[i]);
        }

        pkPSystem->AddModifier(pkMeshMod);
        ms_uiTotalCount++;
    }
    return true;
}
//---------------------------------------------------------------------------
void ScaleFloatInterpolator(NiFloatInterpolator* pkInterp, float fScalar)
{
    if (pkInterp)
    {
        if (fScalar != 1.0f)
        {
            for (unsigned int ui = 0; ui < pkInterp->GetKeyCount(0); ui++)
            {
                NiFloatKey* pkKey = (NiFloatKey*)pkInterp->GetKeyAt(ui,0);
                pkKey->SetValue(pkKey->GetValue() * fScalar);
            }
        }
    }
}
//---------------------------------------------------------------------------
bool NiMAXPSysConverter::BuildRotationInfo(NiPSysDesc* pkDesc, 
    NiParticleSystem* pkPSystem, NiPSysSpawnModifier* pkSpawnModifier)
{
    if (pkDesc->m_fNiPSysRotationSpeed != 0.0f || 
        pkDesc->m_fNiPSysRotationVariation != 0.0f ||
        pkDesc->m_fSpinPhase != 0.0f ||
        pkDesc->m_fSpinPhaseVar != 0.0f  ||
        pkDesc->m_spRotationSpeedInterp ||
        pkDesc->m_spRotationVariationInterp ||
        pkDesc->m_spSpinPhaseInterp ||
        pkDesc->m_spSpinPhaseVarInterp)
    {
        char acName[256];
        NiSprintf(acName, 256, "NiPSysRotationModifier:%d", ms_uiTotalCount);

        NiPSysRotationModifier* pkRotationMod = 
            NiNew NiPSysRotationModifier(acName);
        pkRotationMod->SetRandomInitialAxis(pkDesc->m_bRandomSpinAxis);
        pkDesc->m_kSpinAxis.Unitize();
        pkRotationMod->SetInitialAxis(pkDesc->m_kSpinAxis);
        
        pkRotationMod->SetInitialRotSpeed(
            pkDesc->m_fNiPSysRotationSpeed * (NI_TWO_PI / 360.0f));
        pkRotationMod->SetInitialRotSpeedVar(
            pkDesc->m_fNiPSysRotationVariation * (NI_TWO_PI / 360.0f));
        pkRotationMod->SetInitialRotAngle(-pkDesc->m_fSpinPhase);
        pkRotationMod->SetInitialRotAngleVar(pkDesc->m_fSpinPhaseVar 
            * pkDesc->m_fSpinPhase);
        pkRotationMod->SetRandomRotSpeedSign(
            pkDesc->m_bNiPSysBidirectionalRotation);
        pkPSystem->AddModifier(pkRotationMod);

        if (pkDesc->m_spRotationSpeedInterp)
        {
            ScaleFloatInterpolator(pkDesc->m_spRotationSpeedInterp, 
                (NI_TWO_PI / 360.0f));
            NiPSysInitialRotSpeedCtlr* pkRotationSpeedController = 
                NiNew NiPSysInitialRotSpeedCtlr(pkRotationMod->GetName());
            pkRotationSpeedController->SetInterpolator(
                pkDesc->m_spRotationSpeedInterp);
            pkRotationSpeedController->ResetTimeExtrema();
            pkRotationSpeedController->SetCycleType(
                pkDesc->m_eRotationSpeedInterpCycle);
            pkRotationSpeedController->SetTarget(pkPSystem);
        }
        if (pkDesc->m_spRotationVariationInterp)
        {
            ScaleFloatInterpolator(pkDesc->m_spRotationVariationInterp,
                (NI_TWO_PI / 360.0F));
            NiPSysInitialRotSpeedVarCtlr* pkRotationSpeedVarController = 
                NiNew NiPSysInitialRotSpeedVarCtlr(pkRotationMod->GetName());
            pkRotationSpeedVarController->SetInterpolator(
                pkDesc->m_spRotationVariationInterp);
            pkRotationSpeedVarController->ResetTimeExtrema();
            pkRotationSpeedVarController->SetCycleType(
                pkDesc->m_eRotationVariationInterpCycle);
            pkRotationSpeedVarController->SetTarget(pkPSystem);
        }
        if (pkDesc->m_spSpinPhaseInterp)
        {
            // reverse the direction of our rotation keys
            ScaleFloatInterpolator(pkDesc->m_spSpinPhaseInterp, -1.0f);
            NiPSysInitialRotAngleCtlr* pkInitialRotationController = 
                NiNew NiPSysInitialRotAngleCtlr(pkRotationMod->GetName());
            pkInitialRotationController->SetInterpolator(
                pkDesc->m_spSpinPhaseInterp);
            pkInitialRotationController->ResetTimeExtrema();
            pkInitialRotationController->SetCycleType(
                pkDesc->m_eSpinPhaseInterpCycle);
            pkInitialRotationController->SetTarget(pkPSystem);
        }
        if (pkDesc->m_spSpinPhaseVarInterp)
        {
            ScaleFloatInterpolator(pkDesc->m_spSpinPhaseVarInterp,
                pkDesc->m_fSpinPhase);
            NiPSysInitialRotAngleVarCtlr* pkInitialRotationVarController = 
                NiNew NiPSysInitialRotAngleVarCtlr(pkRotationMod->GetName());
            pkInitialRotationVarController->SetInterpolator(
                pkDesc->m_spSpinPhaseVarInterp);
            pkInitialRotationVarController->ResetTimeExtrema();
            pkInitialRotationVarController->SetCycleType(
                pkDesc->m_eSpinPhaseVarInterpCycle);
            pkInitialRotationVarController->SetTarget(pkPSystem);
        }

        ms_uiTotalCount++;
    }
    return true;
}

//---------------------------------------------------------------------------

#define PSYSLOOP_STRING         "NiPSysLoop"    
#define PSYSLOOPSTART_STRING    "NiPSysLoopStart"
#define PSYSLOOPEND_STRING      "NiPSysLoopEnd"
#define PSYSLUSEVIEWPORT_STRING "NiPSysUseViewport"
#define PSYSLOOPRESET_STRING    "NiPSysResetOnLoop"
#define PSYSMAXALLOC_STRING     "NiPSysMaxAlloc"
#define PSYSLOCALCOORDS_STRING      "NiPSysLocalCoords"
#define PSYSBOUNDSUPDATETYPE_STRING "NiPSysBoundsUpdateType" 
#define PSYSBOUNDPOSITION_STRING    "NiPSysBoundPosition"
#define PSYSBOUNDRADIUS_STRING      "NiPSysBoundRadius"
#define PSYSBOUNDSDYNAMICSKIP_STRING   "NiPSysDynamicBoundsSkip"
#define PSYSPARENTNODE_STRING   "NiPSysParentNode"
#define PSYSROTATIONSPEED_STRING "NiPSysRotationSpeed"
#define PSYSROTATIONVARIATION_STRING "NiPSysRotationVariation"
#define PSYSBIDIRECTIONALROTATION_STRING "NiPSysBidirectionalRotation"

int NiMAXPSysConverter::ExtractCustomAttribs(NiPSysDesc* pkDesc, 
    SimpleParticle* pkPart)
{
    NiNodePtr m_spTempNode = NiNew NiNode;
    NiMAXCustAttribConverter kConverter(m_kAnimStart, m_kAnimEnd);
    kConverter.Convert(pkPart, m_spTempNode, false);
    
    NiBooleanExtraData* pkLoopED = 
        (NiBooleanExtraData*) m_spTempNode->GetExtraData(PSYSLOOP_STRING);
    NIASSERT(pkLoopED);
    if (pkLoopED)
        pkDesc->m_bNiPSysLoop = pkLoopED->GetValue();
    else
        pkDesc->m_bNiPSysLoop = true;

    Interface* pIntf = GetCOREInterface();
    float animStart = (float) pIntf->GetAnimRange().Start();
    float animEnd = (float) pIntf->GetAnimRange().End();

    NiIntegerExtraData* pkLoopStartED = 
        (NiIntegerExtraData*) 
        m_spTempNode->GetExtraData(PSYSLOOPSTART_STRING);
    NIASSERT(pkLoopStartED);
    if (pkLoopStartED)
    {
        pkDesc->m_fNiPSysLoopStart = ((float)pkLoopStartED->GetValue())/
            GetFrameRate();
    }
    else
    {
        pkDesc->m_fNiPSysLoopStart = animStart * sfTimeFactor;
    }

    NiIntegerExtraData* pkLoopEndED = 
        (NiIntegerExtraData*) m_spTempNode->GetExtraData(PSYSLOOPEND_STRING);
    NIASSERT(pkLoopEndED);
    if (pkLoopEndED)
    {
        pkDesc->m_fNiPSysLoopEnd = ((float)pkLoopEndED->GetValue())/
            GetFrameRate();
    }
    else
    {
        pkDesc->m_fNiPSysLoopEnd =  animEnd * sfTimeFactor;
    }

    NiBooleanExtraData* pkUseViewportED = 
        (NiBooleanExtraData*) m_spTempNode->GetExtraData(
        PSYSLUSEVIEWPORT_STRING);
    NIASSERT(pkUseViewportED);
    if (pkUseViewportED)
        pkDesc->m_bNiPSysUseViewport = pkUseViewportED->GetValue();
    else
        pkDesc->m_bNiPSysUseViewport = true;

    NiBooleanExtraData* pkLoopResetED = 
        (NiBooleanExtraData*) m_spTempNode->GetExtraData(
        PSYSLOOPRESET_STRING);
    NIASSERT(pkLoopResetED);
    if (pkLoopResetED)
        pkDesc->m_bNiPSysResetOnLoop = pkLoopResetED->GetValue();
    else
        pkDesc->m_bNiPSysResetOnLoop = true;

    NiIntegerExtraData* pkMaxAllocED = 
        (NiIntegerExtraData*) m_spTempNode->GetExtraData(
        PSYSMAXALLOC_STRING);
    NIASSERT(pkMaxAllocED);
    if (pkMaxAllocED)
        pkDesc->m_usNiPSysMaxAlloc = pkMaxAllocED->GetValue();
    else
        pkDesc->m_usNiPSysMaxAlloc = 100;
    
    NiBooleanExtraData* pkLocalCoordsED = 
        (NiBooleanExtraData*) m_spTempNode->GetExtraData(
        PSYSLOCALCOORDS_STRING);
    NIASSERT(pkLocalCoordsED);
    if (pkLocalCoordsED)
        pkDesc->m_bLocalCoords = pkLocalCoordsED->GetValue();
    else
        pkDesc->m_bLocalCoords = false;

    NiIntegerExtraData* pkBoundsUpdateTypeED = 
        (NiIntegerExtraData*) m_spTempNode->GetExtraData(
        PSYSBOUNDSUPDATETYPE_STRING);
    NIASSERT(pkBoundsUpdateTypeED);
    if (pkBoundsUpdateTypeED)
        pkDesc->m_iBoundsUpdateType = pkBoundsUpdateTypeED->GetValue();
    else
        pkDesc->m_iBoundsUpdateType = 1;

    NiIntegerExtraData* pkBoundsDynamicSkipED = 
        (NiIntegerExtraData*) m_spTempNode->GetExtraData(
        PSYSBOUNDSDYNAMICSKIP_STRING);
    NIASSERT(pkBoundsDynamicSkipED);
    if (pkBoundsDynamicSkipED)
        pkDesc->m_iBoundsDynamicSkip = pkBoundsDynamicSkipED->GetValue()-1;
    else
        pkDesc->m_iBoundsDynamicSkip = 0;


    NiVectorExtraData* pkBoundPositionED = 
        (NiVectorExtraData*) m_spTempNode->GetExtraData(
        PSYSBOUNDPOSITION_STRING);
    NIASSERT(pkBoundPositionED);
    if (pkBoundPositionED)
    {
        pkDesc->m_kBoundPosition.x = pkBoundPositionED->GetValue()[0];
        pkDesc->m_kBoundPosition.y = pkBoundPositionED->GetValue()[1];
        pkDesc->m_kBoundPosition.z = pkBoundPositionED->GetValue()[2];
    }
    else
    {
        pkDesc->m_kBoundPosition = NiPoint3(0.0f, 0.0f, 0.0f);
    }

    NiBinaryExtraData* pkParentNodeED = 
        (NiBinaryExtraData*) m_spTempNode->GetExtraData(
        PSYSPARENTNODE_STRING);
    NIASSERT(pkParentNodeED);
    if (pkParentNodeED)
    {
        unsigned int uiSize = 0;
        char* pcBinaryInterp = NULL;
        pkParentNodeED->GetValue(uiSize, pcBinaryInterp);
        if (pcBinaryInterp == NULL || uiSize < sizeof(INode*))
        {
            pkDesc->m_pkPSysRoot = NULL;
        }
        else
        {
            unsigned long* pkPointer = (unsigned long*) pcBinaryInterp;
            pkDesc->m_pkPSysRoot = (INode*) pkPointer[0];
        }
    }
    else
    {
        pkDesc->m_pkPSysRoot = NULL;
    }

    NiFloatExtraData* pkBoundsRadiusED = 
        (NiFloatExtraData*) m_spTempNode->GetExtraData(
        PSYSBOUNDRADIUS_STRING);
    NIASSERT(pkBoundsRadiusED);
    if (pkBoundsRadiusED)
        pkDesc->m_fBoundRadius = pkBoundsRadiusED->GetValue();
    else
        pkDesc->m_fBoundRadius = 1.0f;

    NiFloatExtraData* pkRotationSpeedED = 
        (NiFloatExtraData*) 
        m_spTempNode->GetExtraData(PSYSROTATIONSPEED_STRING);
    NIASSERT(pkRotationSpeedED);
    if (pkRotationSpeedED)
    {
        pkDesc->m_fNiPSysRotationSpeed = 
            ((float)pkRotationSpeedED->GetValue());
    }
    else
    {
        pkDesc->m_fNiPSysRotationSpeed = 0;
    }

    NiFloatExtraData* pkRotationVariationED = 
        (NiFloatExtraData*) 
        m_spTempNode->GetExtraData(PSYSROTATIONVARIATION_STRING);
    NIASSERT(pkRotationVariationED);
    if (pkRotationVariationED)
    {
        pkDesc->m_fNiPSysRotationVariation = 
            ((float)pkRotationVariationED->GetValue());
    }
    else
    {
        pkDesc->m_fNiPSysRotationVariation = 0;
    }

    NiBooleanExtraData* pkBidirectionalED = 
        (NiBooleanExtraData*) m_spTempNode->GetExtraData(
        PSYSBIDIRECTIONALROTATION_STRING);
    NIASSERT(pkBidirectionalED);
    if (pkBidirectionalED)
        pkDesc->m_bNiPSysBidirectionalRotation = pkBidirectionalED->GetValue();
    else
        pkDesc->m_bNiPSysBidirectionalRotation = false;
    
    // Now we loop through the object's time controllers to find ones for
    // the animatable extra data
    NiTimeController* pkController = m_spTempNode->GetControllers();
    while (pkController != NULL)
    {
        if (NiIsKindOf(NiFloatExtraDataController, pkController))
        {
            NiFloatExtraDataController* pkEDController = 
                (NiFloatExtraDataController*) pkController;
            const char* pcExtraDataName = pkEDController->GetExtraDataName();
            if (strcmp(pcExtraDataName, PSYSROTATIONSPEED_STRING) == 0)
            {
                pkDesc->m_spRotationSpeedInterp = NiDynamicCast(
                    NiFloatInterpolator, pkEDController->GetInterpolator(0));
                pkDesc->m_eRotationSpeedInterpCycle = 
                    pkEDController->GetCycleType();
            }
            else if (strcmp(pcExtraDataName, PSYSROTATIONVARIATION_STRING) 
                == 0)
            {
                pkDesc->m_spRotationVariationInterp = NiDynamicCast(
                    NiFloatInterpolator, pkEDController->GetInterpolator(0));
                pkDesc->m_eRotationVariationInterpCycle = 
                    pkEDController->GetCycleType();
            }
        }
        pkController = pkController->GetNext();
    }

    return(W3D_STAT_OK);
}
//---------------------------------------------------------------------------
NiNode* GetRoot(NiAVObject* pkObject)
{
    NIASSERT(pkObject);
    while (pkObject->GetParent() != NULL)
    {
        pkObject = pkObject->GetParent();
    }
    if (NiIsKindOf(NiNode, pkObject))
        return (NiNode*)pkObject;
    else
        return NULL;
}
//---------------------------------------------------------------------------
NiParticleSystem* NiMAXPSysConverter::CreatePSys(NiPSysDesc* pkDesc, 
    SimpleParticle* pkPart, float fParentScale, NiNode* pkParent)
{
    CHECK_MEMORY();
    NiParticleSystem* pkPSystem = NULL;
    // to create the particle system, we need to know 4 things: 
    // how many total particles it has, if it needs a color array, 
    // if it needs rotation angles, and if it is in world space
    unsigned short usNumParticles = pkDesc->m_usNiPSysMaxAlloc;
    bool bUsesColor = pkDesc->m_bColorAges;
    bool bWorldSpace = !pkDesc->m_bLocalCoords;
    bool bUsesRotation = false;
    if (pkDesc->m_fNiPSysRotationSpeed != 0.0f || 
        pkDesc->m_fNiPSysRotationVariation != 0.0f ||
        pkDesc->m_fSpinPhase != 0.0f ||
        pkDesc->m_fSpinPhaseVar != 0.0f  ||
        pkDesc->m_spRotationSpeedInterp ||
        pkDesc->m_spRotationVariationInterp ||
        pkDesc->m_spSpinPhaseInterp ||
        pkDesc->m_spSpinPhaseVarInterp)
    {
        bUsesRotation = true;
    }

    if (pkDesc->m_usNumMeshes != 0)
    {
        pkPSystem = (NiParticleSystem*) NiNew NiMeshParticleSystem(
            usNumParticles, bUsesColor, bUsesRotation, bWorldSpace);
    }
    else
    {
        pkPSystem = (NiParticleSystem*) NiNew NiParticleSystem(
            usNumParticles, bUsesColor, bUsesRotation, bWorldSpace);
    }

    NiPSysEmitter* pkEmitter = BuildEmitterInfo(pkDesc, pkPSystem);
    NiPSysSpawnModifier* pkSpawnModifier = BuildSpawnInfo(pkDesc, pkPSystem);
    BuildAgeBasedDeathInfo(pkDesc, pkPSystem, pkSpawnModifier);
    BuildGrowFadeInfo(pkDesc, pkPSystem, pkSpawnModifier);
    BuildColorModInfo(pkDesc, pkPSystem, pkSpawnModifier);
    BuildMeshModInfo(pkDesc, pkPSystem, pkSpawnModifier);
    BuildRotationInfo(pkDesc, pkPSystem, pkSpawnModifier);
   
    float fLifetime = (pkDesc->m_fEmitStop - pkDesc->m_fEmitStart);

    NiParticles* pkParticles = pkPSystem;
    if (pkDesc->m_usNumMeshes != 0)
    {
        // NiParticleMeshes
        NiMeshPSysData* pkParticlesData = NiNew NiMeshPSysData(
            usNumParticles, bUsesColor, bUsesRotation);

        if (pkDesc->m_usNumFrags)
        {
            // PArray Fragments
            NIASSERT(usNumParticles == pkDesc->m_usNumFrags);

            for (unsigned int i = 0; i < usNumParticles; i++)
            {
                pkParticlesData->SetMeshAt(i, pkDesc->m_pspMeshes[i]);
            }

            unsigned int uiPoolSize = (unsigned int)(0.25f * usNumParticles);
            uiPoolSize = NiMax(uiPoolSize, 10);
            bool bFillPoolsOnLoad = false;
            pkParticlesData->SetDefaultPoolsSize(uiPoolSize);
            pkParticlesData->SetFillPoolsOnLoad(bFillPoolsOnLoad);
            pkParticlesData->SetActiveVertexCount(usNumParticles);
        }
        else
        {
            pkParticlesData->SetActiveVertexCount(0);
        }

        pkPSystem->SetModelData(pkParticlesData);
    }
    else 
    {
        // NiAutoNormalParticles
        NiPSysData* pkParticlesData = NiNew NiPSysData(usNumParticles, 
            bUsesColor, bUsesRotation);

        pkParticlesData->SetActiveVertexCount(0);
        pkPSystem->SetModelData(pkParticlesData);
    }

    pkParticles->SetName(pkPart->GetObjectName());

    // We want the particle system to be updated
    NiPSysUpdateCtlr* pkPSysUpdater = NiNew NiPSysUpdateCtlr;
    pkPSysUpdater->SetTarget(pkPSystem);

    char acName[256];
    NiSprintf(acName, 256, "NiPSysPositionModifier:%d", ms_uiTotalCount);
    
    NiPSysPositionModifier* pkPosUpdater = 
        NiNew NiPSysPositionModifier(acName);
    pkPSystem->AddModifier(pkPosUpdater);
    ms_uiTotalCount++;

    fParentScale = fabs(fParentScale);
    pkPSystem->SetScale(fParentScale);
    pkPSystem->SetWorldSpace(bWorldSpace);

    if (pkDesc->m_iBoundsUpdateType == 1)
    {
        NiSprintf(acName, 256, "NiPSysBoundUpdateModifier:%d",
            ms_uiTotalCount);
        
        NiPSysBoundUpdateModifier* pkBoundUpdater = 
            NiNew NiPSysBoundUpdateModifier(acName);
        pkBoundUpdater->SetUpdateSkip((unsigned short) 
            pkDesc->m_iBoundsDynamicSkip);
        pkPSystem->AddModifier(pkBoundUpdater);
        ms_uiTotalCount++;
    }
    else if (pkDesc->m_iBoundsUpdateType == 2)
    {
        NiBound kStaticBound;
        NiPoint3 kBoundPosition = pkDesc->m_kBoundPosition;
        float fBoundRadius = pkDesc->m_fBoundRadius;
        kStaticBound.SetCenterAndRadius(kBoundPosition, fBoundRadius);
        ParticleStaticBoundInfo* pkParticleStaticBoundInfo = 
            NiNew ParticleStaticBoundInfo;
        pkParticleStaticBoundInfo->m_spSystem = pkPSystem;
        pkParticleStaticBoundInfo->m_kBound = kStaticBound;
        pkParticleStaticBoundInfo->m_spOldParent = pkParent;
        ms_kStaticBoundList.AddHead(pkParticleStaticBoundInfo);
    }


    if (pkDesc->m_bNiPSysResetOnLoop)
    {
        NiPSysResetOnLoopCtlr* pkLoopResetCtrl = NiNew NiPSysResetOnLoopCtlr;
        pkLoopResetCtrl->SetBeginKeyTime(pkDesc->m_fNiPSysLoopStart);
        pkLoopResetCtrl->SetEndKeyTime(pkDesc->m_fNiPSysLoopEnd);
        pkLoopResetCtrl->SetTarget(pkPSystem);
    }
    
    return(pkPSystem);
}
//---------------------------------------------------------------------------
int NiMAXPSysConverter::ExtractPlanarDeflector(Interface* pkIntf, 
    Modifier* pkMod, INode* pkMaxNode, NiParticleSystem* pkPSystem, 
    NiPSysDesc* pkDesc, ScaleInfo* pkParentScale)
{
    DeflectMod* pkDef = (DeflectMod*)pkMod;
                    
    if (pkDesc->m_spColliderManager == NULL)
    {
        char acName[256];
        NiSprintf(acName, 256, "NiPSysColliderManagerModifier:%d",
            ms_uiTotalCount);
    
        pkDesc->m_spColliderManager = NiNew 
            NiPSysColliderManager(acName);
        pkPSystem->AddModifier(pkDesc->m_spColliderManager);
        ms_uiTotalCount++;
    }
    
    NiPSysColliderManagerPtr spCollideManager =  pkDesc->m_spColliderManager;

    float fBounce, fWidth, fHeight;
    IParamBlock2* pkPBlock2 = pkDef->deflect.obj->pblock2;
    pkPBlock2->GetValue(deflect_bounce,m_kAnimStart,fBounce,FOREVER);
    pkPBlock2->GetValue(deflect_width,m_kAnimStart,fWidth,FOREVER);
    pkPBlock2->GetValue(deflect_height,m_kAnimStart,fHeight,FOREVER);

    Matrix3 kDeflectXform =
        pkDef->deflect.node->GetNodeTM(m_kAnimStart);
    Matrix3 kParticleXform = 
        pkMaxNode->GetNodeTM(m_kAnimStart);
    Matrix3 kDef2PartXform = 
        kDeflectXform*Inverse(kParticleXform);

    Point3 kNorm;
    kNorm.x = 0.0f;
    kNorm.y = 0.0f;
    kNorm.z = 1.0f;
    kNorm = VectorTransform(kDef2PartXform, kNorm);
    kNorm = Normalize(kNorm);

    Point3 kX;
    kX.x = fWidth;
    kX.y = 0.0f;
    kX.z = 0.0f;
    kX = VectorTransform(kDef2PartXform, kX);
    fWidth = Length(kX);
    kX = Normalize(kX);

    Point3 kY;
    kY.x = 0.0f;
    kY.y = fHeight;
    kY.z = 0.0f;
    kY = VectorTransform(kDef2PartXform, kY);
    fHeight = Length(kY);
    kY = Normalize(kY);
        
    Point3 kPos = kDef2PartXform.GetTrans();

    float fD = DotProd(kNorm, kDef2PartXform.GetTrans());
    NiPoint3 kNiNorm;
    kNiNorm.x = kNorm.x;
    kNiNorm.y = kNorm.y;
    kNiNorm.z = kNorm.z;

    NiPSysPlanarCollider* pkColl = NiNew NiPSysPlanarCollider;

    ParticleLinkUpInfo* pkParticleLinkUpInfo = NiNew ParticleLinkUpInfo;
    if (pkParticleLinkUpInfo)
    {
        pkParticleLinkUpInfo->m_pkMAXNode = pkDef->deflect.node;
        pkParticleLinkUpInfo->m_spSystem = pkPSystem;
        pkParticleLinkUpInfo->m_spModifier = spCollideManager;
        pkParticleLinkUpInfo->m_spCollider = pkColl;
        ms_kLinkUpList.AddHead(pkParticleLinkUpInfo);
    }

    pkColl->SetXAxis(NiPoint3::UNIT_X);
    pkColl->SetYAxis(NiPoint3::UNIT_Y);
    pkColl->SetWidth(fWidth);
    pkColl->SetHeight(fHeight);

    pkColl->SetBounce(fBounce);
    pkColl->SetSpawnOnCollide(pkDesc->m_bSpawnOnCollide);
    pkColl->SetDieOnCollide(pkDesc->m_bDieOnCollide);

    pkColl->SetSpawnModifier(pkDesc->m_pkSpawnModifier);
    spCollideManager->AddCollider(pkColl);

    return(W3D_STAT_OK);
}
//---------------------------------------------------------------------------
int NiMAXPSysConverter::ExtractSphereDeflector(Interface* pkIntf, 
    Modifier* pkMod, INode* pkMaxNode, NiParticleSystem* pkPSystem, 
    NiPSysDesc* pkDesc, ScaleInfo* pkParentScale)
{
    SphereDeflectMod* pkSphDef = (SphereDeflectMod*) pkMod;
    if (pkDesc->m_spColliderManager == NULL)
    {
        char acName[256];
        NiSprintf(acName, 256, "NiPSysColliderManagerModifier:%d",
            ms_uiTotalCount);
    
        pkDesc->m_spColliderManager = NiNew NiPSysColliderManager(acName);
        pkPSystem->AddModifier(pkDesc->m_spColliderManager);
        ms_uiTotalCount++;

    }
    
    NiPSysColliderManagerPtr spCollideManager =  pkDesc->m_spColliderManager;

    
    float fBounce, fChaos, fDiameter, fBounceVar, fVInher;

    IParamBlock2* pkPBlock2 = pkSphDef->deflect.obj->pblock2;
    pkPBlock2->GetValue(sdeflectrobj_bounce, m_kAnimStart, fBounce,
        FOREVER);
    pkPBlock2->GetValue(sdeflectrobj_bouncevar, m_kAnimStart, fBounceVar,
        FOREVER);
    pkPBlock2->GetValue(sdeflectrobj_chaos, m_kAnimStart, fChaos,
        FOREVER);
    pkPBlock2->GetValue(sdeflectrobj_radius, m_kAnimStart, fDiameter,
        FOREVER);
    pkPBlock2->GetValue(sdeflectrobj_velocity, m_kAnimStart, fVInher,
        FOREVER);

    Matrix3 kDeflectXform = 
        pkSphDef->deflect.node->GetNodeTM(m_kAnimStart);
    Matrix3 kParticleXform = 
        pkMaxNode->GetNodeTM(m_kAnimStart);

    Point3 kPos = kDeflectXform.GetTrans() - 
        kParticleXform.GetTrans();

    NiPSysSphericalCollider* pkColl = NiNew NiPSysSphericalCollider;

    ParticleLinkUpInfo* pkParticleLinkUpInfo = NiNew ParticleLinkUpInfo;
    if (pkParticleLinkUpInfo)
    {
        pkParticleLinkUpInfo->m_pkMAXNode = pkSphDef->deflect.node;
        pkParticleLinkUpInfo->m_spSystem = pkPSystem;
        pkParticleLinkUpInfo->m_spModifier = spCollideManager;
        pkParticleLinkUpInfo->m_spCollider = pkColl;
        ms_kLinkUpList.AddHead(pkParticleLinkUpInfo);
    }

    pkColl->SetRadius(fDiameter * 0.5f);

    pkColl->SetBounce(fBounce);
    pkColl->SetSpawnOnCollide(pkDesc->m_bSpawnOnCollide);
    pkColl->SetDieOnCollide(pkDesc->m_bDieOnCollide);

    pkColl->SetSpawnModifier(pkDesc->m_pkSpawnModifier);
    spCollideManager->AddCollider(pkColl);

    return(W3D_STAT_OK);
}

//---------------------------------------------------------------------------
int NiMAXPSysConverter::ExtractGravity(Interface* pkIntf, Modifier* pkMod, 
    INode* pkMaxNode, NiParticleSystem* pkPSystem, NiPSysDesc* pkDesc, 
    ScaleInfo* pkParentScale)
{
    GravityMod* pkGrav = (GravityMod*) pkMod;
    
    float fStrength, fDecay, fDispLength;
    int iType;

    IParamBlock2* pkPBlock2 = pkGrav->force.obj->pblock2;
    pkPBlock2->GetValue(PB_GRAV_STRENGTH, m_kAnimStart, fStrength,
        FOREVER);
    pkPBlock2->GetValue(PB_GRAV_DECAY, m_kAnimStart, fDecay,
        FOREVER);
    pkPBlock2->GetValue(PB_GRAV_TYPE, m_kAnimStart, iType, FOREVER);
    pkPBlock2->GetValue(PB_GRAV_DISPLENGTH, m_kAnimStart, 
        fDispLength, FOREVER);

    // Here we need to get the object transform of the gravity relative to
    // it's pivot.  If the orientation of the pivot has been altered, we
    // must reflect that change in our gravity's axis
    Quat kQuat = pkGrav->force.node->GetObjOffsetRot();
    Matrix3 kObjectTransform(1);
    PreRotateMatrix(kObjectTransform, kQuat);
    Point3 kMaxAxis;
    kMaxAxis.x = 0.0f;
    kMaxAxis.y = 0.0f; 
    kMaxAxis.z = 1.0f;
    kMaxAxis = VectorTransform(kObjectTransform, kMaxAxis);
    kMaxAxis = Normalize(kMaxAxis);

    NiPoint3 kAxis = NiPoint3(kMaxAxis.x, kMaxAxis.y, kMaxAxis.z);
    
    NiAVObject* pkGravityObj = NULL;
    char acName[256];
        NiSprintf(acName, 256, "NiPSysGravityModifier:%d", ms_uiTotalCount);
    NiPSysGravityModifier* pkNIGrav = 
        NiNew NiPSysGravityModifier(acName, pkGravityObj);

    if (iType == 0)
        pkNIGrav->SetType(NiPSysGravityModifier::FORCE_PLANAR);
    else
        pkNIGrav->SetType(NiPSysGravityModifier::FORCE_SPHERICAL);

    ParticleLinkUpInfo* pkParticleLinkUpInfo = NiNew ParticleLinkUpInfo;
    if (pkParticleLinkUpInfo)
    {
        pkParticleLinkUpInfo->m_pkMAXNode = pkGrav->force.node;
        pkParticleLinkUpInfo->m_spSystem = pkPSystem;
        pkParticleLinkUpInfo->m_spModifier = pkNIGrav;
        ms_kLinkUpList.AddHead(pkParticleLinkUpInfo);
    }

    pkNIGrav->SetDecay(fDecay);

    float fModifier = 1.0f;
    // Hack to account for behavior differences between
    //   spray/snow and other particle systems
    float fGravity = fStrength * 
        GetFrameRate() * GetFrameRate();
    Class_ID eID = pkMaxNode->GetObjectRef()->ClassID();
    if (eID == Class_ID(RAIN_CLASS_ID, 0) ||
        eID == Class_ID(SNOW_CLASS_ID, 0))
    {
        fModifier = 0.1f;
        fGravity *= 0.1f;
    }

    pkNIGrav->SetStrength(fGravity);
    pkNIGrav->SetGravityAxis(kAxis);

    pkPSystem->AddModifier(pkNIGrav);
    ms_uiTotalCount++;

    NiTimeController::CycleType eCycleType = NiTimeController::CLAMP;
    NiFloatInterpolatorPtr spStrengthInterp = BuildFloatInterp(pkPBlock2, 
        PB_GRAV_STRENGTH, eCycleType, GetFrameRate() * GetFrameRate() * 
        fModifier);

    if (spStrengthInterp)
    {
        NiPSysGravityStrengthCtlr* pkGravStrengthController = 
            NiNew NiPSysGravityStrengthCtlr(pkNIGrav->GetName());
        pkGravStrengthController->SetInterpolator(spStrengthInterp);
        pkGravStrengthController->ResetTimeExtrema();
        pkGravStrengthController->SetCycleType(eCycleType);
        pkGravStrengthController->SetTarget(pkPSystem);
    }
    return(W3D_STAT_OK);
}
//---------------------------------------------------------------------------
int NiMAXPSysConverter::ExtractDrag(Interface* pkIntf, Modifier* pkMod, 
    INode* pkMaxNode, NiParticleSystem* pkPSystem, NiPSysDesc* pkDesc, 
    ScaleInfo* pkParentScale)
{
    SimpleWSMMod* pkDrag = (SimpleWSMMod*) pkMod;
    float fTimeOn;
    float fTimeOff;
    int iType;

    int X = 0;
    int Y = 1;
    int Z = 2;

    float fDamping[3];
    float fRange[3];
    float fFalloff[3];

    bool bUnlimitedRange;

    bool bFound = false;
    if (pkDrag->nodeRef)
    {
        TSTR kName = pkDrag->nodeRef->GetName();
        for (unsigned int ui = 0; ui < (unsigned int )
            pkDrag->nodeRef->NumSubs(); ui++)
        {
            Animatable* pkAnim = pkDrag->nodeRef->SubAnim(ui);
            NiString strName = pkDrag->nodeRef->SubAnimName(ui);

            if (strName == "Object (Drag)")
            {
                bFound = true;
                for (unsigned int uj = 0; uj < (unsigned int)
                    pkAnim->NumParamBlocks(); uj++)
                {
                    IParamBlock2* pkParams = pkAnim->GetParamBlock(uj);
                    if (!pkParams)
                        continue;

                    for (unsigned int up = 0; up < 
                        (unsigned int) pkParams->NumParams(); up++)
                    {
                        NiString strName = pkParams->GetLocalName(up);
                        ParamType2 eType = pkParams->GetParameterType(up);
                        Interval kInt = FOREVER;

                        if (strName == "Time On")
                        {
                            int iVal;
                            pkParams->GetValue(up, 0, iVal, kInt);
                            fTimeOn = iVal  * sfTimeFactor;
                        }
                        else if (strName == "Time Off")
                        {
                            int iVal;
                            pkParams->GetValue(up, 0, iVal, kInt);
                            fTimeOff = iVal * sfTimeFactor;
                        }
                        else if (strName == "Damping Symmetry")
                        {
                            int iVal;
                            pkParams->GetValue(up, 0, iVal, kInt);
                            iType = iVal;
                        }
                        else if (strName == "X Damping")
                        {
                            pkParams->GetValue(up, 0, fDamping[X], kInt); 
                        }
                        else if (strName == "X Range")
                        {
                            pkParams->GetValue(up, 0, fRange[X], kInt); 
                        }
                        else if (strName == "X Falloff")
                        {
                            pkParams->GetValue(up, 0, fFalloff[X], kInt); 
                        }
                        else if (strName == "Y Damping")
                        {
                            pkParams->GetValue(up, 0, fDamping[Y], kInt); 
                        }
                        else if (strName == "Y Range")
                        {
                            pkParams->GetValue(up, 0, fRange[Y], kInt); 
                        }
                        else if (strName == "Y Falloff")
                        {
                            pkParams->GetValue(up, 0, fFalloff[Y], kInt); 
                        }
                        else if (strName == "Z Damping")
                        {
                            pkParams->GetValue(up, 0, fDamping[Z], kInt); 
                        }
                        else if (strName == "Z Range")
                        {
                            pkParams->GetValue(up, 0, fRange[Z], kInt); 
                        }
                        else if (strName == "Z Falloff")
                        {
                            pkParams->GetValue(up, 0, fFalloff[Z], kInt); 
                        }
                        else if (strName == "Unlimited Range")
                        {
                            int iVal;
                            pkParams->GetValue(up, 0, iVal, kInt);
                            bUnlimitedRange = iVal ? true : false;
                        }
                    }
                }

            }
        }
    }
    
    if (bFound && iType == 0)
    {
        for (unsigned int ui = 0; ui < 3; ui++)
        {
            unsigned int uiNumKeys = 0;
            NiStepBoolKey* pkBoolKeys = BuildActiveKeys(
                pkDesc->m_fNiPSysLoopStart, pkDesc->m_fNiPSysLoopEnd, 
                fTimeOn, fTimeOff, uiNumKeys);

            NiAVObject* pkDragObj = NULL;
            char acAxisName[256];
            NiPoint3 kWhichAxis;
            // Do all the iteration-dependant assignment here
            switch (ui)
            {
            case 0:
                NiSprintf(acAxisName, 256, "X");
                kWhichAxis = NiPoint3::UNIT_X;
                break;
            case 1:
                NiSprintf(acAxisName, 256, "Y");
                kWhichAxis = NiPoint3::UNIT_Y;
                break;
            case 2:
                NiSprintf(acAxisName, 256, "Z");
                kWhichAxis = NiPoint3::UNIT_Z;
                break;
            }

            if (fDamping[ui] > 0.0f)
            {
                char acName[256];
                NiSprintf(acName, 256, "NiPSysDragModifier(%s-Axis):%d", 
                    acAxisName, ms_uiTotalCount);

                NiPSysDragModifierPtr spNIDrag = 
                    NiNew NiPSysDragModifier(acName, pkDragObj);

                ParticleLinkUpInfo* pkParticleLinkUpInfo = 
                    NiNew ParticleLinkUpInfo;
                if (pkParticleLinkUpInfo)
                {
                    pkParticleLinkUpInfo->m_pkMAXNode = pkDrag->nodeRef;
                    pkParticleLinkUpInfo->m_spSystem = pkPSystem;
                    pkParticleLinkUpInfo->m_spModifier = spNIDrag;
                    ms_kLinkUpList.AddHead(pkParticleLinkUpInfo);
                }

                spNIDrag->SetPercentage(fDamping[ui]);
                spNIDrag->SetDragAxis(kWhichAxis);

                if (!bUnlimitedRange)
                {
                    spNIDrag->SetRange(fRange[ui]);
                    spNIDrag->SetRangeFalloff(fFalloff[ui]);
                }
                else
                {
                    spNIDrag->SetRange(NI_INFINITY);
                    spNIDrag->SetRangeFalloff(NI_INFINITY);
                }

                pkPSystem->AddModifier(spNIDrag);
                NiBoolData* pkBoolData = NiNew NiBoolData();
                pkBoolData->ReplaceAnim(pkBoolKeys, uiNumKeys, 
                    NiAnimationKey::STEPKEY);

                NiPSysModifierActiveCtlr* pkCtrl = NiNew 
                    NiPSysModifierActiveCtlr(acName);
                NiBoolTimelineInterpolator* pkInterp = NiNew 
                    NiBoolTimelineInterpolator(pkBoolData);

                pkCtrl->SetInterpolator(pkInterp);
                pkCtrl->ResetTimeExtrema();
                if (pkDesc->m_bNiPSysLoop)
                    pkCtrl->SetCycleType(NiTimeController::LOOP);
                else
                    pkCtrl->SetCycleType(NiTimeController::CLAMP);
                pkCtrl->SetTarget(pkPSystem);
                ms_uiTotalCount++;
            }
            else // if damping is zero, clean up memory
            {
                if (pkBoolKeys)
                    NiDelete[] pkBoolKeys;
            }
        }

    }
    else
    {
        NILOGWARNING("Warning: Gamebryo only supports Linear Drag.\n");
    }

    return(W3D_STAT_OK);
}
//---------------------------------------------------------------------------

int NiMAXPSysConverter::ExtractWind(Interface* pkIntf, Modifier* pkMod, 
    INode* pkMaxNode, NiParticleSystem* pkPSystem, NiPSysDesc* pkDesc, 
    ScaleInfo* pkParentScale)
{
    WindMod* pkWind = (WindMod*) pkMod;
    
    float fStrength, fDecay, fDispLength, fTurb, fScale;
    int iType;

    IParamBlock2* pkPBlock2 = pkWind->force.obj->pblock2;
    pkPBlock2->GetValue(PB_WIND_STRENGTH, m_kAnimStart, fStrength,
        FOREVER);
    pkPBlock2->GetValue(PB_WIND_DECAY, m_kAnimStart, fDecay,
        FOREVER);
    pkPBlock2->GetValue(PB_WIND_TYPE, m_kAnimStart, iType, FOREVER);
    pkPBlock2->GetValue(PB_WIND_DISPLENGTH, m_kAnimStart, 
        fDispLength, FOREVER);
    pkPBlock2->GetValue(PB_WIND_TURBULENCE, m_kAnimStart, fTurb, FOREVER);
    pkPBlock2->GetValue(PB_WIND_SCALE, m_kAnimStart, fScale, FOREVER);

    Matrix3 kWindXform = 
        pkWind->force.node->GetNodeTM(m_kAnimStart);
    Matrix3 kParticleXform = 
        pkMaxNode->GetNodeTM(m_kAnimStart);
    Matrix3 kWind2PartXform = 
        kWindXform*Inverse(kParticleXform);
    
    Point3 kPos = kWind2PartXform.GetTrans();

    Point3 kNorm;
    kNorm.x = 0.0f;
    kNorm.y = 0.0f;
    kNorm.z = 1.0f;
    kNorm = VectorTransform(kWind2PartXform, kNorm);
    kNorm = Normalize(kNorm);
    
    NiAVObject* pkGravityObj = NULL;
    char acName[256];
    NiSprintf(acName, 256, "NiPSysGravityModifier:%d",
            ms_uiTotalCount);
    NiPSysGravityModifier* pkNIGravity = 
        NiNew NiPSysGravityModifier(acName, pkGravityObj);

    if (iType == 0)
        pkNIGravity->SetType(NiPSysGravityModifier::FORCE_PLANAR);
    else
        pkNIGravity->SetType(NiPSysGravityModifier::FORCE_SPHERICAL);

    ParticleLinkUpInfo* pkParticleLinkUpInfo = NiNew ParticleLinkUpInfo;
    if (pkParticleLinkUpInfo)
    {
        pkParticleLinkUpInfo->m_pkMAXNode = pkWind->force.node;
        pkParticleLinkUpInfo->m_spSystem = pkPSystem;
        pkParticleLinkUpInfo->m_spModifier = pkNIGravity;
        ms_kLinkUpList.AddHead(pkParticleLinkUpInfo);
    }

    pkNIGravity->SetDecay(fDecay);

    // Hack to account for behavior differences between
    //   spray/snow and other particle systems
    float fModifier = 1.0f;
    float fGravity = fStrength * 
        GetFrameRate() * GetFrameRate();
    Class_ID eID = pkMaxNode->GetObjectRef()->ClassID();
    if (eID == Class_ID(RAIN_CLASS_ID, 0) ||
        eID == Class_ID(SNOW_CLASS_ID, 0))
    {
        fModifier = 0.1f;
        fGravity *= 0.1f;
        fTurb *= 0.01f;
    }

    pkNIGravity->SetStrength(fGravity);
    pkNIGravity->SetGravityAxis(NiPoint3(0.0f, 0.0f, 1.0f));
    pkNIGravity->SetTurbulence(fTurb);
    pkNIGravity->SetTurbulenceScale(fScale);

    pkPSystem->AddModifier(pkNIGravity);
    ms_uiTotalCount++;

    NiTimeController::CycleType eCycleType = NiTimeController::CLAMP;
    NiFloatInterpolatorPtr spStrengthInterp = BuildFloatInterp(pkPBlock2,
        PB_WIND_STRENGTH, eCycleType, GetFrameRate() * GetFrameRate() * 
        fModifier);

    
    if (spStrengthInterp)
    {
        NiPSysGravityStrengthCtlr* pkGravStrengthController = 
            NiNew NiPSysGravityStrengthCtlr(pkNIGravity->GetName());
        pkGravStrengthController->SetInterpolator(spStrengthInterp);
        pkGravStrengthController->ResetTimeExtrema();
        pkGravStrengthController->SetCycleType(eCycleType);
        pkGravStrengthController->SetTarget(pkPSystem);
    }
    return(W3D_STAT_OK);
}
//---------------------------------------------------------------------------
int NiMAXPSysConverter::ExtractParticleBomb(Interface* pkIntf, 
    Modifier* pkMod, INode* pkMaxNode, NiParticleSystem* pkPSystem, 
    NiPSysDesc* pkDesc, ScaleInfo* pkParentScale)
{
    PBombMod* pkPBomb = (PBombMod*) pkMod;
    IParamBlock* pkPBlock = pkPBomb->force.obj->pblock;
    
    TimeValue kStart, kDuration;
    int iDecayType, iSymm;
    float fDecay, fChaos, fDeltaV;
    
    pkPBlock->GetValue(PBOMB_STARTTIME, m_kAnimStart, kStart,
        FOREVER);
    pkPBlock->GetValue(PBOMB_LASTSFOR, m_kAnimStart, kDuration,
        FOREVER);
    pkPBlock->GetValue(PBOMB_DECAY, m_kAnimStart, fDecay,
        FOREVER);
    pkPBlock->GetValue(PBOMB_DECAYTYPE, m_kAnimStart, 
        iDecayType, FOREVER);
    pkPBlock->GetValue(PBOMB_DELTA_V, m_kAnimStart, fDeltaV,
        FOREVER);
    pkPBlock->GetValue(PBOMB_CHAOS, m_kAnimStart, fChaos,
        FOREVER);
    pkPBlock->GetValue(PBOMB_SYMMETRY, m_kAnimStart, iSymm,
        FOREVER);
    
    // Here we need to get the object transform of the bomb relative to
    // it's pivot.  If the orientation of the pivot has been altered, we
    // must reflect that change in our bomb's axis
    Quat kQuat = pkPBomb->force.node->GetObjOffsetRot();
    Matrix3 kObjectTransform(1);
    PreRotateMatrix(kObjectTransform, kQuat);
    Point3 kMaxAxis;
    kMaxAxis.x = 0.0f;
    kMaxAxis.y = 0.0f; 
    kMaxAxis.z = 1.0f;
    kMaxAxis = VectorTransform(kObjectTransform, kMaxAxis);
    kMaxAxis = Normalize(kMaxAxis);

    NiPoint3 kAxis = NiPoint3(kMaxAxis.x, kMaxAxis.y, kMaxAxis.z);

    char acName[256];
    NiSprintf(acName, 256, "NiPSysBombModifier:%d", ms_uiTotalCount);
    NiPSysBombModifier* pkPBombModifier = NiNew NiPSysBombModifier(acName);
    ParticleLinkUpInfo* pkParticleLinkUpInfo = NiNew ParticleLinkUpInfo;
    if (pkParticleLinkUpInfo)
    {
        pkParticleLinkUpInfo->m_pkMAXNode = pkPBomb->force.node;
        pkParticleLinkUpInfo->m_spSystem = pkPSystem;
        pkParticleLinkUpInfo->m_spModifier = pkPBombModifier;
        ms_kLinkUpList.AddHead(pkParticleLinkUpInfo);
    }

    // our DeltaV value is a modification of Max's.
    // we multiply their strength value (deltaV) by the framerate squared, 
    // which effectively divides by delta time, putting it in units per 
    // second squared. We multiply by 10 because that is a magic number 
    // that shows up in the max sdk.
    pkPBombModifier->SetDeltaV(fDeltaV * GetFrameRate() * 
        GetFrameRate() * 10.0f);
   
    pkPBombModifier->SetDecay(fDecay);
    pkPBombModifier->SetDecayType((
        NiPSysBombModifier::DecayType)iDecayType);
    pkPBombModifier->SetSymmType((NiPSysBombModifier::SymmType)iSymm);
    
    pkPBombModifier->SetBombAxis(kAxis);

    pkPSystem->AddModifier(pkPBombModifier);
    ms_uiTotalCount++;

    // Create the modifier active controller
    unsigned int uiNumKeys = 0;
    NiStepBoolKey* pkBoolKeys = BuildActiveKeys(pkDesc->m_fNiPSysLoopStart, 
        pkDesc->m_fNiPSysLoopEnd, kStart * sfTimeFactor, (kStart + kDuration)
        * sfTimeFactor, uiNumKeys);  
    NiBoolData* pkBoolData = NiNew NiBoolData();
    pkBoolData->ReplaceAnim(pkBoolKeys, uiNumKeys, NiAnimationKey::STEPKEY);
    NiPSysModifierActiveCtlr* pkCtrl = NiNew NiPSysModifierActiveCtlr(
        acName);
    NiBoolTimelineInterpolator* pkInterp = 
        NiNew NiBoolTimelineInterpolator(pkBoolData);
    pkCtrl->SetInterpolator(pkInterp);
    pkCtrl->ResetTimeExtrema();

    if (pkDesc->m_bNiPSysLoop)
        pkCtrl->SetCycleType(NiTimeController::LOOP);
    else
        pkCtrl->SetCycleType(NiTimeController::CLAMP);
    pkCtrl->SetTarget(pkPSystem);

    return(W3D_STAT_OK);
}
//---------------------------------------------------------------------------
int NiMAXPSysConverter::ExtractForces(Interface* pkIntf, INode* pkMaxNode, 
    NiParticleSystem* pkPSystem, NiPSysDesc* pkDesc, 
    ScaleInfo* pkParentScale)
{
    CHECK_MEMORY();
    if (!pkPSystem)
        return W3D_STAT_FAILED;

    for (int i = 0; i < pkMaxNode->NumRefs(); i++)
    {
        ReferenceTarget* pkRefTarg = pkMaxNode->GetReference(i);
        
        if (pkRefTarg && 
            pkRefTarg->ClassID() == Class_ID(WSM_DERIVOB_CLASS_ID, 0))
        {
            IDerivedObject* pkWSMDerObj = (IDerivedObject*)pkRefTarg;
            
            for (int j = 0; j < pkWSMDerObj->NumModifiers(); j++)
            {
                Modifier* pkMod = pkWSMDerObj->GetModifier(j);
                
                Class_ID eModID = pkMod->ClassID();
                
                if (eModID == Class_ID(DEFLECTMOD_CLASS_ID, 0))
                {
                    ExtractPlanarDeflector(pkIntf, pkMod, pkMaxNode, 
                        pkPSystem, pkDesc, pkParentScale);
                }
                else if (eModID == SPHEREDEFMOD_CLASS_ID)
                {
                    ExtractSphereDeflector(pkIntf, pkMod, pkMaxNode,
                        pkPSystem, pkDesc, pkParentScale);
                }
                else if (eModID == Class_ID(GRAVITYMOD_CLASS_ID, 0))
                {
                    ExtractGravity(pkIntf, pkMod, pkMaxNode,
                        pkPSystem, pkDesc, pkParentScale);
                }
                else if (eModID == Class_ID(WINDMOD_CLASS_ID, 0))
                {
                    ExtractWind(pkIntf, pkMod, pkMaxNode,
                        pkPSystem, pkDesc, pkParentScale);
                }
                else if (eModID == PBOMBMOD_CLASS_ID)
                {
                    ExtractParticleBomb(pkIntf, pkMod, pkMaxNode,
                        pkPSystem, pkDesc, pkParentScale);
                }
                else if (eModID == PDRAGMOD_CLASS_ID)
                {
                    ExtractDrag(pkIntf, pkMod, pkMaxNode,
                        pkPSystem, pkDesc, pkParentScale);
                }
            }
        }
    }
    CHECK_MEMORY();
    return(W3D_STAT_OK);
}
//---------------------------------------------------------------------------
int NiMAXPSysConverter::Convert(Interface* pkIntf, INode* pkMaxNode, 
    NiNode* pkParent, ScaleInfo* pkParentScale)
{
    ms_uiTotalCount = 0;
    CHECK_MEMORY();
    Object* pkObjRef = pkMaxNode->GetObjectRef();
    SimpleParticle* pkPart = (SimpleParticle*) pkObjRef;
    Class_ID eID = pkObjRef->ClassID();

    NiPSysDesc kDesc;

    ExtractCustomAttribs(&kDesc, pkPart);

    if (eID == SUPRSPRAY_CLASS_ID)
    {
        ExtractSuperSpray(pkIntf, pkPart, &kDesc, pkParentScale);
    }
    else if (eID == BLIZZARD_CLASS_ID)
    {
        ExtractBlizzard(pkIntf, pkPart, &kDesc, pkParentScale);
    }
    else if (eID == PArray_CLASS_ID)
    {
        ExtractPArray(pkMaxNode, pkParent, pkIntf, pkPart, &kDesc, 
            pkParentScale);
    }
    else if (eID == PCLOUD_CLASS_ID)
    {
        ExtractPCloud(pkMaxNode, pkIntf, pkPart, &kDesc, pkParentScale);
    }
    else if (eID == Class_ID(RAIN_CLASS_ID, 0) || 
        eID == Class_ID(SNOW_CLASS_ID, 0))
    {
        ExtractOldPSys(pkMaxNode, pkPart, &kDesc);
    }
    else
    {
        NILOGWARNING("We are being asked to convert an unknown "
            "particle system!\n");
        NIASSERT(0);
        return 0;
    }

    NiMAXMaterialConverter::GetPSysColorInfo(pkMaxNode->GetMtl(), 0, 
        kDesc.m_pkColors, kDesc.m_bColorAges, kDesc.m_uiNumColorKeys);

    float fScale = 1.0f;
    if (pkParentScale && !pkParentScale->bIsIdent)
        fScale = pkParentScale->scale.s.x;

    NiParticleSystem* pkPSys = CreatePSys(&kDesc, pkPart, fScale, pkParent);
    
    ExtractForces(pkIntf, pkMaxNode, pkPSys, &kDesc, pkParentScale);

    pkParent->AttachChild(pkPSys);
    pkParent->SetSelectiveUpdate(true);
    pkParent->SetSelectiveUpdatePropertyControllers(true);
    pkParent->SetSelectiveUpdateRigid(false);
    pkParent->SetSelectiveUpdateTransforms(true);

    if (kDesc.m_pkPSysRoot != NULL && kDesc.m_pkPSysRoot != 
        pkMaxNode && kDesc.m_bLocalCoords)
    {
        // We're actually using a different root than the emitter, so 
        // place it in the remap list 
        // for after all the nodes are converted
    
        ParticleRemapInfo* pkParticleRemapInfo = NiNew ParticleRemapInfo;
        pkParticleRemapInfo->m_pkMAXNode = kDesc.m_pkPSysRoot;
        pkParticleRemapInfo->m_spSystem = pkPSys;
        ms_kRemapList.AddHead(pkParticleRemapInfo);
    }
    else if (kDesc.m_bLocalCoords)
    {
        ParticleRemapInfo* pkParticleRemapInfo = NiNew ParticleRemapInfo;
        pkParticleRemapInfo->m_pkMAXNode = NULL;
        pkParticleRemapInfo->m_spSystem = pkPSys;
        ms_kRemapList.AddHead(pkParticleRemapInfo);
    }
    
    NiString strName = pkParent->GetName();
    pkPSys->SetName((const char*)strName);
    strName += "-Emitter";
    pkParent->SetName((const char*)strName);

    NiMAXUV* pkNiMAXUV = NULL;
    NiMAXMaterialConverter::GetUVInfo(pkMaxNode->GetMtl(), m_kAnimStart, 
        pkNiMAXUV, -1);

    // For materials applied to particle systems, we assign the first
    // converted MAX UV channel to NI channel 0.
    if (pkNiMAXUV->IsChannel(0))
        pkNiMAXUV->SetMAXtoNI(0, 0);

    NiMAXMaterialConverter kMtl(m_kAnimStart, m_kAnimEnd);
    NiNode* pkEffectNode = NULL;
    int iStatus = kMtl.Convert(pkMaxNode->GetMtl(), pkParent, pkPSys, 
        pkMaxNode, 0, pkNiMAXUV, pkEffectNode);
    NiDelete pkEffectNode;

    NiDelete pkNiMAXUV;
    if (iStatus)
        return(iStatus);

    // move pkParent visibility controller, if any, to pkParticles
    NiVisController* pkVC = NiGetController(NiVisController, pkParent);
    if (pkVC)
    {
        pkVC->SetTarget(pkPSys);
    }

    if (iStatus)
        return(iStatus);

    NiTimeController* pkController = pkPSys->GetControllers();
    bool bUseAppTime = NiMAXConverter::GetUseAppTime();
    while (pkController != NULL)
    {
        if (bUseAppTime)
            pkController->SetAnimType(NiTimeController::APP_TIME);
        else
            pkController->SetAnimType(NiTimeController::APP_INIT);
        pkController = pkController->GetNext();
    }

    CHECK_MEMORY();
    return(0);
}
//---------------------------------------------------------------------------
void NiMAXPSysConverter::PopulateMeshEmitter(NiAVObject* pkObj,
    NiPSysMeshEmitter* pkEmitter)
{
    if (!pkObj || !pkEmitter)
        return;

    NiNode* pkNode = NiDynamicCast(NiNode, pkObj);
    if (!pkNode)
    {
        // Only NiNodes should be passed into this function.
        NIASSERT(false);
        return;
    }

    unsigned int ui;
    for (ui = 0; ui < pkNode->GetArrayCount(); ui++)
    {
        NiAVObject* pkChild = pkNode->GetAt(ui);
        if (NiIsKindOf(NiGeometry, pkChild))
        {
            pkEmitter->AddGeometryEmitter((NiGeometry*) pkChild);

            // Make sure NiOptimize will not remove the emitter object.
            NiStringExtraData* pkStringED = NiNew NiStringExtraData(
                "NiOptimizeKeep\n");

            pkChild->AddExtraData(pkStringED);
        }
        else if (NiIsKindOf(NiNode, pkChild))
        {
            const char* pcName = pkChild->GetName();
            if (pcName != NULL && strcmp(pcName, gpcMultiMtlName) == 0)
            {
                PopulateMeshEmitter((NiNode*) pkChild, pkEmitter);
            }
        }
    }
}

//---------------------------------------------------------------------------
void NiMAXPSysConverter::Finish(NiNode* pkNode)
{
    CHECK_MEMORY();
    // Assign emitters
    if (!ms_kLinkUpList.IsEmpty())
    {
        NiTListIterator kIter = ms_kLinkUpList.GetHeadPos();
        
        while (kIter)
        {
            ParticleLinkUpInfo* pkMAXEmitterInfo = 
                ms_kLinkUpList.GetNext(kIter);
            NiNode* pkMAXEmitterNode = NiMAXHierConverter::Lookup(
                pkMAXEmitterInfo->m_pkMAXNode, NULL);
            
            if (!pkMAXEmitterNode)
            {
                NiNode* pkParent = pkMAXEmitterInfo->m_spSystem->GetParent();
                pkMAXEmitterNode = (NiNode*)pkParent;

            }

            if (NiIsKindOf(NiPSysVolumeEmitter, 
                pkMAXEmitterInfo->m_spModifier))
            {
                NiPSysVolumeEmitter* pkEmitter = 
                    NiDynamicCast(NiPSysVolumeEmitter,
                    pkMAXEmitterInfo->m_spModifier);
                pkEmitter->SetEmitterObj(pkMAXEmitterNode);
            }
            else if (NiIsKindOf(NiPSysMeshEmitter, 
                pkMAXEmitterInfo->m_spModifier))
            {
                NiPSysMeshEmitter* pkEmitter = 
                    NiDynamicCast(NiPSysMeshEmitter,
                    pkMAXEmitterInfo->m_spModifier);
                PopulateMeshEmitter(pkMAXEmitterNode, pkEmitter);
                continue;
            }
            else if (NiIsKindOf(NiPSysColliderManager, 
                pkMAXEmitterInfo->m_spModifier))
            {
                if (NiIsKindOf(NiPSysPlanarCollider, 
                    pkMAXEmitterInfo->m_spCollider))
                {
                    NiPSysPlanarCollider* pkCollider =
                        NiDynamicCast(NiPSysPlanarCollider,
                        pkMAXEmitterInfo->m_spCollider);
                    pkCollider->SetColliderObj(pkMAXEmitterNode);
                }
                else if (NiIsKindOf(NiPSysSphericalCollider,
                    pkMAXEmitterInfo->m_spCollider))
                {
                    NiPSysSphericalCollider* pkCollider = 
                        NiDynamicCast(NiPSysSphericalCollider,
                        pkMAXEmitterInfo->m_spCollider);
                    pkCollider->SetColliderObj(pkMAXEmitterNode);
                }
            }
            else if (NiIsKindOf(NiPSysGravityModifier,
                pkMAXEmitterInfo->m_spModifier))
            {
                NiPSysGravityModifier* pkModifier = 
                    NiDynamicCast(NiPSysGravityModifier,
                    pkMAXEmitterInfo->m_spModifier);
                pkModifier->SetGravityObj(pkMAXEmitterNode);
            }
            else if (NiIsKindOf(NiPSysBombModifier, 
                pkMAXEmitterInfo->m_spModifier))
            {
                NiPSysBombModifier* pkModifier = 
                    NiDynamicCast(NiPSysBombModifier, 
                    pkMAXEmitterInfo->m_spModifier);
                pkModifier->SetBombObj(pkMAXEmitterNode);
            }
            else if (NiIsKindOf(NiPSysDragModifier,
                pkMAXEmitterInfo->m_spModifier))
            {
                NiPSysDragModifier* pkModifier = 
                    NiDynamicCast(NiPSysDragModifier,
                    pkMAXEmitterInfo->m_spModifier);
                pkModifier->SetDragObj(pkMAXEmitterNode);
            }


            // Make sure NiOptimize will not remove the emitter node.
            NiStringExtraData* pkString = 
                NiNew NiStringExtraData("sgoKeep");

            pkMAXEmitterNode->AddExtraData(pkString);
        }
    }

    if (!ms_kStaticBoundList.IsEmpty())
    {
        NiTListIterator kIter = ms_kStaticBoundList.GetHeadPos();

        while (kIter)
        {
            ParticleStaticBoundInfo* pkMAXBoundInfo = 
                ms_kStaticBoundList.GetNext(kIter);

            NiParticleSystem* pkPSystem = pkMAXBoundInfo->m_spSystem;
            NiNode* pkParent = pkPSystem->GetParent();
           
            NiPoint3 kBoundPosition = pkMAXBoundInfo->m_kBound.GetCenter();
            float fBoundRadius = pkMAXBoundInfo->m_kBound.GetRadius();
            NiBound kStaticBound;

            GetRoot(pkParent)->UpdateDownwardPass(0.0f, false);
                
            if (!pkPSystem->GetWorldSpace())
            {
                NiTransform kPSysCoordSystem = pkPSystem->GetWorldTransform();
                NiTransform kPSysWorldBounds;
                kPSysWorldBounds.m_Translate = kBoundPosition;
                kPSysWorldBounds.m_Rotate.MakeIdentity();
                kPSysWorldBounds.m_fScale = fBoundRadius;
                NiTransform kInvPSysCoordSystem;
                kPSysCoordSystem.Invert(kInvPSysCoordSystem);
                NiTransform kPSysLocalBounds = kInvPSysCoordSystem * 
                    kPSysWorldBounds;
                kBoundPosition = kPSysLocalBounds.m_Translate;
                fBoundRadius = kPSysLocalBounds.m_fScale;
            }
            else
            {
                float fInverseScale = 1.0f / pkPSystem->GetWorldScale();
                fBoundRadius *= fInverseScale;
                kBoundPosition *= fInverseScale;
            }
            kStaticBound.SetCenterAndRadius(kBoundPosition, fBoundRadius);
            pkPSystem->SetModelBound(kStaticBound);
        }
    }

    if (!ms_kRemapList.IsEmpty())
    {
        NiTListIterator kIter = ms_kRemapList.GetHeadPos();
        
        while (kIter)
        {
            ParticleRemapInfo* pkMAXRootInfo = ms_kRemapList.GetNext(kIter);

            NiNode* pkParent = NULL;
            if (pkMAXRootInfo->m_pkMAXNode)
            {
                pkParent = NiMAXHierConverter::Lookup(
                    pkMAXRootInfo->m_pkMAXNode, NULL);
            }
            else
            {
                pkParent = pkMAXRootInfo->m_spSystem->GetParent();
                while (pkParent->GetParent() != NULL)
                {
                    pkParent = pkParent->GetParent();
                }
            }

            if (pkParent != NULL)
            {
                NiAVObject* pkOldParent = 
                    pkMAXRootInfo->m_spSystem->GetParent();
                if (pkOldParent != NULL)
                {
                    NiZBufferPropertyPtr spProp = (NiZBufferProperty*)
                       pkOldParent->GetProperty(NiZBufferProperty::GetType());
                    if (spProp)
                    {
                        spProp = (NiZBufferProperty*) spProp->Clone();
                        pkMAXRootInfo->m_spSystem->AttachProperty(spProp);
                    }
                }

                if (!pkMAXRootInfo->m_spSystem->GetWorldSpace())
                {
                    GetRoot(pkParent)->UpdateDownwardPass(0.0f, false);
                    pkMAXRootInfo->m_spSystem->Update(0.0f);
                    NiTransform kNewParent = pkParent->GetWorldTransform();
                    NiTransform kOldParent;
                    kOldParent = 
                        pkMAXRootInfo->m_spSystem->GetWorldTransform();
                    
                    NiTransform kInvNewParent;
                    kNewParent.Invert(kInvNewParent);
                    NiTransform kOldParentToNewParent = kInvNewParent * 
                        kOldParent;
                    pkMAXRootInfo->m_spSystem->SetTranslate(
                        kOldParentToNewParent.m_Translate);
                    pkMAXRootInfo->m_spSystem->SetRotate(
                        kOldParentToNewParent.m_Rotate);
                    pkMAXRootInfo->m_spSystem->SetScale(
                        kOldParentToNewParent.m_fScale);
                }
                pkParent->AttachChild(pkMAXRootInfo->m_spSystem);
                pkParent->SetSelectiveUpdate(true);
                pkParent->SetSelectiveUpdatePropertyControllers(true);
                pkParent->SetSelectiveUpdateRigid(false);
                pkParent->SetSelectiveUpdateTransforms(true);

                // Make sure NiOptimize will not remove the psys parent node.
                NiStringExtraData* pkString = 
                    NiNew NiStringExtraData("sgoKeep");

                pkParent->AddExtraData(pkString);
            }
        }
    }

    // Delete the list
    NiTListIterator kIter = ms_kLinkUpList.GetHeadPos();
    
    while (kIter)
    {
        NiDelete ms_kLinkUpList.RemovePos(kIter);
    }
    ms_kLinkUpList.RemoveAll();
    
    kIter = ms_kStaticBoundList.GetHeadPos();
    while (kIter)
    {
        NiDelete ms_kStaticBoundList.RemovePos(kIter);
    }
    ms_kStaticBoundList.RemoveAll();

    kIter = ms_kRemapList.GetHeadPos();
    while (kIter)
    {
        NiDelete ms_kRemapList.RemovePos(kIter);
    }
    ms_kRemapList.RemoveAll();
}
//---------------------------------------------------------------------------
NiFloatInterpolator* NiMAXPSysConverter::BuildFloatInterp(
    IParamBlock* pkParams, int iParamIdx, 
    NiTimeController::CycleType& eCycleType, float fScalar,
    bool bBoundInterp, float fStartTime, float fEndTime)
{
    if (!pkParams)
        return NULL;

    NiMAXAnimationConverter kConverter(m_kAnimStart, m_kAnimEnd);

    Control* pkControl = pkParams->GetController(iParamIdx);

    if (pkControl == NULL)
        return NULL;

    kConverter.SetORT(pkControl, eCycleType);
    NiInterpolator* pkInterp = NULL;

    pkInterp = kConverter.ConvertFloatAnim(pkControl);

    if (!pkInterp)
        return NULL;

    NiFloatInterpolator* pkFloatInterp = NiDynamicCast(
        NiFloatInterpolator, pkInterp);
    if (!pkFloatInterp || pkFloatInterp->GetKeyCount(0) == 0)
        return NULL;

    if (fScalar != 1.0f)
    {
        for (unsigned int ui = 0; ui < pkFloatInterp->GetKeyCount(0); ui++)
        {
            NiFloatKey* pkKey = (NiFloatKey*)pkFloatInterp->GetKeyAt(ui, 0);
            pkKey->SetValue(pkKey->GetValue() * fScalar);
        }
    }

    return pkFloatInterp;
}
//---------------------------------------------------------------------------
NiFloatInterpolator* NiMAXPSysConverter::BuildFloatInterp(
    IParamBlock2* pkParams, int iParamIdx, 
    NiTimeController::CycleType& eCycleType, float fScalar, 
    bool bBoundInterp, float fStartTime, float fEndTime)
{
    if (!pkParams)
        return NULL;

    NiMAXAnimationConverter kConverter(m_kAnimStart, m_kAnimEnd);

    Control* pkControl = pkParams->GetController(iParamIdx);

    if (pkControl == NULL)
        return NULL;

    eCycleType = kConverter.GetORT(pkControl);
    NiInterpolator* pkInterp;
    NiFloatInterpolator* pkFloatInterp;

    pkInterp = kConverter.ConvertFloatAnim(pkControl);

    if (!pkInterp || !NiIsKindOf(NiFloatInterpolator, pkInterp))
        return NULL;
    pkFloatInterp = NiDynamicCast(NiFloatInterpolator, pkInterp);

    if (fScalar != 1.0f)
    {
        for (unsigned int ui = 0; ui < pkFloatInterp->GetKeyCount(0); ui++)
        {
            NiFloatKey* pkKey = (NiFloatKey*)pkFloatInterp->GetKeyAt(ui,0);
            pkKey->SetValue(pkKey->GetValue() * fScalar);
        }
    }

    return pkFloatInterp;
}
//---------------------------------------------------------------------------
NiStepBoolKey* NiMAXPSysConverter::BuildActiveKeys(float fLoopStart, 
    float fLoopEnd, float fActiveStart, float fActiveEnd, 
    unsigned int& uiNumKeys)
{
    uiNumKeys = 0;
    NiStepBoolKey* pkBoolKeys = NiNew NiStepBoolKey[4];
    unsigned int uiIndex = 0;

    if (fActiveStart == fActiveEnd)
        fActiveEnd = fActiveStart + PSYS_INTERVAL_TIME;

    if (fLoopEnd <= fActiveStart)
    {
        NILOGWARNING("WARNING! A particle system has a loop end time less "
            "than the emission start time.\nSetting the loop end time to "
            "equal the emission stop time.\n");
        fLoopEnd = fActiveEnd;
    }

    if (fActiveStart <= fLoopStart)
    {
        pkBoolKeys[uiIndex].SetTime(fActiveStart);
        pkBoolKeys[uiIndex].SetBool(true);
        uiIndex++;
    }
    else
    {
        pkBoolKeys[uiIndex].SetTime(fLoopStart);
        pkBoolKeys[uiIndex].SetBool(false);
        uiIndex++;

        pkBoolKeys[uiIndex].SetTime(fActiveStart);
        pkBoolKeys[uiIndex].SetBool(true);
        uiIndex++;
    }
    
    if (fActiveEnd < fLoopEnd)
    {
        pkBoolKeys[uiIndex].SetTime(fActiveEnd);
        pkBoolKeys[uiIndex].SetBool(false);
        uiIndex++;

        pkBoolKeys[uiIndex].SetTime(fLoopEnd);
        pkBoolKeys[uiIndex].SetBool(false);
        uiIndex++;
    }
    else
    {
        pkBoolKeys[uiIndex].SetTime(fLoopEnd);
        pkBoolKeys[uiIndex].SetBool(false);
        uiIndex++;
    }
    
    uiNumKeys = uiIndex;

    return pkBoolKeys;
}
//---------------------------------------------------------------------------
void NiMAXPSysConverter::SetEmitterControllerStartAndStop(
    unsigned int& uiNumEmitterActiveKeys, NiStepBoolKey*& pkEmitterActiveKeys,
    unsigned int uiNumBirthRateKeys, NiFloatKey* pkBirthRateKeys,
    NiFloatKey::KeyType eBirthRateKeyType)
{
    double dTime;
    double dStep = 1.0f / (GetFrameRate() * 4.0);
    double dStartTime = 0.0;
    double dEndTime = 0.0;

    // Determine the Start and End Keys
    switch(eBirthRateKeyType)
    {
    case NiAnimationKey::BEZKEY:
        dStartTime = ((NiBezFloatKey*)pkBirthRateKeys)[0].GetTime();
        dEndTime = ((NiBezFloatKey*)pkBirthRateKeys)
            [uiNumBirthRateKeys-1].GetTime();
        break;
    case NiAnimationKey::LINKEY:
        dStartTime = ((NiLinFloatKey*)pkBirthRateKeys)[0].GetTime();
        dEndTime = ((NiLinFloatKey*)pkBirthRateKeys)
            [uiNumBirthRateKeys-1].GetTime();
        break;
    default:
        NIASSERT(false);
    }

    // Count the number of necessary keys
    uiNumEmitterActiveKeys = 0;

    float fRate = 0.0f;
    float fLastRate = 0.0f;
    unsigned int uiLastIdx = 0;

    unsigned char ucBirthRateSize = NiFloatKey::GetKeySize(eBirthRateKeyType);
    
    // Super Sample the Keys looking for where keys become positive
    for(dTime = dStartTime; dTime <= (dEndTime + (dStep / 2.0)); 
        dTime += dStep)
    {
        fRate = NiFloatKey::GenInterp((float) dTime, pkBirthRateKeys, 
            eBirthRateKeyType, uiNumBirthRateKeys, uiLastIdx, 
            ucBirthRateSize);

        // First Key
        if(NiOptimize::CloseTo((float) dTime, (float) dStartTime, (float)
            (dStep / 10.0)))
        {
            uiNumEmitterActiveKeys++;
        }
        else
        {
            // Starting Positive
            if ((fRate > 0.0f) && (fLastRate <= 0.0f))
            {
                uiNumEmitterActiveKeys++;
            }
        }

        // Last Key
        if(NiOptimize::CloseTo((float) dTime, (float) dEndTime, (float)
            (dStep / 10.0)))
        {
            uiNumEmitterActiveKeys++;
        }
        else
        {
            // Starting Negative
            if ((fRate <= 0.0f) && (fLastRate > 0.0f))
            {
                uiNumEmitterActiveKeys++;
            }
        }

        fLastRate = fRate;
    }

    // Allocate the necessary Keys
    pkEmitterActiveKeys = NiNew NiStepBoolKey[uiNumEmitterActiveKeys];

    // Create the Keys
    fRate = 0.0f;
    fLastRate = 0.0f;
    uiLastIdx = 0;
    unsigned int uiCurrentKey = 0;

    // Super Sample the Keys looking for where keys become positive
    for(dTime = dStartTime; dTime <= (dEndTime + (dStep / 2.0)); 
        dTime += dStep)
    {
        fRate = NiFloatKey::GenInterp((float) dTime, pkBirthRateKeys, 
            eBirthRateKeyType, uiNumBirthRateKeys, uiLastIdx, 
            ucBirthRateSize);

        // First Key
        if(NiOptimize::CloseTo((float) dTime, (float) dStartTime, (float)
            (dStep / 10.0)))
        {
            // Create a begin key at the current time
            pkEmitterActiveKeys[uiCurrentKey].SetTime((float) dTime);
            pkEmitterActiveKeys[uiCurrentKey++].SetBool(fRate > 0.0f);
        }
        else
        {
            // Starting Positive
            if ((fRate > 0.0f) && (fLastRate <= 0.0f))
            {
                // Create a key at the last time
                pkEmitterActiveKeys[uiCurrentKey].SetTime((float)
                    (dTime - dStep));
                pkEmitterActiveKeys[uiCurrentKey++].SetBool(true);
            }
        }

        // Last Key
        if(NiOptimize::CloseTo((float) dTime, (float) dEndTime, (float)
            (dStep / 10.0)))
        {
            // Create an end key at the current time
            pkEmitterActiveKeys[uiCurrentKey].SetTime((float) dTime);
            pkEmitterActiveKeys[uiCurrentKey++].SetBool(fRate > 0.0f);
        }
        else
        {
            // Starting Negative
            if ((fRate <= 0.0f) && (fLastRate > 0.0f))
            {
                // Create a key at the last time
                pkEmitterActiveKeys[uiCurrentKey].SetTime((float)
                    (dTime - dStep));
                pkEmitterActiveKeys[uiCurrentKey++].SetBool(false);
            }
        }

        fLastRate = fRate;
    }
}

//---------------------------------------------------------------------------
