
#pragma once 

class cPartyItemLoot : public cBaseLoot
{
public:
	cPartyItemLoot();
	~cPartyItemLoot();

public:
	virtual eWINDOW_TYPE	GetWindowType(){ return WT_LOOTING; }
	virtual void			Destroy();
	virtual void			DeleteResource();
	virtual void			Create( int nValue = 0 );

protected:
	virtual	bool			Construct(void);
	virtual	void			Notify(int const& iNotifiedEvt, ContentsStream const& kStream);

	void					_MakeLootingUI();
};

