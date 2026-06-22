#include "stdafx.h"
#include "SecondPassword.h"

#include "../../Flow/Flow.h"
#include "../../Flow/FlowMgr.h"
#include "../../ContentsSystem/ContentsSystemDef.h"
#include "../../ContentsSystem/ContentsSystem.h"

#define NUMBER_BUTTON_SIZE CsPoint(50, 50)
#define NUMBER_BUTTON_POS CsPoint(11, 7)

cSecondPassword::cSecondPassword()
    : m_ConversionStr(NULL)
    , m_pOk(NULL)
    , m_pDelete(NULL)
    , m_pClear(NULL)
    , m_pCancel(NULL)
    , m_pUsingSPW(NULL)
    , m_pUsingLaterSPW(NULL)
    , m_pCheckUseSPW(NULL)
    , m_pChange(NULL)
{
}

cSecondPassword::~cSecondPassword()
{
    if (GetSystem())
        GetSystem()->Remove3DBackgroundData();

    if (GetSystem())
        GetSystem()->UnRegisterAll(this);

    Destroy();
}

void cSecondPassword::Destroy()
{
    ClearUI();
    DeleteScript();
    m_Copyright.Delete();
}

void cSecondPassword::ClearUI()
{
    /*
     * IMPORTANTE:
     *
     * Primeiro removemos referências locais.
     * Só depois destruímos os scripts.
     *
     * Isto evita que UpdateMouse() tente usar ponteiros antigos
     * depois de uma troca de janela.
     */
    m_NumButtonList.clear();

    m_pOk = NULL;
    m_pDelete = NULL;
    m_pClear = NULL;
    m_pCancel = NULL;
    m_pChange = NULL;
    m_ConversionStr = NULL;
    m_pUsingSPW = NULL;
    m_pUsingLaterSPW = NULL;
    m_pCheckUseSPW = NULL;

    m_StringList.Delete();

    m_AskWindow.SetVisible(false);
    m_AskWindow.DeleteScript();

    m_KeyPadWindow.DeleteScript();
}

void cSecondPassword::ResetDevice()
{
    m_Copyright.ResetDevice();
    m_StringList.ResetDevice();
    m_KeyPadWindow.ResetDeviceScript();
    m_AskWindow.ResetDeviceScript();
    ResetDeviceScript();
}

void cSecondPassword::MakeCopyright()
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

void cSecondPassword::Init()
{
    InitScript(NULL, CsPoint::ZERO, CsPoint(DEFAULT_SCREEN_WIDTH, DEFAULT_SCREEN_HEIGHT), true);

    AddSprite(CsPoint::ZERO, CsPoint(g_nScreenWidth, g_nScreenHeight), "Lobby\\SecondPassword\\background.dds");

    MakeCopyright();

    m_AskWindow.SetVisible(false);

    GetSystem()->Load3DBackgroundData();

    switch (GetSystem()->GetPassWindowType())
    {
    case SecondPasswordContents::eWindowType_Regist:
    {
        _CreateWindow_Regist();

        if (GLOBALDATA_ST.Is2ndPassInputPass())
            _CreateWindow_UsingAsk();
    }
    break;

    case SecondPasswordContents::eWindowType_AskPW:
    {
        _CreateWindow_AskPW();
    }
    break;
    }
}

void cSecondPassword::Render()
{
    if (m_Copyright.GetSize() > 0)
    {
        m_Copyright.Render(
            CsPoint(g_nScreenWidth >> 1, g_nScreenHeight - 70),
            0,
            DT_CENTER);
    }

    if (m_KeyPadWindow.IsVisible())
    {
        m_KeyPadWindow.RenderScript();

        if (m_ConversionStr)
            m_StringList.Render(m_KeyPadWindow.GetRootClient() + CsPoint(13, 110), 5);

        m_KeyPadWindow.EndRenderScript();
    }

    if (m_AskWindow.IsVisible())
        m_AskWindow.RenderScript();
}

void cSecondPassword::RenderScreenUI()
{
    RenderScript();
}

void cSecondPassword::Render3DModel()
{
    if (GetSystem())
        GetSystem()->RenderBackgroundMap();
}

void cSecondPassword::Update3DAccumtime(float const& fAccumTime)
{
    if (GetSystem())
        GetSystem()->UpdateBackgroundMap(fAccumTime);
}

