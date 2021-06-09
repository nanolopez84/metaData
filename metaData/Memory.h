#pragma once

#include "Console.h"

#include <memory>
#include <map>

extern Console g_console;

class Memory
{
protected:
    void attachProcess();
    void checkHash();
    void getBaseAddress();
    void getProcessByName();

public:
    uint64_t m_baseAddress;
    std::wstring m_imageName;
    DWORD m_processPID;
    HANDLE m_processWindowHandle;
    std::wstring m_targetProcessName;

    Memory(const std::wstring& targetProcessName);
    virtual ~Memory();
};

typedef std::unique_ptr<Memory> (__stdcall *CreateMemoryFn)(const std::wstring& targetProcessName);

class NinjaMemory : public Memory
{
public:
    NinjaMemory(const std::wstring& targetProcessName);
    virtual ~NinjaMemory() {}
    static std::unique_ptr<Memory> __stdcall Create(const std::wstring& targetProcessName);
};

class MemoryFactory
{
private:
    MemoryFactory();

    typedef std::map<const std::wstring, CreateMemoryFn> FactoryMap;
    FactoryMap m_FactoryMap;
public:
    static MemoryFactory& Get();

    void registerConstructor(const std::wstring& targetProcessName, CreateMemoryFn pfnCreate);
    std::unique_ptr<Memory> createMemory(const std::wstring& targetProcessName);
};