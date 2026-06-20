#pragma once

#include "../../ContentsSystem/ContentsSystem.h"

class LogoContents
	: public IUIContentSubject< LogoContents >
{
public:
	static int const IsContentsIdentity(void);

public:
	LogoContents(void);
	virtual ~LogoContents(void);

	virtual int const GetContentsIdentity(void) const;
	virtual bool Initialize(void);
	virtual void UnInitialize(void);
	virtual bool IntraConnection(ContentsSystem& System);
	virtual void Update(float const& fElapsedTime);
	virtual void NotifyEvent(int const& iNotifiedEvt);
	virtual void MakeMainActorData(void);
	virtual void ClearMainActorData(void);

	void		EndLogo(void) const;
private:

};
