#include "Memory.h"

#include <TlHelp32.h>

#ifdef WIN_MEM
#pragma region CDispatcher
CDispatcher *CDispatcher::m_pDispatcher = nullptr;

CDispatcher::CDispatcher() :
	m_iProcessId(-1),
	m_hProcess(INVALID_HANDLE_VALUE) { }
CDispatcher::~CDispatcher()
{
	if (m_hProcess != INVALID_HANDLE_VALUE)
		CloseHandle(m_hProcess);
}

// [Private]:
int CDispatcher::GetProcessId(std::string_view sProcessName)
{
	PROCESSENTRY32 pEntry32;

	memset(&pEntry32, 0, sizeof(PROCESSENTRY32));
	pEntry32.dwSize = sizeof(pEntry32);

	HANDLE hProcHandle = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

	if (hProcHandle != INVALID_HANDLE_VALUE || Process32First(hProcHandle, &pEntry32))
	{
		do
		{
			if (sProcessName == pEntry32.szExeFile)
			{
				CloseHandle(hProcHandle);
				return pEntry32.th32ProcessID;
			}

		} while (Process32Next(hProcHandle, &pEntry32));

		CloseHandle(hProcHandle);
	}

	return -1;
}

// [Public]:
CDispatcher* CDispatcher::Get()
{
	if (!m_pDispatcher)
		m_pDispatcher = new CDispatcher();

	return m_pDispatcher;
}

void CDispatcher::Attach(std::string_view sProcessName)
{
	int iProcessId = GetProcessId(sProcessName);

	if (iProcessId != -1)
	{
		if (m_hProcess)
			CloseHandle(m_hProcess);

		m_sProcessName = sProcessName;
		m_iProcessId = iProcessId;
		m_hProcess = OpenProcess(PROCESS_ALL_ACCESS, false, m_iProcessId);
	}
}

std::string_view CDispatcher::GetProcessName()
{
	return m_sProcessName;
}

unsigned __int64 CDispatcher::GetModuleBase(std::string_view sModule)
{
	HANDLE hProcHandle = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, m_iProcessId);

	if (hProcHandle != INVALID_HANDLE_VALUE)
	{
		MODULEENTRY32 mEntry;
		mEntry.dwSize = sizeof(MODULEENTRY32);

		if (Module32First(hProcHandle, &mEntry))
		{
			do
			{
				if (mEntry.szModule == sModule)
				{
					CloseHandle(hProcHandle);
					return (unsigned __int64)mEntry.modBaseAddr;
				}
			} while (Module32Next(hProcHandle, &mEntry));
		}

		CloseHandle(hProcHandle);
	}

	return 0x0;
}


bool CDispatcher::WriteProtection(unsigned __int64 uAddress, unsigned int uSize, int iProtection)
{
	SProtectionRecord pRecord{ 0, uAddress, uSize };

	if (VirtualProtectEx(m_hProcess, (void*)pRecord.m_iProtectionBase, pRecord.m_uSize, iProtection, (unsigned long*)&pRecord.m_iOldProtection))
	{
		m_vProtectionHistory.push_back(pRecord);
		return true;
	}

	return false;
}
void CDispatcher::RestoreProtection(unsigned __int64 uAddress)
{
	for (int i = 0; i < m_vProtectionHistory.size(); i++)
	{
		SProtectionRecord pRecord = m_vProtectionHistory[i];

		if (pRecord.m_iProtectionBase == uAddress)
		{
			int iOldProtection;
			VirtualProtectEx(m_hProcess, (void*)pRecord.m_iProtectionBase, pRecord.m_uSize, pRecord.m_iOldProtection, (unsigned long*)&iOldProtection);

			m_vProtectionHistory.erase(m_vProtectionHistory.begin() + i);
		}
	}
}
#pragma endregion
#endif