
#pragma once 


#define		TOTAL_WIDTH					1680
#define		TOTAL_HEIGHT				1050
#define		VIEWER_WIDTH				1280
#define		VIEWER_HEIGHT				1015

#define		MAX_LEVEL					99

#define		INVALIDE_FT_ID				( (DWORD)-1 )


extern CsPoint	g_ptScreen;






// 조그레스 Define 각 국가 헤더로 이동, 조그레스 관련 문제 발생 시 한국,GSP,태국은 조그레스 Define 주석 해제 14.03.19 kimhoshok


#define SAFE_NIDELETE(p)       { if(p) { NiDelete (p);     (p)=NULL; } }
#define SAFE_NIDELETE_ARRAY(p) { if(p) { NiDelete[] (p);   (p)=NULL; } }
