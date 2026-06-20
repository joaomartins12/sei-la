#include "stdafx.h"
#include "CFReadWriteLock.h"

CFReadWriteLock::CFReadWriteLock()
{
}

CFReadWriteLock::~CFReadWriteLock()
{

}

void CFReadWriteLock::TryReadLock( CFRWCondition &condition )
{
	condition.SetWriteLock( FALSE );
	m_RWLockManager.Lock( &condition );
}

void CFReadWriteLock::TryWriteLock( CFRWCondition &condition )
{
	condition.SetWriteLock( TRUE );
	m_RWLockManager.Lock( &condition );
}

void CFReadWriteLock::Unlock( CFRWCondition &condition )
{
	m_RWLockManager.Unlock( &condition );
}

//======================================================================================

CFRWCondition::CFRWCondition() : m_bWriteLock( FALSE )
{
}

BOOL CFRWCondition::IsExclusive( CFRWCondition* lhs )
{
	// 내가 WriteLock이면 이전 Lock의 Type과 상관없이 Block
	// 내가 ReadLock이면 이전 Lock이 Read면 Block되지 않고 Write면 Block된다.
	if( IsWriteLock() || lhs->IsWriteLock() ){
		return FALSE;
	}
	return TRUE;
}

void CFRWCondition::SetWriteLock( BOOL bWriteLock /*= FALSE*/ )
{
	m_bWriteLock = bWriteLock;
}

BOOL CFRWCondition::IsWriteLock()
{
	return m_bWriteLock;
}