BOOL cSecondPassword::UpdateMouse()
{
    /*
     * Proteção extra:
     * Se o sistema de SecondPassword já não existir,
     * não processa botões.
     */
    if (!GetSystem())
        return FALSE;

    /*
     * Janela pequena de pergunta:
     * "usar agora" / "usar depois".
     */
    if (m_AskWindow.IsVisible())
    {
        if (!m_AskWindow.IsEnableWindow())
            return TRUE;

        /*
         * Guardamos ponteiros locais para evitar que uma mudança de UI
         * durante o Update_ForMouse() deixe a função a usar membros antigos.
         */
        cButton* pUsingSPW = m_pUsingSPW;
        cButton* pUsingLaterSPW = m_pUsingLaterSPW;
        cCheckBox* pCheckUseSPW = m_pCheckUseSPW;

        if (pUsingSPW && pUsingSPW->Update_ForMouse() == cButton::ACTION_CLICK)
            return TRUE;

        if (pUsingLaterSPW && pUsingLaterSPW->Update_ForMouse() == cButton::ACTION_CLICK)
            return TRUE;

        if (pCheckUseSPW && pCheckUseSPW->Update_ForMouse() == cCheckBox::ACTION_CLICK)
            return TRUE;

        return TRUE;
    }

    if (!m_KeyPadWindow.IsVisible() || !m_KeyPadWindow.IsEnableWindow())
        return FALSE;

    if (m_KeyPadWindow.IsInWindow(CURSOR_ST.GetPos()) != TRUE)
        return FALSE;

    /*
     * Atualiza primeiro os botões de ação.
     * Estes são os que podem mudar a janela inteira.
     * Se algum clicar, saímos logo.
     */
    cButton* pOk = m_pOk;
    cButton* pCancel = m_pCancel;
    cButton* pChange = m_pChange;
    cButton* pDelete = m_pDelete;
    cButton* pClear = m_pClear;

    if (pOk && pOk->Update_ForMouse() == cButton::ACTION_CLICK)
        return TRUE;

    if (pCancel && pCancel->Update_ForMouse() == cButton::ACTION_CLICK)
        return TRUE;

    if (pChange && pChange->Update_ForMouse() == cButton::ACTION_CLICK)
        return TRUE;

    if (pDelete && pDelete->Update_ForMouse() == cButton::ACTION_CLICK)
        return TRUE;

    if (pClear && pClear->Update_ForMouse() == cButton::ACTION_CLICK)
        return TRUE;

    /*
     * Depois os botões numéricos.
     * Fazemos uma cópia da lista para evitar problemas caso algum evento
     * reconstrua a UI durante o update.
     */
    std::list<cButton*> numberButtons = m_NumButtonList;

    std::list<cButton*>::const_iterator it = numberButtons.begin();
    std::list<cButton*>::const_iterator itEnd = numberButtons.end();

    for (; it != itEnd; ++it)
    {
        cButton* pButton = (*it);

        if (pButton && pButton->Update_ForMouse() == cButton::ACTION_CLICK)
            return TRUE;
    }

    return TRUE;
}

BOOL cSecondPassword::UpdateKeyboard(const MSG& p_kMsg)
{
    return FALSE;
}

void cSecondPassword::Update()
{
}

void cSecondPassword::SetConversionStr()
{
    SAFE_POINTER_RET(GetSystem());
    SAFE_POINTER_RET(m_ConversionStr);

    TCHAR szTemp[255] = { 0, };

    _tcscpy_s(szTemp, 255, GetSystem()->GetCurrPassword().c_str());

    size_t nLen = _tcslen(szTemp);

    if (nLen > 1)
        _tcsncset(szTemp, L'*', nLen - 1);

    m_ConversionStr->SetText(szTemp);
}

void cSecondPassword::ClickNumberButton(void* pSender, void* pData)
{
    SAFE_POINTER_RET(pSender);
    SAFE_POINTER_RET(GetSystem());

    cButton* pNumButton = static_cast<cButton*>(pSender);
    SAFE_POINTER_RET(pNumButton);

    sPasswordNumber* pNum = dynamic_cast<sPasswordNumber*>(pNumButton->GetUserData());
    SAFE_POINTER_RET(pNum);

    GetSystem()->AddPasswordData(pNum->m_nNumber);
}

void cSecondPassword::PressDeleteButton(void* pSender, void* pData)
{
    SAFE_POINTER_RET(GetSystem());

    GetSystem()->DeleteOnePassNumber();
}

void cSecondPassword::PressClearButton(void* pSender, void* pData)
{
    SAFE_POINTER_RET(GetSystem());

    GetSystem()->ClearPasswordData();
    SetConversionStr();
}

