#include "stdafx.h"
#include "SelectServer.h"

#include "../../Flow/Flow.h"
#include "../../Flow/FlowMgr.h"
#include "../../ContentsSystem/ContentsSystemDef.h"
#include "../../ContentsSystem/ContentsSystem.h"

namespace
{
    static const int SERVER_CARD_W = 154;
    static const int SERVER_CARD_H = 314;
    static const int SERVER_CARD_GAP = 5;

    static const int SERVER_LIST_WINDOW_W = 1024;
    static const int SERVER_LIST_WINDOW_H = 440;

    static const int SERVER_LIST_WINDOW_BASE_H = 410;
    static const int SERVER_LIST_Y_IN_WINDOW = 35;

    static int GetServerListWidth(size_t nServerCount)
    {
        if (nServerCount <= 0)
            return SERVER_CARD_W;

        return (SERVER_CARD_W * (int)nServerCount) + (SERVER_CARD_GAP * ((int)nServerCount - 1));
    }

    static int GetClampedServerListWidth(size_t nServerCount)
    {
        int listW = GetServerListWidth(nServerCount);

        if (listW > SERVER_LIST_WINDOW_W)
            listW = SERVER_LIST_WINDOW_W;

        return listW;
    }

    /*
     * Posição da janela principal da lista.
     * Esta janela é centrada no ecrã inteiro.
     */
    static CsPoint GetServerListWindowPos()
    {
        return CsPoint(
            (g_nScreenWidth - SERVER_LIST_WINDOW_W) / 2,
            (g_nScreenHeight - SERVER_LIST_WINDOW_BASE_H) / 2);
    }

    /*
     * Posição local da grid dentro de m_SelectListWindow.
     *
     * IMPORTANTE:
     * Isto NÃO pode usar g_nScreenWidth/g_nScreenHeight diretamente,
     * porque m_pServerListBox é adicionado como child de m_SelectListWindow.
     *
     * Se usarmos posição absoluta aqui e depois AddChildControl(),
     * o offset da janela é somado duas vezes.
     */
    static CsPoint GetServerListLocalPos(size_t nServerCount)
    {
        const int listW = GetClampedServerListWidth(nServerCount);

        return CsPoint(
            (SERVER_LIST_WINDOW_W - listW) / 2,
            SERVER_LIST_Y_IN_WINDOW);
    }
}

CSelectServer::CSelectServer()
    : m_pServerListBox(NULL)
    , m_pConnectServerBtn(NULL)
    , m_pGotoBackBtn(NULL)
    , m_pListRefreshBtn(NULL)
{
}

CSelectServer::~CSelectServer()
{
    if (GetSystem())
    {
        GetSystem()->ServerListClear();
        GetSystem()->UnRegisterAll(this);
        GetSystem()->Remove3DBackgroundData();
    }

    m_Copyright.Delete();

    DeleteScript();

    m_SelectListWindow.DeleteScript();
    m_SelectBack.DeleteScript();
}

void CSelectServer::_MakeCopyright()
{
    cText::sTEXTINFO ti;
    ti.Init(&g_pEngine->m_FontText, CFont::FS_11, NiColor::WHITE);

    std::wstring copyRight = UISTRING_TEXT("COPYRIGHT_TOEI");

    if (!copyRight.empty())
    {
        ti.SetString(copyRight.c_str());

        cString* pStr = NiNew cString;
        pStr->AddText(&ti, CsPoint::ZERO);

        m_Copyright.AddTail(pStr);
    }

    copyRight = UISTRING_TEXT("COPYRIGHT_MOVEGAMES");

    if (!copyRight.empty())
    {
        ti.SetString(copyRight.c_str());

        cString* pStr = NiNew cString;
        pStr->AddText(&ti, CsPoint::ZERO);

        m_Copyright.AddTail(pStr);
    }
}

bool CSelectServer::Init()
{
    OutputDebugStringA("[SelectServer] Init begin\n");

    if (!GetSystem())
    {
        OutputDebugStringA("[SelectServer] Init failed: GetSystem NULL\n");
        return false;
    }

    InitScript(NULL, CsPoint::ZERO, CsPoint(g_nScreenWidth, g_nScreenHeight), false);

    AddSprite(
        CsPoint::ZERO,
        CsPoint(g_nScreenWidth, g_nScreenHeight),
        "Lobby\\SelectServer\\background.dds",
        true);

    _MakeCopyright();
    _MakeServerListBack();
    _MakeServerList();

    GetSystem()->Load3DBackgroundData();

    /*
        IMPORTANTE:
        Antes estava no início:
            GetSystem()->CheckConnectAccountSever();

        Isso crashava porque era chamado antes da UI/lista estar pronta.

        Agora pedimos a lista só depois da UI estar criada.
    */
    OutputDebugStringA("[SelectServer] RequestServerList begin\n");
    GetSystem()->RequestServerList();
    OutputDebugStringA("[SelectServer] RequestServerList end\n");

    OutputDebugStringA("[SelectServer] Init ok\n");

    return true;
}

