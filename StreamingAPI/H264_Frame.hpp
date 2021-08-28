#pragma once

#include <cstdint>

class H264_Frame
{
private:
    uint8_t* data = NULL;
    size_t capacity = 0;
    size_t size = 0;
public:
    H264_Frame() = default;
    H264_Frame(size_t size);
    ~H264_Frame();

    void close();
    void setFrame(uint8_t* data, size_t size);
    size_t getFrameSize() const;
    size_t getCapacity() const;
    const uint8_t* getData() const;
    void setSize(size_t size);
    void setCapacity(size_t capacity);
};
