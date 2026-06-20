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

#include <NiFilename.h>

#include <NxPhysics.h>

NiPhysXManager* NiPhysXManager::ms_pPhysXManager;
char* NiPhysXManager::ms_pcPlatformSubDir;

//---------------------------------------------------------------------------
NiPhysXManager::NiPhysXManager()
{
    m_pkAllocator = 0;
    m_pkOutputStream = 0;
    m_pkPhysXSDK = 0;
    ms_pcPlatformSubDir = 0;
}
//---------------------------------------------------------------------------
NiPhysXManager::~NiPhysXManager()
{
    if (ms_pcPlatformSubDir)
    {
        NiFree(ms_pcPlatformSubDir);
    }
    ms_pcPlatformSubDir = 0;
}
//---------------------------------------------------------------------------
bool NiPhysXManager::Initialize(NxUserAllocator* pkAllocator,
        NxUserOutputStream* pkOutputStream, const NxPhysicsSDKDesc& kSDKDesc)
{
    // If allocator or output stream are NULL, then allocate and use the
    // defaults. If someone is already using an SDK, our suggestions will be
    // ignored, but that should only happen in art tools.
    if (!pkAllocator)
    {
        m_pkAllocator = NiNew NiPhysXAllocator;
        pkAllocator = m_pkAllocator;
    }
    if (!pkOutputStream)
    {
        m_pkOutputStream = NiNew NiPhysXUserOutput;
        pkOutputStream = m_pkOutputStream;
    }
    
    WaitSDKLock();
    m_pkPhysXSDK = NxCreatePhysicsSDK(NX_PHYSICS_SDK_VERSION, pkAllocator,
        pkOutputStream, kSDKDesc);
    ReleaseSDKLock();
    
    if (m_pkPhysXSDK == 0)
    {
        NiOutputDebugString("NiPhysXManager: Failed to create PhysX SDK\n");
        return false;
    }
    
    if (!PlatformSpecificPostSDKInit())
    {
        Shutdown();
        return false;
    }
    
    return true;
}
//---------------------------------------------------------------------------
void NiPhysXManager::Shutdown()
{
    if (m_pkPhysXSDK)
    {    
        // Clean up the SDK
        WaitSDKLock();
        NxReleasePhysicsSDK(m_pkPhysXSDK);
        m_pkPhysXSDK = 0;
        ReleaseSDKLock();
    }
    
    // Clean up the allocator and output stream
    NiDelete m_pkOutputStream;
    m_pkOutputStream = 0;
    NiDelete m_pkAllocator;
    m_pkAllocator = 0;
    
    // Clear out the meshes
    WaitMeshLock();
    m_kClothMeshes.RemoveAll();
    m_kConvexMeshes.RemoveAll();
    m_kTriangleMeshes.RemoveAll();
    ReleaseMeshLock();

    PlatformSpecificSDKShutdown();
}
//---------------------------------------------------------------------------
const char* NiPhysXManager::GetPlatformSpecificSubdirectory() 
{
    return ms_pcPlatformSubDir;
}
//---------------------------------------------------------------------------
void NiPhysXManager::SetPlatformSpecificSubdirectory(const char* pcSubDir)
{
    if (ms_pcPlatformSubDir)
    {
        NiFree(ms_pcPlatformSubDir);
    }

    if (pcSubDir)
    {
        unsigned int uiLen = strlen(pcSubDir) + 1;
        ms_pcPlatformSubDir = NiAlloc(char, uiLen);
        NiStrcpy(ms_pcPlatformSubDir, uiLen, pcSubDir);
        NiPath::Standardize(ms_pcPlatformSubDir);
    }
    else
    {
        ms_pcPlatformSubDir = NULL;
    }
}
//---------------------------------------------------------------------------
char* NiPhysXManager::ConvertFilenameToPlatformSpecific(const char* pcPath)
{
    char* pcPlatformPath = NiAlloc(char, 260);
    if (ms_pcPlatformSubDir)
    {
        NiFilename kPlatform(pcPath);
        kPlatform.SetPlatformSubDir(ms_pcPlatformSubDir);
        kPlatform.GetFullPath(pcPlatformPath, 260);
    }
    else
    {
        NiStrncpy(pcPlatformPath, 260, pcPath, strlen(pcPath));
    }
    return pcPlatformPath;
}
//---------------------------------------------------------------------------
void NiPhysXManager::ConvertFilenameToPlatformSpecific(
    const NiFixedString& kPath, NiFixedString& kPlatformSpecificPath)
{
    if (ms_pcPlatformSubDir)
    {
        char acPlatformPath[260];
        NiFilename kPlatform((const char*)kPath);
        kPlatform.SetPlatformSubDir(ms_pcPlatformSubDir);
        kPlatform.GetFullPath(acPlatformPath, 260);
        kPlatformSpecificPath = acPlatformPath;
    }
    else
    {
        kPlatformSpecificPath = kPath;
    }
}
//---------------------------------------------------------------------------
NiPhysXClothMesh* NiPhysXManager::GetClothMesh(const NxClothMesh* pkMesh)
{
    WaitMeshLock();
    NiTMapIterator iter = m_kClothMeshes.GetFirstPos();
    while (iter)
    {
        const char* pcKey;
        NiPhysXClothMeshPtr spThisMesh;
        m_kClothMeshes.GetNext(iter, pcKey, spThisMesh);
        if (spThisMesh->GetMesh() == pkMesh)
        {
            ReleaseMeshLock();    
            return spThisMesh;
        }
    }
    
    ReleaseMeshLock();
    return NULL;
}
//---------------------------------------------------------------------------
NiPhysXConvexMesh* NiPhysXManager::GetConvexMesh(const NxConvexMesh* pkMesh)
{
    WaitMeshLock();
    NiTMapIterator iter = m_kConvexMeshes.GetFirstPos();
    while (iter)
    {
        const char* pcKey;
        NiPhysXConvexMeshPtr spThisMesh;
        m_kConvexMeshes.GetNext(iter, pcKey, spThisMesh);
        if (spThisMesh->GetMesh() == pkMesh)
        {
            ReleaseMeshLock();    
            return spThisMesh;
        }
    }
    
    ReleaseMeshLock();
    return NULL;
}
//---------------------------------------------------------------------------
NiPhysXTriangleMesh* NiPhysXManager::GetTriangleMesh(
    const NxTriangleMesh* pkMesh)
{
    WaitMeshLock();
    NiTMapIterator iter = m_kTriangleMeshes.GetFirstPos();
    while (iter)
    {
        const char* pcKey;
        NiPhysXTriangleMeshPtr spThisMesh;
        m_kTriangleMeshes.GetNext(iter, pcKey, spThisMesh);
        if (spThisMesh->GetMesh() == pkMesh)
        {
            ReleaseMeshLock();    
            return spThisMesh;
        }
    }
    
    ReleaseMeshLock();    
    return NULL;
}
//---------------------------------------------------------------------------
