#ifndef VULKAN_IMAGE_H_
#define VULKAN_IMAGE_H_

#include "common.h"
#include "renderer/gpuResource.h"
#include "vkUtils.h"

namespace vkl {
class VulkanDevice;
class VulkanImage : public Image, public ResourceHandle<VkImage> {
public:
    static VulkanImage *createFromHandle(VulkanDevice *pDevice, ImageCreateInfo *pCreateInfo, VkImage image, VkDeviceMemory memory = VK_NULL_HANDLE);

    VkDeviceMemory getMemory();
    VulkanDevice  *getDevice();

    VkResult bind(VkDeviceSize offset = 0) const;

private:
    VulkanDevice  *_device             = nullptr;
    VkDeviceMemory _memory             = VK_NULL_HANDLE;

    void *_mapped = nullptr;
};

} // namespace vkl

#endif // IMAGE_H_
