
#include "stdafx.h"
#include "MoveObject.h"
#include <assert.h>

CsMoveObject::CsMoveObject()
{
	m_fTotalTime = 0.0f;
	m_dwMapID = 0;
	m_listAct.clear();
}

void CsMoveObject::Delete()
{
	std::list< sACT_BASE* >::iterator it = m_listAct.begin();
	std::list< sACT_BASE* >::iterator itEnd = m_listAct.end();
	for( ; it!=itEnd; ++it )
	{
		( *it )->Delete();
		switch( (*it)->GetType() )
		{
		case AT_START:				delete (sACT_START*)*it;			break;
		case AT_WAIT:				delete (sACT_WAIT*)*it;				break;
		case AT_MOVE:				delete (sACT_MOVE*)*it;				break;
		case AT_GOTO_FIRST_LOOP:	delete (sACT_GOTO_FIRST_LOOP*)*it;	break;
		default:					assert( false ); 
		}		
	}
	m_listAct.clear();
}

void CsMoveObject::Init()
{
	assert( m_listAct.empty() == true );	
}

float CalLength( int x, int y )
{
	return (float)sqrt( (double)( x*x + y*y ) );
}

bool CsMoveObject::GetPos( float fAccumTime, float& x, float& y, float& dx, float& dy, DWORD& nAni, int& nMoveSpeed )
{
	assert( m_fTotalTime != 0 );	
	assert( fAccumTime >= 0.0f );
	assert( fAccumTime <= m_fTotalTime );

	int nStartX, nStartY;
	int nPrevX, nPrevY;
	int nNextX, nNextY;
	int nVelocity = 0;
	float fActTime;
	std::list< sACT_BASE* >::iterator it = m_listAct.begin();
	std::list< sACT_BASE* >::iterator itEnd = m_listAct.end();

	bool bEnd = false;
	for( ; it!=itEnd; ++it )
	{
		fActTime = fAccumTime;
		switch( (*it)->GetType() )
		{
		case AT_START:
			{
				nStartX = nPrevX = nNextX = (*it)->GetPosX();
				nStartY = nPrevY = nNextY = (*it)->GetPosY();
			}
			break;
		case AT_WAIT:
			{
				if( ( fAccumTime -= (*it)->GetTime() ) < 0.0f )
				{
					nAni = (*it)->GetAni();
					nVelocity = 0;
					bEnd = true;
				}
			}
			break;
		case AT_MOVE:
			{
				nNextX = (*it)->GetPosX();
				nNextY = (*it)->GetPosY();
				nVelocity = (*it)->GetTime();		// ¼Óµµ
				if( ( fAccumTime -= (*it)->GetTime( CalLength( nNextX - nPrevX, nNextY - nPrevY ) ) ) < 0.0f )
				{					
					nAni = (*it)->GetAni();
					bEnd = true;
				}
				else
				{
					nPrevX = nNextX;
					nPrevY = nNextY;
				}
			}
			break;
		case AT_GOTO_FIRST_LOOP:
			{
				nNextX = nStartX;
				nNextY = nStartY;
				nVelocity = (*it)->GetTime();		// ¼Óµµ
				if( ( fAccumTime -= (*it)->GetTime( CalLength( nNextX - nPrevX, nNextY - nPrevY ) ) ) < 0.0f )
				{					
					nAni = (*it)->GetAni();
					bEnd = true;
				}
				else
				{
					nPrevX = nNextX;
					nPrevY = nNextY;
				}
			}
			break;
		default:
			assert( false ); 
		}

		if( bEnd == true )
			break;
	}

	if( bEnd == false )
	{
		nAni = (DWORD)-1;		//ANI::INVAILIDE_ID
	}

	if( ( nVelocity == 0 )|| ( bEnd == false ) )
	{
		x = (float)nNextX;
		y = (float)nNextY;
		dx = 0;
		dy = 0;
		nMoveSpeed = 0;
		return false;
	}


	float dirX = (float)( nNextX - nPrevX );
	float dirY = (float)( nNextY - nPrevY );
	float fLength = CalLength( (int)dirX, (int)dirY );
	float fRecip = ( 1.0f / fLength );
	dx = dirX * fRecip;
	dy = dirY * fRecip;

	dirX *= fRecip * ( fActTime*nVelocity );
	dirY *= fRecip * ( fActTime*nVelocity );
	x = nPrevX + dirX;
	y = nPrevY + dirY;
	nMoveSpeed = nVelocity;
	return true;
}

void CsMoveObject::Save( FILE* fp )
{
	int nCount = (int)m_listAct.size();
	assert( nCount > 0 );
	fwrite( &nCount, sizeof( int ), 1, fp );
	fwrite( &m_dwMapID, sizeof( int ), 1, fp );	
	fwrite( &m_fTotalTime, sizeof( float ), 1, fp );

	eACT_TYPE at;
	std::list< sACT_BASE* >::iterator it = m_listAct.begin();
	std::list< sACT_BASE* >::iterator itEnd = m_listAct.end();
	for( ; it!=itEnd; ++it )
	{
		at = (*it)->GetType();
		fwrite( &at, sizeof( int ), 1, fp );
		(*it)->Save( fp );
	}
}

