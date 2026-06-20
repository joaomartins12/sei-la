#pragma once 
//2017-03-07-nova

class cRewardPromotionUI : public cScriptUI
{
public:
	cRewardPromotionUI(CsPoint pos);
	virtual ~cRewardPromotionUI();


public:
	virtual void		Update(float const& fDeltaTime);	
	virtual void		Render(CsPoint rootPos);

private:

	//cSprite*			sCpyImage[3];

	//CsPoint				pos;

	//int					iSelect;
	//int					iCount;		//2017-03-29-nova 프로모션 갯수
};