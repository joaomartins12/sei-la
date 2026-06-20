
#include "stdafx.h"
#include "PathProperty.h"


THREAD_MEMPOOL_CPP( CsC_PathProp )
#define PARENT_CALSS		CsC_Property

CsC_PathProp::CsC_PathProp()
{
	m_eType = CsC_Property::PATH;
	m_eMoveState = MS_RUN;
	m_pfMoveSpeed = NULL;

	m_AbsolutePathInfo.s_bActive = false;
	m_bAbsoluteRotation = false;
	m_bUserControl = false;
}

CsC_PathProp::~CsC_PathProp()
{
}

void CsC_PathProp::Delete()
{
	PARENT_CALSS::Delete();

	m_Path.DeleteAgent();
	m_pfMoveSpeed = NULL;
	m_eType = CsC_Property::PATH;
	m_eMoveState = MS_RUN;
	m_AbsolutePathInfo.s_bActive = false;
	m_bAbsoluteRotation = false;
	m_bUserControl = false;
}


void CsC_PathProp::PreDelete()
{
	m_Path.DeleteAgent();

	m_AbsolutePathInfo.s_bActive = false;
}

void CsC_PathProp::Init( CsC_AvObject* pParent, float* pfMoveSpeed, bool bUserControl )
{
	PARENT_CALSS::Init( pParent );

	AddAgent( m_pParent->GetPos().x, m_pParent->GetPos().y );

	assert_cs( m_pfMoveSpeed == NULL );
	m_pfMoveSpeed = pfMoveSpeed;
	m_eMoveState = MS_RUN;

	m_AbsolutePathInfo.s_bActive = false;
	m_bAbsoluteRotation = false;

	m_bUserControl = bUserControl;	
	if( m_bUserControl == true )
	{
		m_vArrivePos = NiPoint2( m_pParent->GetPos().x, m_pParent->GetPos().y );
		m_vArriveDir = NiPoint2( m_pParent->GetDir().x, m_pParent->GetDir().y );
		m_vArriveDir.Unitize();

		m_fMoveDist = 0.0f;
		m_fOrgTotalMoveDist = 0.0f;
	}
}

void CsC_PathProp::PreUpdate( float fDeltaTime )
{
	if( NULL == m_Path.m_iPath )
	{
		if( m_AbsolutePathInfo.s_bActive )
			m_AbsolutePathInfo.s_bActive = false;
		return;
	}

	if( m_AbsolutePathInfo.s_bActive )
	{
		_PreUpdate_AbsolutePath( fDeltaTime );
		return;
	}

	if( m_pParent->IsPause( CsC_AvObject::PAUSE_PATH ) )
		return;

	NiPoint3 pos;
	float dx, dy;

	// 리턴 값 - 방향 성 잡히면 리셋	
	bool bUpdateRotation = g_pCsPathEngine->UpdatePath( &m_Path, GetMoveSpeed(), dx, dy, pos.x, pos.y, fDeltaTime, m_bAbsoluteRotation );	
	pos.z = nsCsGBTerrain::g_pCurRoot->GetHeight( pos );	

	m_pParent->SetPos( pos );
	m_bAbsoluteRotation = false;

	if( bUpdateRotation == true )
	{
		NiPoint3 vDestDir = NiPoint3( (float)dx, (float)dy, 0.0f );
		m_pParent->SetRotation_AniDir( vDestDir );		
	}

	// 도착
	if( m_Path.m_iPath->getLength() == 0 )
	{
		m_pParent->DeletePath();		
		return;
	}

	if( m_bUserControl == true )
	{
		m_fMoveDist = (float)m_Path.m_iPath->getLength();
	}
}

void CsC_PathProp::_PreUpdate_AbsolutePath( float fDeltaTime )
{
	NiPoint3 pos;
	float dx, dy;

	// 리턴 값 - 방향 성 잡히면 리셋
	g_pCsPathEngine->UpdatePath( &m_Path, m_AbsolutePathInfo.s_fSpeed, dx, dy, pos.x, pos.y, fDeltaTime );
	pos.z = nsCsGBTerrain::g_pCurRoot->GetHeight( pos );
	m_pParent->SetPos( pos );

	// 도착
	if( m_Path.m_iPath->getLength() == 0 )
	{
		m_AbsolutePathInfo.s_bActive = false;
		m_pParent->CsC_AvObject::DeletePath();
		return;
	}

	if( m_bUserControl == true )
	{
		m_fMoveDist = (float)m_Path.m_iPath->getLength();
	}
}

void CsC_PathProp::CheckModelComp()
{
	PARENT_CALSS::CheckModelComp();
}


//============================================================================================
//
//	Status
//
//============================================================================================
float CsC_PathProp::GetMoveSpeed()
{
	float fReturn = *m_pfMoveSpeed;
	switch( GetMoveState() )
	{
	case MS_WALK:	fReturn *= 0.25f;		break;
	case MS_RUN:							break;
	default:		assert_cs( false );
	}	 
	return fReturn;
}

//============================================================================================
//
//	Path
//
//============================================================================================
bool CsC_PathProp::SetPath( float x, float y )
{
	if( m_AbsolutePathInfo.s_bActive == true )
		return false;

	assert_cs( m_pfMoveSpeed != NULL );
	bool bEnablePath = g_pCsPathEngine->SetPath( &m_Path, x, y );
	if( m_Path.m_iPath == NULL )
		return false;

	if( m_bUserControl == true )
	{
		if( m_Path.m_iPath )
		{
			m_fMoveDist = (float)m_Path.m_iPath->getLength();
			m_fOrgTotalMoveDist = m_fMoveDist;
			// 너무 짧은 거리는 패스
			if( m_fMoveDist < PATH_IGNORE_DIST )
			{
				m_pParent->DeletePath();
				m_fMoveDist = 0;
				m_fOrgTotalMoveDist = 0;
				return false;
			}
			m_vArrivePos.x = x;
			m_vArrivePos.y = y;
			return true;
		}		
	}
	assert_cs( bEnablePath == true );
	return bEnablePath;
}

