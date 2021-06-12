#pragma once

#include "Console.h"

#include <memory>
#include <map>
#include <functional>

#include "MultilevelPointer.h"

typedef std::map<WPARAM, std::string>                       MAP_KEY;
typedef std::map<std::string, std::function<void(void)>>    MAP_METHOD;
typedef std::map<std::wstring, std::vector<std::string>>    MEM_CONFIG;

class Memory
{
protected:
    enum CONIFG
    {
        HASH = 0,
        EXEC_NAME,
        PREFIX,
        SPLASH
    };

    uint64_t            m_baseAddress;
    MEM_CONFIG          m_configuration;
    std::wstring        m_imageName;
    MAP_KEY             m_mapKey;
    MAP_METHOD          m_mapMethod;
    HANDLE              m_processHandle;
    DWORD               m_processPID;
    std::wstring        m_targetProcessName;

    void attachProcess();
    void checkHash();
    void createConfiguration();
    void getBaseAddress();
    void getProcessByName();
    void loadKeyMapping();

public:
    Memory(const std::wstring& targetProcessName);
    virtual ~Memory();
    void update(WPARAM vkCode);
};

typedef std::unique_ptr<Memory> (__stdcall *CreateMemoryFn)(const std::wstring& targetProcessName);

class Dishonored2 : public Memory
{
protected:
    std::unique_ptr<MultilevelPointer> m_mpMP;

public:
    static std::unique_ptr<Memory> __stdcall Create(const std::wstring& targetProcessName);

    Dishonored2(const std::wstring& targetProcessName);
    virtual ~Dishonored2() {}
    void infiniteMPOff();
    void infiniteMPOn();
};

class NinjaMemory : public Memory
{
protected:
    std::unique_ptr<MultilevelPointer> m_mpInfinityItems;
    std::unique_ptr<MultilevelPointer> m_mpInstantCast;

public:
    static std::unique_ptr<Memory> __stdcall Create(const std::wstring& targetProcessName);

    NinjaMemory(const std::wstring& targetProcessName);
    virtual ~NinjaMemory() {}
    void infiniteItemsOff();
    void infiniteItemsOn();
    void instantCastOff();
    void instantCastOn();
};

class RE2Memory : public Memory
{
protected:
    std::unique_ptr<MultilevelPointer>              m_mpDamage;
    std::unique_ptr<MultilevelPointer>              m_mpFullDamage;
    std::unique_ptr<MultilevelPointer>              m_mpHealth;
    std::vector<std::unique_ptr<MultilevelPointer>> m_mpEnemies;
    std::unique_ptr<MultilevelPointer>              m_mpShots;

public:
    static std::unique_ptr<Memory> __stdcall Create(const std::wstring& targetProcessName);

    RE2Memory(const std::wstring& targetProcessName);
    virtual ~RE2Memory() {}
    void fullDamage();
    void killAll();
    void noDamage();
    void normalDamage();
    void restoreHealth();
    void shotsInfinite();
    void shotsNormal();
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