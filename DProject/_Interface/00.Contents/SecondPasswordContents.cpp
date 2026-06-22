#include "StdAfx.h"
#include "SecondPasswordContents.h"
#include "common_vs2019/pPass2.h"

#include "../../Flow/FlowMgr.h"
#include "../../Flow/Flow.h"
#include "../../ContentsSystem/ContentsSystemDef.h"

#include <Windows.h>
#include <sstream>

//////////////////////////////////////////////////////////////////////////
// DEBUG HELPERS
//////////////////////////////////////////////////////////////////////////

static void DebugSecondPassW(const wchar_t* message)
{
    OutputDebugStringW(L"[2PASS][CONTENTS] ");
    OutputDebugStringW(message);
    OutputDebugStringW(L"\n");
}

static void DebugSecondPassValueW(const wchar_t* label, const std::wstring& value)
{
    std::wstringstream ss;
    ss << L"[2PASS][CONTENTS] "
        << label
        << L"=["
        << value
        << L"] len="
        << value.length()
        << L"\n";

    OutputDebugStringW(ss.str().c_str());
}

static void DebugSecondPassIntW(const wchar_t* label, int value)
{
    wchar_t buffer[256] = { 0, };

    swprintf_s(
        buffer,
        L"[2PASS][CONTENTS] %s=%d\n",
        label,
        value);

    OutputDebugStringW(buffer);
}

// 하루 동안 2차 비밀번호 설정 페이지가 안보이게
//#define SKIP_TIME_CHECK_DAY (24 * 60 * 60)

int const SecondPasswordContents::IsContentsIdentity(void)
{
    return E_CT_SECOND_PASSWORD;
}

SecondPasswordContents::SecondPasswordContents(void)
    : m_ePassWindowType(eEnd)
{
    DebugSecondPassW(L"Constructor");

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
    DebugSecondPassW(L"Destructor");

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
    DebugSecondPassW(L"Initialize");

    m_strCurrPW.clear();
    m_strPrePW.clear();
    m_strConfirmPW.clear();

    return true;
}

void SecondPasswordContents::UnInitialize(void)
{
    DebugSecondPassW(L"UnInitialize");
}