void cSecondPassword::_CreatePassNumberButton()
{
    /*
     * Layout fixo para evitar que o keypad mude entre Regist e Confirmation.
     */
#ifdef SDM_SECONDPASSWORD_REINFORCE_20180330
    std::wstring keypad = L"1234567890~!@#$%^&*";
#else
    std::wstring keypad = L"1234567890**";
#endif

    CsPoint vPos;

    for (size_t i = 0; i < keypad.size(); ++i)
    {
        vPos.x = 53 + (i % 7) * 50;

#ifdef SDM_SECONDPASSWORD_REINFORCE_20180330
        vPos.y = 117 + (i / 7) * 50;
#else
        vPos.y = 142 + (i / 7) * 50;
#endif

        std::wstring value = keypad.substr(i, 1);

        cButton* pButton = m_KeyPadWindow.AddButton(
            vPos,
            NUMBER_BUTTON_SIZE,
            CsPoint(0, 48),
            "Lobby\\SecondPassword\\lobby_Password_numpad.tga",
            cWindow::SD_At2);

        if (!pButton)
            continue;

        pButton->AddEvent(cButton::BUTTON_LBUP_EVENT, this, &cSecondPassword::ClickNumberButton);

        /*
         * Mantemos a estrutura original do projeto.
         * O sPasswordNumber fica associado ao botão.
         */
        pButton->SetUserData(new sPasswordNumber(value.c_str()));

        cText::sTEXTINFO ti;
        ti.Init(&g_pEngine->m_FontSystem, CFont::FS_16, FONT_BLACK);
        ti.s_bOutLine = false;
        ti.s_eTextAlign = DT_CENTER;
        ti.SetText(value.c_str());

        pButton->SetText(&ti);

        m_NumButtonList.push_back(pButton);
    }

    vPos += CsPoint(50, 0);

    m_pDelete = m_KeyPadWindow.AddButton(
        vPos,
        CsPoint(48, 48),
        CsPoint(0, 48),
        "Lobby\\SecondPassword\\lobby_Password_numpad_back.tga",
        cWindow::SD_At2);

    if (m_pDelete)
        m_pDelete->AddEvent(cButton::BUTTON_LBUP_EVENT, this, &cSecondPassword::PressDeleteButton);

    vPos += CsPoint(50, 0);

    m_pClear = m_KeyPadWindow.AddButton(
        vPos,
        CsPoint(48, 48),
        CsPoint(0, 48),
        "Lobby\\SecondPassword\\lobby_Password_numpad_del.tga",
        cWindow::SD_At2);

    if (m_pClear)
        m_pClear->AddEvent(cButton::BUTTON_LBUP_EVENT, this, &cSecondPassword::PressClearButton);
}

bool cSecondPassword::Construct(void)
{
    if (!CONTENTSSYSTEM_PTR)
        return false;

    SetSystem(CONTENTSSYSTEM_PTR->GetContents<SystemType>(SystemType::IsContentsIdentity()));

    if (!GetSystem())
        return false;

    GetSystem()->Register(SystemType::eSet_ConversionStr, this);
    GetSystem()->Register(SystemType::eCreate_InitRegistWindow, this);
    GetSystem()->Register(SystemType::eCreate_RegistConfirmWindow, this);
    GetSystem()->Register(SystemType::eCreate_AskPasswordWindow, this);
    GetSystem()->Register(SystemType::eCreate_AskPwChangeWindow, this);
    GetSystem()->Register(SystemType::eCreate_PwChangeWindow, this);
    GetSystem()->Register(SystemType::eCreate_PwChangeConfirmWindow, this);
    GetSystem()->Register(SystemType::eCreate_CurrentPasswordWindow, this);

    return true;
}

void cSecondPassword::Notify(int const& iNotifiedEvt, ContentsStream const& kStream)
{
    switch (iNotifiedEvt)
    {
    case SystemType::eSet_ConversionStr:
    {
        SetConversionStr();
    }
    break;

    case SystemType::eCreate_InitRegistWindow:
    {
        ClearUI();
        _CreateWindow_Regist();
    }
    break;

    case SystemType::eCreate_RegistConfirmWindow:
    {
        ClearUI();
        _CreateWindow_RegistConfirm();
    }
    break;

    case SystemType::eCreate_AskPasswordWindow:
    {
        ClearUI();
        _CreateWindow_AskPW();
    }
    break;

    case SystemType::eCreate_CurrentPasswordWindow:
    {
        ClearUI();
        _CreateWindow_CurrentPwWindow();
    }
    break;

    case SystemType::eCreate_PwChangeWindow:
    {
        ClearUI();
        _CreateWindow_PwChange();
    }
    break;

    case SystemType::eCreate_PwChangeConfirmWindow:
    {
        ClearUI();
        _CreateWindow_PwChangeConfirm();
    }
    break;

    case SystemType::eCreate_AskPwChangeWindow:
    {
        ClearUI();
        _CreateWindow_Change2ndPassword_Ask();
    }
    break;
    }
}