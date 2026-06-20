

#pragma once

#define		TOTAL_WIDTH					1280
#define		TOTAL_HEIGHT				1024

// 3D 윈도우를 사용할 윈도우 인덱스
#define		PROC_3D_PANE_X				0
#define		PROC_3D_PANE_Y				0

#define		PROC_3D_WIN_WIDTH			1024
#define		PROC_3D_WIN_HEIGHT			768


#define SAFE_NIDELETE(p)       { if(p) { NiDelete (p);     (p)=NULL; } }
#define SAFE_NIDELETE_ARRAY(p) { if(p) { NiDelete[] (p);   (p)=NULL; } }


extern CsPoint	g_ptScreen;