

inline bool nsCSGBFUNC::sTERRAIN_BOUND::InBoundCheck( sTERRAIN_BOUND Bound )
{
	return ( ( s_ptMin.x <= Bound.s_ptMin.x )&&( s_ptMin.y <= Bound.s_ptMin.y )&&
			( s_ptMax.x >= Bound.s_ptMin.x )&&( s_ptMax.y >= Bound.s_ptMin.y )&&
			( s_ptMin.x <= Bound.s_ptMax.x )&&( s_ptMin.y <= Bound.s_ptMax.y )&&
			( s_ptMax.x >= Bound.s_ptMax.x )&&( s_ptMax.y >= Bound.s_ptMax.y )	);
}

inline bool nsCSGBFUNC::sTERRAIN_BOUND::InBoundCheck( float x, float y )
{
	return ( ( s_ptMin.x <= x )&&( s_ptMin.y <= y )&&( s_ptMax.x >= x )&&( s_ptMax.y >= y ) );
}

inline bool nsCSGBFUNC::sTERRAIN_BOUND::CrossBoundCheck( sTERRAIN_BOUND Bound )
{
	return ( ( s_ptMax.x >= Bound.s_ptMin.x )&&( s_ptMin.x <= Bound.s_ptMax.x )&&
		( s_ptMax.y >= Bound.s_ptMin.y )&&( s_ptMin.y <= Bound.s_ptMax.y ) );
}

inline void nsCSGBFUNC::sTERRAIN_BOUND::ReCalInBound( sTERRAIN_BOUND* pBound )
{
	if( s_ptMax.x < pBound->s_ptMin.x )		pBound->s_ptMin.x = s_ptMax.x;
	if( s_ptMax.y < pBound->s_ptMin.y )		pBound->s_ptMin.y = s_ptMax.y;
	if( s_ptMin.x > pBound->s_ptMax.x )		pBound->s_ptMax.x = s_ptMin.x;
	if( s_ptMin.y > pBound->s_ptMax.y )		pBound->s_ptMax.y = s_ptMin.y;

	if( s_ptMin.x > pBound->s_ptMin.x )		pBound->s_ptMin.x = s_ptMin.x;
	if( s_ptMin.y > pBound->s_ptMin.y )		pBound->s_ptMin.y = s_ptMin.y;
	if( s_ptMax.x < pBound->s_ptMax.x )		pBound->s_ptMax.x = s_ptMax.x;
	if( s_ptMax.y < pBound->s_ptMax.y )		pBound->s_ptMax.y = s_ptMax.y;

	if( pBound->s_ptMin.x > pBound->s_ptMax.x )
		pBound->s_ptMax.x = pBound->s_ptMin.x;
	if( pBound->s_ptMin.y > pBound->s_ptMax.y )
		pBound->s_ptMax.y = pBound->s_ptMin.y;

	assert_cs( InBoundCheck( *pBound ) == true );
}


