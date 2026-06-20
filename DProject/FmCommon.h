


#pragma once

namespace FMCommon
{
	float	CalDist_2D( CsC_AvObject* pSrc, CsC_AvObject* pDest );
	float	CalDist( CsC_AvObject* pSrc, CsC_AvObject* pDest );

	int		GetSkillAtt( DWORD nSkillID, int nSkillLevel, int nApplyIndex );
	bool	GetSkillAttStr( int nA, int nLen, TCHAR* sz, int nB = 0 );
	bool	GetStrApply_ID( TCHAR* sz, int nStrLen, int nID, bool bMinus );
}