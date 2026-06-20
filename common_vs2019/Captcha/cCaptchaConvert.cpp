#include"cCaptchaConvert.h"
#include "cCaptchaDefine.h"
#include <Windows.h>
#include "QuickLz.h"

void cCaptchaConvert::ConvertToTga( const char * pCompressData, char * pOutData)
{
	char *pDecompBuff = new char[CAPTCHA_IMGSIZE / 2];
	//LZ4_decompress_fast(pCompressData, pDecompBuff, CAPTCHA_IMGSIZE / 2);
	qlz_state_decompress *state_decompress = new qlz_state_decompress;
	qlz_decompress(pCompressData, pDecompBuff, state_decompress);
	delete state_decompress;

	stTGAHEADER head;
	memset(&head, 0, sizeof(head));
	head.ImageType = 2;
	head.Width = CAPTCHA_WIDTH;
	head.Height = CAPTCHA_HEIGHT;
	head.Bits = 24;
	memcpy(pOutData, &head, sizeof(head));
	pOutData += sizeof(head);

	for (int i = 0; i < CAPTCHA_HEIGHT; i++)
	{
		for (int j = 0; j < CAPTCHA_WIDTH; j++)
		{
			int nArrPos = ((CAPTCHA_HEIGHT - i - 1)*CAPTCHA_WIDTH + j) / 2;
			char nColor = j % 2 ? pDecompBuff[nArrPos] & 0x0F : (pDecompBuff[nArrPos] & 0xF0) >> 4 ;
			memcpy(pOutData, g_ArrColorPalette[nColor], 3);
			pOutData += 3;
		}		
	}

	delete[] pDecompBuff;
}

void cCaptchaConvert::ConvertToBmp(const char * pCompressData, char * pOutData)
{
	char *pDecompBuff = new char[CAPTCHA_IMGSIZE/2];
	//LZ4_decompress_fast(pCompressData, pDecompBuff, CAPTCHA_IMGSIZE / 2);

	qlz_state_decompress *state_decompress = new qlz_state_decompress;
	qlz_decompress(pCompressData, pDecompBuff, state_decompress);
	delete state_decompress;

	BITMAPFILEHEADER *cBitHeader = (BITMAPFILEHEADER*)pOutData;
	memset(cBitHeader, 0, sizeof(BITMAPFILEHEADER));
	cBitHeader->bfType = 0x4D42;
	cBitHeader->bfSize = CAPTCHA_BMP_SIZE;
	cBitHeader->bfOffBits = 0x36;	

	pOutData += sizeof(BITMAPFILEHEADER);

	BITMAPINFOHEADER *cBitInfo = (BITMAPINFOHEADER*)pOutData;
	memset(cBitInfo, 0, sizeof(BITMAPINFOHEADER));
	cBitInfo->biSize = sizeof(BITMAPINFOHEADER);
	cBitInfo->biWidth = CAPTCHA_WIDTH;
	cBitInfo->biHeight = CAPTCHA_HEIGHT;
	cBitInfo->biPlanes = 1;
	cBitInfo->biSizeImage = CAPTCHA_IMGSIZE * 3;
	cBitInfo->biBitCount = 24;
	cBitInfo->biXPelsPerMeter = 0; //?
	cBitInfo->biYPelsPerMeter = 0; //?

	pOutData += sizeof(BITMAPINFOHEADER);


	for (int i = 0; i < CAPTCHA_HEIGHT; i++)
	{
		for (int j = 0; j < CAPTCHA_WIDTH; j++)
		{
			int nArrPos = ((CAPTCHA_HEIGHT - i - 1)*CAPTCHA_WIDTH + j) / 2;
			char nColor = j % 2 ? pDecompBuff[nArrPos] & 0x0F : (pDecompBuff[nArrPos] & 0xF0) >> 4;
			memcpy(pOutData, g_ArrColorPalette[nColor], 3);
			pOutData += 3;
		}
	}
	delete[] pDecompBuff;
}