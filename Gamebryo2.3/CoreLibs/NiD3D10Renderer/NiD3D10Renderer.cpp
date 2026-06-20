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
#include "NiD3D10RendererPCH.h"

#include "NiD3D10Renderer.h"

#include "NiD3D102DBufferData.h"
#include "NiD3D10DeviceState.h"
#include "NiD3D10Direct3DTexture.h"
#include "NiD3D10Direct3DTextureData.h"
#include "NiD3D10Error.h"
#include "NiD3D10ErrorShader.h"
#include "NiD3D10FragmentShader.h"
#include "NiD3D10GeometryData.h"
#include "NiD3D10GeometryPacker.h"
#include "NiD3D10GPUProgramCache.h"
#include "NiD3D10PixelFormat.h"
#include "NiD3D10RenderedTextureData.h"
#include "NiD3D10RendererSDM.h"
#include "NiD3D10RenderStateManager.h"
#include "NiD3D10ResourceManager.h"
#include "NiD3D10ShaderInterface.h"
#include "NiD3D10ShaderConstantManager.h"
#include "NiD3D10ShaderFactory.h"
#include "NiD3D10ShaderProgramFactory.h"
#include "NiD3D10SourceTextureData.h"
#include "NiD3D10Utility.h"
#include "NiD3D10VertexDescription.h"

#include <NiDirectionalShadowWriteMaterial.h>
#include <NiFragmentMaterial.h>
#include <NiLines.h>
#include <NiParticles.h>
#include <NiPointShadowWriteMaterial.h>
#include <NiRenderedCubeMap.h>
#include <NiRenderedTexture.h>
#include <NiScreenElements.h>
#include <NiScreenTexture.h>
#include <NiShadowGenerator.h>
#include <NiShadowManager.h>
#include <NiSingleShaderMaterial.h>
#include <NiSourceCubeMap.h>
#include <NiSourceTexture.h>
#include <NiSpotShadowWriteMaterial.h>
#include <NiStandardMaterial.h>
#include <NiTriShape.h>
#include <NiTriStrips.h>
#include <NiVersion.h>
#include <NiVertexColorProperty.h>

//---------------------------------------------------------------------------
// The following copyright notice may not be removed.
static char EmergentCopyright[] NI_UNUSED = 
    "Copyright 2007 Emergent Game Technologies";
//---------------------------------------------------------------------------
static char acGamebryoVersion[] NI_UNUSED = 
    GAMEBRYO_MODULE_VERSION_STRING(NiD3D10);
//---------------------------------------------------------------------------

HINSTANCE NiD3D10Renderer::ms_hD3D10 = NULL;
bool NiD3D10Renderer::ms_bForceNonDebugD3D10 = false;

NILPD3D10CREATEBLOB NiD3D10Renderer::ms_pfnD3D10CreateBlob = NULL;
NILPD3D10CREATEDEVICE NiD3D10Renderer::ms_pfnD3D10CreateDevice = NULL;
NILPD3D10CREATEDEVICEANDSWAPCHAIN 
    NiD3D10Renderer::ms_pfnD3D10CreateDeviceAndSwapChain = NULL;
NILPD3D10CREATESTATEBLOCK NiD3D10Renderer::ms_pfnD3D10CreateStateBlock = NULL;
NILPD3D10GETINPUTANDOUTPUTSIGNATUREBLOB 
    NiD3D10Renderer::ms_pfnD3D10GetInputAndOutputSignatureBlob = NULL;
NILPD3D10GETINPUTSIGNATUREBLOB 
    NiD3D10Renderer::ms_pfnD3D10GetInputSignatureBlob = NULL;
NILPD3D10GETOUTPUTSIGNATUREBLOB 
    NiD3D10Renderer::ms_pfnD3D10GetOutputSignatureBlob = NULL;
NILPD3D10STATEBLOCKMASKDIFFERENCE 
    NiD3D10Renderer::ms_pfnD3D10StateBlockMaskDifference = NULL;
NILPD3D10STATEBLOCKMASKDISABLEALL 
    NiD3D10Renderer::ms_pfnD3D10StateBlockMaskDisableAll = NULL;
NILPD3D10STATEBLOCKMASKDISABLECAPTURE 
    NiD3D10Renderer::ms_pfnD3D10StateBlockMaskDisableCapture = NULL;
NILPD3D10STATEBLOCKMASKENABLEALL 
    NiD3D10Renderer::ms_pfnD3D10StateBlockMaskEnableAll = NULL;
NILPD3D10STATEBLOCKMASKENABLECAPTURE 
    NiD3D10Renderer::ms_pfnD3D10StateBlockMaskEnableCapture = NULL;
NILPD3D10STATEBLOCKMASKGETSETTING 
    NiD3D10Renderer::ms_pfnD3D10StateBlockMaskGetSetting = NULL;
NILPD3D10STATEBLOCKMASKINTERSECT 
    NiD3D10Renderer::ms_pfnD3D10StateBlockMaskIntersect = NULL;
NILPD3D10STATEBLOCKMASKUNION 
    NiD3D10Renderer::ms_pfnD3D10StateBlockMaskUnion = NULL;

NILPD3D10COMPILESHADER NiD3D10Renderer::ms_pfnD3D10CompileShader = NULL;
NILPD3D10DISASSEMBLESHADER NiD3D10Renderer::ms_pfnD3D10DisassembleShader = 
    NULL;
NILPD3D10GETGEOMETRYSHADERPROFILE 
    NiD3D10Renderer::ms_pfnD3D10GetGeometryShaderProfile = NULL;
NILPD3D10GETPIXELSHADERPROFILE 
    NiD3D10Renderer::ms_pfnD3D10GetPixelShaderProfile = NULL;
NILPD3D10GETVERTEXSHADERPROFILE 
    NiD3D10Renderer::ms_pfnD3D10GetVertexShaderProfile = NULL;
NILPD3D10PREPROCESSSHADER 
    NiD3D10Renderer::ms_pfnD3D10PreprocessShader = NULL;
NILPD3D10REFLECTSHADER 
    NiD3D10Renderer::ms_pfnD3D10ReflectShader = NULL;

NILPD3D10COMPILEEFFECTFROMMEMORY 
    NiD3D10Renderer::ms_pfnD3D10CompileEffectFromMemory = NULL;
NILPD3D10CREATEEFFECTFROMMEMORY 
    NiD3D10Renderer::ms_pfnD3D10CreateEffectFromMemory = NULL;
NILPD3D10CREATEEFFECTPOOLFROMMEMORY 
    NiD3D10Renderer::ms_pfnD3D10CreateEffectPoolFromMemory = NULL;
NILPD3D10DISASSEMBLEEFFECT NiD3D10Renderer::ms_pfnD3D10DisassembleEffect = 
    NULL;

NiCriticalSection NiD3D10Renderer::ms_kD3D10LibraryCriticalSection;

static NiD3D10RendererSDM NiD3D10RendererSDMObject;

NiImplementRTTI(NiD3D10Renderer,NiRenderer);

//---------------------------------------------------------------------------
NiD3D10Renderer::CreationParameters::CreationParameters() :
    m_uiAdapterIndex(0),
    m_uiOutputIndex(0),
    m_eDriverType(DRIVER_HARDWARE),
#if defined(_DEBUG)
    m_uiCreateFlags(CREATE_DEVICE_DEBUG | CREATE_DEVICE_SWITCH_TO_REF),
#else //#if defined(_DEBUG)
    m_uiCreateFlags(0),
#endif //#if defined(_DEBUG)
    m_bCreateSwapChain(false),
    m_bCreateDepthStencilBuffer(true),
    m_bAssociateWithWindow(false),
    m_uiWindowAssociationFlags(0),
    m_eDepthStencilFormat(DXGI_FORMAT_UNKNOWN)
{
    m_kSwapChain.BufferDesc.Width = 640;
    m_kSwapChain.BufferDesc.Height= 480;
    m_kSwapChain.BufferDesc.RefreshRate.Numerator = 60;
    m_kSwapChain.BufferDesc.RefreshRate.Denominator = 1;
    m_kSwapChain.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    m_kSwapChain.BufferDesc.ScanlineOrdering = 
        DXGI_MODE_SCANLINE_ORDER_PROGRESSIVE;
    m_kSwapChain.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
    m_kSwapChain.SampleDesc.Count = 1;
    m_kSwapChain.SampleDesc.Quality = 0;
    m_kSwapChain.BufferUsage = DXGI_USAGE_BACK_BUFFER | 
        DXGI_USAGE_RENDER_TARGET_OUTPUT;
    m_kSwapChain.BufferCount = 1;
    m_kSwapChain.OutputWindow = NULL;
    m_kSwapChain.Windowed = false;
    m_kSwapChain.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
    m_kSwapChain.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
}
//---------------------------------------------------------------------------
NiD3D10Renderer::CreationParameters::CreationParameters(HWND hWnd) :
    m_uiAdapterIndex(0),
    m_uiOutputIndex(0),
    m_eDriverType(DRIVER_HARDWARE),
#if defined(_DEBUG)
    m_uiCreateFlags(CREATE_DEVICE_DEBUG | CREATE_DEVICE_SWITCH_TO_REF),
#else //#if defined(_DEBUG)
    m_uiCreateFlags(0),
