#pragma once

#include "Console.h"

#include <memory>
#include <map>

extern Console g_console;

class Memory
{
public:
	static std::unique_ptr<Memory> AttachProcess(LPWSTR targetProcessName);
    virtual ~Memory() {}
};

typedef std::unique_ptr<Memory> (__stdcall *CreateMemoryFn)(void);

class NinjaMemory : public Memory
{
public:
    virtual ~NinjaMemory() {}
    static std::unique_ptr<Memory> __stdcall Create();
};

class MemoryFactory
{
private:
    MemoryFactory();

    typedef std::map<const std::wstring, CreateMemoryFn> FactoryMap;
    FactoryMap m_FactoryMap;
public:
    static MemoryFactory& Get();

    void Register(const std::wstring& targetProcessName, CreateMemoryFn pfnCreate);
    std::unique_ptr<Memory> CreateMemory(const std::wstring& targetProcessName);
};