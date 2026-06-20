


// Helper functions:
HRESULT GetPin(IBaseFilter *pFilter, PIN_DIRECTION PinDir, IPin **ppPin)
{
	IEnumPins  *pEnum;
	IPin       *pPin;
	pFilter->EnumPins(&pEnum);
	while(pEnum->Next(1, &pPin, 0) == S_OK)
	{
		PIN_DIRECTION PinDirThis;
		pPin->QueryDirection(&PinDirThis);
		if (PinDir == PinDirThis)
		{
			pEnum->Release();
			*ppPin = pPin;
			return S_OK;
		}
		pPin->Release();
	}
	pEnum->Release();
	return E_FAIL;  
}


// 주의 !!! 잘못된 파일 실행시 데드락으로 죽는다.
HRESULT ConnectFilters(IGraphBuilder *pGraph, IBaseFilter *pFirst, IBaseFilter *pSecond)
{
	IPin *pOut = NULL, *pIn = NULL;
	HRESULT hr = GetPin(pSecond, PINDIR_INPUT, &pIn);
	if (FAILED(hr)) return hr;
	// The previous filter may have multiple outputs, so try each one!
	IEnumPins  *pEnum;
	pFirst->EnumPins(&pEnum);
	while(pEnum->Next(1, &pOut, 0) == S_OK)
	{
		PIN_DIRECTION PinDirThis;
		pOut->QueryDirection(&PinDirThis);
		if (PINDIR_OUTPUT == PinDirThis)
		{
			hr = pGraph->Connect(pOut, pIn);
			if(!FAILED(hr))
			{
				break;
			}
		}
		SAFE_RELEASE(pOut);
	}
	SAFE_RELEASE(pOut);
	SAFE_RELEASE(pEnum);
	SAFE_RELEASE(pIn);
	return hr;
}

//HRESULT GetUnconnectedPin(
//						  IBaseFilter *pFilter,   // 필터의 포인터.
//						  PIN_DIRECTION PinDir,   // 검색하는 핀의 방향.
//						  IPin **ppPin)           // 핀의 포인터를 받는다.
//{
//	*ppPin = 0;
//	IEnumPins *pEnum = 0;
//	IPin *pPin = 0;
//	HRESULT hr = pFilter->EnumPins(&pEnum);
//	if (FAILED(hr))
//	{
//		return hr;
//	}
//	while (pEnum->Next(1, &pPin, NULL) == S_OK)
//	{
//		PIN_DIRECTION ThisPinDir;
//		pPin->QueryDirection(&ThisPinDir);
//		if (ThisPinDir == PinDir)
//		{
//			IPin *pTmp = 0;
//			hr = pPin->ConnectedTo(&pTmp);
//			if (SUCCEEDED(hr))  // 이미 접속 끝나, 필요한 핀은 아니다.
//			{
//				pTmp->Release();
//			}
//			else  // 미접속, 이것이 필요한 핀이다.
//			{
//				pEnum->Release();
//				*ppPin = pPin;
//				return S_OK;
//			}
//		}
//		pPin->Release();
//	}
//	pEnum->Release();
//	// 일치하는 핀이 발견되지 않았다.
//	return E_FAIL;
//}
//
//
//HRESULT ConnectFilters(
//					   IGraphBuilder *pGraph, // 필터 그래프 매니저.
//					   IPin *pOut,            // 업 스트림 필터의 출력 핀.
//					   IBaseFilter *pDest)    // 다운 스트림 필터.
//{
//	if ((pGraph == NULL) || (pOut == NULL) || (pDest == NULL))
//	{
//		return E_POINTER;
//	}
//#ifdef debug
//	PIN_DIRECTION PinDir;
//	pOut->QueryDirection(&PinDir);
//	_ASSERTE(PinDir == PINDIR_OUTPUT);
//#endif
//
//	// 다운 스트림 필터의 입력 핀을 검색한다.
//	IPin *pIn = 0;
//	HRESULT hr = GetUnconnectedPin(pDest, PINDIR_INPUT, &pIn);
//	if (FAILED(hr))
//	{
//		return hr;
//	}
//	// 접속을 시험한다.
//	hr = pGraph->Connect(pOut, pIn);
//	pIn->Release();
//	return hr;
//}
//
//
//
//HRESULT ConnectFilters(
//					   IGraphBuilder *pGraph, 
//					   IBaseFilter *pSrc, 
//					   IBaseFilter *pDest)
//{
//	if ((pGraph == NULL) || (pSrc == NULL) || (pDest == NULL))
//	{
//		return E_POINTER;
//	}
//
//	// 최초의 필터의 출력 핀을 검색한다.
//	IPin *pOut = 0;
//	HRESULT hr = GetUnconnectedPin(pSrc, PINDIR_OUTPUT, &pOut);
//	if (FAILED(hr)) 
//	{
//		return hr;
//	}
//	hr = ConnectFilters(pGraph, pOut, pDest);
//	pOut->Release();
//	return hr;
//}
