#pragma once

#include "Console.h"

#include <memory>
#include <map>

#include "MultilevelPointer.h"

class Memory
{
protected:
    uint64_t        m_baseAddress;
    std::wstring    m_imageName;
    HANDLE          m_processHandle;
    DWORD           m_processPID;
    std::wstring    m_targetProcessName;

    void attachProcess();
    void checkHash();
    void getBaseAddress();
    void getProcessByName();

public:
    Memory(const std::wstring& targetProcessName);
    virtual ~Memory();
    virtual void update(WPARAM vkCode) = 0;
};

typedef std::unique_ptr<Memory> (__stdcall *CreateMemoryFn)(const std::wstring& targetProcessName);

class NinjaMemory : public Memory
{
protected:
    std::unique_ptr<MultilevelPointer> m_mpInstantCast;

public:
    static std::unique_ptr<Memory> __stdcall Create(const std::wstring& targetProcessName);

    NinjaMemory(const std::wstring& targetProcessName);
    virtual ~NinjaMemory() {}
    void            instantCast(bool flag);
    virtual void    update(WPARAM vkCode);
};

class MemoryFactory
{
private:
    MemoryFactory();

    std::map<const std::wstring, CreateMemoryFn> m_FactoryMap;
public:
    static MemoryFactory& Get();

    std::unique_ptr<Memory> createMemory(const std::wstring& targetProcessName);
    void                    registerConstructor(const std::wstring& targetProcessName, CreateMemoryFn pfnCreate);
};