
#pragma once 

class cMyItemLoot : public cBaseLoot
{
public:
	cMyItemLoot();
	~cMyItemLoot();

public:
	virtual eWINDOW_TYPE	GetWindowType(){ return WT_MYLOOTING; }
	virtual void			Destroy();
	virtual void			DeleteResource();

	virtual void			Create( int nValue = 0 );

protected:
	virtual	bool			Construct(void);
	virtual	void			Notify(int const& iNotifiedEvt, ContentsStream const& kStream);

	void					_MakeLootingUI();
};

