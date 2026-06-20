

#pragma once 

class cMinimap
{
protected:
	cSprite*		m_pBG;
	cSprite*		m_pRegion;

	CsVectorPB< cSprite* >	m_vpMapBG;
	CsVectorPB< cSprite* >	m_vpQuestObj;

public:
	void			Delete();
	void			Init();

	void			Reset();	
	
	void			Render();
	void			ResetDevice();

	void			OnLBtnPress( CPoint pt );
	bool			OnMouseDown( CPoint pt );

protected:
	void			_DeleteQuestObj();


protected:
	CsPoint			_World_To_Minimap( CsPoint ptTotalSize, CsPoint ptWorld );
	CsPoint			_Minimap_To_World( CsPoint ptMimimap );
};