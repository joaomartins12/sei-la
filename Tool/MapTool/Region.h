
#pragma once 

class cRegion
{
public:
	cRegion();
protected:
	//NiNodePtr	m_pNiBox100;
	NiNodePtr	m_pNiRegion;
public:
	void		Delete();
	void		Init();

	void		Render( NiPoint2 ptPos, float fRadius, bool bSelect );
	void		Render( NiPoint2 ptPos, float fRadius, NiColor color );
	//void		RenderBox100( NiPoint3 ptPos, bool bRed );
};

