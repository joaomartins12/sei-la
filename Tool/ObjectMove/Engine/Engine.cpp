
#include "stdafx.h"
#include "Engine.h"

bool	ResetDevice(bool bBeforeReset, void* pvData)
{
	if( bBeforeReset == true )
	{
		if( g_pFont )
			g_pFont->Release();
	}
	else
	{
		if( g_pFont )
			g_pFont->Init( "Data\\Font\\2002L.ttf" );		
		if( g_pProc3D )
			g_pProc3D->ResetDevice();
	}
	return true;
}

bool cEngine::CALLBACK_ThreadDelete( sTCUnit* pUnit )
{
	CsC_Object* pObject = pUnit->s_pLoadedObject;
	pObject->Delete();

	switch( pObject->GetLeafRTTI() )
	{
	case RTTI_MOVE_MON:
	case RTTI_MOVE_NPC:
		NiDelete (cMoveNpc*)pObject;
		return true;
	}
	return false;
}



cEngine* g_pEngine = NULL;

void cEngine::GlobalShutDown()
{
	g_pEngine->Delete();
	SAFE_DELETE( g_pEngine );

	NiShutdown();
}

void cEngine::GlobalInit()
{
	g_pEngine = new cEngine;
	g_pEngine->Init( AfxGetApp()->m_hInstance, g_pProc3DWin->GetSafeHwnd() );
}

void cEngine::Delete()
{
	cFont::GlobalRelease();
	cSprite::GlobalShotDown();
	cStringAnalysis::GlobalShotDown();
	cProc3D::GlobalShotdown();
	CsC_Thread::GlobalShotDown();

	g_pRenderer = 0;

	SAFE_NIDELETE( m_pInput );
}

void cEngine::Init( HINSTANCE hInst, HWND hWnd )
{
	srand( GetTickCount() );

#ifdef _DEBUG
	NiInit();
#else
	NiInitOptions* pkInitOptions = NiExternalNew NiInitOptions( NiExternalNew NiMemTracker(NiExternalNew NiStandardAllocator(), false) );
	NiInit(pkInitOptions);
#endif

	NiImageConverter::SetImageConverter(NiNew NiDevImageConverter);	
	NiTexture::SetMipmapByDefault(false);
	NiSourceTexture::SetUseMipmapping(false);
	NiMaterial::SetDefaultWorkingDirectory(".\\Data\\Shaders\\Generated");	
	NiSourceTexture::SetDestroyAppDataFlag( false );

	g_pRenderer = CsGBDX9Renderer::CsGBCreate( PROC_3D_WIN_WIDTH, PROC_3D_WIN_HEIGHT,
		NiDX9Renderer::USE_MULTITHREADED | NiDX9Renderer::USE_STENCIL, hWnd, hWnd, 0,
		NiDX9Renderer::DEVDESC_PURE, NiDX9Renderer::FBFMT_UNKNOWN, NiDX9Renderer::DSFMT_D24S8, NiDX9Renderer::PRESENT_INTERVAL_ONE,
		NiDX9Renderer::SWAPEFFECT_COPY,	NiDX9Renderer::FBMODE_MULTISAMPLES_2, 1, NiDX9Renderer::REFRESHRATE_DEFAULT);

	g_pRenderer->SetBackgroundColor( NiColor( 0, 0, 0 ) );
	g_pRenderer->AddResetNotificationFunc( ::ResetDevice, NULL );

	char cShader[ MAX_PATH ] = ".\\Data\\Shaders";
	char* apcShaderDirectories[1];
	apcShaderDirectories[0] = cShader;
	m_ShaderHelper.SetupShaderSystem( NULL, 0, apcShaderDirectories, 1 );

	cStringAnalysis::GlobalInit();
	cFont::GlobalInit( hWnd );
	cSprite::GlobalInit();
	cProc3D::GlobalInit();

	m_pInput = NiNew CInput;
	m_pInput->CreateInputSystem( hInst, hWnd );
	g_Cursor.Init();
	m_pkKeyboard = NULL;
	m_pkMouse = NULL;

	CsC_Thread::GlobalInit();
	g_pThread->UseThread( false );
	g_pThread->ResistCallBack_DeleteUnit( CALLBACK_ThreadDelete );
}

void cEngine::Update()
{
	m_pkKeyboard = NULL;
	m_pkMouse = NULL;
	if( m_pInput->GetInputSystem() )
	{
		m_pkKeyboard = m_pInput->GetInputSystem()->GetKeyboard();
		m_pkMouse = m_pInput->GetInputSystem()->GetMouse();
	}	
	g_Cursor.Update( g_pProc3DWin->GetSafeHwnd() );
	m_pInput->UpdateInput();

	g_pProc3D->Update( g_fDeltaTime );
}


bool cEngine::__BeginRender()
{
	if( g_pRenderer->IsDeviceLost() == true )
	{
		g_pRenderer->Recreate();
		return false;
	}

	assert_cs( g_Sorting.empty() == true );
	if( g_pRenderer->BeginFrame() == false )
	{
		assert_cs( false );
	}

	bool bSuccess = g_pRenderer->BeginUsingDefaultRenderTargetGroup( NiRenderer::CLEAR_ALL );
	return true;
}

bool cEngine::__EndRender()
{
	g_pRenderer->EndUsingRenderTargetGroup();

	if( g_pRenderer->EndFrame() == false )
	{
		assert_cs( false );
		return false;
	}
	if( g_pRenderer->DisplayFrame() == false )
	{
		assert_cs( false );
		return false;
	}
	return true;
}


void cEngine::Render()
{
	if( __BeginRender() == false )
		return;

	// ·£´õ ±¸¹®
	g_pProc3D->Render();
	g_Sorting.Render( CAMERA_ST.GetCameraObj() );

	__EndRender();	
}