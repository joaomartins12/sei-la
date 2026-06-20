#include "StdAfx.h"
#include "ContentsStream.h"

ContentsStream::ContentsStream(void)
	: m_Rp( 0 )
	, m_Wp( 0 )
{
	m_cnStreamvec.clear();
}

ContentsStream::~ContentsStream(void)
{
}

dm_size_t ContentsStream::Remain(void) const
{
	return ( m_cnStreamvec.size() - m_Wp );
}

bool ContentsStream::IsEOS(void) const
{
	if( m_cnStreamvec.empty() )
		return true;
	
	if( m_Rp >= m_Wp )
		return true;

	return false;
}

dm_size_t ContentsStream::Size(void) const
{
	return m_cnStreamvec.size();
}

void ContentsStream::ExtendSize(dm_size_t NewSize)
{
	m_cnStreamvec.resize( NewSize );
}

void ContentsStream::MoveWp(dm_size_t Pos)
{
	m_Wp += Pos;

	if( m_Wp < 0 )
		m_Wp = 0;
	else if( m_Wp > m_cnStreamvec.size() )
		m_Wp = m_cnStreamvec.size();
}

void ContentsStream::SetWp(dm_size_t Pos)
{
	if( Pos < 0 )
		Pos = 0;
	else if( Pos > m_cnStreamvec.size() )
		Pos = m_cnStreamvec.size();

	m_Wp = Pos;
}

void ContentsStream::MoveRp(dm_size_t Pos) const
{
	m_Rp += Pos;

	if( m_Rp < 0 )
		m_Rp = 0;
	else if( m_Rp > m_cnStreamvec.size() )
		m_Rp = m_cnStreamvec.size();
}

void ContentsStream::SetRp(dm_size_t Pos) const
{
	if( Pos < 0 )
		Pos = 0;
	else if( Pos > m_cnStreamvec.size() )
		Pos = m_cnStreamvec.size();

	m_Rp = Pos;
}

void ContentsStream::operator=(ContentsStream const& rhs)
{
	m_Rp = rhs.m_Rp;
	m_Wp = rhs.m_Wp;
	m_cnStreamvec = rhs.m_cnStreamvec;
}

