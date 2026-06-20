#include "stdafx.h"
#include "Engine.h"
#include "Dsetup.h"

#include <NiDX9SystemDesc.h>
#include <NiPNGReader.h>

#include "./Flow/FlowMgr.h"


#pragma comment(lib, "NiDX9Renderer.lib")
#pragma comment(lib, "NiD3D10Renderer.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "NiD3DXEffectShaderLib.lib")
#pragma comment(lib, "dsetup.lib" )

static IDirectDraw*	g_lpDD = NULL;
CEngine*	g_pEngine = NULL;

bool	ResetDevice(bool bBeforeReset, void* pvData)
{
	if( bBeforeReset == true )
	{
		g_pEngine->ResetDevice_Pre();

		if( *(bool*)pvData == false )
		{
			g_pEngine->m_nOldScreenWidth = GAMEAPP_STPTR->GetWidth();
			g_pEngine->m_nOldScreenHeight = GAMEAPP_STPTR->GetHeight();
		}
	}
	else
	{
		// Flow ResetDevice
		if( FLOWMGR_STPTR && !FLOWMGR_ST.ResetDevice(bBeforeReset, pvData))
			return false;

		g_pEngine->ResetDevice_Post();

		cMessageBox::ResetDeviceList();

		if( TOOLTIPMNG_STPTR )
			TOOLTIPMNG_STPTR->ResetDevice();

		if( g_pGameIF != NULL )
			g_pGameIF->ResetDevice();

		if( g_pEventScene )
			g_pEventScene->ResetDevice();
#ifdef CROSSWARS_SYSTEM		
		if( g_pCrossEvent )
			g_pCrossEvent->ResetDevice();
#endif
		if( g_pCharResMng )
			g_pCharResMng->ResetDevice();

		if( g_pIconMng != NULL )
			g_pIconMng->ResetDevice();	

// 		if( g_pLogin )
// 			g_pLogin->ResetDevice();		
//		if( g_pCharacterSelect != NULL )
//			g_pCharacterSelect->ResetDevice();
//		if( g_pCharacterCreate != NULL )
//			g_pCharacterCreate->ResetDevice();

		if( g_pShadow )
			g_pShadow->ResetDevice();

//		if( g_pUserGrade )
//			g_pUserGrade->ResetDevice();
	}

	return true;
}

CEngine::CEngine()
{
//	m_lpDD				= NULL;

	m_pInput			= NULL;
//	m_pD3DXFont			= NULL;

	m_bD3D10			= false;

	m_bOffScreen		= false;
	m_bUseToggleWindow	= false;

//	m_pD3DXLine			= NULL;
	m_spRenderer		= NULL;

	m_ePostEffect_Start = PE_NONE;
	m_nFrameBufferFormat = NiDX9Renderer::FBFMT_UNKNOWN;
	m_nDepthBufferFormat = NiDX9Renderer::DSFMT_UNKNOWN;

	_InitPostEffect();
}

CEngine::~CEngine()
{
	Delete();
}
//==============================================================================
//
//		베이스
//
//==============================================================================

void CEngine::GBInit()
{
	NiInitOptions* pkInitOptions = NiExternalNew NiInitOptions(	NiExternalNew NiMemTracker(NiExternalNew NiStandardAllocator(),	false) );
	NiInit(pkInitOptions);

	// ImageConverter
#if 1
	NiDevImageConverter* pkImgConverter = NiNew NiDevImageConverter;
	NiImageConverter::SetImageConverter( pkImgConverter );
	NiPNGReader* pkPNGReader = NiNew NiPNGReader();
	pkImgConverter->AddReader( pkPNGReader );
#else
	NiImageConverter::SetImageConverter(NiNew NiDevImageConverter);
#endif

	// Texture Setup
	NiTexture::SetMipmapByDefault(false);
	NiSourceTexture::SetUseMipmapping(false);
	NiSourceTexture::SetUsePreloading(true);
	NiSourceTexture::SetDestroyAppDataFlag(false);

	// Shader
	char* pcWorkingDir = "Data\\Shaders\\Generated";
	NiMaterial::SetDefaultWorkingDirectory(pcWorkingDir);
}

void CEngine::ShutDown()
{
	NiShutdown();

	const NiInitOptions* pkInitOptions = NiStaticDataManager::GetInitOptions();
	NiAllocator* pkAllocator = pkInitOptions->GetAllocator();
	NiExternalDelete pkInitOptions;
	NiExternalDelete pkAllocator;
}

bool CEngine::Init()
{
	g_pEngine = NiNew CEngine;	

	// 랜더러 생성
	if( g_pEngine->_CreateRender() == false )
	{
		TCHAR sz[ 512 ];
		M2W( sz, g_pEngine->m_spRenderer->GetLastErrorString(), 512 );
		CsMessageBox(MB_OK, __T( "디바이스 생성 실패\n%s" ), sz );
		return false;
	}

	if( g_pEngine->_CheckDeviceCaps() == false )
	{
		return false;
	}

	// 쉐이더 시스템 생성
	g_pEngine->_CreateShaderSystem();

	return true;
}

