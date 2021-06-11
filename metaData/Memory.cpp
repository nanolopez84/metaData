#include "Memory.h"

#include <tlhelp32.h>
#include <psapi.h>

#include <string>
#include <fstream>
#include <filesystem>

#include "State.h"
#include "metaData.h"

extern Console g_console;
extern Context g_state;

Memory::Memory(const std::wstring& targetProcessName)
    : m_processHandle(NULL), m_targetProcessName(targetProcessName), m_processPID(0),
    m_imageName(L""), m_baseAddress(0)
{
    this->attachProcess();
    this->loadKeyMapping();
}

Memory::~Memory()
{
    if (m_processHandle)
    {
        CloseHandle(m_processHandle);
    }
}

void Memory::attachProcess()
{
    this->getProcessByName();
    this->checkHash();
    this->getBaseAddress();
}

void Memory::checkHash()
{
    std::map<std::wstring, unsigned long long> hashes = {
        { L"ninja", 10504905621138366064 }
    };

    LPWSTR imageName = new WCHAR[2048];
    DWORD imageNameSize = 2048;
    QueryFullProcessImageNameW(m_processHandle, 0, imageName, &imageNameSize);

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

    if (!EnumProcessModulesEx(m_processHandle, lphModule, sizeof(lphModule), &lpcbNeeded, LIST_MODULES_ALL))
    {
        throw std::string("List modules ERROR");
    }

    int moduleCount = lpcbNeeded / sizeof(long);
    LPWSTR moduleFileName = new WCHAR[1024];
    int moduleFileNameSize;
    std::wstring moduleFileNameStr;
    for (int i = 0; i < moduleCount; i++)
    {
        moduleFileNameSize = GetModuleFileNameEx(m_processHandle, lphModule[i], moduleFileName, 1024);
        moduleFileNameStr = std::wstring(moduleFileName);
        if (m_imageName.compare(moduleFileNameStr) == 0)
        {
            m_baseAddress = (uint64_t)lphModule[i];
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
                m_processHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, entry.th32ProcessID);
                m_processPID = entry.th32ProcessID;
                break;
            }
        }
    }

    CloseHandle(snapshot);

    if (!m_processHandle)
    {
        throw std::string("Error attaching to process");
    }
}

void Memory::loadKeyMapping()
{
    std::ifstream fin(KEY_MAP_FILE_NAME);
    if (fin.bad())
    {
        g_state.setState(Context::STATES::ERR);

        std::stringstream ss;
        ss << "Can not open key mapping file: " << KEY_MAP_FILE_NAME;
        g_state.setErrorMessage(ss.str());
        return;
    }

    std::map<std::wstring, std::string> mapPrefix{
        {L"ninja", "NINJA"}
    };

    std::string prefix = mapPrefix[m_targetProcessName];
    std::string line;
    std::string action;
    std::string vkCode;
    while (std::getline(fin, line))
    {
        std::stringstream ss(line);
        if (ss >> action >> vkCode)
        {
            if (action.find(prefix) != std::string::npos)
            {
                m_mapKey[toupper(vkCode[0])] = action;
            }
        }
    }
}

void Memory::update(WPARAM vkCode)
{
    auto itKey = m_mapKey.find(vkCode);
    if (itKey != m_mapKey.end())
    {
        auto itMethod = m_mapMethod.find(itKey->second);
        if (itMethod != m_mapMethod.end())
        {
            itMethod->second();
        }
        else
        {
            std::stringstream ss;
            ss << "ERROR: " << itKey->second << " not found";
            g_console.append(ss.str());
        }
    }
}

std::unique_ptr<Memory> NinjaMemory::Create(const std::wstring& targetProcessName)
{
    return std::make_unique<NinjaMemory>(targetProcessName);
}

NinjaMemory::NinjaMemory(const std::wstring& targetProcessName)
    : Memory(targetProcessName)
{
    m_mpInstantCast = std::make_unique<MultilevelPointer>(m_processHandle, m_baseAddress + 0x2A47E8);

    m_mapMethod["NINJA_INSTANT_CAST_OFF"]   = std::bind(&NinjaMemory::instantCastOff, this);
    m_mapMethod["NINJA_INSTANT_CAST_ON"]    = std::bind(&NinjaMemory::instantCastOn, this);
}

void NinjaMemory::instantCastOff()
{
    std::vector<uint8_t> buffer { 0xF3, 0x0F, 0x5C, 0xC7, 0x0F, 0x2F, 0xF0, 0xF3, 0x0F, 0x11, 0x43, 0x08, 0x72, 0x24 };
    g_console.printResult(m_mpInstantCast->setBytes(buffer), "NINJA_INSTANT_CAST_OFF");
}

void NinjaMemory::instantCastOn()
{
    std::vector<uint8_t> buffer { 0xC7, 0x43, 0x08, 0x00, 0x00, 0x00, 0x00, 0xEB, 0x29, 0x90, 0x90, 0x90, 0x90, 0x90 };
    g_console.printResult(m_mpInstantCast->setBytes(buffer), "NINJA_INSTANT_CAST_ON");
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

std::unique_ptr<Memory> MemoryFactory::createMemory(const std::wstring& targetProcessName)
{
    auto it = m_FactoryMap.find(targetProcessName);
    if (it != m_FactoryMap.end())
    {
        try
        {
            return it->second(targetProcessName);
        }
        catch (std::string& msg)
        {
            g_state.setState(Context::STATES::ERR);
            g_state.setErrorMessage(msg);
        }
    }
    else
    {
        g_state.setState(Context::STATES::ERR);
        g_state.setErrorMessage("Option not supported");
    }
    return nullptr;
}

void MemoryFactory::registerConstructor(const std::wstring& targetProcessName, CreateMemoryFn pfnCreate)
{
   m_FactoryMap[targetProcessName] = pfnCreate;
}