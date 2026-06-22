#include "StdAfx.h"
#include "SecondPasswordContents.h"
#include "common_vs2019/pPass2.h"

#include "../../Flow/FlowMgr.h"
#include "../../Flow/Flow.h"
#include "../../ContentsSystem/ContentsSystemDef.h"

// 하루 동안 2차 비밀번호 설정 페이지가 안보이게
//#define SKIP_TIME_CHECK_DAY (24 * 60 * 60)

int const SecondPasswordContents::IsContentsIdentity(void)
{
    return E_CT_SECOND_PASSWORD;
}

SecondPasswordContents::SecondPasswordContents(void)
    : m_ePassWindowType(eEnd)
{
    m_strCurrPW.clear();
    m_strPrePW.clear();
    m_strConfirmPW.clear();

    GAME_EVENT_ST.AddEvent(EVENT_CODE::RECV_SECONDPASS_CHECK, this, &SecondPasswordContents::Recv2ndPasswordCheck);
    GAME_EVENT_ST.AddEvent(EVENT_CODE::RECV_SECONDPASS_CHANGE, this, &SecondPasswordContents::Recv2ndPasswordChange);
    GAME_EVENT_ST.AddEvent(EVENT_CODE::RECV_SECONDPASS_REGISTER, this, &SecondPasswordContents::Recv2ndPasswordRegister);

    EventRouter()->Register(CONTENTS_EVENT::EStreamEvt_SecondPassword_Type, this);
}

SecondPasswordContents::~SecondPasswordContents(void)
{
    GAME_EVENT_ST.DeleteEventAll(this);

    if (EventRouter())
        EventRouter()->UnRegisterAll(this);
}

int const SecondPasswordContents::GetContentsIdentity(void) const
{
    return IsContentsIdentity();
}

bool SecondPasswordContents::Initialize(void)
{
    m_strCurrPW.clear();
    m_strPrePW.clear();
    m_strConfirmPW.clear();

    return true;
}

void SecondPasswordContents::UnInitialize(void)
{
}

bool SecondPasswordContents::IntraConnection(ContentsSystem& System)
{
    return true;
}

void SecondPasswordContents::Update(float const& fElapsedTime)
{
}

void SecondPasswordContents::NotifyEvent(int const& iNotifiedEvt)
{
}

void SecondPasswordContents::NotifyEventStream(int const& iNotifiedEvt, ContentsStream const& kStream)
{
    switch (iNotifiedEvt)
    {
    case CONTENTS_EVENT::EStreamEvt_SecondPassword_Type:
    {
        u1 nType;
        kStream >> nType;

        switch (nType)
        {
        case nsLogin::eLOGINSUBTYPE::CHECK2NDPASS:
        {
            m_ePassWindowType = eWindowType_AskPW;
        }
        break;

        case nsLogin::eLOGINSUBTYPE::SET2NDPASS:
        {
            m_ePassWindowType = eWindowType_Regist;
        }
        break;
        }
    }
    break;
    }
}

void SecondPasswordContents::MakeWorldData(void)
{
}

void SecondPasswordContents::ClearWorldData(void)
{
}

void SecondPasswordContents::MakeMainActorData(void)
{
}

void SecondPasswordContents::ClearMainActorData(void)
{
    ClearAllRegistered();
}

bool SecondPasswordContents::AddPasswordData(TCHAR const& pNumber)
{
#ifndef SDM_SECONDPASSWORD_REINFORCE_20180330
    if (pNumber < _T('0') || pNumber > _T('9'))
        return false;
#endif

    if (m_strCurrPW.length() >= 8)
        return false;

    m_strCurrPW += pNumber;

    Notify(eSet_ConversionStr);

    return true;
}

std::wstring SecondPasswordContents::GetCurrPassword() const
{
    return m_strCurrPW;
}

void SecondPasswordContents::ClearPasswordData()
{
    m_strCurrPW.clear();
}

bool SecondPasswordContents::_CheckPasswordUnabled(std::wstring const& pw, size_t nMinSize, size_t nMaxSize)
{
    if (pw.empty())
    {
        cPrintMsg::PrintMsg(20054);
        return false;
    }

    const size_t nLen = pw.length();

    bool bCheck = true;

    if (!(nLen >= nMinSize && nLen <= nMaxSize))
        bCheck = false;

    /*
     * Não permite passwords com todos os caracteres iguais.
     * Exemplo: 111111, 222222, aaaaaa.
     */
    if (bCheck && nLen > 1)
    {
        bool bAllSame = true;

        for (size_t i = 1; i < nLen; ++i)
        {
            if (pw[0] != pw[i])
            {
                bAllSame = false;
                break;
            }
        }

        if (bAllSame)
            bCheck = false;
    }

    if (!bCheck)
        cPrintMsg::PrintMsg(20054);

    return bCheck;
}