bool CEngine::Create()
{
	//ToggleWindow( g_pResist->IsFullScreen(), true );

	CFont::GlobalInit();

 	if( CreateFont() == false )
 	{
 		return false;
 	}

	g_pEngine->m_pInput = NiNew CInput;
	if( g_pEngine->m_pInput->CreateInputSystem() == false )
	{
		return false;
	}

	//CURSOR_ST.Init();

	g_pEngine->CreateCamera();

	// 전역 호출	
	CBillboard::GlobalInit();

	SetPostEffect( m_ePostEffect_Set );

	//CreateLine(); // Line 객체 생성

	return true;
}

void CEngine::Delete()
{
	_DeletePostEffect();

	SAFE_RELEASE( g_lpDD );
	CBillboard::GlobalDelete();
	_DeleteCamera();
	DeleteFont();
//	DeleteLine();
	SAFE_NIDELETE( m_pInput );
	CFont::GlobalRelease();
	m_spRenderer = NULL;
}

bool CEngine::ResetDevice()
{	
//#ifdef CRASHDUMP_0305	// Device Lost Recreate 함수 확장으로 변경

	m_eDeviceStatus = m_spRenderer->Recreate( GAMEAPP_STPTR->GetWidth(), GAMEAPP_STPTR->GetHeight(),
		NiDX9Renderer::USE_MULTITHREADED | NiDX9Renderer::USE_STENCIL,
		//g_hWnd,
		GAMEAPP_ST.GetHWnd(),
		m_nFrameBufferFormat,
		m_nDepthBufferFormat,
		g_pResist->m_Global.s_bHSync == true ? NiDX9Renderer::PRESENT_INTERVAL_ONE : NiDX9Renderer::PRESENT_INTERVAL_IMMEDIATE,
		NiDX9Renderer::SWAPEFFECT_DISCARD,
		(NiDX9Renderer::FramebufferMode)g_pResist->m_Global.s_nAntialicing,
		1,
		NiDX9Renderer::REFRESHRATE_DEFAULT
		);

	if( m_eDeviceStatus == NiDX9Renderer::RECREATESTATUS_OK )
		return true;

	return false;

// #else
// 	m_eDeviceStatus = m_spRenderer->Recreate( GAMEAPP_STPTR->GetWidth(), GAMEAPP_STPTR->GetHeight() );
// 	if( m_eDeviceStatus == NiDX9Renderer::RECREATESTATUS_FAILED )
// 		return false;
// 
//  	//if( m_spRenderer->Recreate( GAMEAPP_STPTR->GetWidth(), GAMEAPP_STPTR->GetHeight() ) == 0 )
//  	//{
//  	//	return false;
//  	//}
// #endif

	return true;
}

bool CEngine::Update()
{
	m_pInput->UpdateInput();

	return UpdateDeltaTime();	
}

bool CEngine::UpdateDeltaTime()
{
	static float g_fLastUpdate = 0.f;


	g_fLastUpdate = m_Frame.m_fAccumTime;

	if( m_Frame.MeasureTime() == false )
		return false;

	g_fAccumTime = m_Frame.m_fAccumTime;
	g_fDeltaTime = m_Frame.m_fAccumTime - g_fLastUpdate;

	if( g_fDeltaTime < 0 )
		g_fDeltaTime = 0.00000001f;

	if( m_Frame.UpdateFrame() == false )
		return false;

	return true;
}

void CEngine::ResetDevice_Pre()
{
	_ResetDevice_PostEffect( true );

	if( CFont::IsGlobalInit() )
	{
		g_pEngine->DeleteFont();
	}
}

void CEngine::ResetDevice_Post()
{
	_ResetDevice_PostEffect( false );

	if( CFont::IsGlobalInit() )
	{
		// 폰트 인풋 커서 같이 초기화 된다
		g_pEngine->CreateFont();
	}
}

//==============================================================================
//
//		렌더러
//
//==============================================================================

