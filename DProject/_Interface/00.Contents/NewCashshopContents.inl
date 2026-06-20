
inline const DWORD NewCashshopContents::GetTotalCash(void) const
{
	return mCashBalance.GetTotalCash();
}

inline const DWORD NewCashshopContents::GetRealCash(void) const
{
	return mCashBalance.GetRealCash();
}

inline const DWORD NewCashshopContents::GetBonusCash(void) const
{
	return mCashBalance.GetBonusCash();
}

inline NewCashshopContents::PurchaseItems const& NewCashshopContents::GetPurchaseItems() const
{
	return mPurchaseItems.GetList();
}

inline NewCashshopContents::Baskets const& NewCashshopContents::GetBasketsItemAll(void) const
{
	return mBaskets;
}

inline int NewCashshopContents::GetHistoryIdx() const
{
	return m_nHistoryIdx;
}

inline NewCashshopContents::CashStorageItems const& NewCashshopContents::GetCashStorage(void) const
{
	return mCashStorageItems.GetCashWarehouseInfo();
}

inline cItemInfo* NewCashshopContents::GetCashStorage(int const& nSlotNum)
{
	return mCashStorageItems.GetItemInfo( nSlotNum );
}

inline NewCashshopContents::Equipments const& NewCashshopContents::GetEquipments_Origin(void) const
{
	return OriginEquipments;
}

inline NewCashshopContents::Equipments const& NewCashshopContents::GetEquipments_Changed(void) const
{
	return ChangedEquip;
}