void CSelectServer::_MakeServerListBack()
{
    m_SelectBack.InitScript(
        NULL,
        CsPoint::ZERO,
        CsPoint(g_nScreenWidth, g_nScreenHeight),
        false);

    m_SelectBack.AddSprite(
        CsPoint((g_nScreenWidth - 446) / 2, 0),
        CsPoint(446, 118),
        "Lobby\\TopText_BG.tga");

    {
        cText::sTEXTINFO ti;
        ti.Init(CFont::FS_16, FONT_WHITE);
        ti.s_eTextAlign = DT_CENTER;
        ti.SetText(UISTRING_TEXT("SERVERLIST_TITLE").c_str());

        m_SelectBack.AddText(&ti, CsPoint(g_nScreenWidth / 2, 49), true);
    }

    // 뒤로 가기
    m_pGotoBackBtn = m_SelectBack.AddButton(
        CsPoint(100, g_nScreenHeight - 80),
        CsPoint(176, 50),
        CsPoint(0, 50),
        "CommonUI\\Simple_btn_L.tga",
        cWindow::SD_Bu3);

    if (m_pGotoBackBtn)
    {
        cText::sTEXTINFO ti;
        ti.Init(&g_pEngine->m_FontText, CFont::FS_14);

        if (GLOBALDATA_ST.IsLoginSkiped())
            ti.SetText(UISTRING_TEXT("SERVERLIST_EXIT_GAME").c_str());
        else
            ti.SetText(UISTRING_TEXT("SERVERLIST_GOTO_BACK_PAGE").c_str());

        ti.s_eTextAlign = DT_CENTER;

        m_pGotoBackBtn->SetText(&ti);
        m_pGotoBackBtn->AddEvent(cButton::BUTTON_LBUP_EVENT, this, &CSelectServer::PressCancelButton);
    }

    // 캐릭터 선택화면 이동
    m_pConnectServerBtn = m_SelectBack.AddButton(
        CsPoint(g_nScreenWidth - 100 - 176, g_nScreenHeight - 80),
        CsPoint(176, 50),
        CsPoint(0, 50),
        "CommonUI\\Simple_btn_L.tga",
        cWindow::SD_Bu3);

    if (m_pConnectServerBtn)
    {
        cText::sTEXTINFO ti;
        ti.Init(&g_pEngine->m_FontText, CFont::FS_14);
        ti.SetText(UISTRING_TEXT("SERVERLIST_CONNECT_SERVER").c_str());
        ti.s_eTextAlign = DT_CENTER;

        m_pConnectServerBtn->SetText(&ti);
        m_pConnectServerBtn->AddEvent(cButton::BUTTON_LBUP_EVENT, this, &CSelectServer::PressOkButton);
    }
}

void CSelectServer::_MakeServerList()
{
    m_SelectListWindow.InitScript(
        NULL,
        GetServerListWindowPos(),
        CsPoint(SERVER_LIST_WINDOW_W, SERVER_LIST_WINDOW_H),
        false);

    {
        cText::sTEXTINFO ti;
        ti.Init(&g_pEngine->m_FontText, CFont::FS_14);
        ti.SetText(UISTRING_TEXT("SERVERLIST_SELECT_SERVER_MSG").c_str());
        ti.s_eTextAlign = DT_CENTER;

        m_SelectListWindow.AddText(&ti, CsPoint(SERVER_LIST_WINDOW_W / 2, 0));
    }

    ResetServerList();

    // 새로 고침
    m_pListRefreshBtn = m_SelectListWindow.AddButton(
        CsPoint((SERVER_LIST_WINDOW_W - 52) / 2, 359),
        CsPoint(52, 51),
        CsPoint(0, 51),
        "Lobby\\SelectServer\\lobby_server_refresh.tga",
        cWindow::SD_Bu3);

    if (m_pListRefreshBtn)
        m_pListRefreshBtn->AddEvent(cButton::BUTTON_LBUP_EVENT, this, &CSelectServer::PressRefreshButton);
}

