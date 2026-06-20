#include "stdafx.h"
#include "TalkMng.h"

//======================================================================================================
//
// MENSAGEM
//
//======================================================================================================

void CsTalkMng::_Delete_Message()
{
	CsTalk_Message::MAP_IT it = m_mapMessage.begin();
	CsTalk_Message::MAP_IT itEnd = m_mapMessage.end();

	for (; it != itEnd; ++it)
	{
		it->second->Delete();
		delete it->second;
	}

	m_mapMessage.clear();
}

CsTalk_Message* CsTalkMng::GetTalk_Message(DWORD dwID)
{
	CsTalk_Message::MAP_IT it = m_mapMessage.find(dwID);

	if (it == m_mapMessage.end())
		return NULL;

	return it->second;
}

void CsTalkMng::_LoadExcel_Message()
{
#define IL_COL_START				0
#define TALK_SHEET_NAME				"DM_Message"

#define IL_COL_ID					1
#define IL_COL_MSG_TYPE				IL_COL_ID + 2
#define IL_COL_TYPE					IL_COL_MSG_TYPE + 2
#define IL_COL_TITLE_KOR			IL_COL_TYPE + 2
#define IL_COL_TITLE_ENG			IL_COL_TITLE_KOR + 1
#define IL_COL_BODY_KOR				IL_COL_TITLE_ENG + 1
#define IL_COL_BODY_ENG				IL_COL_BODY_KOR + 1
#define IL_COL_LINKID				IL_COL_BODY_ENG + 1

	char cBuffer[CEXCLE_CELL_LEN];
	wchar_t wBuffer[CEXCLE_CELL_LEN];

	TCHAR sz[FT_TALK_MSG_BODY_LEN];

	cExcelReader excel;
	bool bSuccess = excel.Open(nsCsFileTable::GetPath_DM_Talk(), TALK_SHEET_NAME);
	assert_cs(bSuccess == true);

	// 2줄은 버린다.
	bSuccess = excel.MoveFirst();
	assert_cs(bSuccess == true);

	bSuccess = excel.MoveNext();
	assert_cs(bSuccess == true);

	CsTalk_Message::sINFO info;

	while (excel.MoveNext() == true)
	{
		// ID
		info.s_dwID = atoi(excel.GetData(IL_COL_ID, cBuffer));
		if (info.s_dwID == 0)
			continue;

		info.s_MsgType = (CsTalk_Message::eMESSAGE_TYPE)atoi(excel.GetData(IL_COL_MSG_TYPE, cBuffer));
		info.s_Type = (CsTalk_Message::eTYPE)atoi(excel.GetData(IL_COL_TYPE, cBuffer));

		switch (nsCsFileTable::g_eLanguage)
		{
		case nsCsFileTable::KOREA_ORIGINAL:
		case nsCsFileTable::KOREA_TRANSLATION:
		{
			M2W(info.s_TitleName, excel.GetData(IL_COL_TITLE_KOR, cBuffer), FT_TALK_MSG_TITLE_LEN);
			assert_cs(_tcslen(info.s_TitleName) < FT_TALK_MSG_TITLE_LEN);

			M2W(sz, excel.GetData(IL_COL_BODY_KOR, cBuffer), FT_TALK_MSG_BODY_LEN);
			_tcscpy_s(info.s_Message, FT_TALK_MSG_BODY_LEN, __CsStringChange::NewLineCheck(sz));

			assert_csm2(
				_tcslen(info.s_Message) < FT_TALK_MSG_BODY_LEN,
				_T("nLen = %d\n%s"),
				_tcslen(info.s_Message),
				info.s_Message);
		}
		break;

		case nsCsFileTable::ENGLISH:
		case nsCsFileTable::ENGLISH_A:
		{
			TCHAR szBuffer[2048];

			M2W(szBuffer, excel.GetData(IL_COL_TITLE_ENG, cBuffer), 2048);
			if ((int)_tcslen(szBuffer) >= FT_TALK_MSG_TITLE_LEN - 1)
			{
				CsMessageBox(
					MB_OK,
					L"DM_Message : %d, 의 TitleName 문자열이 너무 깁니다. \n현재길이 : %d / 제한길이 : %d",
					info.s_dwID,
					(int)_tcslen(szBuffer),
					FT_TALK_MSG_TITLE_LEN);
			}
			else
			{
				_tcscpy_s(info.s_TitleName, FT_TALK_MSG_TITLE_LEN, szBuffer);
			}

			M2W(szBuffer, excel.GetData(IL_COL_BODY_ENG, cBuffer), 2048);
			if ((int)_tcslen(szBuffer) >= FT_TALK_MSG_BODY_LEN - 1)
			{
				CsMessageBox(
					MB_OK,
					L"DM_Message : %d, 의 Eng_Message 문자열이 너무 깁니다. \n현재길이 : %d / 제한길이 : %d",
					info.s_dwID,
					(int)_tcslen(szBuffer),
					FT_TALK_MSG_BODY_LEN);
			}
			else
			{
				_tcscpy_s(
					info.s_Message,
					FT_TALK_MSG_BODY_LEN,
					__CsStringChange::NewLineCheck(szBuffer));
			}

			M2W(szBuffer, excel.GetData(IL_COL_BODY_KOR, cBuffer), 2048);
			if ((int)_tcslen(szBuffer) >= FT_TALK_MSG_BODY_LEN - 1)
			{
				CsMessageBox(
					MB_OK,
					L"DM_Message : %d, 의 Kor_Message 문자열이 너무 깁니다. \n현재길이 : %d / 제한길이 : %d",
					info.s_dwID,
					(int)_tcslen(szBuffer),
					FT_TALK_MSG_BODY_LEN);
			}
			else
			{
				if (atoi(excel.GetData(18, cBuffer)) == 0)
					_CheckWordOrder(szBuffer, info.s_Message, info.s_dwID);
			}
		}
		break;

		case nsCsFileTable::THAILAND:
		case nsCsFileTable::TAIWAN:
		case nsCsFileTable::HONGKONG:
		{
			wcscpy_s(info.s_TitleName, FT_TALK_MSG_TITLE_LEN, excel.GetData(IL_COL_TITLE_ENG, wBuffer));
			assert_cs(_tcslen(info.s_TitleName) < FT_TALK_MSG_TITLE_LEN);

			wcscpy_s(sz, FT_TALK_MSG_BODY_LEN, excel.GetData(IL_COL_BODY_ENG, wBuffer));
			_tcscpy_s(info.s_Message, FT_TALK_MSG_BODY_LEN, __CsStringChange::NewLineCheck(sz));

			assert_csm2(
				_tcslen(info.s_Message) < FT_TALK_MSG_BODY_LEN,
				_T("nLen = %d\n%s"),
				_tcslen(info.s_Message),
				info.s_Message);

			TCHAR MesKorea[FT_TALK_MSG_BODY_LEN];
			M2W(MesKorea, excel.GetData(IL_COL_BODY_KOR, cBuffer), FT_TALK_MSG_BODY_LEN);

			if (atoi(excel.GetData(18, cBuffer)) == 0)
				_CheckWordOrder(MesKorea, info.s_Message, info.s_dwID);
		}
		break;

		default:
			assert_cs(false);
			break;
		}

		info.s_dwLinkID = atoi(excel.GetData(IL_COL_LINKID, cBuffer));

		// Fix:
		// Some clients/packs can contain duplicated message IDs.
		// The original code asserted and crashed. Keep the first loaded entry.
		if (m_mapMessage.find(info.s_dwID) != m_mapMessage.end())
			continue;

		CsTalk_Message* pMessage = csnew CsTalk_Message;
		pMessage->Init(&info);

		m_mapMessage[info.s_dwID] = pMessage;
	}

	excel.Close();
}

