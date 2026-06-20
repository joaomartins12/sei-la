#include "../LibProj/CsFilePack/stdafx.h"
#include "../LibProj/CsFilePack/CsFilePackSystem.h"
#include "../LibProj/CsFilePack/FPHeader.h"

int main(int argc, char** argv)
{
	CsFPS::CsFPSystem h;

	std::list<std::string> r;
	r.push_back(argv[1]);

	if (argc > 2)
	{

		if (argv[2][0] == 'C')
		{
			if (!CsFPS::CsFPSystem::Initialize(true, r, true))
			{
				printf("csfpsystem init fail\n");
				return 1;
			}

			//if (!CsFPS::CsFPSystem::AddFile)

		}
		else if (argv[2][0] == 'H')
		{

			printf("%u\n", CsFPS::GetHashCode(argv[1]));
			return 0;
		}
	}
	else
	{

		if (!CsFPS::CsFPSystem::Initialize(true, r, false))
		{
			printf("csfpsystem init fail\n");
			return 1;
		}

		if (!CsFPS::CsFPSystem::UnPacking())
		{
			printf("unpack fail\n");
			return 1;
		}
	}

	printf("ok\n");

	return 0;
}
