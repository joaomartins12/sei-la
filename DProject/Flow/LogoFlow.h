#pragma once
//---------------------------------------------------------------------------
#include "Flow.h"
class CLogo;
class CFade;
//---------------------------------------------------------------------------

namespace Flow
{
	class CLogoFlow : public CFlow
	{
	public:
		CLogoFlow(int p_iID);
		virtual ~CLogoFlow();

	public:
		// ************************
		// Lost, Reset Device
		// ************************
		virtual bool LostDevice(void* p_pvData);
		virtual bool ResetDevice(bool p_bBeforeReset, void* p_pvData);
		// ************************

	protected:
		// ************************
		// Init, Term
		// ************************
		virtual BOOL Initialize();
		virtual void Terminate();
		// ************************

		// ************************
		// Update, Cull, Render
		// ************************
		virtual bool BeginRenderTarget();
		virtual void UpdateFrame();
		virtual void RenderUIFrame();
		// ************************

	private:
		CLogo*		m_pLogo;	
	};
}
//---------------------------------------------------------------------------