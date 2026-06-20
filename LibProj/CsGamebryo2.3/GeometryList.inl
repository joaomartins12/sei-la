

//////////////////////////////////////////////////////////////////////////////////////////////
//																							//
//																							//
//																							//
//////////////////////////////////////////////////////////////////////////////////////////////


inline void CGeometry::ResetZBuffer()
{
	DelPlag( CGeometry::GP_NO_ZBUFFER_WRITE );

	SAFE_POINTER_RET( m_pGeometry );
	if( 0 == m_pGeometry->GetRefCount() )
		return;

	NiPropertyState* pState = m_pGeometry->GetPropertyState();
	if( pState )
	{
		NiZBufferProperty* pZBuffer = pState->GetZBuffer();
		if( pZBuffer && ( pZBuffer->GetZBufferWrite() == false ) )
			AddPlag( CGeometry::GP_NO_ZBUFFER_WRITE );
	}
}
