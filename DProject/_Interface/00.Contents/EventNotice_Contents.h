#pragma once

#include "../../ContentsSystem/ContentsSystem.h"

class EventNoticeContents : public IUIContentSubject< EventNoticeContents >
{

public:
	static int const IsContentsIdentity(void);

public:
	EventNoticeContents(void);
	virtual ~EventNoticeContents(void);

public:
	virtual int const			GetContentsIdentity(void) const;
	virtual bool				Initialize(void);
	virtual void				UnInitialize(void);
	virtual void				Update(float const& fElapsedTime);
	virtual void				MakeWorldData(void);

	void						Req_OpenEventNotice(void* pkData);
	void						Req_CloseEventNotice(void* pkData);
	void						Req_ToggleEventNotice(void* pkData);

private:
	bool						_CheckEnableEventNotice();
	bool						_OpenWindow();
	bool						_CloseWindow();
};
