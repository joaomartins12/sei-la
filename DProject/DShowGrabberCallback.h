

#pragma once

#if(_WIN32_WINNT >= 0x0400) || (_WIN32_WINDOWS > 0x0400)

typedef
VOID
(APIENTRY *PAPCFUNC)(
					 __in ULONG_PTR dwParam
					 );

WINBASEAPI
DWORD
WINAPI
QueueUserAPC(
			 __in PAPCFUNC pfnAPC,
			 __in HANDLE hThread,
			 __in ULONG_PTR dwData
			 );
#endif /* _WIN32_WINNT >= 0x0400 || _WIN32_WINDOWS > 0x0400 */

#pragma include_alias( "dxtrans.h", "qedit.h" )
#define __IDxtCompositor_INTERFACE_DEFINED__
#define __IDxtAlphaSetter_INTERFACE_DEFINED__
#define __IDxtJpeg_INTERFACE_DEFINED__
#define __IDxtKey_INTERFACE_DEFINED__
#include "Qedit.h"


class cDShowGrabberCallback : public ISampleGrabberCB
{
public:
	cDShowGrabberCallback(){ m_pDShowTex = NULL; }
	virtual ~cDShowGrabberCallback(){ Delete(); }
public:
	STDMETHOD(BufferCB)(double SampleTime, BYTE *pBuffer, long nBufferLen);
	STDMETHOD(SampleCB)(double, IMediaSample *){return S_OK;}
	STDMETHOD(QueryInterface)( REFIID iid, LPVOID *ppv )
	{
		if( iid == IID_ISampleGrabberCB || iid == IID_IUnknown )
		{ 
			*ppv = (void *) static_cast<ISampleGrabberCB*>( this );
			return NOERROR;
		} 
		return E_NOINTERFACE;
	}

	STDMETHOD_( ULONG, AddRef )(){return 2;}
	STDMETHOD_( ULONG, Release)(){return 1;}

protected:
	int						m_nWidth;
	int						m_nHeight;
	NiDynamicTexturePtr		m_pDShowTex;

	// ÅØ½ºÃÄ Žô°É¶§´Â ¾È±×·ÁÁö±â ¶§¹®¿¡ Å©¸®Æ¼ÄÃ ¼½¼ÇÀ» ´«¹°À» ¸Ó±Ý°í ÁÖÀÚ
	CsCriticalSection		m_CS;

public:
	void					Init( BITMAPINFOHEADER* bi );
	void					Delete();

	NiDynamicTexture*		GetDShowTex(){ return m_pDShowTex; }
	CsCriticalSection*		GetCliticalSection(){ return &m_CS; }
};

extern cDShowGrabberCallback* g_pGrabberCallback;