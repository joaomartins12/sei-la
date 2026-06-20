

inline void CsGBObjProp_Alpha::SetAlphaBlend( bool bAlpha )
{
	if( bAlpha == true )
	{
		m_eAlphaType = AT_ALPHA_DECREASE;
	}
	else if( m_CsBackupProp.s_listProperty.empty() == false )
	{
		m_eAlphaType = AT_ALPHA_INCREASE;
	}
}