#include "Memory.h"

#include <tlhelp32.h>
#include <psapi.h>

#include <string>
#include <fstream>
#include <filesystem>

#include "State.h"

extern Context g_state;

Memory::Memory(const std::wstring& targetProcessName)
    : m_processWindowHandle(NULL), m_targetProcessName(targetProcessName), m_processPID(0),
    m_imageName(L""), m_baseAddress(0)
{
    this->attachProcess();
}

Memory::~Memory()
{
    if (m_processWindowHandle)
    {
        CloseHandle(m_processWindowHandle);
    }
}

void Memory::attachProcess()
{
    try
    {
        this->getProcessByName();
        this->checkHash();
        this->getBaseAddress();
    }
    catch (std::string& msg)
    {
        g_state.setState(Context::STATES::ERR);
        g_state.setErrorMessage(msg);
    }
}

void Memory::checkHash()
{
    std::map<std::wstring, unsigned long long> hashes = {
        { L"ninja", 10504905621138366064 }
    };

    LPWSTR imageName = new WCHAR[2048];
    DWORD imageNameSize = 2048;
    QueryFullProcessImageNameW(m_processWindowHandle, 0, imageName, &imageNameSize);

    std::filesystem::path p{imageName};
    uintmax_t size = std::filesystem::file_size(p);

    std::string buffer;
    buffer.resize(static_cast<const unsigned int>(size));

    std::ifstream input(imageName, std::ios::binary);
    input.read(&buffer[0], size);

    m_imageName = std::wstring(imageName);
    delete[] imageName;

    size_t hash = std::hash<std::string>{}(buffer);

    if (hash != hashes[m_targetProcessName])
    {
        throw std::string("Executable version not supported");
    }
}

void Memory::getBaseAddress()
{
    HMODULE lphModule[1024];
    DWORD lpcbNeeded = 0;

    if (!EnumProcessModulesEx(m_processWindowHandle, lphModule, sizeof(lphModule), &lpcbNeeded, LIST_MODULES_ALL))
    {
        throw std::string("List modules ERROR");
    }

    int moduleCount = lpcbNeeded / sizeof(long);
    LPWSTR moduleFileName = new WCHAR[1024];
    int moduleFileNameSize;
    std::wstring moduleFileNameStr;
    for (int i = 0; i < moduleCount; i++)
    {
        moduleFileNameSize = GetModuleFileNameEx(m_processWindowHandle, lphModule[i], moduleFileName, 1024);
        moduleFileNameStr = std::wstring(moduleFileName);
        if (m_imageName.compare(moduleFileNameStr) == 0)
        {
            m_baseAddress = (uint64_t) lphModule[i];
            break;
        }
    }

    if (m_baseAddress == 0)
    {
        throw std::string("Base address not found");
    }
}

void Memory::getProcessByName()
{
    std::map<std::wstring, std::wstring> execNames = {
        { L"ninja", L"Ninja.exe" }
    };

    std::wstring targetExecName = execNames[m_targetProcessName];

    if (targetExecName.length() == 0)
    {
        throw std::string("Error getting process name");
    }

    PROCESSENTRY32 entry;
    entry.dwSize = sizeof(PROCESSENTRY32);
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);

    if (Process32First(snapshot, &entry) == TRUE)
    {
        while (Process32Next(snapshot, &entry) == TRUE)
        {
            if (targetExecName.compare(entry.szExeFile) == 0)
            {
                m_processWindowHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, entry.th32ProcessID);
                m_processPID = entry.th32ProcessID;
                break;
            }
        }
    }

    CloseHandle(snapshot);

    if (!m_processWindowHandle)
    {
        throw std::string("Error attaching to process");
    }
}

NinjaMemory::NinjaMemory(const std::wstring& targetProcessName)
    : Memory(targetProcessName)
{
}

std::unique_ptr<Memory> NinjaMemory::Create(const std::wstring& targetProcessName)
{
    return std::make_unique<NinjaMemory>(targetProcessName);
}

MemoryFactory::MemoryFactory()
{
    registerConstructor(L"ninja", &NinjaMemory::Create);
}

MemoryFactory& MemoryFactory::Get()
{
    static MemoryFactory instance;
    return instance;
}

void MemoryFactory::registerConstructor(const std::wstring& targetProcessName, CreateMemoryFn pfnCreate)
{
   m_FactoryMap[targetProcessName] = pfnCreate;
}

std::unique_ptr<Memory> MemoryFactory::createMemory(const std::wstring& targetProcessName)
{
    FactoryMap::iterator it = m_FactoryMap.find(targetProcessName);
    if (it != m_FactoryMap.end())
    {
        return it->second(targetProcessName);
    }
    return nullptr;
}
