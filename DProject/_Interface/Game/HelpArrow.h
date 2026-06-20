
#pragma once

#include "QuestFindWay.h"

class cHelpArrow : public NiMemObject
{
public:
	cHelpArrow(){}

public:
	static	void		GlobalInit();
	static	void		GlobalShotDown();

	struct sPosFind : public NiMemObject
	{
		cQuestFindWay::eFindWayDestType	s_nFindWayType;	
		int								s_nFileID;
		DWORD							s_dwMapID;		// 최종 도착 맵
		NiPoint3						s_vArrivePos;	// 최종 도착 위치
		int								s_nRad;			// 몬스터 범위변수
		TCHAR							s_szName[ NPC_NAME_LEN ];
	};

public:
	void				Init();	
	void				Delete();	
	void				ReleaseArrowPoint();

	void				Update();
	void				Render();

public:	
	bool			SetArrowPoint( int nFileID, int nMapID, cQuestFindWay::eFindWayDestType nType, TCHAR const* szName ); //테이블ID, 맵ID, 파싱타입, 찾는대상이름
	void			FindRoot();

	bool			IsArrowRender() { return m_bArrowPointRender; }		
	sPosFind*		GetInfo() { return m_sPosFindInfo; }

protected:
	CsNodeObj						m_ArrowPoint;
	bool							m_bArrowPointRender;
	NiPoint3						m_vArrowPoint;	
	
protected:			
	sPosFind*						m_sPosFindInfo;

	NiPoint3						m_vTargetPos; //현재 위치 타겟
	CsVectorPB< CsPoint >			m_vRoot; //지나온길 기억

	std::vector< CsPoint >			m_vecRoute; //지나온 경로
	float							m_fCurRange; //현재 맵에서 나와 타겟까지의 거리
	float							m_fRange; //현재맵 제외한 타겟까지의 거리
	cText3D*						m_pRange;

//내부호출 함수
protected:
	void			_CreateArrowPoint();
	void			_DeleteArrowPoint();	

	void			_RenderRange();	
	bool			_FindNode( DWORD dwDestMapID, DWORD dwSrcMapID, float& fRange );
	void			_FindMonster( NiPoint3 vPos, float& fRange );
	
private:
	void			SetArrivePosInfo();

};

extern cHelpArrow*	g_pHelpArrow;


