
inline bool CsGBObject::IsCallPicked()
{
	bool bReturn = m_bCallPicked;
	m_bCallPicked = true;
	return bReturn;
}

inline bool CsGBObject::IsCallReady()
{
	bool bReturn = m_bCallReady;
	m_bCallReady = false;
	return bReturn;
}

inline CsGBObjProp* CsGBObject::GetProp( CsGBObjProp::ePROP_TYPE eType )
{ 
	return m_pPropFactor == NULL ? NULL : m_pPropFactor->GetProp( eType ); 
}

//==========================================================================================
//
//		Prop
//
//==========================================================================================

inline CsGBObjPropFactor* CsGBObject::_GetPropFactor()
{
	if( m_pPropFactor == NULL )
		m_pPropFactor = CsGBObjPropFactor::NewInstance();
	return m_pPropFactor;
}

inline CsGBObjOptFactor* CsGBObject::_GetOptFactor()
{
	if( m_pOptionFactor == NULL )
		m_pOptionFactor = CsGBObjOptFactor::NewInstance();
	return m_pOptionFactor;
}

//CsGBObjProp* CsGBObject::GetProp( CsGBObjProp::ePROP_TYPE pt )
//{
//	if( m_mapProp.find( pt ) == m_mapProp.end() )
//		return NULL;
//	return m_mapProp[ pt ];
//}