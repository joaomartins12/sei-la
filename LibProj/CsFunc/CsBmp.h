
#pragma once 

struct sCS_BMPDATA
{
	sCS_BMPDATA(){ s_pData = NULL; }
	BITMAPINFOHEADER	s_InfoHeader;
	BYTE*				s_pData;
};

class CsBmp
{
public:
	static bool		DeleteData( sCS_BMPDATA* pData );
	static bool		GetBitmapSize( int& nWidth, int& nHeight, LPCTSTR szBitmap );
	static bool		GetBitmapBit( int& nBit, LPCTSTR szBitmap );
	static bool		GetBmpData( LPCTSTR str, sCS_BMPDATA* pData );	
	static bool		IsBmpData_GrayMap( LPCTSTR str );
	static bool		GetBmpData_GrayMap( LPCTSTR str, sCS_BMPDATA* pData );	
};