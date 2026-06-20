// CLRDmCS.h

#pragma once

#include "Dm-PCH.h"

using namespace System;
using namespace System::Collections::Generic;

namespace CLIDmCS {

	public ref class Compress
	{
	public:
		Compress();
		virtual ~Compress();

	public:
		array<System::Byte>^ CompressData(List<String^>^ datalist);
		List<String^>^ DeCompressData(array<System::Byte>^ decomp);
	};
}