bool CEngine::_CreateRender( bool bD3D10 )
{
	m_bD3D10 = bD3D10;

	assert_cs( m_spRenderer == NULL );

	//Get System Description by NiDX9SystemDesc pointer
	const NiDX9SystemDesc *pkInfo = NiDX9Renderer::GetSystemDesc();	

	LPDIRECT3D9 pD3D = NiDX9Renderer::GetDirect3D();
	if( pD3D == NULL )
	{
		DWORD dwError = GetLastError();
		CsMessageBox(MB_OK, _T( "CheckAdapter : GetDevice Failed! ( ErrorCode = %d )" ), dwError );
		return false;
	}

	D3DDISPLAYMODE curMode;
	pD3D->GetAdapterDisplayMode( D3DADAPTER_DEFAULT, &curMode );
	D3DFORMAT eFrameFormat = curMode.Format;
	
	//Get Adapter information index 0
	const NiDX9AdapterDesc *pkAdapter = pkInfo->GetAdapter(0);

	//Get device inforamtion by HAL type
	const NiDX9DeviceDesc *pkDevice = pkAdapter->GetDevice(D3DDEVTYPE_HAL);
	if( pkDevice == NULL )
	{
		DWORD dwError = GetLastError();
		CsMessageBox(MB_OK, _T( "pkAdapter->GetDevice( D3DDEVTYPE_HAL ) : GetDevice Failed! ( ErrorCode = %d )" ), dwError );
		return false;
	}

	//Get device capabilities
	const D3DCAPS9& kD3DCaps9 = pkDevice->GetCaps();

	///디바이스에서지원되는기능중에하드웨어변환이지원되지않을경우소프트웨어버텍스처리로
	NiDX9Renderer::DeviceDesc DeviceDesc = NiDX9Renderer::DEVDESC_HAL_HWVERTEX;	
	if( !(kD3DCaps9.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT) )
		DeviceDesc = NiDX9Renderer::DEVDESC_HAL_SWVERTEX;

	//Get nearest frame buffer format	
	switch( eFrameFormat )
	{
	case D3DFMT_R8G8B8:			m_nFrameBufferFormat = NiDX9Renderer::FBFMT_R8G8B8;				break;
	case D3DFMT_A8R8G8B8:		m_nFrameBufferFormat = NiDX9Renderer::FBFMT_A8R8G8B8;			break;
	case D3DFMT_X8R8G8B8:		m_nFrameBufferFormat = NiDX9Renderer::FBFMT_X8R8G8B8;			break;
	case D3DFMT_R5G6B5:			m_nFrameBufferFormat = NiDX9Renderer::FBFMT_R5G6B5;				break;
	case D3DFMT_X1R5G5B5:		m_nFrameBufferFormat = NiDX9Renderer::FBFMT_X1R5G5B5;			break;
	case D3DFMT_A1R5G5B5:		m_nFrameBufferFormat = NiDX9Renderer::FBFMT_A1R5G5B5;			break;
	case D3DFMT_X4R4G4B4:		m_nFrameBufferFormat = NiDX9Renderer::FBFMT_X4R4G4B4;			break;
	default:
		CsMessageBox( MB_OK, _LAN( "지원되지 않는 모니터 색 품질입니다. eFrameFormat = %d" ), eFrameFormat );
		return false;
	}

	//Get nearest depth-stencil buffer format
	if( SUCCEEDED( pD3D->CheckDeviceFormat( 
									pkAdapter->GetAdapterIndex(),
									pkDevice->GetDeviceType(),
									eFrameFormat,
									D3DUSAGE_DEPTHSTENCIL,
									D3DRTYPE_SURFACE, D3DFMT_D24S8 ) ) )
	{
		if( SUCCEEDED( pD3D->CheckDepthStencilMatch(
									pkAdapter->GetAdapterIndex(),
									pkDevice->GetDeviceType(),
									eFrameFormat,
									eFrameFormat,
									D3DFMT_D24S8 ) ) )
		{
			m_nDepthBufferFormat = NiDX9Renderer::DSFMT_D24S8;
		}
	}
	

	int nCheckCount = 5;
	while( m_spRenderer == NULL )
	{		
		m_spRenderer = CsGBDX9Renderer::CsGBCreate(GAMEAPP_STPTR->GetWidth(), GAMEAPP_STPTR->GetHeight(),
													NiDX9Renderer::USE_MULTITHREADED | NiDX9Renderer::USE_STENCIL, 
													GAMEAPP_STPTR->GetHWnd(), GAMEAPP_STPTR->GetHWnd(), 0,
													DeviceDesc,
													m_nFrameBufferFormat,
													m_nDepthBufferFormat,
													g_pResist->m_Global.s_bHSync == true ? NiDX9Renderer::PRESENT_INTERVAL_ONE : NiDX9Renderer::PRESENT_INTERVAL_IMMEDIATE,
													//NiDX9Renderer::SWAPEFFECT_DEFAULT,
													NiDX9Renderer::SWAPEFFECT_DISCARD,
													//NiDX9Renderer::SWAPEFFECT_FLIP,
													//NiDX9Renderer::FBMODE_DEFAULT,
													(NiDX9Renderer::FramebufferMode) g_pResist->m_Global.s_nAntialicing,
													1,
													NiDX9Renderer::REFRESHRATE_DEFAULT);

		if( m_spRenderer == NULL )
		{
			if( (--nCheckCount) <= 0 )
			{
				CsMessageBox( MB_OK, _T( "Direct3D renderer creation error" ) );
				return false;
			}
			Sleep( 100 );
		}
	}


	g_pRenderer = (NiRenderer*)m_spRenderer;
	m_spRenderer->SetBackgroundColor( NiColorA::WHITE );
	//m_spRenderer->SetBackgroundColor( NiColorA::BLACK );

	m_nOldScreenWidth = m_nBackupScreenWidth = GAMEAPP_STPTR->GetWidth();
	m_nOldScreenHeight = m_nBackupScreenHeight = GAMEAPP_STPTR->GetHeight();

	m_spRenderer->AddResetNotificationFunc( ::ResetDevice, &m_bUseToggleWindow );

// #ifdef MOVIECAPTURE
// 	MovieCapture::GetInstance().Initialize(m_spRenderer->GetD3DDevice());
// #endif

	return true;
}

//==============================================================================
//
//		파일 패킹
//
//==============================================================================

