#pragma once


class StatusCallBack : public IBindStatusCallback
{
public:	
	virtual HRESULT STDMETHODCALLTYPE OnStartBinding( 	DWORD dwReserved,	IBinding *pib) { return S_OK; }	
	virtual HRESULT STDMETHODCALLTYPE GetPriority( 	LONG *pnPriority) { return S_OK; }	
	virtual HRESULT STDMETHODCALLTYPE OnLowResource( 	DWORD reserved) { return S_OK; }	
	virtual HRESULT STDMETHODCALLTYPE OnProgress( 	ULONG ulProgress, ULONG ulProgressMax, ULONG ulStatusCode,	LPCWSTR szStatusText) 	{ 		SetEvent(itsProgressEvent);		return S_OK; 	}	
	virtual HRESULT STDMETHODCALLTYPE OnStopBinding( 		HRESULT hresult,		LPCWSTR szError) { return S_OK; }	
	virtual HRESULT STDMETHODCALLTYPE GetBindInfo( 	DWORD *grfBINDF,  BINDINFO *pbindinfo) { return S_OK; }	
	virtual HRESULT STDMETHODCALLTYPE OnDataAvailable( 	DWORD grfBSCF,	DWORD dwSize,	FORMATETC *pformatetc,	STGMEDIUM *pstgmed) { return S_OK; }	
	virtual HRESULT STDMETHODCALLTYPE OnObjectAvailable( 	REFIID riid,	IUnknown *punk) { return S_OK; }    
	STDMETHOD_(ULONG, AddRef)() { return 0; }    
	STDMETHOD_(ULONG, Release)() { return 0; }    
	STDMETHOD(QueryInterface)(   REFIID riid,   void __RPC_FAR *__RPC_FAR *ppvObject) { return E_NOTIMPL; }
public:	
	StatusCallBack(HANDLE progressEvent)	{		itsProgressEvent = progressEvent;	}	
private:	
	HANDLE itsProgressEvent;
};

class CDownloadInfo
{
public:	
	CDownloadInfo(TCHAR* url, TCHAR* path)	{		itsUrl = url;		itsPath = path;		itsResult = -1;		itsProgressEvent = CreateEvent(NULL, false, false, NULL);		itsCompleteEvent = CreateEvent(NULL, false, false, NULL);	}	
	~CDownloadInfo()	{		CloseHandle(itsProgressEvent);		CloseHandle(itsCompleteEvent);	}	
	HANDLE GetProgressEvent() { return itsProgressEvent; }	
	HANDLE GetCompleteEvent() { return itsCompleteEvent; }	
	TCHAR* GetUrl() { return itsUrl; }	
	TCHAR* GetPath() { return itsPath; }
	void SetResult(HRESULT hr) { itsResult = hr; }	
	HRESULT GetResult() { return itsResult; }
private:	
	HANDLE itsProgressEvent;	
	HANDLE itsCompleteEvent;	
	TCHAR* itsUrl;	
	TCHAR* itsPath;	
	HRESULT itsResult;
};
