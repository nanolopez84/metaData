#include "Memory.h"

#include <string>

std::unique_ptr<Memory> Memory::AttachProcess(LPWSTR targetProcessName)
{
	std::unique_ptr<Memory> mem = MemoryFactory::Get().CreateMemory(std::wstring(targetProcessName));
    return mem;
}

std::unique_ptr<Memory> NinjaMemory::Create()
{
    return std::make_unique<NinjaMemory>();
}

MemoryFactory::MemoryFactory()
{
    Register(L"ninja", &NinjaMemory::Create);
}

MemoryFactory& MemoryFactory::Get()
{
    static MemoryFactory instance;
    return instance;
}

void MemoryFactory::Register(const std::wstring& targetProcessName, CreateMemoryFn pfnCreate)
{
   m_FactoryMap[targetProcessName] = pfnCreate;
}

std::unique_ptr<Memory> MemoryFactory::CreateMemory(const std::wstring& targetProcessName)
{
    FactoryMap::iterator it = m_FactoryMap.find(targetProcessName);
    if (it != m_FactoryMap.end())
    {
        return it->second();
    }
    return nullptr;
}
