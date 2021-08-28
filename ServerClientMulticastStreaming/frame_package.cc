#include "frame_package.h"

FramePackage::FramePackage(const H264_Frame* h264_frame) :
	frame{h264_frame}
{

}

FramePackage::~FramePackage() = default;

const char* FramePackage::cData() const
{
	return (const char*)frame->getData();
}

uint16_t FramePackage::getCurrentSize() const
{
	return (uint16_t)frame->getFrameSize();
}

uint16_t FramePackage::getMaxSize() const
{
	return (uint16_t)frame->getCapacity();
}

char* FramePackage::data()
{
	return (char*)frame->getData();
}
