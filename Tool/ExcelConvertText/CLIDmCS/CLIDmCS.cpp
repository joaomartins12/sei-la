// 기본 DLL 파일입니다.

#include "stdafx.h"
#include <msclr\marshal_cppstd.h>
#include "CLIDmCS.h"
namespace CLIDmCS {

	Compress::Compress()
	{

	}

	Compress::~Compress()
	{

	}	

	array<System::Byte>^ Compress::CompressData(List<String^>^ datalist)
	{	
		// System String -> std::wstring
		dm_stringlist_t readData;
		for (INT32 n = 0; n < datalist->Count; ++n)
			readData.push_back(msclr::interop::marshal_as<std::wstring>(datalist[n]));
		
		dm_bytevec data;
 		dm_size_t size = 0;
 		data.resize(DmCS::Measure::exec(readData));
 		if (!DmCS::Write::exec(readData, data, size))
 			return nullptr;
 
 		if (!DmCS::CompressZip::Compress(data, data))
			return nullptr;

		if (!DmFS::SecuritySelf::Encript(DmFS::Enc::Kind::All, data, data))
			return nullptr;

		array<System::Byte>^ byResult = gcnew array<System::Byte>(data.size());
		for (size_t n = 0; n < data.size(); ++n)
			byResult[n] = data[n];
		return byResult;
	}


	List<String^>^ Compress::DeCompressData(array<System::Byte>^ decomp)
	{
		if (nullptr == decomp || 0 == decomp->Length)
			return nullptr;

		dm_bytevec binary_data(decomp->Length);
		for (INT32 n = 0; n < decomp->Length; ++n)
			binary_data[n] = decomp[n];

		// 암호 해제
		if (!DmFS::SecuritySelf::Decript(DmFS::Enc::Kind::All, binary_data, binary_data))
			return nullptr;

		// 압축 해제
		if (!DmCS::CompressZip::UnCompress(binary_data, binary_data))
			return nullptr;

		dm_stringlist_t read_Data;
		dm_size_t size = 0;
		if (!DmCS::Read::exec(read_Data, binary_data, size))
			return nullptr;

		List<String^>^ newData = gcnew List<String ^>();
		for (auto it : read_Data)
			newData->Add(gcnew String(it.c_str()));
		
		return newData;
	}

}