//////////////////////////////////////////////////////////////////////////
// 2차 비밀 번호 확인 요청
//////////////////////////////////////////////////////////////////////////
bool SecondPasswordContents::SendCurrent2ndPassword()
{
    if (!net::account)
        return false;

    std::wstring currentPassword = m_strCurrPW;

    if (!_CheckPasswordUnabled(currentPassword, 4, 8))
    {
        ClearInputPassword();
        return false;
    }

    std::string pw;
    DmCS::StringFn::ToMB(currentPassword, pw);

    GLOBALDATA_ST.Set2ndPassword(pw);
    GLOBALDATA_ST.Set2ndPassType(GData::e2ndNumberPass);

    net::account->SendSecondPassCertified(pw.c_str());

    ClearInputPassword();

    return true;
}

//////////////////////////////////////////////////////////////////////////
// 2차 비밀 번호 등록 - primeira janela
//////////////////////////////////////////////////////////////////////////
void SecondPasswordContents::SaveRegistPassword()
{
    std::wstring firstPassword = m_strCurrPW;

    if (!_CheckPasswordUnabled(firstPassword, 6, 8))
    {
        ClearInputPassword();
        return;
    }

    /*
     * Guarda a primeira password.
     * Não envia nada ao server ainda.
     */
    m_strPrePW = firstPassword;
    m_strConfirmPW.clear();

    ClearInputPassword();

    Notify(eCreate_RegistConfirmWindow);
}

//////////////////////////////////////////////////////////////////////////
// 2차 비밀 번호 등록 - confirmação
//////////////////////////////////////////////////////////////////////////
void SecondPasswordContents::CheckRegistPasswordAndSend()
{
    SAFE_POINTER_RET(net::account);

    std::wstring confirmPassword = m_strCurrPW;

    if (!_CheckPasswordUnabled(confirmPassword, 6, 8))
    {
        ClearInputPassword();
        return;
    }

    /*
     * Se por algum motivo a primeira password se perdeu,
     * volta ao início do registo.
     */
    if (m_strPrePW.empty())
    {
        ClearInputPassword();

        cPrintMsg::PrintMsg(cPrintMsg::SECOND_PASS_DISACCORD);

        Notify(eCreate_InitRegistWindow);
        return;
    }

    /*
     * Comparação local limpa:
     * primeira password == confirmação.
     */
    if (m_strPrePW != confirmPassword)
    {
        ClearInputPassword();

        cPrintMsg::PrintMsg(cPrintMsg::SECOND_PASS_DISACCORD);

        /*
         * Mantém na janela de confirmação.
         * Assim o jogador só precisa repetir a confirmação,
         * não voltar ao primeiro passo.
         */
        Notify(eCreate_RegistConfirmWindow);
        return;
    }

    std::string pw;
    DmCS::StringFn::ToMB(confirmPassword, pw);

    /*
     * Envia o packet original:
     *
     * pPass2::Register = 9801
     * payload = md5 da password em 32 bytes ASCII
     */
    net::account->SendRegister2ndPass(const_cast<char*>(pw.c_str()));

    if (g_pResist)
        g_pResist->m_Global.SetSkip2ndPassword(FALSE);

    GLOBALDATA_ST.Set2ndPassType(GData::e2ndNumberPass);

    /*
     * Não mudamos já para AskPasswordWindow aqui.
     * Agora esperamos pela resposta do server em Recv2ndPasswordRegister().
     */
    ClearInputPassword();
}

//////////////////////////////////////////////////////////////////////////
// 2차 비밀 번호 변경
//////////////////////////////////////////////////////////////////////////
void SecondPasswordContents::OpendChangeWindow()
{
    ClearInputPassword();

    m_strPrePW.clear();
    m_strConfirmPW.clear();

    Notify(eCreate_CurrentPasswordWindow);
}

void SecondPasswordContents::SaveCurrentPassword()
{
    std::wstring currentPassword = m_strCurrPW;

    if (!_CheckPasswordUnabled(currentPassword, 4, 8))
    {
        ClearInputPassword();
        return;
    }

    m_strPrePW = currentPassword;
    m_strConfirmPW.clear();

    ClearInputPassword();

    Notify(eCreate_PwChangeWindow);
}

