#include "cCaptchaGenerator.h"
#include "cCaptchaDefine.h"
#include "nlib/cRandom.h"
#include "nlib/base.h"
#include <algorithm>
#include "QuickLz.h"


#define RAND_RANGE(rand , min , max) (rand.Random()%(max-min)+min)
#define MAX(x,y)				((x>y)?(x):(y))

cCaptchaGenerator* g_pCaptchaGenerator = NULL;

const char CURVES_DATA[CAPTCHA_CURVES_LENGTH] =
{ 0,3,6,9,12,15,18,21,24,27,30,33,36,39,42,45,48,51,54,57,59,62,65,67,70,73,75,78,80,82,85,87,89,91,94,96,98,100,102,103,105,107,108,110,112,113,114,116,117,118,119,120,121,122,123,123,124,125,125,126,126,126,126,126,127,126,126,126,126,126,125,125,124,123,123,122,121,120,119,118,117,116,114,113,112,110,108,107,105,103,102,100,98,96,94,91,89,87,85,82,80,78,75,73,70,67,65,62,59,57,54,51,48,45,42,39,36,33,30,27,24,21,18,15,12,9,6,3,0,-3,-6,-9,-12,-15,-18,-21,-24,-27,-30,-33,-36,-39,-42,-45,-48,-51,-54,-57,-59,-62,-65,-67,-70,-73,-75,-78,-80,-82,-85,-87,-89,-91,-94,-96,-98,-100,-102,-103,-105,-107,-108,-110,-112,-113,-114,-116,-117,-118,-119,-120,-121,-122,-123,-123,-124,-125,-125,-126,-126,-126,-126,-126,-127,-126,-126,-126,-126,-126,-125,-125,-124,-123,-123,-122,-121,-120,-119,-118,-117,-116,-114,-113,-112,-110,-108,-107,-105,-103,-102,-100,-98,-96,-94,-91,-89,-87,-85,-82,-80,-78,-75,-73,-70,-67,-65,-62,-59,-57,-54,-51,-48,-45,-42,-39,-36,-33,-30,-27,-24,-21,-18,-15,-12,-9,-6,-3, };


cCaptchaGenerator::cCaptchaGenerator()
{
	
}

cCaptchaGenerator::~cCaptchaGenerator()
{
	
}

void cCaptchaGenerator::Generate(int nRandKey, int & nCompressedSizeOut, char ** ppDataOut, char* szAnswer)
{
	char *pImgBuffer = new char[CAPTCHA_IMGSIZE];
	memset(pImgBuffer, CAPTCHA_BACKGROUND_COLOR_IDX , CAPTCHA_IMGSIZE);
	cRandom cRand;
	cRand.SetRandomSeed(nRandKey);

	unsigned int nQuestionIndex[CAPTCHA_QUESTION_LENGTH] = { 0 , };

	GetQuestion(cRand , szAnswer, nQuestionIndex);	

	int nXOffset = RAND_RANGE(cRand , 10, CAPTCHA_WIDTH - 235);
	int nYOffset = RAND_RANGE(cRand , 8, CAPTCHA_HEIGHT - 50);
	int nSk1 = cRand.Random() & 0x7f;
	int nSk2 = cRand.Random() & 0x3f;

	for (int i = 0; i<CAPTCHA_QUESTION_LENGTH; i++)
	{
		nXOffset = DrawCharacter(cRand , pImgBuffer , nQuestionIndex[i] , nXOffset, nYOffset, nSk1, nSk2);
	}
	DrawLine(cRand, pImgBuffer);	
	DrawDots(cRand, pImgBuffer);
	DrawCurves(cRand, pImgBuffer);

	char *pImgOut = new char[CAPTCHA_IMGSIZE / 2];	
	memset(pImgOut, 0, CAPTCHA_IMGSIZE / 2);

	for (int i = 0; i < CAPTCHA_IMGSIZE; i++)
	{
		pImgOut[i / 2] |= i % 2 ? pImgBuffer[i] : (pImgBuffer[i] << 4);
	}

	char *pImgComp = new char[CAPTCHA_IMGSIZE / 2];
	memset(pImgComp, 0, CAPTCHA_IMGSIZE / 2);

	/*FILE *p;
	fopen_s(&p, "img", "wb");
	fwrite(pImgOut, CAPTCHA_IMGSIZE / 2, 1, p);
	fclose(p);*/

	//nCompressedSize = LZ4_compress_default(pImgOut, pImgComp, CAPTCHA_IMGSIZE / 2 , CAPTCHA_IMGSIZE / 2);
	qlz_state_compress *state_compress = new qlz_state_compress;
	nCompressedSizeOut = qlz_compress(pImgOut, pImgComp, CAPTCHA_IMGSIZE / 2, state_compress);
	delete state_compress;

		
	*ppDataOut = pImgComp;
	delete[] pImgOut;
	delete[] pImgBuffer;
}

