
void sIFIcon::Init( CsRect rc )
{
	s_Rect = rc;
	s_eState = NONE;
}

bool sIFIcon::MouseOn( CsPoint ptLocal )
{
	if( s_eState == MOUSE_DRAG )
		return false;

	s_eState = s_Rect.PtInRect( ptLocal ) == TRUE ? MOUSE_ON : NONE;
	return s_eState != NONE;
}

bool sIFIcon::MouseLBtnClick( CsPoint ptLocal )
{
	if( s_eState == MOUSE_DRAG )
		return true;

	s_eState = s_Rect.PtInRect( ptLocal ) == TRUE ? MOUSE_LCLICK : NONE;
	return s_eState != NONE;
}

bool sIFIcon::MouseRBtnClick( CsPoint ptLocal )
{
	if( s_eState == MOUSE_DRAG )
		return true;

	s_eState = s_Rect.PtInRect( ptLocal ) == TRUE ? MOUSE_RCLICK : NONE;
	return s_eState != NONE;
}

void sIFIcon::RenderMask( CsPoint pos, CsPoint size )
{
	g_pIconMng->RenderIFIconMask( s_eState, pos, size );	
	if( s_eState == MOUSE_ON )
		s_eState = NONE;
}

CsPoint sIFIcon::GetPos()
{
	return CsPoint( s_Rect.left, s_Rect.top );
}

CsPoint sIFIcon::GetSize()
{
	return CsPoint( s_Rect.right - s_Rect.left, s_Rect.bottom - s_Rect.top );
}

void sIFIcon::ReleaseState( bool bIncluseDrag /*=false*/ )
{
	if( ( s_eState == MOUSE_DRAG )&&( bIncluseDrag == false ) )
		return;
	s_eState = NONE;
}