//#include <Mss.h>
//#define SOUND_FILEPACK_INDEX		2
// struct sPLAY_SOUND
// {
// 	int					s_nHandle;
// 	size_t				s_nCurOffset;
// 	CsFileHash::sINFO*	s_pHashInfo;
// };
// 
// U32 AILCALL Sound_file_open_callback( MSS_FILE const FAR* Filename, UINTa FAR* FileHandle)
// {	
// 	CsFileHash::sINFO* pInfo = FPMNG_PTR->GetHashData( SOUND_FILEPACK_INDEX, (char*)Filename );
// 	if( pInfo == NULL )
// 		return 0;
// 
// 	sPLAY_SOUND* pSound = csnew sPLAY_SOUND;
// 	pSound->s_pHashInfo = pInfo;
// 	pSound->s_nHandle = FPMNG_PTR->GetFilePack( SOUND_FILEPACK_INDEX )->s_pFilePack->__Handle();
// 	pSound->s_nCurOffset = (size_t)pInfo->s_nOffset;
// #pragma warning( disable : 4311 )
// 	*FileHandle = (UINT)pSound;
// #pragma warning( default : 4311 )
// 	return 1;
// }
// 
// void AILCALL Sound_file_close_callback( UINTa FileHandle )
// {
// 	delete (sPLAY_SOUND*)FileHandle;
// }
// 
// S32 AILCALLBACK Sound_file_seek_callback( UINTa FileHandle, S32 Offset, U32 Type )
// {
// 	sPLAY_SOUND* pSound = (sPLAY_SOUND*)FileHandle;
// 
// 	size_t nOrgStart = (size_t)pSound->s_pHashInfo->s_nOffset;
// 	switch( Type )
// 	{
// 	case AIL_FILE_SEEK_BEGIN:
// 		pSound->s_nCurOffset = nOrgStart + Offset;
// 		break;
// 	case AIL_FILE_SEEK_CURRENT:
// 		pSound->s_nCurOffset += Offset;
// 		break;
// 	case AIL_FILE_SEEK_END:
// 		pSound->s_nCurOffset =	nOrgStart + pSound->s_pHashInfo->s_nDataSize + Offset;
// 		break;
// 	default:
// 		assert_cs( false );
// 	}
// 	return pSound->s_nCurOffset - nOrgStart;
// }
// 
// U32 AILCALLBACK Sound_file_read_callback( UINTa FileHandle, void FAR* Buffer, U32 Bytes )
// {
// 	sPLAY_SOUND* pSound = (sPLAY_SOUND*)FileHandle;
// 
// 	FPMNG_PTR->SeekLock( SOUND_FILEPACK_INDEX );
// 	_lseeki64( pSound->s_nHandle, pSound->s_nCurOffset, SEEK_SET );
// 	U32 nReturn = _read( pSound->s_nHandle, Buffer, Bytes );
// 	FPMNG_PTR->SeekUnlock( SOUND_FILEPACK_INDEX );
// 	pSound->s_nCurOffset += nReturn;
// 	return nReturn;
// }



//U32 AILCALL Sound_file_seek_callback( UINTa FileHandle, S32 Offset, U32 Type )
//{
//	SeekLock( int nFilePackIndex ){ GetFilePack( nFilePackIndex )->s_pFilePack->SeekLock(); }
//
//	FPMNG_PTR->GetFileHandle(  )
//
//	switch( Type )
//	{
//	case AIL_FILE_SEEK_BEGIN:		
//	case AIL_FILE_SEEK_CURRENT:
//	case AIL_FILE_SEEK_END:
//	default:						assert_cs( false );
//	}	
//	return 1;
//}


bool CEngine::_CheckDeviceCaps()
{
	D3DCAPS9 D_Cap;
	HRESULT hr = m_spRenderer->GetD3DDevice()->GetDeviceCaps( &D_Cap );
	if( FAILED( hr ) )
	{
		CsMessageBox( MB_OK, _LAN( "하드웨어 3D가속을 받지 못합니다. 그래픽 드라이버 or DirectX or D3D가속을 확인해 주십시요." ) );
		return false;
	}

	// 픽쉘쉐이더 2.0 지원 가능한가
	m_DeviceCaps.s_bEnablePS_2_0 = ( (D_Cap.PixelShaderVersion < D3DPS_VERSION( 2,0 ) ) == false );

	// 최대 텍스쳐 갯수 - 셋팅과 샘플링
	m_DeviceCaps.s_nMaxTextureBlendStage = D_Cap.MaxTextureBlendStages;
	m_DeviceCaps.s_nMaxSimultaneousTextures = D_Cap.MaxSimultaneousTextures;
	if( m_DeviceCaps.s_nMaxSimultaneousTextures >= 8 )
	{
		m_DeviceCaps.s_nMaxSimultaneousTextures = 8;
	}
	else if( m_DeviceCaps.s_nMaxSimultaneousTextures >= 4 )
	{
		m_DeviceCaps.s_nMaxSimultaneousTextures = 4;
	}
	else
	{
		m_DeviceCaps.s_nMaxSimultaneousTextures = 0;
	}

	// 픽쉘쉐이더 사용 안할시 지형 랜더링 가능한지
	m_DeviceCaps.s_bTRsw_Blend_Enalbe = ( ( D_Cap.TextureOpCaps & D3DTEXOPCAPS_BLENDCURRENTALPHA ) != 0 );

	// DirectX 버젼
	DWORD dwVersion, dwRevision;
	DirectXSetupGetVersion( &dwVersion, &dwRevision );
	m_DeviceCaps.s_bDxVersion_9_0 = ( dwVersion >= 0x00040009 );

	// DeviceCaps
	if( m_DeviceCaps.s_bDxVersion_9_0 == false )
	{
		CsMessageBox( MB_OK, _LAN( "DirectX 버젼이 9.0 보다 낮습니다. DirectX 버젼을 올려주세요." ) );
		return false;
	}
	if( m_DeviceCaps.s_bEnablePS_2_0 == false )
	{
		CsMessageBox( MB_OK, _LAN( "현재의 그래픽 카드로는 사양이 낮아 지형이나 케릭터등이 정상적이지 않은 화면으로 보일 수 있습니다." ) );
		g_pResist->m_Global.s_bUseMyShader = true;
		g_pResist->m_Global.s_bCharOutLine = false;
		g_pResist->m_Global.s_nShadowType = 0;
		g_pResist->m_Global.s_nWeather = 0;
		//return false;
	}

	bool bLowGraphic = false;
	if( m_DeviceCaps.s_bTRsw_Blend_Enalbe == false )
	{
		bLowGraphic = true;
	}
	if( m_DeviceCaps.s_nMaxSimultaneousTextures == 0 )
	{
		bLowGraphic = true;
	}
	if( m_DeviceCaps.s_nMaxTextureBlendStage < 8 )
	{
		bLowGraphic = true;
	}

	if( m_DeviceCaps.s_bEnablePS_2_0 == true )
	{
		if( ( bLowGraphic == true )&&( g_bInstallStart == true ) )
		{
			CsMessageBox( MB_OK, _LAN( "그래픽 카드 사양이 낮아 기본 쉐이더를 사용하겠습니다.\n쉐이더 설정은 그래픽 옵션에서 조절 가능합니다." ) );
			g_pResist->m_Global.s_bUseMyShader = true;
			g_pResist->m_Global.s_bCharOutLine = false;
			g_pResist->m_Global.s_nShadowType = 0;
			g_pResist->m_Global.s_nWeather = 0;
		}
	}	
	return true;
}