#ifdef SIMPLE_TOOLTIP

CsTalk_Tooltip* CsTalkMng::GetTalk_Tooltip(DWORD dwID)
{
	CsTalk_Tooltip::MAP_IT it = m_mapTooltip.find(dwID);

	if (it == m_mapTooltip.end())
		return NULL;

	return it->second;
}

void CsTalkMng::_LoadExcel_ToolTip()
{
#define TOOLTIP_SHEET_NAME				"DM_ToolTip"

#define IL_COL_CODE				0

#ifdef VERSION_TH
#define IL_COL_TITLE			2
#define IL_COL_MSG				4
#else
#define IL_COL_TITLE			1
#define IL_COL_MSG				2
#endif

	cExcelReader excel;
	bool bSuccess = excel.Open(nsCsFileTable::GetPath_DM_Talk(), TOOLTIP_SHEET_NAME);
	assert_cs(bSuccess == true);

	// 2줄은 버린다.
	bSuccess = excel.MoveFirst();
	assert_cs(bSuccess == true);

	bSuccess = excel.MoveNext();
	assert_cs(bSuccess == true);

	char cBuffer[CEXCLE_CELL_LEN];
	wchar_t wBuffer[CEXCLE_CELL_LEN];

	TCHAR sz[FT_TOOLTIP_MSG_BODY_LEN];

	CsTalk_Tooltip::sINFO info;

	while (excel.MoveNext() == true)
	{
		// ID
		info.s_dwID = atoi(excel.GetData(IL_COL_CODE, cBuffer));
		if (info.s_dwID == 0)
			continue;

		switch (nsCsFileTable::g_eLanguage)
		{
		case nsCsFileTable::KOREA_ORIGINAL:
		case nsCsFileTable::KOREA_TRANSLATION:
		{
			M2W(sz, excel.GetData(IL_COL_TITLE, cBuffer), FT_TOOLTIP_TITLE_BODY_LEN);
			_tcscpy_s(info.s_Title, FT_TOOLTIP_TITLE_BODY_LEN, __CsStringChange::NewLineCheck(sz));

			M2W(sz, excel.GetData(IL_COL_MSG, cBuffer), FT_TOOLTIP_MSG_BODY_LEN);
			_tcscpy_s(info.s_Message, FT_TOOLTIP_MSG_BODY_LEN, __CsStringChange::NewLineCheck(sz));

			assert_csm2(
				_tcslen(info.s_Message) < FT_TOOLTIP_MSG_BODY_LEN,
				_T("nLen = %d\n%s"),
				_tcslen(info.s_Message),
				info.s_Message);
		}
		break;

		case nsCsFileTable::ENGLISH:
		case nsCsFileTable::ENGLISH_A:
		{
			assert_cs(false);
		}
		break;

		case nsCsFileTable::THAILAND:
		case nsCsFileTable::TAIWAN:
		case nsCsFileTable::HONGKONG:
		{
			wcscpy_s(sz, FT_TOOLTIP_MSG_BODY_LEN, excel.GetData(IL_COL_TITLE, wBuffer));
			_tcscpy_s(info.s_Title, FT_TOOLTIP_TITLE_BODY_LEN, __CsStringChange::NewLineCheck(sz));

			wcscpy_s(sz, FT_TOOLTIP_MSG_BODY_LEN, excel.GetData(IL_COL_MSG, wBuffer));
			_tcscpy_s(info.s_Message, FT_TOOLTIP_MSG_BODY_LEN, __CsStringChange::NewLineCheck(sz));

			assert_csm2(
				_tcslen(info.s_Message) < FT_TOOLTIP_MSG_BODY_LEN,
				_T("nLen = %d\n%s"),
				_tcslen(info.s_Message),
				info.s_Message);
		}
		break;

		default:
			assert_cs(false);
			break;
		}

		// Fix:
		// Prevent crash when duplicated tooltip IDs exist in DM_ToolTip.
		// Keep the first loaded tooltip and skip duplicates.
		if (m_mapTooltip.find(info.s_dwID) != m_mapTooltip.end())
			continue;

		CsTalk_Tooltip* pTooltip = csnew CsTalk_Tooltip;
		pTooltip->Init(&info);

		m_mapTooltip[info.s_dwID] = pTooltip;
	}

	excel.Close();
}

