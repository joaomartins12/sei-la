// EMERGENT GAME TECHNOLOGIES PROPRIETARY INFORMATION
//
// This software is supplied under the terms of a license agreement or
// nondisclosure agreement with Emergent Game Technologies and may not 
// be copied or disclosed except in accordance with the terms of that 
// agreement.
//
//      Copyright (c) 1996-2007 Emergent Game Technologies.
//      All Rights Reserved.
//
// Emergent Game Technologies, Chapel Hill, North Carolina 27517
// http://www.emergent.net
#ifndef NIMAXEXPORTSETTINGSDIALOG_H
#define NIMAXEXPORTSETTINGSDIALOG_H
#include <NiDialog.h>
#include "NiMAXOptions.h"

#define NUM_PLATFORMS_SUPPORTED 4
class NiMAXExportSettingsDialog : public NiDialog
{
    public:
        NiMAXExportSettingsDialog(NiModuleRef hInstance, 
            NiWindowRef hWndParent);
        ~NiMAXExportSettingsDialog();
        int DoModal();
        virtual void InitDialog();
        virtual BOOL OnCommand(int iWParamLow, int iWParamHigh, 
            long lParam);
        virtual void OnOK();
        virtual BOOL OnMessage(unsigned int uiMessage, int iWParamLow, 
            int iWParamHigh, long lParam);
        bool IsChecked(int iResource);

    protected:
        void OnPlatform();
        void OnSkinPercentageEdit();
        void OnTextureExport();
        void OnEditExportScriptButton();
        void OnEditProcessScriptButton();
        void OnSelectedExportScript();
        void OnSelectedProcessScript();
        void RefreshImageFormatOptions();

        void CheckTextureExportRadio(int iSelection);
        void CheckPixelLayoutRadio(int iSelection);

        NiString GetTextureExport();
        NiPluginToolkitRenderer GetPlatform();
        NiTexture::FormatPrefs::PixelLayout GetPixelLayout();
        void EnableImageFormatOptions(HWND hDlg, bool bEnable);

        bool m_bOptionsChanged;
        NiScriptInfoPtr m_spNewProcessScript;
        NiScriptInfoPtr m_spNewExportScript;
        NiScriptInfoSetPtr m_spAvailableExportScripts;
        NiScriptInfoSetPtr m_spAvailableProcessScripts;
        static const char * ms_aPlatforms[NUM_PLATFORMS_SUPPORTED];

};

#endif