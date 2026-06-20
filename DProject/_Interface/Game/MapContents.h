#pragma once

#include "../Adapt/AdaptPartySystem.h"

class CMapContents	: public IUIContentSubject< CMapContents >
{
public:
	CMapContents(void);
	virtual ~CMapContents(void);
public:
	static int const IsContentsIdentity(void);

	virtual int const GetContentsIdentity(void) const;
	virtual bool Initialize(void);
	virtual void UnInitialize(void);
	virtual bool IntraConnection(ContentsSystem& System);
	virtual void Update(float const& fElapsedTime);
	virtual void NotifyEvent(int const& iNotifiedEvt);
	virtual void NotifyEventStream(int const& iNotifiedEvt, ContentsStream const& kStream);
	virtual void MakeMainActorData(void);
	virtual void ClearMainActorData(void);
	virtual void MakeWorldData(void);
	virtual void ClearWorldData(void);


public:
	bool				IsJoinParty() const;
	SPartyInfo const*	GetPartyInfo() const;

private:
	//void		openItemProductionWindow( void* pData );
	//void		closeItemProductionWindow( void* pData );
	//void		UpdateItemCount( void* pData );
	void		InitContentsData(void);

private:
	bool		m_bRegionShow;

public:
	void		SetRegionShow(bool	show);
	bool		IsRegionShow() const;

	TOwnershipGuard<AdaptPartySystem>		m_AdaptParty;
};