void CsTalkMng::_SaveBin_ToolTip(FILE* fp)
{
	int nCount = (int)m_mapTooltip.size();
	fwrite(&nCount, sizeof(int), 1, fp);

	CsTalk_Tooltip::MAP_IT it = m_mapTooltip.begin();
	CsTalk_Tooltip::MAP_IT itEnd = m_mapTooltip.end();

	for (; it != itEnd; ++it)
	{
		fwrite(it->second->GetInfo(), sizeof(CsTalk_Tooltip::sINFO), 1, fp);
	}
}

void CsTalkMng::_LoadBin_ToolTip(FILE* fp)
{
	int nCount;
	fread(&nCount, sizeof(int), 1, fp);

	CsTalk_Tooltip::sINFO info;

	for (int i = 0; i < nCount; ++i)
	{
		fread(&info, sizeof(CsTalk_Tooltip::sINFO), 1, fp);

		// Fix:
		// Prevent duplicated tooltip IDs from crashing the client.
		if (m_mapTooltip.find(info.s_dwID) != m_mapTooltip.end())
			continue;

		CsTalk_Tooltip* pObject = csnew CsTalk_Tooltip;
		pObject->Init(&info);

		m_mapTooltip[info.s_dwID] = pObject;
	}
}

void CsTalkMng::_LoadFilePack_ToolTip(int nHandle)
{
	int nCount;
	_read(nHandle, &nCount, sizeof(int));

	CsTalk_Tooltip::sINFO info;

	for (int i = 0; i < nCount; ++i)
	{
		_read(nHandle, &info, sizeof(CsTalk_Tooltip::sINFO));

		// Fix:
		// Prevent duplicated tooltip IDs from crashing the client.
		if (m_mapTooltip.find(info.s_dwID) != m_mapTooltip.end())
			continue;

		CsTalk_Tooltip* pObject = csnew CsTalk_Tooltip;
		pObject->Init(&info);

		m_mapTooltip[info.s_dwID] = pObject;
	}
}