void CsMoveObject::Load( FILE* fp )
{
	int nCount;
	fread( &nCount, sizeof( int ), 1, fp );
	fread( &m_dwMapID, sizeof( int ), 1, fp );
	fread( &m_fTotalTime, sizeof( float ), 1, fp );

	eACT_TYPE at;
	for( int i=0; i<nCount; ++i )
	{
		fread( &at, sizeof( int ), 1, fp );

		sACT_BASE* pAct = NULL;
		switch( at )
		{
		case AT_START:
			pAct = new sACT_START;
			break;
		case AT_WAIT:
			pAct = new sACT_WAIT;
			break;
		case AT_MOVE:
			pAct = new sACT_MOVE;
			break;
		case AT_GOTO_FIRST_LOOP:
			pAct = new sACT_GOTO_FIRST_LOOP;
			break;
		default:
			assert( false ); 
		}
		pAct->Load( fp );
		m_listAct.push_back( pAct );
	}
}

void CsMoveObject::LoadFilePack( int nHandle )
{
	int nCount;
	_read( nHandle, &nCount, sizeof( int ) );
	_read( nHandle, &m_dwMapID, sizeof( int ) );
	_read( nHandle, &m_fTotalTime, sizeof( float ) );

	eACT_TYPE at;
	for( int i=0; i<nCount; ++i )
	{
		_read( nHandle, &at, sizeof( int ) );

		sACT_BASE* pAct = NULL;
		switch( at )
		{
		case AT_START:
			pAct = new sACT_START;
			break;
		case AT_WAIT:
			pAct = new sACT_WAIT;
			break;
		case AT_MOVE:
			pAct = new sACT_MOVE;
			break;
		case AT_GOTO_FIRST_LOOP:
			pAct = new sACT_GOTO_FIRST_LOOP;
			break;
		default:
			assert( false ); 
		}
		pAct->LoadFilePack( nHandle );
		m_listAct.push_back( pAct );
	}
}

bool CsMoveObject::IsAct( eACT_TYPE at )
{
	std::list< sACT_BASE* >::iterator it = m_listAct.begin();
	std::list< sACT_BASE* >::iterator itEnd = m_listAct.end();
	for( ; it!=itEnd; ++it )
	{
		if( ( *it )->GetType() == at )
			return true;
	}
	return false;
}

void CsMoveObject::AddAct( sACT_BASE* pSrc )
{
	m_listAct.push_back( pSrc->Clone( pSrc ) );
}

void CsMoveObject::ResetActMove()
{
	std::list< sACT_BASE* >::iterator it = m_listAct.begin();
	std::list< sACT_BASE* >::iterator itEnd = m_listAct.end();
	for( ; it!=itEnd;  )
	{
		if( (*it)->GetType() == AT_MOVE )
		{
			delete (sACT_MOVE*)*it;		
			it = m_listAct.erase( it );
		}
		else
			++it;		
	}
}

void CsMoveObject::GetFirstMovePoint( int& x, int& y )
{
	x = 0;
	y = 0;
	std::list< sACT_BASE* >::iterator it = m_listAct.begin();
	std::list< sACT_BASE* >::iterator itEnd = m_listAct.end();
	for( ; it!=itEnd; ++it )
	{
		switch( (*it)->GetType() )
		{
		case AT_START:
			x = ( (sACT_START*)(*it) )->s_PosX;
			y = ( (sACT_START*)(*it) )->s_PosY;
			return;
		case AT_WAIT:			
			break;
		case AT_MOVE:
			x = ( (sACT_MOVE*)(*it) )->s_PosX;
			y = ( (sACT_MOVE*)(*it) )->s_PosY;
			return;
		case AT_GOTO_FIRST_LOOP:			
			break;
		default:
			assert( false ); 
		}		
	}
}

void CsMoveObject::GetLastMovePoint( int& x, int& y )
{
	x = 0;
	y = 0;
	std::list< sACT_BASE* >::iterator it = m_listAct.begin();
	std::list< sACT_BASE* >::iterator itEnd = m_listAct.end();
	for( ; it!=itEnd; ++it )
	{
		switch( (*it)->GetType() )
		{
		case AT_START:
			x = ( (sACT_START*)(*it) )->s_PosX;
			y = ( (sACT_START*)(*it) )->s_PosY;
			break;
		case AT_WAIT:			
			break;
		case AT_MOVE:
			x = ( (sACT_MOVE*)(*it) )->s_PosX;
			y = ( (sACT_MOVE*)(*it) )->s_PosY;
			break;
		case AT_GOTO_FIRST_LOOP:			
			break;
		default:
			assert( false ); 
		}		
	}
}