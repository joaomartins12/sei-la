#include "stdafx.h"
// #include "MovieCapture.h"
// 
// MovieCapture::MovieCapture()
// : m_lpDevice9(NULL)
// , m_bRecording(false)
// , m_bInit(false)
// {
// 
// }
// 
// 
// MovieCapture::~MovieCapture()
// {
// 	Release();
// }
// 
// void MovieCapture::Release()
// {
// 	if( !m_bInit )
// 		return;
// 
// 	if( m_BandiLibrary.IsCapturing() )
// 		m_BandiLibrary.Stop();
// 
// 	m_BandiLibrary.Destroy();
// }
// 
// 
// HRESULT MovieCapture::Initialize( LPDIRECT3DDEVICE9 pD3d )
// {
// 	if(!pD3d)
// 		return E_FAIL;
// 
// 	m_lpDevice9 = pD3d;
// 
// 	if(FAILED(m_BandiLibrary.Create(_T("bdcap32.dll"))))
// 	{
// 		MessageBoxA(NULL, "bdcap32 못찾음", "MovieCapture::Initialize", MB_OK);
// 		return E_FAIL;
// 	}
// 	
// 
// 	// Config
// 	BCAP_CONFIG cfg;
// 
// 	cfg.VideoSizeW = 0; // half size
// 	cfg.VideoSizeH = 0;
// 	cfg.VideoCodec = FOURCC_MP4V;
// 	cfg.VideoKBitrate = 5000;
// 	cfg.VideoQuality = 100;
// 	cfg.VideoRateControl = BCAP_CONFIG::V_CBR;
// 
// 	cfg.AudioChannels = 2;
// 	cfg.AudioSampleRate = 48000;
// 	cfg.AudioCodec = WAVETAG_MP2;
// 	cfg.IncludeCursor = TRUE;
// 	cfg.VideoFPS = 60;			// 다들 PC 사양 좋은듯?
// 
// 	m_BandiLibrary.CheckConfig(&cfg);
// 	m_BandiLibrary.SetConfig(&cfg);
// 	m_BandiLibrary.SetMinMaxFPS(30, 60);
// 
// 	m_bInit = true;
// 
// 	return S_OK;
// }
// 
// 
// void MovieCapture::StartRecord()
// {
// 	SYSTEMTIME	st;
// 	GetLocalTime(&st);
// 
// 	TCHAR buf2[MAX_PATH] ={0};
// 
// 	wsprintf(buf2, _T("DigimonMasters_%02d-%02d-%02d %02d-%02d-%02d.mp4"), 
// 		st.wYear-2000, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);
// 
// 	// Path
// 	TCHAR dir[MAX_PATH] = {0,};
// 	GetCurrentDirectory(MAX_PATH, dir);
// 	_tcscat(dir, _T("\\Movie\\"));
// 
// 	CreateDirectory(dir, NULL);
// 	_tcscat(dir, buf2);
// 
// 	m_BandiLibrary.Start(dir, NULL, BCAP_MODE_D3D9_SCALE, (LONG_PTR)m_lpDevice9);
// }
// 
// void MovieCapture::RecordRender( LPDIRECT3DDEVICE9 pD3d )
// {
// 	if( !m_bInit || !pD3d )
// 		return;
// 
// 	// 캡쳐 중이 아니면 리턴
// 	if( !m_BandiLibrary.IsCapturing() )
// 		return;		
// 
// 	// 캡쳐 중일 때 프레임을 저장합니다.
// 	m_BandiLibrary.Work( (LONG_PTR)pD3d );
// 
// 	m_lpDevice9 = pD3d;
// 	//g_pEngine->DrawText_OutLine( 0, 0, 0xffffffff, DT_LEFT, _T( "디지몬 마스터즈를 녹화하고 있습니다." ) );
// }
// 
// 
// 
// void MovieCapture::RecordLoop()
// {
// 	if( !m_bInit )
// 		return;
// 
// 	// 캡쳐 중에 키 입력 시 Stop
// 	if( m_BandiLibrary.IsCapturing())
// 	{
// 		m_BandiLibrary.Stop();
// 		return;
// 	}
// 
// 	else
// 	{
// 		StartRecord();
// 	}
// }