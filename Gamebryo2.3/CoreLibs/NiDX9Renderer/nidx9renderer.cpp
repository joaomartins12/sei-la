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
#include "NiD3DRendererPCH.h"

#include "NiDX92DBufferData.h"
#include "NiDX9SystemDesc.h"
#include "NiDX9Renderer.h"
#include "NiDX9VBManager.h"
#include "NiDX9IBManager.h"
#include "NiDX9TextureManager.h"
#include "NiDX9LightManager.h"
#include "NiDX9PixelFormat.h"
#include "NiDX9TextureData.h"
#include "NiDX9Direct3DTexture.h"
#include "NiDX9Direct3DTextureData.h"
#include "NiDX9DynamicTextureData.h"
#include "NiDX9FragmentShader.h"
#include "NiDX9ShadowWriteShader.h"
#include "NiDX9SourceCubeMapData.h"
#include "NiDX9SourceTextureData.h"
#include "NiDX9RenderedCubeMapData.h"
#include "NiDX9RenderedTextureData.h"
#include "NiD3DGeometryGroupManager.h"
#include "NiDynamicGeometryGroup.h"
#include "NiUnsharedGeometryGroup.h"
#include "NiVBChip.h"
#include "NiVBBlock.h"
#include "NiD3DShaderInterface.h"
#include "NiD3DDefaultShader.h"
#include "NiD3DUtils.h"
#include "NiDX9ErrorShader.h"

#include <NiAVObject.h>
#include <NiBound.h>
#include <NiPoint3.h>
#include <NiPoint2.h>
#include <NiPlane.h>
#include <NiCamera.h>
#include <NiDynamicTexture.h>
#include <NiGeometry.h>
#include <NiGeometryData.h>
#include <NiLight.h>
#include <NiLines.h>
#include <NiParticles.h>
#include <NiRenderedCubeMap.h>
#include <NiRenderedTexture.h>
#include <NiSourceCubeMap.h>
#include <NiScreenTexture.h>
#include <NiShaderTimeController.h>
#include <NiSkinInstance.h>
#include <NiSkinPartition.h>
#include <NiSystem.h>
#include <NiTexturingProperty.h>
#include <NiTriShape.h>
#include <NiTriShapeData.h>
#include <NiTriStrips.h>
#include <NiTriStripsData.h>

#include "NiD3DGPUProgramCache.h"

//---------------------------------------------------------------------------
// The following copyright notice may not be removed.
static char EmergentCopyright[] NI_UNUSED = 
    "Copyright 2007 Emergent Game Technologies";
//---------------------------------------------------------------------------
static char acGamebryoVersion[] NI_UNUSED = 
    GAMEBRYO_MODULE_VERSION_STRING(NiDX9);
//---------------------------------------------------------------------------

NiImplementRTTI(NiDX9Renderer,NiRenderer);

//HINSTANCE NiDX9Renderer::ms_hDXLib = NULL;
LPDIRECT3D9 NiDX9Renderer::ms_pkD3D9 = NULL;
NiCriticalSection NiDX9Renderer::ms_kD3DCriticalSection;

//NILPD3DCREATE NiDX9Renderer::ms_pfnD3DCreate9 = NULL;
// Map NiTexturingProperty::ClampModes to D3D clamping
NiDX9Renderer::AddressSettings NiDX9Renderer::ms_akD3DAddressMapping[
    NiTexturingProperty::CLAMP_MAX_MODES];
// Map NiTexturingProperty::FilterModes to D3D filtering
NiDX9Renderer::FilterSettings NiDX9Renderer::ms_akD3DFilterMapping[
    NiTexturingProperty::FILTER_MAX_MODES];
unsigned char NiDX9Renderer::ms_aucD3DFormatSizes[
    NiDX9Renderer::D3DFORMATSIZECOUNT];

// Projected flags
unsigned int NiDX9Renderer::ms_uiProjectedTextureFlags = D3DTTFF_DISABLE;

NiTPointerList<unsigned int> NiDX9Renderer::ms_kDisplayFormats;

static NiDX9RendererSDM NiDX9RendererSDMObject;

const NiDX9SystemDesc* NiDX9Renderer::ms_pkSystemDesc = NULL;

#if (defined(_DEBUG) && defined(DEBUG_INTERFACE_POINTERS))
//  These are to do reference count checking during debug development
//  char buffer defined in NiDX9Renderer.cpp
char g_szIFDebugBuff[1024];
int g_iDeviceRefCount = 0;
LPDIRECT3DDEVICE9 g_pkD3DDevice = NULL;
#endif  //#if (defined(_DEBUG) && defined(DEBUG_INTERFACE_POINTERS))

