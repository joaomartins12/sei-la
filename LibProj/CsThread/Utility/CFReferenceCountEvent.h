/********************************************************************
	created:	2006/10/26
	created:	26:10:2006   17:13
	filename: 	d:\work\KUF-COD\Network\CFMTLib\Utility\CFReferenceCountEvent.h
	file path:	d:\work\KUF-COD\Network\CFMTLib\Utility
	file base:	CFReferenceCountEvent
	file ext:	h
	author:		celes
	
	Reference Count가 0이 될때까지 기다리는 Event
*********************************************************************/

#pragma once

#include "../CsWaitableObject.h"
#include "../CsAutolock.h"

class CFReferenceCountEvent : public CsWaitableObject
{
public:
	CFReferenceCountEvent();
	
	void Reference();
	void Release();

protected:
	volatile LONG m_lReferenceCount;
	CsCriticalSection m_cs;
};