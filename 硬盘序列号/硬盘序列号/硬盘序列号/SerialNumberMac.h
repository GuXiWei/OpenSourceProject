#pragma once
#include <nb30.h>
#include <Wbemidl.h>
#include <comdef.h>
#include <strsafe.h>

#pragma comment(lib, "wbemuuid.lib")
#pragma comment(lib,"netapi32.lib")
typedef struct _ASTAT
{
	ADAPTER_STATUS   adapt;
	NAME_BUFFER   NameBuff[30];
}ASTAT,*PASTAT;
class SerialNumberMac
{
public:
	SerialNumberMac(void);
	~SerialNumberMac(void);
private:
	HRESULT InitializeSecrity();
	HRESULT QueryValue(IWbemServices *pSvc, wchar_t* strQuery,wchar_t* strKey);
public:
	wchar_t m_SerialNumStr[MAX_PATH];
	char	m_MacStr[MAX_PATH];
public:
	wchar_t* GetSerialNumber();
	char* GetMac();
};

