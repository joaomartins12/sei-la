
#include "stdafx.h"
#include "Actor_Char.h"


void CDigimon::DeletePath()
{
	CsC_AvObject::DeletePath();
	GetProp_Animation()->SetAnimation( ANI::IDLE_NORMAL );
}

void CDigimon::Init( UINT nIDX, UINT nFileTableID, NiPoint3 vPos, float fRot, sCREATEINFO* pProp )
{
	CsC_AvObject::Init( nIDX, nFileTableID, vPos, fRot, pProp );

	m_MoveCoolTimeSeq.SetEndTime( 0.3f );
	m_MoveOnceTimeSeq.SetDeltaTime( 200 );
}

void CDigimon::Update( float fDeltaTime, bool bAnimation )
{
	_UpdateMove();

	CsC_AvObject::Update( fDeltaTime, bAnimation );
}

void CDigimon::_UpdateMove()
{
	if( m_MoveCoolTimeSeq.IsCoolTime() == true )
		return;

	// 디지몬이 앞서가는거 방지
	_MoveStop_ByArriveDist();

	// 디지몬이 이동해야 하는지 체크해서 이동
	if( ( _IsDoMove() == true )||( m_MoveOnceTimeSeq.IsEnable() ) )
	{
		// 테이머 도착점으로 보낸다.
		DoPos_ByTamer();
	}	
}

void CDigimon::DoPos_ByTamer()
{	
	GetProp_Path()->SetPath( _GetArrivePos_ByTamer() );
}

NiPoint2 CDigimon::_GetArrivePos_ByTamer()
{
	NiPoint2 vTarget = g_Actor.GetTamer()->GetProp_Path()->GetArrivePos();

	NiPoint3 vPath = NiPoint3( vTarget.x, vTarget.y, 0.0f );
	NiPoint3 vPath2( 0, 0, 0 );
	NiPoint3 vDir = vPath - g_Actor.GetTamer()->GetPos();	
	vDir.z = 0.0f;
	if( vDir.Unitize() < 1.0f )
	{
		vDir = g_Actor.GetTamer()->GetDir();
		vDir.z = 0.0f;
		vDir.Unitize();
	}

	vPath -= vDir * DIGIMONUSER_TAMER_RAMAIN_MOVE_LENGTH_CONSTANT/2.f;
	vPath2 = vPath;

	NiPoint3 cross = NiPoint3( 0, 0, 1 );
	NiPoint3 v = cross.Cross( vDir );
	vPath -= v * DIGIMONUSER_TAMER_RAMAIN_MOVE_LENGTH_CONSTANT/2.f*GetToolWidth()*0.01f;

	if( GetProp_Path()->PathTest( vPath ) == true )
	{
		return NiPoint2( vPath.x, vPath.y );
	}

	assert_cs( vPath2.x != 0 && vPath2.y != 0 );
	if( GetProp_Path()->PathTest( vPath2 ) == true )
	{
		return NiPoint2( vPath2.x, vPath2.y );
	}

	return vTarget;
}

void CDigimon::SetTargetPos()
{
	if( m_MoveCoolTimeSeq.IsCoolTime() == true )
		return;

	m_MoveCoolTimeSeq.Start();
	if( GetProp_Path()->IsMove() == true )
	{
		m_MoveOnceTimeSeq.Reset();
	}
}

void CDigimon::_MoveStop_ByArriveDist()
{
	if( g_Actor.GetTamer()->GetProp_Path()->IsMove() == false )
		return;

	if( GetProp_Path()->IsMove() == false )
		return;

	// 테이머가 이동하고 있고 디지몬도 이동하고 있다면
	// 테이머 도착점
	float fTamerDist = g_Actor.GetTamer()->GetProp_Path()->GetMoveDist();
	// 테이머 도착점을 기준으로한 디지몬 도착점
	NiPoint2 vDest = _GetArrivePos_ByTamer();
	NiPoint3 vCurPos = GetPos();	
	float fDigimonDist = ( NiPoint2( vCurPos.x, vCurPos.y ) - vDest ).Length();

	if( ( fDigimonDist + DIGIMONUSER_TAMER_RAMAIN_MOVE_LENGTH_CONSTANT*0.1f )< fTamerDist )
	{
		SetRotation_Ani( vDest );
		DeletePath();
	}
}

bool CDigimon::_IsDoMove()
{
	float fDeltaLength = DIGIMONUSER_TAMER_RAMAIN_MOVE_LENGTH_CONSTANT*1.2f*GetToolWidth()*0.01f;

	NiPoint2 vPos;
	vPos.x = m_vPos.x;
	vPos.y = m_vPos.y;

	NiPoint2 vTarget = g_Actor.GetTamer()->GetProp_Path()->GetArrivePos();
	float fTamerMoveDist = g_Actor.GetTamer()->GetProp_Path()->GetMoveDist();

	float f1,f2;
	f1 = (vTarget - vPos).Length();			// 테이머 도착점과 디지몬과의 거리
	f2 = fTamerMoveDist + fDeltaLength;		// 테이머 남은 이동거리

	return ( f1 > f2 + PATH_IGNORE_DIST );
}




void CTamer::DeletePath()
{
	CsC_AvObject::DeletePath();
	GetProp_Animation()->SetAnimation( ANI::IDLE_NORMAL );
}
