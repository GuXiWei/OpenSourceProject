#include "StdAfx.h"
#include "SerialNumberMac.h"

static BOOL g_bSecuritySInitialized = FALSE;
SerialNumberMac::SerialNumberMac(void)
{
	memset(m_SerialNumStr,0,sizeof(wchar_t)*MAX_PATH);
}


SerialNumberMac::~SerialNumberMac(void)
{
}
HRESULT SerialNumberMac::InitializeSecrity()
{
	HRESULT hres =  CoInitializeEx(0, COINIT_MULTITHREADED);
	hres = CoInitializeSecurity(
		NULL,
		-1,      // COM negotiates service                  
		NULL,    // Authentication services
		NULL,    // Reserved
		RPC_C_AUTHN_LEVEL_DEFAULT,    // authentication
		RPC_C_IMP_LEVEL_IMPERSONATE,  // Impersonation
		NULL,             // Authentication info 
		EOAC_NONE,        // Additional capabilities
		NULL              // Reserved
		);

	if (FAILED(hres) && RPC_E_TOO_LATE != hres)
	{
		return E_FAIL;
	}

	return S_OK;
}

HRESULT SerialNumberMac::QueryValue(IWbemServices *pSvc, wchar_t* strQuery,wchar_t* strKey)
{
	HRESULT hres;
	IEnumWbemClassObject* pEnumerator = NULL;
	hres = pSvc->ExecQuery(
		bstr_t("WQL"),
		bstr_t(strQuery),
		WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY,
		NULL,
		&pEnumerator);

	if (FAILED(hres))
	{
		return hres;              // Program has failed.
	}
	else
	{
		IWbemClassObject *pclsObj;
		ULONG uReturn = 0;

		while (pEnumerator)
		{
			hres = pEnumerator->Next(WBEM_INFINITE, 1,
				&pclsObj, &uReturn);

			if (0 == uReturn)
			{
				break;
			}

			VARIANT vtProp;

			// Get the value of the Name property
			hres = pclsObj->Get(strKey, 0, &vtProp, 0, 0);

			if (hres == S_OK && vtProp.vt == VT_BSTR)
			{
				wcscpy(m_SerialNumStr, vtProp.bstrVal);
			}

			VariantClear(&vtProp);

			if (pclsObj)
			{
				pclsObj->Release();
			}
		}
	}
	return S_OK;
}
//获取硬盘序列号
wchar_t* SerialNumberMac::GetSerialNumber()
{
	HRESULT hres = S_OK;
	// Initialize 

	if (!g_bSecuritySInitialized)
	{
		hres = InitializeSecrity();
		if (SUCCEEDED(hres))
		{
			g_bSecuritySInitialized = TRUE;
		}
	}
	if (FAILED(hres))
	{
		return L"";          // Program has failed.
	}
	IWbemLocator *pLoc = 0;
	hres = CoCreateInstance(
		CLSID_WbemLocator,
		0,
		CLSCTX_INPROC_SERVER,
		IID_IWbemLocator, (LPVOID *)&pLoc);

	if (FAILED(hres))
	{
		return L"";      // Program has failed.
	}

	IWbemServices *pSvc = 0;
	hres = pLoc->ConnectServer(
		_bstr_t(L"ROOT\\CIMV2"), // WMI namespace
		NULL,                    // User name
		NULL,                    // User password
		0,                       // Locale
		NULL,                    // Security flags                 
		0,                       // Authority       
		0,                       // Context object
		&pSvc                    // IWbemServices proxy
		);

	if (FAILED(hres))
	{
		pLoc->Release();
		return L"";
	}
	hres = CoSetProxyBlanket(

		pSvc,                         // the proxy to set
		RPC_C_AUTHN_WINNT,            // authentication service
		RPC_C_AUTHZ_NONE,             // authorization service
		NULL,                         // Server principal name
		RPC_C_AUTHN_LEVEL_CALL,       // authentication level
		RPC_C_IMP_LEVEL_IMPERSONATE,  // impersonation level
		NULL,                         // client identity 
		EOAC_NONE                     // proxy capabilities     
		);

	if (FAILED(hres))
	{
		pSvc->Release();
		pLoc->Release();
		return L"";           // Program has failed.
	}
	hres = QueryValue(pSvc, _T("SELECT * FROM Win32_DiskDrive"), _T("SerialNumber"));
	pSvc->Release();
	pLoc->Release();

	return m_SerialNumStr;
}
//获取MAC
char* SerialNumberMac::GetMac()
{
    ASTAT Adapter;
    NCB Ncb;
    UCHAR uRetCode;
    LANA_ENUM lenum;
    int i = 0;
 
    memset(&Ncb, 0, sizeof(Ncb));
    Ncb.ncb_command = NCBENUM;
    Ncb.ncb_buffer = (UCHAR *)&lenum;
    Ncb.ncb_length = sizeof(lenum);
 
    uRetCode = Netbios( &Ncb );
    for(i=0; i < lenum.length ; i++)
    {
        memset(&Ncb, 0, sizeof(Ncb));
        Ncb.ncb_command = NCBRESET;
        Ncb.ncb_lana_num = lenum.lana[i];
        uRetCode = Netbios( &Ncb );
 
        memset(&Ncb, 0, sizeof(Ncb));
        Ncb.ncb_command = NCBASTAT;
        Ncb.ncb_lana_num = lenum.lana[i];
        strcpy((char *)Ncb.ncb_callname, "* ");
        Ncb.ncb_buffer = (unsigned char *) &Adapter;
        Ncb.ncb_length = sizeof(Adapter);
        uRetCode = Netbios( &Ncb );
 
        if (uRetCode == 0)
        {
			sprintf(m_MacStr, "%02X-%02X-%02X-%02X-%02X-%02X ",
				Adapter.adapt.adapter_address[0],
				Adapter.adapt.adapter_address[1],
				Adapter.adapt.adapter_address[2],
				Adapter.adapt.adapter_address[3],
				Adapter.adapt.adapter_address[4],
				Adapter.adapt.adapter_address[5]
			);
			return m_MacStr;
		}
		else
			return "";
    }
}