//---------------------------------------------------------------------------
void NiDX9Renderer::InitToDefault()
{
    m_pkD3DDevice9 = NULL;
    memset(&m_kD3DCaps9, 0, sizeof(m_kD3DCaps9));
    m_kWndDevice = 0;
    m_acDriverDesc[0] = '\0';
    m_uiAdapter = D3DADAPTER_DEFAULT;
    m_eDevType = D3DDEVTYPE_HAL;
    m_kWndFocus = 0;
    m_uiBehaviorFlags = 0;
    m_eAdapterFormat = D3DFMT_UNKNOWN;
    m_bSWVertexCapable = false;
    m_bSWVertexSwitchable = false;

    m_uiHWBones = 0;
    m_uiMaxStreams = 0;
    m_uiMaxPixelShaderVersion = 0;
    m_uiMaxVertexShaderVersion = 0;
    m_bMipmapCubeMaps = false;
    m_bDynamicTexturesCapable = false;

    m_uiMaxNumRenderTargets = 0;
    m_bIndependentBitDepths = false;
    m_bMRTPostPixelShaderBlending = false;

    m_uiResetCounter = 0;
    m_bDeviceLost = false;

    m_uiBackground = D3DCOLOR_XRGB(128, 128, 128);
    m_fZClear = 1.0;
    m_uiStencilClear = 0;
    m_uiRendFlags = 0;
    m_acBehavior[0] = '\0';

    m_kCamRight = NiPoint3::UNIT_X;
    m_kCamUp = NiPoint3::UNIT_Y;
    m_kModelCamRight = NiPoint3::UNIT_X;
    m_kModelCamUp = NiPoint3::UNIT_Y;

    m_fNearDepth = 0.1f;
    m_fDepthRange = 100.0f;

    memset(&m_kD3DIdentity, 0, sizeof(D3DMATRIX));
    m_kD3DIdentity._11 = m_kD3DIdentity._22 = 
        m_kD3DIdentity._33 = m_kD3DIdentity._44 = 1.0f;

    m_pkBatchHead = 0;
    m_pkBatchTail = 0;

    memset(&m_kD3DPort, 0, sizeof(D3DVIEWPORT9));
    m_kD3DPort.X = 0;
    m_kD3DPort.Y = 0;
    m_kD3DPort.Width = 1; // placeholder
    m_kD3DPort.Height = 1; // placeholder
    m_kD3DPort.MinZ = 0.0f;     
    m_kD3DPort.MaxZ = 1.0f;

    unsigned int i, j;
    for (j = 0; j < TEXUSE_NUM; j++)
    {
        for (i = 0; i < TEX_NUM; i++)
            m_aapkTextureFormats[j][i] = NULL;

        m_apkDefaultTextureFormat[j] = NULL;
        m_aspDefaultTextureData[j] = 0;
    }
    
    m_eReplacementDataFormat = TEX_DEFAULT;

    m_pkCurrRenderTargetGroup = NULL;
    m_pkCurrOnscreenRenderTargetGroup = NULL;

    m_pkVBManager = NULL;
    m_pkIBManager = NULL;
    m_pkTextureManager = NULL;
    m_pkRenderState = NULL;
    m_pkLightManager = NULL;

    m_pkGeometryGroupManager = NULL;
    m_pkDefaultGroup = NULL;
    m_pkDynamicGroup = NULL;

    m_pkScreenTextureVerts = NULL;
    m_pkScreenTextureColors = NULL;
    m_pkScreenTextureTexCoords = NULL;
    m_pusScreenTextureIndices = NULL;
    m_usNumScreenTextureVerts = 0;
    m_uiNumScreenTextureIndices = 0;

    m_kD3DMat._14 = 0.0f;
    m_kD3DMat._24 = 0.0f;
    m_kD3DMat._34 = 0.0f;
    m_kD3DMat._44 = 1.0f;

    memset(ms_aucD3DFormatSizes, 0, sizeof(ms_aucD3DFormatSizes));
    ms_aucD3DFormatSizes[D3DFMT_R8G8B8] = 24;
    ms_aucD3DFormatSizes[D3DFMT_A8R8G8B8] = 32;
    ms_aucD3DFormatSizes[D3DFMT_X8R8G8B8] = 32;
    ms_aucD3DFormatSizes[D3DFMT_R5G6B5] = 16;
    ms_aucD3DFormatSizes[D3DFMT_X1R5G5B5] = 16;
    ms_aucD3DFormatSizes[D3DFMT_A1R5G5B5] = 16;
    ms_aucD3DFormatSizes[D3DFMT_A4R4G4B4] = 16;
    ms_aucD3DFormatSizes[D3DFMT_R3G3B2] = 8;
    ms_aucD3DFormatSizes[D3DFMT_A8] = 8;
    ms_aucD3DFormatSizes[D3DFMT_A8R3G3B2] = 16;
    ms_aucD3DFormatSizes[D3DFMT_X4R4G4B4] = 16;
    ms_aucD3DFormatSizes[D3DFMT_A2B10G10R10] = 32;
    ms_aucD3DFormatSizes[D3DFMT_A8B8G8R8] = 32;
    ms_aucD3DFormatSizes[D3DFMT_X8B8G8R8] = 32;
    ms_aucD3DFormatSizes[D3DFMT_G16R16] = 32;
    ms_aucD3DFormatSizes[D3DFMT_A2R10G10B10] = 32;
    ms_aucD3DFormatSizes[D3DFMT_A16B16G16R16] = 64;
    ms_aucD3DFormatSizes[D3DFMT_A8P8] = 16;
    ms_aucD3DFormatSizes[D3DFMT_P8] = 8;
    ms_aucD3DFormatSizes[D3DFMT_L8] = 8;
    ms_aucD3DFormatSizes[D3DFMT_A8L8] = 16;
    ms_aucD3DFormatSizes[D3DFMT_A4L4] = 8;
    ms_aucD3DFormatSizes[D3DFMT_V8U8] = 16;
    ms_aucD3DFormatSizes[D3DFMT_L6V5U5] = 16;
    ms_aucD3DFormatSizes[D3DFMT_X8L8V8U8] = 32;
    ms_aucD3DFormatSizes[D3DFMT_Q8W8V8U8] = 32;
    ms_aucD3DFormatSizes[D3DFMT_V16U16] = 32;
    ms_aucD3DFormatSizes[D3DFMT_A2W10V10U10] = 32;
    ms_aucD3DFormatSizes[D3DFMT_D16_LOCKABLE] = 16;
    ms_aucD3DFormatSizes[D3DFMT_D32] = 32;
    ms_aucD3DFormatSizes[D3DFMT_D15S1] = 16;
    ms_aucD3DFormatSizes[D3DFMT_D24S8] = 32;
    ms_aucD3DFormatSizes[D3DFMT_D24X8] = 32;
    ms_aucD3DFormatSizes[D3DFMT_D24X4S4] = 32;
    ms_aucD3DFormatSizes[D3DFMT_D16] = 16;
    ms_aucD3DFormatSizes[D3DFMT_D32F_LOCKABLE] = 32;
    ms_aucD3DFormatSizes[D3DFMT_D24FS8] = 32;
    ms_aucD3DFormatSizes[D3DFMT_L16] = 16;
    ms_aucD3DFormatSizes[D3DFMT_INDEX16] = 16;
    ms_aucD3DFormatSizes[D3DFMT_INDEX32] = 32;
    ms_aucD3DFormatSizes[D3DFMT_Q16W16V16U16] = 64;
    ms_aucD3DFormatSizes[D3DFMT_R16F] = 16;
    ms_aucD3DFormatSizes[D3DFMT_G16R16F] = 32;
    ms_aucD3DFormatSizes[D3DFMT_A16B16G16R16F] = 64;
    ms_aucD3DFormatSizes[D3DFMT_R32F] = 32;
    ms_aucD3DFormatSizes[D3DFMT_G32R32F] = 64;
    ms_aucD3DFormatSizes[D3DFMT_A32B32G32R32F] = 128;
    ms_aucD3DFormatSizes[D3DFMT_CxV8U8] = 16;

    // setup the safe zone to the inner 98% of the display
    m_kDisplaySafeZone.m_top =  0.01f;
    m_kDisplaySafeZone.m_left =  0.01f;
    m_kDisplaySafeZone.m_right =  0.99f;
    m_kDisplaySafeZone.m_bottom =  0.99f;

    // Initialize cached frustum.
    m_kCachedFrustum.m_fLeft = -0.5f;
    m_kCachedFrustum.m_fRight = 0.5f;
    m_kCachedFrustum.m_fTop = 0.5f;
    m_kCachedFrustum.m_fBottom = -0.5f;
    m_kCachedFrustum.m_fNear = 1.0f;
    m_kCachedFrustum.m_fFar = 2.0f;
    m_kCachedFrustum.m_bOrtho = false;

    // Initialize cached port.
    m_kCachedPort.m_left = 0.0f;
    m_kCachedPort.m_right = 1.0f;
    m_kCachedPort.m_top = 1.0f;
    m_kCachedPort.m_bottom = 0.0f;
}
//---------------------------------------------------------------------------
bool NiDX9Renderer::IsD3D9Create()
{
    // This function assumes the critical section has already been locked.
  //  if (ms_pfnD3DCreate9)
	 if (ms_pkD3D9)
        return true;

// #if defined(_DEBUG)
//     ms_hDXLib = LoadLibraryA("D3D9D.DLL");
//     if (ms_hDXLib == NULL)
//         ms_hDXLib = LoadLibraryA("D3D9.DLL");
// #else //#if defined(_DEBUG)
//     ms_hDXLib = LoadLibraryA("D3D9.DLL");
// #endif //#if defined(_DEBUG)
// 
//     if (ms_hDXLib)
//     {
// //         // Find the D3D Create function, and call it
// //         // This function is statically linked into DX9 and above, but
// //         // not on NT4.0.  So, we need to query it
// //         ms_pfnD3DCreate9 = (NILPD3DCREATE)
// //             GetProcAddress(ms_hDXLib, "Direct3DCreate9");
// // 
// //         if (ms_pfnD3DCreate9)
//             return true;
//     }

    return false;
}
//---------------------------------------------------------------------------
int NiDX9Renderer::CreateDirect3D9()
{
    int iRet = 0;
    ms_kD3DCriticalSection.Lock();

    if (ms_pkD3D9 == 0)
    {
        if (!IsD3D9Create())
        {
            ms_pkD3D9 = Direct3DCreate9(D3D_SDK_VERSION);// (*ms_pfnD3DCreate9)(D3D_SDK_VERSION);
            if (ms_pkD3D9 == 0)
            {
                Message("Create D3D9 instance...FAILED\n");
                iRet = -1;
            }
            else
            {
                Message("Create D3D9 instance...SUCCESSFUL\n");
            }
        }
        else
        {
            Message("Create D3D9 instance...FAILED\n");
            iRet = -1;
        }
    }

    ms_kD3DCriticalSection.Unlock();
    return iRet;
}
//---------------------------------------------------------------------------
void NiDX9Renderer::_SDMShutdown()
{
    ms_kD3DCriticalSection.Lock();

    if (ms_pkD3D9)
    {
        D3D_POINTER_RELEASE(ms_pkD3D9);
        ms_pkD3D9 = 0;
    }

    NiDelete ms_pkSystemDesc;
    ms_pkSystemDesc = NULL;

//     if (ms_hDXLib)
//         FreeLibrary(ms_hDXLib);
  //  ms_pfnD3DCreate9 = 0;
    ms_kDisplayFormats.RemoveAll();

    ms_kD3DCriticalSection.Unlock();
}
//---------------------------------------------------------------------------
NiDX9Renderer::NiDX9Renderer() :
    m_kDepthStencilFormats(7)
{
    InitToDefault();
}
//---------------------------------------------------------------------------
NiDX9Renderer::~NiDX9Renderer()
{
    if (m_pkD3DDevice9)
    {
        // Set ALL stage texture to 0
        unsigned int ui;
        if (m_pkRenderState)
        {
            for (ui = 0; ui < NiD3DPass::ms_uiMaxTextureBlendStages; ui++)
                m_pkRenderState->SetTexture(ui,0);
        }

        // Set ALL streams to 0
        for (ui = 0; ui < m_uiMaxStreams; ui++)
            m_pkD3DDevice9->SetStreamSource(ui, 0, 0, 0);
        m_pkD3DDevice9->SetIndices(0);
    }

    for (unsigned int j = 0; j < TEXUSE_NUM; j++)
    {
        for (unsigned int i = 0; i < TEX_NUM; i++)
            NiDelete m_aapkTextureFormats[j][i];

        m_aspDefaultTextureData[j] = 0;
    }

    EmptyBatch();
    BatchedObject::Purge();
    
    NiTMapIterator kIter = m_kPrePackObjects.GetFirstPos();
    while (kIter)
    {
        NiVBBlock* pkBlock;
        PrePackObject* pkObject;
        m_kPrePackObjects.GetNext(kIter, pkBlock, pkObject);
        while (pkObject)
        {
            // Keep the linked list up-to-date as we remove objects!
            // This is because the list may be used during the deletion of 
            // another object (possibly released when the PrePackObject is
            // destroyed) to release resources.

            // We know there is no "previous" member to this object, because
            // we're removing them all.
            PrePackObject* pkNext = pkObject->m_pkNext;
            if (pkNext)
            {
                m_kPrePackObjects.SetAt(pkBlock, 
                    pkNext);
            }
            else
            {
                m_kPrePackObjects.RemoveAt(pkBlock);
            }
            NiDelete pkObject;
            pkObject = pkNext;
        }
    }
    NIASSERT(m_kPrePackObjects.IsEmpty());

    NiMaterial::UnloadShadersForAllMaterials();
    if (m_spLegacyDefaultShaderMaterial && NiIsKindOf(NiSingleShaderMaterial,
        m_spLegacyDefaultShaderMaterial))
    {
        NiSmartPointerCast(NiSingleShaderMaterial, 
            m_spLegacyDefaultShaderMaterial)->SetCachedShader(NULL);
    }
    if (m_spInitialDefaultMaterial && NiIsKindOf(NiSingleShaderMaterial,
        m_spInitialDefaultMaterial))
    {
        NiSmartPointerCast(NiSingleShaderMaterial, 
            m_spInitialDefaultMaterial)->SetCachedShader(NULL);
    }
    if (m_spCurrentDefaultMaterial && NiIsKindOf(NiSingleShaderMaterial,
        m_spCurrentDefaultMaterial))
    {
        NiSmartPointerCast(NiSingleShaderMaterial, 
            m_spCurrentDefaultMaterial)->SetCachedShader(NULL);
    }

    // Purge the D3DShaders
    PurgeAllD3DShaders();
    // Shut down the shader system
    NiD3DShaderInterface::ShutdownShaderSystem();

    NiDX9AdditionalDepthStencilBufferData::ShutdownAll();
    m_kRenderedTextures.RemoveAll();
    m_kRenderedCubeMaps.RemoveAll();
    m_kDynamicTextures.RemoveAll();
    
    NiDX9TextureData::ClearTextureData(this);

    // loop for each onscreen buffer
    kIter = m_kOnscreenRenderTargetGroups.GetFirstPos();
    while (kIter)
    {
        NiWindowRef kHWND;
        NiRenderTargetGroupPtr spRenderTargetGroup;

        m_kOnscreenRenderTargetGroups.GetNext(kIter, kHWND, 
            spRenderTargetGroup);
        if (spRenderTargetGroup)
        {
            for (unsigned int ui = 0; 
                ui < spRenderTargetGroup->GetBufferCount();
                ui++)
            {
                Ni2DBuffer* pkBuffer = spRenderTargetGroup->GetBuffer(ui);
                if (pkBuffer)
                {
                    pkBuffer->SetRendererData(NULL);
                }
            }

            Ni2DBuffer* pkBuffer = 
                spRenderTargetGroup->GetDepthStencilBuffer();
            if (pkBuffer)
            {
                pkBuffer->SetRendererData(NULL);
            }
        }
    }

    m_kOnscreenRenderTargetGroups.RemoveAll();

    // Clear out the m_kDepthStencilFormats map
    kIter = m_kDepthStencilFormats.GetFirstPos();
    while (kIter)
    {
        D3DFORMAT eFormat;
        NiPixelFormat* pkPixelFormat = NULL;
        m_kDepthStencilFormats.GetNext(kIter, eFormat, pkPixelFormat);
        NiDelete pkPixelFormat;
    }

    // Explicitly delete this so the Screen Texture CTD is released before
    // the Direct3DDevice9 is released.
    m_spScreenTexturePropertyState = 0;

    NiDelete m_pkVBManager;
    m_pkVBManager = NULL;

    if (m_pkIBManager)
    {
        NiDelete m_pkIBManager;
        m_pkIBManager = NULL;
    }

    if (m_pkGeometryGroupManager)
    {
        m_pkGeometryGroupManager->DestroyGroup(m_pkDefaultGroup);
        m_pkGeometryGroupManager->DestroyGroup(m_pkDynamicGroup);
        NiDelete m_pkGeometryGroupManager;
        m_pkGeometryGroupManager = NULL;
    }


    NiDelete m_pkRenderState;
    m_pkRenderState = NULL;

    NiDelete m_pkTextureManager;
    m_pkTextureManager = NULL;

    NiDelete m_pkLightManager;
    m_pkLightManager = NULL;


    if (m_pkD3DDevice9)
    {
        D3D_POINTER_RELEASE(m_pkD3DDevice9);
        m_pkD3DDevice9 = 0;
    }

    NiDelete[] m_pkScreenTextureVerts;
    NiDelete[] m_pkScreenTextureColors;
    NiDelete[] m_pkScreenTextureTexCoords;
    NiFree( m_pusScreenTextureIndices);

    RemoveAllResetNotificationFuncs();
    RemoveAllLostDeviceNotificationFuncs();

    NiStream::UnregisterLoader("NiPersistentSrcTextureRendererData");
    NiPersistentSrcTextureRendererData::ResetStreamingFunctions();
}
//---------------------------------------------------------------------------
NiDX9Renderer* NiDX9Renderer::Create(unsigned int uiWidth, 
    unsigned int uiHeight, unsigned int uiUseFlags, NiWindowRef kWndDevice, 
    NiWindowRef kWndFocus, unsigned int uiAdapter, DeviceDesc eDesc, 
    FrameBufferFormat eFBFormat, DepthStencilFormat eDSFormat, 
    PresentationInterval ePresentationInterval, SwapEffect eSwapEffect, 
    unsigned int uiFBMode, unsigned int uiBackBufferCount, 
    unsigned int uiRefreshRate)
{
    // Create the Direct3D9 interface
    if (CreateDirect3D9() == -1)
    {
        SetLastErrorString("Creation failed: Could not initialize DirectX9");
        Error("NiDX9Renderer::Create> Invalid device ID specified "
            "- FAILING\n");
        return NULL;
    }

    // Create an 'empty' instance of the renderer
    NiDX9Renderer* pkR = NiNew NiDX9Renderer;
    NIASSERT(pkR);

    ms_kD3DCriticalSection.Lock();

    bool bSuccess = pkR->Initialize(uiWidth, uiHeight, uiUseFlags, kWndDevice, 
        kWndFocus, uiAdapter, eDesc, eFBFormat, eDSFormat, 
        ePresentationInterval, eSwapEffect, uiFBMode, uiBackBufferCount, 
        uiRefreshRate);

    ms_kD3DCriticalSection.Unlock();

    if (bSuccess == false)
    {
        NiDelete pkR;
        pkR = NULL;
    }
    else
    {
        // Registration of the DX9-specific
        // NiDX9PersistentSrcTextureRendererData::LoadBinary() that overrides
        // NiPersistentSrcTextureRendererData::LoadBinary();
        NiStream::UnregisterLoader("NiPersistentSrcTextureRendererData");
        NiStream::RegisterLoader("NiPersistentSrcTextureRendererData", 
            NiDX9PersistentSrcTextureRendererData::CreateObject);
    }
    return pkR;
}
//---------------------------------------------------------------------------
bool NiDX9Renderer::Initialize(unsigned int uiWidth, unsigned int uiHeight, 
    unsigned int uiUseFlags, NiWindowRef kWndDevice, NiWindowRef kWndFocus, 
    unsigned int uiAdapter, DeviceDesc eDesc, FrameBufferFormat eFBFormat, 
    DepthStencilFormat eDSFormat, PresentationInterval ePresentationInterval, 
    SwapEffect eSwapEffect, unsigned int uiFBMode, 
    unsigned int uiBackBufferCount, unsigned int uiRefreshRate)
{
    m_uiCreationWidth = uiWidth;
    m_uiCreationHeight = uiHeight;
    m_uiCreationUseFlags = uiUseFlags;
    m_kCreationWndDevice = kWndDevice;
    m_kCreationWndFocus = kWndFocus;
    m_uiCreationAdapter = uiAdapter;
    m_eCreationDesc = eDesc;
    m_eCreationFBFormat = eFBFormat;
    m_eCreationDSFormat = eDSFormat;
    m_eCreationPresentationInterval = ePresentationInterval;
    m_eCreationSwapEffect = eSwapEffect;
    m_uiCreationFBMode = uiFBMode;
    m_uiCreationBackBufferCount = uiBackBufferCount;
    m_uiCreationRefreshRate = uiRefreshRate;

    // Unlock so GetSystemDesc can pick up the lock - shouldn't make a
    // difference if the process gets interrupted here.
    ms_kD3DCriticalSection.Unlock();
    const NiDX9SystemDesc* pkSystemDesc = GetSystemDesc();
    ms_kD3DCriticalSection.Lock();
    NIASSERT(pkSystemDesc);

    //  Adapter
    m_uiAdapter = uiAdapter;

    m_kWndDevice = kWndDevice;
    m_kWndFocus = kWndFocus;

    if (!ExpandDeviceType(eDesc, m_eDevType, m_uiBehaviorFlags))
    {
        SetLastErrorString("Creation failed: Invalid 3D device type");
        Error("NiDX9Renderer::Initialize> Invalid device ID specified - "
            "FAILING\n");

        return false;
    }

    if (uiUseFlags & USE_FPU_PRESERVE)
        m_uiBehaviorFlags |= D3DCREATE_FPU_PRESERVE;

    if (uiUseFlags & USE_MULTITHREADED)
        m_uiBehaviorFlags |= D3DCREATE_MULTITHREADED;

    m_pkAdapterDesc = ms_pkSystemDesc->GetAdapter(m_uiAdapter);

    if (!m_pkAdapterDesc)
    {
        SetLastErrorString("Creation failed: Invalid Adapter");
        Error("NiDX9Renderer::Initialize> Invalid Adapter specified - "
            "FAILING\n");

        return false;
    }

    m_pkDeviceDesc = m_pkAdapterDesc->GetDevice(m_eDevType);

    if (!m_pkDeviceDesc)
    {
        SetLastErrorString("Creation failed: Invalid 3D device type");
        Error("NiDX9Renderer::Initialize> Invalid device ID specified - "
            "FAILING\n");

        return false;
    }

    HRESULT eD3dRet;

    D3DPRESENT_PARAMETERS kD3DPresentParams;
    memset(&kD3DPresentParams, 0, sizeof(kD3DPresentParams));

    while (1)
    {
        Message("NiDX9Renderer::Initialize> Attempting to create "
            "rendering device\n");

        if (!ms_pkSystemDesc->GetAdapter(m_uiAdapter)->GetDevice(m_eDevType))
        {
            SetLastErrorString("Creation failed: Requested device not valid");
            Warning("NiDX9Renderer::Initialize> No device of specified "
                "type \n");
        }
        else
        {
            if (InitializePresentParams(m_kWndDevice, uiWidth, uiHeight, 
                uiUseFlags, uiFBMode, eFBFormat, eDSFormat, uiBackBufferCount, 
                eSwapEffect, uiRefreshRate, ePresentationInterval, 
                kD3DPresentParams))
            {
                // Cache the backbuffer count (Validate may change it)
                uiBackBufferCount = kD3DPresentParams.BackBufferCount;

                //  Create the device
                eD3dRet = ms_pkD3D9->CreateDevice(m_uiAdapter, m_eDevType, 
                    m_kWndFocus, m_uiBehaviorFlags, &kD3DPresentParams, 
                    &m_pkD3DDevice9);
                
                if (SUCCEEDED(eD3dRet))
                    break;

                m_bDeviceLost = false;

                // If the call failed and D3D changed the backbuffer count 
                // for us in the creation call, then we should try again with
                // the new (valid) backbuffer count
                if (uiBackBufferCount != kD3DPresentParams.BackBufferCount)
                {
                    Warning("NiDX9Renderer::Initialize> Specified number of "
                        "backbuffers was invalid - trying fewer "
                        "backbuffers\n");

                    eD3dRet = ms_pkD3D9->CreateDevice(m_uiAdapter, m_eDevType,
                        m_kWndFocus, m_uiBehaviorFlags, &kD3DPresentParams, 
                        &m_pkD3DDevice9);

                    if (SUCCEEDED(eD3dRet))
                        break;
                }
            }
        }

        if (eDesc == DEVDESC_REF)
        {
            SetLastErrorString("Creation failed: Could not create reference "
                "device");
            Error("NiDX9Renderer::Initialize> Could not create requested "
                "reference device - FAILING\n");

            return false;
        }

        // Try the next "lower" device type
        // this is safe, as the ExpandDeviceType call above will catch
        // bogus values and simply exit
        DeviceDesc ePrevDevice = eDesc;

        eDesc = (DeviceDesc)(((unsigned int)eDesc) + 1);
        
        // We do _not_ fall back to the reference device automatically -
        // an app must request it specifically
        if (eDesc == DEVDESC_REF)
        {
            // do not change error string - earlier functions set better
            // error messages
            Error("NiDX9Renderer::Initialize> Could not create hardware "
                "device of any type - FAILING\n");

            return false;
        }

        Warning("NiDX9Renderer::Initialize> Could not create device of "
            "desired type - trying next-lower device type\n");

        if (!ExpandDeviceType(eDesc, m_eDevType, m_uiBehaviorFlags))
        {
            SetLastErrorString("Creation failed: Invalid 3D device type");
            Error("NiDX9Renderer::Initialize> Invalid device ID specified - "
                "FAILING\n");

            return false;
        }
    }

    if (!m_pkD3DDevice9)
    {
        SetLastErrorString("Creation failed: Could not create hardware "
            "device");
        Error("NiDX9Renderer::Initialize> Could not create device of any "
            "type - FAILING\n");

        return false;
    }

#if (defined(_DEBUG) && defined(DEBUG_INTERFACE_POINTERS))
    g_pkD3DDevice = m_pkD3DDevice9;
#endif  //#if (defined(_DEBUG) && defined(DEBUG_INTERFACE_POINTERS))

    if (!InitializeDeviceCaps(kD3DPresentParams))
    {
        // do not set error string - InitializeDeviceCaps will set it
        return false;
    }

    // Store info about software vertex processing
    m_bSWVertexCapable = 
        ((m_uiBehaviorFlags & D3DCREATE_HARDWARE_VERTEXPROCESSING) == 0);
    m_bSWVertexSwitchable = 
        ((m_uiBehaviorFlags & D3DCREATE_MIXED_VERTEXPROCESSING) != 0);

    // Add info to the current onscreen render target
    Ni2DBuffer* pkBackBuffer = NULL;
    NiDepthStencilBuffer* pkDepthBuffer = NULL;
    NiDX9ImplicitBufferData* pkBackData = 
        NiDX9ImplicitBufferData::Create(
        m_pkD3DDevice9, kD3DPresentParams, pkBackBuffer);
    NiDX9ImplicitDepthStencilBufferData* pkDepthData = 
        NiDX9ImplicitDepthStencilBufferData::Create(
        m_pkD3DDevice9, pkDepthBuffer);

    m_spDefaultRenderTargetGroup = NiRenderTargetGroup::Create(1, this);
    m_spDefaultRenderTargetGroup->AttachBuffer(pkBackBuffer, 0);
    m_spDefaultRenderTargetGroup->AttachDepthStencilBuffer(pkDepthBuffer);

    m_kOnscreenRenderTargetGroups.SetAt(kWndDevice,
        m_spDefaultRenderTargetGroup);

    m_eAdapterFormat = GetCompatibleAdapterFormat(GetNiFBFormat((D3DFORMAT)
        m_spDefaultRenderTargetGroup->GetPixelFormat(0)->GetRendererHint()));

    if (!InitializeTextureDefaults())
    {
        // do not set error string - InitializeTextureDefaults will set it
        Error("NiDX9Renderer::Initialize> Could not initialize texture "
            "defaults - FAILING\n");

        return false;
    }

    // Create render state manager
    m_pkRenderState = NiDX9RenderState::Create(this, m_kD3DCaps9, true);

    // Inform the shadowing system of the shader model versions used by the 
    // current hardware.
    if (NiShadowManager::GetShadowManager())
    {
        NiShadowManager::ValidateShaderVersions(
            D3DSHADER_VERSION_MAJOR(m_uiMaxVertexShaderVersion),
            D3DSHADER_VERSION_MINOR(m_uiMaxVertexShaderVersion),
            0, 0,
            D3DSHADER_VERSION_MAJOR(m_uiMaxPixelShaderVersion),
            D3DSHADER_VERSION_MINOR(m_uiMaxPixelShaderVersion));
    }

    // Initialize the shader system
    NiD3DShaderInterface::InitializeShaderSystem(this);

    NIASSERT(NiD3DShaderFactory::GetD3DShaderFactory());
    NiD3DShaderFactory::GetD3DShaderFactory()->SetAsActiveFactory();

    m_spLegacyDefaultShader = NiNew NiD3DDefaultShader();
    NIASSERT(m_spLegacyDefaultShader != NULL);
    m_spLegacyDefaultShader->Initialize();
    m_spLegacyDefaultShaderMaterial = NiSingleShaderMaterial::Create(
        m_spLegacyDefaultShader, true);
    if (m_spLegacyDefaultShaderMaterial && NiIsKindOf(NiSingleShaderMaterial,
        m_spLegacyDefaultShaderMaterial))
    {
        NiSmartPointerCast(NiSingleShaderMaterial,
            m_spLegacyDefaultShaderMaterial)->
            SetCachedShader(m_spLegacyDefaultShader);
    }

    //  Create the vertex buffer manager
    m_pkVBManager = NiNew NiDX9VertexBufferManager(m_pkD3DDevice9);
    NIASSERT(m_pkVBManager);

    //  Create the index buffer manager
    m_pkIBManager = NiNew NiDX9IndexBufferManager(m_pkD3DDevice9);
    NIASSERT(m_pkIBManager);

    //  Create the texture manager
    m_pkTextureManager = NiNew NiDX9TextureManager(this);
    NIASSERT(m_pkTextureManager);

    NIASSERT(pkBackData);

    // Create NiGeometryGroupManager
    m_pkGeometryGroupManager = NiD3DGeometryGroupManager::Create(
        m_pkD3DDevice9, m_pkVBManager);
    m_pkGeometryGroupManager->SetHWSkinning(
        (GetFlags() & CAPS_HARDWARESKINNING) != 0);

    // Create default groups
    m_pkDefaultGroup = (NiUnsharedGeometryGroup*)m_pkGeometryGroupManager->
        CreateGroup(NiGeometryGroupManager::UNSHARED);
    m_pkDynamicGroup = (NiDynamicGeometryGroup*)m_pkGeometryGroupManager->
        CreateGroup(NiGeometryGroupManager::DYNAMIC);

    // Lights initialization
    m_pkLightManager = 
        NiNew NiDX9LightManager(m_pkRenderState, m_pkD3DDevice9);

    if (pkBackData->GetPresentParams().MultiSampleType != 
        D3DMULTISAMPLE_NONE)
    {
        m_pkRenderState->SetAntiAlias(true);
    }

    // Camera initialization
    m_pkCurrRenderTargetGroup = m_spDefaultRenderTargetGroup;
    NiFrustum kFrust;
    kFrust.m_fLeft = kFrust.m_fBottom = -1.0f;
    kFrust.m_fRight = kFrust.m_fTop = 1.0f;
    kFrust.m_fNear = 0.1f;
    kFrust.m_fFar = 1.0f;

    Do_SetCameraData(NiPoint3::ZERO, NiPoint3::UNIT_X, NiPoint3::UNIT_Y, 
        NiPoint3::UNIT_Z, kFrust, NiRect<float>(0.0f, 1.0f, 1.0f, 0.0f));
    m_pkCurrRenderTargetGroup = NULL;
    m_pkCurrOnscreenRenderTargetGroup = NULL;

    NiD3DShaderFactory* pkFactory = 
        (NiD3DShaderFactory*)NiShaderFactory::GetInstance();

    if (m_uiMaxPixelShaderVersion >=
        pkFactory->CreatePixelShaderVersion(2, 0) &&
        m_uiMaxVertexShaderVersion >=
        pkFactory->CreateVertexShaderVersion(2, 0))
    {
        NiStandardMaterial* pkStandardMaterial = NiDynamicCast(
            NiStandardMaterial, m_spInitialDefaultMaterial);
        if (pkStandardMaterial)
        {
            ((NiRenderer*)this)->SetDefaultProgramCache(pkStandardMaterial);
        }

        if ( NiShadowManager::GetShadowManager())
        {
            NiDirectionalShadowWriteMaterial* pkDirShadowWriteMaterial = 
                (NiDirectionalShadowWriteMaterial*)NiShadowManager::
                GetShadowWriteMaterial(NiStandardMaterial::LIGHT_DIR);

            if (pkDirShadowWriteMaterial)
            {
                ((NiRenderer*)this)->SetDefaultProgramCache(
                    pkDirShadowWriteMaterial);
            }

            NiPointShadowWriteMaterial* pkPointShadowWriteMaterial = 
                (NiPointShadowWriteMaterial*)NiShadowManager::
                GetShadowWriteMaterial(NiStandardMaterial::LIGHT_POINT);

            if (pkPointShadowWriteMaterial)
            {
                ((NiRenderer*)this)->SetDefaultProgramCache(
                    pkPointShadowWriteMaterial);
            }

            NiSpotShadowWriteMaterial* pkSpotShadowWriteMaterial = 
                (NiSpotShadowWriteMaterial*)NiShadowManager::
                GetShadowWriteMaterial(NiStandardMaterial::LIGHT_SPOT);

            if (pkSpotShadowWriteMaterial)
            {
                ((NiRenderer*)this)->SetDefaultProgramCache(
                    pkSpotShadowWriteMaterial);
            }
        }
    }
    else
    {
        m_spInitialDefaultMaterial = m_spLegacyDefaultShaderMaterial;
    }

    m_spCurrentDefaultMaterial = m_spInitialDefaultMaterial;

    NiShader* pkErrorShader = NiNew NiDX9ErrorShader;
    pkErrorShader->Initialize();

    SetErrorShader(pkErrorShader);

    return true;
}
//---------------------------------------------------------------------------
void NiDX9Renderer::SetDefaultProgramCache(NiFragmentMaterial* pkMaterial, 
    bool bAutoWriteToDisk, bool bWriteDebugFile, bool bLoad,
    bool bNoNewProgramCreation, const char* pcWorkingDir)
{
    const char* pcVSProfile = 
        D3DXGetVertexShaderProfile(m_pkD3DDevice9);
    const char* pcPSProfile = 
        D3DXGetPixelShaderProfile(m_pkD3DDevice9);

    if (pcVSProfile == NULL || pcPSProfile == NULL)
        return;

    // Check for ps_3_0/vs_3_0 mismatch - technically it's allowed in some
    // circumstances with specific restrictions, but we don't follow those
    // restrictions in NiStandardMaterial, so instead we just fall back
    if (strcmp(pcVSProfile, "vs_3_0") == 0 && 
        strcmp(pcPSProfile, "ps_3_0") != 0)
    {
        pcVSProfile = "vs_2_a";
    }

    NIASSERT(
        (strcmp(pcVSProfile, "vs_3_0") == 0 && 
        strcmp(pcPSProfile, "ps_3_0") == 0) || 
        (strcmp(pcVSProfile, "vs_3_0") != 0 && 
        strcmp(pcPSProfile, "ps_3_0") != 0));

    if (pcWorkingDir == NULL)
        pcWorkingDir = NiMaterial::GetDefaultWorkingDirectory();

    NiGPUProgram::ProgramType eType = NiGPUProgram::PROGRAM_VERTEX;
    NiD3DGPUProgramCache* pkCache = 
        NiNew NiD3DGPUProgramCache(
        pkMaterial->GetProgramVersion(eType),
        pcWorkingDir, eType, pcVSProfile, 
        pkMaterial->GetName(), bAutoWriteToDisk,
        bWriteDebugFile, bNoNewProgramCreation, bLoad);
    pkMaterial->SetProgramCache(pkCache, eType);

    eType = NiGPUProgram::PROGRAM_PIXEL;
    pkCache = 
        NiNew NiD3DGPUProgramCache(
        pkMaterial->GetProgramVersion(eType),
        pcWorkingDir, eType, pcPSProfile, 
        pkMaterial->GetName(), bAutoWriteToDisk,
        bWriteDebugFile, bNoNewProgramCreation, bLoad);
    pkMaterial->SetProgramCache(pkCache, eType);
}
//---------------------------------------------------------------------------
bool NiDX9Renderer::Recreate()
{
    // Release non-managed resources
    m_pkLightManager->ReleaseLights();

    m_pkIBManager->DestroyDefaultBuffers();
    m_pkVBManager->DestroyDefaultBuffers();

    NiDX92DBufferData::ClearRenderTarget(m_pkD3DDevice9, 1);
    NiDX92DBufferData::ClearRenderTarget(m_pkD3DDevice9, 2);
    NiDX92DBufferData::ClearRenderTarget(m_pkD3DDevice9, 3);

    NiTMapIterator pkIter = m_kOnscreenRenderTargetGroups.GetFirstPos();
    while (pkIter)
    {
        NiWindowRef kHWND;
        NiRenderTargetGroupPtr spRenderTargetGroup;
        const Ni2DBuffer* pkBuffer;
        NiDX9OnscreenBufferData* pkTarget;
        NiDX9DepthStencilBufferData* pkDSTarget;

        m_kOnscreenRenderTargetGroups.GetNext(pkIter, kHWND, 
            spRenderTargetGroup);
        NIASSERT(spRenderTargetGroup);

        pkBuffer = spRenderTargetGroup->GetBuffer(0);
        NIASSERT(pkBuffer);

        pkTarget = NiDynamicCast(NiDX9OnscreenBufferData, 
            (NiDX92DBufferData*)pkBuffer->GetRendererData());

        NIASSERT(pkTarget);
        pkTarget->Shutdown();

        pkBuffer = spRenderTargetGroup->GetDepthStencilBuffer();
        if (pkBuffer)
        {
            pkDSTarget = NiDynamicCast(NiDX9DepthStencilBufferData, 
                (NiDX92DBufferData*)pkBuffer->GetRendererData());

            if (pkDSTarget)
                pkDSTarget->Shutdown();
        }
    }

    NiTMapIterator kPos = m_kRenderedTextures.GetFirstPos();
    while (kPos)
    {
        NiRenderedTexture* pkRenderedTex;
        NiDX9RenderedTextureData* pkTexData;
        m_kRenderedTextures.GetNext(kPos, pkRenderedTex, pkTexData);

        PurgeTexture(pkRenderedTex);

        m_kRenderedTextures.SetAt(pkRenderedTex, 0);
    }

    kPos = m_kRenderedCubeMaps.GetFirstPos();
    while (kPos)
    {
        NiRenderedCubeMap* pkRenderedCubeMap;
        NiDX9RenderedCubeMapData* pkCubeMapData;
        m_kRenderedCubeMaps.GetNext(kPos, pkRenderedCubeMap, pkCubeMapData);
        
        PurgeTexture(pkRenderedCubeMap);

        m_kRenderedCubeMaps.SetAt(pkRenderedCubeMap, 0);
    }

    kPos = m_kDynamicTextures.GetFirstPos();
    while (kPos)
    {
        NiDynamicTexture* pkDynTxtr;
        NiDX9DynamicTextureData* pkDynTxtrData;
        m_kDynamicTextures.GetNext(kPos, pkDynTxtr, pkDynTxtrData);

        pkDynTxtr->SetRendererData(NULL);
        NiDelete pkDynTxtrData;

        m_kDynamicTextures.SetAt(pkDynTxtr, 0);
    }

    NiDX9AdditionalDepthStencilBufferData::ShutdownAll();

    NiTListIterator kShaderPos = m_kD3DShaders.GetHeadPos();
    while (kShaderPos)
    {
        NiD3DShaderInterface* pkD3DShader = 
            m_kD3DShaders.GetNext(kShaderPos);
        pkD3DShader->HandleLostDevice();
    }

    m_pkDynamicGroup->Purge();

    // Notify any application-set functions about device reset
    unsigned int uiFuncCount = m_kResetNotifyFuncs.GetSize();
    unsigned int i = 0;
    for (; i < uiFuncCount; i++)
    {
        RESETNOTIFYFUNC pfnFunc = m_kResetNotifyFuncs.GetAt(i);
        void* pvData = m_kResetNotifyFuncData.GetAt(i);
        if (pfnFunc)
        {
            bool bResult = (*pfnFunc)(true, pvData);

            if (bResult == false)
            {
                Error("NiDX9Renderer::Recreate> Reset notification function "
                    "failed before device reset - FAILING\n");
                return false;
            }
        }
    }

    const Ni2DBuffer* pkBuffer = m_spDefaultRenderTargetGroup->GetBuffer(0);
    NiDX9OnscreenBufferData* pkOnscreenBuffData = NiDynamicCast(
        NiDX9OnscreenBufferData, (NiDX92DBufferData*)
        pkBuffer->GetRendererData());
    NIASSERT(pkOnscreenBuffData);

    HRESULT eD3dRet = m_pkD3DDevice9->Reset((D3DPRESENT_PARAMETERS*)
        &pkOnscreenBuffData->GetPresentParams());
    
    if (FAILED(eD3dRet))
        return false;

    m_uiResetCounter++;
    m_bDeviceLost = false;

    // Re-create NiDX9Renderer settings

    // Recreate all of the onscreen buffers - be careful when dealing with
    // the current onscreen buffer, as it will already have been created
    // again when the device
    pkIter = m_kOnscreenRenderTargetGroups.GetFirstPos();
    while (pkIter)
    {
        NiWindowRef kHWND;
        NiRenderTargetGroupPtr spRenderTargetGroup;
        const Ni2DBuffer* pkBuffer;
        NiDX9OnscreenBufferData* pkTarget;
        NiDX9DepthStencilBufferData* pkDSTarget;

        m_kOnscreenRenderTargetGroups.GetNext(pkIter, kHWND, 
            spRenderTargetGroup);
        NIASSERT(spRenderTargetGroup);

        pkBuffer = spRenderTargetGroup->GetBuffer(0);
        NIASSERT(pkBuffer);

        pkTarget = NiDynamicCast(NiDX9OnscreenBufferData,
            (NiDX92DBufferData*)pkBuffer->GetRendererData());

        NIASSERT(pkTarget);
        // This call may fail if the window is iconified, etc
        pkTarget->Recreate(m_pkD3DDevice9);

        pkDSTarget = NiDynamicCast(NiDX9DepthStencilBufferData,
            (NiDX92DBufferData*) 
            spRenderTargetGroup->GetDepthStencilBufferRendererData());
        if (pkDSTarget)
            pkDSTarget->Recreate(m_pkD3DDevice9);
    }

    if (!InitializeTextureDefaults())
    {
        // do not set error string - InitializeTextureDefaults will set it
        Error("NiDX9Renderer::Recreate> Could not initialize texture defaults "
            "- FAILING\n");
        return false;
    }

    kPos = m_kRenderedTextures.GetFirstPos();
    while (kPos)
    {
        NiRenderedTexture* pkRenderedTex;
        NiDX9RenderedTextureData* pkTexData;
        m_kRenderedTextures.GetNext(kPos, pkRenderedTex, pkTexData);

        CreateRenderedTextureRendererData(pkRenderedTex);
    }

    kPos = m_kRenderedCubeMaps.GetFirstPos();
    while (kPos)
    {
        NiRenderedCubeMap* pkRenderedCubeMap;
        NiDX9RenderedCubeMapData* pkCubeMapData;
        m_kRenderedCubeMaps.GetNext(kPos, pkRenderedCubeMap, pkCubeMapData);

        CreateRenderedCubeMapRendererData(pkRenderedCubeMap);
    }

    kPos = m_kDynamicTextures.GetFirstPos();
    while (kPos)
    {
        NiDynamicTexture* pkDynTxtr;
        NiDX9DynamicTextureData* pkDynTxtrData;
        m_kDynamicTextures.GetNext(kPos, pkDynTxtr, pkDynTxtrData);

        CreateDynamicTextureRendererData(pkDynTxtr);
    }

    NiDX9AdditionalDepthStencilBufferData::RecreateAll(m_pkD3DDevice9);

    kShaderPos = m_kD3DShaders.GetHeadPos();
    while (kShaderPos)
    {
        NiD3DShaderInterface* pkD3DShader = 
            m_kD3DShaders.GetNext(kShaderPos);
        pkD3DShader->HandleResetDevice();
    }

    m_pkRenderState->InitRenderModes();
    m_pkLightManager->InitRenderModes();

    // Notify any application-set functions about device reset
    for (i = 0; i < uiFuncCount; i++)
    {
        RESETNOTIFYFUNC pfnFunc = m_kResetNotifyFuncs.GetAt(i);
        void* pvData = m_kResetNotifyFuncData.GetAt(i);
        if (pfnFunc)
        {
            bool bResult = (*pfnFunc)(false, pvData);

            if (bResult == false)
            {
                Error("NiDX9Renderer::Recreate> Reset notification function "
                    "failed after device reset - FAILING\n");
                return false;
            }
        }
    }

    // Notify the shadow manager about the device reset
    if (NiShadowManager::GetShadowManager())
    {
        NiShadowManager::RendererRecreated();
    }

    return true;
}
//---------------------------------------------------------------------------
NiDX9Renderer::RecreateStatus NiDX9Renderer::Recreate(unsigned int uiWidth, 
    unsigned int uiHeight, unsigned int uiUseFlags, NiWindowRef kWndDevice, 
    FrameBufferFormat eFBFormat, DepthStencilFormat eDSFormat, 
    PresentationInterval ePresentationInterval, SwapEffect eSwapEffect, 
    unsigned int uiFBMode, unsigned int uiBackBufferCount, 
    unsigned int uiRefreshRate)
{
    const Ni2DBuffer* pkBuffer = m_spDefaultRenderTargetGroup->GetBuffer(0);
    NiDX9OnscreenBufferData* pkOnscreenBuffData = NiDynamicCast(
        NiDX9OnscreenBufferData, (NiDX92DBufferData*)
        pkBuffer->GetRendererData());
    NIASSERT(pkOnscreenBuffData);

    D3DPRESENT_PARAMETERS& kPrimaryPresent = 
        pkOnscreenBuffData->GetPresentParams();
    D3DPRESENT_PARAMETERS kTempBackup;
    NiMemcpy(&kTempBackup, &kPrimaryPresent, sizeof(kTempBackup));

    // Get present parameters
    if (!InitializePresentParams(kWndDevice, uiWidth, uiHeight, uiUseFlags, 
        uiFBMode, eFBFormat, eDSFormat, uiBackBufferCount, eSwapEffect, 
        uiRefreshRate, ePresentationInterval, kPrimaryPresent))
    {
        NiMemcpy(&kPrimaryPresent, sizeof(kPrimaryPresent), &kTempBackup,
            sizeof(kTempBackup));
        return RECREATESTATUS_FAILED;
    }

    if (kWndDevice)
    {
        kPrimaryPresent.hDeviceWindow = kWndDevice;
    }

    if (!Recreate())
    {
        Warning("NiDX9Renderer::Recreate> Could not reset device to "
            "desired parameters - trying to recover original values\n");
        NiMemcpy(&kPrimaryPresent, sizeof(kPrimaryPresent), &kTempBackup,
            sizeof(kTempBackup));
        if (!Recreate())
        {
            Error("NiDX9Renderer::Recreate> Could not recover original "
                "presentation parameters\n");

            return RECREATESTATUS_FAILED;
        }
        else
        {
            return RECREATESTATUS_RESTORED;
        }
    }

    return RECREATESTATUS_OK;
}
//---------------------------------------------------------------------------
NiDX9Renderer::RecreateStatus NiDX9Renderer::Recreate(unsigned int uiWidth, 
    unsigned int uiHeight)
{
    const Ni2DBuffer* pkBuffer = m_spDefaultRenderTargetGroup->GetBuffer(0);
    NiDX9OnscreenBufferData* pkOnscreenBuffData = NiDynamicCast(
        NiDX9OnscreenBufferData, (NiDX92DBufferData*)
        pkBuffer->GetRendererData());
    NIASSERT(pkOnscreenBuffData);

    D3DPRESENT_PARAMETERS& kPrimaryPresent = 
        pkOnscreenBuffData->GetPresentParams();
    D3DPRESENT_PARAMETERS kTempBackup;
    NiMemcpy(&kTempBackup, sizeof(kTempBackup), &kPrimaryPresent, 
        sizeof(kPrimaryPresent));

    // Change just width and height
    kPrimaryPresent.BackBufferWidth = uiWidth;
    kPrimaryPresent.BackBufferHeight = uiHeight;

    if (!Recreate())
    {
        Warning("NiDX9Renderer::Recreate> Could not reset device to "
            "desired parameters - trying to recover original values\n");
        NiMemcpy(&kPrimaryPresent, sizeof(kPrimaryPresent), &kTempBackup, 
            sizeof(kTempBackup));
        if (!Recreate())
        {
            Error("NiDX9Renderer::Recreate> Could not recover original "
                "presentation parameters\n");

            return RECREATESTATUS_FAILED;
        }
        else
        {
            return RECREATESTATUS_RESTORED;
        }
    }

    return RECREATESTATUS_OK;
}
//---------------------------------------------------------------------------
void NiDX9Renderer::GetCreationParameters(unsigned int& uiWidth, 
    unsigned int& uiHeight, unsigned int& uiUseFlags, NiWindowRef& kWndDevice, 
    NiWindowRef& kWndFocus, unsigned int& uiAdapter, DeviceDesc& eDesc, 
    FrameBufferFormat& eFBFormat, DepthStencilFormat& eDSFormat, 
    PresentationInterval& ePresentationInterval, SwapEffect& eSwapEffect,
    unsigned int& uiFBMode, unsigned int& uiBackBufferCount, 
    unsigned int& uiRefreshRate) const
{
    uiWidth = m_uiCreationWidth;
    uiHeight = m_uiCreationHeight;
    uiUseFlags = m_uiCreationUseFlags;
    kWndDevice = m_kCreationWndDevice;
    kWndFocus = m_kCreationWndFocus;
    uiAdapter = m_uiCreationAdapter;
    eDesc = m_eCreationDesc;
    eFBFormat = m_eCreationFBFormat;
    eDSFormat = m_eCreationDSFormat;
    ePresentationInterval = m_eCreationPresentationInterval;
    eSwapEffect = m_eCreationSwapEffect;
    uiFBMode = m_uiCreationFBMode;
    uiBackBufferCount = m_uiCreationBackBufferCount;
    uiRefreshRate = m_uiCreationRefreshRate;
}
//---------------------------------------------------------------------------
bool NiDX9Renderer::CreateSwapChainRenderTargetGroup(unsigned int uiUseFlags,
    NiWindowRef kWndDevice, FrameBufferFormat eFBFormat, 
    DepthStencilFormat eDSFormat, PresentationInterval ePresentationInterval,
    SwapEffect eSwapEffect, unsigned int uiFBMode, 
    unsigned int uiBackBufferCount, unsigned int uiRefreshRate)
{
    // Does the framebuffer already exist?
    if (!kWndDevice)
    {
        return true;
    }
    else
    {
        NiRenderTargetGroupPtr spRenderTargetGroup;
        
        // Does the framebuffer exist?
        if (m_kOnscreenRenderTargetGroups.GetAt(kWndDevice, 
            spRenderTargetGroup))
        {
            return true;
        }
    }

    D3DPRESENT_PARAMETERS kD3DPresentParams;

    if (!InitializePresentParams(kWndDevice, 0, 0, uiUseFlags, 
        uiFBMode, eFBFormat, eDSFormat, uiBackBufferCount, 
        eSwapEffect, uiRefreshRate, ePresentationInterval,
        kD3DPresentParams))
    {
        return false;
    }

    Ni2DBuffer* pkNewBuffer = NULL;
    NiDX9SwapChainBufferData* pkNewTarget = 
        NiDX9SwapChainBufferData::Create(m_pkD3DDevice9, 
        kD3DPresentParams, pkNewBuffer);

    if (pkNewTarget)
    {
        NiRenderTargetGroup* pkRenderTargetGroup = 
            NiRenderTargetGroup::Create(1, this);
        pkRenderTargetGroup->AttachBuffer(pkNewBuffer, 0);
        m_kOnscreenRenderTargetGroups.SetAt(kWndDevice, pkRenderTargetGroup);

        NiDepthStencilBuffer* pkDSBuffer = NULL;
        NiDX9SwapChainDepthStencilBufferData* pkNewDS = 
            NiDX9SwapChainDepthStencilBufferData::Create(m_pkD3DDevice9, 
            pkNewTarget, pkDSBuffer);

        if (pkDSBuffer)
        {
            pkRenderTargetGroup->AttachDepthStencilBuffer(pkDSBuffer);
        }
    
    }
    else
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
void NiDX9Renderer::DestroySwapChainRenderTargetGroup(NiWindowRef kWndDevice)
{
    // Does the framebuffer exist?
    if (!kWndDevice)
    {
        return;
    }
    else
    {
        NiRenderTargetGroupPtr spRenderTargetGroup = NULL;

        // Does the framebuffer exist?
        if (!m_kOnscreenRenderTargetGroups.GetAt(kWndDevice, 
            spRenderTargetGroup))
        {
            return;
        }

        // can only delete non-primary buffers that are not current targets
        if (spRenderTargetGroup != m_pkCurrOnscreenRenderTargetGroup)
        {
            if (spRenderTargetGroup != m_spDefaultRenderTargetGroup)
                m_kOnscreenRenderTargetGroups.RemoveAt(kWndDevice);
            spRenderTargetGroup = NULL;
        }
        else
        {
            Error("NiDX9Renderer::DestroyWindowedFramebuffer - destroying "
                "swap chain that is a current render target\n");
        }
    }
}
//---------------------------------------------------------------------------
NiRenderTargetGroup* NiDX9Renderer::GetSwapChainRenderTargetGroup(NiWindowRef 
    kWndDevice) const
{
    if (!kWndDevice)
    {
        return NULL;
    }
    else
    {
        NiRenderTargetGroupPtr spRenderTargetGroup = NULL;

        // Does the framebuffer exist?
        if (m_kOnscreenRenderTargetGroups.GetAt(kWndDevice, 
            spRenderTargetGroup))
        {
            return spRenderTargetGroup;
        }
    }
    return NULL;
}
//---------------------------------------------------------------------------
bool NiDX9Renderer::InitializePresentParams(NiWindowRef kWndDevice,
    unsigned int uiWidth, unsigned int uiHeight, unsigned int uiUseFlags, 
    unsigned int uiFBMode, FrameBufferFormat eFBFormat, 
    DepthStencilFormat eDSFormat, unsigned int uiBackBufferCount, 
    SwapEffect eSwapEffect, unsigned int uiRefreshRate, 
    PresentationInterval ePresentationInterval, 
    D3DPRESENT_PARAMETERS& kD3DPresentParams) const
{
    // Assumes that the following members are valid at call time:
    // m_pkDeviceInfo
    // m_uiAdapter, m_eDevType
    // m_kWndDevice

    bool bWindowed = (uiUseFlags & USE_FULLSCREEN) ? false : true;

    const D3DCAPS9& kD3dCaps9 = m_pkDeviceDesc->GetCaps();

    unsigned int uiBitDepth = 32;
    if (uiUseFlags & USE_16BITBUFFERS)
        uiBitDepth = 16;

    // If eFBFormat is UNKNOWN, then the renderer must come up with the best
    // format, based on supported formats in the hardware
    if (eFBFormat == FBFMT_UNKNOWN)
    {
        eFBFormat = m_pkDeviceDesc->GetNearestFrameBufferFormat(bWindowed, 
            uiBitDepth);
    }

    if (eFBFormat == FBFMT_UNKNOWN)
    {
        SetLastErrorString("Creation failed: Could not find desired "
            "framebuffer format");
        Warning("NiDX9Renderer::Initialize> No matching "
            "framebuffer format available\n");

        return false;
    }

    D3DFORMAT eD3DFBFormat = GetD3DFormat(eFBFormat);
    D3DFORMAT eAdapterFormat = GetCompatibleAdapterFormat(eFBFormat);

    // If eDSFormat is UNKNOWN, then the renderer must come up with the best
    // format, based on supported formats in the hardware
    if (eDSFormat == DSFMT_UNKNOWN)
    {
        eDSFormat = m_pkDeviceDesc->GetNearestDepthStencilFormat(
            eAdapterFormat, eD3DFBFormat, 
            uiBitDepth, (uiUseFlags & USE_STENCIL) ? 8 : 0);
    }

    if (eDSFormat == DSFMT_UNKNOWN)
    {
        SetLastErrorString("Creation failed: Could not find desired "
            "depth/stencil format");
        Warning("NiDX9Renderer::Initialize> No matching "
            "depth/stencil format available\n");

        return false;
    }

    memset((void*)&kD3DPresentParams, 0, sizeof(kD3DPresentParams));

    kD3DPresentParams.BackBufferWidth = uiWidth;
    kD3DPresentParams.BackBufferHeight = uiHeight;

    kD3DPresentParams.BackBufferFormat = eD3DFBFormat;

    kD3DPresentParams.MultiSampleType = GetMultiSampleType(uiFBMode);
    kD3DPresentParams.MultiSampleQuality = GetMultiSampleQuality(uiFBMode);

    kD3DPresentParams.AutoDepthStencilFormat = GetD3DFormat(eDSFormat);
    
    kD3DPresentParams.EnableAutoDepthStencil = 
        (uiUseFlags & USE_MANUALDEPTHSTENCIL) ? false : true;
    
    kD3DPresentParams.BackBufferCount = uiBackBufferCount;

    kD3DPresentParams.Windowed = bWindowed;

    kD3DPresentParams.SwapEffect = GetSwapEffect(eSwapEffect, uiFBMode, 
        uiUseFlags);
    kD3DPresentParams.Flags = (uiFBMode == FBMODE_LOCKABLE) 
        ? D3DPRESENTFLAG_LOCKABLE_BACKBUFFER : 0;

    kD3DPresentParams.hDeviceWindow = kWndDevice;

    kD3DPresentParams.FullScreen_RefreshRateInHz = 
        GetRefreshRate(uiRefreshRate);
    kD3DPresentParams.PresentationInterval = 
        GetPresentInterval(ePresentationInterval);

    ///////////////////////
    // Multisample Validate
    DWORD uiQualityLevels;
    HRESULT eD3dRet = ms_pkD3D9->CheckDeviceMultiSampleType(m_uiAdapter,
        m_eDevType, kD3DPresentParams.BackBufferFormat,
        kD3DPresentParams.Windowed, kD3DPresentParams.MultiSampleType, 
        &uiQualityLevels);

    // If the selected mode can't be supported, default to NONE
    if (FAILED(eD3dRet))
    {
        Warning("NiDX9Renderer::Initialize> Could not support "
            "requested antialiasing mode - disabling\n");
        kD3DPresentParams.MultiSampleType = D3DMULTISAMPLE_NONE;
    }
    else if (uiQualityLevels <= kD3DPresentParams.MultiSampleQuality)
    {
        kD3DPresentParams.MultiSampleQuality = uiQualityLevels - 1;
    }

    ///////////////////////////////////
    // Backbuffer number validate
    // This cannot be fully validated until we create the device, but we'll
    // avoid at least one iteration by validating the basic range
    if (kD3DPresentParams.BackBufferCount == 0)
        kD3DPresentParams.BackBufferCount = 1;
    else if (kD3DPresentParams.BackBufferCount > 3)
        kD3DPresentParams.BackBufferCount = 3;
    
    unsigned int uiRequestedRefresh 
        = kD3DPresentParams.FullScreen_RefreshRateInHz;

    ///////////////////////////////////
    // Fullscreen display mode validate (including refresh rate)
    // Iterate over all modes, looking for a match - will return a
    // valid refresh rate if everything but the refresh match
    if (kD3DPresentParams.Windowed)
    {
        kD3DPresentParams.FullScreen_RefreshRateInHz = 0;
    }
    else
    {
        if (!m_pkAdapterDesc->ValidateFullscreenMode(
            GetCompatibleAdapterFormat(GetNiFBFormat(
            kD3DPresentParams.BackBufferFormat)),
            kD3DPresentParams.BackBufferWidth,
            kD3DPresentParams.BackBufferHeight, 
            kD3DPresentParams.FullScreen_RefreshRateInHz))
        {
            SetLastErrorString("Creation failed: Could not match desired "
                "fullscreen mode");
            Warning("NiDX9Renderer::Initialize> Could not find "
                "matching fullscreen display mode\n");

            return false;
        }

        if (uiRequestedRefresh 
            != kD3DPresentParams.FullScreen_RefreshRateInHz)
        {
            Warning("NiDX9Renderer::Initialize> Could not match "
                "requested fullscreen refresh rate - using closest\n");
        }
    }

    ////////////////////////////
    // Present Interval Validate
    if (kD3DPresentParams.Windowed)
    {
        if (kD3DPresentParams.PresentationInterval != 
            D3DPRESENT_INTERVAL_DEFAULT &&
            kD3DPresentParams.PresentationInterval != 
            D3DPRESENT_INTERVAL_IMMEDIATE &&
            kD3DPresentParams.PresentationInterval != 
            D3DPRESENT_INTERVAL_ONE)
        {
            kD3DPresentParams.PresentationInterval = 
                D3DPRESENT_INTERVAL_IMMEDIATE;
        }
    }
    else if (!(kD3DPresentParams.PresentationInterval &
        kD3dCaps9.PresentationIntervals))
    {
        kD3DPresentParams.PresentationInterval = D3DPRESENT_INTERVAL_ONE;
    }

    return true;
}
//---------------------------------------------------------------------------
bool NiDX9Renderer::InitializeTextureDefaults() 
{
    // Texture initialization.  Dynamic textures use Source Texture formats.
    CreatePixelFormatArray(m_aapkTextureFormats[TEXUSE_TEX], 0, 
        D3DRTYPE_TEXTURE);
    CreatePixelFormatArray(m_aapkTextureFormats[TEXUSE_RENDERED_TEX], 
        D3DUSAGE_RENDERTARGET, D3DRTYPE_TEXTURE);
    CreatePixelFormatArray(m_aapkTextureFormats[TEXUSE_CUBE], 0, 
        D3DRTYPE_CUBETEXTURE);
    CreatePixelFormatArray(m_aapkTextureFormats[TEXUSE_RENDERED_CUBE], 
        D3DUSAGE_RENDERTARGET, D3DRTYPE_CUBETEXTURE);
    GenerateDefaultTextureData();

    // Create default "clipper" texture
    NiPixelFormat* pkDefault = m_apkDefaultTextureFormat[TEXUSE_TEX];

    NiPixelDataPtr spRaw = NiNew NiPixelData(2, 1, *pkDefault);
    unsigned char* pkData = spRaw->GetPixels();
    if (pkDefault->GetBitsPerPixel() == 16)
    {
        pkData[0] = pkData[1] = 0;
        pkData[2] = pkData[3] = 255;
    }
    else if (pkDefault->GetBitsPerPixel() == 32)
    {
        pkData[0] = pkData[1] = pkData[2] = pkData[3] = 0;
        pkData[4] = pkData[5] = pkData[6] = pkData[7] = 255;
    }

    NiTexture::FormatPrefs kPrefs;
    kPrefs.m_eMipMapped = NiTexture::FormatPrefs::NO;
    kPrefs.m_eAlphaFmt = NiTexture::FormatPrefs::NONE;
    m_spClipperImage = NiSourceTexture::Create(spRaw, kPrefs);

    return true;
}
//---------------------------------------------------------------------------
bool NiDX9Renderer::InitializeDeviceCaps(
    const D3DPRESENT_PARAMETERS& kPresentParams) 
{
    // Device capability initialization
    HRESULT hr = m_pkD3DDevice9->GetDeviceCaps(&m_kD3DCaps9);

    if (FAILED(hr))
    {
        SetLastErrorString("Creation failed: Could not query device caps");
        Error("NiDX9Renderer::Initialize> Could not query device caps "
            "- FAILING\n");
     
        return false;
    }

    // The device must be able to handle 2-texture, 2-stage multitexture for
    // now, as we assume it.
    if ((m_kD3DCaps9.MaxTextureBlendStages < 2) || 
        (m_kD3DCaps9.MaxSimultaneousTextures < 2))
    {
        SetLastErrorString("Creation failed: Device does not support "
            "single-pass multi-texture");
        Error("NiDX9Renderer::Initialize> Device does not support single-pass "
            "multi-texture - FAILING\n");
     
        return false;
    }

    m_uiRendFlags = 0;

    if ((m_kD3DCaps9.TextureCaps & D3DPTEXTURECAPS_POW2) == 0)
        m_uiRendFlags |= CAPS_NONPOW2_TEXT;
    else if (m_kD3DCaps9.TextureCaps & D3DPTEXTURECAPS_NONPOW2CONDITIONAL)
        m_uiRendFlags |= CAPS_NONPOW2_CONDITIONAL_TEXT;

    m_uiHWBones = m_kD3DCaps9.MaxVertexBlendMatrices;

    // Software vertex processing allows 4 "HW" bones
    if (m_bSWVertexCapable && !m_bSWVertexSwitchable)
        m_uiHWBones = 4;

    if (m_uiHWBones >= HW_BONE_LIMIT)
        m_uiRendFlags |= CAPS_HARDWARESKINNING;

    m_uiMaxStreams = m_kD3DCaps9.MaxStreams;
    m_uiMaxPixelShaderVersion = m_kD3DCaps9.PixelShaderVersion;
    m_uiMaxVertexShaderVersion = m_kD3DCaps9.VertexShaderVersion;
    m_bMipmapCubeMaps = 
        (m_kD3DCaps9.TextureCaps & D3DPTEXTURECAPS_MIPCUBEMAP) != 0;
    m_bDynamicTexturesCapable =
        (m_kD3DCaps9.Caps2 & D3DCAPS2_DYNAMICTEXTURES) != 0;
    m_uiMaxNumRenderTargets = m_kD3DCaps9.NumSimultaneousRTs;
    m_bIndependentBitDepths = (m_kD3DCaps9.PrimitiveMiscCaps & 
        D3DPMISCCAPS_MRTINDEPENDENTBITDEPTHS) != 0;
    m_bMRTPostPixelShaderBlending = (m_kD3DCaps9.PrimitiveMiscCaps & 
        D3DPMISCCAPS_MRTPOSTPIXELSHADERBLENDING) != 0;

    // Setup the default shader version information
    m_kShaderLibraryVersion.SetSystemPixelShaderVersion(
        m_uiMaxPixelShaderVersion);
    m_kShaderLibraryVersion.SetMinPixelShaderVersion(0, 0);
    m_kShaderLibraryVersion.SetPixelShaderVersionRequest(
        m_uiMaxPixelShaderVersion);
    m_kShaderLibraryVersion.SetSystemVertexShaderVersion(
        m_uiMaxVertexShaderVersion);
    m_kShaderLibraryVersion.SetMinVertexShaderVersion(0, 0);
    m_kShaderLibraryVersion.SetVertexShaderVersionRequest(
        m_uiMaxVertexShaderVersion);
    m_kShaderLibraryVersion.SetSystemUserVersion(0, 0);
    m_kShaderLibraryVersion.SetMinUserVersion(0, 0);
    m_kShaderLibraryVersion.SetUserVersionRequest(0, 0);
    m_kShaderLibraryVersion.SetPlatform(NiShader::NISHADER_DX9);

    NiD3DPass::SetMaxTextureBlendStages(
        m_kD3DCaps9.MaxTextureBlendStages);
    NiD3DPass::SetMaxSimultaneousTextures(
        m_kD3DCaps9.MaxSimultaneousTextures);

    if (m_uiMaxPixelShaderVersion >= D3DPS_VERSION(2, 0))
        NiD3DPass::SetMaxSamplers(16);
    else
        NiD3DPass::SetMaxSamplers(m_kD3DCaps9.MaxTextureBlendStages);

    // Address mappings
    ms_akD3DAddressMapping[NiTexturingProperty::WRAP_S_WRAP_T].m_eU = 
        D3DTADDRESS_WRAP;
    ms_akD3DAddressMapping[NiTexturingProperty::WRAP_S_WRAP_T].m_eV = 
        D3DTADDRESS_WRAP;
    ms_akD3DAddressMapping[NiTexturingProperty::WRAP_S_CLAMP_T].m_eU = 
        D3DTADDRESS_WRAP;
    ms_akD3DAddressMapping[NiTexturingProperty::WRAP_S_CLAMP_T].m_eV = 
        D3DTADDRESS_CLAMP;
    ms_akD3DAddressMapping[NiTexturingProperty::CLAMP_S_WRAP_T].m_eU = 
        D3DTADDRESS_CLAMP;
    ms_akD3DAddressMapping[NiTexturingProperty::CLAMP_S_WRAP_T].m_eV = 
        D3DTADDRESS_WRAP;
    ms_akD3DAddressMapping[NiTexturingProperty::CLAMP_S_CLAMP_T].m_eU = 
        D3DTADDRESS_CLAMP;
    ms_akD3DAddressMapping[NiTexturingProperty::CLAMP_S_CLAMP_T].m_eV = 
        D3DTADDRESS_CLAMP;

    // FILTER_NEAREST
    ms_akD3DFilterMapping[NiTexturingProperty::FILTER_NEAREST].m_eMag = 
        D3DTEXF_POINT;
    ms_akD3DFilterMapping[NiTexturingProperty::FILTER_NEAREST].m_eMin = 
        D3DTEXF_POINT;
    ms_akD3DFilterMapping[NiTexturingProperty::FILTER_NEAREST].m_eMip = 
        D3DTEXF_NONE;
    //  FILTER_BILERP
    ms_akD3DFilterMapping[NiTexturingProperty::FILTER_BILERP].m_eMag = 
        D3DTEXF_LINEAR;
    ms_akD3DFilterMapping[NiTexturingProperty::FILTER_BILERP].m_eMin = 
        D3DTEXF_LINEAR;
    ms_akD3DFilterMapping[NiTexturingProperty::FILTER_BILERP].m_eMip = 
        D3DTEXF_NONE;
    //  FILTER_TRILERP
    ms_akD3DFilterMapping[NiTexturingProperty::FILTER_TRILERP].m_eMag = 
        D3DTEXF_LINEAR;
    ms_akD3DFilterMapping[NiTexturingProperty::FILTER_TRILERP].m_eMin = 
        D3DTEXF_LINEAR;
    ms_akD3DFilterMapping[NiTexturingProperty::FILTER_TRILERP].m_eMip = 
        D3DTEXF_LINEAR;
    //  FILTER_NEAREST_MIPNEAREST
    ms_akD3DFilterMapping[
        NiTexturingProperty::FILTER_NEAREST_MIPNEAREST].m_eMag = 
        D3DTEXF_POINT;
    ms_akD3DFilterMapping[
        NiTexturingProperty::FILTER_NEAREST_MIPNEAREST].m_eMin = 
        D3DTEXF_POINT;
    ms_akD3DFilterMapping[
        NiTexturingProperty::FILTER_NEAREST_MIPNEAREST].m_eMip = 
        D3DTEXF_POINT;
    //  FILTER_NEAREST_MIPLERP
    ms_akD3DFilterMapping[
        NiTexturingProperty::FILTER_NEAREST_MIPLERP].m_eMag = 
        D3DTEXF_POINT;
    ms_akD3DFilterMapping[
        NiTexturingProperty::FILTER_NEAREST_MIPLERP].m_eMin = 
        D3DTEXF_POINT;
    ms_akD3DFilterMapping[
        NiTexturingProperty::FILTER_NEAREST_MIPLERP].m_eMip = 
        D3DTEXF_LINEAR;
    //  FILTER_BILERP_MIPNEAREST
    ms_akD3DFilterMapping[
        NiTexturingProperty::FILTER_BILERP_MIPNEAREST].m_eMag = 
        D3DTEXF_LINEAR;
    ms_akD3DFilterMapping[
        NiTexturingProperty::FILTER_BILERP_MIPNEAREST].m_eMin = 
        D3DTEXF_LINEAR;
    ms_akD3DFilterMapping[
        NiTexturingProperty::FILTER_BILERP_MIPNEAREST].m_eMip = 
        D3DTEXF_POINT;

    // Projected
    if (m_kD3DCaps9.TextureCaps & D3DPTEXTURECAPS_PROJECTED)
    {
        ms_uiProjectedTextureFlags = D3DTTFF_COUNT3 | D3DTTFF_PROJECTED;
    }
    else
    {
        ms_uiProjectedTextureFlags = D3DTTFF_COUNT2;
    }

    return true;
}
//---------------------------------------------------------------------------
bool NiDX9Renderer::ExpandDeviceType(DeviceDesc eDesc, D3DDEVTYPE& eDevType,
    unsigned int& uiBehaviorFlags)
{
    // Remove VP/pure flags from uiBehaviorFlags
    uiBehaviorFlags &= ~(D3DCREATE_HARDWARE_VERTEXPROCESSING | 
        D3DCREATE_MIXED_VERTEXPROCESSING |
        D3DCREATE_SOFTWARE_VERTEXPROCESSING | D3DCREATE_PUREDEVICE);

    switch (eDesc)
    {
        case DEVDESC_PURE:
            eDevType = D3DDEVTYPE_HAL;
            uiBehaviorFlags |= D3DCREATE_HARDWARE_VERTEXPROCESSING |
                D3DCREATE_PUREDEVICE;
            break;
        case DEVDESC_HAL_HWVERTEX:
            eDevType = D3DDEVTYPE_HAL;
            uiBehaviorFlags |= D3DCREATE_HARDWARE_VERTEXPROCESSING;
            break;
        case DEVDESC_HAL_MIXEDVERTEX:
            eDevType = D3DDEVTYPE_HAL;
            uiBehaviorFlags |= D3DCREATE_MIXED_VERTEXPROCESSING;
            break;
        case DEVDESC_HAL_SWVERTEX:
            eDevType = D3DDEVTYPE_HAL;
            uiBehaviorFlags |= D3DCREATE_SOFTWARE_VERTEXPROCESSING;
            break;
        case DEVDESC_REF_HWVERTEX:
            eDevType = D3DDEVTYPE_REF;
            uiBehaviorFlags |= D3DCREATE_HARDWARE_VERTEXPROCESSING;
            break;
        case DEVDESC_REF_MIXEDVERTEX:
            eDevType = D3DDEVTYPE_REF;
            uiBehaviorFlags |= D3DCREATE_MIXED_VERTEXPROCESSING;
            break;
        case DEVDESC_REF:
            eDevType = D3DDEVTYPE_REF;
            uiBehaviorFlags |= D3DCREATE_SOFTWARE_VERTEXPROCESSING;
            break;
        default:
            return false;
    };

    return true;
}
//---------------------------------------------------------------------------
bool NiDX9Renderer::GenerateDefaultTextureData()
{
    const unsigned int uiDefaultFormatNum = 6;
    TexFormat aeFormats[uiDefaultFormatNum] =
    {
        TEX_RGB565,
        TEX_RGB555,
        TEX_RGB888,
        TEX_RGBA4444,
        TEX_RGBA5551,
        TEX_RGBA8888
    };

    unsigned int uiFmtIndex, uiUsage;
    for (uiUsage = 0; uiUsage < TEXUSE_NUM; uiUsage++)
    {
        m_apkDefaultTextureFormat[uiUsage] = NULL;

        for (uiFmtIndex = 0; uiFmtIndex < uiDefaultFormatNum; uiFmtIndex++)
        {
            TexFormat eFmt = aeFormats[uiFmtIndex];

            if (m_aapkTextureFormats[uiUsage][eFmt])
            {
                m_apkDefaultTextureFormat[uiUsage] = 
                    m_aapkTextureFormats[uiUsage][eFmt];
                break;
            }
        }

        if (m_apkDefaultTextureFormat[uiUsage])
        {
            m_aspDefaultTextureData[uiUsage] = NiNew NiPixelData(4, 4, 
                *m_apkDefaultTextureFormat[uiUsage], 1);

            unsigned char* pkPixels = 
                m_aspDefaultTextureData[uiUsage]->GetPixels();
            memset(pkPixels, 0xff, 
                4 * 4 * m_aspDefaultTextureData[uiUsage]->GetPixelStride());
        }
    }

    // Must find a nonrendered, flat texture format (other types such
    // as cubes and rendered textures are optional)
    if (!m_apkDefaultTextureFormat[TEXUSE_TEX])
        return false;

    NiTexture::RendererData::SetTextureReplacementFormat(
        *m_apkDefaultTextureFormat[TEXUSE_TEX]);
    m_eReplacementDataFormat = (TexFormat)
        m_apkDefaultTextureFormat[TEXUSE_TEX]->GetExtraData();

    return true;
}
//---------------------------------------------------------------------------
const char* NiDX9Renderer::GetDriverInfo() const
{
    D3DADAPTER_IDENTIFIER9 kD3DAdapterIdentifier9;

    m_acDriverDesc[0] = 0;
    memset((void*)&kD3DAdapterIdentifier9, 0, sizeof(kD3DAdapterIdentifier9));

    if (SUCCEEDED(ms_pkD3D9->GetAdapterIdentifier(m_uiAdapter,
        0, &kD3DAdapterIdentifier9)))
    {
        NiSprintf(m_acDriverDesc, 512, "%s (%s-%s)", 
            kD3DAdapterIdentifier9.Description, GetDevTypeString(), 
            GetBehaviorString());
    }

    return m_acDriverDesc;
}
//---------------------------------------------------------------------------
const NiPixelFormat* NiDX9Renderer::FindClosestPixelFormat(
    NiTexture::FormatPrefs& kFmtPrefs) const
{
    return NiDX9TextureData::FindClosestPixelFormat(kFmtPrefs,
        m_aapkTextureFormats[TEXUSE_TEX]);
}
//---------------------------------------------------------------------------
const NiPixelFormat* NiDX9Renderer::FindClosestPixelFormat(
    TexFormat eFormat, TexUsage eUsage) const
{
    NIASSERT(m_aapkTextureFormats != NULL);
    NIASSERT(TEX_NUM > eFormat);
    NIASSERT(TEXUSE_NUM > eUsage);
    return m_aapkTextureFormats[eUsage][eFormat];
}
//---------------------------------------------------------------------------
const NiPixelFormat* NiDX9Renderer::FindClosestDepthStencilFormat(
    const NiPixelFormat* pkFrontBufferFormat, unsigned int uiDepthBPP,
    unsigned int uiStencilBPP) const
{
    NIASSERT(pkFrontBufferFormat);

    NiDX9OnscreenBufferData* pkData = NiDynamicCast(
        NiDX9OnscreenBufferData, (NiDX92DBufferData*)
        m_spDefaultRenderTargetGroup->GetBufferRendererData(0));
    NIASSERT(pkData);

    D3DFORMAT eD3DFBFormat = NiDX9PixelFormat::DetermineD3DFormat(
        *pkFrontBufferFormat);
    NiDX9Renderer::DepthStencilFormat eDSFormat = 
        m_pkDeviceDesc->GetNearestDepthStencilFormat(
        pkData->GetPresentParams().BackBufferFormat, 
        eD3DFBFormat, uiDepthBPP, uiStencilBPP);

    NIASSERT(eDSFormat != DSFMT_UNKNOWN);

    D3DFORMAT eD3DDSFormat = NiDX9Renderer::GetD3DFormat(eDSFormat);
    NIASSERT(eD3DDSFormat != D3DFMT_UNKNOWN);

    NiPixelFormat* pkTemp = NULL;
    if (!m_kDepthStencilFormats.GetAt(eD3DDSFormat, pkTemp) || pkTemp == NULL)
    {
        pkTemp = NiDX9PixelFormat::CreateFromD3DFormat(eD3DDSFormat);
        m_kDepthStencilFormats.SetAt(eD3DDSFormat, pkTemp);
    }

    return pkTemp;
}
//---------------------------------------------------------------------------
bool NiDX9Renderer::GetMRTPostPixelShaderBlendingCapability() const
{
    return m_bMRTPostPixelShaderBlending;
}
//---------------------------------------------------------------------------
bool NiDX9Renderer::GetFormatPostPixelShaderBlendingCapability(
    D3DFORMAT eFormat) const
{
    HRESULT hr = ms_pkD3D9->CheckDeviceFormat(m_uiAdapter, m_eDevType,
        m_eAdapterFormat, D3DUSAGE_QUERY_POSTPIXELSHADER_BLENDING, 
        D3DRTYPE_TEXTURE, eFormat);
    return SUCCEEDED(hr);
}
//---------------------------------------------------------------------------
bool NiDX9Renderer::Do_BeginUsingRenderTargetGroup(
    NiRenderTargetGroup* pkTarget, 
    unsigned int uiClearMode)
{
    if (!m_bDeviceLost)
    {
        NIMETRICS_DX9RENDERER_AGGREGATEVALUE(RENDER_TARGET_CHANGES, 1);

        NIASSERT(ValidateRenderTargetGroup(pkTarget));

        // Clear existing render targets
        NiDX92DBufferData::ClearRenderTarget(m_pkD3DDevice9, 1);
        NiDX92DBufferData::ClearRenderTarget(m_pkD3DDevice9, 2);
        NiDX92DBufferData::ClearRenderTarget(m_pkD3DDevice9, 3);
        
        for (unsigned int ui = 0; ui < pkTarget->GetBufferCount(); ui++)
        {
            NiDX92DBufferData* pkBuffData = NiDynamicCast(NiDX92DBufferData, 
                (NiDX92DBufferData*)pkTarget->GetBufferRendererData(ui));
            if (pkBuffData)
            {
                bool bSet = pkBuffData->SetRenderTarget(m_pkD3DDevice9, ui);
                if (!bSet)
                {
                    bool bSuccessfulReset = 
                        Do_BeginUsingRenderTargetGroup(
                            m_spDefaultRenderTargetGroup,
                            uiClearMode);
                    NIASSERT(bSuccessfulReset);
                    Warning("NiDX9Renderer::BeginUsingRenderTargetGroup> "
                        "Failed - %s", "Setting an output target failed."
                        " Resetting to default target.");
                    return false;
                }
            }
        }

        NiDX9DepthStencilBufferData* pkDepthData = NiDynamicCast(
            NiDX9DepthStencilBufferData, (NiDX92DBufferData*)
            pkTarget->GetDepthStencilBufferRendererData());

        if (pkDepthData)
        {
            bool bSet = pkDepthData->SetDepthTarget(m_pkD3DDevice9);
            if (!bSet)
            {
                bool bSuccessfulReset = 
                    Do_BeginUsingRenderTargetGroup(
                    m_spDefaultRenderTargetGroup, uiClearMode);
                NIASSERT(bSuccessfulReset);
                Warning("NiDX9Renderer::BeginUsingRenderTargetGroup> "
                    "Failed - %s", "Setting the depth/stencil buffer failed."
                    " Resetting to default target.");
                return false;
            }
        }
        else
        {
            if (!NiDX9DepthStencilBufferData::SetNULLDepthStencilTarget(
                m_pkD3DDevice9))
            {
                bool bSuccessfulReset = 
                    Do_BeginUsingRenderTargetGroup(
                    m_spDefaultRenderTargetGroup, uiClearMode);
                NIASSERT(bSuccessfulReset);
                Warning("NiDX9Renderer::BeginUsingRenderTargetGroup> "
                    "Failed - %s", "Setting the depth/stencil buffer to NULL"
                    " failed. Resetting to default target.");
                return false;
            }
        }

        NiDX9OnscreenBufferData* pkOnscreenBuffData = NiDynamicCast(
            NiDX9OnscreenBufferData, (NiDX92DBufferData*)
            pkTarget->GetBufferRendererData(0));

        m_pkCurrRenderTargetGroup = pkTarget;
        if (pkOnscreenBuffData)
            m_pkCurrOnscreenRenderTargetGroup = pkTarget;

        Do_ClearBuffer(NULL, uiClearMode);
    }

    // Return true even if the device is lost.
    return true;
}
//---------------------------------------------------------------------------
bool NiDX9Renderer::Do_EndUsingRenderTargetGroup()
{
    if (!m_bDeviceLost)
    {
        NIASSERT(m_pkCurrRenderTargetGroup);

        for (unsigned int ui = 0; 
            ui < m_pkCurrRenderTargetGroup->GetBufferCount(); ui++)
        {

            NiDX92DBufferData* pkBuffData = NiDynamicCast(
                NiDX92DBufferData, (NiDX92DBufferData*)
                m_pkCurrRenderTargetGroup->GetBufferRendererData(ui));
            NIASSERT(pkBuffData);

            // Store for DisplayFrame
            if (pkBuffData->CanDisplayFrame())
            {
                // Only store once
                if (!m_kBuffersToUseAtDisplayFrame.FindPos(pkBuffData))
                    m_kBuffersToUseAtDisplayFrame.AddTail(pkBuffData);
            }
        }
        
        m_pkCurrRenderTargetGroup = NULL;
        m_pkCurrOnscreenRenderTargetGroup = NULL;
    }
    return true;
}
//---------------------------------------------------------------------------
bool NiDX9Renderer::ValidateRenderTargetGroup(
    NiRenderTargetGroup* pkTarget)
{
    unsigned int uiBuffers = pkTarget->GetBufferCount();
    if (uiBuffers > GetMaxBuffersPerRenderTargetGroup())
        return false;

    for (unsigned int ui = 0; ui < uiBuffers; ui++)
    {
        NiDX92DBufferData* pkBuffer = (NiDX92DBufferData*)
            pkTarget->GetBufferRendererData(ui);

        if (!pkBuffer || !pkBuffer->IsValid())
            return false;

        for (unsigned int uj = 0; uj < ui; uj++)
        {
            NiDX92DBufferData* pkCompBuffer = (NiDX92DBufferData*)
                pkTarget->GetBufferRendererData(uj);

            LPDIRECT3DSURFACE9 pkCompare = 
                pkCompBuffer->GetSurface();

            D3DSURFACE_DESC kDesc;
            HRESULT hr = pkCompare->GetDesc(&kDesc);
            if (FAILED(hr))
                return false;

            // Antialiasing not supported
            if (kDesc.MultiSampleType != D3DMULTISAMPLE_NONE)
                return false;

            // Render target must be same width and height
            if (pkBuffer->GetWidth() != kDesc.Width || 
                pkBuffer->GetHeight() != kDesc.Height)
            {
                return false;
            }

            // Render target may need to be the same texture format, or
            // possibly a format with the same bit depth
            if (!GetIndependentBufferBitDepths())
            {
                unsigned char ucCompareBitDepth = 
                    NiDX9Renderer::GetD3DFormatSize(kDesc.Format);

                unsigned char ucBufferBitDepth = 
                    pkBuffer->GetPixelFormat()->GetBitsPerPixel();

                if (ucCompareBitDepth != ucBufferBitDepth)
                    return false;
            }
        }
    }

    NiDepthStencilBuffer* pkDSBuffer = pkTarget->GetDepthStencilBuffer();
    if (pkDSBuffer)
    {
        for (unsigned int ui = 0; ui < uiBuffers; ui++)
        {
            if (!IsDepthBufferCompatible(pkTarget->GetBuffer(ui), pkDSBuffer))
                return false;
        }
    }
    
    NiDX9DepthStencilBufferData* pkDSData = NiDynamicCast(
        NiDX9DepthStencilBufferData, (NiDX92DBufferData*)
        pkTarget->GetDepthStencilBufferRendererData());

    if (pkDSData != NULL)
    {
        if (!pkDSData->IsValid())
            return false;
    }

    return true;
}
//---------------------------------------------------------------------------
bool NiDX9Renderer::IsDepthBufferCompatible(Ni2DBuffer* pkBuffer,
    NiDepthStencilBuffer* pkDSBuffer)
{
    // Depth buffer may be NULL, but color buffer may not
    if (pkBuffer == NULL)
        return false;
    if (pkDSBuffer == NULL)
        return true;

    // Render target must be smaller than Depth Stencil
    if ((pkBuffer->GetWidth() > pkDSBuffer->GetWidth()) || 
        pkBuffer->GetHeight() > pkDSBuffer->GetHeight())
    {
        return false;
    }

    // Test targets are compatible together
    NiDX92DBufferData* pkColorData = 
        (NiDX92DBufferData*)pkBuffer->GetRendererData();
    NIASSERT(pkColorData);

    NiDX92DBufferData* pkDepthData =
        (NiDX92DBufferData*)pkDSBuffer->GetRendererData();
    NIASSERT(pkDepthData);

    D3DSURFACE_DESC kBufferDesc;
    D3DSURFACE_DESC kDepthDesc;
    HRESULT hr;

    hr = pkColorData->GetSurface()->GetDesc(&kBufferDesc);
    NIASSERT(SUCCEEDED(hr));

    hr = pkDepthData->GetSurface()->GetDesc(&kDepthDesc);
    NIASSERT(SUCCEEDED(hr));

    if ((kBufferDesc.MultiSampleQuality != kDepthDesc.MultiSampleQuality) ||
        (kBufferDesc.MultiSampleType != kDepthDesc.MultiSampleType))
        return false;

    hr = ms_pkD3D9->CheckDepthStencilMatch(GetAdapter(),
        GetDevType(), m_eAdapterFormat,
        kBufferDesc.Format, kDepthDesc.Format);
    if (FAILED(hr))
        return false;

    return true;
}
//---------------------------------------------------------------------------
unsigned int NiDX9Renderer::GetMaxBuffersPerRenderTargetGroup() const
{
    return m_uiMaxNumRenderTargets;
}
//---------------------------------------------------------------------------
bool NiDX9Renderer::GetIndependentBufferBitDepths() const
{
    return m_bIndependentBitDepths;
}
//---------------------------------------------------------------------------
NiRenderTargetGroup* NiDX9Renderer::GetDefaultRenderTargetGroup() const
{   
    return m_spDefaultRenderTargetGroup;
}
//---------------------------------------------------------------------------
void NiDX9Renderer::Do_ClearBuffer(const NiRect<float>* pkR, 
    unsigned int uiMode)
{
    if (uiMode == CLEAR_NONE)
        return;

    NIASSERT(m_pkCurrRenderTargetGroup);

    if (!m_bDeviceLost)
    {
        D3DRECT kClear;
        DWORD dwFlags = 0;

        //  If a rect is passed in, use it
        unsigned int uiBuffW = m_pkCurrRenderTargetGroup->GetWidth(0);
        unsigned int uiBuffH = m_pkCurrRenderTargetGroup->GetHeight(0);
        if (pkR)
        {
            float fW = (float)uiBuffW;
            float fH = (float)uiBuffH;
            // Clear the Back buffer
            kClear.x1 = (long)(fW * pkR->m_left);
            kClear.y1 = (long)(fH * (1.0f - pkR->m_top));
            kClear.x2 = (long)(fW * pkR->m_right); // Full window
            kClear.y2 = (long)(fH * (1.0f - pkR->m_bottom));
        }
        else
        {
            kClear.x1 = 0;
            kClear.y1 = 0;
            kClear.x2 = (long)uiBuffW;
            kClear.y2 = (long)uiBuffH;
        }

        dwFlags |= ((uiMode & NiRenderer::CLEAR_BACKBUFFER) ? 
            D3DCLEAR_TARGET : 0);

        NiDX9DepthStencilBufferData* pkDSData = NiDynamicCast(
            NiDX9DepthStencilBufferData, (NiDX92DBufferData*)
            m_pkCurrRenderTargetGroup->GetDepthStencilBufferRendererData());

        if (pkDSData)
        {
#ifdef _DEBUG
            if (uiMode & NiRenderer::CLEAR_ZBUFFER)
                NIASSERT(pkDSData->HasValidDepthBuffer());
#endif

            dwFlags |= (((uiMode & NiRenderer::CLEAR_ZBUFFER) && 
                pkDSData->HasValidDepthBuffer()) ? D3DCLEAR_ZBUFFER : 0);
        
            dwFlags |= (((uiMode & NiRenderer::CLEAR_STENCIL) && 
                pkDSData->HasValidStencilBuffer()) ? D3DCLEAR_STENCIL : 0);
        }

        m_kD3DPort.Width = m_pkCurrRenderTargetGroup->GetWidth(0);
        m_kD3DPort.Height = m_pkCurrRenderTargetGroup->GetHeight(0);
        m_pkD3DDevice9->SetViewport(&m_kD3DPort);

        HRESULT hrD3DDeviceClear = m_pkD3DDevice9->Clear(1, &kClear, dwFlags, 
            m_uiBackground, m_fZClear, m_uiStencilClear);
        NIASSERT(!FAILED(hrD3DDeviceClear));
    }
}
//---------------------------------------------------------------------------
NiPixelData* NiDX9Renderer::TakeScreenShot(
    const NiRect<unsigned int>* pkScreenRect, 
    const NiRenderTargetGroup* pkTarget)
{
    if (m_bDeviceLost)
        return NULL;

    if (pkTarget == NULL)
        pkTarget = m_spDefaultRenderTargetGroup;

    NIASSERT(pkTarget);
    NIASSERT(pkTarget->IsValid());

    RECT* pkRect = NULL;
    RECT kRect;

    unsigned int uiFBWidth = pkTarget->GetWidth(0);
    unsigned int uiFBHeight = pkTarget->GetHeight(0);
    unsigned int uiSSWidth;     // the width of the screenshot pixeldata
    unsigned int uiSSHeight;    // the height of the screenshot pixeldata

    if (pkScreenRect)
    {
        kRect.left = pkScreenRect->m_left;
        kRect.right = pkScreenRect->m_right;
        kRect.top = pkScreenRect->m_top;
        kRect.bottom = pkScreenRect->m_bottom;

        if ((kRect.left >= (float)uiFBWidth) || 
            (kRect.bottom >= (float)uiFBHeight))
            return NULL;
        if (kRect.right > (float)uiFBWidth)
            kRect.right = uiFBWidth;
        if (kRect.bottom > (float)uiFBHeight)
            kRect.bottom = uiFBHeight;

        uiSSWidth = kRect.right - kRect.left;
        uiSSHeight = kRect.bottom - kRect.top;

        pkRect = &kRect;
    }
    else
    {
        uiSSWidth = uiFBWidth;
        uiSSHeight = uiFBHeight;
    }


    NiDX92DBufferData* pkBuffData = NiDynamicCast(
        NiDX92DBufferData, (NiDX92DBufferData*)
        pkTarget->GetBufferRendererData(0));
    NIASSERT(pkBuffData);

    LPDIRECT3DSURFACE9 pkShotSurf = pkBuffData->GetSurface();

    // Get the surface description. Make sure it's a 32-bit format
    D3DSURFACE_DESC kDesc;
    pkShotSurf->GetDesc(&kDesc);

    // Lock the surface
    D3DLOCKED_RECT kLock;

    if (FAILED(pkShotSurf->LockRect(&kLock, pkRect, D3DLOCK_READONLY)))
        return NULL;

    // Create an NiPixelData object that matches the backbuffer in format and
    // size
    NiPixelData* pkPixelData = NiNew NiPixelData(uiSSWidth, uiSSHeight, 
        *(pkTarget->GetPixelFormat(0)), 1);

    if (pkPixelData == NULL)
    {
        pkShotSurf->UnlockRect();
        return NULL;
    }

    unsigned int uiRowSize 
        = pkPixelData->GetWidth(0) * pkPixelData->GetPixelStride();
    unsigned char* pucDest = pkPixelData->GetPixels(0);
    const unsigned char* pucSrc = (unsigned char*)kLock.pBits;

    for (unsigned int i = 0; i < uiSSHeight; i++)
    {
        NiMemcpy(pucDest, pucSrc, uiRowSize);

        pucDest += uiRowSize;
        pucSrc += kLock.Pitch;
    }

    // Cleanup and return
    pkShotSurf->UnlockRect();

    return pkPixelData;
}
//---------------------------------------------------------------------------

bool NiDX9Renderer::SaveScreenShot(const char* pcFilename,
    EScreenshotFormat eFormat)
{
    D3DXIMAGE_FILEFORMAT eFormatD3D;
    switch (eFormat)
    {
    case FORMAT_PNG:
        eFormatD3D = D3DXIFF_PNG;
        break;
    case FORMAT_JPEG:
        eFormatD3D = D3DXIFF_JPG;
        break;
    default:
        Error("NiDX9Renderer::SaveScreenShot> "
            "Unsupported image format %d\n", eFormat);
        return false;
    }

    char acImagePath[NI_MAX_PATH];

    // Make sure the path to the screenshot directory exists
    NiFilename kDirName(pcFilename);
    kDirName.SetFilename("");
    kDirName.SetExt("");
    kDirName.GetFullPath(acImagePath, NI_MAX_PATH);
    NiPath::Standardize(acImagePath);

    if ((strlen(acImagePath) > 0) && 
        !NiFile::CreateDirectoryRecursive(acImagePath)) {
        Error("NiDX9Renderer::SaveScreenShot> "
            "Could not create directory (\"%s\")\n", acImagePath);
        return false;
    }

    // Get the full filename
    NiFilename kName(pcFilename);
    kName.GetFullPath(acImagePath, NI_MAX_PATH);
    NiPath::Standardize(acImagePath);

    // Acquire the backbuffer
    LPDIRECT3DDEVICE9 pkDevice = GetD3DDevice();
    LPDIRECT3DSURFACE9 pkDestSurface = NULL;
    HRESULT hr = pkDevice->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO,
        &pkDestSurface);
    if (SUCCEEDED(hr))
    {
        // Save the image to a file
        HRESULT hr2 = D3DXSaveSurfaceToFile(acImagePath,
            eFormatD3D, pkDestSurface, NULL, NULL);

        ReleaseResource(pkDestSurface);

        return SUCCEEDED(hr2);
    }
    else
    {
        // Failed to acquire the back buffer surface
        return false;
    }
}
//---------------------------------------------------------------------------
bool NiDX9Renderer::FastCopy(const Ni2DBuffer* pkSrc, Ni2DBuffer* pkDest, 
    const NiRect<unsigned int>* pkSrcRect,
    unsigned int uiDestX , unsigned int uiDestY)
{
    if (m_bDeviceLost)
        return false;

    NIASSERT(pkSrc != NULL);
    NIASSERT(pkDest != NULL);

    NiDX92DBufferData* pkSrcRendData = (NiDX92DBufferData*)
        pkSrc->GetRendererData();
    NiDX92DBufferData* pkDestRendData = (NiDX92DBufferData*)
        pkDest->GetRendererData();

    if (pkSrcRendData == NULL || pkDestRendData == NULL)
    {
        Warning("NiDX9Renderer::FastCopy> "
           "Failed - %s", "No RendererData found");
        return false;
    }

    if (*(pkSrcRendData->GetPixelFormat()) != 
        *(pkDestRendData->GetPixelFormat()))
    {
        Warning("NiDX9Renderer::FastCopy> "
           "Failed - %s", "Pixel formats do not match");
        return false;
    }
    
    LPDIRECT3DSURFACE9 pkSourceSurface = pkSrcRendData->GetSurface();
    LPDIRECT3DSURFACE9 pkDestSurface = pkDestRendData->GetSurface();

    if (pkSourceSurface == NULL || pkDestSurface == NULL)
    {
        Warning("NiDX9Renderer::FastCopy> "
           "Failed - %s", "NULL Surface found");
        return false;
    }

    RECT kD3DSrcRect;
    if (pkSrcRect != NULL)
    {
        kD3DSrcRect.left    = pkSrcRect->m_left;
        kD3DSrcRect.right   = pkSrcRect->m_right;
        kD3DSrcRect.top     = pkSrcRect->m_top;
        kD3DSrcRect.bottom  = pkSrcRect->m_bottom;
    }


    NIASSERT(m_pkD3DDevice9);
    HRESULT hr;
    if (pkSrcRect != NULL)
    {
        RECT kD3DDestRect;
        kD3DDestRect.left   = uiDestX;
        kD3DDestRect.right  = uiDestX + kD3DSrcRect.right - kD3DSrcRect.left;
        kD3DDestRect.top    = uiDestY;
        kD3DDestRect.bottom = uiDestY + kD3DSrcRect.bottom - kD3DSrcRect.top;

        hr = m_pkD3DDevice9->StretchRect(pkSourceSurface,
            &kD3DSrcRect, pkDestSurface, &kD3DDestRect,
            D3DTEXF_NONE);
    }
    else
    {
        hr = m_pkD3DDevice9->StretchRect(pkSourceSurface,
            NULL, pkDestSurface, NULL, D3DTEXF_NONE);
    }

    if (FAILED(hr))
    {
        Warning("NiDX9Renderer::FastCopy> "
           "Failed StretchRect - %s", 
            NiDX9ErrorString((unsigned int)hr));
        return false;
    }
    return true;
}
//---------------------------------------------------------------------------
bool NiDX9Renderer::Copy(const Ni2DBuffer* pkSrc, Ni2DBuffer* pkDest, 
    const NiRect<unsigned int>* pkSrcRect,
    const NiRect<unsigned int>* pkDestRect,
    Ni2DBuffer::CopyFilterPreference ePref)
{
    if (m_bDeviceLost)
        return false;

    NIASSERT(pkSrc != NULL);
    NIASSERT(pkDest != NULL);

    NiDX92DBufferData* pkSrcRendData = (NiDX92DBufferData*)
        pkSrc->GetRendererData();
    NiDX92DBufferData* pkDestRendData = (NiDX92DBufferData*)
        pkDest->GetRendererData();

    if (pkSrcRendData == NULL || pkDestRendData == NULL)
    {
        Warning("NiDX9Renderer::Copy> "
           "Failed - %s", "No RendererData found");
        return false;
    }
    
    LPDIRECT3DSURFACE9 pkSourceSurface = pkSrcRendData->GetSurface();
    LPDIRECT3DSURFACE9 pkDestSurface = pkDestRendData->GetSurface();

    if (pkSourceSurface == NULL || pkDestSurface == NULL)
    {
        Warning("NiDX9Renderer::Copy> "
           "Failed - %s", "NULL Surface found");
        return false;
    }

    RECT kD3DSrcRect;
    if (pkSrcRect != NULL)
    {
        kD3DSrcRect.left    = pkSrcRect->m_left;
        kD3DSrcRect.right   = pkSrcRect->m_right;
        kD3DSrcRect.top     = pkSrcRect->m_top;
        kD3DSrcRect.bottom  = pkSrcRect->m_bottom;
    }
    else
    {
        kD3DSrcRect.left    = 0;
        kD3DSrcRect.right   = pkSrc->GetWidth();
        kD3DSrcRect.top     = 0;
        kD3DSrcRect.bottom  = pkSrc->GetHeight();
    }

    RECT kD3DDestRect;
    if (pkDestRect != NULL)
    {
        kD3DDestRect.left    = pkDestRect->m_left;
        kD3DDestRect.right   = pkDestRect->m_right;
        kD3DDestRect.top     = pkDestRect->m_top;
        kD3DDestRect.bottom  = pkDestRect->m_bottom;
    }
    else
    {
        kD3DDestRect.left    = 0;
        kD3DDestRect.right   = pkDest->GetWidth();
        kD3DDestRect.top     = 0;
        kD3DDestRect.bottom  = pkDest->GetHeight();
    }

    D3DTEXTUREFILTERTYPE eFilterType;
    switch (ePref)
    {
        default:
        case Ni2DBuffer::COPY_FILTER_NONE:
            eFilterType = D3DTEXF_NONE;
            break;
        case Ni2DBuffer::COPY_FILTER_POINT:
            eFilterType = D3DTEXF_POINT;
            break;
        case Ni2DBuffer::COPY_FILTER_LINEAR:
            eFilterType = D3DTEXF_LINEAR;
            break;
    }

    NIASSERT(m_pkD3DDevice9);
    HRESULT hr = m_pkD3DDevice9->StretchRect(pkSourceSurface,
        &kD3DSrcRect, pkDestSurface, &kD3DDestRect,
        eFilterType);

    if (FAILED(hr))
    {
        Warning("NiDX9Renderer::Copy> "
           "Failed StretchRect - %s", 
            NiDX9ErrorString((unsigned int)hr));
        return false;
    }
    return true;
}
//---------------------------------------------------------------------------
NiDepthStencilBuffer* NiDX9Renderer::GetDefaultDepthStencilBuffer() const
{
    return m_spDefaultRenderTargetGroup->GetDepthStencilBuffer();
}
//---------------------------------------------------------------------------
Ni2DBuffer* NiDX9Renderer::GetDefaultBackBuffer() const
{
    return m_spDefaultRenderTargetGroup->GetBuffer(0);
}
//---------------------------------------------------------------------------
bool NiDX9Renderer::PrecacheTexture(NiTexture* pkIm, bool bForceLoad, 
    bool bLocked)
{
    return m_pkTextureManager->PrecacheTexture(pkIm, bForceLoad, bLocked);
}
//---------------------------------------------------------------------------
bool NiDX9Renderer::PurgeAllTextures(bool bPurgeLocked)
{
    NiDX9TextureData::ClearTextureData(this);
    return true;
}
//---------------------------------------------------------------------------
bool NiDX9Renderer::PurgeTexture(NiTexture* pkIm)
{
    NiDX9TextureData* pkData = (NiDX9TextureData*)
        (pkIm->GetRendererData());

    if (pkData)
    {
        NiDX9RenderedTextureData* pkRTData = pkData->GetAsRenderedTexture();

        if (pkRTData)
        {
            if (NiIsKindOf(NiRenderedCubeMap, pkIm))
            {
                NiRenderedCubeMap* pkRenderedCubeMap = 
                    (NiRenderedCubeMap*)pkIm;
                for (unsigned int i = 0; i < NiRenderedCubeMap::FACE_NUM; i++)
                {
                    Ni2DBuffer* pkBuffer = pkRenderedCubeMap->GetFaceBuffer(
                        (NiRenderedCubeMap::FaceID)i);
                    pkBuffer->SetRendererData(NULL);
                }
            }
            else
            {
                NiRenderedTexture* pkRenderedTex = (NiRenderedTexture*)pkIm;
                Ni2DBuffer* pkBuffer = pkRenderedTex->GetBuffer();
                pkBuffer->SetRendererData(NULL);
            }
        }

        pkIm->SetRendererData(NULL);
        NiDelete pkData;
    }

    return true;
}
//---------------------------------------------------------------------------
bool NiDX9Renderer::SetMipmapSkipLevel(unsigned int uiSkip)
{
    return NiDX9SourceTextureData::SetMipmapSkipLevel(uiSkip);
}
//---------------------------------------------------------------------------
unsigned int NiDX9Renderer::GetMipmapSkipLevel() const
{
    return NiDX9SourceTextureData::GetMipmapSkipLevel();
}
//---------------------------------------------------------------------------
void NiDX9Renderer::FreeGeometricData(
    NiGeometryData* pkData, 
    NiSkinInstance* pkSkin, 
    NiSkinPartition::Partition* pkPartition)
{
    if (!pkData)
        return;

    if (pkData->GetConsistency() == NiGeometryData::STATIC)
    {
        unsigned short usVerts = pkData->GetVertexCount();

        // Quick-out if they want to toss everything...
        unsigned int uiKeepFlags = pkData->GetKeepFlags();
        if (uiKeepFlags == 0)
        {
            pkData->Replace(usVerts, 0, 0, 0, 0, 0,
                pkData->GetNormalBinormalTangentMethod());
        }
        else
        {
            NiPoint3* pkVertex = 0;
            NiPoint3* pkNormal = 0;
            NiColorA* pkColor = 0;
            NiPoint2* pkTexture = 0;
            unsigned int uiTextureSets = 0;

            if (uiKeepFlags & NiGeometryData::KEEP_XYZ)
                pkVertex = pkData->GetVertices();
            if (uiKeepFlags & NiGeometryData::KEEP_NORM)
                pkNormal = pkData->GetNormals();
            if (uiKeepFlags & NiGeometryData::KEEP_COLOR)
                pkColor = pkData->GetColors();
            if (uiKeepFlags & NiGeometryData::KEEP_UV)
            {
                pkTexture = pkData->GetTextures();
                uiTextureSets = pkData->GetTextureSets();
            }

            pkData->Replace(usVerts, pkVertex, pkNormal, pkColor, pkTexture, 
                uiTextureSets, pkData->GetNormalBinormalTangentMethod());
        }

        if (pkPartition)
        {
            if ((uiKeepFlags & NiGeometryData::KEEP_BONEDATA) == 0)
                pkSkin->GetSkinData()->FreeBoneVertData();
        }
        else
        {
            if ((uiKeepFlags & NiGeometryData::KEEP_INDICES) == 0)
            {
                // if TriShape...
                if (NiIsKindOf(NiTriShapeData, pkData))
                {
                    NiTriShapeData* pkTriShapeData = (NiTriShapeData*)pkData;
                    pkTriShapeData->Replace(
                        pkTriShapeData->GetTriangleCount(), 0);
                }

                // if TriStrips...
                else if (NiIsKindOf(NiTriStripsData, pkData))
                {
                    NiTriStripsData* pkTriStripsData = 
                        (NiTriStripsData*)pkData;
                    pkTriStripsData->Replace(pkTriStripsData->GetStripCount(), 
                        (unsigned short*)(pkTriStripsData->GetStripLengths()),
                        0);
                }
            }
        }
    }
}
//---------------------------------------------------------------------------
bool NiDX9Renderer::PrecacheGeometry(NiGeometry* pkGeometry, 
    unsigned int uiBonesPerPartition, unsigned int uiBonesPerVertex)
{
    if (pkGeometry == 0 || !NiIsKindOf(NiTriBasedGeom, pkGeometry))
        return false;

    NiGeometryData* pkGeometryData = pkGeometry->GetModelData(); 
    NiSkinInstance* pkSkinInstance = pkGeometry->GetSkinInstance();

    NiD3DShaderInterface* pkShader = NULL;
    if (pkGeometry)
    {
        pkShader = NiDynamicCast(NiD3DShaderInterface, 
            pkGeometry->GetShaderFromMaterial());

        if (pkShader == 0)
        {
            const NiMaterial* pkTempMat = pkGeometry->GetActiveMaterial();
            pkGeometry->ApplyAndSetActiveMaterial(m_spCurrentDefaultMaterial);
            pkShader = NiDynamicCast(NiD3DShaderInterface, 
                pkGeometry->GetShaderFromMaterial());
            pkGeometry->SetActiveMaterial(pkTempMat);
        }
    }
    else
    {
        // In the case of NiScreenTexture
        pkShader = m_spLegacyDefaultShader;
    }

    // If no shader is found, do not precache
    if (pkShader == NULL)
    {
        Error("PrecacheGeometry> No shader found for object %s!\n", 
            pkGeometry->GetName());
        return false;
    }

    // Check for skinned objects on hardware that cannot skin
    bool bHWSkinned = IsHardwareSkinned(pkGeometry);

    if (pkSkinInstance && !bHWSkinned)
        pkGeometryData->SetConsistency(NiGeometryData::VOLATILE);

    // Volatile data cannot be precached.
    if (pkGeometryData->GetConsistency() == NiGeometryData::VOLATILE)
        return true;

    LockPrecacheCriticalSection();

    
    NiD3DShaderDeclaration* pkDecl = 
        (NiD3DShaderDeclaration*)(pkGeometry->GetShaderDecl());

    if (!pkDecl)
    {
        NIVERIFY(NiShaderDeclaration::CreateDeclForGeometry(pkGeometry));
        pkDecl = (NiD3DShaderDeclaration*)(pkGeometry->GetShaderDecl());
    }

    // Place object in default group if it is not already in a group
    // (not volatile - check for multiple streams only)
    m_pkGeometryGroupManager->AddObjectToGroup(m_pkDefaultGroup, 
        pkGeometryData, pkSkinInstance, bHWSkinned,
        uiBonesPerPartition, uiBonesPerVertex);

    bool bSuccess = false;
    if (bHWSkinned)
    {
        //  It's a HW skinned geometric object
        NIASSERT((GetFlags() & CAPS_HARDWARESKINNING) != 0 || 
            m_bSWVertexSwitchable);
        bSuccess = PrePackSkinnedGeometryBuffer(pkGeometry, pkGeometryData, 
            pkSkinInstance, pkDecl,
            uiBonesPerPartition, uiBonesPerVertex);
    }
    else
    {
        if (NiIsKindOf(NiTriShape, pkGeometry))
        {
            NiTriShapeData* pkTriShapeData = (NiTriShapeData*)pkGeometryData;

            unsigned short usTriCount = 
                pkTriShapeData->GetActiveTriangleCount();
            unsigned short usMaxTriCount = 
                pkTriShapeData->GetTriangleCount();
            const unsigned short* pusIndexArray = 
                pkTriShapeData->GetTriList();

            bSuccess = PrePackGeometryBuffer(pkGeometry, pkGeometryData, 
                usTriCount, usMaxTriCount, pusIndexArray, NULL, 1,  
                pkDecl);
        }
        else if (NiIsKindOf(NiTriStrips, pkGeometry))
        {
            NiTriStripsData* pkTriStripsData = 
                (NiTriStripsData*)pkGeometryData;

            unsigned short usTriCount = 
                pkTriStripsData->GetActiveTriangleCount();
            unsigned short usMaxTriCount = 
                pkTriStripsData->GetTriangleCount();
            const unsigned short* pusIndexArray = 
                pkTriStripsData->GetStripLists();
            const unsigned short* pusArrayLengths = 
                pkTriStripsData->GetStripLengths();
            unsigned short usArrayCount = pkTriStripsData->GetStripCount();

            bSuccess = PrePackGeometryBuffer(pkGeometry, pkGeometryData, 
                usTriCount, usMaxTriCount, pusIndexArray, pusArrayLengths, 
                usArrayCount, pkDecl);
        }
    }

    UnlockPrecacheCriticalSection();
    return bSuccess;
}
//---------------------------------------------------------------------------
void NiDX9Renderer::PerformPrecache()
{
    LockPrecacheCriticalSection();
    NiTMapIterator kIter = m_kPrePackObjects.GetFirstPos();
    PrePackObject* pkDeleteList = NULL;

    while (kIter)
    {
        NiVBBlock* pkVBBlock;
        PrePackObject* pkPrePack;
        m_kPrePackObjects.GetNext(kIter, pkVBBlock, pkPrePack);
        if (pkPrePack == NULL)
            continue;
        
        if (pkVBBlock)
        {
            // Pack entire block in one fell swoop

            // Get size of lock
            unsigned int uiLockBegin = 0xffffffff;
            unsigned int uiLockEnd = 0;
            PrePackObject* pkIterator = pkPrePack;
            NiVBChip* pkChip = NULL;

            while (pkIterator)
            {
                PrePackObject* pkNext = pkIterator->m_pkNext;
                pkChip = pkIterator->m_pkBuffData->GetVBChip(
                    pkIterator->m_uiStream);

                if (pkChip->GetOffset() < uiLockBegin)
                {
                    uiLockBegin = pkChip->GetOffset();
                }
                if (pkChip->GetSize() + pkChip->GetOffset() > uiLockEnd)
                {
                    uiLockEnd = pkChip->GetSize() + pkChip->GetOffset();
                }

                pkIterator = pkNext;
            }

            NIASSERT(uiLockEnd <= pkVBBlock->GetSize());

            if (uiLockEnd <= uiLockBegin)
                continue;

            unsigned int uiLockSize = uiLockEnd - uiLockBegin;

            // Lock buffer
            void* pvLockedBuff = m_pkVBManager->LockVB(pkChip->GetVB(), 
                uiLockBegin, uiLockSize, pkChip->GetLockFlags());

            // Pack each object
            pkIterator = pkPrePack;
            while (pkIterator)
            {
                PrePackObject* pkNext = pkIterator->m_pkNext;
                NiGeometryBufferData* pkBuffData = pkIterator->m_pkBuffData;
                pkChip = pkBuffData->GetVBChip(pkIterator->m_uiStream);

                NiVBChip* pkNewChip = NULL;
                void* pvDest = (unsigned char*)pvLockedBuff + 
                    pkChip->GetOffset();

                if (pkIterator->m_pkShaderDecl)
                {
                    pkNewChip = pkIterator->m_pkShaderDecl->
                        PackUntransformedVB(pkIterator->m_pkData, 
                        pkIterator->m_pkSkin, pkIterator->m_pkPartition, 
                        NiGeometryData::DIRTY_MASK, pkChip, 
                        pkIterator->m_uiStream, pvDest);

                    NIASSERT(pkNewChip == NULL || pkNewChip == pkChip);
                }

                if (pkNewChip == NULL)
                {
                    // Can only pack one stream without shader declaration
                    NIASSERT(pkIterator->m_uiStream == 0);

                    // Caps have already been checked for HW skinning
                    if (pkIterator->m_pkPartition)
                    {
                        pkNewChip = m_pkVBManager->PackSkinnedVB(
                            pkIterator->m_pkData, pkIterator->m_pkSkin, 
                            pkIterator->m_pkPartition, 
                            NiGeometryData::DIRTY_MASK, pkChip, pvDest);
                    }
                    else
                    {
                        pkNewChip = m_pkVBManager->PackUntransformedVB(
                            pkIterator->m_pkData, pkIterator->m_pkSkin, 
                            NiGeometryData::DIRTY_MASK, pkChip, pvDest);
                    }

                    NIASSERT(pkNewChip == pkChip);
                }

                // Only pack index array on stream 0
                if (pkIterator->m_uiStream == 0)
                {
                    const unsigned short* pusIndexArray = 
                        pkBuffData->GetIndexArray();
                    if (pusIndexArray)
                    {
                        const unsigned short* pusIndexCount = 
                            pkBuffData->GetArrayLengths();
                        unsigned int uiIndexCount = 0;
                        unsigned int uiMaxIndexCount = 0;

                        if (pusIndexCount)
                        {
                            unsigned int uiNumArrays = 
                                pkBuffData->GetNumArrays();
                            while (uiNumArrays--)
                                uiIndexCount += pusIndexCount[uiNumArrays];
                            uiMaxIndexCount = uiIndexCount;
                        }
                        else
                        {
                            D3DPRIMITIVETYPE eType = pkBuffData->GetType();
                            if (eType == D3DPT_TRIANGLELIST)
                            {
                                uiIndexCount = 3 * pkBuffData->GetTriCount();
                                uiMaxIndexCount = 
                                    3 * pkBuffData->GetMaxTriCount();
                            }
                            else if (eType == D3DPT_TRIANGLESTRIP)
                            {
                                uiIndexCount = pkBuffData->GetTriCount() + 2;
                                uiMaxIndexCount = 
                                    pkBuffData->GetMaxTriCount() + 2;
                            }
                            else
                            {
                                NIASSERT(false);
                            }
                        }

                        unsigned int uiIBSize = pkBuffData->GetIBSize();
                        LPDIRECT3DINDEXBUFFER9 pkIB = 
                            m_pkIBManager->PackBuffer(pusIndexArray, 
                            uiIndexCount, uiMaxIndexCount, pkBuffData->GetIB(),
                            uiIBSize, D3DPOOL_MANAGED, 
                            (pkBuffData->GetSoftwareVP() ? 
                            D3DUSAGE_SOFTWAREPROCESSING : 0));
                        pkBuffData->SetIB(pkIB, uiIndexCount, uiIBSize);
                        NIASSERT(pkIB);
                    }
                }

                // Put object on delete list
                pkIterator->m_pkNext = pkDeleteList;
                pkDeleteList = pkIterator;

                pkIterator = pkNext;
            }

            // Unlock
            m_pkVBManager->UnlockVB(pkChip->GetVB());
        }
        else
        {
            // Each chip will be packed separately

            // Pack each object - VB and IB at the same time
            PrePackObject* pkIterator = pkPrePack;
            while (pkIterator)
            {
                PrePackObject* pkNext = pkIterator->m_pkNext;

                // Only pack when encountering the first stream, since
                // packing here will fill in all streams.
                if (pkIterator->m_uiStream == 0)
                {
                    // Caps have already been checked for HW skinning
                    if (pkIterator->m_pkPartition)
                    {
                        PackSkinnedGeometryBuffer(pkIterator->m_pkBuffData, 
                            pkIterator->m_pkData, pkIterator->m_pkSkin,
                            pkIterator->m_pkPartition, 
                            pkIterator->m_pkShaderDecl,
                            true);
                    }
                    else
                    {
                        PackGeometryBuffer(pkIterator->m_pkBuffData, 
                            pkIterator->m_pkData, pkIterator->m_pkSkin, 
                            pkIterator->m_pkShaderDecl, 
                            true);
                    }
                }

                // Put object on delete list
                pkIterator->m_pkNext = pkDeleteList;
                pkDeleteList = pkIterator;

                pkIterator = pkNext;
            }
        }
    }
    m_kPrePackObjects.RemoveAll();

    // Delete prepack lists and destroy data if requested
    while (pkDeleteList)
    {
        PrePackObject* pkNext = pkDeleteList->m_pkNext;

        if (pkDeleteList->m_pkData->GetConsistency() == 
            NiGeometryData::STATIC)
        {
            // Only deal with this when encountering first stream
            if (pkDeleteList->m_uiStream == 0)
            {
                NiGeometryData* pkData = pkDeleteList->m_pkData;
                pkData->ClearRevisionID();

                //  Free it if they requested it freed...
                FreeGeometricData(pkData,
                    pkDeleteList->m_pkSkin, pkDeleteList->m_pkPartition);
            }
        }

        NiDelete pkDeleteList;
        pkDeleteList = pkNext;
    }
    UnlockPrecacheCriticalSection();
}
//---------------------------------------------------------------------------
bool NiDX9Renderer::Do_BeginFrame()
{
    if (LostDeviceRestore())
    {
        PerformPrecache();

        m_spLegacyDefaultShader->NextFrame();
        m_pkLightManager->NextFrame();

        // Reset any states we are tracking frame-by-frame
        // BeginScene
        if (FAILED(m_pkD3DDevice9->BeginScene()))
        {
#ifdef _DEBUG
            Warning("NiDX9Renderer::Do_BeginFrame> FAILED "
                "- probably alt-tabbed away from app");
#endif
            return false;
        }

        while (m_kBuffersToUseAtDisplayFrame.GetSize())
            m_kBuffersToUseAtDisplayFrame.RemoveHead();
    }

    return true;
}
//---------------------------------------------------------------------------
bool NiDX9Renderer::Do_EndFrame()
{
    if (!m_bDeviceLost)
    {
        if (FAILED(m_pkD3DDevice9->EndScene()))
        {
#ifdef _DEBUG
            Warning("NiDX9Renderer::Do_EndFrame> FAILED - Lost Device?");
#endif
            // Do not re-open the scene if this call fails
            return false;
        }

        m_pkVBManager->EndFrame();
    }

    return true;
}
//---------------------------------------------------------------------------
bool NiDX9Renderer::Do_DisplayFrame()
{
    if (!m_bDeviceLost)
    {
        while (m_kBuffersToUseAtDisplayFrame.GetSize())
        {
            NiDX92DBufferDataPtr spBuffer = 
                m_kBuffersToUseAtDisplayFrame.RemoveHead();

            spBuffer->DisplayFrame();
        }
    }

    NIMETRICS_DX9RENDERER_RECORDMETRICS();

    return true;
}
//---------------------------------------------------------------------------
void NiDX9Renderer::Do_SetCameraData(const NiPoint3& kLoc,
    const NiPoint3& kDir, const NiPoint3& kUp, const NiPoint3& kRight,
    const NiFrustum& kFrustum, const NiRect<float>& kPort)
{
    if (!m_bDeviceLost)
    {
        // View matrix update
        m_kD3DView._11 = kRight.x;    
        m_kD3DView._12 = kUp.x;    
        m_kD3DView._13 = kDir.x;
        m_kD3DView._14 = 0.0f;
        m_kD3DView._21 = kRight.y; 
        m_kD3DView._22 = kUp.y;    
        m_kD3DView._23 = kDir.y;
        m_kD3DView._24 = 0.0f;
        m_kD3DView._31 = kRight.z; 
        m_kD3DView._32 = kUp.z;    
        m_kD3DView._33 = kDir.z;
        m_kD3DView._34 = 0.0f;
        m_kD3DView._41 = -(kRight * kLoc);
        m_kD3DView._42 = -(kUp * kLoc);
        m_kD3DView._43 = -(kDir * kLoc);
        m_kD3DView._44 = 1.0f;

        m_kInvView._11 = kRight.x;
        m_kInvView._12 = kRight.y;
        m_kInvView._13 = kRight.z;
        m_kInvView._14 = 0.0f;
        m_kInvView._21 = kUp.x;
        m_kInvView._22 = kUp.y;
        m_kInvView._23 = kUp.z;
        m_kInvView._24 = 0.0f;
        m_kInvView._31 = kDir.x;
        m_kInvView._32 = kDir.y;
        m_kInvView._33 = kDir.z;
        m_kInvView._34 = 0.0f;
        m_kInvView._41 = kLoc.x;
        m_kInvView._42 = kLoc.y;
        m_kInvView._43 = kLoc.z;
        m_kInvView._44 = 1.0f;

        m_pkD3DDevice9->SetTransform(D3DTS_VIEW, &m_kD3DView);

        m_kModelCamRight = kRight;
        m_kCamRight = kRight;
        m_kModelCamUp = kUp;
        m_kCamUp = kUp;

        m_fNearDepth = kFrustum.m_fNear;
        m_fDepthRange = kFrustum.m_fFar - kFrustum.m_fNear;

        // Projection matrix update
        float fRmL = kFrustum.m_fRight - kFrustum.m_fLeft;
        float fRpL = kFrustum.m_fRight + kFrustum.m_fLeft;
        float fTmB = kFrustum.m_fTop - kFrustum.m_fBottom;
        float fTpB = kFrustum.m_fTop + kFrustum.m_fBottom;
        float fInvFmN = 1.0f / m_fDepthRange;

        if (kFrustum.m_bOrtho)
        {
            if (m_pkRenderState->GetLeftHanded())
            {
                m_kD3DProj._11 = -2.0f / fRmL;
                m_kD3DProj._21 = 0.0f;
                m_kD3DProj._31 = 0.0f;      
                m_kD3DProj._41 = fRpL / fRmL; 
            }
            else
            {
                m_kD3DProj._11 = 2.0f / fRmL;
                m_kD3DProj._21 = 0.0f;
                m_kD3DProj._31 = 0.0f; 
                m_kD3DProj._41 = -fRpL / fRmL; 
            }

            m_kD3DProj._12 = 0.0f;
            m_kD3DProj._22 = 2.0f / fTmB;
            m_kD3DProj._32 = 0.0f; 
            m_kD3DProj._42 = -fTpB / fTmB;
            m_kD3DProj._13 = 0.0f;
            m_kD3DProj._23 = 0.0f;
            m_kD3DProj._33 = fInvFmN; 
            m_kD3DProj._43 = -(kFrustum.m_fNear * fInvFmN); 

            // A "w-friendly" projection matrix to make fog, w-buffering work
            m_kD3DProj._14 = 0.0f;
            m_kD3DProj._24 = 0.0f;
            m_kD3DProj._34 = 0.0f;
            m_kD3DProj._44 = 1.0f;               
        }
        else
        {
            if (m_pkRenderState->GetLeftHanded())
            {
                m_kD3DProj._11 = -2.0f / fRmL;
                m_kD3DProj._21 = 0.0f;
                m_kD3DProj._31 = fRpL / fRmL;
                m_kD3DProj._41 = 0.0f;
            }
            else
            {
                m_kD3DProj._11 = 2.0f / fRmL;
                m_kD3DProj._21 = 0.0f;
                m_kD3DProj._31 = -fRpL / fRmL;
                m_kD3DProj._41 = 0.0f;
            }
            m_kD3DProj._12 = 0.0f;
            m_kD3DProj._22 = 2.0f / fTmB;
            m_kD3DProj._32 = -fTpB / fTmB;
            m_kD3DProj._42 = 0.0f;
            m_kD3DProj._13 = 0.0f;
            m_kD3DProj._23 = 0.0f;
            m_kD3DProj._33 = kFrustum.m_fFar * fInvFmN;
            m_kD3DProj._43 = -(kFrustum.m_fNear * kFrustum.m_fFar * fInvFmN);

            // A "w-friendly" projection matrix to make fog, w-buffering work
            m_kD3DProj._14 = 0.0f;
            m_kD3DProj._24 = 0.0f;
            m_kD3DProj._34 = 1.0f;
            m_kD3DProj._44 = 0.0f;
        }

        m_pkD3DDevice9->SetTransform(D3DTS_PROJECTION, &m_kD3DProj);

        // Viewport update
        NIASSERT(m_pkCurrRenderTargetGroup);
        float fWidth = (float)m_pkCurrRenderTargetGroup->GetWidth(0);
        float fHeight = (float)m_pkCurrRenderTargetGroup->GetHeight(0);

        D3DVIEWPORT9 kD3DPort;
 
        kD3DPort.X = (unsigned int)(kPort.m_left * fWidth);
        kD3DPort.Y = (unsigned int)((1.0f - kPort.m_top) * fHeight);
        kD3DPort.Width = 
            (unsigned int)((kPort.m_right - kPort.m_left) * fWidth);
        kD3DPort.Height = 
            (unsigned int)((kPort.m_top - kPort.m_bottom) * fHeight);
        kD3DPort.MinZ = 0.0f;     
        kD3DPort.MaxZ = 1.0f;

        m_pkD3DDevice9->SetViewport(&kD3DPort);

        // Fog information to the camera
        NIASSERT(m_pkRenderState);
        m_pkRenderState->SetCameraNearAndFar(kFrustum.m_fNear, 
            kFrustum.m_fFar);

        // Set cached camera data.
        m_kCachedFrustum = kFrustum;
        m_kCachedPort = kPort;
    }
}
//---------------------------------------------------------------------------
void NiDX9Renderer::Do_SetScreenSpaceCameraData(const NiRect<float>* pkPort)
{
    if (!m_bDeviceLost)
    {
        // Screen elements are in normalized display coordinates, (x,y), where 
        // 0 <= x <= 1 
        // 0 <= y <= 1
        // 0 <= z <= 9999
        // The screen buffer width is w and the screen height is h.
        // The camera parameters are:
        // Camera location: (1/2, 1/2, -1)
        // Camera right:    (1,  0, 0)
        // Camera up:       (0, -1, 0)
        // Camera dir:      (0,  0, 1)
        // Camera frustum:
        //      Left:   -1/2 + 1/(2*w)
        //      Right:  1/2 + 1/(2*w)
        //      Top:    1/2 - 1/(2*h)
        //      Bottom: -1/2 - 1/(2*h)
        //      Near:   1
        //      Far:    10000

        NIASSERT(m_pkCurrRenderTargetGroup);
        unsigned int uiWidth = m_pkCurrRenderTargetGroup->GetWidth(0);
        unsigned int uiHeight = m_pkCurrRenderTargetGroup->GetHeight(0);
        float fWidth = (float)uiWidth;
        float fHeight = (float)uiHeight;

        // View matrix update
        m_kD3DView._11 = 1.0f;    
        m_kD3DView._12 = 0.0f;    
        m_kD3DView._13 = 0.0f;
        m_kD3DView._14 = 0.0f;
        m_kD3DView._21 = 0.0f; 
        m_kD3DView._22 = -1.0;    
        m_kD3DView._23 = 0.0f;
        m_kD3DView._24 = 0.0f;
        m_kD3DView._31 = 0.0f; 
        m_kD3DView._32 = 0.0f;    
        m_kD3DView._33 = 1.0f;
        m_kD3DView._34 = 0.0f;
        m_kD3DView._41 = -0.5f;
        m_kD3DView._42 = 0.5f;
        m_kD3DView._43 = 1.0f;
        m_kD3DView._44 = 1.0f;

        m_kInvView._11 = 1.0f;
        m_kInvView._12 = 0.0f;
        m_kInvView._13 = 0.0f;
        m_kInvView._14 = 0.0f;
        m_kInvView._21 = 0.0f;
        m_kInvView._22 = -1.0f;
        m_kInvView._23 = 0.0f;
        m_kInvView._24 = 0.0f;
        m_kInvView._31 = 0.0f;
        m_kInvView._32 = 0.0f;
        m_kInvView._33 = 1.0f;
        m_kInvView._34 = 0.0f;
        m_kInvView._41 = 0.5f;
        m_kInvView._42 = 0.5f;
        m_kInvView._43 = -1.0f;
        m_kInvView._44 = 1.0f;

        m_pkD3DDevice9->SetTransform(D3DTS_VIEW, &m_kD3DView);

        m_kModelCamRight = NiPoint3(1.0f, 0.0f, 0.0f);
        m_kCamRight = m_kModelCamRight;
        m_kModelCamUp = NiPoint3(0.0f, -1.0f, 0.0f);
        m_kCamUp = m_kModelCamUp;

        m_fNearDepth = 1.0f;
        m_fDepthRange = 9999.0f;
        const float fNearDepthDivDepthRange = 1.0f / 9999.0f;

        // Projection matrix update
        m_kD3DProj._11 = 2.0f;
        m_kD3DProj._21 = 0.0f;
        m_kD3DProj._31 = 0.0f; 
        m_kD3DProj._41 = -1.0f / fWidth;
        m_kD3DProj._12 = 0.0f;
        m_kD3DProj._22 = 2.0f;
        m_kD3DProj._32 = 0.0f; 
        m_kD3DProj._42 = 1.0f / fHeight;
        m_kD3DProj._13 = 0.0f;
        m_kD3DProj._23 = 0.0f;
        m_kD3DProj._33 = fNearDepthDivDepthRange; 
        m_kD3DProj._43 = -fNearDepthDivDepthRange; 
        m_kD3DProj._14 = 0.0f;
        m_kD3DProj._24 = 0.0f;
        m_kD3DProj._34 = 0.0f;
        m_kD3DProj._44 = 1.0f;               

        m_pkD3DDevice9->SetTransform(D3DTS_PROJECTION, &m_kD3DProj);

        // Viewport update
        D3DVIEWPORT9 kD3DPort;

        if (pkPort)
        {
            kD3DPort.X = (unsigned int)(pkPort->m_left * fWidth);
            kD3DPort.Y = (unsigned int)((1.0f - pkPort->m_top) * fHeight);
            kD3DPort.Width = 
                (unsigned int)((pkPort->m_right - pkPort->m_left) * fWidth);
            kD3DPort.Height = 
                (unsigned int)((pkPort->m_top - pkPort->m_bottom) * fHeight);

            // Set cached port.
            m_kCachedPort = *pkPort;
        }
        else
        {
            kD3DPort.X = 0;
            kD3DPort.Y = 0;
            kD3DPort.Width = uiWidth;
            kD3DPort.Height = uiHeight;

            // Set cached port.
            m_kCachedPort = NiRect<float>(0.0f, 1.0f, 1.0f, 0.0f);
        }

        kD3DPort.MinZ = 0.0f;     
        kD3DPort.MaxZ = 1.0f;

        m_pkD3DDevice9->SetViewport(&kD3DPort);

        // Fog information to the camera
        NIASSERT(m_pkRenderState);
        m_pkRenderState->SetCameraNearAndFar(1.0f, 10000.0f);

        // Set cached frustum.
        const float fDoubleWidth = 2.0f * fWidth;
        const float fDoubleHeight = 2.0f * fHeight;
        m_kCachedFrustum.m_fLeft = -0.5f + 1.0f / fDoubleWidth;
        m_kCachedFrustum.m_fRight = 0.5f + 1.0f / fDoubleWidth;
        m_kCachedFrustum.m_fTop = 0.5f - 1.0f / fDoubleHeight;
        m_kCachedFrustum.m_fBottom = -0.5f - 1.0f / fDoubleHeight;
        m_kCachedFrustum.m_fNear = 1.0f;
        m_kCachedFrustum.m_fFar = 10000.0f;
        m_kCachedFrustum.m_bOrtho = true;
    }
}
//---------------------------------------------------------------------------
void NiDX9Renderer::Do_GetCameraData(NiPoint3& kWorldLoc,
    NiPoint3& kWorldDir, NiPoint3& kWorldUp, NiPoint3& kWorldRight,
    NiFrustum& kFrustum, NiRect<float>& kPort)
{
    if (!m_bDeviceLost)
    {
        kWorldRight.x = m_kInvView._11;
        kWorldRight.y = m_kInvView._12;
        kWorldRight.z = m_kInvView._13;

        kWorldUp.x = m_kInvView._21;
        kWorldUp.y = m_kInvView._22;
        kWorldUp.z = m_kInvView._23;

        kWorldDir.x = m_kInvView._31;
        kWorldDir.y = m_kInvView._32;
        kWorldDir.z = m_kInvView._33;

        kWorldLoc.x = m_kInvView._41;
        kWorldLoc.y = m_kInvView._42;
        kWorldLoc.z = m_kInvView._43;

        kFrustum = m_kCachedFrustum;
        kPort = m_kCachedPort;
    }
}
//---------------------------------------------------------------------------
void NiDX9Renderer::SetModelTransform(const NiTransform& kXform,
    bool bPushToDevice)
{
    NiD3DUtility::GetD3DFromNi(m_kD3DMat, kXform);

    if (bPushToDevice)
        m_pkD3DDevice9->SetTransform(D3DTS_WORLD, &m_kD3DMat);

    m_pkRenderState->SetModelTransform(kXform);

    NiMatrix3 kRotScale = kXform.m_Rotate*kXform.m_fScale;
    m_kModelCamRight = m_kCamRight*kRotScale;
    m_kModelCamUp = m_kCamUp*kRotScale;
}
//---------------------------------------------------------------------------
void NiDX9Renderer::SetSkinnedModelTransforms(const NiSkinInstance* pkSkin,
    const NiSkinPartition::Partition* pkPartition, const NiTransform& kWorld)
{
    NIASSERT(pkPartition->m_usBones == HW_BONE_LIMIT);

    m_pkRenderState->SetModelTransform(kWorld);

    NiD3DUtility::GetD3DFromNi(m_kD3DMat, kWorld);

    // Quick-out if there are more than HW_BONE_LIMIT
    if (pkPartition->m_usBones > HW_BONE_LIMIT)
        return;

    D3DMATRIX* pkBoneMatrics = (D3DMATRIX*)pkSkin->GetBoneMatrices();
    for (unsigned int i = 0; i < HW_BONE_LIMIT; i++)
    {
        unsigned int uiBone = pkPartition->m_pusBones[i];

        m_pkD3DDevice9->SetTransform(D3DTS_WORLDMATRIX(i),
            &pkBoneMatrics[uiBone]);
    }
}
//---------------------------------------------------------------------------
void NiDX9Renderer::CalculateBoneMatrices(
    const NiSkinInstance* pkSkinInstance, const NiTransform& kWorld, 
    bool bTranspose, unsigned int uiBoneMatrixRegisters, 
    bool bPalettizedBones)
{
    // Handle case where different shaders are used on same geometry in
    // same frame
    if (pkSkinInstance->GetFrameID() == m_pkVBManager->GetFrameID() &&
        uiBoneMatrixRegisters == pkSkinInstance->GetBoneMatrixRegisters())
    {
        return;
    }

    ((NiSkinInstance*)pkSkinInstance)->SetFrameID(m_pkVBManager->GetFrameID());

    const NiSkinData* pkSkinData = pkSkinInstance->GetSkinData();
    unsigned int uiBones = pkSkinData->GetBoneCount();

    unsigned int uiNumStoredBoneMatrices = 
        pkSkinInstance->GetNumBoneMatrices();
    float* pfBoneMatrices = (float*)pkSkinInstance->GetBoneMatrices();

    unsigned int uiNumStoredBoneMatrixRegisters = 
        pkSkinInstance->GetBoneMatrixRegisters();
    unsigned int uiCurrentAllocatedSize = 
        pkSkinInstance->GetBoneMatrixAllocatedSize();

    // If only using 3 registers, matrices must be transposed.
    if (uiBoneMatrixRegisters == 3)
        bTranspose = true;

    unsigned int uiNeededSize = uiBones * uiBoneMatrixRegisters * 4 * 4;

    if (uiNeededSize > uiCurrentAllocatedSize)
    {
        NiAlignedFree(pfBoneMatrices);
        pfBoneMatrices = NiAlignedAlloc(float, 
            uiBones * uiBoneMatrixRegisters * 4, 16);
        NIASSERT(pfBoneMatrices != NULL);
    
        ((NiSkinInstance*)pkSkinInstance)->SetBoneMatrices(
            pfBoneMatrices, uiNeededSize);
    }

    ((NiSkinInstance*)pkSkinInstance)->SetBoneMatrixInfo(
        uiBones, uiBoneMatrixRegisters);

    // Initialize constant elements of each new matrix:
    if (uiBoneMatrixRegisters == 4)
    {
        for (unsigned int i = 0; i < uiBones; i++)
        {
            unsigned int uiIndex = 16 * i;
            if (bTranspose)
            {
                pfBoneMatrices[uiIndex + 12] =
                    pfBoneMatrices[uiIndex + 13] =
                    pfBoneMatrices[uiIndex + 14] = 0.0f;
            }
            else
            {
                pfBoneMatrices[uiIndex + 3] =
                    pfBoneMatrices[uiIndex + 7] =
                    pfBoneMatrices[uiIndex + 11] = 0.0f;
            }
            pfBoneMatrices[uiIndex + 15] = 1.0f;
        }
    }

    NiTransform kSkinFromWorld, kWorldFromSkinFromWorld;
    pkSkinInstance->GetWorldToSkinTransform(kSkinFromWorld);
    kWorldFromSkinFromWorld = kWorld * kSkinFromWorld;

    bool bSetNormalizeNormals = false;

    if ((kSkinFromWorld.m_fScale < 0.99f) && 
        (kSkinFromWorld.m_fScale > 1.01f))
    {
        bSetNormalizeNormals = true;
    }

    if (bPalettizedBones)
    {
        D3DMATRIX* pkSkin2World_World2Skin = 
            (D3DMATRIX*)pkSkinInstance->GetSkinToWorldWorldToSkinMatrix();
        if (!pkSkin2World_World2Skin)
        {
            pkSkin2World_World2Skin = NiExternalNew D3DMATRIX;
            NIASSERT(pkSkin2World_World2Skin);
            ((NiSkinInstance*)pkSkinInstance)->SetSkinToWorldWorldToSkinMatrix(
                pkSkin2World_World2Skin);
            pkSkin2World_World2Skin->_14 = 0.0f;
            pkSkin2World_World2Skin->_24 = 0.0f;
            pkSkin2World_World2Skin->_34 = 0.0f;
            pkSkin2World_World2Skin->_44 = 1.0f;
        }

        NiD3DUtility::GetD3DFromNi(*pkSkin2World_World2Skin,
            kWorldFromSkinFromWorld);
    }

    NiAVObject*const* ppkBones = pkSkinInstance->GetBones();
    const NiSkinData::BoneData* pkBoneData = pkSkinData->GetBoneData();

    for (unsigned int i = 0; i < uiBones; i++)
    {
        NiTransform kBoneMatrix = 
            ppkBones[i]->GetWorldTransform() *
            pkBoneData[i].m_kSkinToBone;

        if ((ppkBones[i]->GetWorldScale() < 0.99f) && 
            (ppkBones[i]->GetWorldScale() > 1.01f))
        {
            bSetNormalizeNormals = true;
        }

        if (bPalettizedBones == false)
            kBoneMatrix = kWorldFromSkinFromWorld * kBoneMatrix;

        if (uiBoneMatrixRegisters == 4)
        {
            D3DMATRIX* pkD3DMat = &(((D3DMATRIX*)pfBoneMatrices)[i]);
            if (bTranspose)
                NiD3DUtility::GetD3DTransposeFromNi(*pkD3DMat, kBoneMatrix);
            else
                NiD3DUtility::GetD3DFromNi(*pkD3DMat, kBoneMatrix);
        }
        else
        {
            unsigned int uiIndex = 4 * 3 * i;
            //  Store it... directly in the 'shader constant' map
            pfBoneMatrices[uiIndex + 0] = 
                kBoneMatrix.m_Rotate.GetEntry(0,0) * kBoneMatrix.m_fScale;
            pfBoneMatrices[uiIndex + 4] = 
                kBoneMatrix.m_Rotate.GetEntry(1,0) * kBoneMatrix.m_fScale;
            pfBoneMatrices[uiIndex + 8] = 
                kBoneMatrix.m_Rotate.GetEntry(2,0) * kBoneMatrix.m_fScale;
            pfBoneMatrices[uiIndex + 1] = 
                kBoneMatrix.m_Rotate.GetEntry(0,1) * kBoneMatrix.m_fScale;
            pfBoneMatrices[uiIndex + 5] = 
                kBoneMatrix.m_Rotate.GetEntry(1,1) * kBoneMatrix.m_fScale;
            pfBoneMatrices[uiIndex + 9] = 
                kBoneMatrix.m_Rotate.GetEntry(2,1) * kBoneMatrix.m_fScale;
            pfBoneMatrices[uiIndex + 2] = 
                kBoneMatrix.m_Rotate.GetEntry(0,2) * kBoneMatrix.m_fScale;
            pfBoneMatrices[uiIndex + 6] = 
                kBoneMatrix.m_Rotate.GetEntry(1,2) * kBoneMatrix.m_fScale;
            pfBoneMatrices[uiIndex +10] = 
                kBoneMatrix.m_Rotate.GetEntry(2,2) * kBoneMatrix.m_fScale;
            pfBoneMatrices[uiIndex + 3] = kBoneMatrix.m_Translate.x;
            pfBoneMatrices[uiIndex + 7] = kBoneMatrix.m_Translate.y;
            pfBoneMatrices[uiIndex + 11] = kBoneMatrix.m_Translate.z;
        }
    }

    // Set the normalize normals flag in the render state
    m_pkRenderState->SetInternalNormalizeNormals(bSetNormalizeNormals);
}
//---------------------------------------------------------------------------
void NiDX9Renderer::Do_RenderShape(NiTriShape* pkTriShape)
{
    NIMETRICS_DX9RENDERER_SCOPETIMER(DRAW_TIME_SHAPE);

    NiTriShapeData* pkData = (NiTriShapeData*)pkTriShape->GetModelData();
    NIASSERT(pkData);

    unsigned int uiActiveVertexCount = pkData->GetActiveVertexCount();
    if (m_bDeviceLost || uiActiveVertexCount == 0)
        return;

    NiSkinInstance* pkSkinInstance = pkTriShape->GetSkinInstance();

    if (pkSkinInstance)
    {
        if (IsHardwareSkinned(pkTriShape))
        {
            // Hardware skinned
            NiGeometryGroup* pkGeometryGroup = m_pkDefaultGroup;
            if (pkData->GetConsistency() == NiGeometryData::VOLATILE)
                pkGeometryGroup = m_pkDynamicGroup;

            m_pkGeometryGroupManager->AddObjectToGroup(pkGeometryGroup,
                pkData, pkSkinInstance, true, 0, 0);

            DrawSkinnedPrimitive(pkTriShape, pkData, pkSkinInstance);
        }
        else
        {
            // Software skinned
            m_pkGeometryGroupManager->AddObjectToGroup(m_pkDynamicGroup,
                pkData, pkSkinInstance, false, 0, 0);

            DrawPrimitive(pkTriShape, pkData, NULL, 
                pkTriShape->GetWorldTransform(), pkTriShape->GetWorldBound(),
                (NiGeometryBufferData*)pkData->GetRendererData());
        }
    }
    else
    {
        // Unskinned
        NiGeometryGroup* pkGeometryGroup = m_pkDefaultGroup;
        if (pkData->GetConsistency() == NiGeometryData::VOLATILE)
            pkGeometryGroup = m_pkDynamicGroup;

        m_pkGeometryGroupManager->AddObjectToGroup(pkGeometryGroup,
            pkData, NULL, false, 0, 0);

        DrawPrimitive(pkTriShape, pkData, NULL, 
            pkTriShape->GetWorldTransform(), pkTriShape->GetWorldBound(),
            (NiGeometryBufferData*)pkData->GetRendererData());
    }
}
//---------------------------------------------------------------------------
void NiDX9Renderer::Do_RenderTristrips(NiTriStrips* pkTriStrips)
{
    NIMETRICS_DX9RENDERER_SCOPETIMER(DRAW_TIME_STRIPS);

    NiTriStripsData* pkData = (NiTriStripsData*)pkTriStrips->GetModelData();
    NIASSERT(pkData);

    unsigned int uiActiveVertexCount = pkData->GetActiveVertexCount();
    if (m_bDeviceLost || uiActiveVertexCount == 0)
        return;

    NiSkinInstance* pkSkinInstance = pkTriStrips->GetSkinInstance();

    if (pkSkinInstance)
    {
        if (IsHardwareSkinned(pkTriStrips))
        {
            // Hardware skinned
            NiGeometryGroup* pkGeometryGroup = m_pkDefaultGroup;
            if (pkData->GetConsistency() == NiGeometryData::VOLATILE)
                pkGeometryGroup = m_pkDynamicGroup;

            m_pkGeometryGroupManager->AddObjectToGroup(pkGeometryGroup,
                pkData, pkSkinInstance, true, 0, 0);

            DrawSkinnedPrimitive(pkTriStrips, pkData, pkSkinInstance);
        }
        else
        {
            // Software skinned
            m_pkGeometryGroupManager->AddObjectToGroup(m_pkDynamicGroup,
                pkData, pkSkinInstance, false, 0, 0);

            DrawPrimitive(pkTriStrips, pkData, NULL, 
                pkTriStrips->GetWorldTransform(), pkTriStrips->GetWorldBound(),
                (NiGeometryBufferData*)pkData->GetRendererData());
        }
    }
    else
    {
        // Unskinned
        NiGeometryGroup* pkGeometryGroup = m_pkDefaultGroup;
        if (pkData->GetConsistency() == NiGeometryData::VOLATILE)
            pkGeometryGroup = m_pkDynamicGroup;

        m_pkGeometryGroupManager->AddObjectToGroup(pkGeometryGroup,
            pkData, NULL, false, 0, 0);

        DrawPrimitive(pkTriStrips, pkData, NULL, 
            pkTriStrips->GetWorldTransform(), pkTriStrips->GetWorldBound(),
            (NiGeometryBufferData*)pkData->GetRendererData());
    }
}
//---------------------------------------------------------------------------
void NiDX9Renderer::Do_RenderPoints(NiParticles* pkParticles)
{
    NIMETRICS_DX9RENDERER_SCOPETIMER(DRAW_TIME_POINTS);

    if (m_bDeviceLost)
        return;

    NiParticlesData* pkData = (NiParticlesData*)pkParticles->GetModelData();
    NiTransform kWorld = pkParticles->GetWorldTransform();
    NiBound kWorldBound = pkParticles->GetWorldBound();

    NIASSERT(pkData);
    NIASSERT(m_pkCurrProp);

    unsigned short usP = pkData->GetActiveVertexCount();
    unsigned short usMaxP = pkData->GetVertexCount();

    if (usP == 0)
        return;

    NiD3DShaderInterface* pkShader = NULL;
    if (pkParticles)
    {
        const NiMaterial* pkTempMat = pkParticles->GetActiveMaterial();
        pkParticles->ApplyAndSetActiveMaterial(
            m_spLegacyDefaultShaderMaterial);
        pkShader = NiDynamicCast(NiD3DShaderInterface, 
            pkParticles->GetShaderFromMaterial());
        pkParticles->SetActiveMaterial(pkTempMat);
    }

    // If no shader is found, use error shader
    if (pkShader == NULL)
    {
        Error("Do_RenderPoints> No shader found for object %s!\n"
            "Using Error Shader!\n", pkParticles->GetName());
        NIASSERT(NiIsKindOf(NiD3DShaderInterface, GetErrorShader()));
        pkShader = (NiD3DShaderInterface*)GetErrorShader();
    }

    NIASSERT(pkShader);

    // Place object in dynamic group if it is not already in a group
    m_pkGeometryGroupManager->AddObjectToGroup(m_pkDynamicGroup, pkData, NULL,
        false, 0, 0);

    // for now, we need to use 4 times as many verts as points
    unsigned int uiVerts = usP << 2;
    unsigned int uiMaxVerts = usMaxP << 2;

    NiGeometryBufferData* pkBuffData = 
        (NiGeometryBufferData*)pkData->GetRendererData();
    pkBuffData->SetStreamCount(1);
    pkBuffData->SetVertCount(uiVerts, uiMaxVerts);
    pkBuffData->SetIndices(usP * 2, usMaxP * 2, NULL, NULL, 1);

    { // Set buffer flags
        bool bColors = true;
        bool bNorms = true;
        bool bBinormsTangents = false;
        unsigned int uiT = 1;
        pkBuffData->SetFlags(CreateVertexFlags(bColors, bNorms, 
            bBinormsTangents, uiT));
    }

    // a static array of connectivities: 0,1,2,3,4,5,6,7...
    LPDIRECT3DINDEXBUFFER9 pkSrcIB = 
        m_pkIBManager->ResizeTempPointConnectivity(uiVerts);
    if (pkSrcIB == 0)
        return;

    // now, pack the incoming data into the D3D vertices
    NiColorA* pkC = pkData->GetColors();
    float* pfR = pkData->GetRadii();
    float* pfS = pkData->GetSizes();
    NiPoint3* pkP = pkData->GetVertices();
    float* pfA = pkData->GetRotationAngles();
    // Ignore particle normals

    // Manually set the transforms (geometry and camera) before making sprites
    m_pkRenderState->SetBoneCount(0);

    // This code replaces a call to SetModelTransform. The only difference
    // is that the scale is not applied to m_kModelCamRight and m_kModelCamUp.
    NiD3DUtility::GetD3DFromNi(m_kD3DMat, kWorld);
    m_pkD3DDevice9->SetTransform(D3DTS_WORLD, &m_kD3DMat);
    m_pkRenderState->SetModelTransform(kWorld);
    m_kModelCamRight = m_kCamRight * kWorld.m_Rotate;
    m_kModelCamUp = m_kCamUp * kWorld.m_Rotate;

    // The normal vector for each quad.
    const NiPoint3 kModelNormal = m_kModelCamRight.Cross(m_kModelCamUp);

    // Grab dynamic chip
    pkBuffData->SetFVF(SourceNormalVertexFVF);
    pkBuffData->SetVertexStride(0, sizeof(SourceNormalVertex));
    m_pkVBManager->AllocateBufferSpace(pkBuffData, 0);
    NiVBChip* pkChip = pkBuffData->GetVBChip(0);

    if (!pkChip->GetVB())
        return;

    unsigned int uiSize = sizeof(SourceNormalVertex) * uiVerts;
    NIASSERT(uiSize <= pkChip->GetSize());

    SourceNormalVertex* pkVerts = (SourceNormalVertex*)m_pkVBManager->LockVB(
        pkChip->GetVB(), pkChip->GetOffset(), uiSize, 
        pkChip->GetLockFlags());

    if (pkVerts == NULL)
        return;

    if (pfA)
    {
        if (pkC)
        {
            unsigned int i;
            for (i = 0; i < usP; i++)
            {
                unsigned int uiColor = 
                    (FastFloatToInt(pkC->a*255.0f) << 24) |
                    (FastFloatToInt(pkC->r*255.0f) << 16) |
                    (FastFloatToInt(pkC->g*255.0f) << 8) |
                    FastFloatToInt(pkC->b*255.0f);

                float fSize = (*pfR) * (*pfS);

                // Compute the rotated top left and top right offset vectors.
                float fSinA, fCosA;
                NiSinCos(*pfA, fSinA, fCosA);
                float fC1 = fSize * (fCosA + fSinA);
                float fC2 = fSize * (fCosA - fSinA);
                const NiPoint3 kV0 = fC1 * m_kModelCamRight + fC2 *
                    m_kModelCamUp;
                const NiPoint3 kV1 = -fC2 * m_kModelCamRight + fC1 *
                    m_kModelCamUp;

                const NiPoint3 kCorner00 = *pkP - kV0;
                pkVerts->fX = kCorner00.x;
                pkVerts->fY = kCorner00.y;
                pkVerts->fZ = kCorner00.z;
                pkVerts->fNX = kModelNormal.x;
                pkVerts->fNY = kModelNormal.y;
                pkVerts->fNZ = kModelNormal.z;
                pkVerts->kColor = uiColor;
                pkVerts->fTU = 0.0f;
                pkVerts->fTV = 1.0f;
                pkVerts++;

                const NiPoint3 kCorner01 = *pkP - kV1;
                pkVerts->fX = kCorner01.x;
                pkVerts->fY = kCorner01.y;
                pkVerts->fZ = kCorner01.z;
                pkVerts->fNX = kModelNormal.x;
                pkVerts->fNY = kModelNormal.y;
                pkVerts->fNZ = kModelNormal.z;
                pkVerts->kColor = uiColor;
                pkVerts->fTU = 1.0f;
                pkVerts->fTV = 1.0f;
                pkVerts++;

                const NiPoint3 kCorner10 = *pkP + kV0;
                pkVerts->fX = kCorner10.x;
                pkVerts->fY = kCorner10.y;
                pkVerts->fZ = kCorner10.z;
                pkVerts->fNX = kModelNormal.x;
                pkVerts->fNY = kModelNormal.y;
                pkVerts->fNZ = kModelNormal.z;
                pkVerts->kColor = uiColor;
                pkVerts->fTU = 1.0f;
                pkVerts->fTV = 0.0f;
                pkVerts++;

                const NiPoint3 kCorner11 = *pkP + kV1;
                pkVerts->fX = kCorner11.x;
                pkVerts->fY = kCorner11.y;
                pkVerts->fZ = kCorner11.z;
                pkVerts->fNX = kModelNormal.x;
                pkVerts->fNY = kModelNormal.y;
                pkVerts->fNZ = kModelNormal.z;
                pkVerts->kColor = uiColor;
                pkVerts->fTU = 0.0f;
                pkVerts->fTV = 0.0f;
                pkVerts++;

                pkP++;
                pkC++;
                pfR++;
                pfS++;
                pfA++;
            }
        }
        else
        {
            unsigned int i;
            for (i = 0; i < usP; i++)
            {
                float fSize = (*pfR) * (*pfS);

                // Compute the rotated top left and top right offset vectors.
                float fSinA, fCosA;
                NiSinCos(*pfA, fSinA, fCosA);
                float fC1 = fSize * (fCosA + fSinA);
                float fC2 = fSize * (fCosA - fSinA);
                const NiPoint3 kV0 = fC1 * m_kModelCamRight + fC2 *
                    m_kModelCamUp;
                const NiPoint3 kV1 = -fC2 * m_kModelCamRight + fC1 *
                    m_kModelCamUp;

                const NiPoint3 kCorner00 = *pkP - kV0;
                pkVerts->fX = kCorner00.x;
                pkVerts->fY = kCorner00.y;
                pkVerts->fZ = kCorner00.z;
                pkVerts->fNX = kModelNormal.x;
                pkVerts->fNY = kModelNormal.y;
                pkVerts->fNZ = kModelNormal.z;
                pkVerts->kColor = 0xffffffff;
                pkVerts->fTU = 0.0f;
                pkVerts->fTV = 1.0f;
                pkVerts++;

                const NiPoint3 kCorner01 = *pkP - kV1;
                pkVerts->fX = kCorner01.x;
                pkVerts->fY = kCorner01.y;
                pkVerts->fZ = kCorner01.z;
                pkVerts->fNX = kModelNormal.x;
                pkVerts->fNY = kModelNormal.y;
                pkVerts->fNZ = kModelNormal.z;
                pkVerts->kColor = 0xffffffff;
                pkVerts->fTU = 1.0f;
                pkVerts->fTV = 1.0f;
                pkVerts++;

                const NiPoint3 kCorner10 = *pkP + kV0;
                pkVerts->fX = kCorner10.x;
                pkVerts->fY = kCorner10.y;
                pkVerts->fZ = kCorner10.z;
                pkVerts->fNX = kModelNormal.x;
                pkVerts->fNY = kModelNormal.y;
                pkVerts->fNZ = kModelNormal.z;
                pkVerts->kColor = 0xffffffff;
                pkVerts->fTU = 1.0f;
                pkVerts->fTV = 0.0f;
                pkVerts++;

                const NiPoint3 kCorner11 = *pkP + kV1;
                pkVerts->fX = kCorner11.x;
                pkVerts->fY = kCorner11.y;
                pkVerts->fZ = kCorner11.z;
                pkVerts->fNX = kModelNormal.x;
                pkVerts->fNY = kModelNormal.y;
                pkVerts->fNZ = kModelNormal.z;
                pkVerts->kColor = 0xffffffff;
                pkVerts->fTU = 0.0f;
                pkVerts->fTV = 0.0f;
                pkVerts++;

                pkP++;
                pfR++;
                pfS++;
                pfA++;
            }
        }
    }
    else
    {
        // Top left and top right corner offset vectors - the others are
        // just + and - of these.
        const NiPoint3 kModelTopLeft = m_kModelCamUp - m_kModelCamRight;
        const NiPoint3 kModelTopRight = m_kModelCamUp + m_kModelCamRight;

        if (pkC)
        {
            unsigned int i;
            for (i = 0; i < usP; i++)
            {
                unsigned int uiColor = 
                    (FastFloatToInt(pkC->a*255.0f) << 24) |
                    (FastFloatToInt(pkC->r*255.0f) << 16) |
                    (FastFloatToInt(pkC->g*255.0f) << 8) |
                    FastFloatToInt(pkC->b*255.0f);

                float fSize = (*pfR) * (*pfS);
                const NiPoint3 kV0 = fSize * kModelTopRight;
                const NiPoint3 kV1 = fSize * kModelTopLeft;

                const NiPoint3 kCorner00 = *pkP - kV0;
                pkVerts->fX = kCorner00.x;
                pkVerts->fY = kCorner00.y;
                pkVerts->fZ = kCorner00.z;
                pkVerts->fNX = kModelNormal.x;
                pkVerts->fNY = kModelNormal.y;
                pkVerts->fNZ = kModelNormal.z;
                pkVerts->kColor = uiColor;
                pkVerts->fTU = 0.0f;
                pkVerts->fTV = 1.0f;
                pkVerts++;

                const NiPoint3 kCorner01 = *pkP - kV1;
                pkVerts->fX = kCorner01.x;
                pkVerts->fY = kCorner01.y;
                pkVerts->fZ = kCorner01.z;
                pkVerts->fNX = kModelNormal.x;
                pkVerts->fNY = kModelNormal.y;
                pkVerts->fNZ = kModelNormal.z;
                pkVerts->kColor = uiColor;
                pkVerts->fTU = 1.0f;
                pkVerts->fTV = 1.0f;
                pkVerts++;

                const NiPoint3 kCorner10 = *pkP + kV0;
                pkVerts->fX = kCorner10.x;
                pkVerts->fY = kCorner10.y;
                pkVerts->fZ = kCorner10.z;
                pkVerts->fNX = kModelNormal.x;
                pkVerts->fNY = kModelNormal.y;
                pkVerts->fNZ = kModelNormal.z;
                pkVerts->kColor = uiColor;
                pkVerts->fTU = 1.0f;
                pkVerts->fTV = 0.0f;
                pkVerts++;

                const NiPoint3 kCorner11 = *pkP + kV1;
                pkVerts->fX = kCorner11.x;
                pkVerts->fY = kCorner11.y;
                pkVerts->fZ = kCorner11.z;
                pkVerts->fNX = kModelNormal.x;
                pkVerts->fNY = kModelNormal.y;
                pkVerts->fNZ = kModelNormal.z;
                pkVerts->kColor = uiColor;
                pkVerts->fTU = 0.0f;
                pkVerts->fTV = 0.0f;
                pkVerts++;

                pkP++;
                pkC++;
                pfR++;
                pfS++;
            }
        }
        else
        {
            unsigned int i;
            for (i = 0; i < usP; i++)
            {
                float fSize = (*pfR) * (*pfS);
                const NiPoint3 kV0 = fSize * kModelTopRight;
                const NiPoint3 kV1 = fSize * kModelTopLeft;

                const NiPoint3 kCorner00 = *pkP - kV0;
                pkVerts->fX = kCorner00.x;
                pkVerts->fY = kCorner00.y;
                pkVerts->fZ = kCorner00.z;
                pkVerts->fNX = kModelNormal.x;
                pkVerts->fNY = kModelNormal.y;
                pkVerts->fNZ = kModelNormal.z;
                pkVerts->kColor = 0xffffffff;
                pkVerts->fTU = 0.0f;
                pkVerts->fTV = 1.0f;
                pkVerts++;

                const NiPoint3 kCorner01 = *pkP - kV1;
                pkVerts->fX = kCorner01.x;
                pkVerts->fY = kCorner01.y;
                pkVerts->fZ = kCorner01.z;
                pkVerts->fNX = kModelNormal.x;
                pkVerts->fNY = kModelNormal.y;
                pkVerts->fNZ = kModelNormal.z;
                pkVerts->kColor = 0xffffffff;
                pkVerts->fTU = 1.0f;
                pkVerts->fTV = 1.0f;
                pkVerts++;

                const NiPoint3 kCorner10 = *pkP + kV0;
                pkVerts->fX = kCorner10.x;
                pkVerts->fY = kCorner10.y;
                pkVerts->fZ = kCorner10.z;
                pkVerts->fNX = kModelNormal.x;
                pkVerts->fNY = kModelNormal.y;
                pkVerts->fNZ = kModelNormal.z;
                pkVerts->kColor = 0xffffffff;
                pkVerts->fTU = 1.0f;
                pkVerts->fTV = 0.0f;
                pkVerts++;

                const NiPoint3 kCorner11 = *pkP + kV1;
                pkVerts->fX = kCorner11.x;
                pkVerts->fY = kCorner11.y;
                pkVerts->fZ = kCorner11.z;
                pkVerts->fNX = kModelNormal.x;
                pkVerts->fNY = kModelNormal.y;
                pkVerts->fNZ = kModelNormal.z;
                pkVerts->kColor = 0xffffffff;
                pkVerts->fTU = 0.0f;
                pkVerts->fTV = 0.0f;
                pkVerts++;

                pkP++;
                pfR++;
                pfS++;
            }
        }
    }

    m_pkVBManager->UnlockVB(pkChip->GetVB());

    unsigned int uiRet;

    // Preprocess the pipeline
    uiRet = pkShader->PreProcessPipeline(NULL, NULL, pkBuffData, m_pkCurrProp, 
        m_pkCurrEffects, kWorld, kWorldBound);

    if (uiRet != 0)
    {
        NIASSERT(!"PreProcess failed - skipping render");
        return;
    }

    // Update the pipeline
    uiRet = pkShader->UpdatePipeline(NULL, NULL, pkBuffData, m_pkCurrProp, 
        m_pkCurrEffects, kWorld, kWorldBound);

    unsigned int uiRemainingPasses = pkShader->FirstPass();

    // Prepare the geometry for rendering manually
    m_pkD3DDevice9->SetStreamSource(0, pkChip->GetVB(), 0,
        pkBuffData->GetVertexStride(0));
    m_pkD3DDevice9->SetIndices(pkSrcIB);

    unsigned int uiMetricDPCalls = 0;

    //uiPasses
    while (uiRemainingPasses != 0)
    {
        // Setup the rendering pass
        uiRet = pkShader->SetupRenderingPass(NULL, NULL, pkBuffData, 
            m_pkCurrProp, m_pkCurrEffects, kWorld, kWorldBound);

        // Transforms and geometry set earlier.

        // Set the shader programs
        // This is to give the shader final 'override' authority
        uiRet = pkShader->SetupShaderPrograms(NULL, NULL, NULL, pkBuffData, 
            m_pkCurrProp, m_pkCurrEffects, kWorld, kWorldBound);

        NIASSERT(pkSrcIB);

        m_pkRenderState->CommitShaderConstants();

        m_pkD3DDevice9->DrawIndexedPrimitive(pkBuffData->GetType(), 
            pkBuffData->GetBaseVertexIndex(), 0, 
            pkBuffData->GetVertCount(), 0, pkBuffData->GetTriCount());

        uiMetricDPCalls++;
        NIMETRICS_DX9RENDERER_AGGREGATEVALUE(DRAW_TRIS, 
            pkBuffData->GetTriCount());
        NIMETRICS_DX9RENDERER_AGGREGATEVALUE(DRAW_VERTS, 
            pkBuffData->GetVertCount());

        // Perform any post-rendering steps
        uiRet = pkShader->PostRender(NULL, NULL, NULL, pkBuffData, 
            m_pkCurrProp, m_pkCurrEffects, kWorld, kWorldBound);

        // Inform the shader to move to the next pass
        uiRemainingPasses = pkShader->NextPass();
    }

    // PostProcess the pipeline
    pkShader->PostProcessPipeline(NULL, NULL, pkBuffData, 
        m_pkCurrProp, m_pkCurrEffects, kWorld, kWorldBound);

    NIMETRICS_DX9RENDERER_AGGREGATEVALUE(DRAW_PRIMITIVE, uiMetricDPCalls);
}
//---------------------------------------------------------------------------
void NiDX9Renderer::Do_RenderLines(NiLines* pkLines)
{
    NIMETRICS_DX9RENDERER_SCOPETIMER(DRAW_TIME_LINES);

    if (m_bDeviceLost)
        return;

    NiLinesData* pkData = (NiLinesData*)pkLines->GetModelData();
    NIASSERT(pkData);

    unsigned short usP = pkData->GetVertexCount();
    if (usP == 0)
        return;

    NiTransform kWorld = pkLines->GetWorldTransform();
    NiBound kWorldBound = pkLines->GetWorldBound();

    // Place object in dynamic group if it is not already in a group
    m_pkGeometryGroupManager->AddObjectToGroup(m_pkDynamicGroup, pkData, NULL,
        false, 0, 0);

    NiBool* pkL = pkData->GetFlags();    
    NiPoint3* pkP = pkData->GetVertices();    
    NiPoint2* pkT = pkData->GetTextures();    
    NiColorA* pkC = pkData->GetColors();    

    unsigned int uiConn = 0;
    // Pack the index buffer
    LPDIRECT3DINDEXBUFFER9 pkSrcIB = 
        m_pkIBManager->ResizeTempLineConnectivity(usP, pkL, uiConn);
    if (pkSrcIB == 0 || uiConn == 0)
        return;
    unsigned int uiVertexFVF = D3DFVF_XYZ;
    unsigned int uiVertexStride = sizeof(float) * 3;

    if (pkT)
    {
        uiVertexFVF |= D3DFVF_TEXCOORDSIZE2(0) | D3DFVF_TEX1;
        uiVertexStride += sizeof(float) * 2;
    }
    if (pkC)
    {
        uiVertexFVF |= D3DFVF_DIFFUSE;
        uiVertexStride += sizeof(unsigned int);
    }

    NiD3DShaderInterface* pkShader = NULL;
    if (pkLines)
    {
        const NiMaterial* pkTempMat = pkLines->GetActiveMaterial();
        pkLines->ApplyAndSetActiveMaterial(m_spLegacyDefaultShaderMaterial);
        pkShader = NiDynamicCast(NiD3DShaderInterface, 
            pkLines->GetShaderFromMaterial());
        pkLines->SetActiveMaterial(pkTempMat);
    }

    // If no shader is found, use error shader
    if (pkShader == NULL)
    {
        Error("Do_RenderLines> No shader found for object %s!\n"
            "Using Error Shader!\n", pkLines->GetName());
        NIASSERT(NiIsKindOf(NiD3DShaderInterface, GetErrorShader()));
        pkShader = (NiD3DShaderInterface*)GetErrorShader();
    }

    NIASSERT(pkShader);

    unsigned long ulSize = usP * uiVertexStride;
    NiGeometryBufferData* pkBuffData = 
        (NiGeometryBufferData*)pkData->GetRendererData();

    // Grab dynamic chip
    pkBuffData->SetStreamCount(1);
    pkBuffData->SetVertCount(usP, usP);
    pkBuffData->SetFVF(uiVertexFVF);
    pkBuffData->SetVertexStride(0, uiVertexStride);
    m_pkVBManager->AllocateBufferSpace(pkBuffData, 0);
    NiVBChip* pkChip = pkBuffData->GetVBChip(0);

    if (!pkChip->GetVB())
    {
        NIASSERT(!"RenderLines> No VB!");
        return;
    }

    // Now, we need to lock the buffer and fill it...
    if (pkT && pkC)
    {
        SourceVertex* pkVerts = (SourceVertex*)m_pkVBManager->LockVB(
            pkChip->GetVB(), pkChip->GetOffset(), pkChip->GetSize(), 
            pkChip->GetLockFlags());

        if (pkVerts == NULL)
            return;

        // Loop through and fill it...
        for (unsigned int ui = 0; ui < usP; ui++)
        {
            pkVerts->fX       = pkP->x;
            pkVerts->fY       = pkP->y;
            pkVerts->fZ       = pkP->z;
            pkVerts->kColor   = 
                (FastFloatToInt(pkC->a * 255.0f) << 24) |
                (FastFloatToInt(pkC->r * 255.0f) << 16) |
                (FastFloatToInt(pkC->g * 255.0f) <<  8) |
                 FastFloatToInt(pkC->b * 255.0f);
            pkVerts->fTU      = pkT->x;
            pkVerts->fTV      = pkT->y;

            pkVerts++;
            pkP++;
            pkC++;
            pkT++;
        }
    }
    else if (pkT)
    {
        TexturedLineVertex* pkVerts = 
            (TexturedLineVertex*)m_pkVBManager->LockVB(pkChip->GetVB(), 
            pkChip->GetOffset(), pkChip->GetSize(), pkChip->GetLockFlags());

        if (pkVerts == NULL)
            return;

        // Loop through and fill it...
        for (unsigned int ui = 0; ui < usP; ui++)
        {
            pkVerts->fX       = pkP->x;
            pkVerts->fY       = pkP->y;
            pkVerts->fZ       = pkP->z;
            pkVerts->fTU      = pkT->x;
            pkVerts->fTV      = pkT->y;

            pkVerts++;
            pkP++;
            pkT++;
        }
    }
    else if (pkC)
    {
        ColoredLineVertex* pkVerts = 
            (ColoredLineVertex*)m_pkVBManager->LockVB(pkChip->GetVB(), 
            pkChip->GetOffset(), pkChip->GetSize(), pkChip->GetLockFlags());

        if (pkVerts == NULL)
            return;

        // Loop through and fill it...
        for (unsigned int ui = 0; ui < usP; ui++)
        {
            pkVerts->fX       = pkP->x;
            pkVerts->fY       = pkP->y;
            pkVerts->fZ       = pkP->z;
            pkVerts->kColor   = 
                (FastFloatToInt(pkC->a * 255.0f) << 24) |
                (FastFloatToInt(pkC->r * 255.0f) << 16) |
                (FastFloatToInt(pkC->g * 255.0f) <<  8) |
                 FastFloatToInt(pkC->b * 255.0f);

            pkVerts++;
            pkP++;
            pkC++;
        }
    }
    else
    {
        SimpleLineVertex* pkVerts = 
            (SimpleLineVertex*)m_pkVBManager->LockVB(pkChip->GetVB(), 
            pkChip->GetOffset(), pkChip->GetSize(), pkChip->GetLockFlags());

        if (pkVerts == NULL)
            return;

        // Loop through and fill it...
        for (unsigned int ui = 0; ui < usP; ui++)
        {
            pkVerts->fX       = pkP->x;
            pkVerts->fY       = pkP->y;
            pkVerts->fZ       = pkP->z;

            pkVerts++;
            pkP++;
        }
    }

    m_pkVBManager->UnlockVB(pkChip->GetVB());

    // Render them
    unsigned int uiRet;

    // Preprocess the pipeline
    uiRet = pkShader->PreProcessPipeline(NULL, NULL, pkBuffData, m_pkCurrProp, 
        m_pkCurrEffects, kWorld, kWorldBound);

    if (uiRet != 0)
    {
        NIASSERT(!"PreProcess failed - skipping render");
        return;
    }

    // Update the pipeline
    uiRet = pkShader->UpdatePipeline(NULL, NULL, pkBuffData, m_pkCurrProp, 
        m_pkCurrEffects, kWorld, kWorldBound);

    unsigned int uiRemainingPasses = pkShader->FirstPass();

    // Prepare the geometry for rendering manually
    m_pkD3DDevice9->SetStreamSource(0, pkChip->GetVB(), 0, 
        pkBuffData->GetVertexStride(0));
    m_pkD3DDevice9->SetIndices(pkSrcIB);

    unsigned int uiMetricDPCalls = 0;

    //uiPasses
    while (uiRemainingPasses != 0)
    {
        // Setup the rendering pass
        uiRet = pkShader->SetupRenderingPass(NULL, NULL, pkBuffData, 
            m_pkCurrProp, m_pkCurrEffects, kWorld, kWorldBound);

        // Set the transformations
        uiRet = pkShader->SetupTransformations(NULL, NULL, NULL, pkBuffData, 
            m_pkCurrProp, m_pkCurrEffects, kWorld, kWorldBound);

        // Set the shader programs
        // This is to give the shader final 'override' authority
        uiRet = pkShader->SetupShaderPrograms(NULL, NULL, NULL, pkBuffData, 
            m_pkCurrProp, m_pkCurrEffects, kWorld, kWorldBound);

        NIASSERT(pkSrcIB);

        m_pkRenderState->CommitShaderConstants();

        m_pkD3DDevice9->DrawIndexedPrimitive(D3DPT_LINELIST, 
            pkBuffData->GetBaseVertexIndex(), 0, 
            pkBuffData->GetVertCount(), 0, uiConn / 2);

        uiMetricDPCalls++;
        NIMETRICS_DX9RENDERER_AGGREGATEVALUE(DRAW_VERTS, 
            pkBuffData->GetVertCount());

        // Perform any post-rendering steps
        uiRet = pkShader->PostRender(NULL, NULL, NULL, pkBuffData, 
            m_pkCurrProp, m_pkCurrEffects, kWorld, kWorldBound);

        // Inform the shader to move to the next pass
        uiRemainingPasses = pkShader->NextPass();
    }

    // PostProcess the pipeline
    pkShader->PostProcessPipeline(NULL, NULL, pkBuffData, 
        m_pkCurrProp, m_pkCurrEffects, kWorld, kWorldBound);

    NIMETRICS_DX9RENDERER_AGGREGATEVALUE(DRAW_PRIMITIVE, uiMetricDPCalls);
}
//---------------------------------------------------------------------------
void NiDX9Renderer::Do_RenderScreenTexture(NiScreenTexture* pkScreenTexture)
{
    NIMETRICS_DX9RENDERER_SCOPETIMER(DRAW_TIME_SCREENTEXTURE);

    if (m_bDeviceLost)
        return;

    if (pkScreenTexture == 0 || 
        pkScreenTexture->GetNumScreenRects() == 0)
    {
        return;
    }

    // Grab current set of properties
    NiPropertyState* pkOldProp = m_pkCurrProp;
    NiDynamicEffectState* pkOldEffects = m_pkCurrEffects;

    // Set up special set of properties (with CTD) for screen textures
    if (m_spScreenTexturePropertyState == 0)
    {
        m_spScreenTexturePropertyState = NiNew NiPropertyState;

        // use vertex colors
        NiVertexColorProperty* pkVertex = NiNew NiVertexColorProperty;
        pkVertex->SetSourceMode(NiVertexColorProperty::SOURCE_EMISSIVE);
        pkVertex->SetLightingMode(NiVertexColorProperty::LIGHTING_E);
        m_spScreenTexturePropertyState->SetProperty(pkVertex);

        // use alpha blending
        NiAlphaProperty* pkAlpha = NiNew NiAlphaProperty;
        pkAlpha->SetAlphaBlending(true);
        pkAlpha->SetSrcBlendMode(NiAlphaProperty::ALPHA_SRCALPHA);
        pkAlpha->SetDestBlendMode(NiAlphaProperty::ALPHA_INVSRCALPHA);
        m_spScreenTexturePropertyState->SetProperty(pkAlpha);

        // disable z-buffering
        NiZBufferProperty* pkZBuf = NiNew NiZBufferProperty;
        pkZBuf->SetZBufferTest(false);
        pkZBuf->SetZBufferWrite(false);
        m_spScreenTexturePropertyState->SetProperty(pkZBuf);
    }

    m_spScreenTexturePropertyState->SetProperty(
        pkScreenTexture->GetTexturingProperty()); 

    // Set property state
    m_pkCurrProp = m_spScreenTexturePropertyState;
    m_pkCurrEffects = NULL;

    // Place object in default group if it is not already in a group
    m_pkGeometryGroupManager->AddObjectToGroup(m_pkDefaultGroup, 
        pkScreenTexture);

    PackScreenTextureGeomBuffer(pkScreenTexture);

    NiTransform kWorld;
    kWorld.m_Rotate = NiMatrix3::IDENTITY;
    kWorld.m_Translate = NiPoint3::ZERO;
    kWorld.m_fScale = 1.0f;
    NiBound kWorldBound;

    NiGeometryBufferData* pkBuffData = 
        (NiGeometryBufferData*)pkScreenTexture->GetRendererData();
    NIASSERT(pkBuffData);

    DrawPrimitive(NULL, NULL, NULL, kWorld, kWorldBound, pkBuffData);

    // Allow for freeing of screen texture
    m_spScreenTexturePropertyState->SetProperty(
        NiTexturingProperty::GetDefault());

    // Reset original set of properties and effects
    m_pkCurrProp = pkOldProp;
    m_pkCurrEffects = pkOldEffects;
}
//---------------------------------------------------------------------------
// Batch rendering
//---------------------------------------------------------------------------
NiDX9Renderer::BatchedObject* 
    NiDX9Renderer::BatchedObject::ms_pkFreeList = NULL;