#endif

void CsTalkMng::_SaveBin_Message(FILE* fp)
{
	int nCount = (int)m_mapMessage.size();
	fwrite(&nCount, sizeof(int), 1, fp);

	CsTalk_Message::MAP_IT it = m_mapMessage.begin();
	CsTalk_Message::MAP_IT itEnd = m_mapMessage.end();

	for (; it != itEnd; ++it)
	{
		fwrite(it->second->GetInfo(), sizeof(CsTalk_Message::sINFO), 1, fp);
	}
}

void CsTalkMng::_LoadBin_Message(FILE* fp)
{
	int nCount;
	fread(&nCount, sizeof(int), 1, fp);

	CsTalk_Message::sINFO info;

	for (int i = 0; i < nCount; ++i)
	{
		fread(&info, sizeof(CsTalk_Message::sINFO), 1, fp);

		// Fix:
		// Prevent duplicated message IDs from crashing the client.
		if (m_mapMessage.find(info.s_dwID) != m_mapMessage.end())
			continue;

		CsTalk_Message* pObject = csnew CsTalk_Message;
		pObject->Init(&info);

		m_mapMessage[info.s_dwID] = pObject;
	}
}

void CsTalkMng::_LoadFilePack_Message(int nHandle)
{
	int nCount;
	_read(nHandle, &nCount, sizeof(int));

	CsTalk_Message::sINFO info;

	for (int i = 0; i < nCount; ++i)
	{
		_read(nHandle, &info, sizeof(CsTalk_Message::sINFO));

		// Fix:
		// Prevent duplicated message IDs from crashing the client.
		if (m_mapMessage.find(info.s_dwID) != m_mapMessage.end())
			continue;

		CsTalk_Message* pObject = csnew CsTalk_Message;
		pObject->Init(&info);

		m_mapMessage[info.s_dwID] = pObject;
	}
}

void CsTalkMng::_CheckWordOrder(TCHAR* szKor, TCHAR* szEng, DWORD dwID)
{
	int nLen = (int)_tcslen(szEng);

	if (nLen <= 0)
		return;

	if (szEng[nLen - 1] != '%')
		return;

	TCHAR KorPar[FT_TALK_MSG_BODY_LEN] = { 0, };
	TCHAR EngPar[FT_TALK_MSG_BODY_LEN] = { 0, };

	int nPar = 0;
	nLen = (int)_tcslen(szKor);

	bool bFind = false;

	for (int i = 0; i < nLen - 1; i++)
	{
		if (szKor[i] == '[')
		{
			bFind = true;
		}
		else if (szKor[i + 1] == ']')
		{
			bFind = false;
		}

		if (bFind == true)
		{
			KorPar[nPar] = szKor[i + 1];
			nPar++;
		}
	}

	nPar = 0;
	nLen = (int)_tcslen(szEng);
	bFind = false;

	for (int i = 0; i < nLen - 1; i++)
	{
		if (szEng[i] == '[')
		{
			bFind = true;
		}
		else if (szEng[i + 1] == ']')
		{
			bFind = false;
		}

		if (bFind == true)
		{
			EngPar[nPar] = szEng[i + 1];
			nPar++;
		}
	}

#ifdef _DEBUG
	// GSP 토크확인... 별로 필요 없어보여서 제외
	// if (_tcscmp(KorPar, EngPar) != 0)
	// {
	//     CsMessageBox(MB_OK, _T("ID : %d\n한글 : %s\n영문 : %s"), dwID, szKor, szEng);
	// }
#endif
}