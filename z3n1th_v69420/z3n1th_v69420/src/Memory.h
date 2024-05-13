#pragma once

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include <string>
#include <vector>
#include <string_view>

#define WIN_MEM

#ifdef WIN_MEM
class CDispatcher
{
private:
	static CDispatcher *m_pDispatcher;
	CDispatcher();

	struct SProtectionRecord
	{
		int m_iOldProtection;
		unsigned __int64 m_iProtectionBase;
		unsigned int m_uSize;
	};

	std::string_view m_sProcessName;
	int m_iProcessId;
	void *m_hProcess;

	std::vector<SProtectionRecord> m_vProtectionHistory;

	int GetProcessId(std::string_view sProcessName);

public:
	~CDispatcher();

	static CDispatcher* Get();

	void Attach(std::string_view sProcessName);

	std::string_view GetProcessName();

	unsigned __int64 GetModuleBase(std::string_view sModule);

	template <class T>
	bool ReadMemory(unsigned __int64 uAddress, T &tObject)
	{
		return ReadProcessMemory(m_hProcess, (void*)uAddress, (void*)&tObject, sizeof(T), nullptr);
	}
	template <class T>
	T ReadMemory(unsigned __int64 uAddress)
	{
		T tObject{};
		ReadMemory<T>(uAddress, tObject);

		return tObject;
	}
	template <class T>
	T* ReadArray(unsigned __int64 uAddress, unsigned int uCount)
	{
		T *pObjects = new T[uCount];
		ReadProcessMemory(m_hProcess, (void*)uAddress, pObjects, sizeof(T) * uCount, nullptr);

		return pObjects;
	}
	template <class T>
	void ReadArray(unsigned __int64 uAddress, T *pObject, unsigned int uCount)
	{
		ReadProcessMemory(m_hProcess, (void*)uAddress, pObject, sizeof(T) * uCount, nullptr);
	}

	template <class T>
	bool WriteMemory(unsigned __int64 uAddress, T tObject)
	{
		return WriteProcessMemory(m_hProcess, (void*)uAddress, (void*)&tObject, sizeof(T), nullptr);
	}
	template <class T>
	bool WriteArray(unsigned __int64 uAddress, T *pObject, unsigned int uCount)
	{
		return WriteProcessMemory(m_hProcess, (void*)uAddress, (void*)pObject, sizeof(T) * uCount, nullptr);
	}

	bool WriteProtection(unsigned __int64 uAddress, unsigned int uSize, int iProtection);
	void RestoreProtection(unsigned __int64 uAddress);

	bool ProtectThread(bool bProtect, unsigned int uThreadId = 0xFFFFFFFF)
	{
		return true;
	}
};
#endif

template <class T>
class CPointer
{
private:
	unsigned __int64 m_uBase;
	bool m_bSuccess;

public:
	CPointer() :
		m_uBase(0x0) { }
	CPointer(unsigned __int64 uBase) :
		m_bSuccess(false),
		m_uBase(uBase) { }

	bool operator==(CPointer<T> pRight)
	{
		return pRight.GetBase() == m_uBase;
	}
	T operator()()
	{
		T tTemp{};
		m_bSuccess = CDispatcher::Get()->ReadMemory<T>(m_uBase, tTemp);

		return tTemp;
	}

	template <class T>
	T Cast()
	{
		return CDispatcher::Get()->ReadMemory<T>(m_uBase);
	}

	template <class T>
	T Read(__int64 iOffset = 0x0)
	{
		return CDispatcher::Get()->ReadMemory<T>(m_uBase + iOffset);
	}
	template <class T>
	void Read(T *pReturn, int iCount, __int64 iOffset = 0x0)
	{
		CDispatcher::Get()->ReadMemory<T>(m_uBase + iOffset, iCount, pReturn);
	}

	template <class T>
	void Write(T tData, __int64 iOffset = 0x0)
	{
		CDispatcher::Get()->WriteMemory<T>(m_uBase + iOffset, tData);
	}

	unsigned __int64 GetBase()
	{
		return m_uBase;
	}
	void SetBase(unsigned __int64 uBase)
	{
		m_uBase = uBase;
	}

	bool Succeeded()
	{
		return m_bSuccess;
	}
};

template <class T>
class CCached
{
private:
	CPointer<T> m_pObject;
	T m_tObject;

public:
	CCached() { }
	CCached(unsigned __int64 uBase) :
		m_pObject(uBase)
	{
		Update();
	}

	T& operator()()
	{
		return m_tObject;
	}
	template <class T>
	T Cast()
	{
		return *(T*)&m_tObject;
	}

	void Update()
	{
		m_tObject = m_pObject();
	}
	bool Update(unsigned __int64 uBase)
	{
		m_pObject.SetBase(uBase);
		m_tObject = m_pObject();

		return uBase && m_pObject.Succeeded();
	}

	unsigned __int64 GetBase()
	{
		return m_pObject.GetBase();
	}
	void SetBase(unsigned __int64 uBase)
	{
		if (uBase)
			m_pObject.SetBase(uBase);
	}

	CPointer<T> Pointer()
	{
		return m_pObject;
	}
};