void CSelectServer::_MakeListControl(size_t const& nServerCount)
{
    if (NULL != m_pServerListBox || nServerCount <= 0)
        return;

    m_pServerListBox = NiNew cGridListBox;

    SAFE_POINTER_RET(m_pServerListBox);

    CsPoint uiSize(GetClampedServerListWidth(nServerCount), SERVER_CARD_H);

    /*
     * IMPORTANTE:
     *
     * O primeiro argumento do Init() tem de ser cScriptUI*.
     * m_SelectListWindow é cWindow, por isso NÃO pode ser passado aqui.
     *
     * Usamos NULL no Init(), e depois fazemos:
     * m_SelectListWindow.AddChildControl(m_pServerListBox);
     *
     * A posição continua a ser local dentro da m_SelectListWindow.
     */
    m_pServerListBox->Init(
        NULL,
        GetServerListLocalPos(nServerCount),
        uiSize,
        CsPoint(SERVER_CARD_GAP, 0),
        CsPoint(SERVER_CARD_W, SERVER_CARD_H),
        cGridListBox::LowRightDown,
        cGridListBox::LeftTop,
        0,
        false,
        0);

    m_pServerListBox->SetMouseOverImg("Lobby\\SelectServer\\lobby_server_window_glow.tga");
    m_pServerListBox->SetSelectedImg("Lobby\\SelectServer\\lobby_server_window_glow.tga");
    m_pServerListBox->SetBackOverAndSelectedImgRender(false);

    m_pServerListBox->AddEvent(cGridListBox::GRID_SELECTED_ITEM, this, &CSelectServer::SeletedServer);

    m_SelectListWindow.AddChildControl(m_pServerListBox);
}

void CSelectServer::_ChangeListSize(size_t const& nServerCount)
{
    SAFE_POINTER_RET(m_pServerListBox);

    CsPoint uiSize(GetClampedServerListWidth(nServerCount), SERVER_CARD_H);

    m_pServerListBox->SetPos(GetServerListLocalPos(nServerCount));
    m_pServerListBox->SetSize(uiSize);
    m_pServerListBox->ResetItemExtendSize();
    m_pServerListBox->ResetDevice();
}

