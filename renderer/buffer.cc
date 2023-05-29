#include "renderer/buffer.h"
#include "renderer/device.h"

Buffer::~Buffer() { vkDestroyBuffer(device.getHandle(), handle, nullptr); }
