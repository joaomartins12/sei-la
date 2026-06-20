
#include "stdafx.h"
#include "DirectShow.h"

#pragma comment(lib, "strmiids.lib" )
#include <vmr9.h>
#include <DShow.h>
#include "DirectShowNeed.h"


// 공용
IGraphBuilder*			pGB				= NULL;          // GraphBuilder 
IMediaControl*			pMC				= NULL;          // Media Control
IMediaEvent*			pME				= NULL;          // Media Event

// 화면에 직접 랜더링용
IBaseFilter*			pVRM			= NULL;
IVMRWindowlessControl*	pWC				= NULL;
IVMRFilterConfig*		pConfig			= NULL;

// 텍스쳐에 랜더링용
IBaseFilter*			pGrabberFiler	= NULL;
IBaseFilter*			pSrcFilter		= NULL;
IBaseFilter*			pNullRenderer	= NULL;
IBaseFilter*			pSoundFilter	= NULL;
ISampleGrabber*			pGrabber		= NULL;

#define RELEASE_POINTER_WINDOW	{\
	if( pWC )\
		pWC->SetVideoClippingWindow( NULL );\
	SAFE_RELEASE( pConfig );\
	SAFE_RELEASE( pME );\
	SAFE_RELEASE( pMC );\
	SAFE_RELEASE( pWC );\
	SAFE_RELEASE( pVRM );\
	SAFE_RELEASE( pGB );\
	CoUninitialize();\
}

// 콜백은 가장 마지막에 제거
#define RELEASE_POINTER_TEXTURE	{\
	SAFE_RELEASE( pME );\
	SAFE_RELEASE( pMC );\
	SAFE_RELEASE( pGrabber );\
	SAFE_RELEASE( pGrabberFiler );\
	SAFE_RELEASE( pSrcFilter );\
	SAFE_RELEASE( pNullRenderer );\
	SAFE_RELEASE( pSoundFilter );\
	SAFE_RELEASE( pGB );\
	CoUninitialize();\
	SAFE_DELETE( g_pGrabberCallback );\
}

// 콜백은 가장 마지막에 제거
#define RELEASE_POINTER_ALL		{\
	if( pWC )\
		pWC->SetVideoClippingWindow( NULL );\
	SAFE_RELEASE( pConfig );\
	SAFE_RELEASE( pME );\
	SAFE_RELEASE( pMC );\
	SAFE_RELEASE( pWC );\
	SAFE_RELEASE( pGrabber );\
	SAFE_RELEASE( pGrabberFiler );\
	SAFE_RELEASE( pSrcFilter );\
	SAFE_RELEASE( pVRM );\
	SAFE_RELEASE( pNullRenderer );\
	SAFE_RELEASE( pSoundFilter );\
	SAFE_RELEASE( pGB );\
	CoUninitialize();\
	SAFE_DELETE( g_pGrabberCallback );\
}

void nsDIRECTSHOW::GlobalShotDown()
{
	if( IsPlay() == false )
		return;

	StopDShow();
	while( true )
	{
		if( UpdateDShow() == false )
			break;
	}
}

bool nsDIRECTSHOW::IsPlay()
{
	return ( pGB != NULL );
}

