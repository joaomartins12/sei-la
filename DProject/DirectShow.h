

#pragma once 

#include "DShowGrabberCallback.h"

namespace nsDIRECTSHOW
{
	void	GlobalShotDown();

	bool	IsPlay();
	bool	InitDShowWindow( TCHAR* szFile, CsPoint pos, CsPoint size, bool bKeepAspective );
	bool	InitDShowTexture( char* cFile, bool bSound );
	bool	InitDShowTexture( TCHAR* szFile, bool bSound );

	bool	UpdateDShow();
	bool	StopDShow();

	void	WaitForComplete_ForWindow();
	//void	WaitForKey_ForWindow();
	void	WaitForKey_ForWindow( NiInputKeyboard::KeyCode SrcCode, uint nSrcModifier );
	void	WaitForKey_ForWindow_AniKey();

	void	OtherSound_Off();
	void	OtherSound_On();
}
