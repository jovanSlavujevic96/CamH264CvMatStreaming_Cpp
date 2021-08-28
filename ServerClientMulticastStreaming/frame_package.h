#pragma once

#include <cstdint>

#include "ipackage.h"
#include "H264_Frame.hpp"

class FramePackage : public IPackage
{
public:
    explicit FramePackage(const H264_Frame* h264_frame);
    ~FramePackage();

    const char* cData() const override;
    uint16_t getCurrentSize() const override;
    uint16_t getMaxSize() const override;

private:
    char* data() override;
    const H264_Frame* frame = NULL;
};
