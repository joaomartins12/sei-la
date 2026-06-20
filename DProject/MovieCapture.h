#pragma once

// #include "bandicap.h"
// class MovieCapture
// {
// 
// protected:
// 	LPDIRECT3DDEVICE9		m_lpDevice9;
// 	CBandiCaptureLibrary	m_BandiLibrary;
// 
// 	bool	m_bInit;		// 녹화가 가능하면 생성시 true가 되어 있음.
// 	bool	m_bRecording;
// 
// 
// 
// public:
// 	HRESULT Initialize(LPDIRECT3DDEVICE9 pD3d);
// 	void RecordRender(LPDIRECT3DDEVICE9 pD3d);
// 
// 	void Release();
// 
// 
// public:
// 	void StartRecord();		// 동영상 녹화를 시작합니다.
// 	void RecordLoop();		// 내부에서 녹화 여부를 결정한다.
// 
// 
// public:
// 	static MovieCapture& GetInstance()
// 	{
// 		static MovieCapture _kInstance;
// 		return _kInstance;
// 	}
// 
// private:
// 	MovieCapture();
// public:
// 	~MovieCapture();
// };
// 
// 