bool nsDIRECTSHOW::InitDShowWindow( TCHAR* szFile, CsPoint pos, CsPoint size, bool bKeepAspective )
{
	if( IsPlay() == true )
		return false;

	CoInitialize( NULL );

	if( SUCCEEDED( CoCreateInstance( CLSID_FilterGraph, NULL, CLSCTX_INPROC_SERVER, IID_IGraphBuilder, (void**)&pGB ) ) == false )
	{
		RELEASE_POINTER_WINDOW;
		return false;
	}
	
	if( SUCCEEDED( CoCreateInstance( CLSID_VideoMixingRenderer9, NULL, CLSCTX_INPROC, IID_IBaseFilter, (void**)&pVRM ) ) == false )
	{
		RELEASE_POINTER_WINDOW;
		return false;
	}

	if( SUCCEEDED( pGB->AddFilter( pVRM, _T( "Video Mixing Renderer 9" ) ) ) == false )
	{
		RELEASE_POINTER_WINDOW;
		return false;
	}

	if (SUCCEEDED( pVRM->QueryInterface(IID_IVMRFilterConfig9, (void**)&pConfig) ) == false ) 
	{
		RELEASE_POINTER_WINDOW;
		return false;
	}
	if( SUCCEEDED( pConfig->SetRenderingMode(VMR9Mode_Windowless) ) == false )
	{
		RELEASE_POINTER_WINDOW;
		return false;
	}
	SAFE_RELEASE( pConfig );
	
	if( SUCCEEDED( pVRM->QueryInterface(IID_IVMRWindowlessControl9, (void**)&pWC) ) == false )
	{
		RELEASE_POINTER_WINDOW;
		return false;
	}
	SAFE_RELEASE( pVRM );

	pWC->SetVideoClippingWindow( GAMEAPP_ST.GetHWnd() );
	if( SUCCEEDED( pGB->QueryInterface( IID_IMediaControl, (void **)&pMC ) ) == false )
	{
		RELEASE_POINTER_WINDOW;
		return false;
	}

	if( SUCCEEDED( pGB->QueryInterface( IID_IMediaEvent, (void **)&pME ) ) == false )
	{
		RELEASE_POINTER_WINDOW;
		return false;
	}

	if( SUCCEEDED( pGB->RenderFile( szFile, NULL ) ) == false )
	{
		RELEASE_POINTER_WINDOW;
		return false;
	}

	long lWidth, lHeight; 
	if( SUCCEEDED( pWC->GetNativeVideoSize(&lWidth, &lHeight, NULL, NULL) ) == false )
	{
		RELEASE_POINTER_WINDOW;
		return false;
	}
	RECT rcSrc, rcDest; 
	SetRect(&rcSrc, 0, 0, lWidth, lHeight); 

	if( size.x == 0 )
	{
		if( bKeepAspective == true )
		{
			float fRate = lHeight / (float)lWidth;
			GetClientRect( GAMEAPP_ST.GetHWnd(), &rcDest);

			int nNewY = (int)( (rcDest.right-rcDest.left)*fRate );
			SetRect(&rcDest, pos.x, pos.y + (rcDest.bottom-nNewY)/2, pos.x + rcDest.right, pos.y + (rcDest.bottom-nNewY)/2 + nNewY );
		}
		else
		{
			GetClientRect( GAMEAPP_ST.GetHWnd(), &rcDest);
			SetRect(&rcDest, pos.x, pos.y, pos.x + rcDest.right, pos.y + rcDest.bottom );
		}
	}
	else
	{
		SetRect(&rcDest, pos.x, pos.y, pos.x + size.x, pos.y + size.y );
	}

	if( SUCCEEDED( pWC->SetVideoPosition(&rcSrc, &rcDest) ) == false )
	{
		RELEASE_POINTER_WINDOW;
		return false;
	}

	pMC->Run();
	
	OtherSound_Off();

	return true;
}

bool nsDIRECTSHOW::InitDShowTexture( char* cFile, bool bSound )
{
	TCHAR szFile[ MAX_PATH ];
	M2W( szFile, cFile, MAX_PATH );
	return InitDShowTexture( szFile, bSound );
}

