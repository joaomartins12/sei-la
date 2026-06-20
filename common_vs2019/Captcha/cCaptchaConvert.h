#pragma once

#pragma pack(push, 1)
struct stTGAHEADER				// TGA파일헤더 포맷 18바이트
{
	unsigned char  IdentSize;			// 식별필드 길이
	unsigned char  ColorMapType;		// 항상0
	unsigned char  ImageType;			// 2 압축되지않은RGB
	unsigned short ColorMapStart;		// 항상0
	unsigned short ColorMapLength;		// 0
	unsigned char  ColorMapBits;		// 0
	unsigned short XStart;				// 
	unsigned short YStart;				// 
	unsigned short Width;				// 
	unsigned short Height;				// 
	unsigned char  Bits;				// 24
	unsigned char  Descriptor;			// 
};
#pragma pack(pop)


class cCaptchaConvert
{
public:
	static void ConvertToTga(const char *pCompressData, char *pOutData);	//pOutData는 CAPTCHA_TGA_SIZE 의 크기 필요
	static void ConvertToBmp(const char *pCompressData, char *pOutData); //pOutData는 CAPTCHA_BMP_SIZE 의 크기 필요
};