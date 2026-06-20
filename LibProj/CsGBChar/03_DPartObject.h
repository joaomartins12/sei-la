
#pragma once

namespace nsDPART
{
	enum ePART
	{
		None = 0,
		Skin = 1,
	};

	struct sDPART_INFO
	{
		sDPART_INFO() { s_nFileTableID = 0; s_nPartIndex = 0; s_nRemainTime = 0; }
		UINT					s_nFileTableID;
		int						s_nPartIndex;
		std::string				s_File;
		UINT					s_nRemainTime;
	};
}


class CsC_DPartObject : public CsC_AvObject
{
	CsCRTTI_H
	THREAD_MEMPOOL_H(CsC_DPartObject)

public:
	CsC_DPartObject();

public:
	virtual void		Delete();
	virtual void		DeleteUpdate();

	virtual void		Update(float fDeltaTime, bool bAnimation = true);

	virtual void		ChangeKFM(UINT nFTID);
	virtual void		ChangeModel(UINT nFTID, LPCTSTR szName);
	//====================================================================================================
	//		Load
	//====================================================================================================
protected:
	bool				m_bInitialLoadPart;

protected:
	virtual bool		_IsInitialLoad() { return m_bInitialLoadPart; }

	//====================================================================================================
	//	Part
	//====================================================================================================


public:
	virtual void		ThreadCallBack_LoadKFM(sLOAD_KFM* pLoad);
	virtual void		ThreadCallBack_AttachPart(sLOAD_PART* pPart);
	virtual void		ThreadLoad_Part(std::list<nsDPART::sDPART_INFO> const* dPartsInfo, bool bIncludeBaseKFM = true);

public:
	void				ChangeParts(nsDPART::sDPART_INFO const& pPartInfo);
	bool				IsPartsApplied(int const& nType) const;
	void				DetachParts(int const& nType);
	void				ClearAllParts();

protected:
	void				_UpdateReadyPart();

private:
	void				_SetParts(int const& nType, NiNodePtr pPart);
	bool				RemapGeomSkinInstance(NiNode* p_pkSceneRoot, NiGeometry* p_pkGeom);
	void				RemapObjectSkinInstance(NiNode* p_pkSceneRoot, NiAVObject* p_pkObject);
	
private:
	struct sParts
	{
		sParts() : s_pPartObj(0),s_nFileTableID(0),s_nRemainTime(0)
		{}

		NiNodePtr		s_pPartObj;
		std::string		s_FileName;
		UINT			s_nFileTableID;
		UINT			s_nRemainTime;
	};
private:
	CsCriticalSection					s_CS_UpdateReadyPart;
	CThreadSafeQueue< sLOAD_PART* >		m_queueReadyPart;
	std::map<int, sParts>				m_Parts;
};