void cCaptchaGenerator::ReleaseData(char * pData)
{
	delete[] pData;
}

void cCaptchaGenerator::GetQuestion(cRandom &cRand, char * szQuestion , unsigned int *nQuestionIndex)
{
	int totalFontCounts = m_fontFile.GetCodeCounts();
	int codeIndex[CAPTCHA_QUESTION_LENGTH] = { 0 };

	for (int i = 0; i<CAPTCHA_QUESTION_LENGTH; i++)
	{
		int counts = 0;
		int answer = 0;
		do
		{
			answer = cRand.Random() % totalFontCounts;
			bool same = false;
			for (int j = 0; j<i; j++)
			{
				if (answer == codeIndex[j])
				{
					same = true;
					break;
				}
			}
			counts++;
			if (!same) break;
		} while (counts<1000);

		codeIndex[i] = answer;
		nQuestionIndex[i] = answer;
		szQuestion[i] = m_fontFile.GetCodeFromIndex(answer);
	}	
}

int cCaptchaGenerator::DrawCharacter(cRandom & cRand, char * pimgBuff, int nCodeIdx, int nPosX, int nPosY, int nSk1, int nSk2)
{
	char *pLineBegin = pimgBuff + CAPTCHA_WIDTH*nPosY;			//y 시작축
	char *pLineStart = pimgBuff + CAPTCHA_WIDTH*nPosY + nPosX;//x,y 시작축
	char nColor = (cRand.Random() % (CAPTCHA_COLORCOUNT - 1)) + 1;

	char *pCur = pLineStart;
	int sk1 = nSk1 + nPosX;
	int sk2 = nSk2 + nPosX;
	int nMPos = nPosX;
	int nRow = 0;	
	
	for (char *pFont = m_fontFile.GetCodeGlyphFromIndex(nCodeIdx); *pFont != cFontFile::GLPYH_EOF; pFont++)
	{
		if (*pFont<0)
		{
			if (*pFont == cFontFile::GLPYH_EOL)
			{
				pLineBegin += CAPTCHA_WIDTH;
				pLineStart += CAPTCHA_WIDTH;
				pCur = pLineStart;
				sk1 = nSk1 + nPosX;
				nRow++;
			}
			else
			{
				pCur += -*pFont;
			}
			continue;
		}

		if (sk1 >= CAPTCHA_CURVES_LENGTH) sk1 = sk1 % CAPTCHA_CURVES_LENGTH;
		int skew = CURVES_DATA[sk1] >> 4;
		sk1 += RAND_RANGE(cRand , 1, 2);

		if (sk2 >= CAPTCHA_CURVES_LENGTH) sk2 = sk2 % CAPTCHA_CURVES_LENGTH;
		int skewh = CURVES_DATA[sk2] >> 6;
		sk2 += cRand.Random() & 0x1;

		char *x = pCur + skew * CAPTCHA_WIDTH + skewh;
		nMPos = MAX(nMPos,  nPosX + (int)(pCur - pLineStart) );

		if ((x - pimgBuff)<CAPTCHA_HEIGHT*CAPTCHA_WIDTH && (pCur - pLineBegin + skewh)<CAPTCHA_WIDTH)
		{			
			*x = nColor;
		}
		pCur++;
	}	
	return nMPos;
}