NiPoint3 CEngine::Cal3D_To_2D( NiPoint3 vPos )
{
	D3DXVECTOR4 v;
	NiPoint3 vReturn;
	D3DXVec3Transform( &v, (D3DXVECTOR3*)&vPos, GetD3DViewProj() );
	v = v/v.w;
	vReturn.x = m_ViewPort.X + m_ViewPort.Width*(1+v.x)*0.5f + 0.5f;
	vReturn.y = m_ViewPort.Y + m_ViewPort.Height*(1-v.y)*0.5f + 0.5f;
	vReturn.z = m_ViewPort.MinZ + v.z*( m_ViewPort.MaxZ - m_ViewPort.MinZ );
	return vReturn;
}

void CEngine::Change_ShaderRender_Render()
{
	/*
	m_spRenderer->EndUsingRenderTargetGroup();
	m_spRenderer->BeginUsingRenderTargetGroup( m_pRTSrc2, NiRenderer::CLEAR_ALL );
	ScreenSpace();

	// 좌로 확대
	m_pEffectScreenSrc1->Draw( m_spRenderer );

	m_spRenderer->EndUsingRenderTargetGroup();
	m_spRenderer->BeginUsingRenderTargetGroup( m_pRTSrc3, NiRenderer::CLEAR_ALL );

	// 우로 확대
	m_pEffectScreenSrc2->Draw( m_spRenderer );

	//m_spRenderer->EndUsingRenderTargetGroup();
	//m_spRenderer->BeginUsingRenderTargetGroup( m_pRTSrc2, NiRenderer::CLEAR_ALL );

	// 추출된 블러를 우로 확대
	//m_pEffectScreenSrc3->Draw( m_spRenderer );

	//m_spRenderer->EndUsingRenderTargetGroup();
	//m_spRenderer->BeginUsingRenderTargetGroup( m_pRTSrc1, NiRenderer::CLEAR_NONE );

	m_spRenderer->EndUsingRenderTargetGroup();
	m_spRenderer->BeginUsingDefaultRenderTargetGroup( NiRenderer::CLEAR_NONE );

	// 확대된 텍스쳐와 원본텍스쳐의 결합
	// 소스1번 텍스쳐와 소스 2번 텍스쳐를 넣어준다	
	m_pEffectScreenResult->Draw( m_spRenderer );

	m_spRenderer->SetCameraData( CGeometry::m_pRenderCamera );
	*/
}

void CEngine::Change_Render_ShaderRender()
{
	/*
	m_spRenderer->EndUsingRenderTargetGroup();
	m_spRenderer->BeginUsingRenderTargetGroup( m_pRTSrc1, NiRenderer::CLEAR_ALL );
	*/
}

void CEngine::ResetRenderer()
{
	m_bUseToggleWindow = true;

	m_eDeviceStatus = m_spRenderer->Recreate( GAMEAPP_STPTR->GetWidth(), GAMEAPP_STPTR->GetHeight(),
							NiDX9Renderer::USE_MULTITHREADED | NiDX9Renderer::USE_STENCIL,
							GAMEAPP_STPTR->GetHWnd(),
							m_nFrameBufferFormat,
							m_nDepthBufferFormat,
							g_pResist->m_Global.s_bHSync == true ? NiDX9Renderer::PRESENT_INTERVAL_ONE : NiDX9Renderer::PRESENT_INTERVAL_IMMEDIATE,
							NiDX9Renderer::SWAPEFFECT_DISCARD,
							(NiDX9Renderer::FramebufferMode)g_pResist->m_Global.s_nAntialicing,
							1,
							NiDX9Renderer::REFRESHRATE_DEFAULT
						);

	m_bUseToggleWindow = false;

	if( m_eDeviceStatus == NiDX9Renderer::RECREATESTATUS_FAILED )
		assert_cs( false );
}

