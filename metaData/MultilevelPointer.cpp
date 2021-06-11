#include "MultilevelPointer.h"


MultilevelPointer::MultilevelPointer(HANDLE processHandler, uint64_t baseAddress,
    std::unique_ptr<std::vector<uint8_t>> offsets)
    : m_processHandler(processHandler), m_baseAddress(baseAddress)
{
    m_offsets = offsets ? std::move(offsets) : std::make_unique<std::vector<uint8_t>>();

    this->resizeBuffer(sizeof(uint64_t));
}

uint64_t MultilevelPointer::getLong()
{
    this->resizeBuffer(sizeof(uint64_t));

    uint8_t  i1 = (uint8_t) (m_buffer[0] | m_buffer[1] << 8 | m_buffer[2] << 16 | m_buffer[3] << 24);
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

bool MultilevelPointer::setBytes(std::vector<uint8_t>& buffer)
{
    this->update();
    return this->writeProcessMemory(buffer);
}

bool MultilevelPointer::writeProcessMemory(std::vector<uint8_t>& buffer, uint64_t address)
{
    SIZE_T bytesToWrite = buffer.size();
    SIZE_T bytesWritten = 0;
    return WriteProcessMemory(m_processHandler, (LPVOID)(address ? address : m_effectiveAddress), &buffer[0], bytesToWrite, &bytesWritten) && (bytesToWrite == bytesWritten);
}

void MultilevelPointer::update()
{
    if (m_offsets->size() == 0)
    {
        m_effectiveAddress = m_baseAddress;
        return;
    }
}