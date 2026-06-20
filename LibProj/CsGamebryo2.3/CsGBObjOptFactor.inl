

inline CsGBObjOpt* CsGBObjOptFactor::GetOption( CsGBObjOpt::eOPT_TYPE eType )
{
	assert_cs( m_mapOption.find( eType ) != m_mapOption.end() );
	return m_mapOption[ eType ];
}

inline bool CsGBObjOptFactor::IsOption( CsGBObjOpt::eOPT_TYPE eType )
{
	return ( m_mapOption.find( eType ) != m_mapOption.end() );	
}