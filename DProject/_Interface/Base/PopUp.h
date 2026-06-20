
#pragma once 

class cPopUp : public cWindow
{
public:
	cPopUp(){ memset( m_pSprite, 0, 4*9 ); }
	~cPopUp(){ Delete(); }

public:
	void				Delete();
	void				Init( cWindow* pParent, CsPoint pos, CsPoint size, CsPoint patch, char* cPathName, char* cExpName, bool bApplyWindowSize );
	void				ReSize( CsPoint pos, CsPoint size );
	void				ReSize( CsPoint size ){ ReSize( m_ptPos, size ); }

	void				Render();
	void				Render( CsPoint ptPos );

	virtual void		Update(float const& fDeltaTime) {}
	virtual void		ResetDevice();
	virtual void		SetAlpha( float fAlpha );

	cSprite*			GetSprite( int nIndex ){ return m_pSprite[ nIndex ]; }

protected:
	cSprite*			m_pSprite[ 9 ];
	CsPoint				m_ptPos;
	CsPoint				m_ptPatch;
	CsPoint				m_ptSize;

public:
	CsPoint				GetPos(){ return m_ptPos; }
	CsPoint				GetSize(){ return m_ptSize; }
};