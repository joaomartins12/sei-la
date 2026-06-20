
#pragma once

#pragma pack(1)
struct TARGAFILEHEADER
{
	unsigned char		umageIDLength;		// 식별 필드의 길이 - 0이면 식별 필드가 포함되지 않음
	unsigned char		colorMapType;		// 색상 맵의 종류 : 항상 0
	unsigned char		imageTypeCode;		// 2 : 압축되지 않은 RGB
											// 3 : 압축되지 않은 그레이스케일
											// 10 : RLE( Run - Length Encoding ) 방식으로 압축된 RGB
											// 11 : RLE( Run - Length Encoding ) 방식으로 압축된 그레이스케일
	short int			colorMapOrigin;		// 색상맵의 시작 위치 ( 하위 - 상위 )
	short int			colorMapLength;		// 색상맵의 길이 ( 하위 - 상위 )
	unsigned char		colorMapEntrySize;	// 색상맵의 항목 크기 ( 하위 - 상위 )
	short int			imageXOrigin;		// 이미지 우측 하단 x좌표 ( 하위 - 상위 )
	short int			imageYOrigin;		// 이미지 좌측 하단 x좌표 ( 하위 - 상위 )
	short int			imageWidth;			// 이미지의 픽셀단위 너비 ( 하위 - 상위 )
	short int			imageHeight;		// 이미지의 픽셀단위 높이 ( 하위 - 상위 )
	unsigned char		bitCount;			// 색상 비트수 : 16, 24, 32
	unsigned char		imageDescriptor;	// 24비트 : 0x00, 32비트 : 0x08
};
#pragma pack()

struct sCS_TARGADATA
{
	sCS_TARGADATA(){ s_pData = NULL; }
	TARGAFILEHEADER		s_Header;
	BYTE*				s_pData;
};

class CsTarga
{
protected:
public:
	static void	DeleteTgaData( sCS_TARGADATA* pData );

	static bool	GetSize( sCS_TARGADATA* pOutData, LPCTSTR szPath );

	static bool	CreateTgaData( sCS_TARGADATA* pOutData, int nWidth, int nHeight, int nBitCount, DWORD dwInitColor );
	static bool	CreateTgaData( sCS_TARGADATA* pOutData, TCHAR* szFileName );
	static void CreateFileFromTgaData( LPCTSTR szFilePath, sCS_TARGADATA* pData );
};