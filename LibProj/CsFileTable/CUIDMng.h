
#pragma once 



class CsCuidMng
{
public:
	enum			{	MESSAGE_COPY_SIZE = 2048 };

	void			Delete();
	bool			Init( char* cPath );

protected:
	void			_LoadExcel_ID( char* cSheetName );
	void			_LoadExcel_Chat( char* cSheetName );

	bool			_LoadBin( char* cPath );
	void			_LoadFilePack( char* cPath );

public:
	bool			CheckID( TCHAR const* sName );
	void			CheckChat( TCHAR* const szOut );

public:
	void			SaveBin( char* cPath );	

public:
	static bool		SortingByLength(CsCuid_Chat* const leftWord, CsCuid_Chat* const rightWord );

	// [3/10/2016 hyun] 금지어 필터 수정 관련 함수 추가
private:
	std::wstring	MakeReplaceStr(int	iLength);
	void			GetBadWordPos(std::wstring const& InputString, std::vector<std::pair<int,int>>& rVecFindPos);
	void			AddChatList(CsCuid_Chat::sINFO& rInfo);

	void			Lwr(TCHAR* const  pText);
	void			Trim(TCHAR*  const  pText, const std::wstring& removeStr );
	// @@ 여기까지


protected:
	CsCuid::LIST		m_List;
	CsCuid_Chat::LIST	m_ChatList;
	TCHAR				m_CheckStr[ 2048 ];
};