void cCaptchaGenerator::DrawLine(cRandom & cRand, char * pImgBuff)
{
	for (int i = 0; i < CAPTCHA_NOISE_LINE_COUNT ; i++)
	{
		int sk = cRand.Random() & 0X7F;
		char nColor = (cRand.Random() % (CAPTCHA_COLORCOUNT - 1)) + 1;

		int line_start_y = RAND_RANGE(cRand, 20, CAPTCHA_HEIGHT - 20);
		for (int x = 0; x<CAPTCHA_WIDTH - 1; x++)
		{
			if (sk >= CAPTCHA_CURVES_LENGTH) sk = sk%CAPTCHA_CURVES_LENGTH;
			int skew = CURVES_DATA[sk] >> 4;
			sk += (cRand.Random() & 1) ? 0 : 5;

			char *i = pImgBuff + (CAPTCHA_WIDTH*(line_start_y + skew) + x);

			i[0] = nColor;
			i[1] = nColor;
			i[CAPTCHA_WIDTH] = nColor;
			i[CAPTCHA_WIDTH + 1] = nColor;
		}
	}	
}

void cCaptchaGenerator::DrawDots(cRandom & cRand, char * pImgBuff)
{
	for (int n = 0; n<CAPTCHA_NOISE_DOT_COUNT; n++)
	{
		unsigned int v = (unsigned int)cRand.Random();
		char *i = pImgBuff + v % (CAPTCHA_WIDTH*(CAPTCHA_HEIGHT - 3));
		char nColor = (cRand.Random() % (CAPTCHA_COLORCOUNT - 1)) + 1;

		i[0] = nColor;
		i[1] = nColor;
		//i[2] = nColor;
		i[CAPTCHA_WIDTH + 0] = nColor;
		i[CAPTCHA_WIDTH + 1] = nColor;
		//i[CAPTCHA_WIDTH + 2] = nColor;
	}
}

void cCaptchaGenerator::DrawCurves(cRandom & cRand, char * pImgBuff)
{
	for (int n = 0; n<CAPTCHA_NOISE_CURVES_COUNTS; n++)
	{
		char nColor = (cRand.Random() % (CAPTCHA_COLORCOUNT - 1)) + 1;
		int rand_x = RAND_RANGE(cRand , 5, CAPTCHA_WIDTH - 10);
		int rand_y = RAND_RANGE(cRand, 5, CAPTCHA_HEIGHT - 10);
		int len = RAND_RANGE(cRand, 10, 15);
		int sk = cRand.Random() & 0x3f;

		for (int ii = 0; ii<len; ii++)
		{
			if (sk >= CAPTCHA_CURVES_LENGTH) sk = sk%CAPTCHA_CURVES_LENGTH;
			int skew = CURVES_DATA[sk] >> 4;
			sk += (cRand.Random() & 1) ? 0 : 5;

			char *i = pImgBuff + (CAPTCHA_WIDTH*(rand_y + skew) + rand_x + ii);
			if ((i - pImgBuff)<CAPTCHA_HEIGHT*CAPTCHA_WIDTH)
			{
				i[0] = nColor;
			}
		}
	}
}



bool cCaptchaGenerator::Load()
{	
	FILE *pFile = NULL;
	fopen_s(&pFile, GetDataPath("Captcha_Font.cpf") , "rb");

	xstop( pFile , "Captcha Fond File Not Found");

	fseek(pFile, 0, SEEK_END);
	unsigned int nFileSize = ftell(pFile);
	fseek(pFile, 0, SEEK_SET);

	char *pFontBuffer = new char[nFileSize];

	fread(pFontBuffer, 1, nFileSize, pFile);
	fclose(pFile);
	
	bool bLoadRet = LoadFont(pFontBuffer , nFileSize);
	delete[] pFontBuffer;

	return bLoadRet;
}

bool nsCaptcha::IsCharactor(char c)
{
	if(c >= 0x41 && c <= 0x5a)
	{
		return true;
	}
	if(c >=0x61 && c <= 0x7a)
	{
		return true;
	}
	return false;
}

bool nsCaptcha::IsNumber(char c)
{
	if(c >= 0x30 && c <= 0x39)
	{
		return true;
	}
	return false;
}
