#pragma once
#include <vector>
#include <cstdint>
#include <memory>
#include <string>

class MemoryReader {
public:
    template <typename T>
    T Read() {
        T val;
        std::memcpy(&val, m_data + m_position, sizeof(T));
        m_position += sizeof(T);
        return val;
    }
    template <typename T>
    void Read(T* value) {
        std::memcpy(value, m_data + m_position, sizeof(T));
        m_position += sizeof(T);
    }
    template <typename T>
    std::string readString() {
        T length = this->Read<T>();
        std::string val = std::string(reinterpret_cast<char*>(m_data + m_position), length);
        m_position += length;
        return val;
    }
    std::string ReadString() {
        std::uint16_t length = this->Read<std::uint16_t>();
        std::string val = std::string(reinterpret_cast<char*>(m_data + m_position), length);
        m_position += length;
        return val;
    }
    template <typename T>
    void readString(std::string& value) {
        T length = this->Read<T>();
        value = std::string(reinterpret_cast<char*>(m_data + m_position), length);
        m_position += length;
    }

    void Skip(std::size_t length) { m_position += length; }
    void BackToPos(std::size_t length) { m_position -= length; }
    void SkipUntil() { m_position++; };
    void SkipString() { m_position += Read<std::uint16_t>(); };

	void ResetPosition() { m_position = 0; }
	void SetPosition(std::size_t position) { m_position = position; }

    std::uint8_t* GetData() { return m_data; }
    std::size_t GetPosition() const { return m_position; }

public:
    MemoryReader(uint8_t* data, std::size_t len) : m_position(0) {
        m_data = new uint8_t[len];
        std::memset(m_data, 0, len);
        std::memcpy(m_data, data, len);
    }
    MemoryReader(void* pData) : m_data(static_cast<std::uint8_t*>(pData)), m_position(0) {}
    ~MemoryReader() = default;

private:
    std::uint8_t* m_data;
    std::size_t m_position;
};