void SecondPasswordContents::SaveChangePassword()
{
    std::wstring newPassword = m_strCurrPW;

    if (!_CheckPasswordUnabled(newPassword, 6, 8))
    {
        ClearInputPassword();
        return;
    }

    /*
     * Não permite a nova second password igual à atual.
     */
    if (m_strPrePW == newPassword)
    {
        ClearInputPassword();

        cPrintMsg::PrintMsg(20054);
        return;
    }

    m_strConfirmPW = newPassword;

    ClearInputPassword();

    Notify(eCreate_PwChangeConfirmWindow);
}

void SecondPasswordContents::CheckChangePasswordAndSend()
{
    SAFE_POINTER_RET(net::account);

    std::wstring confirmPassword = m_strCurrPW;

    if (!_CheckPasswordUnabled(confirmPassword, 6, 8))
    {
        ClearInputPassword();
        return;
    }

    if (m_strConfirmPW.empty() || m_strConfirmPW != confirmPassword)
    {
        ClearInputPassword();

        cPrintMsg::PrintMsg(cPrintMsg::SECOND_PASS_DISACCORD);

        Notify(eCreate_PwChangeConfirmWindow);
        return;
    }

    std::string prevPass;
    DmCS::StringFn::ToMB(m_strPrePW, prevPass);

    std::string changePass;
    DmCS::StringFn::ToMB(confirmPassword, changePass);

    net::account->SendChange2ndPass(
        const_cast<char*>(prevPass.c_str()),
        const_cast<char*>(changePass.c_str()));

    ClearInputPassword();
}

void SecondPasswordContents::ClearInputPassword()
{
    ClearPasswordData();

    Notify(eSet_ConversionStr);
}

void SecondPasswordContents::DeleteOnePassNumber()
{
    if (m_strCurrPW.empty())
        return;

    std::wstring::size_type length = m_strCurrPW.length();

    m_strCurrPW.erase(length - 1);

    Notify(eSet_ConversionStr);
}

const int SecondPasswordContents::GetPassWindowType() const
{
    return m_ePassWindowType;
}

void SecondPasswordContents::SkipInputSecondPassword(bool bHideOneDay)
{
    if (g_pResist)
    {
        g_pResist->m_Global.SetSkip2ndPassword(bHideOneDay);

        GLOBALDATA_ST.SetSkip2ndPass();
    }

    if (net::account)
        net::account->SendSkipSecondPass();
}

void SecondPasswordContents::GotoBackFromAskWindow()
{
    if (GLOBALDATA_ST.IsLoginSkiped())
    {
        PostQuitMessage(0);
    }
    else
    {
        GLOBALDATA_ST.ResetAccountInfo();
        FLOWMGR_ST.ChangeFlow(Flow::CFlow::FLW_LOGIN);
    }
}

void SecondPasswordContents::Cancel2ndPassChange()
{
    m_strPrePW.clear();
    m_strConfirmPW.clear();

    ClearInputPassword();

    FLOWMGR_ST.ChangeFlow(Flow::CFlow::FLW_SERVERSEL);
}

void SecondPasswordContents::Cancel2ndPassChangingWindow()
{
    std::string str2Pss = GLOBALDATA_ST.Get2ndPassword();

    m_strPrePW.clear();
    m_strConfirmPW.clear();

    if (str2Pss.empty())
    {
        ClearInputPassword();

        Notify(eCreate_AskPasswordWindow);
    }
    else
    {
        FLOWMGR_ST.ChangeFlow(Flow::CFlow::FLW_SERVERSEL);
    }
}

void SecondPasswordContents::Cancel2ndPassRegistWindow()
{
    m_strPrePW.clear();
    m_strConfirmPW.clear();

    ClearInputPassword();

    if (GLOBALDATA_ST.IsLoginSkiped())
    {
        PostQuitMessage(0);
    }
    else
    {
        GLOBALDATA_ST.ResetAccountInfo();
        FLOWMGR_ST.ChangeFlow(Flow::CFlow::FLW_LOGIN);
    }
}

//////////////////////////////////////////////////////////////////////////
// 3D Object 맵 로드
//////////////////////////////////////////////////////////////////////////
bool SecondPasswordContents::Load3DEffect(std::string const& loadFileName)
{
    NiStream kStream;

    if (!kStream.Load(loadFileName.c_str()))
        return false;

    NiNodePtr pNode = (NiNode*)kStream.GetObjectAt(0);

    nsCSGBFUNC::InitAnimation(pNode, NiTimeController::APP_INIT, NiTimeController::LOOP);

    m_Effect.SetNiObject(pNode, CGeometry::Normal);

    NiTimeController::StartAnimations(pNode, 0.0f);

    pNode->UpdateEffects();
    pNode->Update(0.0f);

    return true;
}

