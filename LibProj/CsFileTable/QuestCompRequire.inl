
inline bool	CsQuestCompRequire::SetCurCount( int nCurCount )
{
	if( nCurCount > m_nTypeCount )
		nCurCount = m_nTypeCount;
	
	return ( ( m_nCurTypeCount = nCurCount ) == m_nTypeCount );
}

inline bool CsQuestCompRequire::IncreaseCurCount()
{
	if( ++m_nCurTypeCount > m_nTypeCount )
		m_nCurTypeCount = m_nTypeCount;

	return IsCompleat();
}

inline bool CsQuestCompRequire::IsCompleat()
{
	assert_cs( m_nCurTypeCount <= m_nTypeCount );
	return ( m_nCurTypeCount == m_nTypeCount );
}