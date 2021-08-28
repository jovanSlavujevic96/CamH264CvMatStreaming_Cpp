#include <cstdlib>
#include <cstring>

#include "H264_Frame.hpp"

/// <summary>
/// H264_Frame
/// </summary>

H264_Frame::H264_Frame(size_t size) :
    size{size}
{
    data = (uint8_t*)std::malloc(size);
    capacity = size;
}

H264_Frame::~H264_Frame()
{
    H264_Frame::close();
}

void H264_Frame::close()
{
    if (data)
    {
        std::free(data);
        data = NULL;
    }
}

void H264_Frame::setFrame(uint8_t* setData, size_t setSize)
{
    if (!setData || !setSize)
    {
        return;
    }
    size = setSize;
    if (!data)
    {
        data = (uint8_t*)std::malloc(size);
        capacity = size;
    }
    else if (size > capacity)
    {
        data = (uint8_t*)std::realloc(data, size);
        capacity = size;
    }
    std::memcpy(data, setData, size);
}

size_t H264_Frame::getFrameSize() const
{
    return size;
}

size_t H264_Frame::getCapacity() const
{
    return capacity;
}

const uint8_t* H264_Frame::getData() const
{
    return data;
}

void H264_Frame::setSize(size_t size)
{
    this->size = size;
}

void H264_Frame::setCapacity(size_t capacity)
{
    this->capacity = capacity;
}