//==============================================================================
//
//		쉐이더
//
//==============================================================================

// void CEngine::DrawText( int x, int y, DWORD color, UINT Align, TCHAR const* strText, ... )
// {
// 	va_list argptr;
// 	TCHAR szText[256];
// 
// 	va_start( argptr, strText ); 
// 	wvsprintf( szText, strText, argptr ); 
// 	va_end( argptr );
// 
// 	//strText = szText;
// 	DrawTextRender( x+1, y+1, 0xff000000 & color, szText, Align );
// 	DrawTextRender( x, y, color, szText, Align );
// }
// void CEngine::DrawText_OutLine( int x, int y, DWORD color, UINT Align, TCHAR const* szStr )
// {
// 	DrawTextRender( x+1, y+1, 0xff000000 & color, szStr, Align );
// 	DrawTextRender( x, y, color, szStr, Align );
// }

// void CEngine::DrawTextRender( int x, int y, DWORD color, TCHAR const* str, UINT Align )
// {
// 	assert_cs( m_pD3DXFont != NULL );
// 	RECT TextRect = { x, y, 0, 0 };
// 	if( Align & DT_CENTER )
// 		TextRect.left += x;
// 	m_pD3DXFont->DrawText( NULL, str, -1, &TextRect, DT_NOCLIP | Align, color );
// }
//////////////////////////////////////////////////////////////////////////
// void CEngine::DrawText( CsPoint pos, CsPoint size, DWORD color, UINT Align, TCHAR const* strText, ... )
// {
// 	va_list argptr;
// 	TCHAR szText[256];
// 
// 	va_start( argptr, strText ); 
// 	wvsprintf( szText, strText, argptr ); 
// 	va_end( argptr );
// 
// 	DrawTextRender( pos + CsPoint(1,1), size, 0xff000000 & color, Align, szText );
// 	DrawTextRender( pos, size, color, Align, szText );
// }
// void CEngine::DrawText_OutLine( CsPoint pos, CsPoint size, DWORD color, UINT Align, TCHAR const* szStr )
// {
// 	DrawTextRender( pos + CsPoint(1,1), size, 0xff000000 & color, Align, szStr );
// 	DrawTextRender( pos, size, color, Align, szStr );
// }
// 
// void CEngine::DrawTextRender( CsPoint pos, CsPoint size, DWORD color, UINT Align, TCHAR const* str )
// {
// 	assert_cs( m_pD3DXFont != NULL );
// 	RECT TextRect = { pos.x, pos.y, pos.x + size.x, pos.y + size.y };
// 	m_pD3DXFont->DrawText( NULL, str, -1, &TextRect, DT_NOCLIP | Align, color );
// }

//==============================================================================
//
//		인터페이스
//
//==============================================================================
//bool CEngine::SetFullScreen( bool bFullScreen )
//{
//	if( bFullScreen )
//	{
//		HRESULT	hr;
//		if( m_lpDD == NULL )
//		{
//			hr = DirectDrawCreate(NULL, &m_lpDD, NULL);
//			if( FAILED(hr) )
//				return false;
//		}
//		hr = m_lpDD->SetCooperativeLevel(GetDesktopWindow(), DDSCL_NORMAL);
//		if( FAILED(hr) )
//			return false;
//		hr = m_lpDD->SetDisplayMode( g_nScreenWidth, g_nScreenHeight, 32 );
//		if( FAILED(hr) )
//			return false;
//	}
//	else
//	{
//		SAFE_RELEASE( m_lpDD );
//	}
//	return true;
//}

