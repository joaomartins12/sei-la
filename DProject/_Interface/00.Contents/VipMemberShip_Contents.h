#pragma once

#include "../../ContentsSystem/ContentsSystem.h"


class CVipMemberShipContents
	: public IUIContentSubject< CVipMemberShipContents >
{
public:
	enum eEventType
	{
		eOnMemberShip,
		eOffMemberShip,
	};

	CVipMemberShipContents(void);
	virtual ~CVipMemberShipContents(void);

	static int const IsContentsIdentity(void);
	virtual int const GetContentsIdentity(void) const;
	virtual bool	Initialize(void);
	virtual void	UnInitialize(void);
	virtual bool	IntraConnection(ContentsSystem& System);
	virtual void	Update(float const& fElapsedTime);
	virtual void	NotifyEvent(int const& iNotifiedEvt);
	virtual void	MakeMainActorData(void);
	virtual void	ClearMainActorData(void);
	virtual void	MakeWorldData(void);
	virtual void	ClearWorldData(void);

	void			ShopVipPopupMemu(CsPoint csPosition);
	void			OnClick_OpenBanner(std::string const& sOpenUrl);
	void			ShowInGameCashshopItemSelect();

private:
	void			_ShowAndCloseMemberShipContents( bool const& bValue, __int64 const& fEndTime );
	void			_ShowVipSystem(__int64 const& fVipEndTime);
	void			_CloseVipSystem();

	void			RecvVipSystemData(void* pData);
	void			OnVipSystemContents(void*pData);

private:
	bool			m_bIsVip;
	float			m_fVipEndTime;
};
