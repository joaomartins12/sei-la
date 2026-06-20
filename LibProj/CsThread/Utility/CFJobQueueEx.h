/********************************************************************
	created:	2007/11/16
	created:	16:11:2007   11:14
	filename: 	d:\work\Network\CFMTLib\Utility\CFJobQueueEx.h
	file path:	d:\work\Network\CFMTLib\Utility
	file base:	CFJobQueueEx
	file ext:	h
	author:		celes
	
*********************************************************************/

#pragma once

#include"CFBackgroundJobProcessor.h"

class CFJobQueueEx : public CFBackgroundJobProcessorBase
{
public:
	CFJobQueueEx(){};
	virtual ~CFJobQueueEx();

	static BOOL InitializeGlobalInstance();
	static CFJobQueueEx* GetGlobalInstance();
	static BOOL DeInitializeGlobalInstance();

protected:
	CsThreadBase * CreateWorkerThread( const char *szThreadName, ITSJobQueue ** paWaitJobQueue, CFPortInfo* paPortInfo );
	void DestroyWorkerThread( CsThreadBase *pWorkerThread );
	ITSJobQueue * AllocCompletionQueue();
	void AllocWaitQueue( ITSJobQueue ** paWaitQueue, DWORD &dwCount );

};