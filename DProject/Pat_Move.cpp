
#include "stdafx.h"
#include "Pat.h"

void CPat::sDEST_POS::SetStartPos( float fSpeed, NiPoint3 vBegin, NiPoint3 vSrc, NiPoint3 vDest )
{
	s_vBegin = vBegin;
	s_vSrcPos = vSrc;
	s_vDestPos = vDest;
	s_vEnd = vDest;

	s_fSpeed = fSpeed;
	s_fCurTime = 0.0f;
	s_fEndTime = ( s_vDestPos - s_vSrcPos ).Length() / s_fSpeed;

}

void CPat::sDEST_POS::SetNextPos( float fSpeed, sDEST_POS* pPrev, NiPoint3 vDest, NiPoint3* pEnd /*=NULL*/ )
{
	pPrev->s_vEnd = vDest;

	s_vBegin = pPrev->s_vSrcPos;
	s_vSrcPos = pPrev->s_vDestPos;
	s_vDestPos = vDest;
	s_vEnd = pEnd ? *pEnd : vDest;

	s_fSpeed = fSpeed;
	s_fCurTime = 0.0f;
	s_fEndTime = ( s_vDestPos - s_vSrcPos ).Length() / s_fSpeed;
}

NiPoint3 CPat::_Cal_Parent_Right( float fDist )
{
	return m_pParent->GetDir().Cross( NiPoint3( 0, 0, 1 ) ) * fDist;
}

NiPoint3 CPat::_Cal_Parent_Left( float fDist )
{
	return NiPoint3( 0, 0, 1 ).Cross( m_pParent->GetDir() ) * fDist;
}

NiPoint3 CPat::_Cal_Parent_Forward( float fDist )
{
	return m_pParent->GetDir() * fDist;
}

NiPoint3 CPat::_Cal_Parent_Back( float fDist )
{
	return -m_pParent->GetDir() * fDist;
}

void CPat::_SetAI_Move_Tamer_Forward( CPat_AI_Mng::sAI_Element* pAI )
{
	sDEST_POS* pPrev = NULL;

	// 테이머와의 각의 따른 분리
	float fDegree = CsR2D( nsCSGBFUNC::CalRotation( m_pParent->GetPos(), m_pParent->GetDir(), GetPos() ) );
	if( ( fDegree > -20 )&&( fDegree < 20 ) )
	{
		// 빠르게 뒤로 한바퀴 돈다
		NiPoint3 vParentPos = m_pParent->GetPos();		
		{
			sDEST_POS dp;
			NiPoint3 vBegin	= vParentPos + _Cal_Parent_Left( 100 ) + _Cal_Parent_Forward( 200 );
			NiPoint3 vSrc	= GetPos();
			NiPoint3 vDest	= vParentPos + _Cal_Parent_Right( 100 ) + _Cal_Parent_Forward( 200 );

			dp.SetStartPos( 100*2.0f, vBegin, vSrc, vDest );
			m_vpDestPos.PushBack( dp );
			pPrev = &m_vpDestPos[ m_vpDestPos.Size() - 1 ];
		}

		{
			sDEST_POS dp;
			NiPoint3 vDest	= vParentPos + _Cal_Parent_Forward( 300 );
			dp.SetNextPos( 100*2.0f, pPrev, vDest );
			m_vpDestPos.PushBack( dp );
			pPrev = &m_vpDestPos[ m_vpDestPos.Size() - 1 ];
		}

		{
			sDEST_POS dp;
			NiPoint3 vDest = vParentPos + _Cal_Parent_Left( 100 ) + _Cal_Parent_Forward( 200 );
			dp.SetNextPos( 100*2.0f, pPrev, vDest );
			m_vpDestPos.PushBack( dp );
			pPrev = &m_vpDestPos[ m_vpDestPos.Size() - 1 ];
		}

		{
			sDEST_POS dp;
			NiPoint3 vDest = vParentPos + _Cal_Parent_Left( 50 ) + _Cal_Parent_Forward( 180 );
			dp.SetNextPos( 100*2.0f, pPrev, vDest );
			m_vpDestPos.PushBack( dp );
			pPrev = &m_vpDestPos[ m_vpDestPos.Size() - 1 ];
		}

		{
			sDEST_POS dp;
			NiPoint3 vDest = vParentPos + _Cal_Parent_Forward( 100 );
			NiPoint3 vEnd = vParentPos;
			dp.SetNextPos( 100, pPrev, vDest, &vEnd );
			m_vpDestPos.PushBack( dp );
			pPrev = &m_vpDestPos[ m_vpDestPos.Size() - 1 ];
		}
	}
	else
	{
		{
			sDEST_POS dp;
			NiPoint3 vBegin	= m_pParent->GetPos() - m_pParent->GetDir()*20.0f;
			NiPoint3 vSrc	= GetPos();
			NiPoint3 vDest	= m_pParent->GetPos() + NiPoint3( 0, 0, 1 ).Cross( m_pParent->GetDir() ) * 200.0f;

			dp.SetStartPos( 100, vBegin, vSrc, vDest );
			m_vpDestPos.PushBack( dp );
			pPrev = &m_vpDestPos[ m_vpDestPos.Size() - 1 ];
		}

		{
			sDEST_POS dp;
			NiPoint3 vDest = m_pParent->GetPos() + m_pParent->GetDir()*150.0f;
			dp.SetNextPos( 100, pPrev, vDest );
			m_vpDestPos.PushBack( dp );
			pPrev = &m_vpDestPos[ m_vpDestPos.Size() - 1 ];
		}

		{
			sDEST_POS dp;
			NiPoint3 vDest = m_pParent->GetPos() + m_pParent->GetDir()*100.0f;
			NiPoint3 vEnd = m_pParent->GetPos();
			dp.SetNextPos( 100, pPrev, vDest, &vEnd );
			m_vpDestPos.PushBack( dp );
			pPrev = &m_vpDestPos[ m_vpDestPos.Size() - 1 ];
		}
	}
}