bool nsDIRECTSHOW::InitDShowTexture( TCHAR* szFile, bool bSound )
{
	if( IsPlay() == true )
		return false;

	CoInitialize( NULL );

	if( SUCCEEDED( CoCreateInstance( CLSID_FilterGraph, NULL, CLSCTX_INPROC_SERVER, IID_IGraphBuilder, (void**)&pGB ) ) == false )
	{
		RELEASE_POINTER_TEXTURE;
		return false;
	}

	if( SUCCEEDED( pGB->AddSourceFilter( szFile, _T( "Source" ), &pSrcFilter ) ) == false )
	{
		RELEASE_POINTER_TEXTURE;
		return false;
	}

	if( bSound )
	{
		// 사운드 필터 생성
		if( SUCCEEDED( CoCreateInstance( CLSID_DSoundRender, NULL, CLSCTX_INPROC_SERVER, IID_IBaseFilter, (void**)&pSoundFilter ) ) == false )
		{
			RELEASE_POINTER_TEXTURE;
			return false;
		}

		if( SUCCEEDED( pGB->AddFilter( pSoundFilter, _T( "Audio Filter" ) ) ) == false )
		{
			RELEASE_POINTER_TEXTURE;
			return false;
		}

		// 소스 필터에 사운드 필터를 붙인다.
		if( SUCCEEDED( ConnectFilters( pGB, pSrcFilter, pSoundFilter ) ) == false )
		{
			RELEASE_POINTER_TEXTURE;
			return false;
		}
	}


	// Grabber 필터 생성 - 디코더
	if( SUCCEEDED( CoCreateInstance( CLSID_SampleGrabber, NULL, CLSCTX_INPROC_SERVER, IID_IBaseFilter, (void**)&pGrabberFiler ) ) == false )
	{
		RELEASE_POINTER_TEXTURE;
		return false;
	}

	if( SUCCEEDED( pGrabberFiler->QueryInterface(IID_ISampleGrabber, (void**)&pGrabber) ) == false )
	{
		RELEASE_POINTER_TEXTURE;
		return false;
	}

	if( SUCCEEDED( pGB->AddFilter( pGrabberFiler, _T( "Sample Grabber" ) ) ) == false )
	{
		RELEASE_POINTER_TEXTURE;
		return false;
	}	

	AM_MEDIA_TYPE mt;
	ZeroMemory(&mt, sizeof(AM_MEDIA_TYPE));
	mt.formattype = FORMAT_VideoInfo; 
	mt.majortype = MEDIATYPE_Video;
	mt.subtype = MEDIASUBTYPE_ARGB32;
	if( SUCCEEDED( pGrabber->SetMediaType(&mt) ) == false )
	{
		RELEASE_POINTER_TEXTURE;
		return false;
	}	
	
	// 소스필터에 그래버 필터를 붙이고
	if( SUCCEEDED( ConnectFilters( pGB, pSrcFilter, pGrabberFiler ) ) == false )
	{
		RELEASE_POINTER_TEXTURE;
		return false;
	}

	// 널 랜더링 필터 생성
	if( SUCCEEDED( CoCreateInstance(CLSID_NullRenderer, NULL, CLSCTX_INPROC_SERVER, IID_IBaseFilter, (void **)&pNullRenderer ) ) == false )
	{
		RELEASE_POINTER_TEXTURE;
		return false;
	}

	if( SUCCEEDED( pGB->AddFilter( pNullRenderer, _T( "NullRenderer" ) ) ) == false )
	{
		RELEASE_POINTER_TEXTURE;
		return false;
	}

	// 그래버 필터에 널랜더링 필터를 붙인다.
	if( SUCCEEDED( ConnectFilters( pGB, pGrabberFiler, pNullRenderer ) ) == false )
	{
		RELEASE_POINTER_TEXTURE;
		return false;
	}

	// 컨트롤러
	if( SUCCEEDED( pGB->QueryInterface( IID_IMediaControl, (void **)&pMC ) ) == false )
	{
		RELEASE_POINTER_TEXTURE;
		return false;
	}

	// 이벤터
	if( SUCCEEDED( pGB->QueryInterface( IID_IMediaEvent, (void **)&pME ) ) == false )
	{
		RELEASE_POINTER_TEXTURE;
		return false;
	}

	// 콜백 모드에서는 사용하지 않는다.
	if( SUCCEEDED( pGrabber->SetBufferSamples( FALSE ) ) == false )
	{
		RELEASE_POINTER_TEXTURE;
		return false;
	}

	// 콜백 등록
	assert_cs( g_pGrabberCallback == NULL );
	g_pGrabberCallback = csnew cDShowGrabberCallback;
	if( SUCCEEDED( pGrabber->SetCallback( g_pGrabberCallback, 1 ) ) == false )
	{
		RELEASE_POINTER_TEXTURE;
		return false;
	}

	// 정보를 얻어 오자
	ZeroMemory(&mt, sizeof(mt));
	if( SUCCEEDED( pGrabber->GetConnectedMediaType(&mt) ) == false )
	{
		RELEASE_POINTER_TEXTURE;
		return false;
	}
	if( mt.formattype != FORMAT_VideoInfo )
	{
		RELEASE_POINTER_TEXTURE;
		return false;		
	}
	VIDEOINFOHEADER *pVih = reinterpret_cast<VIDEOINFOHEADER*>(mt.pbFormat);

	// Save the video info header
	BITMAPINFOHEADER bi;
	memcpy( &bi, &pVih->bmiHeader, sizeof(BITMAPINFOHEADER) );

	// Free the media type
	if( mt.cbFormat != 0 )
	{
		CoTaskMemFree( (PVOID)mt.pbFormat );
		// Strictly unnecessary but tidier
		mt.cbFormat = 0;
		mt.pbFormat = NULL;
	}
	if( mt.pUnk != NULL )
	{
		// Unnecessary because pUnk should not be used, but safest.
		mt.pUnk->Release();
		mt.pUnk = NULL;
	}

	// 콜백 클래스에 정보 연결
	g_pGrabberCallback->Init( &bi ); 

	pMC->Run();

	OtherSound_Off();

	return true;
}

bool nsDIRECTSHOW::UpdateDShow()
{
	if( IsPlay() == false )
		return false;
	
	OAFilterState state;
	pMC->GetState( INFINITE, &state );
	if( state == State_Stopped )
	{
		RELEASE_POINTER_ALL;
		return false;
	}

	long EventCode;
	LONG_PTR param1, param2;
	pME->GetEvent( &EventCode, &param1, &param2, 0 );
	if( EventCode == EC_COMPLETE )
	{
		RELEASE_POINTER_ALL;
		OtherSound_On();

		return false;
	}
	return true;
}

