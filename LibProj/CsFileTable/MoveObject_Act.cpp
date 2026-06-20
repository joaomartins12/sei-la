
#include "stdafx.h"
#include "MoveObject.h"
#include <assert.h>

CsMoveObject::sACT_BASE*
CsMoveObject::sACT_START::Clone( sACT_BASE* pSrc )
{
	assert( pSrc->GetType() == AT_START );
	sACT_START* pNew = new sACT_START;
	*pNew = *(sACT_START*)pSrc;
	return pNew;
}

void CsMoveObject::sACT_START::Save( FILE* fp )
{
	fwrite( &s_PosX, sizeof( int ), 1, fp );
	fwrite( &s_PosY, sizeof( int ), 1, fp );
}

void CsMoveObject::sACT_START::Load( FILE* fp )
{
	fread( &s_PosX, sizeof( int ), 1, fp );
	fread( &s_PosY, sizeof( int ), 1, fp );
}

void CsMoveObject::sACT_START::LoadFilePack( int nHandle )
{
	_read( nHandle, &s_PosX, sizeof( int ) );
	_read( nHandle, &s_PosY, sizeof( int ) );
}

CsMoveObject::sACT_BASE*
CsMoveObject::sACT_WAIT::Clone( sACT_BASE* pSrc )
{
	assert( pSrc->GetType() == AT_WAIT );
	sACT_WAIT* pNew = new sACT_WAIT;
	*pNew = *(sACT_WAIT*)pSrc;
	return pNew;
}

void CsMoveObject::sACT_WAIT::Save( FILE* fp )
{
	fwrite( &s_dwAni, sizeof( DWORD ), 1, fp );
	fwrite( &s_nTimeSec, sizeof( int ), 1, fp );
}

void CsMoveObject::sACT_WAIT::Load( FILE* fp )
{
	fread( &s_dwAni, sizeof( DWORD ), 1, fp );
	fread( &s_nTimeSec, sizeof( int ), 1, fp );
}

void CsMoveObject::sACT_WAIT::LoadFilePack( int nHandle )
{
	_read( nHandle, &s_dwAni, sizeof( DWORD ) );
	_read( nHandle, &s_nTimeSec, sizeof( int ) );
}

CsMoveObject::sACT_BASE*
CsMoveObject::sACT_MOVE::Clone( sACT_BASE* pSrc )
{
	assert( pSrc->GetType() == AT_MOVE );
	sACT_MOVE* pNew = new sACT_MOVE;
	*pNew = *(sACT_MOVE*)pSrc;
	return pNew;
}

void CsMoveObject::sACT_MOVE::Save( FILE* fp )
{
	fwrite( &s_PosX, sizeof( int ), 1, fp );
	fwrite( &s_PosY, sizeof( int ), 1, fp );
	fwrite( &s_dwAni, sizeof( DWORD ), 1, fp );
	fwrite( &s_nVelocity, sizeof( int ), 1, fp );
}

void CsMoveObject::sACT_MOVE::Load( FILE* fp )
{
	fread( &s_PosX, sizeof( int ), 1, fp );
	fread( &s_PosY, sizeof( int ), 1, fp );
	fread( &s_dwAni, sizeof( DWORD ), 1, fp );
	fread( &s_nVelocity, sizeof( int ), 1, fp );
}

void CsMoveObject::sACT_MOVE::LoadFilePack( int nHandle )
{
	_read( nHandle, &s_PosX, sizeof( int ) );
	_read( nHandle, &s_PosY, sizeof( int ) );
	_read( nHandle, &s_dwAni, sizeof( DWORD ) );
	_read( nHandle, &s_nVelocity, sizeof( int ) );
}

CsMoveObject::sACT_BASE*
CsMoveObject::sACT_GOTO_FIRST_LOOP::Clone( sACT_BASE* pSrc )
{
	assert( pSrc->GetType() == AT_GOTO_FIRST_LOOP );
	sACT_GOTO_FIRST_LOOP* pNew = new sACT_GOTO_FIRST_LOOP;
	*pNew = *(sACT_GOTO_FIRST_LOOP*)pSrc;
	return pNew;
}

void CsMoveObject::sACT_GOTO_FIRST_LOOP::Save( FILE* fp )
{
	fwrite( &s_dwAni, sizeof( DWORD ), 1, fp );
	fwrite( &s_nVelocity, sizeof( int ), 1, fp );
}

void CsMoveObject::sACT_GOTO_FIRST_LOOP::Load( FILE* fp )
{
	fread( &s_dwAni, sizeof( DWORD ), 1, fp );
	fread( &s_nVelocity, sizeof( int ), 1, fp );
}

void CsMoveObject::sACT_GOTO_FIRST_LOOP::LoadFilePack( int nHandle )
{
	_read( nHandle, &s_dwAni, sizeof( DWORD ) );
	_read( nHandle, &s_nVelocity, sizeof( int ) );
}