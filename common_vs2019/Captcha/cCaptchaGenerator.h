#pragma once
#include "cCaptFont.h"
#include "cCaptchaDefine.h"
class cRandom;

namespace nsCaptcha
{
	bool IsCharactor(char c);
	bool IsNumber(char c);
}


class cCaptchaGenerator
{
public:
	cCaptchaGenerator();
	~cCaptchaGenerator();
public:
	
	void Generate(int nRandKey , int &nCompressedSizeOut , char** ppDataOut , char* szAnswerOut);
	//nRandKey - 생성Seed  , nCompressedSizeOut - 압축된 바이너리 크기(byte) 
	//ppDataOut - 데이터(내부에서 new) ,  szAnswerOut - CAPTCHA_QUESTION_LENGTH + 1 이상의 크기를 가지고있는 버퍼
	//사용후 반드시 ReleaseData로 해제해줘야함

	void ReleaseData(char* pData);
	// Generate 에서 생성된 ppDataOut을 해제

	bool LoadFont(char* pStream, unsigned int nStreamSize) { return m_fontFile.LoadFromDataStream((unsigned char*)pStream, nStreamSize); };


	//static void ConvertToTga(char *pCompressData , int nCompressSize , char *pOutData);	//pOutData는 CAPTCHA_TGA_SIZE 의 크기 필요
	//static void ConvertToBmp(char *pCompressData , int nCompressSize, char *pOutData); //pOutData는 CAPTCHA_BMP_SIZE 의 크기 필요
	bool Load();
private:
	void GetQuestion(cRandom &cRand , char *szQuestion, unsigned int *nQuestionIndex);
	int DrawCharacter(cRandom &cRand , char *pImgBuff , int nCodeIdx , int nPosX , int nPosY , int nSk1 , int nSk2);
	void DrawLine(cRandom &cRand , char*pImgBuff);
	void DrawDots(cRandom &cRand, char*pImgBuff);
	void DrawCurves(cRandom &cRand, char*pImgBuff);

	cFontFile m_fontFile;
};

extern cCaptchaGenerator* g_pCaptchaGenerator;