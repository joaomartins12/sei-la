#pragma once

#include "LevelUpMsg.h"
#include "Random_Regen.h"
#include "FileIslandBless.h"

class cGlobalMessage : public NiMemObject
{
public:
	cGlobalMessage() { };
	virtual ~cGlobalMessage();

public:
 	enum eMsgType { NONE, LEVELITEM, FILEBLESS = 1605, RANDOMREGENBEGIN = 1606, RANDOMREGENEND = 1609, };

protected:
	std::list< cGlobal* >	m_listMsg;

public:
	void			Delete();

	void			Update(float const& fDeltaTime);
	bool			Render();

	void			SetMessage( n2 nType, TCHAR const* szText, TCHAR const* szText2 = NULL, TCHAR const* szText3 = NULL, TCHAR const* szText4 = NULL );
	void			BackGroundAniUpdate();
};

