


#pragma once 

class cRaidRank : public cBaseWindow
{
public:
	cRaidRank(){}

public:
	virtual eWINDOW_TYPE	GetWindowType(){ return WT_RAIDRANK; }
	virtual void			Destroy();
	virtual void			DeleteResource();	

	virtual void			ResetDevice();	

	virtual void			Create( int nValue = 0 );
	virtual bool			OnEscapeKey();

	virtual void			Update(float const& fDeltaTime);
	virtual eMU_TYPE		Update_ForMouse();
	virtual void			OnMouseOutWindow();

	virtual void			Render();		

	virtual void			PreResetMap(){ Close( false ); }


	void					SetRanker(int nIndex, int nRank, TCHAR* szTamer, TCHAR* szDigimon, int nDamage, NiColor color = NiColor::WHITE);
	void					ResetRankList();



protected:
	
	cButton*				m_pBtnClose;
	cStringList				m_pRankingString;
	
};