//==============================================================================
// script : ToggleWindow
//==============================================================================
void CEngine::ToggleWindow( bool bFullScreen, bool bForceToggle /*=false*/ )
{
	m_nOldScreenWidth = g_nScreenWidth;
	m_nOldScreenHeight = g_nScreenHeight;
	g_nScreenWidth = g_pResist->m_Global.s_nResolutionWidth;
	g_nScreenHeight = g_pResist->m_Global.s_nResolutionHeight;

	if( bFullScreen )
	{
		g_pResist->SetFullScreen( true );

		HRESULT	hr;
		if( g_lpDD == NULL )
		{
			hr = DirectDrawCreate(NULL, &g_lpDD, NULL);
			assert_cs( FAILED(hr) == false );
		}
		
		DWORD dwStyle = GetWindowLong( GAMEAPP_ST.GetHWnd(), GWL_STYLE ) | WS_POPUP;		
		dwStyle &=  ~WS_OVERLAPPEDWINDOW;
		SetWindowLong( GAMEAPP_ST.GetHWnd(), GWL_STYLE, dwStyle );
		SetWindowPos( GAMEAPP_ST.GetHWnd(), HWND_TOP, 0, 0, g_nScreenWidth, g_nScreenHeight, 0 );		


		hr = g_lpDD->SetCooperativeLevel(GetDesktopWindow(), DDSCL_NORMAL | DDSCL_NOWINDOWCHANGES );
		assert_cs( FAILED(hr) == false );
		hr = g_lpDD->SetDisplayMode( g_nScreenWidth, g_nScreenHeight, 32 );
		assert_cs( FAILED(hr) == false );
	}
	else
	{
		SAFE_RELEASE( g_lpDD );

		g_pResist->SetFullScreen( false );

		DWORD dwStyle = GetWindowLong( GAMEAPP_ST.GetHWnd(), GWL_STYLE ) | WS_OVERLAPPEDWINDOW;
		RECT rect = { 0, 0, g_nScreenWidth, g_nScreenHeight};
		AdjustWindowRect(&rect, dwStyle, false);

		dwStyle &=  ~WS_POPUP;
		SetWindowLong( GAMEAPP_ST.GetHWnd(), GWL_STYLE, dwStyle );
		SetWindowPos( GAMEAPP_ST.GetHWnd(), HWND_TOP, g_pResist->m_Global.s_nLeft, g_pResist->m_Global.s_nTop, rect.right-rect.left, rect.bottom-rect.top, 0 );
		GetClientRect( GAMEAPP_ST.GetHWnd(), &rect );

		g_nScreenWidth = rect.right-rect.left;
		g_nScreenHeight = rect.bottom-rect.top;
	}

	if( bForceToggle == true )
	{
		// 윈도우 보이게 셋팅
		ShowWindow(GAMEAPP_ST.GetHWnd(), GAMEAPP_ST.GetFullMode());
		UpdateWindow(GAMEAPP_ST.GetHWnd());
		// 화면을 클리어 화면으로 한번 채워준다.
		m_spRenderer->BeginFrame();	
		m_spRenderer->BeginUsingDefaultRenderTargetGroup(NiRenderer::CLEAR_ALL);
		m_spRenderer->EndUsingRenderTargetGroup();
		m_spRenderer->EndFrame();
		m_spRenderer->DisplayFrame();
	}	

	m_bUseToggleWindow = true;

	if( bForceToggle == false )
	{
		CAMERA_ST.ChangeStandardFov( (float)g_nScreenWidth, (float)g_nScreenHeight );
		m_spRenderer->Recreate( g_nScreenWidth, g_nScreenHeight );
	}
	else
	{
		m_spRenderer->Recreate( g_nScreenWidth, g_nScreenHeight );
	}

	m_bUseToggleWindow = false;
}

void CEngine::ChangeResolutionMode( bool bFullMode, int nWidth, int nHeight )
{
	if( bFullMode )
	{
		HRESULT	hr;
		if( g_lpDD == NULL )
		{
			hr = DirectDrawCreate(NULL, &g_lpDD, NULL);
			assert_cs( FAILED(hr) == false );
		}
		hr = g_lpDD->SetCooperativeLevel(GetDesktopWindow(), DDSCL_NORMAL);
		assert_cs( FAILED(hr) == false );
		hr = g_lpDD->SetDisplayMode( nWidth, nHeight, 32 );
		assert_cs( FAILED(hr) == false );
	}
	else
		SAFE_RELEASE( g_lpDD );

	CAMERA_ST.ChangeStandardFov( (float)nWidth, (float)nHeight );
	m_spRenderer->Recreate( nWidth, nHeight );
}

void CEngine::ChangeResolution()
{
	m_nOldScreenWidth = GAMEAPP_ST.GetWidth();
	m_nOldScreenHeight = GAMEAPP_ST.GetHeight();

	GAMEAPP_ST.CalculateSize( g_pResist->m_Global.s_nResolutionWidth, g_pResist->m_Global.s_nResolutionHeight, g_pResist->m_Global.s_bFullScreen );

	g_nScreenWidth = GAMEAPP_ST.GetWidth();
	g_nScreenHeight = GAMEAPP_ST.GetHeight();

	ChangeResolutionMode( GAMEAPP_ST.GetFullMode(), GAMEAPP_ST.GetWidth(), GAMEAPP_ST.GetHeight() );

	GAMEAPP_ST.ReSize( GAMEAPP_ST.GetWidth(), GAMEAPP_ST.GetHeight(), GAMEAPP_ST.GetFullMode(), true );


// 	if( g_pResist->m_Global.s_bFullScreen )
// 	{
// 		HRESULT	hr;
// 		if( g_lpDD == NULL )
// 		{
// 			hr = DirectDrawCreate(NULL, &g_lpDD, NULL);
// 			assert_cs( FAILED(hr) == false );
// 		}
// 		hr = g_lpDD->SetCooperativeLevel(GetDesktopWindow(), DDSCL_NORMAL);
// 		assert_cs( FAILED(hr) == false );
// 		hr = g_lpDD->SetDisplayMode( g_nScreenWidth, g_nScreenHeight, 32 );
// 		assert_cs( FAILED(hr) == false );
// 
// 		DWORD dwStyle = GetWindowLong( GAMEAPP_ST.GetHWnd(), GWL_STYLE ) | WS_POPUP;		
// 		dwStyle &=  ~WS_OVERLAPPEDWINDOW;
// 		SetWindowLong( GAMEAPP_ST.GetHWnd(), GWL_STYLE, dwStyle );
// 		SetWindowPos( GAMEAPP_ST.GetHWnd(), HWND_TOP, 0, 0, g_nScreenWidth, g_nScreenHeight, 0 );		
// 	}
// 	else
// 	{
// 		SAFE_RELEASE( g_lpDD );
// 
// 		RECT rect = { 0, 0, g_nScreenWidth, g_nScreenHeight};		
// 		AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, false);
// 		DWORD dwStyle = GetWindowLong( GAMEAPP_ST.GetHWnd(), GWL_STYLE ) | WS_OVERLAPPEDWINDOW;
// 		dwStyle &=  ~WS_POPUP;
// 		SetWindowLong( GAMEAPP_ST.GetHWnd(), GWL_STYLE, dwStyle );
// 		SetWindowPos( GAMEAPP_ST.GetHWnd(), HWND_TOP, g_pResist->m_Global.s_nLeft, g_pResist->m_Global.s_nTop, rect.right-rect.left, rect.bottom-rect.top, 0 );
// 
// 		GetClientRect( GAMEAPP_ST.GetHWnd(), &rect );
// 		g_nScreenWidth = rect.right-rect.left;
// 		g_nScreenHeight = rect.bottom-rect.top;
// 	}
// 
// 	CAMERA_ST.ChangeStandardFov( (float)g_nScreenWidth, (float)g_nScreenHeight );
// 	m_spRenderer->Recreate( g_nScreenWidth, g_nScreenHeight );
}