void CSelectServer::ResetServerList()
{
    if (!GetSystem())
    {
        OutputDebugStringA("[SelectServer] ResetServerList skipped: GetSystem NULL\n");
        return;
    }

    std::list<sServerInfo> const& listInfo = GetSystem()->GetServerList();

    char log[256];
    sprintf_s(log, "[SelectServer] ResetServerList server count = %d\n", (int)listInfo.size());
    OutputDebugStringA(log);

    if (NULL == m_pServerListBox)
        _MakeListControl(listInfo.size());
    else
        _ChangeListSize(listInfo.size());

    SAFE_POINTER_RET(m_pServerListBox);

    m_pServerListBox->RemoveAllItem();

    int nLastConnectServerIDX = GetSystem()->GetSelectedLastServerIdx();

    std::list<sServerInfo>::const_iterator it = listInfo.begin();

    for (int n = 0; it != listInfo.end(); ++it, ++n)
    {
        cString* pString = NiNew cString;

        SAFE_POINTER_BEK(pString);

        cSprite* pBackImg = NiNew cSprite;

        if (pBackImg)
        {
            pBackImg->Init(
                NULL,
                CsPoint::ZERO,
                CsPoint(SERVER_CARD_W, SERVER_CARD_H),
                "Lobby\\SelectServer\\lobby_server_window.tga",
                false);

            cString::sSPRITE* sSprite = pString->AddSprite(pBackImg);

            if (sSprite)
                sSprite->SetAutoPointerDelete(true);
        }

        cSprite* pServerImg = NiNew cSprite;

        if (pServerImg)
        {
            std::string serverimg = GetSystem()->GetServerImg((*it).m_nIDX);

            pServerImg->Init(
                NULL,
                CsPoint::ZERO,
                CsPoint(126, 245),
                serverimg.c_str(),
                false);

            cString::sSPRITE* sSprite = pString->AddSprite(pServerImg, CsPoint(14, 53));

            if (sSprite)
                sSprite->SetAutoPointerDelete(true);
        }

        {
            // 서버 명
            cText::sTEXTINFO ti;
            ti.Init(&g_pEngine->m_FontSystem, CFont::FS_12, NiColor::WHITE);
            ti.SetText((*it).m_szClusterName.c_str());

            int nHafW = ti.GetTextWidth() / 2;

            pString->AddText(&ti, CsPoint((SERVER_CARD_W / 2) - nHafW, 28));
        }

#ifdef SDM_SERVER_STATE_SHOW_20181011
        if ((*it).m_bNewServer)
        {
            cSprite* pNewServer = NiNew cSprite;

            pNewServer->Init(
                NULL,
                CsPoint::ZERO,
                CsPoint(35, 11),
                "Lobby\\SelectServer\\NewServer.tga",
                false);

            cString::sSPRITE* paddImg = pString->AddSprite(pNewServer, CsPoint(20, 282));

            if (paddImg)
                paddImg->SetAutoPointerDelete(true);
        }

        std::string sHaveIconName = "Lobby\\SelectServer\\lobby_server_icon_02.tga";
        NiColor noHaveCountColor = NiColor::WHITE;

        if ((*it).m_nTamerNumber > 0)
        {
            sHaveIconName = "Lobby\\SelectServer\\lobby_server_icon_01.tga";
            noHaveCountColor = NiColor(1.0f, 0.9f, 0.3f);
        }

        cSprite* pHaveCharacterImg = NiNew cSprite;

        if (pHaveCharacterImg)
        {
            pHaveCharacterImg->Init(
                NULL,
                CsPoint::ZERO,
                CsPoint(44, 39),
                sHaveIconName.c_str(),
                false);

            cString::sSPRITE* sSprite = pString->AddSprite(pHaveCharacterImg, CsPoint(51, 268));

            if (sSprite)
                sSprite->SetAutoPointerDelete(true);
        }

        {
            cText::sTEXTINFO ti2;
            ti2.Init(&g_pEngine->m_FontSystem, CFont::FS_13, noHaveCountColor);
            ti2.SetText((*it).m_nTamerNumber);

            pString->AddText(&ti2, CsPoint(95, 278));
        }
#else
        if ((*it).m_bNewServer)
        {
            cSprite* pNewServer = NiNew cSprite;

            pNewServer->Init(
                NULL,
                CsPoint::ZERO,
                CsPoint(35, 11),
                "Lobby\\SelectServer\\NewServer.tga",
                false);

            cString::sSPRITE* paddImg = pString->AddSprite(pNewServer, CsPoint(20, 276));

            if (paddImg)
                paddImg->SetAutoPointerDelete(true);
        }

        std::string sHaveIconName = "Lobby\\SelectServer\\lobby_server_icon_02.tga";
        NiColor noHaveCountColor = NiColor::WHITE;

        if ((*it).m_nTamerNumber > 0)
        {
            sHaveIconName = "Lobby\\SelectServer\\lobby_server_icon_01.tga";
            noHaveCountColor = NiColor(1.0f, 0.9f, 0.3f);
        }

        cSprite* pHaveCharacterImg = NiNew cSprite;

        if (pHaveCharacterImg)
        {
            pHaveCharacterImg->Init(
                NULL,
                CsPoint::ZERO,
                CsPoint(44, 39),
                sHaveIconName.c_str(),
                false);

            cString::sSPRITE* sSprite = pString->AddSprite(pHaveCharacterImg, CsPoint(51, 262));

            if (sSprite)
                sSprite->SetAutoPointerDelete(true);
        }

        {
            cText::sTEXTINFO ti2;
            ti2.Init(&g_pEngine->m_FontSystem, CFont::FS_13, noHaveCountColor);
            ti2.SetText((*it).m_nTamerNumber);

            pString->AddText(&ti2, CsPoint(95, 272));
        }
#endif

        // 마지막 접속 지역
        if (nLastConnectServerIDX == (*it).m_nIDX)
        {
            cSprite* pLastSelImg = NiNew cSprite;

            if (pLastSelImg)
            {
                pLastSelImg->Init(
                    NULL,
                    CsPoint::ZERO,
                    CsPoint(144, 30),
                    "Lobby\\SelectServer\\lobby_server_recently.tga",
                    false);

                cString::sSPRITE* sSprite = pString->AddSprite(pLastSelImg, CsPoint(5, 219));

                if (sSprite)
                    sSprite->SetAutoPointerDelete(true);
            }

            cText::sTEXTINFO ti2;
            ti2.Init(&g_pEngine->m_FontSystem, CFont::FS_11, NiColor(1.0f, 0.9f, 0.0f));
            ti2.SetText(UISTRING_TEXT("SERVERLIST_LAST_CONNECT_SERVER").c_str());
            ti2.s_eTextAlign = DT_CENTER;

            pString->AddText(&ti2, CsPoint(77, 227));
        }

#ifdef SDM_SERVER_STATE_SHOW_20181011
        cImage* pServerStateImg = NiNew cImage;

        if (pServerStateImg)
        {
            pServerStateImg->Init(
                NULL,
                CsPoint::ZERO,
                CsPoint(138, 20),
                "Lobby\\SelectServer\\lobby_server_status.tga",
                false);

            pServerStateImg->SetTexToken(CsPoint(0, 20));

            cString::sIMAGE* paddImg = pString->AddImage(
                pServerStateImg,
                (*it).m_nClusterState,
                CsPoint(8, 248),
                CsPoint(138, 20));

            if (paddImg)
                paddImg->SetAutoPointerDelete(true);
        }

        {
            std::wstring serverStateText;
            NiColor textColor = NiColor::WHITE;

            switch ((*it).m_nClusterState)
            {
            case sServerInfo::eComfortalbe:
                serverStateText = UISTRING_TEXT("SERVER_SELECT_SERVER_STATE_TEXT_COMFORTABLE");
                textColor = FONT_GREEN;
                break;

            case sServerInfo::eNormal:
                serverStateText = UISTRING_TEXT("SERVER_SELECT_SERVER_STATE_TEXT_NORMAL");
                break;

            case sServerInfo::eCrowded:
                serverStateText = UISTRING_TEXT("SERVER_SELECT_SERVER_STATE_TEXT_CROWDED");
                textColor = FONT_RED;
                break;

            case sServerInfo::eFull:
                serverStateText = UISTRING_TEXT("SERVER_SELECT_SERVER_STATE_TEXT_FULL");
                textColor = FONT_GLAY;
                break;
            }

            cText::sTEXTINFO ti2;
            ti2.Init(&g_pEngine->m_FontSystem, CFont::FS_11, textColor);
            ti2.s_eTextAlign = DT_CENTER;
            ti2.SetText(serverStateText.c_str());

            pString->AddText(&ti2, CsPoint(77, 250));
        }
#endif

        cGridListBoxItem* addItem = NiNew cGridListBoxItem(n, CsPoint(SERVER_CARD_W, SERVER_CARD_H));

        addItem->SetItem(pString);
        addItem->SetUserData(new sSeverNumber((*it).m_nIDX));

        m_pServerListBox->AddItem(addItem);
    }

    LastConnectedServerSelect(GetSystem()->GetSelectedLastServerIdx());
}

