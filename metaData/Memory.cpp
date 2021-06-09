#include "Memory.h"

#include <tlhelp32.h>

#include <string>

#include "State.h"

extern Context g_state;

Memory::Memory(const std::wstring& targetProcessName)
    : m_processWindowHandle(NULL), m_targetProcessName(targetProcessName), m_processPID(0)
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
    this->getProcessByName();

    if (!m_processWindowHandle)
    {
        g_state.setState(Context::STATES::ERR);
        g_state.setErrorMessage("Error attaching to process");
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
        g_state.setState(Context::STATES::ERR);
        g_state.setErrorMessage("Error getting process name");
        return;
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
            }
        }
    }

    CloseHandle(snapshot);
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
