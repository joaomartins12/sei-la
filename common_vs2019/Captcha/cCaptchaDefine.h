#pragma once
#define CAPTCHA_WIDTH 256
#define CAPTCHA_HEIGHT 64
#define CAPTCHA_QUESTION_LENGTH 6
#define CAPTCHA_IMGSIZE (CAPTCHA_WIDTH*CAPTCHA_HEIGHT)
//색 16개가 한계입니다
#define CAPTCHA_COLORCOUNT 16
#define CAPTCHA_BACKGROUND_COLOR_IDX 0
#define CAPTCHA_CURVES_LENGTH 256
#define CAPTCHA_TGA_SIZE (18+(CAPTCHA_IMGSIZE*3))
#define CAPTCHA_BMP_SIZE (54+(CAPTCHA_IMGSIZE*3))
#define CAPTCHA_NOISE_DOT_COUNT 150
#define CAPTCHA_NOISE_CURVES_COUNTS 10
#define CAPTCHA_NOISE_LINE_COUNT 2


//BGR순임
const unsigned char g_ArrColorPalette[CAPTCHA_COLORCOUNT][3] =
{
	{ 255,255,255 }	//흰색
	,{ 0,0,0 }		//검정색
	,{ 255,0,0 }	//빨간색
	,{ 0,0,255 }	//파랑색
	,{ 255,0,255 }	//
	,{ 26,102,30 }	//
	,{ 18,68,152 }	//
	,{ 168,26,39 }	//
	,{ 116,44,158 }//
	,{ 181,23,79 }	//
	,{ 0,0,128 }
	,{ 162,51,181}
	,{ 244,0,98 }
	,{ 0,103,242 }
	,{ 121,21,255 }
	,{ 70,141,46 }	
};