//==============================================================================
//
//		폰트
//
//==================================================`============================
 
bool CEngine::CreateFont()
{
	SAFE_POINTER_RETVAL( GLOBALDATA_STPTR, false );

	std::string systemFontName = GLOBALDATA_STPTR->GetSystemFontFileName();
	std::string textFontName = GLOBALDATA_STPTR->GetTextFontFileName();

// 	HRESULT hr = D3DXCreateFontA(	m_spRenderer->GetD3DDevice(), 11,0,FW_NORMAL,1,FALSE,DEFAULT_CHARSET,OUT_DEFAULT_PRECIS,
// 		DEFAULT_QUALITY,DEFAULT_PITCH|FF_DONTCARE, systemFontName.c_str(), &m_pD3DXFont );
// 
// 	assert_cs( SUCCEEDED( hr ) );
// 	SAFE_POINTER_RETVAL( SUCCEEDED( hr ), false );

// #if ( defined VERSION_TW || defined VERSION_HK )
// 	HRESULT hr = D3DXCreateFont(	m_spRenderer->GetD3DDevice(), 11,0,FW_NORMAL,1,FALSE,DEFAULT_CHARSET,OUT_DEFAULT_PRECIS,
// 		DEFAULT_QUALITY,DEFAULT_PITCH|FF_DONTCARE, _T( "Data\\Font\\msjh.ttc" ), &m_pD3DXFont );
//if (defined VERSION_USA)
// 	HRESULT hr = D3DXCreateFont(	m_spRenderer->GetD3DDevice(), 11,0,FW_NORMAL,1,FALSE,DEFAULT_CHARSET,OUT_DEFAULT_PRECIS,
// 		DEFAULT_QUALITY,DEFAULT_PITCH|FF_DONTCARE, _T( "Data\\Font\\tahoma.ttf" ), &m_pD3DXFont );
// #else
// 	HRESULT hr = D3DXCreateFont(	m_spRenderer->GetD3DDevice(), 11,0,FW_NORMAL,1,FALSE,DEFAULT_CHARSET,OUT_DEFAULT_PRECIS,
// 		DEFAULT_QUALITY,DEFAULT_PITCH|FF_DONTCARE, _T( "Data\\Font\\NanumGothicBold.ttf" ), &m_pD3DXFont );
// #endif
	

	if( m_FontSystem.Init( systemFontName.c_str() ) == false )
	{
		CsMessageBoxA( MB_OK, "Create Font Failed! %s", systemFontName.c_str());
		return false;
	}
	if( m_FontText.Init( textFontName.c_str() ) == false )
	{
		CsMessageBoxA( MB_OK, "Create Font Failed! %s", textFontName.c_str());
		return false;
	}
	return true;
}

void CEngine::DeleteFont()
{
//	SAFE_RELEASE( m_pD3DXFont );
	m_FontSystem.Release();
	m_FontText.Release();
}

//==============================================================================
//
//		카메라
//
//==============================================================================

void CEngine::CreateCamera()
{
	// 픽킹
	CsGBPick::Create( NiPick::FIND_ALL, NiPick::WORLD_COORDINATES, true, false );
	
	CREATE_SINGLETON(CsGBCamera);		//2015-11-30-nova g_Camera -> 싱글톤
	// 카메라
	CAMERA_ST.Create( m_spRenderer );
	CAMERA_ST.ChangeStandardFov( (float)GAMEAPP_STPTR->GetWidth(), (float)GAMEAPP_STPTR->GetHeight() );
}

void CEngine::_DeleteCamera()
{
	if( CAMERA_STPTR )
		CAMERA_ST.Delete();

	CsGBPick::Delete();	

	////2015-11-30-nova
	if( CAMERA_STPTR )
		DESTROY_SINGLETON(CAMERA_STPTR);
}

// void CEngine::CreateLine()
// {
// 	HRESULT hr = D3DXCreateLine( m_spRenderer->GetD3DDevice(), &m_pD3DXLine);
// 	assert_cs( SUCCEEDED( hr ) );	
// 
// 	m_pD3DXLine->SetAntialias(TRUE);
// 	m_pD3DXLine->SetGLLines(TRUE);
// 	m_pD3DXLine->SetWidth(1);
// }

// void CEngine::DeleteLine()
// {
// 	SAFE_RELEASE( m_pD3DXLine );
// }