//---------------------------------------------------------------------------
NiDX9Renderer::BatchedObject* NiDX9Renderer::BatchedObject::GetNextAvailable()
{
    if (ms_pkFreeList == 0)
    {
        return NiNew BatchedObject;
    }
    else
    {
        BatchedObject* pkBatchedObject = ms_pkFreeList;
        ms_pkFreeList = pkBatchedObject->m_pkNext;
        pkBatchedObject->m_pkNext = NULL;
        return pkBatchedObject;
    }
}
//---------------------------------------------------------------------------
void NiDX9Renderer::BatchedObject::Release()
{
    BatchedObject* pkOldFreeList = ms_pkFreeList;
    ms_pkFreeList = this;
    m_pkNext = pkOldFreeList;
}
//---------------------------------------------------------------------------
void NiDX9Renderer::BatchedObject::Purge()
{
    BatchedObject* pkIterator = ms_pkFreeList;
    while (pkIterator)
    {
        BatchedObject* pkNext = pkIterator->m_pkNext;
        NiDelete pkIterator;
        pkIterator = pkNext;
    }
    ms_pkFreeList = NULL;
}
//---------------------------------------------------------------------------
void NiDX9Renderer::Do_BeginBatch(NiPropertyState* pkPropertyState,
    NiDynamicEffectState* pkEffectState)
{
    NIASSERT(m_pkBatchHead == 0);

    m_pkBatchedPropertyState = pkPropertyState;
    m_pkBatchedEffectState = pkEffectState;
}
//---------------------------------------------------------------------------
void NiDX9Renderer::Do_EndBatch()
{
    if (m_pkBatchHead == 0 || m_bDeviceLost)
    {
        m_spBatchedShader = 0;
        m_pkBatchedPropertyState = 0;
        m_pkBatchedEffectState = 0;
        EmptyBatch();
        return;
    }
    
    // Batched objects are expected to have the _same_ render states -
    // including number of texture passes, skinning, presence of 
    // normals/colors, etc.  Essentially, transforms and actual vertex
    // data values are all that can be different.

    // Set up render state for first batched object
    BatchedObject* pkCurrObject = m_pkBatchHead;
    NiGeometryBufferData* pkBuffData = pkCurrObject->m_pkBuffData;
    NiGeometry* pkGeometry = pkCurrObject->m_pkGeometry;
    const NiGeometryData* pkData = pkGeometry->GetModelData();
    const NiSkinInstance* pkSkinInstance = pkGeometry->GetSkinInstance();
    const NiTransform* pkWorld = &pkGeometry->GetWorldTransform();
    const NiBound* pkWorldBound = &pkGeometry->GetWorldBound();

    NIASSERT(m_pkBatchedPropertyState);
    NIASSERT(m_spBatchedShader);

    unsigned int uiRet;

    // Preprocess the pipeline
    uiRet = m_spBatchedShader->PreProcessPipeline(pkGeometry, pkSkinInstance, 
        pkBuffData, m_pkBatchedPropertyState, m_pkBatchedEffectState, 
        *pkWorld, *pkWorldBound);

    if (uiRet != 0)
    {
        NIASSERT(!"PreProcess failed - skipping render");
        m_spBatchedShader = 0;
        m_pkBatchedPropertyState = 0;
        m_pkBatchedEffectState = 0;
        EmptyBatch();
        return;
    }

    // Update the pipeline
    uiRet = m_spBatchedShader->UpdatePipeline(pkGeometry, pkSkinInstance, 
        pkBuffData, m_pkBatchedPropertyState, m_pkBatchedEffectState, 
        *pkWorld, *pkWorldBound);

    unsigned int uiRemainingPasses = m_spBatchedShader->FirstPass();
    bool bFirstBoneMatrixUpdate = true;

    unsigned int uiMetricDPCalls = 0;

    //uiPasses
    while (uiRemainingPasses != 0)
    {
        pkCurrObject = m_pkBatchHead;
        pkBuffData = pkCurrObject->m_pkBuffData;
        pkGeometry = pkCurrObject->m_pkGeometry;
        pkData = pkGeometry->GetModelData();
        pkSkinInstance = pkGeometry->GetSkinInstance();
        pkWorld = &pkGeometry->GetWorldTransform();
        pkWorldBound = &pkGeometry->GetWorldBound();

        // Setup the rendering pass
        uiRet = m_spBatchedShader->SetupRenderingPass(pkGeometry, 
            pkSkinInstance, pkBuffData, m_pkBatchedPropertyState, 
            m_pkBatchedEffectState, *pkWorld, *pkWorldBound);

        // For each batched object (or partition)
        while (pkCurrObject)
        {
            pkBuffData = pkCurrObject->m_pkBuffData;
            pkGeometry = pkCurrObject->m_pkGeometry;
            NiSkinPartition::Partition* pkPartition = 
                pkCurrObject->m_pkCurrPartition;
            pkData = pkGeometry->GetModelData();
            pkSkinInstance = pkGeometry->GetSkinInstance();
            pkWorld = &pkGeometry->GetWorldTransform();
            pkWorldBound = &pkGeometry->GetWorldBound();

            // This should be a more generic check that compares the
            // number of bones the shader supports with the number of bones
            // in the partition
            if (m_spBatchedShader == m_spLegacyDefaultShader && pkPartition &&
                pkPartition->m_usBones > m_uiHWBones)
            {
                // Can't render a partition with too many bones without
                // a shader
                Warning("EndBatch> Cannot render "
                    "partition with %d bones without a shader!\n", 
                    pkPartition->m_usBones);

                continue;
            }

            // Set the transformations
            uiRet = m_spBatchedShader->SetupTransformations(pkGeometry, 
                pkSkinInstance, pkPartition, pkBuffData, 
                m_pkBatchedPropertyState, m_pkBatchedEffectState, *pkWorld, 
                *pkWorldBound);

            // Prepare the geometry for rendering
            pkBuffData = (NiGeometryBufferData*)
                m_spBatchedShader->PrepareGeometryForRendering(
                pkGeometry, pkPartition, pkBuffData, 
                m_pkBatchedPropertyState);

            // Set the shader programs
            // This is to give the shader final 'override' authority
            uiRet = m_spBatchedShader->SetupShaderPrograms(pkGeometry, 
                pkSkinInstance, pkPartition, pkBuffData, 
                m_pkBatchedPropertyState, m_pkBatchedEffectState, *pkWorld, 
                *pkWorldBound);

            m_pkRenderState->CommitShaderConstants();

            if (pkBuffData->GetIB())
            {
                unsigned int uiStartIndex = 0;
                for (unsigned int i = 0; i < pkBuffData->GetNumArrays(); i++)
                {
                    unsigned int uiPrimitiveCount;
                    const unsigned short* pusArrayLengths = 
                        pkBuffData->GetArrayLengths();
                    if (pusArrayLengths)
                    {
                        // Assumes multiple arrays mean strips
                        uiPrimitiveCount = pusArrayLengths[i] - 2;
                    }
                    else
                    {
                        NIASSERT(pkBuffData->GetNumArrays() == 1);
                        uiPrimitiveCount = pkBuffData->GetTriCount();
                    }

                    m_pkD3DDevice9->DrawIndexedPrimitive(pkBuffData->GetType(),
                        pkBuffData->GetBaseVertexIndex(), 0, 
                        pkBuffData->GetVertCount(), uiStartIndex, 
                        uiPrimitiveCount);

                    uiStartIndex += uiPrimitiveCount + 2;

                    uiMetricDPCalls++;
                    NIMETRICS_DX9RENDERER_AGGREGATEVALUE(DRAW_TRIS, 
                        uiPrimitiveCount);
                    NIMETRICS_DX9RENDERER_AGGREGATEVALUE(DRAW_VERTS, 
                        pkBuffData->GetVertCount());
                }
            }
            else
            {
                m_pkD3DDevice9->DrawPrimitive(pkBuffData->GetType(), 
                    pkBuffData->GetBaseVertexIndex(), 
                    pkBuffData->GetTriCount());

                uiMetricDPCalls++;
                NIMETRICS_DX9RENDERER_AGGREGATEVALUE(DRAW_TRIS, 
                    pkBuffData->GetTriCount());
                NIMETRICS_DX9RENDERER_AGGREGATEVALUE(DRAW_VERTS, 
                    pkBuffData->GetTriCount() * 3);
            }

            // Perform any post-rendering steps
            uiRet = m_spBatchedShader->PostRender(pkGeometry, pkSkinInstance, 
                pkPartition, pkBuffData, m_pkBatchedPropertyState, 
                m_pkBatchedEffectState, *pkWorld, *pkWorldBound);

            pkCurrObject = pkCurrObject->m_pkNext;
        }

        // Inform the shader to move to the next pass
        uiRemainingPasses = m_spBatchedShader->NextPass();
    }

    // Make sure batched data objects are current
    pkCurrObject = m_pkBatchHead;
    while (pkCurrObject)
    {
        pkCurrObject->m_pkGeometry->GetModelData()->ClearRevisionID();
        pkCurrObject = pkCurrObject->m_pkNext;
    }

    // PostProcess the pipeline
    m_spBatchedShader->PostProcessPipeline(pkGeometry, pkSkinInstance, 
        pkBuffData, m_pkBatchedPropertyState, m_pkBatchedEffectState, 
        *pkWorld, *pkWorldBound);

    m_spBatchedShader = 0;

    m_pkBatchedPropertyState = 0;
    m_pkBatchedEffectState = 0;

    EmptyBatch();

    // NOTE: This could lead to an issue where is a single object in the
    // batch requires this flag, all objects rendered after it will also
    // normalize the normals!
    // Reset the normalize normals flag
    m_pkRenderState->SetInternalNormalizeNormals(false);

    NIMETRICS_DX9RENDERER_AGGREGATEVALUE(DRAW_PRIMITIVE, uiMetricDPCalls);
}
//---------------------------------------------------------------------------
void NiDX9Renderer::Do_BatchRenderShape(NiTriShape* pkTriShape)
{
    AddToBatch(pkTriShape);
}
//---------------------------------------------------------------------------
void NiDX9Renderer::Do_BatchRenderStrips(NiTriStrips* pkTriStrips)
{
    AddToBatch(pkTriStrips);
}
//---------------------------------------------------------------------------
void NiDX9Renderer::AddToBatch(NiGeometry* pkGeometry)
{
    NIASSERT(pkGeometry);

    NiGeometryData* pkData = pkGeometry->GetModelData();
    NiSkinInstance* pkSkinInstance = pkGeometry->GetSkinInstance();
    const NiTransform& kWorld = pkGeometry->GetWorldTransform();
    const NiBound& kWorldBound = pkGeometry->GetWorldBound();

    if (m_spBatchedShader == NULL)
    {
        // Save original property/effect state
        NiPropertyStatePtr spCurrentPropertyState = 
            pkGeometry->GetPropertyState();
        NiDynamicEffectStatePtr spCurrentEffectState = 
            pkGeometry->GetEffectState();

        // Set batch's property/effect state
        pkGeometry->SetPropertyState(m_pkBatchedPropertyState);
        pkGeometry->SetEffectState(m_pkBatchedEffectState);

        m_spBatchedShader = NiDynamicCast(NiD3DShaderInterface, 
            pkGeometry->GetShaderFromMaterial());

        if (m_spBatchedShader == NULL)
        {
            const NiMaterial* pkTempMat = pkGeometry->GetActiveMaterial();
            pkGeometry->ApplyAndSetActiveMaterial(m_spCurrentDefaultMaterial);
            m_spBatchedShader = NiDynamicCast(NiD3DShaderInterface, 
                pkGeometry->GetShaderFromMaterial());
            pkGeometry->SetActiveMaterial(pkTempMat);
        }

        // Restore original property/effect state
        pkGeometry->SetPropertyState(spCurrentPropertyState);
        pkGeometry->SetEffectState(spCurrentEffectState);

        // If no shader is found, do not draw
        if (m_spBatchedShader == NULL)
        {
            Error("AddToBatch> No shader found for object %s!\n", 
                pkGeometry->GetName());
            return;
        }
    }

    if (pkData->GetActiveVertexCount() == 0)
        return;

    bool bHWSkinned = IsHardwareSkinned(pkGeometry, m_spBatchedShader);

    if (pkData->GetConsistency() == NiGeometryData::VOLATILE ||
        (pkSkinInstance != NULL && !bHWSkinned))
    {
        // Dynamic geometry group
        m_pkGeometryGroupManager->AddObjectToGroup(m_pkDynamicGroup, 
            pkData, pkSkinInstance, bHWSkinned, 0, 0);
    }
    else
    {
        // Default geometry group
        m_pkGeometryGroupManager->AddObjectToGroup(m_pkDefaultGroup, 
            pkData, pkSkinInstance, bHWSkinned, 0, 0);
    }

    if (bHWSkinned)
    {
        // Place partitions in batch
        NiSkinPartition* pkSkinPartition = pkSkinInstance->GetSkinPartition();
        NIASSERT(pkSkinPartition);

        NiSkinPartition::Partition* pkPartitionArray = 
            pkSkinPartition->GetPartitions();
        NIASSERT(pkPartitionArray);
    
        unsigned int uiNumPartitions = pkSkinPartition->GetPartitionCount();
        for (unsigned int i = 0; i < uiNumPartitions; i++)
        {
            BatchedObject* pkBatch = BatchedObject::GetNextAvailable();
            pkBatch->m_pkGeometry = pkGeometry;
            pkBatch->m_pkCurrPartition = &pkPartitionArray[i];
            pkBatch->m_pkBuffData = 
                (NiGeometryBufferData*)pkPartitionArray[i].m_pkBuffData;

            pkBatch->m_pkNext = 0;

            if (m_pkBatchTail)
            {
                NIASSERT(m_pkBatchHead);
                m_pkBatchTail->m_pkNext = pkBatch;
            }
            else
            {
                NIASSERT(!m_pkBatchTail);
                m_pkBatchHead = pkBatch;
            }

            m_pkBatchTail = pkBatch;
        }
    }
    else
    {
        // Place object in batch
        BatchedObject* pkBatch = BatchedObject::GetNextAvailable();
        pkBatch->m_pkGeometry = pkGeometry;
        pkBatch->m_pkCurrPartition = 0;
        pkBatch->m_pkBuffData = (NiGeometryBufferData*)
            pkData->GetRendererData();

        pkBatch->m_pkNext = 0;

        if (m_pkBatchTail)
        {
            NIASSERT(m_pkBatchHead);
            m_pkBatchTail->m_pkNext = pkBatch;
        }
        else
        {
            NIASSERT(!m_pkBatchTail);
            m_pkBatchHead = pkBatch;
        }

        m_pkBatchTail = pkBatch;
    }
}
//---------------------------------------------------------------------------
void NiDX9Renderer::EmptyBatch()
{
    BatchedObject* pkBatch = m_pkBatchHead;
    while (pkBatch)
    {
        BatchedObject* pkNextBatch = pkBatch->m_pkNext;
        pkBatch->Release();
        pkBatch = pkNextBatch;
    }
    m_pkBatchHead = 0;
    m_pkBatchTail = 0;
}
//---------------------------------------------------------------------------
void NiDX9Renderer::DrawPrimitive(NiGeometry* pkGeometry, 
    NiGeometryData* pkData, NiSkinInstance* pkSkinInstance, 
    const NiTransform& kWorld, const NiBound& kWorldBound, 
    NiGeometryBufferData* pkBuffData)
{
    NIASSERT(!m_bDeviceLost);

    NiD3DShaderInterface* pkShader = NULL;
    if (pkGeometry)
    {
        pkShader = NiDynamicCast(NiD3DShaderInterface, 
            pkGeometry->GetShaderFromMaterial());

        if (pkShader == 0)
        {
            const NiMaterial* pkTempMat = pkGeometry->GetActiveMaterial();
            pkGeometry->ApplyAndSetActiveMaterial(m_spCurrentDefaultMaterial);
            pkShader = NiDynamicCast(NiD3DShaderInterface, 
                pkGeometry->GetShaderFromMaterial());
            pkGeometry->SetActiveMaterial(pkTempMat);
        }
    }
    else
    {
        // In the case of NiScreenTexture
        pkShader = m_spLegacyDefaultShader;
    }

    // If no shader is found, use error shader
    if (pkShader == NULL)
    {
        Error("DrawPrimitive> No shader found for object %s!\n"
            "Using Error Shader!\n", pkGeometry->GetName());
        NIASSERT(NiIsKindOf(NiD3DShaderInterface, GetErrorShader()));
        pkShader = (NiD3DShaderInterface*)GetErrorShader();
    }

    NIASSERT(pkShader);

    NIASSERT(m_pkCurrProp);

    unsigned int uiRet;

    // Preprocess the pipeline
    uiRet = pkShader->PreProcessPipeline(pkGeometry, pkSkinInstance, 
        pkBuffData, m_pkCurrProp, m_pkCurrEffects, kWorld, kWorldBound);

    if (uiRet != 0)
    {
        NIASSERT(!"PreProcess failed - skipping render");
        return;
    }

    // Update the pipeline
    uiRet = pkShader->UpdatePipeline(pkGeometry, pkSkinInstance, pkBuffData, 
        m_pkCurrProp, m_pkCurrEffects, kWorld, kWorldBound);

    unsigned int uiRemainingPasses = pkShader->FirstPass();

    unsigned int uiMetricDPCalls = 0;

    //uiPasses
    while (uiRemainingPasses != 0)
    {
        // Setup the rendering pass
        uiRet = pkShader->SetupRenderingPass(pkGeometry, pkSkinInstance, 
            pkBuffData, m_pkCurrProp, m_pkCurrEffects, kWorld, 
            kWorldBound);

        // Set the transformations
        uiRet = pkShader->SetupTransformations(pkGeometry, pkSkinInstance, 
            NULL, pkBuffData, m_pkCurrProp, m_pkCurrEffects, kWorld, 
            kWorldBound);

        // Prepare the geometry for rendering
        pkBuffData = (NiGeometryBufferData*)
            pkShader->PrepareGeometryForRendering(pkGeometry, NULL, 
            pkBuffData, m_pkCurrProp);

        // Set the shader programs
        // This is to give the shader final 'override' authority
        uiRet = pkShader->SetupShaderPrograms(pkGeometry, pkSkinInstance, NULL,
            pkBuffData, m_pkCurrProp, m_pkCurrEffects, kWorld, kWorldBound);

        m_pkRenderState->CommitShaderConstants();

        if (pkBuffData->GetIB())
        {
            unsigned int uiStartIndex = 0;
            for (unsigned int i = 0; i < pkBuffData->GetNumArrays(); i++)
            {
                unsigned int uiPrimitiveCount;
                const unsigned short* pusArrayLengths = 
                    pkBuffData->GetArrayLengths();
                if (pusArrayLengths)
                {
                    // Assumes multiple arrays mean strips
                    uiPrimitiveCount = pusArrayLengths[i] - 2;
                }
                else
                {
                    NIASSERT(pkBuffData->GetNumArrays() == 1);
                    uiPrimitiveCount = pkBuffData->GetTriCount();
                }

                m_pkD3DDevice9->DrawIndexedPrimitive(pkBuffData->GetType(), 
                    pkBuffData->GetBaseVertexIndex(), 0, 
                    pkBuffData->GetVertCount(), uiStartIndex, 
                    uiPrimitiveCount);

                uiStartIndex += uiPrimitiveCount + 2;

                uiMetricDPCalls++;
                NIMETRICS_DX9RENDERER_AGGREGATEVALUE(DRAW_TRIS, 
                    uiPrimitiveCount);
                NIMETRICS_DX9RENDERER_AGGREGATEVALUE(DRAW_VERTS, 
                    pkBuffData->GetVertCount());
            }
        }
        else
        {
            m_pkD3DDevice9->DrawPrimitive(pkBuffData->GetType(), 
                pkBuffData->GetBaseVertexIndex(), pkBuffData->GetTriCount());

            uiMetricDPCalls++;
            NIMETRICS_DX9RENDERER_AGGREGATEVALUE(DRAW_TRIS, 
                pkBuffData->GetTriCount());
            NIMETRICS_DX9RENDERER_AGGREGATEVALUE(DRAW_VERTS, 
                pkBuffData->GetTriCount() * 3);
        }

        // Perform any post-rendering steps
        uiRet = pkShader->PostRender(pkGeometry, pkSkinInstance, NULL, 
            pkBuffData, m_pkCurrProp, m_pkCurrEffects, kWorld, kWorldBound);

        // Inform the shader to move to the next pass
        uiRemainingPasses = pkShader->NextPass();
    }

    // PostProcess the pipeline
    pkShader->PostProcessPipeline(pkGeometry, pkSkinInstance, pkBuffData, 
        m_pkCurrProp, m_pkCurrEffects, kWorld, kWorldBound);

    // Reset the normalize normals flag
    m_pkRenderState->SetInternalNormalizeNormals(false);

    NIMETRICS_DX9RENDERER_AGGREGATEVALUE(DRAW_PRIMITIVE, uiMetricDPCalls);
}
//---------------------------------------------------------------------------
void NiDX9Renderer::DrawSkinnedPrimitive(NiGeometry* pkGeometry, 
    NiGeometryData* pkData, const NiSkinInstance* pkSkinInstance)
{
    NIASSERT(!m_bDeviceLost);

    NIASSERT(pkSkinInstance);

    // Setup the texture coordinate count and normals flag
    // Do this by checking the first partition
    NiSkinPartition* pkSkinPartition = pkSkinInstance->GetSkinPartition();
    NIASSERT(pkSkinPartition);

    NiSkinPartition::Partition* pkPartitionArray = 
        pkSkinPartition->GetPartitions();
    NIASSERT(pkPartitionArray);

    D3DPRIMITIVETYPE eType = (pkPartitionArray[0].m_usStrips == 0) ? 
        D3DPT_TRIANGLELIST : D3DPT_TRIANGLESTRIP;

    NiD3DShaderInterface* pkShader = NULL;
    if (pkGeometry)
    {
        pkShader = NiDynamicCast(NiD3DShaderInterface, 
            pkGeometry->GetShaderFromMaterial());

        if (pkShader == 0)
        {
            const NiMaterial* pkTempMat = pkGeometry->GetActiveMaterial();
            pkGeometry->ApplyAndSetActiveMaterial(m_spCurrentDefaultMaterial);
            pkShader = NiDynamicCast(NiD3DShaderInterface, 
                pkGeometry->GetShaderFromMaterial());
            pkGeometry->SetActiveMaterial(pkTempMat);
        }
    }

    // If no shader is found, use error shader
    if (pkShader == NULL)
    {
        Error("DrawSkinnedPrimitive> No shader found for object %s!\n" 
            "Using Error Shader!\n", pkGeometry->GetName());
        NIASSERT(NiIsKindOf(NiD3DShaderInterface, GetErrorShader()));
        pkShader = (NiD3DShaderInterface*)GetErrorShader();
    }

    NIASSERT(pkShader);

    NiGeometryBufferData* pkBuffData = 
        (NiGeometryBufferData*)pkPartitionArray->m_pkBuffData;
    NIASSERT(pkBuffData);

    const NiTransform& kWorld = pkGeometry->GetWorldTransform();
    const NiBound& kWorldBound = pkGeometry->GetWorldBound();

    // Preprocess the pipeline
    unsigned int uiRet = pkShader->PreProcessPipeline(pkGeometry,
        pkSkinInstance, pkBuffData, m_pkCurrProp, m_pkCurrEffects, kWorld,
        kWorldBound);

    if (uiRet != 0)
    {
        NIASSERT(!"PreProcess failed - skipping render");
        return;
    }

    uiRet = pkShader->UpdatePipeline(pkGeometry, pkSkinInstance, pkBuffData, 
        m_pkCurrProp, m_pkCurrEffects, kWorld, kWorldBound);

    unsigned int uiPartitionCount = pkSkinPartition->GetPartitionCount();

    unsigned int uiRemainingPasses = pkShader->FirstPass();

    unsigned int uiMetricDPCalls = 0;

    while (uiRemainingPasses != 0)
    {
        uiRet = pkShader->SetupRenderingPass(pkGeometry, pkSkinInstance, 
            pkBuffData, m_pkCurrProp, m_pkCurrEffects, kWorld, kWorldBound);

        for (unsigned int uiPartition = 0; uiPartition < uiPartitionCount; 
            uiPartition++)
        {
            NiSkinPartition::Partition* pkCurrPartition = 
                &pkPartitionArray[uiPartition];

            // This should be a more generic check that compares the
            // number of bones the shader supports with the number of bones
            // in the partition
            if (pkShader == m_spLegacyDefaultShader &&
                pkCurrPartition->m_usBones > m_uiHWBones)
            {
                // Can't render a partition with too many bones without
                // a shader
                Warning("DrawSkinnedPrimitive> Cannot render "
                    "partition with %d bones without a shader!\n", 
                    pkCurrPartition->m_usBones);

                continue;
            }

            pkBuffData = (NiGeometryBufferData*)pkCurrPartition->m_pkBuffData;

            uiRet = pkShader->SetupTransformations(pkGeometry, pkSkinInstance, 
                pkCurrPartition, pkBuffData, m_pkCurrProp, m_pkCurrEffects, 
                kWorld, kWorldBound);

            pkBuffData = (NiGeometryBufferData*)
                pkShader->PrepareGeometryForRendering(pkGeometry, 
                pkCurrPartition, pkBuffData, m_pkCurrProp);
            if (!pkBuffData)
            {
                NIASSERT(0);
                return;
            }

            uiRet = pkShader->SetupShaderPrograms(pkGeometry, pkSkinInstance, 
                pkCurrPartition, pkBuffData, m_pkCurrProp, m_pkCurrEffects, 
                kWorld, kWorldBound);

            m_pkRenderState->CommitShaderConstants();

            // Render
            unsigned int uiStartIndex = 0;
            for (unsigned int i = 0; i < pkBuffData->GetNumArrays(); i++)
            {
                unsigned int uiPrimitiveCount;
                if (pkBuffData->GetArrayLengths())
                {
                    // Assumes multiple arrays mean strips
                    uiPrimitiveCount = pkBuffData->GetArrayLengths()[i] - 2;
                }
                else
                {
                    NIASSERT(pkBuffData->GetNumArrays() == 1);
                    uiPrimitiveCount = pkBuffData->GetTriCount();
                }

                m_pkD3DDevice9->DrawIndexedPrimitive(eType, 
                    pkBuffData->GetBaseVertexIndex(), 0, 
                    pkCurrPartition->m_usVertices, uiStartIndex, 
                    uiPrimitiveCount);

                uiStartIndex += uiPrimitiveCount + 2;

                uiMetricDPCalls++;
                NIMETRICS_DX9RENDERER_AGGREGATEVALUE(DRAW_TRIS, 
                    uiPrimitiveCount);
                NIMETRICS_DX9RENDERER_AGGREGATEVALUE(DRAW_VERTS, 
                    pkCurrPartition->m_usVertices);
            }

            uiRet = pkShader->PostRender(pkGeometry, pkSkinInstance, 
                pkCurrPartition, pkBuffData, m_pkCurrProp, m_pkCurrEffects, 
                kWorld, kWorldBound);
        }

        pkData->ClearRevisionID();

        uiRemainingPasses = pkShader->NextPass();
    }
    
    pkShader->PostProcessPipeline(pkGeometry, pkSkinInstance, pkBuffData, 
        m_pkCurrProp, m_pkCurrEffects, kWorld, kWorldBound);

    // Reset the normalize normals flag
    m_pkRenderState->SetInternalNormalizeNormals(false);

    NIMETRICS_DX9RENDERER_AGGREGATEVALUE(DRAW_PRIMITIVE, uiMetricDPCalls);
}
//---------------------------------------------------------------------------
void NiDX9Renderer::PurgeEffect(NiDynamicEffect* pkEffect)
{
    NiLight* pkLight = NiDynamicCast(NiLight, pkEffect);

    if (pkLight)
        m_pkLightManager->RemoveLight(*pkLight);
}
//---------------------------------------------------------------------------
void NiDX9Renderer::PurgeGeometryData(NiGeometryData* pkData)
{
    if (pkData)
    {
        NiGeometryBufferData* pkBuffData = (NiGeometryBufferData*)
            pkData->GetRendererData();

        if (!pkBuffData)
            return;

        // Remove any PrePackObjects using this data
        if (pkBuffData->GetVBChip(0))
        {
            LockPrecacheCriticalSection();
            for (unsigned int i = 0; i < pkBuffData->GetStreamCount(); i++)
            {
                NiVBBlock* pkBlock = pkBuffData->GetVBChip(i)->GetBlock();
                PrePackObject* pkPrePack = NULL;
                if (m_kPrePackObjects.GetAt(pkBlock, pkPrePack))
                {
                    PrePackObject* pkPreviousPrePack = NULL;
                    while (pkPrePack != NULL)
                    {
                        if (pkPrePack->m_pkData == pkData)
                        {
                            PrePackObject* pkDelete = pkPrePack;
                            PrePackObject* pkNext = NULL;
                            if (pkPreviousPrePack)
                            {
                                pkPreviousPrePack->m_pkNext = 
                                    pkPrePack->m_pkNext;
                                pkNext = pkPrePack->m_pkNext;
                            }
                            else if (pkPrePack->m_pkNext)
                            {
                                m_kPrePackObjects.SetAt(pkBlock, 
                                    pkPrePack->m_pkNext);
                                pkNext = pkPrePack->m_pkNext;
                            }
                            else
                            {
                                m_kPrePackObjects.RemoveAt(pkBlock);
                            }

                            pkDelete->m_pkNext = NULL;
                            NiDelete pkDelete;

                            pkPrePack = pkNext;
                        }
                        else
                        {
                            pkPreviousPrePack = pkPrePack;
                            pkPrePack = pkPrePack->m_pkNext;
                        }
                    }
                }
            }
            UnlockPrecacheCriticalSection();
        }

        m_pkGeometryGroupManager->RemoveObjectFromGroup(pkData);
    }
}
//---------------------------------------------------------------------------
void NiDX9Renderer::PurgeMaterial(NiMaterialProperty* pkMaterial)
{
    m_pkRenderState->PurgeMaterial(pkMaterial);
}
//---------------------------------------------------------------------------
void NiDX9Renderer::PurgeScreenTextureRendererData(
    NiScreenTexture* pkScreenTexture)
{
    if (pkScreenTexture)
    {
        NiGeometryBufferData* pkBuffData = 
            (NiGeometryBufferData*)pkScreenTexture->GetRendererData();

        if (!pkBuffData)
            return;

        m_pkGeometryGroupManager->RemoveObjectFromGroup(pkScreenTexture);
    }
}
//---------------------------------------------------------------------------
void NiDX9Renderer::PurgeSkinPartitionRendererData(
    NiSkinPartition* pkSkinPartition)
{
    if (pkSkinPartition)
    {
        NiSkinPartition::Partition* pkPartitions = 
            pkSkinPartition->GetPartitions();

        if (pkPartitions == NULL)
            return;

        NiGeometryBufferData* pkBuffData = 
            (NiGeometryBufferData*)pkPartitions->m_pkBuffData;

        if (!pkBuffData)
            return;

        unsigned int uiPartitionCount = pkSkinPartition->GetPartitionCount();

        // Remove any PrePackObjects using this data
        for (unsigned int ui = 0; ui < uiPartitionCount; ui++)
        {
            pkBuffData = (NiGeometryBufferData*)pkPartitions[ui].m_pkBuffData;
            if (pkBuffData->GetVBChip(0))
            {
                LockPrecacheCriticalSection();
                for (unsigned int i = 0; i < pkBuffData->GetStreamCount(); i++)
                {
                    NiVBBlock* pkBlock = pkBuffData->GetVBChip(i)->GetBlock();
                    PrePackObject* pkPrePack = NULL;
                    if (m_kPrePackObjects.GetAt(pkBlock, pkPrePack))
                    {
                        PrePackObject* pkPreviousPrePack = NULL;
                        while (pkPrePack != NULL)
                        {
                            if (pkPrePack->m_pkPartition == &pkPartitions[ui])
                            {
                                PrePackObject* pkDelete = pkPrePack;
                                PrePackObject* pkNext = NULL;
                                if (pkPreviousPrePack)
                                {
                                    pkPreviousPrePack->m_pkNext = 
                                        pkPrePack->m_pkNext;
                                    pkNext = pkPrePack->m_pkNext;
                                }
                                else if (pkPrePack->m_pkNext)
                                {
                                    m_kPrePackObjects.SetAt(pkBlock, 
                                        pkPrePack->m_pkNext);
                                    pkNext = pkPrePack->m_pkNext;
                                }
                                else
                                {
                                    m_kPrePackObjects.RemoveAt(pkBlock);
                                }

                                pkDelete->m_pkNext = NULL;
                                NiDelete pkDelete;

                                pkPrePack = pkNext;
                            }
                            else
                            {
                                pkPreviousPrePack = pkPrePack;
                                pkPrePack = pkPrePack->m_pkNext;
                            }
                        }
                    }
                }
                UnlockPrecacheCriticalSection();
            }
        }

        m_pkGeometryGroupManager->RemoveSkinPartitionFromGroup(
            pkSkinPartition);
    }
}
//---------------------------------------------------------------------------
void NiDX9Renderer::PurgeSkinInstance(NiSkinInstance* pkSkinInstance)
{
    D3DMATRIX* pkMatrices = (D3DMATRIX*)pkSkinInstance->GetBoneMatrices();
    NiAlignedFree(pkMatrices);

    pkSkinInstance->SetBoneMatrices(NULL, 0);

    D3DMATRIX* pkS2WW2S = 
        (D3DMATRIX*)pkSkinInstance->GetSkinToWorldWorldToSkinMatrix();
    NiExternalDelete pkS2WW2S;
    pkSkinInstance->SetSkinToWorldWorldToSkinMatrix(NULL);
}
//---------------------------------------------------------------------------
void NiDX9Renderer::PurgeVertexShader(NiD3DVertexShader* pkShader)
{
    m_pkRenderState->SetVertexShader(NULL);
    LPDIRECT3DVERTEXSHADER9 pkShaderHandle = pkShader->GetShaderHandle();
    if (pkShaderHandle)
        pkShaderHandle->Release();
    pkShader->SetShaderHandle(NULL);
}
//---------------------------------------------------------------------------
void NiDX9Renderer::PurgePixelShader(NiD3DPixelShader* pkShader)
{
    m_pkRenderState->SetPixelShader(NULL);
    LPDIRECT3DPIXELSHADER9 pkShaderHandle = pkShader->GetShaderHandle();
    if (pkShaderHandle)
        pkShaderHandle->Release();
    pkShader->SetShaderHandle(NULL);
}
//---------------------------------------------------------------------------
void NiDX9Renderer::RegisterD3DShader(NiD3DShaderInterface* pkD3DShader)
{
    m_kD3DShaders.AddHead(pkD3DShader);
}
//---------------------------------------------------------------------------
void NiDX9Renderer::ReleaseD3DShader(NiD3DShaderInterface* pkD3DShader)
{
    pkD3DShader->SetD3DRenderer(0);

    m_kD3DShaders.Remove(pkD3DShader);
}
//---------------------------------------------------------------------------
void NiDX9Renderer::PurgeD3DShader(NiD3DShaderInterface* pkD3DShader)
{
    pkD3DShader->DestroyRendererData();
    pkD3DShader->SetD3DRenderer(0);

    m_kD3DShaders.Remove(pkD3DShader);
}
//---------------------------------------------------------------------------
void NiDX9Renderer::PurgeAllD3DShaders()
{
    while (!m_kD3DShaders.IsEmpty())
        PurgeD3DShader(m_kD3DShaders.GetHead());
}
//---------------------------------------------------------------------------
void NiDX9Renderer::PrepareForStreaming(NiObjectNET* pkRoot)
{
    NiShaderTimeController::RemoveBeforeStreaming(pkRoot);
}
//---------------------------------------------------------------------------
void NiDX9Renderer::RestoreAfterStreaming()
{
    NiShaderTimeController::AddAfterStreaming();
}
//---------------------------------------------------------------------------
bool NiDX9Renderer::LostDeviceRestore()
{
    HRESULT eD3dRet = m_pkD3DDevice9->TestCooperativeLevel();
    if (eD3dRet == D3DERR_DEVICENOTRESET)
    {
        bool bSuccess = Recreate();
        return bSuccess;
    }
    else if (eD3dRet == D3DERR_DEVICELOST)
    {
        if (m_bDeviceLost == false)
        {
            m_bDeviceLost = true;

            unsigned int uiFuncCount = m_kLostDeviceNotifyFuncs.GetSize();
            for (unsigned int i = 0; i < uiFuncCount; i++)
            {
                LOSTDEVICENOTIFYFUNC pfnFunc = 
                    m_kLostDeviceNotifyFuncs.GetAt(i);
                void* pvData = m_kLostDeviceNotifyFuncData.GetAt(i);
                if (pfnFunc)
                {
                    bool bResult = (*pfnFunc)(pvData);

                    if (bResult == false)
                    {
                        Error("NiDX9Renderer::LostDeviceRestore> "
                            "Lost device notification function failed\n");
                        return false;
                    }
                }
            }
        }
        return false;
    }
    else
    {
        NIASSERT(SUCCEEDED(eD3dRet));
        return true;
    }
}
//---------------------------------------------------------------------------
bool NiDX9Renderer::PackGeometryBuffer(NiGeometryBufferData* pkBuffData,
    NiGeometryData* pkData, NiSkinInstance* pkSkinInstance,
    NiD3DShaderDeclaration* pkShaderDecl, 
    bool bForce)
{
    // pkSkinInstance should be 0 unless we can't do HW skinning
    NIASSERT(pkSkinInstance == 0 || 
        pkSkinInstance->GetSkinPartition() == 0 ||
        (GetFlags() & CAPS_HARDWARESKINNING) == 0 ||
        m_bSWVertexSwitchable); 

    if (!pkBuffData || !pkData)
        return false;

    bool bVBChipsValid = pkBuffData->IsVBChipValid();

    if (!bForce && bVBChipsValid && 
        pkData->GetConsistency() == NiGeometryData::STATIC)
    {
        return true;
    }

    { // Set buffer flags
        bool bColors = (pkData->GetColors() != NULL);
        bool bNorms = (pkData->GetNormals() != NULL);
        bool bBinormsTangents = (bNorms && 
            pkData->GetNormalBinormalTangentMethod() != 
            NiGeometryData::NBT_METHOD_NONE);
        unsigned int uiT = pkData->GetTextureSets();
        if (uiT > NiD3DPass::ms_uiMaxTextureBlendStages)
            uiT = NiD3DPass::ms_uiMaxTextureBlendStages;
        pkBuffData->SetFlags(CreateVertexFlags(bColors, bNorms, 
            bBinormsTangents, uiT));
    }

    unsigned short usVertexCount = pkData->GetActiveVertexCount();
    unsigned short usMaxVertexCount = pkData->GetVertexCount();
    unsigned short usTriCount;
    unsigned short usMaxTriCount;
    unsigned int uiIndexCount;
    unsigned int uiMaxIndexCount;
    const unsigned short* pusIndexArray = NULL;
    const unsigned short* pusArrayLengths = NULL;
    unsigned short usArrayCount;

    if (NiIsKindOf(NiTriShapeData, pkData))
    {
        NiTriShapeData* pkTSData = (NiTriShapeData*)pkData;

        usTriCount = pkTSData->GetActiveTriangleCount();
        usMaxTriCount = pkTSData->GetTriangleCount();

        pusIndexArray = pkTSData->GetTriList();
        pusArrayLengths = NULL;
        usArrayCount = 1;

        uiIndexCount = 3 * usTriCount;
        uiMaxIndexCount = 3 * usMaxTriCount;
    }
    else if (NiIsKindOf(NiTriStripsData, pkData))
    {
        NiTriStripsData* pkTSData = (NiTriStripsData*)pkData;

        usTriCount = pkTSData->GetActiveTriangleCount();
        usMaxTriCount = pkTSData->GetTriangleCount();

        pusIndexArray = pkTSData->GetStripLists();
        pusArrayLengths = pkTSData->GetStripLengths();
        usArrayCount = pkTSData->GetStripCount();

        uiIndexCount = usTriCount + 2 * usArrayCount;
        uiMaxIndexCount = usMaxTriCount + 2 * usArrayCount;
    }
    else
    {
        NIASSERT(!"NiDX9Renderer::PackGeometryBuffer> Unknown geom type!");
    }

    unsigned int uiNumStreams = 
        (pkShaderDecl ? pkShaderDecl->GetStreamCount() : 1);

    // Handle the software skinning case
    unsigned short usDirtyFlags = pkData->GetRevisionID();
    if (pkSkinInstance != NULL &&
        (pkSkinInstance->GetFrameID() != m_pkVBManager->GetFrameID()))
    {
        // This is the first time this software-skinned object has been 
        // rendered this frame, so ensure vertices and normals get packed. 
        // This way, if the object is rendered in the first pass as a 
        // shadow map, we won't have to redeform for the second rendering 
        // pass.
        usDirtyFlags |= 
            (NiGeometryData::VERTEX_MASK | NiGeometryData::NORMAL_MASK);

        pkSkinInstance->SetFrameID(m_pkVBManager->GetFrameID());
    }

    if (pkData->GetConsistency() == NiGeometryData::VOLATILE || bForce)
        usDirtyFlags = NiGeometryData::DIRTY_MASK;

    if (bVBChipsValid)
    {
        // Packed buffer already exists - check stream count
        if (uiNumStreams != pkBuffData->GetStreamCount())
        {
            // Stream count is different. Mark it for complete repacking
            pkBuffData->SetStreamCount(uiNumStreams);
            usDirtyFlags = NiGeometryData::DIRTY_MASK;
        }
    }
    else
    {
        // Packed buffer does not yet exist
        NiGeometryGroup* pkGeomGroup = pkBuffData->GetGeometryGroup();
        for (unsigned int i = 0; i < uiNumStreams; i++)
            pkGeomGroup->ReleaseChip(pkBuffData, i);

        pkBuffData->SetStreamCount(uiNumStreams);
        pkBuffData->SetBaseVertexIndex(0);
        usDirtyFlags = NiGeometryData::DIRTY_MASK;
    }

    pkBuffData->SetVertCount(usVertexCount, usMaxVertexCount);
    pkBuffData->SetIndices(usTriCount, usMaxTriCount, pusIndexArray, 
        pusArrayLengths, usArrayCount);

    // Vertex buffers
    // Always repack if the TRIANGLE_COUNT_MASK is set, even if fewer 
    // triangles are used.
    // This means that the VB will be repacked unless TRIANGLE_INDEX_MASK
    // is set.
    if ((usDirtyFlags & ~NiTriBasedGeomData::TRIANGLE_INDEX_MASK) != 0)
    {
        for (unsigned int i = 0; i < uiNumStreams; i++)
        {
            NiVBChip* pkExistingVBChip = pkBuffData->GetVBChip(i);
            NiVBChip* pkSrcVBChip = NULL;

            if (pkShaderDecl)
            {
                pkSrcVBChip = pkShaderDecl->PackUntransformedVB(pkData, 
                    pkSkinInstance, 0, usDirtyFlags, 
                    pkExistingVBChip, i);
            }

            // Pack via the standard method on failure (or not shader decl)
            if (pkSrcVBChip == NULL && uiNumStreams == 1)
            {
                pkSrcVBChip = m_pkVBManager->PackUntransformedVB(pkData,
                    pkSkinInstance, usDirtyFlags, pkExistingVBChip);
            }

            if (pkSrcVBChip == NULL)
            {
                // Packing has failed for some reason
                return false;
            }
        }
    }

    // Index Buffers
    // Always repack if TRIANGLE_COUNT_MASK is set, even if fewer
    // triangles are used.
    if (pusIndexArray != NULL && 
        (usDirtyFlags & (NiTriBasedGeomData::TRIANGLE_INDEX_MASK | 
        NiTriBasedGeomData::TRIANGLE_COUNT_MASK)) != 0)
    {
        LPDIRECT3DINDEXBUFFER9 pkExistingIB = pkBuffData->GetIB();
        unsigned int uiIBSize = pkBuffData->GetIBSize();

        LPDIRECT3DINDEXBUFFER9 pkSrcIB = m_pkIBManager->PackBuffer(
            pusIndexArray, uiIndexCount, uiMaxIndexCount, pkExistingIB, 
            uiIBSize, D3DPOOL_MANAGED, 
            (pkBuffData->GetSoftwareVP() ? D3DUSAGE_SOFTWAREPROCESSING : 0));

        if (pkSrcIB == NULL)
        {
            // Packing IB failed for some reason
            return false;
        }

        if (pkExistingIB != pkSrcIB)
        {
            pkBuffData->RemoveIB();
            pkBuffData->SetIB(pkSrcIB, uiIndexCount, uiIBSize);
        }
    }
    
    pkData->ClearRevisionID();

    return true;
}
//---------------------------------------------------------------------------
bool NiDX9Renderer::PackSkinnedGeometryBuffer(
    NiGeometryBufferData* pkBuffData, NiGeometryData* pkData,
    const NiSkinInstance* pkSkin, NiSkinPartition::Partition* pkPartition, 
    NiD3DShaderDeclaration* pkShaderDecl, bool bForce)
{
    NIASSERT(pkSkin && pkPartition);

    if (!pkBuffData || !pkData)
        return false;

    bool bVBChipsValid = pkBuffData->IsVBChipValid();

    if (!bForce && bVBChipsValid && 
        pkData->GetConsistency() == NiGeometryData::STATIC)
    {
        return true;
    }

    { // Set buffer flags
        bool bColors = (pkData->GetColors() != NULL);
        bool bNorms = (pkData->GetNormals() != NULL);
        bool bBinormsTangents = (bNorms && 
            pkData->GetNormalBinormalTangentMethod() != 
            NiGeometryData::NBT_METHOD_NONE);
        unsigned int uiT = pkData->GetTextureSets();
        if (uiT > NiD3DPass::ms_uiMaxTextureBlendStages)
            uiT = NiD3DPass::ms_uiMaxTextureBlendStages;
        pkBuffData->SetFlags(CreateVertexFlags(bColors, bNorms, 
            bBinormsTangents, uiT));
    }
    
    unsigned short usVertexCount = pkPartition->m_usVertices;
    unsigned short usTriCount = pkPartition->m_usTriangles;
    unsigned int uiIndexCount;
    const unsigned short* pusIndexArray = pkPartition->m_pusTriList;
    const unsigned short* pusArrayLengths = pkPartition->m_pusStripLengths;
    unsigned short usArrayCount = pkPartition->m_usStrips;
    NIASSERT(pusIndexArray);

    if (usArrayCount == 0)
    {
        // TriShape instance...
        uiIndexCount = 3 * usTriCount;
        usArrayCount = 1;
        NIASSERT(pusArrayLengths == NULL);
    }
    else // (usArrayCount != 0)
    {
        uiIndexCount = usTriCount + 2 * usArrayCount;
        NIASSERT(pusArrayLengths);
    }

    unsigned int uiNumStreams = 
        (pkShaderDecl ? pkShaderDecl->GetStreamCount() : 1);

    unsigned short usDirtyFlags = pkData->GetRevisionID();
    NiGeometryData::Consistency eConst = pkData->GetConsistency();
    if (eConst == NiGeometryData::VOLATILE || bForce)
        usDirtyFlags = NiGeometryData::DIRTY_MASK;

    if (bVBChipsValid)
    {
        // Packed buffer already exists - check stream count
        if (uiNumStreams != pkBuffData->GetStreamCount())
        {
            pkBuffData->SetStreamCount(uiNumStreams);
            usDirtyFlags = NiGeometryData::DIRTY_MASK;
        }
    }
    else
    {
        // Packed buffer does not yet exist
        NiGeometryGroup* pkGeomGroup = pkBuffData->GetGeometryGroup();
        for (unsigned int i = 0; i < uiNumStreams; i++)
            pkGeomGroup->ReleaseChip(pkBuffData, i);

        pkBuffData->SetStreamCount(uiNumStreams);
        pkBuffData->SetBaseVertexIndex(0);
        usDirtyFlags = NiGeometryData::DIRTY_MASK;
    }

    pkBuffData->SetVertCount(usVertexCount, usVertexCount);
    pkBuffData->SetIndices(usTriCount, usTriCount, pusIndexArray, 
        pusArrayLengths, usArrayCount);

    // Vertex buffers
    // Always repack if TRIANGLE_COUNT_MASK is set, even if fewer
    // triangles are used.
    // This means that the VB will be repacked uless TRIANGLE_INDEX_COUNT
    // is set.
    if ((usDirtyFlags & ~NiTriBasedGeomData::TRIANGLE_INDEX_MASK) != 0)
    {
        for (unsigned int i = 0; i < uiNumStreams; i++)
        {
            NiVBChip* pkExistingVBChip = pkBuffData->GetVBChip(i);
            NiVBChip* pkSrcVBChip = NULL;

            if (pkShaderDecl)
            {
                pkSrcVBChip = pkShaderDecl->PackUntransformedVB(pkData, 
                    (NiSkinInstance*)pkSkin, pkPartition,  
                    usDirtyFlags, pkExistingVBChip, i);
            }

            // Pack via the standard method on failure, or no shader decl
            if (pkSrcVBChip == NULL && uiNumStreams == 1)
            {
                pkSrcVBChip = m_pkVBManager->PackSkinnedVB(pkData, pkSkin, 
                    pkPartition, usDirtyFlags, pkExistingVBChip);
            }

            if (pkSrcVBChip == NULL)
            {
                // Packing failed for some reason...
                return false;
            }
        }
    }

    // Index Buffers
    // Always repack if TRIANGLE_COUNT_MASK is set, even if fewer
    // triangles are used.
    if ((usDirtyFlags & (NiTriBasedGeomData::TRIANGLE_INDEX_MASK | 
        NiTriBasedGeomData::TRIANGLE_COUNT_MASK)) != 0)
    {
        LPDIRECT3DINDEXBUFFER9 pkExistingIB = pkBuffData->GetIB();
        unsigned int uiIBSize = pkBuffData->GetIBSize();

        LPDIRECT3DINDEXBUFFER9 pkSrcIB = m_pkIBManager->PackBuffer(
            pusIndexArray, uiIndexCount, uiIndexCount, pkExistingIB, 
            uiIBSize, D3DPOOL_MANAGED, 
            (pkBuffData->GetSoftwareVP() ? D3DUSAGE_SOFTWAREPROCESSING : 0));

        if (pkSrcIB == NULL)
        {
            // Packing IB failed for some reason
            return false;
        }

        if (pkExistingIB != pkSrcIB)
        {
            pkBuffData->RemoveIB();
            pkBuffData->SetIB(pkSrcIB, uiIndexCount, uiIBSize);
        }
    }

    // Don't clear revision ID, since other partitions will need to be packed
    // as well, using the same geometry data object

    return true;
}
//---------------------------------------------------------------------------
void NiDX9Renderer::PackScreenTextureGeomBuffer(
    NiScreenTexture* pkScreenTexture)
{
    if (pkScreenTexture == NULL)
        return;

    NiGeometryBufferData* pkBuffData = 
        (NiGeometryBufferData*)pkScreenTexture->GetRendererData();
    NIASSERT(pkBuffData);

    bool bVBChipsValid = pkBuffData->IsVBChipValid();

    { // Set buffer flags
        bool bColors = true;
        bool bNorms = false;
        bool bBinormsTangents = false;
        unsigned int uiT = 1;
        pkBuffData->SetFlags(CreateVertexFlags(bColors, bNorms, 
            bBinormsTangents, uiT));
    }

    pkBuffData->SetStreamCount(1);

    unsigned short usDirtyFlags = pkScreenTexture->GetRevisionID();

    NiGeometryGroup* pkGeomGroup = pkBuffData->GetGeometryGroup();
    if (!bVBChipsValid)
    {
        pkGeomGroup->ReleaseChip(pkBuffData, 0);
        pkBuffData->SetBaseVertexIndex(0);
        usDirtyFlags = NiScreenTexture::EVERYTHING_MASK;
    }
    else if (pkGeomGroup->IsDynamic())
    {
        usDirtyFlags = NiScreenTexture::EVERYTHING_MASK;
    }

    // Vertex buffers and index buffers
    if (usDirtyFlags != 0)
    {
        PackScreenTextureBuffers(pkScreenTexture, usDirtyFlags);
    }

    pkScreenTexture->ClearRevisionID();
}
//---------------------------------------------------------------------------
void NiDX9Renderer::PackScreenTextureBuffers(
    NiScreenTexture* pkScreenTexture, unsigned short usDirtyFlags)
{
    NiGeometryBufferData* pkBuffData = 
        (NiGeometryBufferData*)pkScreenTexture->GetRendererData();
    NIASSERT(pkBuffData);

    const unsigned short usNumScreenRects = 
        pkScreenTexture->GetNumScreenRects();
    const unsigned short usVertexCount = 4 * usNumScreenRects;
    const unsigned int uiIndexCount = usVertexCount * 3 / 2;

    bool bNumRectsDirty = 
        (usDirtyFlags & NiScreenTexture::RECTANGLE_COUNT_MASK) != 0;

    pkBuffData->SetVertCount(usVertexCount, usVertexCount);
    pkBuffData->SetIndices(2 * usNumScreenRects, 2 * usNumScreenRects, 
        NULL, NULL, 1);

    // Vertex buffers
    NiVBChip* pkExistingVBChip = pkBuffData->GetVBChip(0);
    if (bNumRectsDirty)
    {
        if (usVertexCount > m_usNumScreenTextureVerts)
        {
            NiDelete[] m_pkScreenTextureVerts;
            NiDelete[] m_pkScreenTextureColors;
            NiDelete[] m_pkScreenTextureTexCoords;
            m_pkScreenTextureVerts = NiNew NiPoint2[usVertexCount];
            m_pkScreenTextureColors = NiNew NiColorA[usVertexCount];
            m_pkScreenTextureTexCoords = NiNew NiPoint2[usVertexCount];
            m_usNumScreenTextureVerts = usVertexCount;
        }

        if (pkExistingVBChip && (usVertexCount > 
            pkExistingVBChip->GetSize() / pkBuffData->GetVertexStride(0)))
        {
            // Need to repack everything - don't know what's in the new rects
            usDirtyFlags = NiScreenTexture::EVERYTHING_MASK;
        }
    }

    if ((usDirtyFlags | ~NiScreenTexture::RECTANGLE_COUNT_MASK) != 0)
    {
        // This assertion is valid because NiScreenTexture buffers are only
        // getting packed when the NiScreenTexture is getting drawn.
        NIASSERT(m_pkCurrRenderTargetGroup);

        // Fill screen texture arrays
        int iScreenWidth = m_pkCurrRenderTargetGroup->GetWidth(0);
        int iScreenHeight = m_pkCurrRenderTargetGroup->GetHeight(0);

        unsigned short usTexWidth = 
            pkScreenTexture->GetTexture()->GetWidth();
        unsigned short usTexHeight = 
            pkScreenTexture->GetTexture()->GetHeight();

        NIASSERT((usTexWidth > 0) && (usTexHeight > 0));

        float fInverseU = 1.0f / (float)usTexWidth;
        float fInverseV = 1.0f / (float)usTexHeight;

        unsigned short i = 0;
        for (; i < usNumScreenRects; i++)
        {
            const NiScreenTexture::ScreenRect& kScreenRect = 
                pkScreenTexture->GetScreenRect(i);

            // Ensure screen location is on screen
            short sPixLeft = kScreenRect.m_sPixLeft;
            while (sPixLeft < 0)
                sPixLeft += iScreenWidth;
            while (sPixLeft >= iScreenWidth)
                sPixLeft -= iScreenWidth;

            short sPixTop = kScreenRect.m_sPixTop;
            while (sPixTop < 0)
                sPixTop += iScreenHeight;
            while (sPixTop >= iScreenHeight)
                sPixTop -= iScreenHeight;

            float fScreenLeft = (float)sPixLeft - 0.5f;
            float fScreenTop = (float)sPixTop - 0.5f;
            float fScreenRight = fScreenLeft + 
                (float)kScreenRect.m_usPixWidth;
            float fScreenBottom = fScreenTop + 
                (float)kScreenRect.m_usPixHeight;
            float fTexLeft = fInverseU * ((float)kScreenRect.m_usTexLeft);
            float fTexTop = fInverseV * ((float)kScreenRect.m_usTexTop);
            float fTexRight = fTexLeft + 
                fInverseU * (float)kScreenRect.m_usPixWidth;
            float fTexBottom = fTexTop + 
                fInverseV * (float)kScreenRect.m_usPixHeight;

            for (unsigned short j = 0; j < 4; j++)
            {
                unsigned short usIndex = i * 4 + j;
                m_pkScreenTextureVerts[usIndex].x = 
                    (j / 2 == 0 ? fScreenLeft : fScreenRight);
                m_pkScreenTextureVerts[usIndex].y = 
                    (j % 2 == 0 ? fScreenTop : fScreenBottom);
                m_pkScreenTextureColors[usIndex] = kScreenRect.m_kColor;
                m_pkScreenTextureTexCoords[usIndex].x = 
                    (j / 2 == 0 ? fTexLeft : fTexRight);
                m_pkScreenTextureTexCoords[usIndex].y = 
                    (j % 2 == 0 ? fTexTop : fTexBottom);
            }
        }

        //  Grab a screen poly vertex buffer and pack it...

        NiVBChip* pkSrcChip = m_pkVBManager->PackTransformedVB(pkBuffData, 
            usVertexCount, m_pkScreenTextureVerts, m_pkScreenTextureColors, 
            m_pkScreenTextureTexCoords, usDirtyFlags, pkExistingVBChip);

        if (pkSrcChip == NULL)
        {
            // Packing VB failed for some reason
            return;
        }
    }

    // Index buffers
    if (bNumRectsDirty)
    {
        if (uiIndexCount > m_uiNumScreenTextureIndices)
        {
            NiFree(m_pusScreenTextureIndices);
            m_pusScreenTextureIndices = NiAlloc(unsigned short, uiIndexCount);
            m_uiNumScreenTextureIndices = uiIndexCount;

            unsigned short usIndex = 0;
            for (unsigned int i = 0; i < usVertexCount; i += 4)
            {
                m_pusScreenTextureIndices[usIndex++] = i;
                m_pusScreenTextureIndices[usIndex++] = i + 1;
                m_pusScreenTextureIndices[usIndex++] = i + 2;
                m_pusScreenTextureIndices[usIndex++] = i + 2;
                m_pusScreenTextureIndices[usIndex++] = i + 1;
                m_pusScreenTextureIndices[usIndex++] = i + 3;
            }
        }

        LPDIRECT3DINDEXBUFFER9 pkExistingIB = pkBuffData->GetIB();
        unsigned int uiIBSize = pkBuffData->GetIBSize();

        LPDIRECT3DINDEXBUFFER9 pkSrcIB = m_pkIBManager->PackBuffer(
            m_pusScreenTextureIndices, uiIndexCount, uiIndexCount, 
            pkExistingIB, uiIBSize, D3DPOOL_MANAGED, 
            (pkBuffData->GetSoftwareVP() ? D3DUSAGE_SOFTWAREPROCESSING : 0));

        if (pkExistingIB != pkSrcIB)
        {
            pkBuffData->RemoveIB();
            pkBuffData->SetIB(pkSrcIB, uiIndexCount, uiIBSize);
        }
    }
}
//---------------------------------------------------------------------------
bool NiDX9Renderer::PrePackGeometryBuffer(
    NiGeometry* pkGeom, 
    NiGeometryData* pkData, 
    unsigned short usTriCount, 
    unsigned short usMaxTriCount, 
    const unsigned short* pusIndexArray, 
    const unsigned short* pusArrayLengths, 
    unsigned short usArrayCount,
    NiD3DShaderDeclaration* pkShaderDecl)
{
    NiGeometryBufferData* pkBuffData = (NiGeometryBufferData*)
        pkData->GetRendererData();
    NIASSERT(pkBuffData);
    if (pkBuffData->IsVBChipValid())
    {
        // Already packed?
        return true;
    }

    unsigned short usVertexCount = pkData->GetActiveVertexCount();
    unsigned short usMaxVertexCount = pkData->GetVertexCount();

    pkBuffData->SetVertCount(usVertexCount, usMaxVertexCount);
    pkBuffData->SetIndices(usTriCount, usMaxTriCount, pusIndexArray, 
        pusArrayLengths, usArrayCount);

    bool bSuccess = ReservePrepackedBufferSpace(pkBuffData, pkData, NULL, 
        pkShaderDecl);

    if (bSuccess)
    {
        unsigned int uiNumStreams = pkBuffData->GetStreamCount();
        for (unsigned int i = 0; i < uiNumStreams; i++)
        {
            // Each stream gets its own PrePack object
            NiVBBlock* pkBlock = pkBuffData->GetVBChip(i)->GetBlock();

            PrePackObject* pkPrePack = NiNew PrePackObject;
            pkPrePack->m_pkData = pkData;
            pkPrePack->m_pkSkin = NULL;
            pkPrePack->m_pkPartition = NULL;
            pkPrePack->m_pkShaderDecl = pkShaderDecl;
            pkPrePack->m_uiBonesPerPartition = 0;
            pkPrePack->m_uiBonesPerVertex = 0;

            pkPrePack->m_pkBuffData = pkBuffData;
            pkPrePack->m_uiStream = i;

            PrePackObject* pkCurrentPrePack;
            if (m_kPrePackObjects.GetAt(pkBlock, pkCurrentPrePack))
                pkPrePack->m_pkNext = pkCurrentPrePack;
            else
                pkPrePack->m_pkNext = NULL;

            m_kPrePackObjects.SetAt(pkBlock, pkPrePack);
        }
    }

    return bSuccess;
}
//---------------------------------------------------------------------------
bool NiDX9Renderer::PrePackSkinnedGeometryBuffer(
    NiGeometry* pkGeom,
    NiGeometryData* pkData, 
    NiSkinInstance* pkSkin, 
    NiD3DShaderDeclaration* pkShaderDecl, 
    unsigned int uiBonesPerPartition, 
    unsigned int uiBonesPerVertex)
{
    // Check for hardware skinning - should not prepack skinned if software
    NIASSERT(IsHardwareSkinned(pkGeom));

    //  Check partition for packing
    NiSkinPartition* pkSkinPartition = pkSkin->GetSkinPartition();
    NIASSERT(pkSkinPartition);
    NiSkinPartition::Partition* pkPartitionArray = 
        pkSkinPartition->GetPartitions();
    unsigned int uiNumPartitions = pkSkinPartition->GetPartitionCount();

    bool bReturn = true;
    for (unsigned int uiPartition = 0; uiPartition < uiNumPartitions; 
        uiPartition++)
    {
        NiSkinPartition::Partition* pkCurrPartition = 
            &pkPartitionArray[uiPartition];
        NIASSERT(pkCurrPartition);

        NiGeometryBufferData* pkBuffData = 
            (NiGeometryBufferData*)pkCurrPartition->m_pkBuffData;
        if (pkBuffData->IsVBChipValid())
        {
            //  Already packed?
            continue;
        }

        unsigned short usVertexCount = pkCurrPartition->m_usVertices;
        unsigned short usTriCount = pkCurrPartition->m_usTriangles;
        const unsigned short* pusIndexArray = 
            pkCurrPartition->m_pusTriList;
        const unsigned short* pusArrayLengths = 
            pkCurrPartition->m_pusStripLengths;
        unsigned short usArrayCount = pkCurrPartition->m_usStrips;

        pkBuffData->SetVertCount(usVertexCount, usVertexCount);
        pkBuffData->SetIndices(usTriCount, usTriCount, pusIndexArray, 
            pusArrayLengths, (usArrayCount == 0 ? 1 : usArrayCount));

        bool bSuccess = ReservePrepackedBufferSpace(pkBuffData, pkData, 
            pkCurrPartition, pkShaderDecl);

        if (bSuccess)
        {
            unsigned int uiNumStreams = pkBuffData->GetStreamCount();
            for (unsigned int i = 0; i < uiNumStreams; i++)
            {
                // Each stream gets its own PrePack object
                NiVBBlock* pkBlock = pkBuffData->GetVBChip(i)->GetBlock();

                PrePackObject* pkPrePack = NiNew PrePackObject;
                pkPrePack->m_pkData = pkData;
                pkPrePack->m_pkSkin = pkSkin;
                pkPrePack->m_pkPartition = pkCurrPartition;
                pkPrePack->m_pkShaderDecl = pkShaderDecl;
                pkPrePack->m_uiBonesPerPartition = uiBonesPerPartition;
                pkPrePack->m_uiBonesPerVertex = uiBonesPerVertex;
                pkPrePack->m_pkNext = NULL;

                pkPrePack->m_pkBuffData = pkBuffData;
                pkPrePack->m_uiStream = i;

                PrePackObject* pkCurrentPrePack;
                if (m_kPrePackObjects.GetAt(pkBlock, pkCurrentPrePack))
                    pkPrePack->m_pkNext = pkCurrentPrePack;
                else
                    pkPrePack->m_pkNext = NULL;

                m_kPrePackObjects.SetAt(pkBlock, pkPrePack);
            }
        }
        else
        {
            bReturn = false;
        }
    }

    return bReturn;
}
//---------------------------------------------------------------------------
bool NiDX9Renderer::ReservePrepackedBufferSpace(
    NiGeometryBufferData* pkBuffData, 
    NiGeometryData* pkData, 
    NiSkinPartition::Partition* pkPartition, 
    NiD3DShaderDeclaration* pkShaderDecl)
{
    NIASSERT(pkBuffData && pkData);

    unsigned int uiFVF = 0;
    NiD3DVertexDeclaration hDeclaration = NULL;
    unsigned int uiNumStreams = 0;
    unsigned int uiVertexStride = 0;
    unsigned int uiPosOffset, uiWeightOffset, uiNormOffset, uiColorOffset, 
        uiTexOffset;

    bool bValid = false;
    if (pkShaderDecl)
    {
        bValid = pkShaderDecl->GenerateVertexParameters(hDeclaration, 
            uiNumStreams);
    }

    if (!bValid)
    {
        if (pkPartition)
        {
            // Ensure partition doesn't have too many bones
            if (pkPartition->m_usBones > HW_BONE_LIMIT)
                return false;
        }

        uiNumStreams = 1;
        m_pkVBManager->GenerateVertexParameters(pkData, pkPartition, uiFVF, 
            uiVertexStride, uiPosOffset, uiWeightOffset, uiNormOffset, 
            uiColorOffset, uiTexOffset);
    }

    pkBuffData->SetStreamCount(uiNumStreams);
    if (uiFVF)
    {
        pkBuffData->SetFVF(uiFVF);
        pkBuffData->SetVertexStride(0, uiVertexStride);
    }
    else
    {
        pkBuffData->SetVertexDeclaration(hDeclaration);
        for (unsigned int i = 0; i < uiNumStreams; i++)
            pkBuffData->SetVertexStride(i, pkShaderDecl->GetVertexStride(i));
    }

    { // Set buffer flags
        bool bColors = (pkData->GetColors() != NULL);
        bool bNorms = (pkData->GetNormals() != NULL);
        bool bBinormsTangents = (bNorms && 
            pkData->GetNormalBinormalTangentMethod() != 
            NiGeometryData::NBT_METHOD_NONE);
        unsigned int uiT = pkData->GetTextureSets();
        if (uiT > NiD3DPass::ms_uiMaxTextureBlendStages)
            uiT = NiD3DPass::ms_uiMaxTextureBlendStages;
        pkBuffData->SetFlags(CreateVertexFlags(bColors, bNorms, 
            bBinormsTangents, uiT));
    }

    if (pkShaderDecl && m_bSWVertexCapable)
        pkBuffData->SetSoftwareVP(pkShaderDecl->GetSoftwareVertexProcessing());

    for (unsigned int i = 0; i < uiNumStreams; i++)
    {
        if (!m_pkVBManager->AllocateBufferSpace(pkBuffData, i))
        {
            // Clear out existing chips
            while (i > 0)
                pkBuffData->GetGeometryGroup()->ReleaseChip(pkBuffData, i--);

            return false;
        }
    }

    return true;
}
//---------------------------------------------------------------------------
bool NiDX9Renderer::CreateSourceTextureRendererData(
    NiSourceTexture* pkTexture)
{
    LockSourceDataCriticalSection();
    NiDX9TextureData* pkData = 
        (NiDX9TextureData*)(pkTexture->GetRendererData());

    if (!pkData)
        pkData = NiDX9SourceTextureData::Create(pkTexture, this); 
    UnlockSourceDataCriticalSection();

    if (!pkData)
        return false;

    return true;
}
//---------------------------------------------------------------------------
bool NiDX9Renderer::CreateRenderedTextureRendererData(
    NiRenderedTexture* pkTexture, Ni2DBuffer::MultiSamplePreference eMSAAPref)
{
    NiDX9RenderedTextureData* pkData = 
        (NiDX9RenderedTextureData*)pkTexture->GetRendererData();
    if (pkData)
        return true;

    pkData = NiDX9RenderedTextureData::Create(pkTexture, this); 
    if (!pkData)
        return false;

    m_kRenderedTextures.SetAt(pkTexture, pkData);

    return true;
}
//---------------------------------------------------------------------------
bool NiDX9Renderer::CreateSourceCubeMapRendererData(
    NiSourceCubeMap* pkCubeMap)
{
    LockSourceDataCriticalSection();
    NiDX9SourceCubeMapData* pkData 
        = (NiDX9SourceCubeMapData*)pkCubeMap->GetRendererData();

    if (pkData)
        return true;

    pkData = NiDX9SourceCubeMapData::Create(pkCubeMap, this); 
    UnlockSourceDataCriticalSection();
    if (!pkData)
        return false;

    return true;
}
//---------------------------------------------------------------------------
bool NiDX9Renderer::CreateRenderedCubeMapRendererData(
    NiRenderedCubeMap* pkCubeMap)
{
    NiDX9RenderedCubeMapData* pkData = 
        (NiDX9RenderedCubeMapData*)pkCubeMap->GetRendererData();
    if (pkData)
        return true;

    pkData = NiDX9RenderedCubeMapData::Create(pkCubeMap, this); 
    if (!pkData)
        return false;

    m_kRenderedCubeMaps.SetAt(pkCubeMap, pkData);

    return true;
}
//---------------------------------------------------------------------------
bool NiDX9Renderer::CreateDynamicTextureRendererData(
    NiDynamicTexture* pkTexture)
{
    NiDX9DynamicTextureData* pkDynTxtrData = NULL;

    NiDX9TextureData* pkTxtrData =
        (NiDX9TextureData*)(pkTexture->GetRendererData());
    if (pkTxtrData)
    {
        pkDynTxtrData = pkTxtrData->GetAsDynamicTexture();
        if (pkDynTxtrData)
            return true;
    }

    pkDynTxtrData = NiDX9DynamicTextureData::Create(pkTexture);
    if (!pkDynTxtrData)
        return false;

    m_kDynamicTextures.SetAt(pkTexture, pkDynTxtrData);

    return true;
}
//---------------------------------------------------------------------------
void* NiDX9Renderer::LockDynamicTexture(
    const NiTexture::RendererData* pkRData, int& iPitch)
{
    // Discard the entire pixel buffer.  Caller will refill with pixels.
    
    iPitch = 0; // Initialize in case there's an error return.

    NiDX9TextureData* pkTxtrData = (NiDX9TextureData*)(pkRData);
    if (!pkTxtrData)
        return NULL;

    // Call GetAsDynamicTexture() so app won't crash in case this function
    // gets called using a non-dynamic texture, accidentally.
    NiDX9DynamicTextureData* pkDynTxtrData = pkTxtrData->GetAsDynamicTexture();
    if (!pkDynTxtrData)
        return NULL;

    // Sanity check - error return if dynamic texture is already locked.
    if (pkDynTxtrData->IsLocked())
        return NULL;

    D3DTexturePtr pkD3DTexture = NULL;
    DWORD dwLockingFlags = 0;
    if (IsDynamicTexturesCapable())
    {   // Get the video memory texture.
        pkD3DTexture = (D3DTexturePtr)(pkDynTxtrData->GetD3DTexture());
        dwLockingFlags = D3DLOCK_DISCARD;
    }
    else
    {   // Get the system memory texture.
        pkD3DTexture = (D3DTexturePtr)(pkDynTxtrData->GetSysMemTexture());
    }

    if (!pkD3DTexture)
        return NULL;

    // Lock the bits of the texture, whether video or system memory.
    D3DLOCKED_RECT kLockedRect;
    HRESULT eD3dRet = pkD3DTexture->LockRect(0, &kLockedRect, NULL,
        dwLockingFlags);
    if (FAILED(eD3dRet))
    {
        NiDX9Renderer::Warning("NiDX9Renderer::LockDynamicTexture Failed> "
            "LockRect - %s", NiDX9ErrorString((unsigned int)eD3dRet));
        return NULL;
    }

    iPitch = kLockedRect.Pitch;
    void* pvBits = (void*)(kLockedRect.pBits);
    pkDynTxtrData->SetLocked(true);

    return pvBits;
}
//---------------------------------------------------------------------------
bool NiDX9Renderer::UnLockDynamicTexture(
    const NiTexture::RendererData* pkRData)
{
    NiDX9TextureData* pkTxtrData = (NiDX9TextureData*)(pkRData);
    if (!pkTxtrData)
        return false;

    // Call GetAsDynamicTexture() so app won't crash in case this function
    // gets called using a non-dynamic texture, accidentally.
    NiDX9DynamicTextureData* pkDynTxtrData =
        pkTxtrData->GetAsDynamicTexture();
    if (!pkDynTxtrData)
        return false;

    // Sanity check - error return if dynamic texture is already unlocked.
    if (!pkDynTxtrData->IsLocked())
        return false;

    D3DTexturePtr pkD3DTexture = NULL;
    if (IsDynamicTexturesCapable())
    {   // Get the video memory texture.
        pkD3DTexture = (D3DTexturePtr)(pkDynTxtrData->GetD3DTexture());
    }
    else
    {   // Get the system memory texture.
        pkD3DTexture = (D3DTexturePtr)(pkDynTxtrData->GetSysMemTexture());
    }
    if (!pkD3DTexture)
        return false;

    HRESULT eD3dRet = pkD3DTexture->UnlockRect(0);
    if (FAILED(eD3dRet))
    {
        NiDX9Renderer::Warning(
            "NiDX9Renderer::UnLockDynamicTexture Failed> "
            "UnlockRect - %s", NiDX9ErrorString((unsigned int)eD3dRet));
        return false;
    }

    if (!IsDynamicTexturesCapable())
    {
        // Get the video memory texture.
        D3DTexturePtr pkD3DVidMemTexture =
            (D3DTexturePtr)(pkDynTxtrData->GetD3DTexture());
        if (!pkD3DVidMemTexture)
            return false;

        // Update the video memory texture.
        eD3dRet =
            m_pkD3DDevice9->UpdateTexture(pkD3DTexture, pkD3DVidMemTexture);
        if (FAILED(eD3dRet))
        {
            NiDX9Renderer::Warning(
                "NiDX9Renderer::UnLockDynamicTexture Failed> "
                "UpdateTexture - %s",
                NiDX9ErrorString((unsigned int)eD3dRet));
            return false;
        }
    }

    pkDynTxtrData->SetLocked(false);

    return true;
}
//---------------------------------------------------------------------------
NiShader* NiDX9Renderer::GetFragmentShader(
    NiMaterialDescriptor* pkMaterialDescriptor)
{
    return NiNew NiDX9FragmentShader(pkMaterialDescriptor);
}
//---------------------------------------------------------------------------
NiShader* NiDX9Renderer::GetShadowWriteShader(
    NiMaterialDescriptor* pkMaterialDescriptor)
{
    return NiNew NiDX9ShadowWriteShader(pkMaterialDescriptor);
}
//---------------------------------------------------------------------------
void NiDX9Renderer::CreatePaletteRendererData(NiPalette* pkPalette)
{
    // Palettes not supported directly due to poor support in DX9 (one pal
    // per device, rather than one per texture stage)
    /* */
}
//---------------------------------------------------------------------------
bool NiDX9Renderer::CreateDepthStencilRendererData(
    NiDepthStencilBuffer* pkDSBuffer, const NiPixelFormat* pkFormat, 
    Ni2DBuffer::MultiSamplePreference eMSAAPref)
{
    if (pkDSBuffer == NULL)
        return false;
    
    NiDX9AdditionalDepthStencilBufferData* pkRendererData = 
        NiDX9AdditionalDepthStencilBufferData::Create(
        m_pkD3DDevice9, pkDSBuffer, pkFormat, eMSAAPref);

    if (pkRendererData == NULL)
        return false;
    else
        return true;
}
//---------------------------------------------------------------------------
void NiDX9Renderer::RemoveRenderedCubeMapData(NiRenderedCubeMap* pkCubeMap)
{
    pkCubeMap->SetRendererData(NULL);
    m_kRenderedCubeMaps.RemoveAt(pkCubeMap);
}
//---------------------------------------------------------------------------
void NiDX9Renderer::RemoveRenderedTextureData(NiRenderedTexture* pkTexture)
{
    pkTexture->SetRendererData(NULL);
    m_kRenderedTextures.RemoveAt(pkTexture);
}
//---------------------------------------------------------------------------
void NiDX9Renderer::RemoveDynamicTextureData(NiDynamicTexture* pkTexture)
{
    pkTexture->SetRendererData(NULL);
    m_kDynamicTextures.RemoveAt(pkTexture);
}
//---------------------------------------------------------------------------
void NiDX9Renderer::CreatePixelFormatArray(NiPixelFormat** ppkFormatArray,
    unsigned int uiUsage, D3DRESOURCETYPE eType)
{
    NIASSERT(m_spDefaultRenderTargetGroup);

    NiPixelFormat* pkPixFmt;
    for (unsigned int ui = 0; ui < TEX_NUM; ui++)
    {
        switch (ui)
        {
        case TEX_RGB555:
            {
                pkPixFmt = NiNew NiPixelFormat(NiPixelFormat::BGR555);
                NIASSERT(pkPixFmt);
                pkPixFmt->SetRendererHint(
                    NiDX9PixelFormat::DetermineD3DFormat(*pkPixFmt));
                pkPixFmt->SetExtraData(ui);
            }
            break;
        case TEX_RGB565:
            {
                pkPixFmt = NiNew NiPixelFormat(NiPixelFormat::BGR565);
                NIASSERT(pkPixFmt);
                pkPixFmt->SetRendererHint(
                    NiDX9PixelFormat::DetermineD3DFormat(*pkPixFmt));
                pkPixFmt->SetExtraData(ui);
            }
            break;
        case TEX_RGBA5551:
            {
                pkPixFmt = NiNew NiPixelFormat(NiPixelFormat::BGRA5551);
                NIASSERT(pkPixFmt);
                pkPixFmt->SetRendererHint(
                    NiDX9PixelFormat::DetermineD3DFormat(*pkPixFmt));
                pkPixFmt->SetExtraData(ui);
            }
            break;
        case TEX_RGBA4444:
            {
                pkPixFmt = NiNew NiPixelFormat(NiPixelFormat::BGRA4444);
                NIASSERT(pkPixFmt);
                pkPixFmt->SetRendererHint(
                    NiDX9PixelFormat::DetermineD3DFormat(*pkPixFmt));
                pkPixFmt->SetExtraData(ui);
            }
            break;
        case TEX_RGBA8888:
            {
                pkPixFmt = NiNew NiPixelFormat(NiPixelFormat::BGRA8888);
                NIASSERT(pkPixFmt);
                pkPixFmt->SetRendererHint(
                    NiDX9PixelFormat::DetermineD3DFormat(*pkPixFmt));
                pkPixFmt->SetExtraData(ui);
            }
            break;
        case TEX_PAL8:
        case TEX_PALA8:
            {
                // We do not support palettes in HW on DX9, as the DX9 
                // support for them is not good - only one per device,
                // rather than one per texture stage
                pkPixFmt = NULL;
            }
            break;
        case TEX_DXT1:
            {
                pkPixFmt = NiNew NiPixelFormat(NiPixelFormat::DXT1);
                NIASSERT(pkPixFmt);
                pkPixFmt->SetRendererHint(
                    NiDX9PixelFormat::DetermineD3DFormat(*pkPixFmt));
                pkPixFmt->SetExtraData(ui);
            }
            break;
        case TEX_DXT3:
            {
                pkPixFmt = NiNew NiPixelFormat(NiPixelFormat::DXT3);
                NIASSERT(pkPixFmt);
                pkPixFmt->SetRendererHint(
                    NiDX9PixelFormat::DetermineD3DFormat(*pkPixFmt));
                pkPixFmt->SetExtraData(ui);
            }
            break;
        case TEX_DXT5:
            {
                pkPixFmt = NiNew NiPixelFormat(NiPixelFormat::DXT5);
                NIASSERT(pkPixFmt);
                pkPixFmt->SetRendererHint(
                    NiDX9PixelFormat::DetermineD3DFormat(*pkPixFmt));
                pkPixFmt->SetExtraData(ui);
            }
            break;
        case TEX_BUMP88:
            {
                pkPixFmt = NiNew NiPixelFormat(NiPixelFormat::BUMP16);
                NIASSERT(pkPixFmt);
                pkPixFmt->SetRendererHint(
                    NiDX9PixelFormat::DetermineD3DFormat(*pkPixFmt));
                pkPixFmt->SetExtraData(ui);
            }
            break;
        case TEX_BUMPLUMA556:
            {
                pkPixFmt = NiNew NiPixelFormat(NiPixelFormat::BUMPLUMA556);
                NIASSERT(pkPixFmt);
                pkPixFmt->SetRendererHint(
                    NiDX9PixelFormat::DetermineD3DFormat(*pkPixFmt));
                pkPixFmt->SetExtraData(ui);
            }
            break;
        case TEX_BUMPLUMA888:
            {
                pkPixFmt = NiNew NiPixelFormat(NiPixelFormat::BUMPLUMA32);
                NIASSERT(pkPixFmt);
                pkPixFmt->SetRendererHint(
                    NiDX9PixelFormat::DetermineD3DFormat(*pkPixFmt));
                pkPixFmt->SetExtraData(ui);
            }
            break;
        case TEX_RGB888:
        case TEX_DEFAULT:
            {
                pkPixFmt = NiNew NiPixelFormat(NiPixelFormat::BGRX8888);
                NIASSERT(pkPixFmt);
                pkPixFmt->SetRendererHint(
                    NiDX9PixelFormat::DetermineD3DFormat(*pkPixFmt));
                pkPixFmt->SetExtraData(ui);
            }
            break;
        case TEX_L8:
            {
                pkPixFmt = NiNew NiPixelFormat(NiPixelFormat::I8);
                NIASSERT(pkPixFmt);
                pkPixFmt->SetRendererHint(
                    NiDX9PixelFormat::DetermineD3DFormat(*pkPixFmt));
                pkPixFmt->SetExtraData(ui);
            }
            break;
        case TEX_A8:
            {
                pkPixFmt = NiNew NiPixelFormat(NiPixelFormat::A8);
                NIASSERT(pkPixFmt);
                pkPixFmt->SetRendererHint(
                    NiDX9PixelFormat::DetermineD3DFormat(*pkPixFmt));
                pkPixFmt->SetExtraData(ui);
            }
            break;
        case TEX_R16F:
            {
                pkPixFmt = NiNew NiPixelFormat(NiPixelFormat::R16);
                NIASSERT(pkPixFmt);
                pkPixFmt->SetRendererHint(
                    NiDX9PixelFormat::DetermineD3DFormat(*pkPixFmt));
                pkPixFmt->SetExtraData(ui);
            }
            break;
        case TEX_RG32F:
            {
                pkPixFmt = NiNew NiPixelFormat(NiPixelFormat::RG32);
                NIASSERT(pkPixFmt);
                pkPixFmt->SetRendererHint(
                    NiDX9PixelFormat::DetermineD3DFormat(*pkPixFmt));
                pkPixFmt->SetExtraData(ui);
            }
            break;
        case TEX_RGBA64F:
            {
                pkPixFmt = NiNew NiPixelFormat(NiPixelFormat::RGBA64);
                NIASSERT(pkPixFmt);
                pkPixFmt->SetRendererHint(
                    NiDX9PixelFormat::DetermineD3DFormat(*pkPixFmt));
                pkPixFmt->SetExtraData(ui);
            }
            break;
        case TEX_R32F:
            {
                pkPixFmt = NiNew NiPixelFormat(NiPixelFormat::R32);
                NIASSERT(pkPixFmt);
                pkPixFmt->SetRendererHint(
                    NiDX9PixelFormat::DetermineD3DFormat(*pkPixFmt));
                pkPixFmt->SetExtraData(ui);
            }
            break;
        case TEX_RG64F:
            {
                pkPixFmt = NiNew NiPixelFormat(NiPixelFormat::RG64);
                NIASSERT(pkPixFmt);
                pkPixFmt->SetRendererHint(
                    NiDX9PixelFormat::DetermineD3DFormat(*pkPixFmt));
                pkPixFmt->SetExtraData(ui);
            }
            break;
        case TEX_RGBA128F:
            {
                pkPixFmt = NiNew NiPixelFormat(NiPixelFormat::RGBA128);
                NIASSERT(pkPixFmt);
                pkPixFmt->SetRendererHint(
                    NiDX9PixelFormat::DetermineD3DFormat(*pkPixFmt));
                pkPixFmt->SetExtraData(ui);
            }
            break;
        }

        if (pkPixFmt)
        {
            NiDelete ppkFormatArray[ui];

            if (ms_pkD3D9->CheckDeviceFormat(m_uiAdapter, m_eDevType,
                m_eAdapterFormat, 0, eType, 
                (D3DFORMAT)pkPixFmt->GetRendererHint()))
            {
                NiDelete pkPixFmt;
                pkPixFmt = NULL;
            }

            ppkFormatArray[ui] = pkPixFmt;
        }
    }
}
//---------------------------------------------------------------------------
unsigned char NiDX9Renderer::GetD3DFormatSize(D3DFORMAT eFormat)
{
    unsigned int uiValue = (unsigned int)eFormat;
    if (uiValue >= D3DFORMATSIZECOUNT)
        return 0;
    else
        return ms_aucD3DFormatSizes[uiValue];
}
//---------------------------------------------------------------------------
const NiDX9SystemDesc* NiDX9Renderer::GetSystemDesc()
{
    ms_kD3DCriticalSection.Lock();
    if (ms_pkSystemDesc)
    {
        ms_kD3DCriticalSection.Unlock();
        return ms_pkSystemDesc;
    }

    // Unlock so CreateDirect3D9 can pick up the lock - shouldn't make a
    // difference if the process gets interrupted here.
    ms_kD3DCriticalSection.Unlock();
    CreateDirect3D9();
    ms_kD3DCriticalSection.Lock();

    NIASSERT(ms_pkD3D9);
    ms_pkSystemDesc = NiNew NiDX9SystemDesc(ms_pkD3D9, ms_kDisplayFormats);
    ms_kD3DCriticalSection.Unlock();

    return ms_pkSystemDesc;
}
//---------------------------------------------------------------------------
bool NiDX9Renderer::IsHardwareSkinned(const NiGeometry* pkGeometry, 
    const NiD3DShaderInterface* pkShader)
{
    // Check if geometry has already been packed as non-HW skinning
    NIASSERT(pkGeometry->GetModelData());
    if (pkGeometry->GetModelData()->GetRendererData() != NULL)
        return false;

    // Check if geometry is skinned
    NiSkinInstance* pkSkinInstance = pkGeometry->GetSkinInstance();
    if (pkSkinInstance == 0)
        return false;

    // Check if skin is partitioned
    NiSkinPartition* pkPartition = pkSkinInstance->GetSkinPartition();
    if (pkPartition == 0)
        return false;

    // Check if geometry has already been packed as HW skinning
    NIASSERT(pkPartition->GetPartitions());
    if (pkPartition->GetPartitions()->m_pkBuffData != NULL)
        return true;

    // Check if device is HW skinning-capable
    if ((GetFlags() & CAPS_HARDWARESKINNING) != 0)
        return true;

    // Ensure pkShader is valid
    if (pkShader == NULL)
    {
        pkShader = NiDynamicCast(NiD3DShaderInterface, 
            pkGeometry->GetShader());
    }

    // Check for a shader that does the skinning
    // (assuming any shader attached that uses vertex shaders
    // will handle skinning)
    if (m_uiMaxVertexShaderVersion > D3DVS_VERSION(0,0) &&
        pkShader && pkShader->GetVSPresentAllPasses())
    {
        return true;
    }

    // Check if device is SW-switchable
    if (m_bSWVertexSwitchable == false)
        return false;

    // Check if shader is SW-skinning capable
    if (pkShader && pkShader->GetShaderDecl() && 
        pkShader->GetShaderDecl()->GetSoftwareVertexProcessing())
    {
        return true;
    }

    // No more tricks
    return false;
}
//---------------------------------------------------------------------------
NiTexturePtr NiDX9Renderer::CreateNiTextureFromD3DTexture(
    D3DBaseTexturePtr pkD3DTexture)
{
    if (pkD3DTexture == NULL)
        return NULL;

    NiDX9Direct3DTexturePtr spTexture = NiDX9Direct3DTexture::Create(this);
    NIASSERT(spTexture && "Failed to create NiDX9Direct3DTexture!");

    NiDX9Direct3DTextureData::Create(spTexture, this, pkD3DTexture);

    return NiSmartPointerCast(NiTexture, spTexture);
}
//---------------------------------------------------------------------------
bool NiDX9Renderer::RecreateNiTextureFromD3DTexture(NiTexture* pkTexture,
    D3DBaseTexturePtr pkD3DTexture)
{
    if (pkTexture == NULL || pkD3DTexture == NULL ||
        !NiIsKindOf(NiDX9Direct3DTexture, pkTexture))
    {
        return false;
    }

    NiDX9Direct3DTextureData* pkData = NiDX9Direct3DTextureData::Create(
        (NiDX9Direct3DTexture*)pkTexture, this, pkD3DTexture);

    return (pkData != NULL);
}
//---------------------------------------------------------------------------
void NiDX9Renderer::UseLegacyPipelineAsDefaultMaterial()
{
    m_spCurrentDefaultMaterial = m_spLegacyDefaultShaderMaterial;
}
//---------------------------------------------------------------------------