#endif //#if defined(_DEBUG)
    m_bCreateSwapChain(true),
    m_bCreateDepthStencilBuffer(true),
    m_bAssociateWithWindow(true),
    m_uiWindowAssociationFlags(0),
    m_eDepthStencilFormat(DXGI_FORMAT_UNKNOWN)
{
    m_kSwapChain.BufferDesc.Width = 640;
    m_kSwapChain.BufferDesc.Height = 480;
    m_kSwapChain.BufferDesc.RefreshRate.Numerator = 60;
    m_kSwapChain.BufferDesc.RefreshRate.Denominator = 1;
    m_kSwapChain.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    m_kSwapChain.BufferDesc.ScanlineOrdering = 
        DXGI_MODE_SCANLINE_ORDER_PROGRESSIVE;
    m_kSwapChain.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
    m_kSwapChain.SampleDesc.Count = 1;
    m_kSwapChain.SampleDesc.Quality = 0;
    m_kSwapChain.BufferUsage = DXGI_USAGE_BACK_BUFFER | 
        DXGI_USAGE_RENDER_TARGET_OUTPUT;
    m_kSwapChain.BufferCount = 1;
    m_kSwapChain.OutputWindow = NULL;
    m_kSwapChain.Windowed = false;
    m_kSwapChain.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
    m_kSwapChain.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
    if (hWnd)
    {
        m_kSwapChain.OutputWindow = hWnd;
        m_kSwapChain.Windowed = true;
        RECT kRect;
        ::GetClientRect(hWnd, &kRect);

        m_kSwapChain.BufferDesc.Width = kRect.right - kRect.left;
        m_kSwapChain.BufferDesc.Height = kRect.bottom - kRect.top;
    }
}
//---------------------------------------------------------------------------
NiD3D10Renderer::NiD3D10Renderer() :
    m_pkD3D10Device(NULL),
    m_uiAdapterIndex(NULL_ADAPTER),
    m_bInitialized(false),
    m_pkCurrentRenderTargetGroup(NULL),
    m_fDepthClear(1.0f),
    m_ucStencilClear(0),
    m_bLeftRightSwap(false),
    m_kCurrentFrustum(-1.0f, 1.0f, -1.0f, 1.0f, 0.0f, 1.0f),
    m_kCurrentViewPort(0.0f, 1.0f, 1.0f, 0.0f),
    m_uiSyncInterval(1),
    m_fMaxFogFactor(0.0f),
    m_fMaxFogValue(1.0f),
    m_pkDeviceState(NULL),
    m_pkGeometryPacker(NULL),
    m_pkRenderStateManager(NULL),
    m_pkResourceManager(NULL),
    m_pkShaderConstantManager(NULL),
    m_uiMaxVertexShaderVersion(0),
    m_uiMaxGeometryShaderVersion(0),
    m_uiMaxPixelShaderVersion(0),
    m_pkTempVertices(NULL),
    m_pkTempColors(NULL),
    m_pkTempTexCoords(NULL),
    m_uiTempArraySize(0),
    m_pkLastScreenTexture(NULL),
    m_bDeviceOccluded(false),
    m_bDeviceRemoved(false)
{
    memset(m_afBackgroundColor, 0, sizeof(m_afBackgroundColor));
    memset(m_auiFormatSupport, 0, sizeof(m_auiFormatSupport));

    D3DXMatrixIdentity(&m_kD3DView);
    D3DXMatrixIdentity(&m_kInvView);
    D3DXMatrixIdentity(&m_kD3DProj);
    D3DXMatrixIdentity(&m_kD3DModel);
}
//---------------------------------------------------------------------------
NiD3D10Renderer::~NiD3D10Renderer()
{
    m_spSTScreenElements = 0;

    NiMaterial::UnloadShadersForAllMaterials();

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

    m_spBatchMaterial = 0;

    NiDelete[] m_pkTempVertices;
    NiDelete[] m_pkTempColors;
    NiDelete[] m_pkTempTexCoords;

    // Purge swap chains
    NiTListIterator kIter = m_kSwapChainRenderTargetGroups.GetFirstPos();
    while (kIter)
    {
        HWND hWnd;
        NiRenderTargetGroupPtr spRenderTargetGroup;

        m_kSwapChainRenderTargetGroups.GetNext(kIter, hWnd, 
            spRenderTargetGroup);
        if (spRenderTargetGroup)
        {
            for (unsigned int i = 0; 
                i < spRenderTargetGroup->GetBufferCount(); i++)
            {
                Ni2DBuffer* pkBuffer = spRenderTargetGroup->GetBuffer(i);
                if (pkBuffer)
                    pkBuffer->SetRendererData(NULL);
            }

            Ni2DBuffer* pkBuffer = 
                spRenderTargetGroup->GetDepthStencilBuffer();
            if (pkBuffer)
                pkBuffer->SetRendererData(NULL);
        }
    }
    m_kSwapChainRenderTargetGroups.RemoveAll();

    // Purge textures
    PurgeAllTextures(true);

    // Purge the D3D10Shaders
    PurgeAllD3D10Shaders();

    // Release all D3D10Shaders loaded by the shader factory.
    NiD3D10ShaderFactory* pkShaderFactory = 
        NiD3D10ShaderFactory::GetD3D10ShaderFactory();
    if (pkShaderFactory)
    {
        pkShaderFactory->RemoveAllShaders();
        pkShaderFactory->RemoveAllLibraries();
    }

    ReleaseManagers();
    ReleaseResources();
    ReleaseDevice();
}
//---------------------------------------------------------------------------
void NiD3D10Renderer::_SDMShutdown()
{
    ReleaseD3D10();
}
//---------------------------------------------------------------------------
bool NiD3D10Renderer::Create(CreationParameters& kCreate, 
    NiD3D10RendererPtr& spRenderer)
{
    spRenderer = NULL;

    spRenderer = NiNew NiD3D10Renderer;
    NIASSERT(spRenderer);
    return spRenderer->Initialize(kCreate);
}
//---------------------------------------------------------------------------
void NiD3D10Renderer::GetCreationParameters(CreationParameters& kCreate) const
{
    kCreate = m_kInitialCreateParameters;
}
//---------------------------------------------------------------------------
bool NiD3D10Renderer::ResizeBuffers(unsigned int uiWidth, 
    unsigned int uiHeight, HWND hOutputWnd)
{
    NiRenderTargetGroup* pkRTG = m_spDefaultRenderTargetGroup;
    if (hOutputWnd)
        pkRTG = GetSwapChainRenderTargetGroup(hOutputWnd);

    if (pkRTG == NULL)
        return false;

    NiD3D10SwapChainBufferData* pkSwapChainBufferData =
        (NiD3D10SwapChainBufferData*)pkRTG->GetBufferRendererData(0);

    if (pkSwapChainBufferData == NULL)
        return false;
    
    // Release depth/stencil buffer
    bool bDSBuffer = pkRTG->HasDepthStencil();
    const NiPixelFormat* pkDSFormat = pkRTG->GetDepthStencilPixelFormat();

    pkRTG->AttachDepthStencilBuffer(NULL);

    bool bSuccess = pkSwapChainBufferData->ResizeSwapChain(uiWidth, uiHeight);
    if (!bSuccess)
    {
        // Don't return here - still need to recreate DS buffer.
    }

    // Recreate depth/stencil buffer
    if (bDSBuffer)
    {
        NiDepthStencilBufferPtr spDepthBuffer = NiDepthStencilBuffer::Create(
            pkSwapChainBufferData->GetWidth(), 
            pkSwapChainBufferData->GetHeight(), this, *pkDSFormat,
            pkSwapChainBufferData->GetMSAAPref());
        pkRTG->AttachDepthStencilBuffer(spDepthBuffer);
    }

    return bSuccess;
}
//---------------------------------------------------------------------------
bool NiD3D10Renderer::Initialize(CreationParameters& kCreate)
{
    if (m_bInitialized)
    {
        NiD3D10Error::ReportWarning("Attempting to initialize renderer "
            "that is already initialized.");
        return false;
    }

    if (CreateDevice(kCreate))
    {
        CreateManagers();

        if (kCreate.m_bCreateSwapChain == false)
        {
            m_bInitialized = true;
        }
        else
        {
            IDXGISwapChain* pkSwapChain = 
                CreateSwapChain(kCreate.m_kSwapChain, kCreate.m_uiOutputIndex);
            if (pkSwapChain != NULL)
            {
                NiRenderTargetGroup* pkRTGroup = 
                    CreateRenderTargetGroupFromSwapChain(pkSwapChain, 
                    kCreate.m_bCreateDepthStencilBuffer,
                    kCreate.m_eDepthStencilFormat);
                if (pkRTGroup)
                {
                    m_spDefaultRenderTargetGroup = pkRTGroup;

                    m_kSwapChainRenderTargetGroups.SetAt(
                        kCreate.m_kSwapChain.OutputWindow, pkRTGroup);

                    m_bInitialized = true;
                }
                else
                {
                    NiD3D10Error::ReportWarning("Initialization failed "
                        "because NiRenderTargetGroup for default swap chain "
                        "could not be created; destroying D3D10 Device.");
                }
            }
            else
            {
                NiD3D10Error::ReportWarning("Initialization failed because "
                    "default swap chain could not be created; "
                    "destroying D3D10 Device.");
            }
        }
    }

    if (m_bInitialized == false)
    {
        // Initialization failed
        ReleaseDevice();

        return false;
    }

    if (kCreate.m_bAssociateWithWindow)
    {
        IDXGIFactory* pkFactory = m_spSystemDesc->GetFactory();
        NIASSERT(pkFactory);

        pkFactory->MakeWindowAssociation(
            kCreate.m_kSwapChain.OutputWindow,
            GetD3D10WindowAssociationFlags(
            kCreate.m_uiWindowAssociationFlags));
    }

    // Initialize shader system
    NIASSERT(NiD3D10ShaderFactory::GetD3D10ShaderFactory());
    NIASSERT(NiD3D10ShaderProgramFactory::GetInstance());

    NiD3D10ShaderFactory* pkShaderFactory = 
        NiD3D10ShaderFactory::GetD3D10ShaderFactory();

    pkShaderFactory->SetAsActiveFactory();

    // Setup the default shader version information
    m_uiMaxVertexShaderVersion = 
        pkShaderFactory->CreateVertexShaderVersion(4, 0);
    m_uiMaxGeometryShaderVersion = 
        pkShaderFactory->CreateGeometryShaderVersion(4, 0);
    m_uiMaxPixelShaderVersion = 
        pkShaderFactory->CreatePixelShaderVersion(4, 0);

    // Inform the shadowing system of the shader model versions used by the 
    // current hardware.
    if (NiShadowManager::GetShadowManager())
    {
        NiShadowManager::ValidateShaderVersions(
            pkShaderFactory->GetMajorVertexShaderVersion(   // Vertex Shader
            m_uiMaxVertexShaderVersion),
            pkShaderFactory->GetMinorVertexShaderVersion(
            m_uiMaxVertexShaderVersion),
            pkShaderFactory->GetMajorVertexShaderVersion(   // Geometry Shader
            m_uiMaxGeometryShaderVersion), 
            pkShaderFactory->GetMinorVertexShaderVersion(
            m_uiMaxGeometryShaderVersion),
            pkShaderFactory->GetMajorVertexShaderVersion(   // Pixel Shader
            m_uiMaxPixelShaderVersion),
            pkShaderFactory->GetMinorVertexShaderVersion(
            m_uiMaxPixelShaderVersion));
    }

    NiStandardMaterial* pkStandardMaterial = NiDynamicCast(
        NiStandardMaterial, m_spInitialDefaultMaterial);
    if (pkStandardMaterial)
    {
        NiRenderer::SetDefaultProgramCache(pkStandardMaterial);
    }
    m_spCurrentDefaultMaterial = m_spInitialDefaultMaterial;

    // Shadow manager
    if (NiShadowManager::GetShadowManager())
    {
        NiDirectionalShadowWriteMaterial* pkDirShadowWriteMaterial = 
            (NiDirectionalShadowWriteMaterial*)NiShadowManager::
            GetShadowWriteMaterial(NiStandardMaterial::LIGHT_DIR);

        if (pkDirShadowWriteMaterial)
        {
            NiRenderer::SetDefaultProgramCache(pkDirShadowWriteMaterial);
        }

        NiPointShadowWriteMaterial* pkPointShadowWriteMaterial = 
            (NiPointShadowWriteMaterial*)NiShadowManager::
            GetShadowWriteMaterial(NiStandardMaterial::LIGHT_POINT);

        if (pkPointShadowWriteMaterial)
        {
            NiRenderer::SetDefaultProgramCache(pkPointShadowWriteMaterial);
        }

        NiSpotShadowWriteMaterial* pkSpotShadowWriteMaterial = 
            (NiSpotShadowWriteMaterial*)NiShadowManager::
            GetShadowWriteMaterial(NiStandardMaterial::LIGHT_SPOT);

        if (pkSpotShadowWriteMaterial)
        {
            NiRenderer::SetDefaultProgramCache(pkSpotShadowWriteMaterial);
        }
    }

    m_kShaderLibraryVersion.SetSystemVertexShaderVersion(
        m_uiMaxVertexShaderVersion);
    m_kShaderLibraryVersion.SetMinVertexShaderVersion(1, 1);
    m_kShaderLibraryVersion.SetVertexShaderVersionRequest(
        m_uiMaxVertexShaderVersion);
    m_kShaderLibraryVersion.SetSystemGeometryShaderVersion(
        m_uiMaxGeometryShaderVersion);
    m_kShaderLibraryVersion.SetMinGeometryShaderVersion(4, 0);
    m_kShaderLibraryVersion.SetGeometryShaderVersionRequest(
        m_uiMaxGeometryShaderVersion);
    m_kShaderLibraryVersion.SetSystemPixelShaderVersion(
        m_uiMaxPixelShaderVersion);
    m_kShaderLibraryVersion.SetMinPixelShaderVersion(1, 1);
    m_kShaderLibraryVersion.SetPixelShaderVersionRequest(
        m_uiMaxPixelShaderVersion);
    m_kShaderLibraryVersion.SetSystemUserVersion(0, 0);
    m_kShaderLibraryVersion.SetMinUserVersion(0, 0);
    m_kShaderLibraryVersion.SetUserVersionRequest(0, 0);
    m_kShaderLibraryVersion.SetPlatform(NiShader::NISHADER_D3D10);
    
    NiD3D10ShaderFactory* pkFactory = 
        NiD3D10ShaderFactory::GetD3D10ShaderFactory();

    NiShader* pkErrorShader = NiNew NiD3D10ErrorShader;
    pkErrorShader->Initialize();

    SetErrorShader(pkErrorShader);

    // Initialize the safe zone to the inner 98% of the display
    m_kDisplaySafeZone.m_top =  0.01f;
    m_kDisplaySafeZone.m_left =  0.01f;
    m_kDisplaySafeZone.m_right =  0.99f;
    m_kDisplaySafeZone.m_bottom =  0.99f;

    m_bDeviceOccluded = false;
    m_bDeviceRemoved = false;

    return m_bInitialized;
}
//---------------------------------------------------------------------------
bool NiD3D10Renderer::CreateSwapChainRenderTargetGroup(
    DXGI_SWAP_CHAIN_DESC& kSwapChainDesc, unsigned int uiOutputIndex, 
    bool bCreateDepthStencilBuffer, DXGI_FORMAT eDepthStencilFormat)
{
    // Is there an output window?
    if (kSwapChainDesc.OutputWindow == NULL)
    {
        return false;
    }

    // Does a swap chain already exist for this window?
    NiRenderTargetGroupPtr spRenderTargetGroup;
    if (m_kSwapChainRenderTargetGroups.GetAt(kSwapChainDesc.OutputWindow, 
        spRenderTargetGroup))
    {
        return false;
    }

    IDXGISwapChain* pkSwapChain = 
        CreateSwapChain(kSwapChainDesc, uiOutputIndex);
    if (pkSwapChain != NULL)
    {
        NiRenderTargetGroup* pkRTGroup = 
            CreateRenderTargetGroupFromSwapChain(pkSwapChain, 
            bCreateDepthStencilBuffer, eDepthStencilFormat);

        m_kSwapChainRenderTargetGroups.SetAt(
            kSwapChainDesc.OutputWindow, pkRTGroup);

        return true;
    }

    return false;
}
//---------------------------------------------------------------------------
void NiD3D10Renderer::DestroySwapChainRenderTargetGroup(HWND hWnd)
{
    // Does the framebuffer exist?
    if (!hWnd)
    {
        return;
    }

    NiRenderTargetGroupPtr spRenderTargetGroup = NULL;

    // Does the framebuffer exist?
    if (!m_kSwapChainRenderTargetGroups.GetAt(hWnd, spRenderTargetGroup))
    {
        return;
    }

    // can only delete non-primary buffers that are not current targets
    if (spRenderTargetGroup != m_pkCurrentRenderTargetGroup)
    {
        m_kSwapChainRenderTargetGroups.RemoveAt(hWnd);

        // Check to see if this is the default render target group
        if (spRenderTargetGroup == m_spDefaultRenderTargetGroup)
        {
            // Try to find another render target group to use as the default
            NiRenderTargetGroupPtr spNewDefaultRenderTargetGroup;
            if (!m_kSwapChainRenderTargetGroups.IsEmpty())
            {
                NiTMapIterator kIter = 
                    m_kSwapChainRenderTargetGroups.GetFirstPos();
                HWND hNewWnd = NULL;
                m_kSwapChainRenderTargetGroups.GetNext(kIter, hNewWnd, 
                    spNewDefaultRenderTargetGroup);
            }

            m_spDefaultRenderTargetGroup = spNewDefaultRenderTargetGroup;
        }

        spRenderTargetGroup = NULL;
    }
    else
    {
        //...
    }
}
//---------------------------------------------------------------------------
NiRenderTargetGroup* NiD3D10Renderer::GetSwapChainRenderTargetGroup(HWND hWnd)
    const
{
    if (!hWnd)
    {
        return NULL;
    }

    NiRenderTargetGroupPtr spRenderTargetGroup;

    // Does the framebuffer exist?
    if (m_kSwapChainRenderTargetGroups.GetAt(hWnd, spRenderTargetGroup))
        return spRenderTargetGroup;

    return NULL;
}
//---------------------------------------------------------------------------
bool NiD3D10Renderer::SetDefaultSwapChainRenderTargetGroup(HWND hWnd)
{
    NiRenderTargetGroup* pkRTG = GetSwapChainRenderTargetGroup(hWnd);
    if (pkRTG == NULL)
        return false;

    m_spDefaultRenderTargetGroup = pkRTG;
    return true;
}
//---------------------------------------------------------------------------
void NiD3D10Renderer::SetForceNonDebugD3D10(bool bForceNonDebugD3D10)
{
    ms_bForceNonDebugD3D10 = bForceNonDebugD3D10;
}
//---------------------------------------------------------------------------
bool NiD3D10Renderer::GetForceNonDebugD3D10()
{
    return ms_bForceNonDebugD3D10;
}
//---------------------------------------------------------------------------
bool NiD3D10Renderer::EnsureD3D10Loaded()
{
    ms_kD3D10LibraryCriticalSection.Lock();

    bool bSuccess = true;
    if (ms_hD3D10 == NULL)
        bSuccess = LoadD3D10();

    ms_kD3D10LibraryCriticalSection.Unlock();

    return bSuccess;
}
//---------------------------------------------------------------------------
bool NiD3D10Renderer::LoadD3D10()
{
    ms_kD3D10LibraryCriticalSection.Lock();

    if (ms_hD3D10 != NULL)
    {
        ms_kD3D10LibraryCriticalSection.Unlock();
        return true;
    }

    const char* const pcDebugLibraryName = "D3D10d.dll";
    const char* const pcReleaseLibraryName = "D3D10.dll";

    const char* pcLibraryName = 
#if defined(_DEBUG)
        (ms_bForceNonDebugD3D10 ? pcReleaseLibraryName : pcDebugLibraryName);
#else //#if defined(_DEBUG)
        pcReleaseLibraryName;
#endif //#if defined(_DEBUG)

    ms_hD3D10 = LoadLibrary(pcLibraryName);
#if defined(_DEBUG)
    if (ms_hD3D10 == NULL && pcLibraryName == pcDebugLibraryName)
        ms_hD3D10 = LoadLibrary(pcReleaseLibraryName);
#endif //#if defined(_DEBUG)

    if (ms_hD3D10 == NULL)
    {
        NiD3D10Error::ReportError(
            NiD3D10Error::NID3D10ERROR_D3D10_LIB_MISSING);
        return false;
    }

    ms_pfnD3D10CreateBlob = (NILPD3D10CREATEBLOB)
        GetProcAddress(ms_hD3D10, "D3D10CreateBlob");
    ms_pfnD3D10CreateDevice = (NILPD3D10CREATEDEVICE)
        GetProcAddress(ms_hD3D10, "D3D10CreateDevice");
    ms_pfnD3D10CreateDeviceAndSwapChain = (NILPD3D10CREATEDEVICEANDSWAPCHAIN)
        GetProcAddress(ms_hD3D10, "D3D10CreateDeviceAndSwapChain");
    ms_pfnD3D10CreateStateBlock = (NILPD3D10CREATESTATEBLOCK)
        GetProcAddress(ms_hD3D10, "D3D10CreateStateBlock");
    ms_pfnD3D10GetInputAndOutputSignatureBlob = 
        (NILPD3D10GETINPUTANDOUTPUTSIGNATUREBLOB)
        GetProcAddress(ms_hD3D10, "D3D10GetInputAndOutputSignatureBlob");
    ms_pfnD3D10GetInputSignatureBlob = (NILPD3D10GETINPUTSIGNATUREBLOB)
        GetProcAddress(ms_hD3D10, "D3D10GetInputSignatureBlob");
    ms_pfnD3D10GetOutputSignatureBlob = (NILPD3D10GETOUTPUTSIGNATUREBLOB)
        GetProcAddress(ms_hD3D10, "D3D10GetOutputSignatureBlob");
    ms_pfnD3D10StateBlockMaskDifference = (NILPD3D10STATEBLOCKMASKDIFFERENCE)
        GetProcAddress(ms_hD3D10, "D3D10StateBlockMaskDifference");
    ms_pfnD3D10StateBlockMaskDisableAll = (NILPD3D10STATEBLOCKMASKDISABLEALL)
        GetProcAddress(ms_hD3D10, "D3D10StateBlockMaskDisableAll");
    ms_pfnD3D10StateBlockMaskDisableCapture = 
        (NILPD3D10STATEBLOCKMASKDISABLECAPTURE)
        GetProcAddress(ms_hD3D10, "D3D10StateBlockMaskDisableCapture");
    ms_pfnD3D10StateBlockMaskEnableAll = (NILPD3D10STATEBLOCKMASKENABLEALL)
        GetProcAddress(ms_hD3D10, "D3D10StateBlockMaskEnableAll");
    ms_pfnD3D10StateBlockMaskEnableCapture = 
        (NILPD3D10STATEBLOCKMASKENABLECAPTURE)
        GetProcAddress(ms_hD3D10, "D3D10StateBlockMaskEnableCapture");
    ms_pfnD3D10StateBlockMaskGetSetting = (NILPD3D10STATEBLOCKMASKGETSETTING)
        GetProcAddress(ms_hD3D10, "D3D10StateBlockMaskGetSetting");
    ms_pfnD3D10StateBlockMaskIntersect = (NILPD3D10STATEBLOCKMASKINTERSECT)
        GetProcAddress(ms_hD3D10, "D3D10StateBlockMaskIntersect");
    ms_pfnD3D10StateBlockMaskUnion = (NILPD3D10STATEBLOCKMASKUNION)
        GetProcAddress(ms_hD3D10, "D3D10StateBlockMaskUnion");

    ms_pfnD3D10CompileShader = (NILPD3D10COMPILESHADER)
        GetProcAddress(ms_hD3D10, "D3D10CompileShader");
    ms_pfnD3D10DisassembleShader = (NILPD3D10DISASSEMBLESHADER)
        GetProcAddress(ms_hD3D10, "D3D10DisassembleShader");
    ms_pfnD3D10GetGeometryShaderProfile = (NILPD3D10GETGEOMETRYSHADERPROFILE)
        GetProcAddress(ms_hD3D10, "D3D10GetGeometryShaderProfile");
    ms_pfnD3D10GetPixelShaderProfile = (NILPD3D10GETPIXELSHADERPROFILE)
        GetProcAddress(ms_hD3D10, "D3D10GetPixelShaderProfile");
    ms_pfnD3D10GetVertexShaderProfile = (NILPD3D10GETVERTEXSHADERPROFILE)
        GetProcAddress(ms_hD3D10, "D3D10GetVertexShaderProfile");
    ms_pfnD3D10PreprocessShader = (NILPD3D10PREPROCESSSHADER)
        GetProcAddress(ms_hD3D10, "D3D10PreprocessShader");
    ms_pfnD3D10ReflectShader = (NILPD3D10REFLECTSHADER)
        GetProcAddress(ms_hD3D10, "D3D10ReflectShader");

    ms_pfnD3D10CompileEffectFromMemory = (NILPD3D10COMPILEEFFECTFROMMEMORY)
        GetProcAddress(ms_hD3D10, "D3D10CompileEffectFromMemory");
    ms_pfnD3D10CreateEffectFromMemory = (NILPD3D10CREATEEFFECTFROMMEMORY)
        GetProcAddress(ms_hD3D10, "D3D10CreateEffectFromMemory");
    ms_pfnD3D10CreateEffectPoolFromMemory = 
        (NILPD3D10CREATEEFFECTPOOLFROMMEMORY)
        GetProcAddress(ms_hD3D10, "D3D10CreateEffectPoolFromMemory");
    ms_pfnD3D10DisassembleEffect = (NILPD3D10DISASSEMBLEEFFECT)
        GetProcAddress(ms_hD3D10, "D3D10DisassembleEffect");

    if (ms_pfnD3D10CreateBlob == NULL ||
        ms_pfnD3D10CreateDevice == NULL ||
        ms_pfnD3D10CreateDeviceAndSwapChain == NULL ||
        ms_pfnD3D10CreateStateBlock == NULL ||
        ms_pfnD3D10GetInputAndOutputSignatureBlob == NULL ||
        ms_pfnD3D10GetInputSignatureBlob == NULL ||
        ms_pfnD3D10GetOutputSignatureBlob == NULL ||
        ms_pfnD3D10StateBlockMaskDifference == NULL ||
        ms_pfnD3D10StateBlockMaskDisableAll == NULL ||
        ms_pfnD3D10StateBlockMaskDisableCapture == NULL ||
        ms_pfnD3D10StateBlockMaskEnableAll == NULL ||
        ms_pfnD3D10StateBlockMaskEnableCapture == NULL ||
        ms_pfnD3D10StateBlockMaskGetSetting == NULL ||
        ms_pfnD3D10StateBlockMaskIntersect == NULL ||
        ms_pfnD3D10StateBlockMaskUnion == NULL ||
        ms_pfnD3D10CompileShader == NULL ||
        ms_pfnD3D10DisassembleShader == NULL ||
        ms_pfnD3D10GetGeometryShaderProfile == NULL ||
        ms_pfnD3D10GetPixelShaderProfile == NULL ||
        ms_pfnD3D10GetVertexShaderProfile == NULL ||
        ms_pfnD3D10PreprocessShader == NULL ||
        ms_pfnD3D10ReflectShader == NULL ||
        ms_pfnD3D10CompileEffectFromMemory == NULL ||
        ms_pfnD3D10CreateEffectFromMemory == NULL ||
        ms_pfnD3D10CreateEffectPoolFromMemory == NULL ||
        ms_pfnD3D10DisassembleEffect == NULL)
    {
        NiD3D10Error::ReportError(NiD3D10Error::NID3D10ERROR_D3D10_LIB_MISSING,
            "Library loaded but some procedure addresses not found; "
            "releasing library.");

        ReleaseD3D10();
        ms_kD3D10LibraryCriticalSection.Unlock();

        return false;
    }

    ms_kD3D10LibraryCriticalSection.Unlock();

    return true;
}
//---------------------------------------------------------------------------
bool NiD3D10Renderer::ReleaseD3D10()
{
    ms_kD3D10LibraryCriticalSection.Lock();

    if (ms_hD3D10)
    {
        FreeLibrary(ms_hD3D10);
        ms_hD3D10 = NULL;
        ms_pfnD3D10CreateBlob = NULL;
        ms_pfnD3D10CreateDevice = NULL;
        ms_pfnD3D10CreateDeviceAndSwapChain = NULL;
        ms_pfnD3D10CreateStateBlock = NULL;
        ms_pfnD3D10GetInputAndOutputSignatureBlob = NULL;
        ms_pfnD3D10GetInputSignatureBlob = NULL;
        ms_pfnD3D10GetOutputSignatureBlob = NULL;
        ms_pfnD3D10StateBlockMaskDifference = NULL;
        ms_pfnD3D10StateBlockMaskDisableAll = NULL;
        ms_pfnD3D10StateBlockMaskDisableCapture = NULL;
        ms_pfnD3D10StateBlockMaskEnableAll = NULL;
        ms_pfnD3D10StateBlockMaskEnableCapture = NULL;
        ms_pfnD3D10StateBlockMaskGetSetting = NULL;
        ms_pfnD3D10StateBlockMaskIntersect = NULL;
        ms_pfnD3D10StateBlockMaskUnion = NULL;
        ms_pfnD3D10CompileShader = NULL;
        ms_pfnD3D10DisassembleShader = NULL;
        ms_pfnD3D10GetGeometryShaderProfile = NULL;
        ms_pfnD3D10GetPixelShaderProfile = NULL;
        ms_pfnD3D10GetVertexShaderProfile = NULL;
        ms_pfnD3D10PreprocessShader = NULL;
        ms_pfnD3D10ReflectShader = NULL;
        ms_pfnD3D10CompileEffectFromMemory = NULL;
        ms_pfnD3D10CreateEffectFromMemory = NULL;
        ms_pfnD3D10CreateEffectPoolFromMemory = NULL;
        ms_pfnD3D10DisassembleEffect = NULL;
    }

    ms_kD3D10LibraryCriticalSection.Unlock();

    return true;
}
//---------------------------------------------------------------------------
bool NiD3D10Renderer::CreateDevice(CreationParameters& kCreate)
{
    m_kInitialCreateParameters = kCreate;

    NiD3D10SystemDesc::GetSystemDesc(m_spSystemDesc);
    if (m_spSystemDesc->IsEnumerationValid() == false)
    {
        if (m_spSystemDesc->Enumerate() == false)
        {
            NiD3D10Error::ReportWarning("System enumeration failed; "
                "device cannot be created.");
            return false;
        }
    }

    unsigned int uiAdapterID = kCreate.m_uiAdapterIndex;
    if (uiAdapterID == NULL_ADAPTER)
        kCreate.m_eDriverType = DRIVER_REFERENCE;
    else if (uiAdapterID >= m_spSystemDesc->GetAdapterCount())
        uiAdapterID = 0;

    IDXGIAdapter* pkAdapter = NULL;
    if (kCreate.m_eDriverType== DRIVER_REFERENCE)
    {
        uiAdapterID = NULL_ADAPTER;
    }
    else
    {
        const NiD3D10AdapterDesc* pkAdapterDesc = 
            m_spSystemDesc->GetAdapterDesc(uiAdapterID);
        NIASSERT(pkAdapterDesc);
        pkAdapter = pkAdapterDesc->GetAdapter();
        NIASSERT(pkAdapter);
    }

    ID3D10Device* pkD3DDevice = NULL;
    HRESULT hr = S_OK;
    ms_kD3D10LibraryCriticalSection.Lock();
    if (EnsureD3D10Loaded())
    {
        hr = D3D10CreateDevice(pkAdapter, 
            NiD3D10Renderer::GetD3D10DriverType(kCreate.m_eDriverType), 
            NULL, kCreate.m_uiCreateFlags, D3D10_SDK_VERSION, &pkD3DDevice);
    }
    else
    {
        NiD3D10Error::ReportWarning("D3D10 library failed loading; "
            "device cannot be created.");
        return false;
    }
    ms_kD3D10LibraryCriticalSection.Unlock();

    if (FAILED(hr) || pkD3DDevice == NULL)
    {
        if (FAILED(hr))
        {
            NiD3D10Error::ReportError(
                NiD3D10Error::NID3D10ERROR_DEVICE_CREATION_FAILED,
                "Error HRESULT = 0x%08X.", (unsigned int)hr);
        }
        else
        {
            NiD3D10Error::ReportError(
                NiD3D10Error::NID3D10ERROR_DEVICE_CREATION_FAILED,
                "No error message from D3D10, but device is NULL.");
        }

        if (pkD3DDevice)
            pkD3DDevice->Release();

        return false;
    }

    m_pkD3D10Device = pkD3DDevice;
    m_uiAdapterIndex = uiAdapterID;

    StoreDeviceSettings();

    return true;
}
//---------------------------------------------------------------------------
void NiD3D10Renderer::StoreDeviceSettings()
{
    NIASSERT(m_pkD3D10Device);
    // (DXGI_FORMAT)0 == DXGI_FORMAT_UNKNOWN
    unsigned int* puiIterator = &(m_auiFormatSupport[1]);
    for (unsigned int i = 1; i < DXGI_FORMAT_COUNT; i++)
    {
        DXGI_FORMAT eFormat = (DXGI_FORMAT)i;
        // These formats are deprecated.
        if (eFormat == DXGI_FORMAT_B5G6R5_UNORM ||
            eFormat == DXGI_FORMAT_B5G5R5A1_UNORM ||
            eFormat == DXGI_FORMAT_B8G8R8A8_UNORM ||
            eFormat == DXGI_FORMAT_B8G8R8X8_UNORM)
        {
            continue;
        }

        HRESULT hr = m_pkD3D10Device->CheckFormatSupport(eFormat, puiIterator);
        if (FAILED(hr))
        {
            NiD3D10Error::ReportWarning("ID3D10Device::CheckFormatSupport "
                "failed on format %s; error HRESULT = 0x%08X.", 
                NiD3D10PixelFormat::GetFormatName(eFormat, false),
                (unsigned int)hr);
        }
        puiIterator++;
    }

    // Set replacement texture format
    // This is D3D10 - it's safe to assume that R8G8B8A8 is supported.
    NIASSERT((m_auiFormatSupport[DXGI_FORMAT_R8G8B8A8_UNORM] & 
        D3D10_FORMAT_SUPPORT_TEXTURE2D) != 0);
    NiTexture::RendererData::SetTextureReplacementFormat(
        NiD3D10PixelFormat::NI_FORMAT_R8G8B8A8_UNORM);
}
//---------------------------------------------------------------------------
void NiD3D10Renderer::CreateManagers()
{
    m_pkDeviceState = NiNew NiD3D10DeviceState(m_pkD3D10Device);
    m_pkGeometryPacker = NiNew NiD3D10GeometryPacker();
    m_pkRenderStateManager = NiNew NiD3D10RenderStateManager(m_pkD3D10Device,
        m_pkDeviceState);
    m_pkResourceManager = NiNew NiD3D10ResourceManager(m_pkD3D10Device);
    m_pkShaderConstantManager = 
        NiNew NiD3D10ShaderConstantManager(m_pkDeviceState);
}
//---------------------------------------------------------------------------
void NiD3D10Renderer::ReleaseManagers()
{
    NiDelete m_pkDeviceState;
    NiDelete m_pkGeometryPacker;
    NiDelete m_pkRenderStateManager;
    NiDelete m_pkResourceManager;
    NiDelete m_pkShaderConstantManager;
}
//---------------------------------------------------------------------------
unsigned int NiD3D10Renderer::AddOccludedNotificationFunc(
    OCCLUDEDNOTIFYFUNC pfnNotify, void* pvData)
{
    unsigned int uiReturn = m_kOccludedNotifyFuncs.Add(pfnNotify);
    m_kOccludedNotifyFuncData.SetAtGrow(uiReturn, pvData);

    return uiReturn;
}
//---------------------------------------------------------------------------
bool NiD3D10Renderer::RemoveOccludedNotificationFunc(
    OCCLUDEDNOTIFYFUNC pfnNotify)
{
    unsigned int uiIndex = FindOccludedNotificationFunc(pfnNotify);
    if (uiIndex == UINT_MAX)
        return false;

    m_kOccludedNotifyFuncs.RemoveAt(uiIndex);
    m_kOccludedNotifyFuncData.RemoveAt(uiIndex);
    return true;
}
//---------------------------------------------------------------------------
bool NiD3D10Renderer::RemoveOccludedNotificationFunc(unsigned int uiIndex)
{
    if (m_kOccludedNotifyFuncs.GetAt(uiIndex) == 0)
        return false;

    m_kOccludedNotifyFuncs.RemoveAt(uiIndex);
    m_kOccludedNotifyFuncData.RemoveAt(uiIndex);
    return true;
}
//---------------------------------------------------------------------------
void NiD3D10Renderer::RemoveAllOccludedNotificationFuncs()
{
    m_kOccludedNotifyFuncs.RemoveAll();
    m_kOccludedNotifyFuncData.RemoveAll();
}
//---------------------------------------------------------------------------
bool NiD3D10Renderer::ChangeOccludedNotificationFuncData(
    OCCLUDEDNOTIFYFUNC pfnNotify, void* pvData)
{
    unsigned int uiIndex = FindOccludedNotificationFunc(pfnNotify);
    if (uiIndex == UINT_MAX)
        return false;

    NIASSERT(m_kOccludedNotifyFuncData.GetSize() > uiIndex);
    m_kOccludedNotifyFuncData.SetAt(uiIndex, pvData);
    return true;
}
//---------------------------------------------------------------------------
bool NiD3D10Renderer::ChangeOccludedNotificationFuncData(unsigned int uiIndex, 
    void* pvData)
{
    if (m_kOccludedNotifyFuncs.GetAt(uiIndex) == 0)
        return false;

    NIASSERT(m_kOccludedNotifyFuncData.GetSize() > uiIndex);
    m_kOccludedNotifyFuncData.SetAt(uiIndex, pvData);
    return true;
}
//---------------------------------------------------------------------------
unsigned int NiD3D10Renderer::GetOccludedNotificationFuncCount() const
{
    return m_kOccludedNotifyFuncs.GetEffectiveSize();
}
//---------------------------------------------------------------------------
unsigned int NiD3D10Renderer::GetOccludedNotificationFuncArrayCount() const
{
    return m_kOccludedNotifyFuncs.GetSize();
}
//---------------------------------------------------------------------------
unsigned int NiD3D10Renderer::FindOccludedNotificationFunc(
    OCCLUDEDNOTIFYFUNC pfnNotify) const
{
    for (unsigned int i = 0; i < m_kOccludedNotifyFuncs.GetSize(); i++)
    {
        if (m_kOccludedNotifyFuncs.GetAt(i) == pfnNotify)
            return i;
    }
    return UINT_MAX;
}
//---------------------------------------------------------------------------
NiD3D10Renderer::OCCLUDEDNOTIFYFUNC 
    NiD3D10Renderer::GetOccludedNotificationFunc(unsigned int uiIndex) const
{
    return m_kOccludedNotifyFuncs.GetAt(uiIndex);
}
//---------------------------------------------------------------------------
unsigned int NiD3D10Renderer::AddDeviceRemovedNotificationFunc(
    DEVICEREMOVEDNOTIFYFUNC pfnNotify, void* pvData)
{
    unsigned int uiReturn = m_kDeviceRemovedNotifyFuncs.Add(pfnNotify);
    m_kDeviceRemovedNotifyFuncData.SetAtGrow(uiReturn, pvData);

    return uiReturn;
}
//---------------------------------------------------------------------------
bool NiD3D10Renderer::RemoveDeviceRemovedNotificationFunc(
    DEVICEREMOVEDNOTIFYFUNC pfnNotify)
{
    unsigned int uiIndex = FindDeviceRemovedNotificationFunc(pfnNotify);
    if (uiIndex == UINT_MAX)
        return false;

    m_kDeviceRemovedNotifyFuncs.RemoveAt(uiIndex);
    m_kDeviceRemovedNotifyFuncData.RemoveAt(uiIndex);
    return true;
}
//---------------------------------------------------------------------------
bool NiD3D10Renderer::RemoveDeviceRemovedNotificationFunc(
    unsigned int uiIndex)
{
    if (m_kDeviceRemovedNotifyFuncs.GetAt(uiIndex) == 0)
        return false;

    m_kDeviceRemovedNotifyFuncs.RemoveAt(uiIndex);
    m_kDeviceRemovedNotifyFuncData.RemoveAt(uiIndex);
    return true;
}
//---------------------------------------------------------------------------
void NiD3D10Renderer::RemoveAllDeviceRemovedNotificationFuncs()
{
    m_kDeviceRemovedNotifyFuncs.RemoveAll();
    m_kDeviceRemovedNotifyFuncData.RemoveAll();
}
//---------------------------------------------------------------------------
bool NiD3D10Renderer::ChangeDeviceRemovedNotificationFuncData(
    DEVICEREMOVEDNOTIFYFUNC pfnNotify, void* pvData)
{
    unsigned int uiIndex = FindDeviceRemovedNotificationFunc(pfnNotify);
    if (uiIndex == UINT_MAX)
        return false;

    NIASSERT(m_kDeviceRemovedNotifyFuncData.GetSize() > uiIndex);
    m_kDeviceRemovedNotifyFuncData.SetAt(uiIndex, pvData);
    return true;
}
//---------------------------------------------------------------------------
bool NiD3D10Renderer::ChangeDeviceRemovedNotificationFuncData(
    unsigned int uiIndex, void* pvData)
{
    if (m_kDeviceRemovedNotifyFuncs.GetAt(uiIndex) == 0)
        return false;

    NIASSERT(m_kDeviceRemovedNotifyFuncData.GetSize() > uiIndex);
    m_kDeviceRemovedNotifyFuncData.SetAt(uiIndex, pvData);
    return true;
}
//---------------------------------------------------------------------------
unsigned int NiD3D10Renderer::GetDeviceRemovedNotificationFuncCount() const
{
    return m_kDeviceRemovedNotifyFuncs.GetEffectiveSize();
}
//---------------------------------------------------------------------------
unsigned int NiD3D10Renderer::GetDeviceRemovedNotificationFuncArrayCount() 
    const
{
    return m_kDeviceRemovedNotifyFuncs.GetSize();
}
//---------------------------------------------------------------------------
unsigned int NiD3D10Renderer::FindDeviceRemovedNotificationFunc(
    DEVICEREMOVEDNOTIFYFUNC pfnNotify) const
{
    for (unsigned int i = 0; i < m_kDeviceRemovedNotifyFuncs.GetSize(); i++)
    {
        if (m_kDeviceRemovedNotifyFuncs.GetAt(i) == pfnNotify)
            return i;
    }
    return UINT_MAX;
}
//---------------------------------------------------------------------------
NiD3D10Renderer::DEVICEREMOVEDNOTIFYFUNC 
    NiD3D10Renderer::GetDeviceRemovedNotificationFunc(unsigned int uiIndex) 
    const
{
    return m_kDeviceRemovedNotifyFuncs.GetAt(uiIndex);
}//---------------------------------------------------------------------------
bool NiD3D10Renderer::DoesFormatSupportFlag(DXGI_FORMAT eFormat, 
    unsigned int uiFlags) const
{
    return (m_auiFormatSupport[(unsigned int)eFormat] & uiFlags) != 0;
}
//---------------------------------------------------------------------------
unsigned int NiD3D10Renderer::GetFormatSupportFlags(DXGI_FORMAT eFormat) const
{
    return m_auiFormatSupport[(unsigned int)eFormat];
}
//---------------------------------------------------------------------------
unsigned int NiD3D10Renderer::GetSyncInterval() const
{
    return m_uiSyncInterval;
}
//---------------------------------------------------------------------------
void NiD3D10Renderer::SetSyncInterval(unsigned int uiSyncInterval)
{
    m_uiSyncInterval = uiSyncInterval;
}
//---------------------------------------------------------------------------
NiD3D10DeviceState* NiD3D10Renderer::GetDeviceState() const
{
    return m_pkDeviceState;
}
//---------------------------------------------------------------------------
NiD3D10GeometryPacker* NiD3D10Renderer::GetGeometryPacker() const
{
    return m_pkGeometryPacker;
}
//---------------------------------------------------------------------------
NiD3D10RenderStateManager* NiD3D10Renderer::GetRenderStateManager() const
{
    return m_pkRenderStateManager;
}
//---------------------------------------------------------------------------
NiD3D10ResourceManager* NiD3D10Renderer::GetResourceManager() const
{
    return m_pkResourceManager;
}
//---------------------------------------------------------------------------
NiD3D10ShaderConstantManager* NiD3D10Renderer::GetShaderConstantManager() const
{
    return m_pkShaderConstantManager;
}
//---------------------------------------------------------------------------
IDXGISwapChain* NiD3D10Renderer::CreateSwapChain(
    DXGI_SWAP_CHAIN_DESC& kSwapChainDesc, unsigned int uiOutputIndex)
{
    if (m_pkD3D10Device == NULL)
    {
        NiD3D10Error::ReportWarning("Can't call CreateSwapChain without "
            "a valid device.");
        return NULL;
    }
    NIASSERT(m_spSystemDesc);

    NIASSERT(m_uiAdapterIndex == NULL_ADAPTER ||
        m_uiAdapterIndex < m_spSystemDesc->GetAdapterCount());

    DXGI_SWAP_CHAIN_DESC kActualSwapChainDesc = kSwapChainDesc;
    if (m_uiAdapterIndex != NULL_ADAPTER)
    {
        const NiD3D10AdapterDesc* pkAdapterDesc = 
            m_spSystemDesc->GetAdapterDesc(m_uiAdapterIndex);
        NIASSERT(pkAdapterDesc);

        unsigned int uiOutputID = uiOutputIndex;
        if (uiOutputID >= pkAdapterDesc->GetOutputCount())
            uiOutputID = 0;
        const NiD3D10OutputDesc* pkOutputDesc = 
            pkAdapterDesc->GetOutputDesc(uiOutputID);
        NIASSERT(pkOutputDesc);
        IDXGIOutput* pkOutput = pkOutputDesc->GetOutput();
        NIASSERT(pkOutput);

        pkOutput->FindClosestMatchingMode(&(kSwapChainDesc.BufferDesc), 
            &(kActualSwapChainDesc.BufferDesc), m_pkD3D10Device);
    }

    IDXGIFactory* pkFactory = m_spSystemDesc->GetFactory();
    NIASSERT(pkFactory);

    IDXGISwapChain* pkSwapChain = NULL;
    HRESULT hr = pkFactory->CreateSwapChain(m_pkD3D10Device, 
        &kActualSwapChainDesc, &pkSwapChain);

    if (FAILED(hr) || pkSwapChain == NULL)
    {
        if (FAILED(hr))
        {
            NiD3D10Error::ReportError(
                NiD3D10Error::NID3D10ERROR_SWAP_CHAIN_CREATION_FAILED,
                "Error HRESULT = 0x%08X.", (unsigned int)hr);
        }
        else
        {
            NiD3D10Error::ReportError(
                NiD3D10Error::NID3D10ERROR_SWAP_CHAIN_CREATION_FAILED,
                "No error message from D3D10, but swap chain is NULL.");
        }

        if (pkSwapChain)
            pkSwapChain->Release();

        return NULL;
    }

    return pkSwapChain;
}
//---------------------------------------------------------------------------
NiRenderTargetGroup* NiD3D10Renderer::CreateRenderTargetGroupFromSwapChain(
    IDXGISwapChain* pkSwapChain, bool bCreateDepthStencilBuffer,
    DXGI_FORMAT eDepthStencilFormat)
{
    if (m_pkD3D10Device == NULL)
    {
        NiD3D10Error::ReportWarning("Can't call "
            "CreateRenderTargetGroupFromSwapChain without a valid device.");
        return NULL;
    }

    if (pkSwapChain == NULL)
    {
        NiD3D10Error::ReportWarning("Can't call "
            "CreateRenderTargetGroupFromSwapChain without a valid "
            "swap chain.");
        return NULL;
    }

    Ni2DBuffer* pkBackBuffer = NULL;
    NiD3D10SwapChainBufferData* pkSwapChainBufferData =
        NiD3D10SwapChainBufferData::Create(pkSwapChain, pkBackBuffer);
    Ni2DBufferPtr spBackBuffer = pkBackBuffer;

    NiDepthStencilBufferPtr spDepthBuffer;
    if (bCreateDepthStencilBuffer)
    {
        if (DoesFormatSupportFlag(eDepthStencilFormat,
            D3D10_FORMAT_SUPPORT_DEPTH_STENCIL))
        {
            const NiPixelFormat* pkFormat = 
                NiD3D10PixelFormat::ObtainFromDXGIFormat(eDepthStencilFormat);
            NIASSERT(pkFormat);
            spDepthBuffer = NiDepthStencilBuffer::Create(
                pkBackBuffer->GetWidth(), 
                pkBackBuffer->GetHeight(),
                this, *pkFormat,
                pkBackBuffer->GetMSAAPref());
        }
        else
        {
            spDepthBuffer = NiDepthStencilBuffer::CreateCompatible(
                pkBackBuffer, this);
            if (spDepthBuffer == NULL)
            {
                NiD3D10Error::ReportWarning("Compatible depth/stencil buffer "
                    "could not be created; render target group will have "
                    "no depth/stencil buffer.");
            }
        }
    }

    NiRenderTargetGroup* pkRTGroup = NiRenderTargetGroup::Create(pkBackBuffer,
        this, spDepthBuffer);

    return pkRTGroup;
}
//---------------------------------------------------------------------------
bool NiD3D10Renderer::ReleaseDevice()
{
    if (m_pkD3D10Device)
    {
        m_pkD3D10Device->Release();
        m_pkD3D10Device = NULL;
    }
    return true;
}
//---------------------------------------------------------------------------
ID3D10Device* NiD3D10Renderer::CreateTempDevice(IDXGIAdapter* pkAdapter, 
    D3D10_DRIVER_TYPE eType, unsigned int uiFlags)
{
    ID3D10Device* pkDevice = NULL;
    ms_kD3D10LibraryCriticalSection.Lock();
    if (EnsureD3D10Loaded())
    {
        if (eType == D3D10_DRIVER_TYPE_REFERENCE)
            pkAdapter = NULL;
        HRESULT hr = D3D10CreateDevice(pkAdapter, eType, NULL, uiFlags, 
            D3D10_SDK_VERSION, &pkDevice);
        if (FAILED(hr) || pkDevice == NULL)
        {
            if (FAILED(hr))
            {
                NiD3D10Error::ReportError(
                    NiD3D10Error::NID3D10ERROR_DEVICE_CREATION_FAILED,
                    "Error HRESULT = 0x%08X.", (unsigned int)hr);
            }
            else
            {
                NiD3D10Error::ReportError(
                    NiD3D10Error::NID3D10ERROR_DEVICE_CREATION_FAILED,
                    "No error message from D3D10, but device is NULL.");
            }

            if (pkDevice)
                pkDevice->Release();
        }
    }
    else
    {
        NiD3D10Error::ReportWarning("D3D10 library failed to be loaded; "
            "device cannot be created.");
    }
    ms_kD3D10LibraryCriticalSection.Unlock();
    return pkDevice;
}
//---------------------------------------------------------------------------
ID3D10Device* NiD3D10Renderer::GetD3D10Device() const
{
    return m_pkD3D10Device;
}
//---------------------------------------------------------------------------
bool NiD3D10Renderer::ReleaseResources()
{
    m_spDefaultRenderTargetGroup = NULL;
    return true;
}
//---------------------------------------------------------------------------
const char* NiD3D10Renderer::GetDriverInfo() const
{
    NIASSERT("Not implemented!" && false);
    return NULL;
}
//---------------------------------------------------------------------------
unsigned int NiD3D10Renderer::GetFlags() const
{
    return CAPS_HARDWARESKINNING |
        CAPS_NONPOW2_TEXT |
        CAPS_AA_RENDERED_TEXTURES;
}
//---------------------------------------------------------------------------
void NiD3D10Renderer::SetDepthClear(const float fZClear)
{
    m_fDepthClear = fZClear;
}
//---------------------------------------------------------------------------
float NiD3D10Renderer::GetDepthClear() const
{
    return m_fDepthClear;
}
//---------------------------------------------------------------------------
void NiD3D10Renderer::SetBackgroundColor(const NiColor& kColor)
{
    m_afBackgroundColor[0] = kColor.r;
    m_afBackgroundColor[1] = kColor.g;
    m_afBackgroundColor[2] = kColor.b;
    m_afBackgroundColor[3] = 1.0f;
}
//---------------------------------------------------------------------------
void NiD3D10Renderer::SetBackgroundColor(const NiColorA& kColor)
{
    m_afBackgroundColor[0] = kColor.r;
    m_afBackgroundColor[1] = kColor.g;
    m_afBackgroundColor[2] = kColor.b;
    m_afBackgroundColor[3] = kColor.a;
}
//---------------------------------------------------------------------------
void NiD3D10Renderer::GetBackgroundColor(NiColorA& kColor) const
{
    kColor.r = m_afBackgroundColor[0];
    kColor.g = m_afBackgroundColor[1];
    kColor.b = m_afBackgroundColor[2];
    kColor.a = m_afBackgroundColor[3];
}
//---------------------------------------------------------------------------
void NiD3D10Renderer::SetStencilClear(unsigned int uiClear)
{
    m_ucStencilClear = (uiClear & 0x000000FF);
}
//---------------------------------------------------------------------------
unsigned int NiD3D10Renderer::GetStencilClear() const
{
    return m_ucStencilClear;
}
//---------------------------------------------------------------------------
bool NiD3D10Renderer::ValidateRenderTargetGroup(NiRenderTargetGroup* pkTarget)
{
    if (pkTarget == NULL)
        return false;

    D3D10_RTV_DIMENSION eResourceType = D3D10_RTV_DIMENSION_UNKNOWN;
    unsigned int uiElementOffset = UINT_MAX;
    unsigned int uiElementWidth = UINT_MAX;
    unsigned int uiMipSlice = UINT_MAX;
    unsigned int uiArraySize = UINT_MAX;
    unsigned int uiBufferWidth = UINT_MAX;
    unsigned int uiBufferHeight = UINT_MAX;

    Ni2DBuffer* pkFirstBuffer = NULL;

    for (unsigned int i = 0; i < D3D10_SIMULTANEOUS_RENDER_TARGET_COUNT; i++)
    {
        // Note that finding *no* render target in the render target group
        // is still valid.
        NiD3D102DBufferData* pk2DBufferData = 
            (NiD3D102DBufferData*)pkTarget->GetBufferRendererData(i);
        if (pk2DBufferData == NULL)
            continue;

        if (pkFirstBuffer == NULL)
            pkFirstBuffer = pkTarget->GetBuffer(i);

        NIASSERT(NiIsKindOf(NiD3D10RenderTargetBufferData, pk2DBufferData));

        NiD3D10RenderTargetBufferData* pkRTBufferData = 
            (NiD3D10RenderTargetBufferData*)pk2DBufferData;

        ID3D10RenderTargetView* pkRTView = 
            pkRTBufferData->GetRenderTargetView();

        if (pkRTView == NULL)
            continue;

        D3D10_RENDER_TARGET_VIEW_DESC kDesc;
        pkRTView->GetDesc(&kDesc);

        // Check for mismatched resource types
        if (eResourceType == D3D10_RTV_DIMENSION_UNKNOWN)
            eResourceType = kDesc.ViewDimension;

        NIASSERT(eResourceType != D3D10_RTV_DIMENSION_UNKNOWN);

        // Check for mismatched resource data
        if (eResourceType == D3D10_RTV_DIMENSION_BUFFER)
        {
            if (uiElementOffset == UINT_MAX)
            {
                NIASSERT(uiElementWidth == UINT_MAX);
                uiElementOffset = kDesc.Buffer.ElementOffset;
                uiElementWidth = kDesc.Buffer.ElementWidth;
            }
            else if (kDesc.ViewDimension != D3D10_RTV_DIMENSION_BUFFER ||
                uiElementOffset != kDesc.Buffer.ElementOffset ||
                uiElementWidth != kDesc.Buffer.ElementWidth)
            {
                return false;
            }
        }
        else if (eResourceType == D3D10_RTV_DIMENSION_TEXTURE1D)
        {
            unsigned int uiTempMipSlice = UINT_MAX;
            if (kDesc.ViewDimension == D3D10_RTV_DIMENSION_TEXTURE1D)
            {
                // OK
                uiTempMipSlice = kDesc.Texture1D.MipSlice;
            }
            else if (kDesc.ViewDimension == D3D10_RTV_DIMENSION_TEXTURE1DARRAY)
            {
                if (uiMipSlice == UINT_MAX ||
                    kDesc.Texture1DArray.ArraySize == 1)
                {
                    // OK
                }
                else
                {
                    return false;
                }
                uiTempMipSlice = kDesc.Texture1DArray.MipSlice;
            }
            else
            {
                return false;
            }

            if (uiMipSlice == UINT_MAX)
                uiMipSlice = uiTempMipSlice;
            else if (uiMipSlice != uiTempMipSlice)
                return false;

            NIASSERT(kDesc.Texture1D.MipSlice == 0);

            // For 1D resources, the height must be 1.
            NIASSERT(pk2DBufferData->GetHeight() == 1);
            if (uiBufferWidth == UINT_MAX)
            {
                uiBufferWidth = pk2DBufferData->GetWidth();
            }
            else if (pk2DBufferData->GetWidth() != uiBufferWidth)
            {
                return false;
            }
        }
        else if (eResourceType == D3D10_RTV_DIMENSION_TEXTURE1DARRAY)
        {
            unsigned int uiTempMipSlice = UINT_MAX;
            if (kDesc.ViewDimension == D3D10_RTV_DIMENSION_TEXTURE1D)
            {
                if (uiArraySize == UINT_MAX || uiArraySize == 1)
                {
                    // OK
                }
                else
                {
                    return false;
                }
                uiTempMipSlice = kDesc.Texture1D.MipSlice;
            }
            else if (kDesc.ViewDimension == D3D10_RTV_DIMENSION_TEXTURE1DARRAY)
            {
                if (uiArraySize == UINT_MAX ||
                    kDesc.Texture1DArray.ArraySize == uiArraySize)
                {
                    // OK
                }
                else
                {
                    return false;
                }
                uiTempMipSlice = kDesc.Texture1DArray.MipSlice;
            }
            else
            {
                return false;
            }

            if (uiMipSlice == UINT_MAX)
            {
                NIASSERT(uiArraySize == UINT_MAX);
                uiMipSlice = uiTempMipSlice;
                uiArraySize = kDesc.Texture1DArray.ArraySize;
            }
            else if (uiMipSlice != uiTempMipSlice)
            {
                return false;
            }

            NIASSERT(kDesc.Texture1DArray.MipSlice == 0);

            // For 1D resources, the height must be 1.
            NIASSERT(pk2DBufferData->GetHeight() == 1);
            if (uiBufferWidth == UINT_MAX)
            {
                uiBufferWidth = pk2DBufferData->GetWidth();
            }
            else if (pk2DBufferData->GetWidth() != uiBufferWidth)
            {
                return false;
            }
        }
        else if (eResourceType == D3D10_RTV_DIMENSION_TEXTURE2D)
        {
            unsigned int uiTempMipSlice = UINT_MAX;
            if (kDesc.ViewDimension == D3D10_RTV_DIMENSION_TEXTURE2D)
            {
                // OK
                uiTempMipSlice = kDesc.Texture2D.MipSlice;
            }
            else if (kDesc.ViewDimension == D3D10_RTV_DIMENSION_TEXTURE2DARRAY)
            {
                if (uiMipSlice == UINT_MAX ||
                    kDesc.Texture2DArray.ArraySize == 1)
                {
                    // OK
                }
                else
                {
                    return false;
                }
                uiTempMipSlice = kDesc.Texture2DArray.MipSlice;
            }
            else
            {
                return false;
            }

            if (uiMipSlice == UINT_MAX)
                uiMipSlice = uiTempMipSlice;
            else if (uiMipSlice != uiTempMipSlice)
                return false;

            NIASSERT(kDesc.Texture2D.MipSlice == 0);

            if (uiBufferWidth == UINT_MAX)
            {
                NIASSERT(uiBufferHeight == UINT_MAX);
                uiBufferWidth = pk2DBufferData->GetWidth();
                uiBufferHeight = pk2DBufferData->GetHeight();
            }
            else if (pk2DBufferData->GetWidth() != uiBufferWidth ||
                pk2DBufferData->GetHeight() != uiBufferHeight)
            {
                return false;
            }
        }
        else if (eResourceType == D3D10_RTV_DIMENSION_TEXTURE2DARRAY)
        {
            unsigned int uiTempMipSlice = UINT_MAX;
            if (kDesc.ViewDimension == D3D10_RTV_DIMENSION_TEXTURE2D)
            {
                if (uiArraySize == UINT_MAX || uiArraySize == 1)
                {
                    // OK
                }
                else
                {
                    return false;
                }
                uiTempMipSlice = kDesc.Texture2D.MipSlice;
            }
            else if (kDesc.ViewDimension == D3D10_RTV_DIMENSION_TEXTURE2DARRAY)
            {
                if (uiArraySize == UINT_MAX ||
                    kDesc.Texture2DArray.ArraySize == uiArraySize)
                {
                    // OK
                }
                else
                {
                    return false;
                }
                uiTempMipSlice = kDesc.Texture2DArray.MipSlice;
            }
            else
            {
                return false;
            }

            if (uiMipSlice == UINT_MAX)
            {
                NIASSERT(uiArraySize == UINT_MAX);
                uiMipSlice = uiTempMipSlice;
                uiArraySize = kDesc.Texture2DArray.ArraySize;
            }
            else if (uiMipSlice != uiTempMipSlice)
            {
                return false;
            }

            NIASSERT(kDesc.Texture2DArray.MipSlice == 0);

            if (uiBufferWidth == UINT_MAX)
            {
                NIASSERT(uiBufferHeight == UINT_MAX);
                uiBufferWidth = pk2DBufferData->GetWidth();
                uiBufferHeight = pk2DBufferData->GetHeight();
            }
            else if (pk2DBufferData->GetWidth() != uiBufferWidth ||
                pk2DBufferData->GetHeight() != uiBufferHeight)
            {
                return false;
            }
        }
        else if (eResourceType == D3D10_RTV_DIMENSION_TEXTURE2DMS)
        {
            if (kDesc.ViewDimension == D3D10_RTV_DIMENSION_TEXTURE2DMS)
            {
                // OK
            }
            else if (kDesc.ViewDimension == 
                D3D10_RTV_DIMENSION_TEXTURE2DMSARRAY)
            {
                if (kDesc.Texture2DMSArray.ArraySize == 1)
                {
                    // OK
                }
                else
                {
                    return false;
                }
            }
            else
            {
                return false;
            }

            // D3D10 doesn't support mipmapped multisampled textures.
            if (uiBufferWidth == UINT_MAX)
            {
                NIASSERT(uiBufferHeight == UINT_MAX);
                uiBufferWidth = pk2DBufferData->GetWidth();
                uiBufferHeight = pk2DBufferData->GetHeight();
            }
            else if (pk2DBufferData->GetWidth() != uiBufferWidth ||
                pk2DBufferData->GetHeight() != uiBufferHeight)
            {
                return false;
            }
        }
        else if (eResourceType == D3D10_RTV_DIMENSION_TEXTURE2DMSARRAY)
        {
            if (kDesc.ViewDimension == D3D10_RTV_DIMENSION_TEXTURE2DMS)
            {
                if (uiArraySize == UINT_MAX || uiArraySize == 1)
                {
                    // OK
                }
                else
                {
                    return false;
                }
            }
            else if (kDesc.ViewDimension == 
                D3D10_RTV_DIMENSION_TEXTURE2DMSARRAY)
            {
                if (uiArraySize == UINT_MAX ||
                    kDesc.Texture2DMSArray.ArraySize == uiArraySize)
                {
                    // OK
                }
                else
                {
                    return false;
                }
            }
            else
            {
                return false;
            }

            if (uiArraySize == UINT_MAX)
            {
                uiArraySize = kDesc.Texture2DArray.ArraySize;
            }

            // D3D10 doesn't support mipmapped multisampled textures.
            if (uiBufferWidth == UINT_MAX)
            {
                NIASSERT(uiBufferHeight == UINT_MAX);
                uiBufferWidth = pk2DBufferData->GetWidth();
                uiBufferHeight = pk2DBufferData->GetHeight();
            }
            else if (pk2DBufferData->GetWidth() != uiBufferWidth ||
                pk2DBufferData->GetHeight() != uiBufferHeight)
            {
                return false;
            }
        }
        else if (eResourceType == D3D10_RTV_DIMENSION_TEXTURE3D)
        {
            if (uiMipSlice == UINT_MAX)
            {
                NIASSERT(uiArraySize == UINT_MAX);
                uiMipSlice = kDesc.Texture3D.MipSlice;
                uiArraySize = kDesc.Texture3D.WSize;
            }
            else if (kDesc.ViewDimension != D3D10_RTV_DIMENSION_TEXTURE3D ||
                uiMipSlice != kDesc.Texture3D.MipSlice ||
                uiArraySize != kDesc.Texture3D.WSize)
            {
                return false;
            }

            NIASSERT(kDesc.Texture3D.MipSlice == 0);

            if (uiBufferWidth == UINT_MAX)
            {
                // Third dimension is checked in WSize above.
                NIASSERT(uiBufferHeight == UINT_MAX);
                uiBufferWidth = pk2DBufferData->GetWidth();
                uiBufferHeight = pk2DBufferData->GetHeight();
            }
            else if (pk2DBufferData->GetWidth() != uiBufferWidth ||
                pk2DBufferData->GetHeight() != uiBufferHeight)
            {
                return false;
            }

        }

        // Check for duplicate views
        for (unsigned int j = 0; j < i; j++)
        {
            NiD3D102DBufferData* pkOther2DBufferData = 
                (NiD3D102DBufferData*)pkTarget->GetBufferRendererData(j);
            if (pkOther2DBufferData == NULL)
                continue;

            NIASSERT(NiIsKindOf(NiD3D10RenderTargetBufferData, 
                pkOther2DBufferData));

            NiD3D10RenderTargetBufferData* pkOtherRTBufferData = 
                (NiD3D10RenderTargetBufferData*)pkOther2DBufferData;

            ID3D10RenderTargetView* pkOtherRTView = 
                pkOtherRTBufferData->GetRenderTargetView();

            if (pkOtherRTView == pkRTView)
                return false;
        }
    }

    // Check depth stencil buffer
    // Assume that the depth stencil buffer only needs to be tested against
    // one active color buffer.
    NiDepthStencilBuffer* pkDSBuffer = pkTarget->GetDepthStencilBuffer();
    if (pkDSBuffer)
    {
        if (IsDepthBufferCompatible(pkFirstBuffer, pkDSBuffer) == false)
            return false;
    }

    return true;
}
//---------------------------------------------------------------------------
bool NiD3D10Renderer::IsDepthBufferCompatible(Ni2DBuffer* pkBuffer, 
    NiDepthStencilBuffer* pkDSBuffer)
{
    // Note that finding *no* render target in the render target group
    // is still valid.
    if (pkBuffer == NULL || pkDSBuffer == NULL)
        return true;

    // Get 2D buffer description
    NiD3D102DBufferData* pk2DBufferData = 
        (NiD3D102DBufferData*)pkBuffer->GetRendererData();
    if (pk2DBufferData == NULL)
        return false;

    NIASSERT(NiIsKindOf(NiD3D10RenderTargetBufferData, pk2DBufferData));

    NiD3D10RenderTargetBufferData* pkRTBufferData = 
        (NiD3D10RenderTargetBufferData*)pk2DBufferData;

    ID3D10RenderTargetView* pkRTView = pkRTBufferData->GetRenderTargetView();

    if (pkRTView == NULL)
        return false;

    D3D10_RENDER_TARGET_VIEW_DESC kRTDesc;
    pkRTView->GetDesc(&kRTDesc);

    // Get depth/stencil buffer description
    pk2DBufferData = (NiD3D102DBufferData*)pkDSBuffer->GetRendererData();
    if (pk2DBufferData == NULL)
        return false;

    NIASSERT(NiIsKindOf(NiD3D10DepthStencilBufferData, pk2DBufferData));

    NiD3D10DepthStencilBufferData* pkDSBufferData = 
        (NiD3D10DepthStencilBufferData*)pk2DBufferData;

    ID3D10DepthStencilView* pkDSView = pkDSBufferData->GetDepthStencilView();

    if (pkDSView == NULL)
        return false;

    D3D10_DEPTH_STENCIL_VIEW_DESC kDSDesc;
    pkDSView->GetDesc(&kDSDesc);

    // Check for mismatched resource types and data.
    // Remember that an array of 1 texture is the same as just that texture.
    if (kRTDesc.ViewDimension == D3D10_RTV_DIMENSION_TEXTURE1D)
    {
        unsigned int uiDSMipSlice = UINT_MAX;
        if (kDSDesc.ViewDimension == D3D10_DSV_DIMENSION_TEXTURE1D)
        {
            // OK
            uiDSMipSlice = kDSDesc.Texture1D.MipSlice;
        }
        else if (kDSDesc.ViewDimension == D3D10_DSV_DIMENSION_TEXTURE1DARRAY)
        {
            if (kDSDesc.Texture1DArray.ArraySize == 1)
            {
                // OK
            }
            else
            {
                return false;
            }
            uiDSMipSlice = kDSDesc.Texture1DArray.MipSlice;
        }
        else
        {
            return false;
        }
        NIASSERT(kRTDesc.Texture1D.MipSlice == 0 && uiDSMipSlice == 0);

        // For 1D resources, the height must be 1.
        NIASSERT(pkRTBufferData->GetHeight() == 1 &&
            pkDSBufferData->GetHeight() == 1);
        if (pkRTBufferData->GetWidth() > pkDSBufferData->GetWidth())
            return false;
    }
    else if (kRTDesc.ViewDimension == D3D10_RTV_DIMENSION_TEXTURE1DARRAY)
    {
        unsigned int uiDSMipSlice = UINT_MAX;
        if (kDSDesc.ViewDimension == D3D10_DSV_DIMENSION_TEXTURE1D)
        {
            if (kRTDesc.Texture1DArray.ArraySize == 1)
            {
                // OK
            }
            else
            {
                return false;
            }
            uiDSMipSlice = kDSDesc.Texture1D.MipSlice;
        }
        else if (kDSDesc.ViewDimension == D3D10_DSV_DIMENSION_TEXTURE1DARRAY)
        {
            if (kDSDesc.Texture1DArray.ArraySize == 
                kRTDesc.Texture1DArray.ArraySize)
            {
                // OK
            }
            else
            {
                return false;
            }
            uiDSMipSlice = kDSDesc.Texture1DArray.MipSlice;
        }
        else
        {
            return false;
        }

        NIASSERT(kRTDesc.Texture1D.MipSlice == 0 && uiDSMipSlice == 0);

        // For 1D resources, the height must be 1.
        NIASSERT(pkRTBufferData->GetHeight() == 1 &&
            pkDSBufferData->GetHeight() == 1);
        if (pkRTBufferData->GetWidth() > pkDSBufferData->GetWidth())
            return false;
    }
    else if (kRTDesc.ViewDimension == D3D10_RTV_DIMENSION_TEXTURE2D)
    {
        unsigned int uiDSMipSlice = UINT_MAX;
        if (kDSDesc.ViewDimension == D3D10_DSV_DIMENSION_TEXTURE2D)
        {
            // OK
            uiDSMipSlice = kDSDesc.Texture2D.MipSlice;
        }
        else if (kDSDesc.ViewDimension == D3D10_DSV_DIMENSION_TEXTURE2DARRAY)
        {
            if (kDSDesc.Texture2DArray.ArraySize == 1)
            {
                // OK
            }
            else
            {
                return false;
            }
            uiDSMipSlice = kDSDesc.Texture2DArray.MipSlice;
        }
        else
        {
            return false;
        }
        NIASSERT(kRTDesc.Texture2D.MipSlice == 0 && uiDSMipSlice == 0);

        if (pkRTBufferData->GetWidth() > pkDSBufferData->GetWidth() ||
            pkRTBufferData->GetHeight() > pkDSBufferData->GetHeight())
        {
            return false;
        }
    }
    else if (kRTDesc.ViewDimension == D3D10_RTV_DIMENSION_TEXTURE2DARRAY)
    {
        unsigned int uiDSMipSlice = UINT_MAX;
        if (kDSDesc.ViewDimension == D3D10_DSV_DIMENSION_TEXTURE2D)
        {
            if (kRTDesc.Texture2DArray.ArraySize == 1)
            {
                // OK
            }
            else
            {
                return false;
            }
            uiDSMipSlice = kDSDesc.Texture2D.MipSlice;
        }
        else if (kDSDesc.ViewDimension == D3D10_DSV_DIMENSION_TEXTURE2DARRAY)
        {
            if (kDSDesc.Texture2DArray.ArraySize == 
                kRTDesc.Texture2DArray.ArraySize)
            {
                // OK
            }
            else
            {
                return false;
            }
            uiDSMipSlice = kDSDesc.Texture2DArray.MipSlice;
        }
        else
        {
            return false;
        }

        NIASSERT(kRTDesc.Texture2D.MipSlice == 0 && uiDSMipSlice == 0);

        if (pkRTBufferData->GetWidth() > pkDSBufferData->GetWidth() ||
            pkRTBufferData->GetHeight() > pkDSBufferData->GetHeight())
        {
            return false;
        }
    }
    else if (kRTDesc.ViewDimension == D3D10_RTV_DIMENSION_TEXTURE2DMS)
    {

        if (kDSDesc.ViewDimension == D3D10_DSV_DIMENSION_TEXTURE2DMS)
        {
            // OK
        }
        else if (kDSDesc.ViewDimension == D3D10_DSV_DIMENSION_TEXTURE2DMSARRAY)
        {
            if (kDSDesc.Texture2DMSArray.ArraySize == 1)
            {
                // OK
            }
            else
            {
                return false;
            }
        }
        else
        {
            return false;
        }

        // D3D10 doesn't support mipmapped multisampled textures.
        if (pkRTBufferData->GetWidth() > pkDSBufferData->GetWidth() ||
            pkRTBufferData->GetHeight() > pkDSBufferData->GetHeight())
        {
            return false;
        }
    }
    else if (kRTDesc.ViewDimension == D3D10_RTV_DIMENSION_TEXTURE2DMSARRAY)
    {
        if (kDSDesc.ViewDimension == D3D10_DSV_DIMENSION_TEXTURE2DMS)
        {
            if (kRTDesc.Texture2DMSArray.ArraySize == 1)
            {
                // OK
            }
            else
            {
                return false;
            }
        }
        else if (kDSDesc.ViewDimension == D3D10_DSV_DIMENSION_TEXTURE2DMSARRAY)
        {
            if (kDSDesc.Texture2DMSArray.ArraySize == 
                kRTDesc.Texture2DMSArray.ArraySize)
            {
                // OK
            }
            else
            {
                return false;
            }
        }
        else
        {
            return false;
        }

        // D3D10 doesn't support mipmapped multisampled textures.
        if (pkRTBufferData->GetWidth() > pkDSBufferData->GetWidth() ||
            pkRTBufferData->GetHeight() > pkDSBufferData->GetHeight())
        {
            return false;
        }
    }
    else if (kRTDesc.ViewDimension == D3D10_RTV_DIMENSION_BUFFER ||
        kRTDesc.ViewDimension == D3D10_RTV_DIMENSION_TEXTURE3D)
    {
        // Any other resource types other than those listed here are
        // not valid for the depth/stencil buffer, though 
        // D3D10_RTV_DIMENSION_UNKNOWN indicates that no render target
        // exists, so the depth/stencil buffer can be whatever it
        // wants to be.
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
NiRenderTargetGroup* NiD3D10Renderer::GetDefaultRenderTargetGroup() const
{
    return m_spDefaultRenderTargetGroup;
}
//---------------------------------------------------------------------------
const NiRenderTargetGroup* NiD3D10Renderer::GetCurrentRenderTargetGroup() const
{
    return m_pkCurrentRenderTargetGroup;
}
//---------------------------------------------------------------------------
NiDepthStencilBuffer* NiD3D10Renderer::GetDefaultDepthStencilBuffer() const
{
    if (m_spDefaultRenderTargetGroup)
        return m_spDefaultRenderTargetGroup->GetDepthStencilBuffer();
    return NULL;
}
//---------------------------------------------------------------------------
Ni2DBuffer* NiD3D10Renderer::GetDefaultBackBuffer() const
{
    if (m_spDefaultRenderTargetGroup)
        return m_spDefaultRenderTargetGroup->GetBuffer(0);
    return NULL;
}
//---------------------------------------------------------------------------
const NiPixelFormat* NiD3D10Renderer::FindClosestPixelFormat(
    NiTexture::FormatPrefs& kFmtPrefs) const
{
    NIASSERT("Not implemented!" && false);
    return NULL;
}
//---------------------------------------------------------------------------
const NiPixelFormat* NiD3D10Renderer::FindClosestDepthStencilFormat(
    const NiPixelFormat* pkFrontBufferFormat, unsigned int uiDepthBPP,
    unsigned int uiStencilBPP) const
{
    // Depth stencil format is independent of front buffer format;
    NiPixelFormat* apkFormats[] = 
    {
        &NiD3D10PixelFormat::NI_FORMAT_D32_FLOAT_S8X24_UINT,
        &NiD3D10PixelFormat::NI_FORMAT_D32_FLOAT,
        &NiD3D10PixelFormat::NI_FORMAT_D24_UNORM_S8_UINT,
        &NiD3D10PixelFormat::NI_FORMAT_D16_UNORM
    };
    const unsigned int uiFormatCount = 
        sizeof(apkFormats) / sizeof(apkFormats[0]);

    // Array for order of attempting various formats
    unsigned int auiFormatPreference[uiFormatCount];
    unsigned int i = 0;
    for (; i < uiFormatCount; i++)
        auiFormatPreference[i] = i;

    if (uiStencilBPP > 0)
    {
        if (uiDepthBPP <= 24)
        {
            auiFormatPreference[0] = 2;
            auiFormatPreference[1] = 0;
            auiFormatPreference[2] = 3;
            auiFormatPreference[3] = 1;
        }
        else
        {
            auiFormatPreference[0] = 0;
            auiFormatPreference[1] = 2;
            auiFormatPreference[2] = 1;
            auiFormatPreference[3] = 3;
        }
    }
    else
    {
        if (uiDepthBPP <= 16)
        {
            auiFormatPreference[0] = 3;
            auiFormatPreference[1] = 2;
            auiFormatPreference[2] = 1;
            auiFormatPreference[3] = 0;
        }
        else if (uiDepthBPP <= 24)
        {
            auiFormatPreference[0] = 2;
            auiFormatPreference[1] = 1;
            auiFormatPreference[2] = 0;
            auiFormatPreference[3] = 3;
        }
        else
        {
            auiFormatPreference[0] = 1;
            auiFormatPreference[1] = 0;
            auiFormatPreference[2] = 2;
            auiFormatPreference[3] = 3;
        }
    }
    
    for (i = 0; i < uiFormatCount; i++)
    {
        NiPixelFormat* pkFormat = apkFormats[auiFormatPreference[i]];
        if (DoesFormatSupportFlag((DXGI_FORMAT)pkFormat->GetRendererHint(), 
            D3D10_FORMAT_SUPPORT_DEPTH_STENCIL))
        {
            return pkFormat;
        }
    }
    return NULL;
}
//---------------------------------------------------------------------------
unsigned int NiD3D10Renderer::GetMaxBuffersPerRenderTargetGroup() const
{
    return D3D10_SIMULTANEOUS_RENDER_TARGET_COUNT;
}
//---------------------------------------------------------------------------
bool NiD3D10Renderer::GetIndependentBufferBitDepths() const
{
    return true;
}
//---------------------------------------------------------------------------
bool NiD3D10Renderer::PrecacheTexture(NiTexture* pkIm, bool bForceLoad, 
    bool bLocked)
{
    if (!pkIm)
        return false;

    LockSourceDataCriticalSection();

    NiD3D10TextureData* pkTexData = 
        (NiD3D10TextureData*)pkIm->GetRendererData();
    if (pkTexData == NULL)
    {
        NiSourceTexture* pkSourceTex = NiDynamicCast(NiSourceTexture, pkIm);
        if (pkSourceTex)
        {
            pkTexData = NiD3D10SourceTextureData::Create(pkSourceTex);
        }
        else
        {
            // All other texture types must have renderer data created
            // during texture construction. If we don't already have a 
            // texture data object by now, we never will.
        }
    }

    UnlockSourceDataCriticalSection();

    return (pkTexData != NULL);
}
//---------------------------------------------------------------------------
bool NiD3D10Renderer::SetMipmapSkipLevel(unsigned int uiSkip)
{
    NIASSERT("Not implemented!" && false);
    return false;
}
//---------------------------------------------------------------------------
unsigned int NiD3D10Renderer::GetMipmapSkipLevel() const
{
    NIASSERT("Not implemented!" && false);
    return 0;
}
//---------------------------------------------------------------------------
bool NiD3D10Renderer::PrecacheGeometry(NiGeometry* pkGeometry, 
    unsigned int uiBonesPerPartition, 
    unsigned int uiBonesPerVertex)
{
    // Get the geometry renderer data that corresponds to this skin instance
    NiD3D10GeometryData* pkD3D10GeomData = (NiD3D10GeometryData*)
        pkGeometry->GetModelData()->GetRendererData();
    NiSkinInstance* pkSkinInstance = pkGeometry->GetSkinInstance();
    while (pkD3D10GeomData != NULL && 
        pkD3D10GeomData->GetSkinInstance() != pkSkinInstance)
    {
        pkD3D10GeomData = pkD3D10GeomData->GetNext();
    }

    // If no geometry renderer data object exists, create one.
    if (pkD3D10GeomData == NULL)
    {
        pkD3D10GeomData = CreateGeometryRendererData(pkGeometry);

        NIASSERT(pkD3D10GeomData != NULL);
    }

    NIASSERT(pkD3D10GeomData != NULL && 
        pkD3D10GeomData->GetSkinInstance() == pkSkinInstance);

    NiD3D10VertexDescription* pkDesc = 
        (NiD3D10VertexDescription*)pkGeometry->GetShaderDecl();
    if (!pkDesc)
    {
        if (NiIsKindOf(NiParticles, pkGeometry))
        {
            // Temporary trick system into thinking normals, binormals, 
            // tangents, and one set of texture coordinates are present.
            // If needed, they will be automatically generated.
            unsigned int uiCurrentFlags = pkD3D10GeomData->GetFlags();
            bool bHasColors = 
                NiD3D10GeometryData::GetHasColorsFromFlags(uiCurrentFlags);
            unsigned int uiTempFlags = NiD3D10GeometryData::CreateVertexFlags(
                bHasColors, true, true, 1);
            pkD3D10GeomData->SetFlags(uiTempFlags);
            NIVERIFY(NiShaderDeclaration::CreateDeclForGeometry(pkGeometry));
            pkDesc = (NiD3D10VertexDescription*)pkGeometry->GetShaderDecl();

            pkD3D10GeomData->SetFlags(uiCurrentFlags);
        }
        else
        {
            NIVERIFY(NiShaderDeclaration::CreateDeclForGeometry(pkGeometry));
            pkDesc = (NiD3D10VertexDescription*)pkGeometry->GetShaderDecl();
        }
    }

    if (NiIsKindOf(NiParticles, pkGeometry))
    {
        if (!PackParticlesSprites(pkD3D10GeomData, pkGeometry->GetModelData(), 
            pkDesc, NiPoint3::UNIT_Y, NiPoint3::UNIT_Z))
        {
            return false;
        }
    }
    else if (NiIsKindOf(NiLines, pkGeometry))
    {
        if (!PackLines(pkD3D10GeomData, pkGeometry->GetModelData(), pkDesc))
        {
            return false;
        }
    }
    else
    {
        if (!PackGeometry(pkD3D10GeomData, pkGeometry->GetModelData(), 
            pkGeometry->GetSkinInstance(), pkDesc))
        {
            return false;
        }
    }

    return true;
}
//---------------------------------------------------------------------------
NiShaderDeclaration* NiD3D10Renderer::CreateShaderDeclaration(
    unsigned int uiEntriesPerStream, unsigned int uiNumStreams)
{
    return NiD3D10VertexDescription::Create(uiEntriesPerStream, uiNumStreams);
}
//---------------------------------------------------------------------------
void NiD3D10Renderer::RegisterD3D10Shader(
    NiD3D10ShaderInterface* pkD3D10Shader)
{
    m_kD3D10Shaders.AddHead(pkD3D10Shader);
}
//---------------------------------------------------------------------------
void NiD3D10Renderer::ReleaseD3D10Shader(NiD3D10ShaderInterface* pkD3D10Shader)
{
    m_kD3D10Shaders.Remove(pkD3D10Shader);
}
//---------------------------------------------------------------------------
void NiD3D10Renderer::PurgeD3D10Shader(NiD3D10ShaderInterface* pkD3D10Shader)
{
    pkD3D10Shader->DestroyRendererData();

    m_kD3D10Shaders.Remove(pkD3D10Shader);
}
//---------------------------------------------------------------------------
void NiD3D10Renderer::PurgeAllD3D10Shaders()
{
    while (!m_kD3D10Shaders.IsEmpty())
        PurgeD3D10Shader(m_kD3D10Shaders.GetHead());
}
//---------------------------------------------------------------------------
void NiD3D10Renderer::PurgeGeometryData(NiGeometryData* pkData)
{
    NiGeometryData::RendererData* pkRendererData = pkData->GetRendererData();
    if (pkRendererData)
    {
        NiDelete pkRendererData;
        pkData->SetRendererData(NULL);
    }
}
//---------------------------------------------------------------------------
void NiD3D10Renderer::PurgeMaterial(NiMaterialProperty* pkMaterial)
{
    // No renderer data stored on materials
}
//---------------------------------------------------------------------------
void NiD3D10Renderer::PurgeEffect(NiDynamicEffect* pLight)
{
    // No renderer data stored on dynamic effects
}
//---------------------------------------------------------------------------
void NiD3D10Renderer::PurgeScreenTextureRendererData(
    NiScreenTexture* pkScreenTexture)
{
    // No renderer data stored on screen textures
}
//---------------------------------------------------------------------------
void NiD3D10Renderer::PurgeSkinPartitionRendererData(
    NiSkinPartition* pkSkinPartition)
{
    // No renderer data stored on skin partitions
}
//---------------------------------------------------------------------------
void NiD3D10Renderer::PurgeSkinInstance(NiSkinInstance* pkSkinInstance)
{
    float* pfMatrices = (float*)pkSkinInstance->GetBoneMatrices();
    NiAlignedFree(pfMatrices);

    pkSkinInstance->SetBoneMatrices(NULL, 0);

    D3DXMATRIXA16* pkSkin2World_World2Skin = (D3DXMATRIXA16*)
        pkSkinInstance->GetSkinToWorldWorldToSkinMatrix();
    NiExternalDelete pkSkin2World_World2Skin;
    pkSkinInstance->SetSkinToWorldWorldToSkinMatrix(NULL);
}
//---------------------------------------------------------------------------
bool NiD3D10Renderer::PurgeTexture(NiTexture* pkTexture)
{
    NiD3D10TextureData* pkData = (NiD3D10TextureData*)
        (pkTexture->GetRendererData());

    if (pkData)
    {
        bool bIsRenderedTexture = pkData->IsRenderedTexture();

        if (bIsRenderedTexture)
        {
            if (NiIsKindOf(NiRenderedCubeMap, pkTexture))
            {
                NiRenderedCubeMap* pkRenderedCubeMap = 
                    (NiRenderedCubeMap*)pkTexture;
                for (unsigned int i = 0; i < NiRenderedCubeMap::FACE_NUM; i++)
                {
                    Ni2DBuffer* pkBuffer = pkRenderedCubeMap->GetFaceBuffer(
                        (NiRenderedCubeMap::FaceID)i);
                    pkBuffer->SetRendererData(NULL);
                }
            }
            else
            {
                NiRenderedTexture* pkRenderedTex = 
                    (NiRenderedTexture*)pkTexture;
                Ni2DBuffer* pkBuffer = pkRenderedTex->GetBuffer();
                pkBuffer->SetRendererData(NULL);
            }
        }

        pkTexture->SetRendererData(NULL);
        NiDelete pkData;
    }

    return true;
}
//---------------------------------------------------------------------------
bool NiD3D10Renderer::PurgeAllTextures(bool bPurgeLocked)
{
    NiD3D10TextureData::ClearTextureData();
    return true;
}
//---------------------------------------------------------------------------
NiPixelData* NiD3D10Renderer::TakeScreenShot(
    const NiRect<unsigned int>* pkScreenRect,
    const NiRenderTargetGroup* pkTarget)
{
    NIASSERT("Not implemented!" && false);
    return NULL;
}
//---------------------------------------------------------------------------
bool NiD3D10Renderer::FastCopy(const Ni2DBuffer* pkSrc, Ni2DBuffer* pkDest, 
    const NiRect<unsigned int>* pkSrcRect, 
    unsigned int uiDestX, unsigned int uiDestY)
{
    NIASSERT("Not implemented!" && false);
    return false;
}
//---------------------------------------------------------------------------
bool NiD3D10Renderer::Copy(const Ni2DBuffer* pkSrc, Ni2DBuffer* pkDest, 
    const NiRect<unsigned int>* pkSrcRect,
    const NiRect<unsigned int>* pkDestRect,
    Ni2DBuffer::CopyFilterPreference ePref)
{
    NIASSERT("Not implemented!" && false);
    return false;
}
//---------------------------------------------------------------------------
bool NiD3D10Renderer::GetLeftRightSwap() const
{
    return m_bLeftRightSwap;
}
//---------------------------------------------------------------------------
bool NiD3D10Renderer::SetLeftRightSwap(bool bSwap)
{
    m_bLeftRightSwap = bSwap;
    NIASSERT(m_pkRenderStateManager);
    m_pkRenderStateManager->SetLeftRightSwap(bSwap);

    return true;
}
//---------------------------------------------------------------------------
float NiD3D10Renderer::GetMaxFogValue() const
{
    return m_fMaxFogValue;
}
//---------------------------------------------------------------------------
void NiD3D10Renderer::SetMaxFogValue(float fFogVal)
{
    if (fFogVal <= 0.0f)
        fFogVal = 1e-5f;

    m_fMaxFogValue = fFogVal;
    m_fMaxFogFactor = 1.0f / fFogVal - 1.0f;
}
//---------------------------------------------------------------------------
float NiD3D10Renderer::GetMaxFogFactor() const
{
    return m_fMaxFogFactor;
}
//---------------------------------------------------------------------------
void NiD3D10Renderer::UseLegacyPipelineAsDefaultMaterial()
{
    // Legacy pipeline not supported in D3D10
}
//---------------------------------------------------------------------------
NiD3D10Renderer* NiD3D10Renderer::GetRenderer()
{
    return (NiD3D10Renderer*)ms_pkRenderer;
}
//---------------------------------------------------------------------------
NiTexturePtr NiD3D10Renderer::CreateNiTextureFromD3D10Texture(
    ID3D10Resource* pkD3D10Texture, 
    ID3D10ShaderResourceView* pkResourceView)
{
    if (pkD3D10Texture == NULL || pkResourceView == NULL)
        return NULL;

    NiD3D10Direct3DTexturePtr spTexture = NiD3D10Direct3DTexture::Create(this);
    NIASSERT(spTexture && "Failed to create NiD3D10Direct3DTexture!");

    NiD3D10Direct3DTextureData::Create(spTexture, pkD3D10Texture, 
        pkResourceView);

    return NiSmartPointerCast(NiTexture, spTexture);
}
//---------------------------------------------------------------------------
bool NiD3D10Renderer::CreateSourceTextureRendererData(
    NiSourceTexture* pkTexture)
{
    LockSourceDataCriticalSection();
    NiD3D10TextureData* pkData = 
        (NiD3D10TextureData*)(pkTexture->GetRendererData());

    if (!pkData)
        pkData = NiD3D10SourceTextureData::Create(pkTexture); 
    UnlockSourceDataCriticalSection();

    if (!pkData)
        return false;

    return true;
}
//---------------------------------------------------------------------------
bool NiD3D10Renderer::CreateRenderedTextureRendererData(
    NiRenderedTexture* pkTexture, 
    Ni2DBuffer::MultiSamplePreference eMSAAPref)
{
    NiD3D10TextureData* pkData = 
        (NiD3D10TextureData*)pkTexture->GetRendererData();
    if (pkData)
        return true;

    pkData = NiD3D10RenderedTextureData::Create(pkTexture, eMSAAPref); 
    if (!pkData)
        return false;

    return true;
}
//---------------------------------------------------------------------------
bool NiD3D10Renderer::CreateSourceCubeMapRendererData(
    NiSourceCubeMap* pkCubeMap)
{
    // Cube maps go through standard 2D texture path.
    return CreateSourceTextureRendererData(pkCubeMap);
}
//---------------------------------------------------------------------------
bool NiD3D10Renderer::CreateRenderedCubeMapRendererData(
    NiRenderedCubeMap* pkCubeMap)
{
    // Cube maps go through standard 2D texture path.
    return CreateRenderedTextureRendererData(pkCubeMap);
}
//---------------------------------------------------------------------------
bool NiD3D10Renderer::CreateDynamicTextureRendererData(
    NiDynamicTexture* pkTexture)
{
    NIASSERT("Not implemented!" && false);
    return false;
}
//---------------------------------------------------------------------------
void NiD3D10Renderer::CreatePaletteRendererData(NiPalette* pkPalette)
{
    // Palettes not supported in D3D10.
}
//---------------------------------------------------------------------------
bool NiD3D10Renderer::CreateDepthStencilRendererData(
    NiDepthStencilBuffer* pkDSBuffer, const NiPixelFormat* pkFormat,
    Ni2DBuffer::MultiSamplePreference eMSAAPref)
{
    if (m_pkD3D10Device == NULL)
    {
        NiD3D10Error::ReportWarning("Can't call "
            "CreateDepthStencilRendererData without a valid device.");
        return false;
    }

    if (pkDSBuffer == NULL)
        return false;

    DXGI_FORMAT eFormat;
    if (pkFormat)
        eFormat = NiD3D10PixelFormat::DetermineDXGIFormat(*pkFormat);
    else
        eFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;

    unsigned int uiMSAACount;
    unsigned int uiMSAAQuality;

    Ni2DBuffer::GetMSAACountAndQualityFromPref(eMSAAPref,
        uiMSAACount, uiMSAAQuality);

    ID3D10Texture2D* pkDSTexture = 
        m_pkResourceManager->CreateTexture2D(pkDSBuffer->GetWidth(),
        pkDSBuffer->GetHeight(), 1, 1, eFormat, uiMSAACount, uiMSAAQuality,
        D3D10_USAGE_DEFAULT, D3D10_BIND_DEPTH_STENCIL, 0, 0);

    if (pkDSTexture == NULL)
    {
        // Resource creation would have thrown error message
        NiD3D10Error::ReportWarning("CreateDepthStencilRendererData "
            "failed because depth/stencil texture could not be created.");
        return false;
    }

    NiD3D10DepthStencilBufferData* pkRendererData = 
        NiD3D10DepthStencilBufferData::Create(pkDSTexture, pkDSBuffer);

    // Reference to DS texture kept in NiD3D10DepthStencilBufferData
    pkDSTexture->Release();

    if (pkRendererData == NULL)
    {
        NiD3D10Error::ReportWarning("CreateDepthStencilRendererData "
            "failed because NiD3D10DepthStencilBufferData could not "
            "be created; destroying depth/stencil texture.");
        return false;
    }
    else
    {
        return true;
    }
}
//---------------------------------------------------------------------------
void NiD3D10Renderer::RemoveRenderedCubeMapData(NiRenderedCubeMap* pkCubeMap)
{
    NIASSERT("Not implemented!" && false);
}
//---------------------------------------------------------------------------
void NiD3D10Renderer::RemoveRenderedTextureData(NiRenderedTexture* pkTexture)
{
    NIASSERT("Not implemented!" && false);
}
//---------------------------------------------------------------------------
void NiD3D10Renderer::RemoveDynamicTextureData(NiDynamicTexture* pkTexture)
{
    NIASSERT("Not implemented!" && false);
}
//---------------------------------------------------------------------------
void* NiD3D10Renderer::LockDynamicTexture(
    const NiTexture::RendererData* pkRData,
    int& iPitch)
{
    NIASSERT("Not implemented!" && false);
    return NULL;
}
//---------------------------------------------------------------------------
bool NiD3D10Renderer::UnLockDynamicTexture(
    const NiTexture::RendererData* pkRData)
{
    NIASSERT("Not implemented!" && false);
    return false;
}
//---------------------------------------------------------------------------
NiD3D10GeometryData* NiD3D10Renderer::CreateGeometryRendererData(
    NiGeometry* pkGeometry)
{
    if (pkGeometry == NULL || pkGeometry->GetModelData() == NULL)
        return NULL;

    D3D10_PRIMITIVE_TOPOLOGY eTopology = D3D10_PRIMITIVE_TOPOLOGY_UNDEFINED;
    if (NiIsKindOf(NiTriShape, pkGeometry))
    {
        eTopology = D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
    }
    else if (NiIsKindOf(NiTriStrips, pkGeometry))
    {
        eTopology = D3D10_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;
    }
    else if (NiIsKindOf(NiParticles, pkGeometry))
    {
        eTopology = D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
    }
    else if (NiIsKindOf(NiLines, pkGeometry))
    {
        eTopology = D3D10_PRIMITIVE_TOPOLOGY_LINELIST;
    }

    // Count number of primitives necessary

    NiD3D10GeometryData* pkGeomData = NULL;
    NiSkinInstance* pkSkinInstance = pkGeometry->GetSkinInstance();

    if (pkSkinInstance && pkSkinInstance->GetSkinPartition())
    {
        // Skinned geometry - partitions * strips per partition
        NiSkinPartition* pkSkinPartition = pkSkinInstance->GetSkinPartition();
        const unsigned int uiPartitions = pkSkinPartition->GetPartitionCount();
        const NiSkinPartition::Partition* pkPartitions =
            pkSkinPartition->GetPartitions();

        // There's no particular reason 256 is used here; it could be
        // increased if necessary, but hopefully won't need to be.
        // It's just nice to use a static array instead of dynamically
        // allocating one.
        const unsigned int uiMaxPartitionCount = 256;
        NIASSERT(uiPartitions <= uiMaxPartitionCount);
        unsigned int auiPrimitiveCount[uiMaxPartitionCount];

        // Check for stripped partitions
        if (pkPartitions[0].m_usStrips == 0)
        {
            eTopology = D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
        }
        else
        {
            eTopology = D3D10_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;
        }

        for (unsigned int i = 0; i < uiPartitions; i++)
        {
            auiPrimitiveCount[i] = 0;
            if (pkPartitions[i].m_usStrips == 0)
                auiPrimitiveCount[i]++; // Triangle list
            else
                auiPrimitiveCount[i] += pkPartitions[i].m_usStrips; //Tristrips
        }
        pkGeomData = NiNew NiD3D10GeometryData(pkSkinInstance, uiPartitions, 
            auiPrimitiveCount);
    }
    else if (NiIsKindOf(NiTriStrips, pkGeometry))
    {
        // Unskinned geometry - total number of strips
        NiTriStripsData* pkData = (NiTriStripsData*)pkGeometry->GetModelData();
        pkGeomData = NiNew NiD3D10GeometryData(pkData->GetStripCount());
    }
    else
    {
        // Unskinned geometry - single primitive
        pkGeomData = NiNew NiD3D10GeometryData(1);
    }

    NIASSERT(eTopology != D3D10_PRIMITIVE_TOPOLOGY_UNDEFINED);
    pkGeomData->SetPrimitiveTopology(eTopology);

    // Append this geometry renderer data if necessary
    NiGeometryData* pkModelData = pkGeometry->GetModelData();
    NiD3D10GeometryData* pkCurrentData = 
        (NiD3D10GeometryData*)pkModelData->GetRendererData();
    if (pkCurrentData == NULL)
    {
        pkModelData->SetRendererData(pkGeomData);
    }
    else
    {
        while (pkCurrentData->GetNext() != NULL)
            pkCurrentData = pkCurrentData->GetNext();
        pkCurrentData->SetNext(pkGeomData);
    }

    return pkGeomData;
}
//---------------------------------------------------------------------------
bool NiD3D10Renderer::PackGeometry(NiD3D10GeometryData* pkD3D10GeomData, 
    NiGeometryData* pkData, NiSkinInstance* pkSkinInstance,
    NiD3D10VertexDescription* pkVertexDescription, bool bForce) const
{
    // From the vertex declaration, find out what vertex elements are required
    // 
    // For now, this will also determine which elements will get packed into
    // which vertex buffers. In the future, this will be determined by the
    // NiMesh data streams.

    if (pkD3D10GeomData == NULL || pkData == NULL || 
        pkVertexDescription == NULL)
    {
        return false;
    }

    pkVertexDescription->UpdateInputElementArray();

    // Check to see if valid VBs exist for each stream
    bool bValidVBs = false;
    const unsigned int uiPartitionCount = pkD3D10GeomData->GetPartitionCount();
    const unsigned int uiNumStreams = pkVertexDescription->GetStreamCount();
    unsigned int i = 0;
    if (pkD3D10GeomData->GetVBArray() == NULL ||
        pkD3D10GeomData->GetVBCount() != uiNumStreams)
    {
        // Release any existing vertex buffers here
        pkD3D10GeomData->SetVBCount(uiNumStreams);
    }
    else
    {
        bValidVBs = true;
    }

    NIASSERT(pkD3D10GeomData->GetSkinInstance() == pkSkinInstance);

    // Unless we are forcing a pack, or don't have a vertex buffer, we
    // don't want to pack static geometry.
    NiGeometryData::Consistency eConsistency = pkData->GetConsistency();
    if (!bForce && bValidVBs && eConsistency == NiGeometryData::STATIC)
    {
        return true;
    }

    // Store vertex buffer description
    pkD3D10GeomData->SetInputElementDescArray(
        pkVertexDescription->GetInputElementArray(), 
        pkVertexDescription->GetInputElementCount());

    // Store information about geometry data
    unsigned short usVertexCount = pkData->GetActiveVertexCount();
    unsigned short usMaxVertexCount = pkData->GetVertexCount();
    unsigned short usTriCount;
    unsigned short usMaxTriCount;
    unsigned int uiIndexCount;
    unsigned int uiMaxIndexCount;
    const unsigned short* pusIndexArray = NULL;
    const unsigned short* pusArrayLengths = NULL;
    unsigned short usArrayCount;
    const NiSkinPartition* pkSkinPartition = 
        (pkSkinInstance ? pkSkinInstance->GetSkinPartition() : NULL);
    NIASSERT(uiPartitionCount == 
        (pkSkinPartition ? pkSkinPartition->GetPartitionCount() : 1));
    const unsigned int uiTotalPrimitiveCount = 
        pkD3D10GeomData->GetTotalPrimitiveCount();

    unsigned int uiCurrentPrimitive = 0;
    unsigned int uiCurrentStartIndex = 0;

    if (pkSkinPartition)
    {
        usVertexCount = 0;
        usTriCount = 0;
        uiIndexCount = 0;
        unsigned int uiBaseVertexLocation = 0;
        unsigned int uiIBOffset = 0;
        for (i = 0; i < uiPartitionCount; i++)
        {
            const unsigned int uiPrimitiveStart = 
                pkD3D10GeomData->GetPrimitiveStart(i);
            const unsigned int uiPrimitiveCount = 
                pkD3D10GeomData->GetPrimitiveCount(i);

            const NiSkinPartition::Partition* pkPartition = 
                pkSkinPartition->GetPartitions() + i;
            usVertexCount += pkPartition->m_usVertices;
            usTriCount += pkPartition->m_usTriangles;

            if (pkPartition->m_usStrips == 0)
            {
                // Triangle list
                unsigned int uiCurrentIndexCount = 
                    pkPartition->m_usTriangles * 3;
                uiIndexCount += uiCurrentIndexCount;

                NIASSERT(uiCurrentPrimitive < 
                    uiPrimitiveStart + uiPrimitiveCount);
                pkD3D10GeomData->SetIBOffset(uiCurrentPrimitive, uiIBOffset);
                pkD3D10GeomData->SetIndexCount(uiCurrentPrimitive, 
                    uiCurrentIndexCount);
                pkD3D10GeomData->SetStartIndexLocation(uiCurrentPrimitive, 
                    uiCurrentStartIndex);
                pkD3D10GeomData->SetBaseVertexLocation(uiCurrentPrimitive, 
                    uiBaseVertexLocation);
                uiCurrentPrimitive++;
                uiCurrentStartIndex += uiCurrentIndexCount;

                uiBaseVertexLocation += pkPartition->m_usVertices;
            }
            else
            {
                // Triangle strips
                unsigned int uiCurrentIndexCount = 
                    pkPartition->m_usTriangles + 2 * pkPartition->m_usStrips;
                uiIndexCount += uiCurrentIndexCount;

                for (unsigned int j = 0; j < pkPartition->m_usStrips; j++)
                {
                    NIASSERT(uiCurrentPrimitive < 
                        uiPrimitiveStart + uiPrimitiveCount);
                    pkD3D10GeomData->SetIBOffset(uiCurrentPrimitive, 0);
                    pkD3D10GeomData->SetIndexCount(uiCurrentPrimitive, 
                        pkPartition->m_pusStripLengths[j]);
                    pkD3D10GeomData->SetStartIndexLocation(uiCurrentPrimitive, 
                        uiCurrentStartIndex);
                    pkD3D10GeomData->SetBaseVertexLocation(uiCurrentPrimitive, 
                        uiBaseVertexLocation);
                    uiCurrentPrimitive++;
                    uiCurrentStartIndex += pkPartition->m_pusStripLengths[j];
                }

                uiBaseVertexLocation += pkPartition->m_usVertices;
            }
        }
        usMaxVertexCount = usVertexCount;
        usMaxTriCount = usTriCount;
        uiMaxIndexCount = uiIndexCount;
    }
    else
    {
        if (NiIsKindOf(NiTriShapeData, pkData))
        {
            NIASSERT(uiPartitionCount == 1);
            NiTriShapeData* pkTSData = (NiTriShapeData*)pkData;

            usTriCount = pkTSData->GetActiveTriangleCount();
            usMaxTriCount = pkTSData->GetTriangleCount();

            pusIndexArray = pkTSData->GetTriList();
            pusArrayLengths = NULL;
            usArrayCount = 1;

            uiIndexCount = 3 * usTriCount;
            uiMaxIndexCount = 3 * usMaxTriCount;

            NIASSERT(uiCurrentPrimitive < uiTotalPrimitiveCount);
            pkD3D10GeomData->SetIBOffset(uiCurrentPrimitive, 0);
            pkD3D10GeomData->SetIndexCount(uiCurrentPrimitive, uiIndexCount);
            pkD3D10GeomData->SetStartIndexLocation(uiCurrentPrimitive, 
                uiCurrentStartIndex);
            pkD3D10GeomData->SetBaseVertexLocation(uiCurrentPrimitive, 0);

            uiCurrentPrimitive++;
            uiCurrentStartIndex += uiMaxIndexCount;
        }
        else if (NiIsKindOf(NiTriStripsData, pkData))
        {
            NIASSERT(uiPartitionCount == 1);
            NiTriStripsData* pkTSData = (NiTriStripsData*)pkData;

            usTriCount = pkTSData->GetActiveTriangleCount();
            usMaxTriCount = pkTSData->GetTriangleCount();

            pusIndexArray = pkTSData->GetStripLists();
            pusArrayLengths = pkTSData->GetStripLengths();
            usArrayCount = pkTSData->GetStripCount();

            uiIndexCount = usTriCount + 2 * usArrayCount;
            uiMaxIndexCount = usMaxTriCount + 2 * usArrayCount;

            for (i = 0; i < usArrayCount; i++)
            {
                NIASSERT(uiCurrentPrimitive < uiTotalPrimitiveCount);
                pkD3D10GeomData->SetIBOffset(uiCurrentPrimitive, 0);
                pkD3D10GeomData->SetIndexCount(uiCurrentPrimitive, 
                    pusArrayLengths[i]);
                pkD3D10GeomData->SetStartIndexLocation(uiCurrentPrimitive, 
                    uiCurrentStartIndex);
                pkD3D10GeomData->SetBaseVertexLocation(uiCurrentPrimitive, 0);

                uiCurrentPrimitive++;
                uiCurrentStartIndex += pusArrayLengths[i];
            }
        }
        else
        {
            NIASSERT(!"NiD3D10Renderer::PackGeometry> Unknown geom type!");
        }
    }
    NIASSERT(uiCurrentStartIndex == uiMaxIndexCount);

    // Track what parts of the geometry need to be repacked.
    unsigned short usDirtyFlags = pkData->GetRevisionID();

    // Handle the software skinning case
    if (pkSkinInstance != NULL && 
        (pkSkinInstance->GetFrameID() != GetFrameID()))
    {
        // This is the first time this software-skinned object has been 
        // rendered this frame, so ensure vertices and normals get packed. 
        // This way, the object won't have to be redeformed if it's rendered
        // multiple times in the same frame.
        usDirtyFlags |= 
            (NiGeometryData::VERTEX_MASK | NiGeometryData::NORMAL_MASK);

        pkSkinInstance->SetFrameID(GetFrameID());
    }

    // Volatile geometry always needs to be completely repacked, as does
    // geometry without valid VBs and geometry that is forced to be packed.
    if (eConsistency == NiGeometryData::VOLATILE || bForce || !bValidVBs)
    {
        usDirtyFlags = NiGeometryData::DIRTY_MASK;
    }

    // NiD3D10GeometryData has already been prepared for the number of 
    // streams, earlier in this function
    NIASSERT(pkD3D10GeomData->GetVBCount() == uiNumStreams);
    ID3D10Buffer*const* ppkVertexBuffers = pkD3D10GeomData->GetVBArray();
    NIASSERT(ppkVertexBuffers != NULL);
    
    // Determine type of vertex buffer to allocate.
    unsigned int uiAccessFlags = NiD3D10DataStream::ACCESS_GPU_READ;
    if (eConsistency == NiGeometryData::VOLATILE)
        uiAccessFlags |= NiD3D10DataStream::ACCESS_DYNAMIC;
    else if (eConsistency == NiGeometryData::MUTABLE)
        uiAccessFlags |= NiD3D10DataStream::ACCESS_CPU_WRITE;

    // Allocate vertex buffers
    unsigned int auiVBStrideArray[15];
    unsigned int auiVBOffsetArray[15];

    // Ensure existing VBs are big enough
    for (i = 0; i < uiNumStreams; i++)
    {
        NIASSERT(ppkVertexBuffers[i] == NULL || 
            pkD3D10GeomData->GetVBDataStream(i) != NULL);

        auiVBStrideArray[i] = pkVertexDescription->GetVertexStride(i);
        const unsigned int uiVBSize = usMaxVertexCount * auiVBStrideArray[i];

        if ((ppkVertexBuffers[i] == NULL) ||
            (pkD3D10GeomData->GetVBDataStream(i)->GetSize() < uiVBSize))
        {
            // Need to create a new VB
    
            NiD3D10DataStreamPtr spNewVB;
            NIVERIFY(NiD3D10DataStream::Create(uiVBSize, uiAccessFlags, 
                NiD3D10DataStream::USAGE_VERTEX, spNewVB));
            NIASSERT(spNewVB);
    
            pkD3D10GeomData->SetVBDataStream(i, spNewVB);

            bValidVBs = false;
        }

        auiVBOffsetArray[i] = 0;
    }
        
    if (bValidVBs == false)
    {
        // Had to reallocate VBs at some point - set dirty flags on everything
        // except 
        usDirtyFlags |= NiGeometryData::VERTEX_MASK |
            NiGeometryData::NORMAL_MASK |
            NiGeometryData::COLOR_MASK |
            NiGeometryData::TEXTURE_MASK;

        pkD3D10GeomData->SetVBStrideArray(auiVBStrideArray, uiNumStreams);

        for (i = 0; i < uiTotalPrimitiveCount; i++)
        {
            pkD3D10GeomData->SetVBOffsetArray(i, 
                auiVBOffsetArray, uiNumStreams);
        }
    }

    // Read information about buffer
    bool bColors = false;
    bool bNorms = false;
    bool bBinorms = false;
    bool bTangents = false;
    unsigned int uiTexCoords = 0;

    // Pack vertex buffers
    // Always repack if the TRIANGLE_COUNT_MASK is set, even if fewer 
    // triangles are used.
    // This means that the VB will be repacked unless TRIANGLE_INDEX_MASK
    // is set.
    if ((usDirtyFlags & ~NiTriBasedGeomData::TRIANGLE_INDEX_MASK) != 0)
    {
        // Check for software skinning
        bool bSoftwareDeform = (pkSkinInstance != 0 && pkSkinPartition == 0);

        NiPoint3* pkPos = NULL;
        NiPoint3* pkNorm = NULL;
        NiPoint3* pkBinorm = NULL;
        NiPoint3* pkTan = NULL;

        unsigned int uiPackedSize = 0;

        for (i = 0; i < uiNumStreams; i++)
        {
            // Lock VB
            NiD3D10DataStream* pkCurrentVB = 
                pkD3D10GeomData->GetVBDataStream(i);
            NIASSERT(pkCurrentVB && pkCurrentVB->GetSize() >= 
                usMaxVertexCount * pkVertexDescription->GetVertexStride(i));

            void* pvBuffer = pkCurrentVB->Lock(NiD3D10DataStream::LOCK_WRITE);

            const unsigned int uiEntryCount = 
                pkVertexDescription->GetEntryCount(i);
            for (unsigned int j = 0; j < uiEntryCount; j++)
            {
                const NiShaderDeclaration::ShaderRegisterEntry* pkEntry = 
                    pkVertexDescription->GetEntry(j, i);

                void* pvDest = (char*)pvBuffer + 
                    pkVertexDescription->GetPackingOffset(i, j);
                const unsigned int uiDestStride = 
                    pkVertexDescription->GetVertexStride(i);

                // Almost always need to pack - because mutable resources
                // (using the DEFAULT usage) need to be updated via a 
                // temp buffer, we need to fill in the entire contents
                // of the temp buffer.
                // The only reason not to pack is if we're doing software
                // deformation, in which case the software deformation
                // will pack the data.
                bool bPack = true;

                switch (pkEntry->m_eInput)
                {
                case NiShaderDeclaration::SHADERPARAM_NI_POSITION:
                    if ((usDirtyFlags & NiGeometryData::VERTEX_MASK) != 0)
                    {
                        NIASSERT(pkEntry->m_eType == 
                            NiShaderDeclaration::SPTYPE_FLOAT3);
                        if (bSoftwareDeform)
                        {
                            pkPos = (NiPoint3*)pvDest;
                            bPack = false;
                        }
                    }
                    break;
                case NiShaderDeclaration::SHADERPARAM_NI_NORMAL:
                    if ((usDirtyFlags & NiGeometryData::NORMAL_MASK) != 0)
                    {
                        NIASSERT(pkEntry->m_eType == 
                            NiShaderDeclaration::SPTYPE_FLOAT3);
                        if (bSoftwareDeform)
                        {
                            pkNorm = (NiPoint3*)pvDest;
                            bPack = false;
                        }
                    }
                    bNorms = true;
                    break;
                case NiShaderDeclaration::SHADERPARAM_NI_BINORMAL:
                    if ((usDirtyFlags & NiGeometryData::NORMAL_MASK) != 0)
                    {
                        NIASSERT(pkEntry->m_eType == 
                            NiShaderDeclaration::SPTYPE_FLOAT3);
                        if (bSoftwareDeform)
                        {
                            pkBinorm = (NiPoint3*)pvDest;
                            bPack = false;
                        }
                    }
                    bBinorms = true;
                    break;
                case NiShaderDeclaration::SHADERPARAM_NI_TANGENT:
                    if ((usDirtyFlags & NiGeometryData::NORMAL_MASK) != 0)
                    {
                        NIASSERT(pkEntry->m_eType == 
                            NiShaderDeclaration::SPTYPE_FLOAT3);
                        if (bSoftwareDeform)
                        {
                            pkTan = (NiPoint3*)pvDest;
                            bPack = false;
                        }
                    }
                    bTangents = true;
                    break;
                case NiShaderDeclaration::SHADERPARAM_NI_COLOR:
                    if ((usDirtyFlags & NiGeometryData::COLOR_MASK) != 0)
                        bPack = true;
                    bColors = true;
                    break;
                case NiShaderDeclaration::SHADERPARAM_NI_TEXCOORD0:
                case NiShaderDeclaration::SHADERPARAM_NI_TEXCOORD1:
                case NiShaderDeclaration::SHADERPARAM_NI_TEXCOORD2:
                case NiShaderDeclaration::SHADERPARAM_NI_TEXCOORD3:
                case NiShaderDeclaration::SHADERPARAM_NI_TEXCOORD4:
                case NiShaderDeclaration::SHADERPARAM_NI_TEXCOORD5:
                case NiShaderDeclaration::SHADERPARAM_NI_TEXCOORD6:
                case NiShaderDeclaration::SHADERPARAM_NI_TEXCOORD7:
                    if ((usDirtyFlags & NiGeometryData::TEXTURE_MASK) != 0)
                        bPack = true;
                    uiTexCoords++;
                    break;
                default:
                    // Pack everything else
                    bPack = true;
                    break;
                }

                if (bPack)
                {
                    uiPackedSize += m_pkGeometryPacker->PackEntry(
                        usVertexCount, pkData, pkSkinInstance, pkEntry, 
                        uiDestStride, pvDest, 0, NULL);
                }
                else
                {
                    uiPackedSize += NiD3D10VertexDescription::GetSPTypeSize(
                        pkEntry->m_eType) * usVertexCount;
                }
            }

            // Unlock VB
            pkCurrentVB->Unlock();
        }

        pkD3D10GeomData->UpdateVBs();
    }

    // Set flags on geometry renderer data
    if (pkD3D10GeomData->GetFlags() == 0)
    {
        pkD3D10GeomData->SetFlags(
            NiD3D10GeometryData::CreateVertexFlags(bColors, bNorms, 
            bBinorms && bTangents, uiTexCoords));
    }

    // Create index buffers, if necessary
    if (pusIndexArray != NULL || pkSkinPartition != NULL)
    {
        // Note that NiGeometry has a limit of 16-bit index buffers
        NiD3D10DataStream* pkIBDataStream = 
            pkD3D10GeomData->GetIBDataStream(0);
        const unsigned int uiIBSize = uiMaxIndexCount * sizeof(unsigned short);
        if (pkIBDataStream == NULL || pkIBDataStream->GetSize() < uiIBSize)
        {
            // All primitives share same IB data stream.

            NiD3D10DataStreamPtr spNewIB;
            // Use same consistency for index buffers as we do for 
            // vertex buffers
            NIVERIFY(NiD3D10DataStream::Create(uiIBSize, uiAccessFlags, 
                NiD3D10DataStream::USAGE_INDEX, spNewIB));
            NIASSERT(spNewIB);

            // Set new IBs, which should release existing ones
            for (i = 0; i < uiTotalPrimitiveCount; i++)
            {
                pkD3D10GeomData->SetIBDataStream(i, spNewIB);
                pkD3D10GeomData->SetIBFormat(i, DXGI_FORMAT_R16_UINT);
            }

            // Had to create new index buffers - mark them as dirty
            usDirtyFlags |= NiTriBasedGeomData::TRIANGLE_INDEX_MASK | 
                NiTriBasedGeomData::TRIANGLE_COUNT_MASK;
        }

        // Pack index buffer
        // Always repack if TRIANGLE_COUNT_MASK is set, even if fewer
        // triangles are used.
        if ((usDirtyFlags & (NiTriBasedGeomData::TRIANGLE_INDEX_MASK | 
            NiTriBasedGeomData::TRIANGLE_COUNT_MASK)) != 0)
        {
            // Lock IB
            NiD3D10DataStream* pkCurrentIB = 
                pkD3D10GeomData->GetIBDataStream(0);

            NIASSERT(pkCurrentIB && pkCurrentIB->GetSize() >= 
                uiMaxIndexCount * sizeof(unsigned short));

            void* pvBuffer = pkCurrentIB->Lock(NiD3D10DataStream::LOCK_WRITE);

            if (pkSkinPartition)
            {
                unsigned short* pusIterator = (unsigned short*)pvBuffer;
                for (i = 0; i < uiPartitionCount; i++)
                {
                    const NiSkinPartition::Partition* pkPartition = 
                        pkSkinPartition->GetPartitions() + i;

                    unsigned int uiIndexCount = (pkPartition->m_usStrips == 0 ?
                        pkPartition->m_usTriangles * 3 : 
                    pkPartition->m_usTriangles + 2 * pkPartition->m_usStrips);

                    NIASSERT(pkPartition->m_pusTriList);
                    NiMemcpy(pusIterator, pkPartition->m_pusTriList, 
                        uiIndexCount * sizeof(unsigned short));
                    pusIterator += uiIndexCount;
                }
            }
            else
            {
                NIASSERT(pusIndexArray);
                NiMemcpy(pvBuffer, (const void*)pusIndexArray, 
                    uiIndexCount * sizeof(unsigned short));

            }

            // Unlock IB
            pkCurrentIB->Unlock();

            pkD3D10GeomData->UpdateIBs();
        }
    }
   
    pkData->ClearRevisionID();

    return true;
}
//---------------------------------------------------------------------------
bool NiD3D10Renderer::PackParticlesSprites(
    NiD3D10GeometryData* pkD3D10GeomData, NiGeometryData* pkData, 
    NiD3D10VertexDescription* pkVertexDescription, 
    const NiPoint3& kModelCameraRight, const NiPoint3& kModelCameraUp, 
    bool bForce)
{
    // From the vertex declaration, find out what vertex elements are required
    // 
    // For now, this will also determine which elements will get packed into
    // which vertex buffers. In the future, this will be determined by the
    // NiMesh data streams.

    if (pkD3D10GeomData == NULL || pkData == NULL || 
        pkVertexDescription == NULL)
    {
        return false;
    }

    pkVertexDescription->UpdateInputElementArray();

    // Check to see if valid VBs exist for each stream
    bool bValidVBs = false;
    const unsigned int uiPartitionCount = pkD3D10GeomData->GetPartitionCount();
    const unsigned int uiNumStreams = pkVertexDescription->GetStreamCount();
    unsigned int i = 0;
    if (pkD3D10GeomData->GetVBArray() == NULL ||
        pkD3D10GeomData->GetVBCount() != uiNumStreams)
    {
        // Release any existing vertex buffers here
        pkD3D10GeomData->SetVBCount(uiNumStreams);
    }
    else
    {
        bValidVBs = true;
    }

    // Unless we are forcing a pack, or don't have a vertex buffer, we
    // don't want to pack static geometry.
    NiGeometryData::Consistency eConsistency = pkData->GetConsistency();
    if (!bForce && bValidVBs && eConsistency == NiGeometryData::STATIC)
    {
        return true;
    }

    // Store vertex buffer description
    pkD3D10GeomData->SetInputElementDescArray(
        pkVertexDescription->GetInputElementArray(), 
        pkVertexDescription->GetInputElementCount());

    // Store information about geometry data
    unsigned short usPointCount = pkData->GetActiveVertexCount();
    unsigned short usMaxPointCount = pkData->GetVertexCount();
    unsigned short usVertexCount = usPointCount * 4;
    unsigned short usMaxVertexCount = usMaxPointCount * 4;
    unsigned short usTriCount = usPointCount * 2;
    unsigned short usMaxTriCount = usMaxPointCount * 2;
    unsigned int uiIndexCount = usTriCount * 3;
    unsigned int uiMaxIndexCount = usMaxTriCount * 3;
    const unsigned int uiTotalPrimitiveCount = 
        pkD3D10GeomData->GetTotalPrimitiveCount();

    pkD3D10GeomData->SetIBOffset(0, 0);
    pkD3D10GeomData->SetIndexCount(0, uiIndexCount);
    pkD3D10GeomData->SetStartIndexLocation(0, 0);
    pkD3D10GeomData->SetBaseVertexLocation(0, 0);

    // Track what parts of the geometry need to be repacked.
    unsigned short usDirtyFlags = pkData->GetRevisionID();

    // Volatile geometry always needs to be completely repacked, as does
    // geometry without valid VBs and geometry that is forced to be packed.
    if (eConsistency == NiGeometryData::VOLATILE || bForce || !bValidVBs)
    {
        usDirtyFlags = NiGeometryData::DIRTY_MASK;
    }

    // NiD3D10GeometryData has already been prepared for the number of 
    // streams, earlier in this function
    NIASSERT(pkD3D10GeomData->GetVBCount() == uiNumStreams);
    ID3D10Buffer*const* ppkVertexBuffers = pkD3D10GeomData->GetVBArray();
    NIASSERT(ppkVertexBuffers != NULL);
    
    // Determine type of vertex buffer to allocate.
    unsigned int uiAccessFlags = NiD3D10DataStream::ACCESS_GPU_READ;
    if (eConsistency == NiGeometryData::VOLATILE)
        uiAccessFlags |= NiD3D10DataStream::ACCESS_DYNAMIC;
    else if (eConsistency == NiGeometryData::MUTABLE)
        uiAccessFlags |= NiD3D10DataStream::ACCESS_CPU_WRITE;

    // Allocate vertex buffers
    // This value is 16, not 15!
    unsigned int auiVBStrideArray[15];
    unsigned int auiVBOffsetArray[15];

    // Ensure existing VBs are big enough
    for (i = 0; i < uiNumStreams; i++)
    {
        NIASSERT(ppkVertexBuffers[i] == NULL || 
            pkD3D10GeomData->GetVBDataStream(i) != NULL);

        auiVBStrideArray[i] = pkVertexDescription->GetVertexStride(i);
        const unsigned int uiVBSize = usMaxVertexCount * auiVBStrideArray[i];

        if ((ppkVertexBuffers[i] == NULL) ||
            (pkD3D10GeomData->GetVBDataStream(i)->GetSize() < uiVBSize))
        {
            // Need to create a new VB
    
            NiD3D10DataStreamPtr spNewVB;
            NIVERIFY(NiD3D10DataStream::Create(uiVBSize, uiAccessFlags, 
                NiD3D10DataStream::USAGE_VERTEX, spNewVB));
            NIASSERT(spNewVB);
    
            pkD3D10GeomData->SetVBDataStream(i, spNewVB);

            bValidVBs = false;
        }

        auiVBOffsetArray[i] = 0;
    }
        
    if (bValidVBs == false)
    {
        // Had to reallocate VBs at some point - set dirty flags on everything
        // except 
        usDirtyFlags |= NiGeometryData::VERTEX_MASK |
            NiGeometryData::NORMAL_MASK |
            NiGeometryData::COLOR_MASK |
            NiGeometryData::TEXTURE_MASK;

        pkD3D10GeomData->SetVBStrideArray(auiVBStrideArray, uiNumStreams);

        for (i = 0; i < uiTotalPrimitiveCount; i++)
        {
            pkD3D10GeomData->SetVBOffsetArray(i, 
                auiVBOffsetArray, uiNumStreams);
        }
    }

    // The normal vector for each quad
    NiPoint3 kModelNormal = kModelCameraRight.Cross(kModelCameraUp);
    NiPoint3 kModelBinormal = -kModelCameraUp;
    NiPoint3 kModelTangent = kModelCameraRight;

    // Fill in temporary buffers here
    ReallocateTempArrays(usMaxVertexCount);

    NIASSERT(NiIsKindOf(NiParticlesData, pkData));
    NiParticlesData* pkParticlesData = (NiParticlesData*)pkData;

    float* pfR = pkParticlesData->GetRadii();
    float* pfS = pkParticlesData->GetSizes();
    float* pfA = pkParticlesData->GetRotationAngles();
    NiPoint3* pkP = pkParticlesData->GetVertices();
    NiColorA* pkC = pkParticlesData->GetColors();
    NiPoint3* pkVerts = m_pkTempVertices;
    NiColorA* pkColors = m_pkTempColors;
    NiPoint2* pkTex = m_pkTempTexCoords;

    // Only the angles and colors arrays can be NULL
    NIASSERT(pfR != NULL && pfS != NULL && pkP != NULL);

    if (pfA != NULL)
    {
        for (i = 0; i < usPointCount; i++)
        {
            float fSize = (*pfR) * (*pfS);

            // Compute the rotated top left and top right offset vectors.
            float fSinA, fCosA;
            NiSinCos(*pfA, fSinA, fCosA);
            float fC1 = fSize * (fCosA + fSinA);
            float fC2 = fSize * (fCosA - fSinA);
            const NiPoint3 kV0 = fC1 * kModelCameraRight + fC2 *
                kModelCameraUp;
            const NiPoint3 kV1 = -fC2 * kModelCameraRight + fC1 *
                kModelCameraUp;

            *pkVerts++ = *pkP - kV0;
            pkTex->x = 0.0f;
            pkTex->y = 1.0f;
            pkTex++;

            *pkVerts++ = *pkP - kV1;
            pkTex->x = 1.0f;
            pkTex->y = 1.0f;
            pkTex++;

            *pkVerts++ = *pkP + kV0;
            pkTex->x = 1.0f;
            pkTex->y = 0.0f;
            pkTex++;

            *pkVerts++ = *pkP + kV1;
            pkTex->x = 0.0f;
            pkTex->y = 0.0f;
            pkTex++;

            pkP++;
            pfR++;
            pfS++;
            pfA++;
        }
    }
    else
    {
        for (i = 0; i < usPointCount; i++)
        {
            float fSize = (*pfR) * (*pfS);

            // Compute the top left and top right offset vectors.
            const NiPoint3 kV0 = fSize * (kModelCameraUp + kModelCameraRight);
            const NiPoint3 kV1 = fSize * (kModelCameraUp - kModelCameraRight);

            *pkVerts++ = *pkP - kV0;
            pkTex->x = 0.0f;
            pkTex->y = 1.0f;
            pkTex++;

            *pkVerts++ = *pkP - kV1;
            pkTex->x = 1.0f;
            pkTex->y = 1.0f;
            pkTex++;

            *pkVerts++ = *pkP + kV0;
            pkTex->x = 1.0f;
            pkTex->y = 0.0f;
            pkTex++;

            *pkVerts++ = *pkP + kV1;
            pkTex->x = 0.0f;
            pkTex->y = 0.0f;
            pkTex++;

            pkP++;
            pfR++;
            pfS++;
            pfA++;
        }
    }

    if (pkC != NULL)
    {
        for (i = 0; i < usPointCount; i++)
        {
            *pkColors++ = *pkC;
            *pkColors++ = *pkC;
            *pkColors++ = *pkC;
            *pkColors++ = *pkC;

            pkC++;
        }
    }
    else
    {
        *pkColors == NiColorA::WHITE;
    }

    // Read information about buffer
    bool bColors = false;
    bool bNorms = false;
    bool bBinorms = false;
    bool bTangents = false;
    unsigned int uiTexCoords = 0;

    // Pack vertex buffers
    // Always repack if the TRIANGLE_COUNT_MASK is set, even if fewer 
    // triangles are used.
    // This means that the VB will be repacked unless TRIANGLE_INDEX_MASK
    // is set.
    if ((usDirtyFlags & ~NiTriBasedGeomData::TRIANGLE_INDEX_MASK) != 0)
    {
        NiPoint3* pkPos = NULL;
        NiPoint3* pkNorm = NULL;
        NiPoint3* pkBinorm = NULL;
        NiPoint3* pkTan = NULL;

        unsigned int uiPackedSize = 0;

        for (i = 0; i < uiNumStreams; i++)
        {
            // Lock VB
            NiD3D10DataStream* pkCurrentVB = 
                pkD3D10GeomData->GetVBDataStream(i);
            NIASSERT(pkCurrentVB && pkCurrentVB->GetSize() >= 
                usMaxVertexCount * pkVertexDescription->GetVertexStride(i));

            void* pvBuffer = pkCurrentVB->Lock(NiD3D10DataStream::LOCK_WRITE);

            const unsigned int uiEntryCount = 
                pkVertexDescription->GetEntryCount(i);
            for (unsigned int j = 0; j < uiEntryCount; j++)
            {
                const NiShaderDeclaration::ShaderRegisterEntry* pkEntry = 
                    pkVertexDescription->GetEntry(j, i);

                void* pvDest = (char*)pvBuffer + 
                    pkVertexDescription->GetPackingOffset(i, j);
                const unsigned int uiDestStride = 
                    pkVertexDescription->GetVertexStride(i);
                void* pvSource = NULL;
                unsigned int uiSourceStride;

                bool bPack = false;

                switch (pkEntry->m_eInput)
                {
                case NiShaderDeclaration::SHADERPARAM_NI_POSITION:
                    if ((usDirtyFlags & NiGeometryData::VERTEX_MASK) != 0)
                    {
                        NIASSERT(pkEntry->m_eType == 
                            NiShaderDeclaration::SPTYPE_FLOAT3);
                        pvSource = m_pkTempVertices;
                        uiSourceStride = sizeof(m_pkTempVertices[0]);
                        bPack = true;
                    }
                    break;
                case NiShaderDeclaration::SHADERPARAM_NI_NORMAL:
                    if ((usDirtyFlags & NiGeometryData::NORMAL_MASK) != 0)
                    {
                        bNorms = true;
                        NIASSERT(pkEntry->m_eType == 
                            NiShaderDeclaration::SPTYPE_FLOAT3);
                        pvSource = &kModelNormal;
                        uiSourceStride = 0;
                        bPack = true;
                    }
                    break;
                case NiShaderDeclaration::SHADERPARAM_NI_BINORMAL:
                    if ((usDirtyFlags & NiGeometryData::NORMAL_MASK) != 0)
                    {
                        bBinorms = true;
                        NIASSERT(pkEntry->m_eType == 
                            NiShaderDeclaration::SPTYPE_FLOAT3);
                        pvSource = &kModelBinormal;
                        uiSourceStride = 0;
                        bPack = true;
                    }
                    break;
                case NiShaderDeclaration::SHADERPARAM_NI_TANGENT:
                    if ((usDirtyFlags & NiGeometryData::NORMAL_MASK) != 0)
                    {
                        bTangents = true;
                        NIASSERT(pkEntry->m_eType == 
                            NiShaderDeclaration::SPTYPE_FLOAT3);
                        pvSource = &kModelTangent;
                        uiSourceStride = 0;
                        bPack = true;
                    }
                    break;
                case NiShaderDeclaration::SHADERPARAM_NI_COLOR:
                    if ((usDirtyFlags & NiGeometryData::COLOR_MASK) != 0)
                    {
                        bColors = true;
                        pvSource = m_pkTempColors;
                        if (pkC == NULL)
                            uiSourceStride = 0;
                        else
                            uiSourceStride = sizeof(m_pkTempColors[0]);
                        bPack = true;
                    }
                    break;
                case NiShaderDeclaration::SHADERPARAM_NI_TEXCOORD0:
                case NiShaderDeclaration::SHADERPARAM_NI_TEXCOORD1:
                case NiShaderDeclaration::SHADERPARAM_NI_TEXCOORD2:
                case NiShaderDeclaration::SHADERPARAM_NI_TEXCOORD3:
                case NiShaderDeclaration::SHADERPARAM_NI_TEXCOORD4:
                case NiShaderDeclaration::SHADERPARAM_NI_TEXCOORD5:
                case NiShaderDeclaration::SHADERPARAM_NI_TEXCOORD6:
                case NiShaderDeclaration::SHADERPARAM_NI_TEXCOORD7:
                    if ((usDirtyFlags & NiGeometryData::TEXTURE_MASK) != 0)
                    {
                        uiTexCoords++;
                        pvSource = m_pkTempTexCoords;
                        uiSourceStride = sizeof(m_pkTempTexCoords[0]);
                        bPack = true;
                    }
                    break;
                default:
                    // Pack everything else
                    bPack = true;
                    break;
                }

                if (bPack)
                {
                    uiPackedSize += m_pkGeometryPacker->PackEntry(
                        usVertexCount, pkData, NULL, pkEntry, uiDestStride, 
                        pvDest, uiSourceStride, pvSource);
                }
                else
                {
                    uiPackedSize += NiD3D10VertexDescription::GetSPTypeSize(
                        pkEntry->m_eType) * usVertexCount;
                }
            }

            // Unlock VB
            pkCurrentVB->Unlock();
        }

        pkD3D10GeomData->UpdateVBs();
    }

    // Set flags on geometry renderer data
    if (pkD3D10GeomData->GetFlags() == 0)
    {
        pkD3D10GeomData->SetFlags(
            NiD3D10GeometryData::CreateVertexFlags(bColors, bNorms, 
            bBinorms && bTangents, uiTexCoords));
    }

    // Create index buffers, if necessary

    // Note that NiGeometry has a limit of 16-bit index buffers
    NiD3D10DataStream* pkIBDataStream = 
        pkD3D10GeomData->GetIBDataStream(0);
    const unsigned int uiIBSize = uiMaxIndexCount * sizeof(unsigned short);
    if (pkIBDataStream == NULL || pkIBDataStream->GetSize() < uiIBSize)
    {
        // All primitives share same IB data stream.

        NiD3D10DataStreamPtr spNewIB;
        // Use same consistency for index buffers as we do for 
        // vertex buffers
        NIVERIFY(NiD3D10DataStream::Create(uiIBSize, uiAccessFlags, 
            NiD3D10DataStream::USAGE_INDEX, spNewIB));
        NIASSERT(spNewIB);

        // Set new IBs, which should release existing ones
        for (i = 0; i < uiTotalPrimitiveCount; i++)
        {
            pkD3D10GeomData->SetIBDataStream(i, spNewIB);
            pkD3D10GeomData->SetIBFormat(i, DXGI_FORMAT_R16_UINT);
        }

        // Had to create new index buffers - mark them as dirty
        usDirtyFlags |= NiTriBasedGeomData::TRIANGLE_INDEX_MASK | 
            NiTriBasedGeomData::TRIANGLE_COUNT_MASK;
    }

    // Pack index buffer
    // Always repack if TRIANGLE_COUNT_MASK is set, even if fewer
    // triangles are used.
    if ((usDirtyFlags & (NiTriBasedGeomData::TRIANGLE_INDEX_MASK | 
        NiTriBasedGeomData::TRIANGLE_COUNT_MASK)) != 0)
    {
        // Lock IB
        NiD3D10DataStream* pkCurrentIB = 
            pkD3D10GeomData->GetIBDataStream(0);

        NIASSERT(pkCurrentIB && pkCurrentIB->GetSize() >= 
            uiMaxIndexCount * sizeof(unsigned short));

        void* pvBuffer = pkCurrentIB->Lock(NiD3D10DataStream::LOCK_WRITE);

        unsigned short* pusTemp = (unsigned short*)pvBuffer;
        unsigned int uiBase = 0;
        for (i = 0; i < usPointCount; i++)
        {
            *(pusTemp++) = uiBase + 0;
            *(pusTemp++) = uiBase + 1;
            *(pusTemp++) = uiBase + 2;
            *(pusTemp++) = uiBase + 0;
            *(pusTemp++) = uiBase + 2;
            *(pusTemp++) = uiBase + 3;
            uiBase += 4;
        }

        // Unlock IB
        pkCurrentIB->Unlock();

        pkD3D10GeomData->UpdateIBs();
    }
   
    pkData->ClearRevisionID();

    return true;
}
//---------------------------------------------------------------------------
bool NiD3D10Renderer::PackLines(NiD3D10GeometryData* pkD3D10GeomData, 
    NiGeometryData* pkData, NiD3D10VertexDescription* pkVertexDescription, 
    bool bForce)
{
    // From the vertex declaration, find out what vertex elements are required
    // 
    // For now, this will also determine which elements will get packed into
    // which vertex buffers. In the future, this will be determined by the
    // NiMesh data streams.

    if (pkD3D10GeomData == NULL || pkData == NULL || 
        pkVertexDescription == NULL)
    {
        return false;
    }

    pkVertexDescription->UpdateInputElementArray();

    // Check to see if valid VBs exist for each stream
    bool bValidVBs = false;
    const unsigned int uiPartitionCount = pkD3D10GeomData->GetPartitionCount();
    const unsigned int uiNumStreams = pkVertexDescription->GetStreamCount();
    unsigned int i = 0;
    if (pkD3D10GeomData->GetVBArray() == NULL ||
        pkD3D10GeomData->GetVBCount() != uiNumStreams)
    {
        // Release any existing vertex buffers here
        pkD3D10GeomData->SetVBCount(uiNumStreams);
    }
    else
    {
        bValidVBs = true;
    }

    // Unless we are forcing a pack, or don't have a vertex buffer, we
    // don't want to pack static geometry.
    NiGeometryData::Consistency eConsistency = pkData->GetConsistency();
    if (!bForce && bValidVBs && eConsistency == NiGeometryData::STATIC)
    {
        return true;
    }

    // Store vertex buffer description
    pkD3D10GeomData->SetInputElementDescArray(
        pkVertexDescription->GetInputElementArray(), 
        pkVertexDescription->GetInputElementCount());

    // Store information about geometry data
    unsigned short usMaxVertexCount = pkData->GetVertexCount();
    unsigned short usVertexCount = pkData->GetActiveVertexCount();
    unsigned short usMaxLineCount = usMaxVertexCount;

    unsigned short usLineCount = 0;
    NIASSERT(NiIsKindOf(NiLinesData, pkData));
    NiLinesData* pkLinesData = (NiLinesData*)pkData;
    const NiBool* pkFlags = pkLinesData->GetFlags();
    for (i = 0; i < usMaxVertexCount; i++)
    {
        if (pkFlags[i])
            usLineCount++;
    }

    unsigned int uiIndexCount = usLineCount * 2;
    unsigned int uiMaxIndexCount = usMaxLineCount * 2;
    const unsigned int uiTotalPrimitiveCount = 
        pkD3D10GeomData->GetTotalPrimitiveCount();

    pkD3D10GeomData->SetIBOffset(0, 0);
    pkD3D10GeomData->SetIndexCount(0, uiIndexCount);
    pkD3D10GeomData->SetStartIndexLocation(0, 0);
    pkD3D10GeomData->SetBaseVertexLocation(0, 0);

    // Track what parts of the geometry need to be repacked.
    unsigned short usDirtyFlags = pkData->GetRevisionID();

    // Volatile geometry always needs to be completely repacked, as does
    // geometry without valid VBs and geometry that is forced to be packed.
    if (eConsistency == NiGeometryData::VOLATILE || bForce || !bValidVBs)
    {
        usDirtyFlags = NiGeometryData::DIRTY_MASK;
    }

    // NiD3D10GeometryData has already been prepared for the number of 
    // streams, earlier in this function
    NIASSERT(pkD3D10GeomData->GetVBCount() == uiNumStreams);
    ID3D10Buffer*const* ppkVertexBuffers = pkD3D10GeomData->GetVBArray();
    NIASSERT(ppkVertexBuffers != NULL);

    // Determine type of vertex buffer to allocate.
    unsigned int uiAccessFlags = NiD3D10DataStream::ACCESS_GPU_READ;
    if (eConsistency == NiGeometryData::VOLATILE)
        uiAccessFlags |= NiD3D10DataStream::ACCESS_DYNAMIC;
    else if (eConsistency == NiGeometryData::MUTABLE)
        uiAccessFlags |= NiD3D10DataStream::ACCESS_CPU_WRITE;

    // Allocate vertex buffers
    // This value is 16, not 15!
    unsigned int auiVBStrideArray[15];
    unsigned int auiVBOffsetArray[15];

    // Ensure existing VBs are big enough
    for (i = 0; i < uiNumStreams; i++)
    {
        NIASSERT(ppkVertexBuffers[i] == NULL || 
            pkD3D10GeomData->GetVBDataStream(i) != NULL);

        auiVBStrideArray[i] = pkVertexDescription->GetVertexStride(i);
        const unsigned int uiVBSize = usMaxVertexCount * auiVBStrideArray[i];

        if ((ppkVertexBuffers[i] == NULL) ||
            (pkD3D10GeomData->GetVBDataStream(i)->GetSize() < uiVBSize))
        {
            // Need to create a new VB

            NiD3D10DataStreamPtr spNewVB;
            NIVERIFY(NiD3D10DataStream::Create(uiVBSize, uiAccessFlags, 
                NiD3D10DataStream::USAGE_VERTEX, spNewVB));
            NIASSERT(spNewVB);

            pkD3D10GeomData->SetVBDataStream(i, spNewVB);

            bValidVBs = false;
        }

        auiVBOffsetArray[i] = 0;
    }

    if (bValidVBs == false)
    {
        // Had to reallocate VBs at some point - set dirty flags on everything
        // except 
        usDirtyFlags |= NiGeometryData::VERTEX_MASK |
            NiGeometryData::NORMAL_MASK |
            NiGeometryData::COLOR_MASK |
            NiGeometryData::TEXTURE_MASK;

        pkD3D10GeomData->SetVBStrideArray(auiVBStrideArray, uiNumStreams);

        for (i = 0; i < uiTotalPrimitiveCount; i++)
        {
            pkD3D10GeomData->SetVBOffsetArray(i, 
                auiVBOffsetArray, uiNumStreams);
        }
    }

    // Read information about buffer
    bool bColors = false;
    bool bNorms = false;
    bool bBinorms = false;
    bool bTangents = false;
    unsigned int uiTexCoords = 0;

    // Pack vertex buffers
    // Always repack if the TRIANGLE_COUNT_MASK is set, even if fewer 
    // triangles are used.
    // This means that the VB will be repacked unless TRIANGLE_INDEX_MASK
    // is set.
    if ((usDirtyFlags & ~NiTriBasedGeomData::TRIANGLE_INDEX_MASK) != 0)
    {
        NiPoint3* pkPos = NULL;
        NiPoint3* pkNorm = NULL;
        NiPoint3* pkBinorm = NULL;
        NiPoint3* pkTan = NULL;

        unsigned int uiPackedSize = 0;

        for (i = 0; i < uiNumStreams; i++)
        {
            // Lock VB
            NiD3D10DataStream* pkCurrentVB = 
                pkD3D10GeomData->GetVBDataStream(i);
            NIASSERT(pkCurrentVB && pkCurrentVB->GetSize() >= 
                usMaxVertexCount * pkVertexDescription->GetVertexStride(i));

            void* pvBuffer = pkCurrentVB->Lock(NiD3D10DataStream::LOCK_WRITE);

            const unsigned int uiEntryCount = 
                pkVertexDescription->GetEntryCount(i);
            for (unsigned int j = 0; j < uiEntryCount; j++)
            {
                const NiShaderDeclaration::ShaderRegisterEntry* pkEntry = 
                    pkVertexDescription->GetEntry(j, i);

                void* pvDest = (char*)pvBuffer + 
                    pkVertexDescription->GetPackingOffset(i, j);
                const unsigned int uiDestStride = 
                    pkVertexDescription->GetVertexStride(i);

                bool bPack = false;

                switch (pkEntry->m_eInput)
                {
                case NiShaderDeclaration::SHADERPARAM_NI_POSITION:
                    if ((usDirtyFlags & NiGeometryData::VERTEX_MASK) != 0)
                    {
                        NIASSERT(pkEntry->m_eType == 
                            NiShaderDeclaration::SPTYPE_FLOAT3);
                        bPack = true;
                    }
                    break;
                case NiShaderDeclaration::SHADERPARAM_NI_NORMAL:
                    if ((usDirtyFlags & NiGeometryData::NORMAL_MASK) != 0)
                    {
                        bNorms = true;
                        NIASSERT(pkEntry->m_eType == 
                            NiShaderDeclaration::SPTYPE_FLOAT3);
                        bPack = true;
                    }
                    break;
                case NiShaderDeclaration::SHADERPARAM_NI_BINORMAL:
                    if ((usDirtyFlags & NiGeometryData::NORMAL_MASK) != 0)
                    {
                        bBinorms = true;
                        NIASSERT(pkEntry->m_eType == 
                            NiShaderDeclaration::SPTYPE_FLOAT3);
                        bPack = true;
                    }
                    break;
                case NiShaderDeclaration::SHADERPARAM_NI_TANGENT:
                    if ((usDirtyFlags & NiGeometryData::NORMAL_MASK) != 0)
                    {
                        bTangents = true;
                        NIASSERT(pkEntry->m_eType == 
                            NiShaderDeclaration::SPTYPE_FLOAT3);
                        bPack = true;
                    }
                    break;
                case NiShaderDeclaration::SHADERPARAM_NI_COLOR:
                    if ((usDirtyFlags & NiGeometryData::COLOR_MASK) != 0)
                    {
                        bColors = true;
                        bPack = true;
                    }
                    break;
                case NiShaderDeclaration::SHADERPARAM_NI_TEXCOORD0:
                case NiShaderDeclaration::SHADERPARAM_NI_TEXCOORD1:
                case NiShaderDeclaration::SHADERPARAM_NI_TEXCOORD2:
                case NiShaderDeclaration::SHADERPARAM_NI_TEXCOORD3:
                case NiShaderDeclaration::SHADERPARAM_NI_TEXCOORD4:
                case NiShaderDeclaration::SHADERPARAM_NI_TEXCOORD5:
                case NiShaderDeclaration::SHADERPARAM_NI_TEXCOORD6:
                case NiShaderDeclaration::SHADERPARAM_NI_TEXCOORD7:
                    if ((usDirtyFlags & NiGeometryData::TEXTURE_MASK) != 0)
                    {
                        uiTexCoords++;
                         bPack = true;
                    }
                    break;
                default:
                    // Pack everything else
                    bPack = true;
                    break;
                }

                if (bPack)
                {
                    uiPackedSize += m_pkGeometryPacker->PackEntry(
                        usVertexCount, pkData, NULL, pkEntry, uiDestStride, 
                        pvDest, 0, NULL);
                }
                else
                {
                    uiPackedSize += NiD3D10VertexDescription::GetSPTypeSize(
                        pkEntry->m_eType) * usVertexCount;
                }
            }

            // Unlock VB
            pkCurrentVB->Unlock();
        }

        pkD3D10GeomData->UpdateVBs();
    }

    // Set flags on geometry renderer data
    if (pkD3D10GeomData->GetFlags() == 0)
    {
        pkD3D10GeomData->SetFlags(
            NiD3D10GeometryData::CreateVertexFlags(bColors, bNorms, 
            bBinorms && bTangents, uiTexCoords));
    }

    // Create index buffers, if necessary

    // Note that NiGeometry has a limit of 16-bit index buffers
    NiD3D10DataStream* pkIBDataStream = 
        pkD3D10GeomData->GetIBDataStream(0);
    const unsigned int uiIBSize = uiMaxIndexCount * sizeof(unsigned short);
    if (pkIBDataStream == NULL || pkIBDataStream->GetSize() < uiIBSize)
    {
        // All primitives share same IB data stream.

        NiD3D10DataStreamPtr spNewIB;
        // Use same consistency for index buffers as we do for 
        // vertex buffers
        NIVERIFY(NiD3D10DataStream::Create(uiIBSize, uiAccessFlags, 
            NiD3D10DataStream::USAGE_INDEX, spNewIB));
        NIASSERT(spNewIB);

        // Set new IBs, which should release existing ones
        for (i = 0; i < uiTotalPrimitiveCount; i++)
        {
            pkD3D10GeomData->SetIBDataStream(i, spNewIB);
            pkD3D10GeomData->SetIBFormat(i, DXGI_FORMAT_R16_UINT);
        }

        // Had to create new index buffers - mark them as dirty
        usDirtyFlags |= NiTriBasedGeomData::TRIANGLE_INDEX_MASK | 
            NiTriBasedGeomData::TRIANGLE_COUNT_MASK;
    }

    // Pack index buffer
    // Always repack if TRIANGLE_COUNT_MASK is set, even if fewer
    // triangles are used.
    if ((usDirtyFlags & (NiTriBasedGeomData::TRIANGLE_INDEX_MASK | 
        NiTriBasedGeomData::TRIANGLE_COUNT_MASK)) != 0)
    {
        // Lock IB
        NiD3D10DataStream* pkCurrentIB = 
            pkD3D10GeomData->GetIBDataStream(0);

        NIASSERT(pkCurrentIB && pkCurrentIB->GetSize() >= 
            uiMaxIndexCount * sizeof(unsigned short));

        void* pvBuffer = pkCurrentIB->Lock(NiD3D10DataStream::LOCK_WRITE);

        unsigned short* pusTemp = (unsigned short*)pvBuffer;
        const unsigned int uiMaxLinesMinusOne = usMaxVertexCount - 1;
        const NiBool* pkFlagIter = pkFlags;
        for (i = 0; i < uiMaxLinesMinusOne; i++)
        {
            if (*pkFlagIter++ != 0)
            {
                *(pusTemp++) = i;
                *(pusTemp++) = i + 1;
            }
        }
        if (*pkFlagIter)
        {
            *(pusTemp++) = uiMaxLinesMinusOne;
            *(pusTemp++) = 0;
        }

        // Unlock IB
        pkCurrentIB->Unlock();

        pkD3D10GeomData->UpdateIBs();
    }

    pkData->ClearRevisionID();

    return true;
}
//---------------------------------------------------------------------------
NiShader* NiD3D10Renderer::GetFragmentShader(
    NiMaterialDescriptor* pkMaterialDescriptor)
{
    return NiNew NiD3D10FragmentShader(pkMaterialDescriptor);
}
//---------------------------------------------------------------------------
NiShader* NiD3D10Renderer::GetShadowWriteShader(
    NiMaterialDescriptor* pkMaterialDescriptor)
{
    return NiNew NiD3D10ShadowWriteShader(pkMaterialDescriptor);
}
//---------------------------------------------------------------------------
void NiD3D10Renderer::SetDefaultProgramCache(NiFragmentMaterial* pkMaterial, 
    bool bAutoWriteToDisk, bool bWriteDebugFile, bool bLoad,
    bool bNoNewProgramCreation, const char* pcWorkingDir)
{
    NIASSERT(m_pkD3D10Device)
    const char* pcProfile = D3D10GetVertexShaderProfile(m_pkD3D10Device);
    NiGPUProgram::ProgramType eType = NiGPUProgram::PROGRAM_VERTEX;

    if (pcWorkingDir == NULL)
        pcWorkingDir = NiMaterial::GetDefaultWorkingDirectory();

    NiD3D10GPUProgramCache* pkCache = 
        NiNew NiD3D10GPUProgramCache(
        pkMaterial->GetProgramVersion(eType),
        pcWorkingDir, eType, pcProfile, 
        pkMaterial->GetName(), bAutoWriteToDisk,
        bWriteDebugFile, bNoNewProgramCreation, bLoad);
    pkMaterial->SetProgramCache(pkCache, eType);

    pcProfile = D3D10GetGeometryShaderProfile(m_pkD3D10Device);
    eType = NiGPUProgram::PROGRAM_GEOMETRY;
    pkCache = 
        NiNew NiD3D10GPUProgramCache(
        pkMaterial->GetProgramVersion(eType),
        pcWorkingDir, eType, pcProfile, 
        pkMaterial->GetName(), bAutoWriteToDisk,
        bWriteDebugFile, bNoNewProgramCreation, bLoad);
    pkMaterial->SetProgramCache(pkCache, eType);

    pcProfile = D3D10GetPixelShaderProfile(m_pkD3D10Device);
    eType = NiGPUProgram::PROGRAM_PIXEL;
    pkCache = 
        NiNew NiD3D10GPUProgramCache(
        pkMaterial->GetProgramVersion(eType),
        pcWorkingDir, eType, pcProfile, 
        pkMaterial->GetName(), bAutoWriteToDisk,
        bWriteDebugFile, bNoNewProgramCreation, bLoad);
    pkMaterial->SetProgramCache(pkCache, eType);
}
//---------------------------------------------------------------------------
const D3DXMATRIXA16& NiD3D10Renderer::GetProjectionMatrix() const
{
    return m_kD3DProj;
}
//---------------------------------------------------------------------------
const D3DXMATRIXA16& NiD3D10Renderer::GetViewMatrix() const
{
    return m_kD3DView;
}
//---------------------------------------------------------------------------
const D3DXMATRIXA16& NiD3D10Renderer::GetInverseViewMatrix() const
{
    return m_kInvView;
}
//---------------------------------------------------------------------------
const D3DXMATRIXA16& NiD3D10Renderer::GetWorldMatrix() const
{
    return m_kD3DModel;
}
//---------------------------------------------------------------------------
void NiD3D10Renderer::GetCameraNearAndFar(float& fNear, float& fFar) const
{
    fNear = m_kCurrentFrustum.m_fNear;
    fFar = m_kCurrentFrustum.m_fFar;
}
//---------------------------------------------------------------------------
bool NiD3D10Renderer::Do_BeginFrame()
{
    if (m_bDeviceRemoved)
    {
        return false;
    }
    else if (m_bDeviceOccluded)
    {
        // Check device state only when the device is already occluded.
        if (m_spDefaultRenderTargetGroup == NULL)
            return false;
        NIASSERT(m_spDefaultRenderTargetGroup->GetBuffer(0));
        Ni2DBuffer::RendererData* pkBackBufferData = 
            m_spDefaultRenderTargetGroup->GetBufferRendererData(0);
        HandleDisplayFrameResult(
            ((NiD3D102DBufferData*)pkBackBufferData)->DisplayFrame(0, true));
        
        if (m_bDeviceOccluded)
            return false;
    }

    while (m_kBuffersToUseAtDisplayFrame.GetSize())
        m_kBuffersToUseAtDisplayFrame.RemoveHead();

    return true;
}
//---------------------------------------------------------------------------
bool NiD3D10Renderer::Do_EndFrame()
{
    return true;
}
//---------------------------------------------------------------------------
bool NiD3D10Renderer::Do_DisplayFrame()
{
    while (m_kBuffersToUseAtDisplayFrame.GetSize())
    {
        NiD3D102DBufferDataPtr spBuffer = 
            m_kBuffersToUseAtDisplayFrame.RemoveHead();

        HRESULT hr = spBuffer->DisplayFrame(m_uiSyncInterval);
        if (HandleDisplayFrameResult(hr) == false)
            return false;
    }
    return true;
}
//---------------------------------------------------------------------------
bool NiD3D10Renderer::HandleDisplayFrameResult(HRESULT hr)
{
    if (hr == DXGI_STATUS_OCCLUDED)
    {
        if (m_bDeviceOccluded == false)
        {
            // Signal application that it is now occluded
            const unsigned int uiFuncCount = m_kOccludedNotifyFuncs.GetSize();
            for (unsigned int i = 0; i < uiFuncCount; i++)
            {
                OCCLUDEDNOTIFYFUNC pfnFunc = m_kOccludedNotifyFuncs.GetAt(i);
                void* pvData = m_kOccludedNotifyFuncData.GetAt(i);
                if (pfnFunc)
                {
                    bool bResult = (*pfnFunc)(true, pvData);

                    if (bResult == false)
                    {
                        return false;
                    }
                }
            }

            m_bDeviceOccluded = true;
        }
    }
    else if (FAILED(hr))
    {
        if (hr == DXGI_ERROR_DEVICE_REMOVED)
        {
            hr = m_pkD3D10Device->GetDeviceRemovedReason();
            if (hr == DXGI_ERROR_DEVICE_REMOVED)
            {
                NiD3D10Error::ReportWarning(
                    "ID3D10Device::GetDeviceRemovedReason reports the "
                    "device has been removed. The D3D10 device must be "
                    "shut down and recreated, though the original "
                    "adapter may be missing.");
            }
            else if (hr == DXGI_ERROR_DEVICE_HUNG)
            {
                NiD3D10Error::ReportWarning(
                    "ID3D10Device::GetDeviceRemovedReason reports the "
                    "device has hung. The D3D10 device must be shut down "
                    "and recreated. The application can continue as "
                    "usual, but it is recommended that the graphics usage "
                    "be scaled back.");
            }
            else if (hr == DXGI_ERROR_DEVICE_RESET)
            {
                NiD3D10Error::ReportWarning(
                    "ID3D10Device::GetDeviceRemovedReason reports the "
                    "device has crashed and been reset by another "
                    "application. The D3D10 device must be shut down and "
                    "recreated.");
            }
        }
        else
        {
            NiD3D10Error::ReportError(
                NiD3D10Error::NID3D10ERROR_DISPLAY_SWAP_CHAIN_FAILED,
                "Error HRESULT = 0x%08X.", (unsigned int)hr);
        }
        if (m_bDeviceRemoved == false)
        {
            const unsigned int uiFuncCount = 
                m_kDeviceRemovedNotifyFuncs.GetSize();
            for (unsigned int i = 0; i < uiFuncCount; i++)
            {
                DEVICEREMOVEDNOTIFYFUNC pfnFunc = 
                    m_kDeviceRemovedNotifyFuncs.GetAt(i);
                void* pvData = m_kDeviceRemovedNotifyFuncData.GetAt(i);
                if (pfnFunc)
                {
                    bool bResult = (*pfnFunc)(pvData);

                    if (bResult == false)
                    {
                        return false;
                    }
                }
            }
            m_bDeviceRemoved = true;
        }
        return false;
    }
    else if (m_bDeviceOccluded)
    {
        // Signal application that it is no longer occluded
        const unsigned int uiFuncCount = 
            m_kOccludedNotifyFuncs.GetSize();
        for (unsigned int i = 0; i < uiFuncCount; i++)
        {
            OCCLUDEDNOTIFYFUNC pfnFunc = m_kOccludedNotifyFuncs.GetAt(i);
            void* pvData = m_kOccludedNotifyFuncData.GetAt(i);
            if (pfnFunc)
            {
                bool bResult = (*pfnFunc)(false, pvData);

                if (bResult == false)
                {
                    return false;
                }
            }
        }
        m_bDeviceOccluded = false;
    }

    return true;
}
//---------------------------------------------------------------------------
void NiD3D10Renderer::Do_ClearBuffer(const NiRect<float>* pkR, 
    unsigned int uiMode)
{
    if (m_pkD3D10Device == NULL)
    {
        NiD3D10Error::ReportWarning("Can't call ClearBuffer without "
            "a valid device.");
        return;
    }

    // This should be verified in ClearBuffer
    NIASSERT(m_pkCurrentRenderTargetGroup);

    if ((uiMode & NiRenderer::CLEAR_BACKBUFFER) != 0)
    {
        for (unsigned int i = 0; 
            i < D3D10_SIMULTANEOUS_RENDER_TARGET_COUNT; i++)
        {
            Ni2DBuffer* pk2DBuffer = 
                m_pkCurrentRenderTargetGroup->GetBuffer(i);
            if (pk2DBuffer == NULL)
                continue;

            NiD3D102DBufferData* pk2DBufferData = 
                (NiD3D102DBufferData*)pk2DBuffer->GetRendererData();
            NIASSERT(NiIsKindOf(NiD3D10RenderTargetBufferData, 
                pk2DBufferData));

            NiD3D10RenderTargetBufferData* pkRTBufferData = 
                (NiD3D10RenderTargetBufferData*)pk2DBufferData;

            ID3D10RenderTargetView* pkRTView = 
                pkRTBufferData->GetRenderTargetView();
            if (pkRTView)
            {
                m_pkD3D10Device->ClearRenderTargetView(pkRTView, 
                    m_afBackgroundColor);
            }
        }
    }


    unsigned int uiDSClearModes = 0;
    if ((uiMode & NiRenderer::CLEAR_ZBUFFER) != 0)
        uiDSClearModes |= D3D10_CLEAR_DEPTH;
    if ((uiMode & NiRenderer::CLEAR_STENCIL) != 0)
        uiDSClearModes |= D3D10_CLEAR_STENCIL;

    if (uiDSClearModes != 0)
    {
        NiDepthStencilBuffer* pkDSBuffer = 
            m_pkCurrentRenderTargetGroup->GetDepthStencilBuffer();
        if (pkDSBuffer)
        {
            NiD3D102DBufferData* pk2DBufferData = 
                (NiD3D102DBufferData*)pkDSBuffer->GetRendererData();
            NIASSERT(NiIsKindOf(NiD3D10DepthStencilBufferData, 
                pk2DBufferData));

            NiD3D10DepthStencilBufferData* pkDSBufferData = 
                (NiD3D10DepthStencilBufferData*)pk2DBufferData;

            ID3D10DepthStencilView* pkDSView = 
                pkDSBufferData->GetDepthStencilView();
            if (pkDSView)
            {
                m_pkD3D10Device->ClearDepthStencilView(pkDSView,
                    uiDSClearModes, m_fDepthClear, m_ucStencilClear);
            }
        }
    }
}
//---------------------------------------------------------------------------
void NiD3D10Renderer::Do_SetCameraData(const NiPoint3& kWorldLoc,
    const NiPoint3& kWorldDir, const NiPoint3& kWorldUp, 
    const NiPoint3& kWorldRight, const NiFrustum& kFrustum, 
    const NiRect<float>& kPort)
{
    // This should be verified in SetCameraData
    NIASSERT(m_pkCurrentRenderTargetGroup);

    if (m_pkD3D10Device == NULL)
    {
        NiD3D10Error::ReportWarning("Can't call SetCameraData without "
            "a valid device.");
        return;
    }

    // View matrix update
    m_kD3DView._11 = kWorldRight.x;    
    m_kD3DView._12 = kWorldUp.x;    
    m_kD3DView._13 = kWorldDir.x;
    m_kD3DView._14 = 0.0f;
    m_kD3DView._21 = kWorldRight.y; 
    m_kD3DView._22 = kWorldUp.y;    
    m_kD3DView._23 = kWorldDir.y;
    m_kD3DView._24 = 0.0f;
    m_kD3DView._31 = kWorldRight.z; 
    m_kD3DView._32 = kWorldUp.z;    
    m_kD3DView._33 = kWorldDir.z;
    m_kD3DView._34 = 0.0f;
    m_kD3DView._41 = -(kWorldRight * kWorldLoc);
    m_kD3DView._42 = -(kWorldUp * kWorldLoc);
    m_kD3DView._43 = -(kWorldDir * kWorldLoc);
    m_kD3DView._44 = 1.0f;

    m_kInvView._11 = kWorldRight.x;
    m_kInvView._12 = kWorldRight.y;
    m_kInvView._13 = kWorldRight.z;
    m_kInvView._14 = 0.0f;
    m_kInvView._21 = kWorldUp.x;
    m_kInvView._22 = kWorldUp.y;
    m_kInvView._23 = kWorldUp.z;
    m_kInvView._24 = 0.0f;
    m_kInvView._31 = kWorldDir.x;
    m_kInvView._32 = kWorldDir.y;
    m_kInvView._33 = kWorldDir.z;
    m_kInvView._34 = 0.0f;
    m_kInvView._41 = kWorldLoc.x;
    m_kInvView._42 = kWorldLoc.y;
    m_kInvView._43 = kWorldLoc.z;
    m_kInvView._44 = 1.0f;

    float fDepthRange = kFrustum.m_fFar - kFrustum.m_fNear;

    // Projection matrix update
    float fRmL = kFrustum.m_fRight - kFrustum.m_fLeft;
    float fRpL = kFrustum.m_fRight + kFrustum.m_fLeft;
    float fTmB = kFrustum.m_fTop - kFrustum.m_fBottom;
    float fTpB = kFrustum.m_fTop + kFrustum.m_fBottom;
    float fInvFmN = 1.0f / fDepthRange;

    if (kFrustum.m_bOrtho)
    {
        if (m_bLeftRightSwap)
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
        if (m_bLeftRightSwap)
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

    // Viewport update

    D3D10_VIEWPORT kViewPort;
    float fWidth = (float)m_pkCurrentRenderTargetGroup->GetWidth(0);
    float fHeight = (float)m_pkCurrentRenderTargetGroup->GetHeight(0);

    kViewPort.TopLeftX = (unsigned int)(kPort.m_left * fWidth);
    kViewPort.TopLeftY = (unsigned int)((1.0f - kPort.m_top) * fHeight);
    kViewPort.Width = 
        (unsigned int)((kPort.m_right - kPort.m_left) * fWidth);
    kViewPort.Height = 
        (unsigned int)((kPort.m_top - kPort.m_bottom) * fHeight);
    kViewPort.MinDepth = 0.0f;     
    kViewPort.MaxDepth = 1.0f;

    m_pkD3D10Device->RSSetViewports(1, &kViewPort);

    m_kCurrentFrustum = kFrustum;
}
//---------------------------------------------------------------------------
void NiD3D10Renderer::Do_SetScreenSpaceCameraData(const NiRect<float>* pkPort)
{
    // This should be verified in SetScreenSpaceCameraData
    NIASSERT(m_pkCurrentRenderTargetGroup);

    if (m_pkD3D10Device == NULL)
    {
        NiD3D10Error::ReportWarning("Can't call SetScreenSpaceCameraData "
            "without a valid device.");
        return;
    }

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

    unsigned int uiWidth = m_pkCurrentRenderTargetGroup->GetWidth(0);
    unsigned int uiHeight = m_pkCurrentRenderTargetGroup->GetHeight(0);
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

    // Viewport update
    D3D10_VIEWPORT kViewPort;

    if (pkPort)
    {
        kViewPort.TopLeftX = (unsigned int)(pkPort->m_left * fWidth);
        kViewPort.TopLeftY = (unsigned int)((1.0f - pkPort->m_top) * fHeight);
        kViewPort.Width = 
            (unsigned int)((pkPort->m_right - pkPort->m_left) * fWidth);
        kViewPort.Height = 
            (unsigned int)((pkPort->m_top - pkPort->m_bottom) * fHeight);

        // Set cached port.
        m_kCurrentViewPort = *pkPort;
    }
    else
    {
        kViewPort.TopLeftX = 0;
        kViewPort.TopLeftY = 0;
        kViewPort.Width = uiWidth;
        kViewPort.Height = uiHeight;

        // Set cached port.
        m_kCurrentViewPort = NiRect<float>(0.0f, 1.0f, 1.0f, 0.0f);
    }

    kViewPort.MinDepth = 0.0f;     
    kViewPort.MaxDepth = 1.0f;

    m_pkD3D10Device->RSSetViewports(1, &kViewPort);

    // Set cached frustum.
    const float fInvDoubleWidth = 1.0f / (2.0f * fWidth);
    const float fInvDoubleHeight = 1.0f / (2.0f * fHeight);
    m_kCurrentFrustum.m_fLeft = -0.5f + fInvDoubleWidth;
    m_kCurrentFrustum.m_fRight = 0.5f + fInvDoubleWidth;
    m_kCurrentFrustum.m_fTop = 0.5f - fInvDoubleHeight;
    m_kCurrentFrustum.m_fBottom = -0.5f - fInvDoubleHeight;
    m_kCurrentFrustum.m_fNear = 1.0f;
    m_kCurrentFrustum.m_fFar = 10000.0f;
    m_kCurrentFrustum.m_bOrtho = true;
}
//---------------------------------------------------------------------------
void NiD3D10Renderer::Do_GetCameraData(NiPoint3& kWorldLoc, 
    NiPoint3& kWorldDir, NiPoint3& kWorldUp, NiPoint3& kWorldRight, 
    NiFrustum& kFrustum, NiRect<float>& kPort)
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

    kFrustum = m_kCurrentFrustum;
    kPort = m_kCurrentViewPort;
}
//---------------------------------------------------------------------------
bool NiD3D10Renderer::Do_BeginUsingRenderTargetGroup(
    NiRenderTargetGroup* pkTarget, unsigned int uiClearMode)
{
    if (m_pkD3D10Device == NULL)
    {
        NiD3D10Error::ReportWarning("Can't call BeginUsingRenderTargetGroup "
            "without a valid device.");
        return false;
    }

    // This should have been checked in BeginUsingRenderTargetGroup
    NIASSERT(pkTarget);

    NIASSERT(ValidateRenderTargetGroup(pkTarget));

    ID3D10RenderTargetView* apkRTViews[D3D10_SIMULTANEOUS_RENDER_TARGET_COUNT];
    memset(apkRTViews, 0, sizeof(apkRTViews));

    for (unsigned int i = 0; i < D3D10_SIMULTANEOUS_RENDER_TARGET_COUNT; i++)
    {
        Ni2DBuffer* pk2DBuffer = pkTarget->GetBuffer(i);
        if (pk2DBuffer == NULL)
            continue;

        NiD3D102DBufferData* pk2DBufferData = 
            (NiD3D102DBufferData*)pk2DBuffer->GetRendererData();
        NIASSERT(NiIsKindOf(NiD3D10RenderTargetBufferData, 
            pk2DBufferData));

        NiD3D10RenderTargetBufferData* pkRTBufferData = 
            (NiD3D10RenderTargetBufferData*)pk2DBufferData;

        apkRTViews[i] = pkRTBufferData->GetRenderTargetView();
    }

    ID3D10DepthStencilView* pkDSView = NULL;
    NiDepthStencilBuffer* pkDSBuffer = pkTarget->GetDepthStencilBuffer();
    if (pkDSBuffer)
    {
        NiD3D102DBufferData* pk2DBufferData = 
            (NiD3D102DBufferData*)pkDSBuffer->GetRendererData();
        NIASSERT(NiIsKindOf(NiD3D10DepthStencilBufferData, 
            pk2DBufferData));

        NiD3D10DepthStencilBufferData* pkDSBufferData = 
            (NiD3D10DepthStencilBufferData*)pk2DBufferData;

        pkDSView = pkDSBufferData->GetDepthStencilView();
    }

    m_pkD3D10Device->OMSetRenderTargets(D3D10_SIMULTANEOUS_RENDER_TARGET_COUNT,
        apkRTViews, pkDSView);

    m_pkCurrentRenderTargetGroup = pkTarget;

    Do_ClearBuffer(NULL, uiClearMode);

    return true;
}
//---------------------------------------------------------------------------
bool NiD3D10Renderer::Do_EndUsingRenderTargetGroup()
{
    if (m_pkD3D10Device == NULL)
    {
        NiD3D10Error::ReportWarning("Can't call EndUsingRenderTargetGroup "
            "without a valid device.");
        return false;
    }

    // This should have been checked in EndUsingRenderTargetGroup
    NIASSERT(m_pkCurrentRenderTargetGroup);

    for (unsigned int i = 0; i < 
        m_pkCurrentRenderTargetGroup->GetBufferCount(); i++)
    {
        NiD3D102DBufferData* pkBuffData = (NiD3D102DBufferData*)
            m_pkCurrentRenderTargetGroup->GetBufferRendererData(i);

        if (pkBuffData == NULL)
            continue;

        // Store for DisplayFrame
        if (pkBuffData->CanDisplayFrame())
        {
            // Only store once
            if (!m_kBuffersToUseAtDisplayFrame.FindPos(pkBuffData))
                m_kBuffersToUseAtDisplayFrame.AddTail(pkBuffData);
        }
    }

    m_pkCurrentRenderTargetGroup = NULL;
    return true;
}
//---------------------------------------------------------------------------
void NiD3D10Renderer::Do_BeginBatch(NiPropertyState* pkPropertyState,
   NiDynamicEffectState* pkEffectState)
{
    m_pkCurrProp = pkPropertyState;
    m_pkCurrEffects = pkEffectState;
    NIASSERT(m_spBatchMaterial == NULL);
}
//---------------------------------------------------------------------------
void NiD3D10Renderer::Do_EndBatch()
{
    m_spBatchMaterial = NULL;
}
//---------------------------------------------------------------------------
void NiD3D10Renderer::Do_BatchRenderShape(NiTriShape* pkTriShape)
{
    NiMaterialPtr spCurrentMaterial = 
        (NiMaterial*)pkTriShape->GetActiveMaterial();
    unsigned int uiMaterialData = NiMaterialInstance::DEFAULT_EXTRA_DATA;
    const NiMaterialInstance* pkMatInstance = 
        pkTriShape->GetActiveMaterialInstance();
    if (pkMatInstance)
        uiMaterialData = pkMatInstance->GetMaterialExtraData();

    if (m_spBatchMaterial == NULL)
    {
        m_spBatchMaterial = spCurrentMaterial;
        if (m_spBatchMaterial == NULL)
        {
            pkTriShape->ApplyAndSetActiveMaterial(m_spCurrentDefaultMaterial);
            m_spBatchMaterial = m_spCurrentDefaultMaterial;
        }
    }
    else
    {
        pkTriShape->ApplyAndSetActiveMaterial(m_spBatchMaterial);
    }

    Do_RenderShape(pkTriShape);

    pkTriShape->ApplyAndSetActiveMaterial(spCurrentMaterial, uiMaterialData);
}
//---------------------------------------------------------------------------
void NiD3D10Renderer::Do_BatchRenderStrips(NiTriStrips* pkTriStrips)
{
    NiMaterialPtr spCurrentMaterial = 
        (NiMaterial*)pkTriStrips->GetActiveMaterial();
    unsigned int uiMaterialData = NiMaterialInstance::DEFAULT_EXTRA_DATA;
    const NiMaterialInstance* pkMatInstance = 
        pkTriStrips->GetActiveMaterialInstance();
    if (pkMatInstance)
        uiMaterialData = pkMatInstance->GetMaterialExtraData();

    if (m_spBatchMaterial == NULL)
    {
        m_spBatchMaterial = spCurrentMaterial;
        if (m_spBatchMaterial == NULL)
        {
            pkTriStrips->ApplyAndSetActiveMaterial(m_spCurrentDefaultMaterial);
            m_spBatchMaterial = m_spCurrentDefaultMaterial;
        }
    }
    else
    {
        pkTriStrips->ApplyAndSetActiveMaterial(m_spBatchMaterial);
    }

    Do_RenderTristrips(pkTriStrips);

    pkTriStrips->ApplyAndSetActiveMaterial(spCurrentMaterial, uiMaterialData);
}
//---------------------------------------------------------------------------
void NiD3D10Renderer::Do_RenderShape(NiTriShape* pkTriShape)
{
    if (m_pkD3D10Device == NULL)
    {
        NiD3D10Error::ReportWarning("Can't call RenderShape without "
            "a valid device.");
        return;
    }

    NIASSERT(pkTriShape->GetModelData());
    if (pkTriShape->GetActiveVertexCount() == 0)
        return;

    Render(pkTriShape, RENDER_SHAPE);
}
//---------------------------------------------------------------------------
void NiD3D10Renderer::Do_RenderTristrips(NiTriStrips* pkTriStrips)
{
    if (m_pkD3D10Device == NULL)
    {
        NiD3D10Error::ReportWarning("Can't call RenderShape without "
            "a valid device.");
        return;
    }

    NiGeometryData* pkData = pkTriStrips->GetModelData();
    NIASSERT(pkTriStrips->GetModelData());
    if (pkTriStrips->GetActiveVertexCount() == 0)
        return;

    Render(pkTriStrips, RENDER_STRIPS);
}
//---------------------------------------------------------------------------
void NiD3D10Renderer::Do_RenderPoints(NiParticles* pkParticles)
{
    if (m_pkD3D10Device == NULL)
    {
        NiD3D10Error::ReportWarning("Can't call RenderShape without "
            "a valid device.");
        return;
    }

    NiGeometryData* pkData = pkParticles->GetModelData();
    NIASSERT(pkParticles->GetModelData());
    if (pkParticles->GetActiveVertexCount() == 0)
        return;

    Render(pkParticles, RENDER_POINTS);
}
//---------------------------------------------------------------------------
void NiD3D10Renderer::Do_RenderLines(NiLines* pkLines)
{
    if (m_pkD3D10Device == NULL)
    {
        NiD3D10Error::ReportWarning("Can't call RenderShape without "
            "a valid device.");
        return;
    }

    NiGeometryData* pkData = pkLines->GetModelData();
    NIASSERT(pkLines->GetModelData());
    if (pkLines->GetActiveVertexCount() == 0)
        return;

    Render(pkLines, RENDER_LINES);
}
//---------------------------------------------------------------------------
void NiD3D10Renderer::Do_RenderScreenTexture(NiScreenTexture* pkScreenTexture)
{
    if (m_pkD3D10Device == NULL)
    {
        NiD3D10Error::ReportWarning("Can't call RenderShape without "
            "a valid device.");
        return;
    }

    if (pkScreenTexture == NULL || pkScreenTexture->GetNumScreenRects() == 0)
    {
        return;
    }

    TransferScreenTextureToScreenElements(pkScreenTexture);

    // Apply texturing property from property state
    NiPropertyStatePtr spPropState = m_spSTScreenElements->GetPropertyState();
    spPropState->SetProperty(pkScreenTexture->GetTexturingProperty()); 

    // Temporarily disable depth clip enable
    D3D10_RASTERIZER_DESC kDesc;
    m_pkRenderStateManager->GetDefaultRasterizerStateDesc(kDesc);
    bool bCurrentDepthClipEnable = (kDesc.DepthClipEnable != 0);
    kDesc.DepthClipEnable = false;
    m_pkRenderStateManager->SetDefaultRasterizerStateDesc(kDesc);

    m_spSTScreenElements->Draw(this);

    // Restore depth clip enable
    kDesc.DepthClipEnable = bCurrentDepthClipEnable;
    m_pkRenderStateManager->SetDefaultRasterizerStateDesc(kDesc);

    // Allow for freeing of screen texture
    spPropState->SetProperty(NiTexturingProperty::GetDefault());
}
//---------------------------------------------------------------------------
void NiD3D10Renderer::TransferScreenTextureToScreenElements(
    NiScreenTexture* pkScreenTexture)
{
    /*if (pkScreenTexture == m_pkLastScreenTexture &&
        pkScreenTexture->GetRevisionID() == 0)
    {
        // Existing m_spSTScreenElements is still valid
        NIASSERT(m_spSTScreenElements);
        return;
    }
    m_pkLastScreenTexture = pkScreenTexture;*/

    const unsigned int uiNumScreenRects = pkScreenTexture->GetNumScreenRects();
    // Screen rect count should have already been checked
    NIASSERT(uiNumScreenRects != 0);
    const unsigned int uiNumVerts = uiNumScreenRects * 4;
    const unsigned int uiNumTris = uiNumScreenRects * 2;

    if (m_spSTScreenElements == NULL)
    {
        NiScreenElementsData* pkScreenElementsData = 
            NiNew NiScreenElementsData(false, true, 1, uiNumScreenRects, 1, 
            uiNumVerts, 1, uiNumTris, 1);

        pkScreenElementsData->SetConsistency(NiGeometryData::VOLATILE);

        m_spSTScreenElements = NiNew NiScreenElements(pkScreenElementsData);

        // use vertex colors
        NiVertexColorProperty* pkVertex = NiNew NiVertexColorProperty;
        pkVertex->SetSourceMode(NiVertexColorProperty::SOURCE_EMISSIVE);
        pkVertex->SetLightingMode(NiVertexColorProperty::LIGHTING_E);
        m_spSTScreenElements->AttachProperty(pkVertex);

        // use alpha blending
        NiAlphaProperty* pkAlpha = NiNew NiAlphaProperty;
        pkAlpha->SetAlphaBlending(true);
        pkAlpha->SetSrcBlendMode(NiAlphaProperty::ALPHA_SRCALPHA);
        pkAlpha->SetDestBlendMode(NiAlphaProperty::ALPHA_INVSRCALPHA);
        m_spSTScreenElements->AttachProperty(pkAlpha);

        // disable z-buffering
        NiZBufferProperty* pkZBuf = NiNew NiZBufferProperty;
        pkZBuf->SetZBufferTest(false);
        pkZBuf->SetZBufferWrite(false);
        m_spSTScreenElements->AttachProperty(pkZBuf);

        m_spSTScreenElements->Update(0.0f);
        m_spSTScreenElements->UpdateProperties();
    }
    else
    {
        unsigned int uiCurrentPolygonCount = 
            m_spSTScreenElements->GetMaxPQuantity();
        if (uiNumScreenRects > uiCurrentPolygonCount)
        {
            // Minimize the number of times resizing needs to be done
            m_spSTScreenElements->SetPGrowBy(uiNumScreenRects - 
                uiCurrentPolygonCount);
            m_spSTScreenElements->SetVGrowBy(uiNumVerts - 
                m_spSTScreenElements->GetVertexCount());
            m_spSTScreenElements->SetTGrowBy(uiNumTris - 
                m_spSTScreenElements->GetTriangleCount());
        }

        m_spSTScreenElements->RemoveAll();
    }

    NiScreenElementsData* pkData = 
        (NiScreenElementsData*)m_spSTScreenElements->GetModelData();

    NIASSERT(pkData);

    unsigned short usScreenWidth = GetCurrentRenderTargetGroup()->GetWidth(0);
    unsigned short usScreenHeight = 
        GetCurrentRenderTargetGroup()->GetHeight(0);
    float fInvScreenWidth = 1.0f / (float)usScreenWidth;
    float fInvScreenHeight = 1.0f / (float)usScreenHeight;

    NiTexture* pkTexture = pkScreenTexture->GetTexture();
    unsigned int uiTexWidth = pkTexture->GetWidth();
    unsigned int uiTexHeight = pkTexture->GetHeight();
    float fInvTexWidth = 1.0f / (float)uiTexWidth;
    float fInvTexHeight = 1.0f / (float)uiTexHeight;

    // Fill in polygons
    for (unsigned int i = 0; i < uiNumScreenRects; i++)
    {
        NiScreenTexture::ScreenRect& kRect = 
            pkScreenTexture->GetScreenRect(i);
        int iIndex = pkData->Insert(4);

        // Ensure screen location is on screen
        short sPixLeft = kRect.m_sPixLeft;
        while (sPixLeft < 0)
            sPixLeft += usScreenWidth;
        while (sPixLeft > usScreenWidth)
            sPixLeft -= usScreenWidth;
        NIASSERT(sPixLeft >= 0 && sPixLeft < usScreenWidth);

        short sPixTop = kRect.m_sPixTop;
        while (sPixTop < 0)
            sPixTop += usScreenHeight;
        while (sPixTop > usScreenHeight)
            sPixTop -= usScreenWidth;
        NIASSERT(sPixTop >= 0 && sPixTop < usScreenHeight);

        pkData->SetRectangle(iIndex, 
            ((float)sPixLeft - 0.5f) * fInvScreenWidth,
            ((float)sPixTop - 0.5f) * fInvScreenHeight, 
            (float)kRect.m_usPixWidth * fInvScreenWidth,
            (float)kRect.m_usPixHeight * fInvScreenHeight);
        pkData->SetColors(i, kRect.m_kColor);
        pkData->SetTextures(i, 0, 
            (float)kRect.m_usTexLeft * fInvTexWidth,
            (float)kRect.m_usTexTop * fInvTexHeight,
            (float)(kRect.m_usTexLeft + kRect.m_usPixWidth) * fInvTexWidth,
            (float)(kRect.m_usTexTop + kRect.m_usPixHeight) * fInvTexHeight);
    }

    pkScreenTexture->ClearRevisionID();
}
//---------------------------------------------------------------------------
void NiD3D10Renderer::Render(NiGeometry* pkGeometry, RenderPath ePath)
{
    NIASSERT(m_pkD3D10Device && pkGeometry && pkGeometry->GetModelData());

    if (m_pkCurrProp == NULL)
    {
        NiD3D10Error::ReportWarning("No property state found for object %s; "
            "be sure UpdateProperties has been called on it.\n", 
            pkGeometry->GetName());
        return;
    }

    NiD3D10ShaderInterface* pkShader = NULL;
    if (ePath == RENDER_SHAPE || ePath == RENDER_STRIPS)
    {
        pkShader = NiDynamicCast(NiD3D10ShaderInterface, 
            pkGeometry->GetShaderFromMaterial());
    }

    if (pkShader == 0)
    {
        const NiMaterial* pkTempMat = pkGeometry->GetActiveMaterial();
        pkGeometry->ApplyAndSetActiveMaterial(m_spCurrentDefaultMaterial);
        pkShader = NiDynamicCast(NiD3D10ShaderInterface, 
            pkGeometry->GetShaderFromMaterial());
        pkGeometry->SetActiveMaterial(pkTempMat);
    }

    // If no shader is found, use error shader
    if (pkShader == NULL)
    {
        NiD3D10Error::ReportWarning("No shader found for object %s; "
            "using Error Shader!\n", pkGeometry->GetName());

        NIASSERT(NiIsKindOf(NiD3D10ShaderInterface, GetErrorShader()));
        pkShader = (NiD3D10ShaderInterface*)GetErrorShader();
    }

    NIASSERT(pkShader);
    NIASSERT(m_pkCurrProp);

    NiSkinInstance* pkSkinInstance = pkGeometry->GetSkinInstance();
    NiSkinPartition* pkSkinPartition = (pkSkinInstance ? 
        pkSkinInstance->GetSkinPartition() : NULL);
    const unsigned int uiPartitionCount = (pkSkinPartition ? 
        pkSkinPartition->GetPartitionCount() : 1);
    NiSkinPartition::Partition* pkPartition = (pkSkinPartition ?
        pkSkinPartition->GetPartitions() : NULL);

    const NiTransform& kWorld = pkGeometry->GetWorldTransform();
    const NiBound& kWorldBound = pkGeometry->GetWorldBound();

    // Get the geometry renderer data that corresponds to this skin instance
    NiD3D10GeometryData* pkD3D10GeomData = (NiD3D10GeometryData*)
        pkGeometry->GetModelData()->GetRendererData();
    while (pkD3D10GeomData != NULL && 
        pkD3D10GeomData->GetSkinInstance() != pkSkinInstance)
    {
        pkD3D10GeomData = pkD3D10GeomData->GetNext();
    }

    // If no geometry renderer data object exists, create one, but do not
    // pack it yet.
    if (pkD3D10GeomData == NULL)
        pkD3D10GeomData = CreateGeometryRendererData(pkGeometry);

    // Preprocess the pipeline
    unsigned int uiRet = pkShader->PreProcessPipeline(pkGeometry, 
        pkSkinInstance, pkD3D10GeomData, m_pkCurrProp, m_pkCurrEffects, kWorld,
        kWorldBound);

    if (uiRet != 0)
    {
        NiD3D10Error::ReportWarning("Shader PreProcessPipeline step failed "
            "for object %s; skipping render.\n", pkGeometry->GetName());
        return;
    }

    // Update the pipeline
    uiRet = pkShader->UpdatePipeline(pkGeometry, pkSkinInstance, 
        pkD3D10GeomData, m_pkCurrProp, m_pkCurrEffects, kWorld, kWorldBound);

    unsigned int uiRemainingPasses = pkShader->FirstPass();

    //uiPasses
    while (uiRemainingPasses != 0)
    {
        // Setup the rendering pass
        uiRet = pkShader->SetupRenderingPass(pkGeometry, pkSkinInstance, 
            pkD3D10GeomData, m_pkCurrProp, m_pkCurrEffects, kWorld, 
            kWorldBound);

        for (unsigned int i = 0; i < uiPartitionCount; i++)
        {

            // Set the transformations
            uiRet = pkShader->SetupTransformations(pkGeometry, pkSkinInstance, 
                pkPartition, pkD3D10GeomData, m_pkCurrProp, m_pkCurrEffects, 
                kWorld, kWorldBound);

            if (ePath == RENDER_POINTS)
            {
                // Prepare the geometry for rendering
                pkD3D10GeomData = (NiD3D10GeometryData*)
                    pkShader->PrepareGeometryForRendering_Points(pkGeometry, 
                    pkPartition, pkD3D10GeomData, m_pkCurrProp);
            }
            else if (ePath == RENDER_LINES)
            {
                // Prepare the geometry for rendering
                pkD3D10GeomData = (NiD3D10GeometryData*)
                    pkShader->PrepareGeometryForRendering_Lines(pkGeometry, 
                    pkPartition, pkD3D10GeomData, m_pkCurrProp);
            }
            else
            {
                // Prepare the geometry for rendering
                pkD3D10GeomData = (NiD3D10GeometryData*)
                    pkShader->PrepareGeometryForRendering(pkGeometry, 
                    pkPartition, pkD3D10GeomData, m_pkCurrProp);
            }

            // Set the shader programs
            // This is to give the shader final 'override' authority
            uiRet = pkShader->SetupShaderPrograms(pkGeometry, pkSkinInstance, 
                pkPartition, pkD3D10GeomData, m_pkCurrProp, m_pkCurrEffects, 
                kWorld, kWorldBound);

            m_pkRenderStateManager->ApplyCurrentState();
            m_pkShaderConstantManager->ApplyShaderConstants();

            if (i == 0)
            {
                // This stuff only needs to be done once per geometry

                m_pkD3D10Device->IASetPrimitiveTopology(
                    pkD3D10GeomData->GetPrimitiveTopology());

                m_pkD3D10Device->IASetInputLayout(
                    pkD3D10GeomData->GetInputLayout());
            }
            
            const unsigned int uiPrimitiveStart =
                pkD3D10GeomData->GetPrimitiveStart(i);
            const unsigned int uiPrimitiveCount = 
                pkD3D10GeomData->GetPrimitiveCount(i);
            const unsigned int uiMaxPrimitive = 
                uiPrimitiveStart + uiPrimitiveCount;
            for (unsigned int j = uiPrimitiveStart; j < uiMaxPrimitive; j++)
            {
                m_pkD3D10Device->IASetIndexBuffer(
                    pkD3D10GeomData->GetIB(j),
                    pkD3D10GeomData->GetIBFormat(j),
                    pkD3D10GeomData->GetIBOffset(j));

                m_pkD3D10Device->IASetVertexBuffers(0,
                    pkD3D10GeomData->GetVBCount(),
                    pkD3D10GeomData->GetVBArray(),
                    pkD3D10GeomData->GetVBStrideArray(),
                    pkD3D10GeomData->GetVBOffsetArray(j));

                m_pkD3D10Device->DrawIndexed(
                    pkD3D10GeomData->GetIndexCount(j),
                    pkD3D10GeomData->GetStartIndexLocation(j),
                    pkD3D10GeomData->GetBaseVertexLocation(j));
            }

            // Perform any post-rendering steps
            uiRet = pkShader->PostRender(pkGeometry, pkSkinInstance, 
                pkPartition, pkD3D10GeomData, m_pkCurrProp, m_pkCurrEffects, 
                kWorld, kWorldBound);

            if (pkPartition)
                pkPartition++;
        }

        // Inform the shader to move to the next pass
        uiRemainingPasses = pkShader->NextPass();
    }

    // PostProcess the pipeline
    pkShader->PostProcessPipeline(pkGeometry, pkSkinInstance, pkD3D10GeomData, 
        m_pkCurrProp, m_pkCurrEffects, kWorld, kWorldBound);
}
//---------------------------------------------------------------------------
void NiD3D10Renderer::SetModelTransform(const NiTransform& kWorld)
{
    NiD3D10Utility::GetD3DFromNi(m_kD3DModel, kWorld);
}
//---------------------------------------------------------------------------
void NiD3D10Renderer::CalculateBoneMatrices(
    const NiSkinInstance* pkSkinInstance, const NiTransform& kWorld, 
    bool bTranspose, unsigned int uiBoneMatrixRegisters, 
    bool bWorldSpaceBones)
{
    // Handle case where different shaders are used on same geometry in
    // same frame
    if (pkSkinInstance->GetFrameID() == GetFrameID() &&
        uiBoneMatrixRegisters == pkSkinInstance->GetBoneMatrixRegisters())
    {
        return;
    }

    ((NiSkinInstance*)pkSkinInstance)->SetFrameID(GetFrameID());

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

    if (bWorldSpaceBones)
    {
        D3DXMATRIXA16* pkSkin2World_World2Skin = (D3DXMATRIXA16*)
            pkSkinInstance->GetSkinToWorldWorldToSkinMatrix();
        if (pkSkin2World_World2Skin == NULL)
        {
            pkSkin2World_World2Skin = NiExternalNew D3DXMATRIXA16;
            NIASSERT(pkSkin2World_World2Skin);
            ((NiSkinInstance*)pkSkinInstance)->SetSkinToWorldWorldToSkinMatrix(
                (void*)pkSkin2World_World2Skin);
            pkSkin2World_World2Skin->_14 = 0.0f;
            pkSkin2World_World2Skin->_24 = 0.0f;
            pkSkin2World_World2Skin->_34 = 0.0f;
            pkSkin2World_World2Skin->_44 = 1.0f;
        }

        NiD3D10Utility::GetD3DFromNi(*pkSkin2World_World2Skin,
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

        if (bWorldSpaceBones == false)
            kBoneMatrix = kWorldFromSkinFromWorld * kBoneMatrix;

        if (uiBoneMatrixRegisters == 4)
        {
            D3DXMATRIXA16* pfD3DMat = (D3DXMATRIXA16*)
                &(pfBoneMatrices[4 * uiBoneMatrixRegisters * i]);
            if (bTranspose)
                NiD3D10Utility::GetD3DTransposeFromNi(*pfD3DMat, kBoneMatrix);
            else
                NiD3D10Utility::GetD3DFromNi(*pfD3DMat, kBoneMatrix);
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
}
//---------------------------------------------------------------------------
void NiD3D10Renderer::ReallocateTempArrays(unsigned int uiArraySize)
{
    if (uiArraySize <= m_uiTempArraySize)
        return;

    NiDelete[] m_pkTempVertices;
    m_pkTempVertices = NiNew NiPoint3[uiArraySize];
    NiDelete[] m_pkTempTexCoords;
    m_pkTempTexCoords = NiNew NiPoint2[uiArraySize];
    NiDelete[] m_pkTempColors;
    m_pkTempColors = NiNew NiColorA[uiArraySize];
    m_uiTempArraySize = uiArraySize;
}
//---------------------------------------------------------------------------
HRESULT NiD3D10Renderer::D3D10CreateBlob(SIZE_T NumBytes, 
    LPD3D10BLOB* ppBuffer)
{
    if (ms_hD3D10 == NULL)
        return E_INVALIDARG;

    return ms_pfnD3D10CreateBlob(NumBytes, ppBuffer);
}
//---------------------------------------------------------------------------
HRESULT NiD3D10Renderer::D3D10CreateDevice(IDXGIAdapter* pAdapter, 
    D3D10_DRIVER_TYPE DriverType, HMODULE Software, UINT Flags, 
    UINT SDKVersion, ID3D10Device** ppDevice)
{
    if (ms_hD3D10 == NULL)
        return E_INVALIDARG;

    return ms_pfnD3D10CreateDevice(pAdapter, DriverType, Software, Flags,
        SDKVersion, ppDevice);
}
//---------------------------------------------------------------------------
HRESULT NiD3D10Renderer::D3D10CreateDeviceAndSwapChain(IDXGIAdapter* pAdapter, 
    D3D10_DRIVER_TYPE DriverType, HMODULE Software, UINT Flags, 
    UINT SDKVersion, DXGI_SWAP_CHAIN_DESC* pSwapChainDesc, 
    IDXGISwapChain** ppSwapChain, ID3D10Device** ppDevice)
{
    if (ms_hD3D10 == NULL)
        return E_INVALIDARG;

    return ms_pfnD3D10CreateDeviceAndSwapChain(pAdapter, DriverType, Software,
        Flags, SDKVersion, pSwapChainDesc, ppSwapChain, ppDevice);
}
//---------------------------------------------------------------------------
HRESULT NiD3D10Renderer::D3D10CreateStateBlock(ID3D10Device* pDevice, 
    D3D10_STATE_BLOCK_MASK* pStateBlockMask, ID3D10StateBlock** ppStateBlock)
{
    if (ms_hD3D10 == NULL)
        return E_INVALIDARG;

    return ms_pfnD3D10CreateStateBlock(pDevice, pStateBlockMask, ppStateBlock);
}
//---------------------------------------------------------------------------
HRESULT NiD3D10Renderer::D3D10GetInputAndOutputSignatureBlob(
    CONST void* pShaderBytecode, SIZE_T BytecodeLength, 
    ID3D10Blob** ppSignatureBlob)
{
    if (ms_hD3D10 == NULL)
        return E_INVALIDARG;

    return ms_pfnD3D10GetInputAndOutputSignatureBlob(pShaderBytecode, 
        BytecodeLength, ppSignatureBlob);
}
//---------------------------------------------------------------------------
HRESULT NiD3D10Renderer::D3D10GetInputSignatureBlob(
    CONST void* pShaderBytecode, SIZE_T BytecodeLength, 
    ID3D10Blob** ppSignatureBlob)
{
    if (ms_hD3D10 == NULL)
        return E_INVALIDARG;

    return ms_pfnD3D10GetInputSignatureBlob(pShaderBytecode, BytecodeLength,
        ppSignatureBlob);
}
//---------------------------------------------------------------------------
HRESULT NiD3D10Renderer::D3D10GetOutputSignatureBlob(
    CONST void* pShaderBytecode, SIZE_T BytecodeLength, 
    ID3D10Blob** ppSignatureBlob)
{
    if (ms_hD3D10 == NULL)
        return E_INVALIDARG;

    return ms_pfnD3D10GetOutputSignatureBlob(pShaderBytecode, BytecodeLength,
        ppSignatureBlob);
}
//---------------------------------------------------------------------------
HRESULT NiD3D10Renderer::D3D10StateBlockMaskDifference(
    D3D10_STATE_BLOCK_MASK* pA, D3D10_STATE_BLOCK_MASK* pB, 
    D3D10_STATE_BLOCK_MASK* pResult)
{
    if (ms_hD3D10 == NULL)
        return E_INVALIDARG;

    return ms_pfnD3D10StateBlockMaskDifference(pA, pB, pResult);
}
//---------------------------------------------------------------------------
HRESULT NiD3D10Renderer::D3D10StateBlockMaskDisableAll(
    D3D10_STATE_BLOCK_MASK* pMask)
{
    if (ms_hD3D10 == NULL)
        return E_INVALIDARG;

    return ms_pfnD3D10StateBlockMaskDisableAll(pMask);
}
//---------------------------------------------------------------------------
HRESULT NiD3D10Renderer::D3D10StateBlockMaskDisableCapture(
    D3D10_STATE_BLOCK_MASK* pMask, D3D10_DEVICE_STATE_TYPES StateType, 
    UINT RangeStart, UINT RangeLength)
{
    if (ms_hD3D10 == NULL)
        return E_INVALIDARG;

    return ms_pfnD3D10StateBlockMaskDisableCapture(pMask, StateType, 
        RangeStart, RangeLength);
}
//---------------------------------------------------------------------------
HRESULT NiD3D10Renderer::D3D10StateBlockMaskEnableAll(
    D3D10_STATE_BLOCK_MASK* pMask)
{
    if (ms_hD3D10 == NULL)
        return E_INVALIDARG;

    return ms_pfnD3D10StateBlockMaskEnableAll(pMask);
}
//---------------------------------------------------------------------------
HRESULT NiD3D10Renderer::D3D10StateBlockMaskEnableCapture(
    D3D10_STATE_BLOCK_MASK* pMask, D3D10_DEVICE_STATE_TYPES StateType, 
    UINT RangeStart, UINT RangeLength)
{
    if (ms_hD3D10 == NULL)
        return E_INVALIDARG;

    return ms_pfnD3D10StateBlockMaskEnableCapture(pMask, StateType, RangeStart,
        RangeLength);
}
//---------------------------------------------------------------------------
BOOL NiD3D10Renderer::D3D10StateBlockMaskGetSetting(
    D3D10_STATE_BLOCK_MASK* pMask, D3D10_DEVICE_STATE_TYPES StateType, 
    UINT Entry)
{
    if (ms_hD3D10 == NULL)
        return E_INVALIDARG;

    return ms_pfnD3D10StateBlockMaskGetSetting(pMask, StateType, Entry);
}
//---------------------------------------------------------------------------
HRESULT NiD3D10Renderer::D3D10StateBlockMaskIntersect(
    D3D10_STATE_BLOCK_MASK* pA, D3D10_STATE_BLOCK_MASK* pB, 
    D3D10_STATE_BLOCK_MASK* pResult)
{
    if (ms_hD3D10 == NULL)
        return E_INVALIDARG;

    return ms_pfnD3D10StateBlockMaskIntersect(pA, pB, pResult);
}
//---------------------------------------------------------------------------
HRESULT NiD3D10Renderer::D3D10StateBlockMaskUnion(D3D10_STATE_BLOCK_MASK* pA, 
    D3D10_STATE_BLOCK_MASK* pB, D3D10_STATE_BLOCK_MASK* pResult)
{
    if (ms_hD3D10 == NULL)
        return E_INVALIDARG;

    return ms_pfnD3D10StateBlockMaskUnion(pA, pB, pResult);
}
//---------------------------------------------------------------------------
HRESULT NiD3D10Renderer::D3D10CompileShader(LPCSTR pSrcData, SIZE_T SrcDataLen,
    LPCSTR pFileName, CONST D3D10_SHADER_MACRO* pDefines, 
    LPD3D10INCLUDE pInclude, LPCSTR pFunctionName, LPCSTR pProfile, UINT Flags,
    ID3D10Blob** ppShader, ID3D10Blob** ppErrorMsgs)
{
    if (ms_hD3D10 == NULL)
        return E_INVALIDARG;

    return ms_pfnD3D10CompileShader(pSrcData, SrcDataLen, pFileName, pDefines,
        pInclude, pFunctionName, pProfile, Flags, ppShader, ppErrorMsgs);
}
//---------------------------------------------------------------------------
HRESULT NiD3D10Renderer::D3D10DisassembleShader(CONST UINT* pShader, 
    SIZE_T Size, BOOL EnableColorCode, LPCSTR pComments, 
    ID3D10Blob** ppDisassembly)
{
    if (ms_hD3D10 == NULL)
        return E_INVALIDARG;

    return ms_pfnD3D10DisassembleShader(pShader, Size, EnableColorCode, 
        pComments, ppDisassembly);
}
//---------------------------------------------------------------------------
LPCSTR NiD3D10Renderer::D3D10GetGeometryShaderProfile(ID3D10Device* pDevice)
{
    if (ms_hD3D10 == NULL)
        return NULL;

    return ms_pfnD3D10GetGeometryShaderProfile(pDevice);
}
//---------------------------------------------------------------------------
LPCSTR NiD3D10Renderer::D3D10GetPixelShaderProfile(ID3D10Device* pDevice)
{
    if (ms_hD3D10 == NULL)
        return NULL;

    return ms_pfnD3D10GetPixelShaderProfile(pDevice);
}
//---------------------------------------------------------------------------
LPCSTR NiD3D10Renderer::D3D10GetVertexShaderProfile(ID3D10Device* pDevice)
{
    if (ms_hD3D10 == NULL)
        return NULL;

    return ms_pfnD3D10GetVertexShaderProfile(pDevice);
}
//---------------------------------------------------------------------------
HRESULT NiD3D10Renderer::D3D10PreprocessShader(LPCSTR pSrcData, 
    SIZE_T SrcDataSize, LPCSTR pFileName, CONST D3D10_SHADER_MACRO* pDefines, 
    LPD3D10INCLUDE pInclude, ID3D10Blob** ppShaderText, 
    ID3D10Blob** ppErrorMsgs)
{
    if (ms_hD3D10 == NULL)
        return E_INVALIDARG;

    return ms_pfnD3D10PreprocessShader(pSrcData, SrcDataSize, pFileName, 
        pDefines, pInclude, ppShaderText, ppErrorMsgs);
}
//---------------------------------------------------------------------------
HRESULT NiD3D10Renderer::D3D10ReflectShader(CONST void* pShaderBytecode, 
    SIZE_T BytecodeLength, ID3D10ShaderReflection** ppReflector)
{
    if (ms_hD3D10 == NULL)
        return E_INVALIDARG;

    return ms_pfnD3D10ReflectShader(pShaderBytecode, BytecodeLength, 
        ppReflector);
}
//---------------------------------------------------------------------------
HRESULT NiD3D10Renderer::D3D10CompileEffectFromMemory(void* pData, 
    SIZE_T DataLength, LPCSTR pSrcFileName, CONST D3D10_SHADER_MACRO* pDefines,
    ID3D10Include* pInclude, UINT HLSLFlags, UINT FXFlags, 
    ID3D10Blob** ppCompiledEffect, ID3D10Blob** ppErrors)
{
    if (ms_hD3D10 == NULL)
        return E_INVALIDARG;

    return ms_pfnD3D10CompileEffectFromMemory(pData, DataLength, pSrcFileName,
        pDefines, pInclude, HLSLFlags, FXFlags, ppCompiledEffect, ppErrors);
}
//---------------------------------------------------------------------------
HRESULT NiD3D10Renderer::D3D10CreateEffectFromMemory(void* pData, 
    SIZE_T DataLength, UINT FXFlags, ID3D10Device* pDevice, 
    ID3D10EffectPool* pEffectPool, ID3D10Effect** ppEffect)
{
    if (ms_hD3D10 == NULL)
        return E_INVALIDARG;

    return ms_pfnD3D10CreateEffectFromMemory(pData, DataLength, FXFlags, 
        pDevice, pEffectPool, ppEffect);
}
//---------------------------------------------------------------------------
HRESULT NiD3D10Renderer::D3D10CreateEffectPoolFromMemory(void* pData, 
    SIZE_T DataLength, UINT FXFlags, ID3D10Device* pDevice, 
    ID3D10EffectPool** ppEffectPool)
{
    if (ms_hD3D10 == NULL)
        return E_INVALIDARG;

    return ms_pfnD3D10CreateEffectPoolFromMemory(pData, DataLength, FXFlags, 
        pDevice, ppEffectPool);
}
//---------------------------------------------------------------------------
HRESULT NiD3D10Renderer::D3D10DisassembleEffect(ID3D10Effect* pEffect, 
    BOOL EnableColorCode, ID3D10Blob** ppDisassembly)
{
    if (ms_hD3D10 == NULL)
        return E_INVALIDARG;

    return ms_pfnD3D10DisassembleEffect(pEffect, EnableColorCode, 
        ppDisassembly);
}
//---------------------------------------------------------------------------
