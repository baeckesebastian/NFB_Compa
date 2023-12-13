#include "Trigger.h"

typedef short	(__stdcall *lpInp32)(short);
typedef BOOL	(__stdcall *lpIsInpOutDriverOpen)(void);

lpInp32 gfpInp32;
lpIsInpOutDriverOpen gfpIsInpOutDriverOpen;

HINSTANCE hInpOutDll;
		
Trigger::Trigger()
{
}

Trigger::~Trigger()
{
} 

bool Trigger::WaitForTrigger(uint address)
{
	hInpOutDll = LoadLibrary((LPCWSTR)L"inpoutx64.dll");

	if (hInpOutDll != NULL)
	{
		gfpInp32 = (lpInp32)GetProcAddress(hInpOutDll, "Inp32");
		gfpIsInpOutDriverOpen = (lpIsInpOutDriverOpen)GetProcAddress(hInpOutDll, "IsInpOutDriverOpen");

		if (gfpIsInpOutDriverOpen())
		{
			//unsigned short iPort = 49169;
			unsigned short iPort = address;
			WORD initialInput = gfpInp32(iPort);	// read the port
			WORD wData = initialInput;

			while (wData == initialInput)
			{
				wData = gfpInp32(iPort);
			}
			FreeLibrary(hInpOutDll);
			return true;
		}
		else
		{
			return false;
		}
	}
	else
	{
		return false;
	}

}
	