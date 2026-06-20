#pragma once

class IUIContentBase;
class cGotchaMachineContents;

class AdaptGotchaMachine
{
public:
	explicit AdaptGotchaMachine(void);
	virtual ~AdaptGotchaMachine(void);

	bool			SetContents( IUIContentBase* pContents );
	bool			IsGotchaPlaying() const;

private:
	cGotchaMachineContents* m_pContents;
};