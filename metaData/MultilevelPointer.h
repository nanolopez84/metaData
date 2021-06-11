#pragma once

#include <windows.h>

#include <stdint.h>
#include <memory>
#include <vector>

/*
 * m_baseAddress --> + offsets[0] --> + ... --> + offsets[n] --> m_effectiveAddress
 *
 * getInt(offset, value) --> m_effectiveAddress + offset --> cast as int into "value"
 */
class MultilevelPointer
{
protected:
    uint64_t                m_baseAddress;
    std::vector<uint8_t>    m_buffer;
    uint64_t                m_effectiveAddress;
    std::vector<uint64_t>   m_offsets;
    HANDLE                  m_processHandler;

    void resizeBuffer(SIZE_T newSize);
    void update();

public:
    MultilevelPointer(HANDLE processHandler, uint64_t baseAddress, const std::vector<uint64_t>& offsets = std::vector<uint64_t>());
    void        getBytes(std::vector<uint8_t>& buffer, size_t count, uint64_t offset = 0);
    uint64_t    getLong();
    bool        readProcessMemory(SIZE_T bytesToRead = sizeof(uint64_t), uint64_t address = 0);
    bool        setBytes(std::vector<uint8_t>& buffer, uint64_t offset = 0);
    bool        writeProcessMemory(std::vector<uint8_t>& buffer, uint64_t address = 0);
};