void CSelectServer::LastConnectedServerSelect(UINT nSelectServerIdx)
{
    if (!m_pServerListBox)
        return;

    int allItemCount = (int)m_pServerListBox->GetItemCount();

    for (int n = 0; n < allItemCount; ++n)
    {
        cGridListBoxItem const* pItem = m_pServerListBox->GetItemFormPos(n);

        SAFE_POINTER_CON(pItem);

        sSeverNumber* pUserData = dynamic_cast<sSeverNumber*>(pItem->GetUserData());

        SAFE_POINTER_CON(pUserData);

        if (pUserData->m_nNumber != nSelectServerIdx)
            continue;

        m_pServerListBox->SetSelectedItemFromIdx(n, false);
        break;
    }
}

BOOL CSelectServer::UpdateMouse()
{
    if (m_pConnectServerBtn && m_pConnectServerBtn->Update_ForMouse() == cButton::ACTION_CLICK)
        return TRUE;

    if (m_pGotoBackBtn && m_pGotoBackBtn->Update_ForMouse() == cButton::ACTION_CLICK)
        return TRUE;

    if (m_pListRefreshBtn && m_pListRefreshBtn->Update_ForMouse() == cButton::ACTION_CLICK)
        return TRUE;

    if (m_pServerListBox && m_pServerListBox->Update_ForMouse(CURSOR_ST.GetPos()))
        return TRUE;

    return FALSE;
}