bool nsDIRECTSHOW::StopDShow()
{
	if( IsPlay() == false )
		return false;

	pMC->Stop();

	OtherSound_On();
	return true;
}

// 오직 윈도우용
void nsDIRECTSHOW::WaitForComplete_ForWindow()
{
	assert_cs( pGrabber == NULL );

	long EventCode;
	LONG_PTR param1, param2;
	pME->GetEvent( &EventCode, &param1, &param2, 0 );

	MSG msg;
	while( EventCode != EC_COMPLETE )
	{
		if( PeekMessage(&msg, NULL, 0, 0, PM_REMOVE) )
		{
			if(msg.message == WM_QUIT)
			{
				//SetNextGameStep( GAME_EXIT );
				break;
			}

			if (!TranslateAccelerator(GAMEAPP_ST.GetHWnd(), NULL, &msg))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
		else
		{
			pME->GetEvent( &EventCode, &param1, &param2, 0 );
		}
	}
	RELEASE_POINTER_WINDOW;

	OtherSound_On();
}


void nsDIRECTSHOW::WaitForKey_ForWindow( NiInputKeyboard::KeyCode SrcCode, uint nSrcModifier )
{
	OAFilterState state;
	pMC->GetState( INFINITE, &state );

	long EventCode;
	LONG_PTR param1, param2;
	pME->GetEvent( &EventCode, &param1, &param2, 0 );

	MSG msg;
	while( ( state == State_Running )&&( EventCode != EC_COMPLETE ) )
	{
		if( PeekMessage(&msg, NULL, 0, 0, PM_REMOVE) )
		{
			if(msg.message == WM_QUIT)
			{
			//	SetNextGameStep( GAME_EXIT );
				break;
			}

			if (!TranslateAccelerator(GAMEAPP_ST.GetHWnd(), NULL, &msg))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
		else
		{
			g_pEngine->m_pInput->UpdateInput();
			uint nModifier;
			if( g_pEngine->m_pInput->GetInputSystem()->GetKeyboard()->KeyWasReleased( SrcCode, nModifier ) )
			{
				if( nModifier == nSrcModifier )
					pMC->Stop();
			}
			pMC->GetState( INFINITE, &state );
			pME->GetEvent( &EventCode, &param1, &param2, 0 );			
		}		
	}
	RELEASE_POINTER_ALL;
	g_pEngine->m_pInput->UpdateInput();

	OtherSound_On();
}

void nsDIRECTSHOW::WaitForKey_ForWindow_AniKey()
{
	OAFilterState state;
	pMC->GetState( INFINITE, &state );

	long EventCode;
	LONG_PTR param1, param2;
	pME->GetEvent( &EventCode, &param1, &param2, 0 );

	MSG msg;
	while( ( state == State_Running )&&( EventCode != EC_COMPLETE ) )
	{
		if( PeekMessage(&msg, NULL, 0, 0, PM_REMOVE) )
		{
			
			if(msg.message == WM_QUIT)
			{
				break;
			}

			if (!TranslateAccelerator(GAMEAPP_ST.GetHWnd(), NULL, &msg))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);

				if(msg.message == WM_KEYDOWN )
				{
					pMC->Stop();
				}
			}
		}
		else
		{
			pMC->GetState( INFINITE, &state );
			pME->GetEvent( &EventCode, &param1, &param2, 0 );			
		}		
	}
	RELEASE_POINTER_ALL;
	g_pEngine->m_pInput->UpdateInput();

	OtherSound_On();	
}

void nsDIRECTSHOW::OtherSound_Off()
{
	if( g_pSoundMgr )
	{
		g_pSoundMgr->Set_BGM_Volume( 0.0f );
		g_pSoundMgr->Set_EFM_Volume( 0.0f );
		g_pSoundMgr->Set_System_Volume( 0.0f );
	}
}

void nsDIRECTSHOW::OtherSound_On()
{
	if( g_pSoundMgr && g_pResist )
	{
		g_pSoundMgr->Set_BGM_Volume( g_pResist->GetMusic() );
		g_pSoundMgr->Set_EFM_Volume( g_pResist->GetSound() );
		g_pSoundMgr->Set_System_Volume( g_pResist->GetSystemSound() );
	}
}


#undef RELEASE_POINTER_WINDOW
#undef RELEASE_POINTER_TEXTURE
#undef RELEASE_POINTER_ALL