bool SecondPasswordContents::IntraConnection(ContentsSystem& System)
{
    DebugSecondPassW(L"IntraConnection");

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
    DebugSecondPassIntW(L"NotifyEventStream event", iNotifiedEvt);

    switch (iNotifiedEvt)
    {
    case CONTENTS_EVENT::EStreamEvt_SecondPassword_Type:
    {
        u1 nType;
        kStream >> nType;

        DebugSecondPassIntW(L"SecondPassword stream type", nType);

        switch (nType)
        {
        case nsLogin::eLOGINSUBTYPE::CHECK2NDPASS:
        {
            DebugSecondPassW(L"WindowType = AskPW / CHECK2NDPASS");
            m_ePassWindowType = eWindowType_AskPW;
        }
        break;

        case nsLogin::eLOGINSUBTYPE::SET2NDPASS:
        {
            DebugSecondPassW(L"WindowType = Regist / SET2NDPASS");
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
    DebugSecondPassW(L"ClearMainActorData");

    ClearAllRegistered();
}

bool SecondPasswordContents::AddPasswordData(TCHAR const& pNumber)
{
#ifndef SDM_SECONDPASSWORD_REINFORCE_20180330
    if (pNumber < _T('0') || pNumber > _T('9'))
    {
        DebugSecondPassW(L"AddPasswordData blocked: invalid non-numeric key");
        return false;
    }
#endif

    if (m_strCurrPW.length() >= 8)
    {
        DebugSecondPassW(L"AddPasswordData blocked: max length reached");
        DebugSecondPassValueW(L"CurrPW", m_strCurrPW);
        return false;
    }

    m_strCurrPW += pNumber;

    DebugSecondPassValueW(L"AddPasswordData CurrPW", m_strCurrPW);

    Notify(eSet_ConversionStr);

    return true;
}

std::wstring SecondPasswordContents::GetCurrPassword() const
{
    return m_strCurrPW;
}

void SecondPasswordContents::ClearPasswordData()
{
    DebugSecondPassValueW(L"ClearPasswordData before CurrPW", m_strCurrPW);

    m_strCurrPW.clear();

    DebugSecondPassW(L"ClearPasswordData done");
}

bool SecondPasswordContents::_CheckPasswordUnabled(std::wstring const& pw, size_t nMinSize, size_t nMaxSize)
{
    DebugSecondPassW(L"_CheckPasswordUnabled called");
    DebugSecondPassValueW(L"_CheckPasswordUnabled pw", pw);
    DebugSecondPassIntW(L"_CheckPasswordUnabled min", (int)nMinSize);
    DebugSecondPassIntW(L"_CheckPasswordUnabled max", (int)nMaxSize);

    if (pw.empty())
    {
        DebugSecondPassW(L"_CheckPasswordUnabled failed: empty password");
        cPrintMsg::PrintMsg(20054);
        return false;
    }

    const size_t nLen = pw.length();

    bool bCheck = true;

    if (!(nLen >= nMinSize && nLen <= nMaxSize))
    {
        DebugSecondPassW(L"_CheckPasswordUnabled failed: invalid length");
        bCheck = false;
    }

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
        {
            DebugSecondPassW(L"_CheckPasswordUnabled failed: all characters are equal");
            bCheck = false;
        }
    }

    if (!bCheck)
        cPrintMsg::PrintMsg(20054);
    else
        DebugSecondPassW(L"_CheckPasswordUnabled success");

    return bCheck;
}

bool SecondPasswordContents::SendCurrent2ndPassword()
{
    DebugSecondPassW(L"SendCurrent2ndPassword called");

    if (!net::account)
    {
        DebugSecondPassW(L"SendCurrent2ndPassword failed: net::account is NULL");
        return false;
    }

    std::wstring currentPassword = m_strCurrPW;

    DebugSecondPassValueW(L"SendCurrent2ndPassword currentPassword", currentPassword);

    if (!_CheckPasswordUnabled(currentPassword, 4, 8))
    {
        DebugSecondPassW(L"SendCurrent2ndPassword failed: invalid password");
        ClearInputPassword();
        return false;
    }

    std::string pw;
    DmCS::StringFn::ToMB(currentPassword, pw);

    DebugSecondPassW(L"SendCurrent2ndPassword sending SendSecondPassCertified");

    GLOBALDATA_ST.Set2ndPassword(pw);
    GLOBALDATA_ST.Set2ndPassType(GData::e2ndNumberPass);

    net::account->SendSecondPassCertified(pw.c_str());

    ClearInputPassword();

    return true;
}

void SecondPasswordContents::SaveRegistPassword()
{
    DebugSecondPassW(L"SaveRegistPassword called");

    std::wstring firstPassword = m_strCurrPW;

    DebugSecondPassValueW(L"SaveRegistPassword firstPassword", firstPassword);
    DebugSecondPassValueW(L"SaveRegistPassword old PrePW", m_strPrePW);
    DebugSecondPassValueW(L"SaveRegistPassword old ConfirmPW", m_strConfirmPW);

    if (!_CheckPasswordUnabled(firstPassword, 6, 8))
    {
        DebugSecondPassW(L"SaveRegistPassword failed: invalid first password");
        ClearInputPassword();
        return;
    }

    m_strPrePW = firstPassword;
    m_strConfirmPW.clear();

    DebugSecondPassValueW(L"SaveRegistPassword saved PrePW", m_strPrePW);

    ClearInputPassword();

    DebugSecondPassW(L"SaveRegistPassword Notify(eCreate_RegistConfirmWindow)");

    Notify(eCreate_RegistConfirmWindow);
}

void SecondPasswordContents::CheckRegistPasswordAndSend()
{
    DebugSecondPassW(L"CheckRegistPasswordAndSend called");

    if (!net::account)
    {
        DebugSecondPassW(L"CheckRegistPasswordAndSend failed: net::account is NULL");
        return;
    }

    std::wstring confirmPassword = m_strCurrPW;

    DebugSecondPassValueW(L"CheckRegistPasswordAndSend PrePW", m_strPrePW);
    DebugSecondPassValueW(L"CheckRegistPasswordAndSend CurrPW", m_strCurrPW);
    DebugSecondPassValueW(L"CheckRegistPasswordAndSend ConfirmPassword", confirmPassword);

    if (!_CheckPasswordUnabled(confirmPassword, 6, 8))
    {
        DebugSecondPassW(L"CheckRegistPasswordAndSend failed: invalid confirm password");
        ClearInputPassword();
        return;
    }

    if (m_strPrePW.empty())
    {
        DebugSecondPassW(L"CheckRegistPasswordAndSend ERROR: PrePW is empty");

        ClearInputPassword();

        cPrintMsg::PrintMsg(cPrintMsg::SECOND_PASS_DISACCORD);

        Notify(eCreate_InitRegistWindow);
        return;
    }

    if (m_strPrePW != confirmPassword)
    {
        DebugSecondPassW(L"CheckRegistPasswordAndSend ERROR: PrePW != ConfirmPassword");
        DebugSecondPassValueW(L"Mismatch PrePW", m_strPrePW);
        DebugSecondPassValueW(L"Mismatch ConfirmPassword", confirmPassword);

        ClearInputPassword();

        cPrintMsg::PrintMsg(cPrintMsg::SECOND_PASS_DISACCORD);

        Notify(eCreate_RegistConfirmWindow);
        return;
    }

    std::string pw;
    DmCS::StringFn::ToMB(confirmPassword, pw);

    DebugSecondPassW(L"CheckRegistPasswordAndSend SUCCESS: calling SendRegister2ndPass");

    net::account->SendRegister2ndPass(const_cast<char*>(pw.c_str()));

    if (g_pResist)
        g_pResist->m_Global.SetSkip2ndPassword(FALSE);

    GLOBALDATA_ST.Set2ndPassType(GData::e2ndNumberPass);

    DebugSecondPassW(L"CheckRegistPasswordAndSend waiting server response 9801");

    /*
     * Não muda já para AskPasswordWindow.
     * Espera pela resposta 9801 do server em Recv2ndPasswordRegister().
     */
    ClearInputPassword();
}

void SecondPasswordContents::OpendChangeWindow()
{
    DebugSecondPassW(L"OpendChangeWindow called");

    ClearInputPassword();

    m_strPrePW.clear();
    m_strConfirmPW.clear();

    Notify(eCreate_CurrentPasswordWindow);
}

void SecondPasswordContents::SaveCurrentPassword()
{
    DebugSecondPassW(L"SaveCurrentPassword called");

    std::wstring currentPassword = m_strCurrPW;

    DebugSecondPassValueW(L"SaveCurrentPassword currentPassword", currentPassword);

    if (!_CheckPasswordUnabled(currentPassword, 4, 8))
    {
        DebugSecondPassW(L"SaveCurrentPassword failed: invalid current password");
        ClearInputPassword();
        return;
    }

    m_strPrePW = currentPassword;
    m_strConfirmPW.clear();

    DebugSecondPassValueW(L"SaveCurrentPassword saved PrePW", m_strPrePW);

    ClearInputPassword();

    Notify(eCreate_PwChangeWindow);
}

void SecondPasswordContents::SaveChangePassword()
{
    DebugSecondPassW(L"SaveChangePassword called");

    std::wstring newPassword = m_strCurrPW;

    DebugSecondPassValueW(L"SaveChangePassword newPassword", newPassword);
    DebugSecondPassValueW(L"SaveChangePassword PrePW", m_strPrePW);

    if (!_CheckPasswordUnabled(newPassword, 6, 8))
    {
        DebugSecondPassW(L"SaveChangePassword failed: invalid new password");
        ClearInputPassword();
        return;
    }

    if (m_strPrePW == newPassword)
    {
        DebugSecondPassW(L"SaveChangePassword failed: new password equals current password");

        ClearInputPassword();

        cPrintMsg::PrintMsg(20054);
        return;
    }

    m_strConfirmPW = newPassword;

    DebugSecondPassValueW(L"SaveChangePassword saved ConfirmPW", m_strConfirmPW);

    ClearInputPassword();

    Notify(eCreate_PwChangeConfirmWindow);
}

void SecondPasswordContents::CheckChangePasswordAndSend()
{
    DebugSecondPassW(L"CheckChangePasswordAndSend called");

    if (!net::account)
    {
        DebugSecondPassW(L"CheckChangePasswordAndSend failed: net::account is NULL");
        return;
    }

    std::wstring confirmPassword = m_strCurrPW;

    DebugSecondPassValueW(L"CheckChangePasswordAndSend PrePW", m_strPrePW);
    DebugSecondPassValueW(L"CheckChangePasswordAndSend ConfirmPW", m_strConfirmPW);
    DebugSecondPassValueW(L"CheckChangePasswordAndSend CurrentConfirm", confirmPassword);

    if (!_CheckPasswordUnabled(confirmPassword, 6, 8))
    {
        DebugSecondPassW(L"CheckChangePasswordAndSend failed: invalid confirm password");
        ClearInputPassword();
        return;
    }

    if (m_strConfirmPW.empty() || m_strConfirmPW != confirmPassword)
    {
        DebugSecondPassW(L"CheckChangePasswordAndSend ERROR: ConfirmPW mismatch");

        ClearInputPassword();

        cPrintMsg::PrintMsg(cPrintMsg::SECOND_PASS_DISACCORD);

        Notify(eCreate_PwChangeConfirmWindow);
        return;
    }

    std::string prevPass;
    DmCS::StringFn::ToMB(m_strPrePW, prevPass);

    std::string changePass;
    DmCS::StringFn::ToMB(confirmPassword, changePass);

    DebugSecondPassW(L"CheckChangePasswordAndSend SUCCESS: calling SendChange2ndPass");

    net::account->SendChange2ndPass(
        const_cast<char*>(prevPass.c_str()),
        const_cast<char*>(changePass.c_str()));

    ClearInputPassword();
}

void SecondPasswordContents::ClearInputPassword()
{
    DebugSecondPassW(L"ClearInputPassword called");

    ClearPasswordData();

    Notify(eSet_ConversionStr);
}

void SecondPasswordContents::DeleteOnePassNumber()
{
    DebugSecondPassW(L"DeleteOnePassNumber called");

    if (m_strCurrPW.empty())
    {
        DebugSecondPassW(L"DeleteOnePassNumber ignored: CurrPW empty");
        return;
    }

    std::wstring::size_type length = m_strCurrPW.length();

    m_strCurrPW.erase(length - 1);

    DebugSecondPassValueW(L"DeleteOnePassNumber CurrPW", m_strCurrPW);

    Notify(eSet_ConversionStr);
}

const int SecondPasswordContents::GetPassWindowType() const
{
    return m_ePassWindowType;
}

void SecondPasswordContents::SkipInputSecondPassword(bool bHideOneDay)
{
    DebugSecondPassW(L"SkipInputSecondPassword called");
    DebugSecondPassIntW(L"SkipInputSecondPassword bHideOneDay", bHideOneDay ? 1 : 0);

    if (g_pResist)
    {
        g_pResist->m_Global.SetSkip2ndPassword(bHideOneDay);

        GLOBALDATA_ST.SetSkip2ndPass();
    }

    if (net::account)
    {
        DebugSecondPassW(L"SkipInputSecondPassword sending SendSkipSecondPass");
        net::account->SendSkipSecondPass();
    }
    else
    {
        DebugSecondPassW(L"SkipInputSecondPassword failed: net::account is NULL");
    }
}

void SecondPasswordContents::GotoBackFromAskWindow()
{
    DebugSecondPassW(L"GotoBackFromAskWindow called");

    if (GLOBALDATA_ST.IsLoginSkiped())
    {
        DebugSecondPassW(L"GotoBackFromAskWindow PostQuitMessage");
        PostQuitMessage(0);
    }
    else
    {
        DebugSecondPassW(L"GotoBackFromAskWindow ChangeFlow Login");
        GLOBALDATA_ST.ResetAccountInfo();
        FLOWMGR_ST.ChangeFlow(Flow::CFlow::FLW_LOGIN);
    }
}

void SecondPasswordContents::Cancel2ndPassChange()
{
    DebugSecondPassW(L"Cancel2ndPassChange called");

    m_strPrePW.clear();
    m_strConfirmPW.clear();

    ClearInputPassword();

    FLOWMGR_ST.ChangeFlow(Flow::CFlow::FLW_SERVERSEL);
}

void SecondPasswordContents::Cancel2ndPassChangingWindow()
{
    DebugSecondPassW(L"Cancel2ndPassChangingWindow called");

    std::string str2Pss = GLOBALDATA_ST.Get2ndPassword();

    m_strPrePW.clear();
    m_strConfirmPW.clear();

    if (str2Pss.empty())
    {
        DebugSecondPassW(L"Cancel2ndPassChangingWindow Notify AskPasswordWindow");

        ClearInputPassword();

        Notify(eCreate_AskPasswordWindow);
    }
    else
    {
        DebugSecondPassW(L"Cancel2ndPassChangingWindow ChangeFlow ServerSelect");

        FLOWMGR_ST.ChangeFlow(Flow::CFlow::FLW_SERVERSEL);
    }
}

void SecondPasswordContents::Cancel2ndPassRegistWindow()
{
    DebugSecondPassW(L"Cancel2ndPassRegistWindow called");

    m_strPrePW.clear();
    m_strConfirmPW.clear();

    ClearInputPassword();

    if (GLOBALDATA_ST.IsLoginSkiped())
    {
        DebugSecondPassW(L"Cancel2ndPassRegistWindow PostQuitMessage");
        PostQuitMessage(0);
    }
    else
    {
        DebugSecondPassW(L"Cancel2ndPassRegistWindow ChangeFlow Login");
        GLOBALDATA_ST.ResetAccountInfo();
        FLOWMGR_ST.ChangeFlow(Flow::CFlow::FLW_LOGIN);
    }
}

bool SecondPasswordContents::Load3DEffect(std::string const& loadFileName)
{
    DebugSecondPassW(L"Load3DEffect called");

    NiStream kStream;

    if (!kStream.Load(loadFileName.c_str()))
    {
        DebugSecondPassW(L"Load3DEffect failed");
        return false;
    }

    NiNodePtr pNode = (NiNode*)kStream.GetObjectAt(0);

    nsCSGBFUNC::InitAnimation(pNode, NiTimeController::APP_INIT, NiTimeController::LOOP);

    m_Effect.SetNiObject(pNode, CGeometry::Normal);

    NiTimeController::StartAnimations(pNode, 0.0f);

    pNode->UpdateEffects();
    pNode->Update(0.0f);

    DebugSecondPassW(L"Load3DEffect success");

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
    DebugSecondPassW(L"Load3DBackgroundData called");

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
    DebugSecondPassW(L"Remove3DBackgroundData called");

    m_Effect.Delete();
}

void SecondPasswordContents::Recv2ndPasswordCheck(void* pData)
{
    DebugSecondPassW(L"Recv2ndPasswordCheck called");

    SAFE_POINTER_RET(pData);

    GS2C_RECV_2ndPass_Certify* result = static_cast<GS2C_RECV_2ndPass_Certify*>(pData);

    DebugSecondPassIntW(L"Recv2ndPasswordCheck result", result->nResult);

    cMessageBox::DelMsg(10019, false);

    if (0 != result->nResult)
    {
        DebugSecondPassW(L"Recv2ndPasswordCheck ERROR result != 0");

        cPrintMsg::PrintMsg(result->nResult);
        GLOBALDATA_ST.Clear2ndPass();
        return;
    }

    DebugSecondPassW(L"Recv2ndPasswordCheck SUCCESS result == 0");

    if (FLOWMGR_ST.IsCurrentFlow(Flow::CFlow::FLW_SERVERSEL))
    {
        DebugSecondPassW(L"Recv2ndPasswordCheck current flow ServerSelect: SendReqClusterList");
        net::account->SendReqClusterList();
    }
    else if (FLOWMGR_ST.IsCurrentFlow(Flow::CFlow::FLW_LOGIN))
    {
        DebugSecondPassW(L"Recv2ndPasswordCheck current flow Login: ChangeFlow ServerSelect");
        FLOWMGR_ST.ChangeFlow(Flow::CFlow::FLW_SERVERSEL);
    }
    else if (FLOWMGR_ST.IsCurrentFlow(Flow::CFlow::FLW_SECURITY))
    {
        DebugSecondPassW(L"Recv2ndPasswordCheck current flow Security");

        cPrintMsg::PrintMsg(cPrintMsg::SECOND_PASS_CONFIRM);

        std::string strPw = GLOBALDATA_ST.Get2ndPassword();

        if (!strPw.empty() && strPw.size() < 6)
        {
            DebugSecondPassW(L"Recv2ndPasswordCheck password size < 6: Notify AskPwChangeWindow");
            Notify(eCreate_AskPwChangeWindow);
        }
        else
        {
            DebugSecondPassW(L"Recv2ndPasswordCheck ChangeFlow ServerSelect");
            FLOWMGR_ST.ChangeFlow(Flow::CFlow::FLW_SERVERSEL);
        }
    }
}

void SecondPasswordContents::Recv2ndPasswordChange(void* pData)
{
    DebugSecondPassW(L"Recv2ndPasswordChange called");

    cMessageBox::DelMsg(10019, false);

    SAFE_POINTER_RET(pData);

    GS2C_RECV_2ndPass_Change* result = static_cast<GS2C_RECV_2ndPass_Change*>(pData);

    DebugSecondPassIntW(L"Recv2ndPasswordChange result", result->nResult);

    if (0 != result->nResult)
    {
        DebugSecondPassW(L"Recv2ndPasswordChange ERROR result != 0");

        cPrintMsg::PrintMsg(result->nResult);

        m_strPrePW.clear();
        m_strConfirmPW.clear();

        Notify(eCreate_CurrentPasswordWindow);
    }
    else
    {
        DebugSecondPassW(L"Recv2ndPasswordChange SUCCESS result == 0");

        cPrintMsg::PrintMsg(cPrintMsg::SECOND_PASS_SET_COMPLETE);

        m_strPrePW.clear();
        m_strConfirmPW.clear();

        Notify(eCreate_AskPasswordWindow);
    }
}

void SecondPasswordContents::Recv2ndPasswordRegister(void* pData)
{
    DebugSecondPassW(L"Recv2ndPasswordRegister called");

    cMessageBox::DelMsg(10019, false);

    SAFE_POINTER_RET(pData);

    GS2C_RECV_2ndPass_Register* result = static_cast<GS2C_RECV_2ndPass_Register*>(pData);

    DebugSecondPassIntW(L"Recv2ndPasswordRegister result", result->nResult);

    if (0 != result->nResult)
    {
        DebugSecondPassW(L"Recv2ndPasswordRegister ERROR result != 0");

        cPrintMsg::PrintMsg(result->nResult);

        Notify(eCreate_RegistConfirmWindow);
        return;
    }

    DebugSecondPassW(L"Recv2ndPasswordRegister SUCCESS result == 0");

    cPrintMsg::PrintMsg(cPrintMsg::SECOND_PASS_SET_COMPLETE);

    m_strPrePW.clear();
    m_strConfirmPW.clear();

    ClearInputPassword();

    Notify(eCreate_AskPasswordWindow);
}