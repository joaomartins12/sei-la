#pragma once

#include "../../ContentsSystem/ContentsSystem.h"

class SeasonPassContents : public IUIContentSubject< SeasonPassContents >
{

public:
	static int const IsContentsIdentity(void);

public:
	SeasonPassContents(void);
	virtual ~SeasonPassContents(void);

public:
	virtual int const			GetContentsIdentity(void) const;
	virtual bool				Initialize(void);
	virtual void				UnInitialize(void);
	virtual void				Update(float const& fElapsedTime);
	virtual void				MakeWorldData(void);

	void						Req_Open(void* pkData);
	void						Req_Close(void* pkData);
	void						Req_Toggle(void* pkData);

private:
	bool						_CheckEnable();
	bool						_OpenWindow();
	bool						_CloseWindow();
};