BOOL CSelectServer::UpdateKeyboard(const MSG& p_kMsg)
{
    const int isBitSet = (DWORD)p_kMsg.lParam & 0x40000000;

    switch (p_kMsg.message)
    {
    case WM_KEYDOWN:
    {
        switch (p_kMsg.wParam)
        {
        case VK_UP:
        case VK_LEFT:
        {
            if (isBitSet == 0)
            {
                if (m_pServerListBox)
                    m_pServerListBox->ChangeSelectFront();
            }

            return TRUE;
        }
        break;

        case VK_DOWN:
        case VK_RIGHT:
        {
            if (isBitSet == 0)
            {
                if (m_pServerListBox)
                    m_pServerListBox->ChangeSelectNext();
            }

            return TRUE;
        }
        break;

        case VK_RETURN:
        {
            if (isBitSet == 0)
            {
                if (m_pConnectServerBtn)
                    m_pConnectServerBtn->KeyboardBtnDn();
            }

            return TRUE;
        }
        break;

        case VK_ESCAPE:
        {
            if (isBitSet == 0)
            {
                if (m_pGotoBackBtn)
                    m_pGotoBackBtn->KeyboardBtnDn();
            }

            return TRUE;
        }
        break;
        }
    }
    break;

    case WM_KEYUP:
    {
        switch (p_kMsg.wParam)
        {
        case VK_RETURN:
        {
            if (m_pConnectServerBtn && GetSystem())
            {
                m_pConnectServerBtn->KeyboardBtnUp();
                PressOkButton(NULL, NULL);
            }

            return TRUE;
        }
        break;

        case VK_ESCAPE:
        {
            if (m_pGotoBackBtn && GetSystem())
            {
                m_pGotoBackBtn->KeyboardBtnUp();
                GetSystem()->gotoBack();
            }

            return TRUE;
        }
        break;
        }
    }
    break;
    }

    return FALSE;
}

// 배경 UI 랜더링
void CSelectServer::RenderScreenUI()
{
    RenderScript();
}

void CSelectServer::Render()
{
    if (m_Copyright.GetSize() > 0)
    {
        m_Copyright.Render(
            CsPoint(g_nScreenWidth >> 1, g_nScreenHeight - 70),
            0,
            DT_CENTER);
    }

    m_SelectBack.RenderScript();
    m_SelectListWindow.RenderScript();
}

void CSelectServer::ResetDevice()
{
    ResetDeviceScript();

    m_SelectBack.ResetDeviceScript();
    m_SelectListWindow.ResetDeviceScript();

    m_Copyright.ResetDevice();
}

void CSelectServer::Render3DModel()
{
    if (GetSystem())
        GetSystem()->RenderBackgroundMap();
}

void CSelectServer::Update3DAccumtime(float const& fAccumTime)
{
    if (GetSystem())
        GetSystem()->UpdateBackgroundMap(fAccumTime);
}

void CSelectServer::SeletedServer(void* pSender, void* pData)
{
    SAFE_POINTER_RET(pData);

    cGridListBoxItem* pItem = static_cast<cGridListBoxItem*>(pData);

    SAFE_POINTER_RET(pItem);

    sSeverNumber* pUserData = dynamic_cast<sSeverNumber*>(pItem->GetUserData());

    SAFE_POINTER_RET(pUserData);

    GetSystem()->ConnectingServer(pUserData->m_nNumber);
}

bool CSelectServer::Construct(void)
{
    if (!CONTENTSSYSTEM_PTR)
        return false;

    SetSystem(CONTENTSSYSTEM_PTR->GetContents<SystemType>(SystemType::IsContentsIdentity()));

    if (!GetSystem())
        return false;

    GetSystem()->Register(SystemType::eRecieve_ServerList, this);

    return true;
}

void CSelectServer::Notify(int const& iNotifiedEvt, ContentsStream const& kStream)
{
    switch (iNotifiedEvt)
    {
    case SystemType::eRecieve_ServerList:
    {
        OutputDebugStringA("[SelectServer] Notify eRecieve_ServerList\n");
        ResetServerList();
    }
    break;
    }
}

void CSelectServer::PressOkButton(void* pSender, void* pData)
{
    SAFE_POINTER_RET(m_pServerListBox);

    cGridListBoxItem const* pItem = m_pServerListBox->GetSelectedItem();

    SAFE_POINTER_RET(pItem);

    sSeverNumber* pUserData = dynamic_cast<sSeverNumber*>(pItem->GetUserData());

    SAFE_POINTER_RET(pUserData);

    GetSystem()->ConnectingServer(pUserData->m_nNumber);
}

void CSelectServer::PressCancelButton(void* pSender, void* pData)
{
    GetSystem()->gotoBack();
}

void CSelectServer::PressRefreshButton(void* pSender, void* pData)
{
    GetSystem()->RequestServerList();
}