bool CsC_PathProp::AbsolutePath( float x, float y, float fSpeedSec )
{
	if( SetPath( x, y ) == false )
		return false;

	m_AbsolutePathInfo.s_bActive = true;
	m_AbsolutePathInfo.s_fSpeed = fSpeedSec;	
	return true;
}

void CsC_PathProp::DeletePath()
{
	m_Path.DeletePath();
	m_pParent->ArriveTarget();

	if( m_bUserControl == true )
	{
		m_vArrivePos.x = m_pParent->GetPos().x;
		m_vArrivePos.y = m_pParent->GetPos().y;
		m_fMoveDist = 0;
		m_fOrgTotalMoveDist = 0;
	}	
}

void CsC_PathProp::ResetAgent()
{
	m_Path.DeleteAgent();	
	AddAgent( m_pParent->GetPos().x, m_pParent->GetPos().y );

	m_vArrivePos = NiPoint2( m_pParent->GetPos().x, m_pParent->GetPos().y );
	m_vArriveDir = NiPoint2( m_pParent->GetDir().x, m_pParent->GetDir().y );
	m_vArriveDir.Unitize();
}

bool CsC_PathProp::CalTargetPath( NiPoint2& vReturnPos, NiPoint3 vTargetPos, float fConnetLength )
{
	assert_cs( m_bUserControl == true );

#define SQRT_2		1.414214f
	fConnetLength -= SQRT_2;

	NiPoint3 vCharPos = m_pParent->GetPos();
	vTargetPos.z = vCharPos.z;
	// 타겟과의 거리가 더 가깝다면 움직이는건 패스하자	
	if( ( vCharPos - vTargetPos ).Length() <= fConnetLength )
	{
		m_pParent->DeletePath();
		return false;
	}

	// 도착 방향
	NiPoint2 vReturnDir = NiPoint2( vTargetPos.x - vCharPos.x, vTargetPos.y - vCharPos.y );	
	float fOrgLength = vReturnDir.Length();
	vReturnDir.Unitize();

	NiPoint2 vPickPos;
	NiPoint2 startPos( vCharPos.x, vCharPos.y);
	NiPoint2 endPos(vTargetPos.x, vTargetPos.y);

	if( nsCsGBTerrain::g_pCurRoot->GetActor_Object_2DPick( vPickPos, "door", startPos, endPos ) )
	{
		vTargetPos.x = vPickPos.x;
		vTargetPos.y = vPickPos.y;
	}

	iPath* pPathTestInfo = NULL;
	while( true )
	{
		if( ( nsCsGBTerrain::g_pCurRoot->IsValidEmr( vTargetPos ) == true )&&
			( PathTest( vTargetPos ) == true ) )
		{
			// 임시 셋팅~!!
			pPathTestInfo = g_pCsPathEngine->FindShortestPathInfo( &m_Path, vTargetPos.x, vTargetPos.y );
			if( NULL == pPathTestInfo )
				return false;

			if( fConnetLength < 0.0f )
				fConnetLength = 0.0f;
			break;
		}
		// 도착 방향의 반대로 계속 이동시켜 주면서 검사
		vTargetPos.x -= vReturnDir.x * 25.f;
		vTargetPos.y -= vReturnDir.y * 25.f;
		fConnetLength -= 25.0f;

		// 가야할 거리보다 더 멀어진다면~ 리턴
		if( fOrgLength <= ( vTargetPos - vCharPos ).Length() )
			return false;
	}

	if( pPathTestInfo == NULL )
		return false;

	//==================================================================================
	// 타겟점
	//==================================================================================	
	long size = pPathTestInfo->size();
	if( size == 1 )
	{
		m_pParent->DeletePath();
		return false;
	}

	vReturnPos = NiPoint2( vTargetPos.x, vTargetPos.y );
	NiPoint2 vStart, vEnd, vPosDir;
	float fvLength;
	for( long i=size-1; i>0; --i )
	{
		// 도착점
		vEnd = NiPoint2( (float)pPathTestInfo->position( i ).x, (float)pPathTestInfo->position( i ).y );

		// 시작점
		vStart = NiPoint2( (float)pPathTestInfo->position( i-1 ).x, (float)pPathTestInfo->position( i-1 ).y );
		// 도착점에서의 방향
		vPosDir = vStart - vEnd;
		// 거리
		fvLength = vPosDir.Length();
		vPosDir.Unitize();

		// 접근 거리보다 두 벡터간 길이가 더 클경우
		if( fConnetLength < fvLength )
		{
			vReturnPos = vEnd + fConnetLength*vPosDir;
			vReturnDir = -vPosDir;
			fConnetLength = 0.0f;
			break;
		}
		fConnetLength -= fvLength;
	}

	// 모든 거리를 더해도 접근거리보다 짧을때는 이동 패스
	if( fConnetLength > 0.0f )
		return false;

	if( PathTest( vReturnPos ) == false )
		return false;

// 	m_vArriveDir = vReturnDir;
// 	m_vArriveDir.Unitize();

	return true;
}


#undef PARENT_CLASS