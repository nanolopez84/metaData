#include "MultilevelPointer.h"


MultilevelPointer::MultilevelPointer(HANDLE processHandler, uint64_t baseAddress,
    const std::vector<uint64_t>& offsets)
    : m_processHandler(processHandler), m_baseAddress(baseAddress), m_offsets(offsets)
{
    this->resizeBuffer(sizeof(uint64_t));
}

void MultilevelPointer::getBytes(std::vector<uint8_t>& buffer, size_t bytesToRead, uint64_t offset)
{
    this->update();
    this->readProcessMemory(bytesToRead, m_effectiveAddress + offset);
    std::copy_n(m_buffer.begin(), bytesToRead, buffer.begin());
}

uint64_t MultilevelPointer::getLong()
{
    this->resizeBuffer(sizeof(uint64_t));

    uint32_t i1 = (uint32_t)(m_buffer[0] | m_buffer[1] << 8 | m_buffer[2] << 16 | m_buffer[3] << 24);
    uint64_t i2 = (uint64_t)(m_buffer[4] | m_buffer[5] << 8 | m_buffer[6] << 16 | m_buffer[7] << 24) << 32;
    return (uint64_t)i1 + i2;
}

bool MultilevelPointer::readProcessMemory(SIZE_T bytesToRead, uint64_t address)
{
    SIZE_T bytesReaded = 0;
    this->resizeBuffer(bytesToRead);
    return ReadProcessMemory(m_processHandler, (LPCVOID)(address ? address : m_effectiveAddress), &m_buffer[0], bytesToRead, &bytesReaded) && (bytesToRead == bytesReaded);
}

void MultilevelPointer::resizeBuffer(SIZE_T newSize)
{
    if (m_buffer.size() < newSize)
    {
        m_buffer.resize(newSize);
    }
}

bool MultilevelPointer::setBytes(std::vector<uint8_t>& buffer, uint64_t offset)
{
    this->update();
    if (!m_effectiveAddress)
    {
        return false;
    }
    return this->writeProcessMemory(buffer, m_effectiveAddress + offset);
}

bool MultilevelPointer::writeProcessMemory(std::vector<uint8_t>& buffer, uint64_t address)
{
    SIZE_T bytesToWrite = buffer.size();
    SIZE_T bytesWritten = 0;
    return WriteProcessMemory(m_processHandler, (LPVOID)(address ? address : m_effectiveAddress), &buffer[0], bytesToWrite, &bytesWritten) && (bytesToWrite == bytesWritten);
}

void MultilevelPointer::update()
{
    m_effectiveAddress = m_baseAddress;

    if (m_offsets.size() == 0)
    {
        return;
    }

    SIZE_T bytesToRead = sizeof(uint64_t);

    if (!this->readProcessMemory(bytesToRead, m_effectiveAddress))
    {
        m_effectiveAddress = 0;
        return;
    }
    m_effectiveAddress = this->getLong();

    for(auto offset : m_offsets)
    {
        m_effectiveAddress += offset;

        if (!this->readProcessMemory(bytesToRead, m_effectiveAddress))
        {
            m_effectiveAddress = 0;
            return;
        }

        m_effectiveAddress = this->getLong();
    }
}