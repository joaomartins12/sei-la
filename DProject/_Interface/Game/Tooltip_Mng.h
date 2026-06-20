#pragma once
#include "ToolTip.h"
#include "ToolTip_RightBottom.h"

class CToolTipMng : public CSingleton<CToolTipMng>
{
public:
	CToolTipMng();
	~CToolTipMng();

	void			Init();
	void			Update(float const& fDeltaTime);
	void			Render();
	void			ResetDevice();

	cTooltip *			GetTooltip();
	cTooltip *			GetCompareTooltip();
	cToolTip_Right *	GetRightTooltip();

private:
	cTooltip*				m_pTooltip;
	cTooltip*				m_pCompareTooltip;
	cToolTip_Right*			m_pRightTooltip;
};


// Singleton
#define TOOLTIPMNG_ST			CToolTipMng::GetSingleton()
#define TOOLTIPMNG_STPTR		CToolTipMng::GetSingletonPtr()