void SecondPasswordContents::RenderBackgroundMap()
{
    m_Effect.Render();
}

void SecondPasswordContents::UpdateBackgroundMap(float const& fAccumTime)
{
    if (m_Effect.m_pNiNode)
    {
        m_Effect.m_pNiNode->Update(fAccumTime);

        CsGBVisible::OnVisible(&m_Effect, m_Effect.m_pNiNode, CsGBVisible::VP_BILLBOARD, fAccumTime);
    }
}

void SecondPasswordContents::Load3DBackgroundData()
{
    sCAMERAINFO ci;

    ci.s_fDist = 2800.0f;
    ci.s_fFarPlane = 100000.0f;
    ci.s_fInitPitch = 0.0f;

    CAMERA_ST.Reset(&ci);
    CAMERA_ST.ReleaseDistRange();
    CAMERA_ST.ReleaseRotationLimit();
    CAMERA_ST.SetDeltaHeight(0.0f);
    CAMERA_ST._UpdateCamera();

    Load3DEffect("Data\\Interface\\Lobby\\BackgroundEff.nif");
}

void SecondPasswordContents::Remove3DBackgroundData()
{
    m_Effect.Delete();
}

//////////////////////////////////////////////////////////////////////////
// Server responses
//////////////////////////////////////////////////////////////////////////
void SecondPasswordContents::Recv2ndPasswordCheck(void* pData)
{
    SAFE_POINTER_RET(pData);

    GS2C_RECV_2ndPass_Certify* result = static_cast<GS2C_RECV_2ndPass_Certify*>(pData);

    cMessageBox::DelMsg(10019, false);

    if (0 != result->nResult)
    {
        cPrintMsg::PrintMsg(result->nResult);
        GLOBALDATA_ST.Clear2ndPass();
        return;
    }

    if (FLOWMGR_ST.IsCurrentFlow(Flow::CFlow::FLW_SERVERSEL))
    {
        net::account->SendReqClusterList();
    }
    else if (FLOWMGR_ST.IsCurrentFlow(Flow::CFlow::FLW_LOGIN))
    {
        FLOWMGR_ST.ChangeFlow(Flow::CFlow::FLW_SERVERSEL);
    }
    else if (FLOWMGR_ST.IsCurrentFlow(Flow::CFlow::FLW_SECURITY))
    {
        cPrintMsg::PrintMsg(cPrintMsg::SECOND_PASS_CONFIRM);

        std::string strPw = GLOBALDATA_ST.Get2ndPassword();

        if (!strPw.empty() && strPw.size() < 6)
        {
            Notify(eCreate_AskPwChangeWindow);
        }
        else
        {
            FLOWMGR_ST.ChangeFlow(Flow::CFlow::FLW_SERVERSEL);
        }
    }
}

void SecondPasswordContents::Recv2ndPasswordChange(void* pData)
{
    cMessageBox::DelMsg(10019, false);

    SAFE_POINTER_RET(pData);

    GS2C_RECV_2ndPass_Change* result = static_cast<GS2C_RECV_2ndPass_Change*>(pData);

    if (0 != result->nResult)
    {
        cPrintMsg::PrintMsg(result->nResult);

        m_strPrePW.clear();
        m_strConfirmPW.clear();

        Notify(eCreate_CurrentPasswordWindow);
    }
    else
    {
        cPrintMsg::PrintMsg(cPrintMsg::SECOND_PASS_SET_COMPLETE);

        m_strPrePW.clear();
        m_strConfirmPW.clear();

        Notify(eCreate_AskPasswordWindow);
    }
}

void SecondPasswordContents::Recv2ndPasswordRegister(void* pData)
{
    cMessageBox::DelMsg(10019, false);

    SAFE_POINTER_RET(pData);

    GS2C_RECV_2ndPass_Register* result = static_cast<GS2C_RECV_2ndPass_Register*>(pData);

    if (0 != result->nResult)
    {
        /*
         * Se isto aparecer, o erro veio do server.
         * O registo local já foi enviado.
         */
        cPrintMsg::PrintMsg(result->nResult);

        Notify(eCreate_RegistConfirmWindow);
        return;
    }

    cPrintMsg::PrintMsg(cPrintMsg::SECOND_PASS_SET_COMPLETE);

    m_strPrePW.clear();
    m_strConfirmPW.clear();

    ClearInputPassword();

    Notify(eCreate_AskPasswordWindow);
}