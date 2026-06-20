

#include "stdafx.h"
#include "DShowGrabberCallback.h"

cDShowGrabberCallback* g_pGrabberCallback;


void cDShowGrabberCallback::Init( BITMAPINFOHEADER* bi )
{
	assert_cs( bi->biBitCount == 32 );
	NiTexture::FormatPrefs kPrefs;
	kPrefs.m_ePixelLayout = NiTexture::FormatPrefs::TRUE_COLOR_32;
	kPrefs.m_eMipMapped = NiTexture::FormatPrefs::NO;
	kPrefs.m_eAlphaFmt = NiTexture::FormatPrefs::ALPHA_DEFAULT;
	m_pDShowTex = NiDynamicTexture::Create( bi->biWidth, bi->biHeight, kPrefs );
	assert_cs( m_pDShowTex != NULL );

	m_nWidth = bi->biWidth;
	m_nHeight = bi->biHeight;
}

void cDShowGrabberCallback::Delete()
{
	m_pDShowTex = 0;
}

STDMETHODIMP cDShowGrabberCallback::BufferCB(double SampleTime, BYTE *pBuffer, long nBufferLen)
{
	_SYNCHRONIZE_THREAD_(&m_CS);

	if( m_pDShowTex == NULL )
		return S_OK;	

	int nPitch = 0;
	BYTE* pSrc = (BYTE*)m_pDShowTex->Lock( nPitch );
	if( pSrc == NULL )
		return S_OK;

	int nTexSize = nPitch / 4;
	assert_cs( nTexSize >= m_nWidth );
	assert_cs( nTexSize >= m_nHeight );

	int wl = m_nWidth*4;
	int tl = nTexSize*4;
	int tht = m_nHeight - 1;
	for( int i=0; i<m_nHeight; ++i )
	{
		memcpy( &pSrc[ tl*( tht - i ) ], &pBuffer[ i*wl ], sizeof( BYTE )*wl );
	}	
	m_pDShowTex->UnLock();

	return S_OK;
}