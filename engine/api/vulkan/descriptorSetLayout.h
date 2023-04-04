#ifndef DESCRIPTORSET_H_
#define DESCRIPTORSET_H_

#include "api/gpuResource.h"
#include "vkUtils.h"

namespace aph
{
class VulkanDevice;
class VulkanDescriptorPool;

class VulkanDescriptorSetLayout : public ResourceHandle<VkDescriptorSetLayout>
{
public:
    static VulkanDescriptorSetLayout *Create(VulkanDevice *device,
                                             VkDescriptorSetLayoutCreateInfo *pCreateInfo,
                                             VkDescriptorSetLayout handle);
    VulkanDescriptorSetLayout();

    ~VulkanDescriptorSetLayout();

    std::vector<VkDescriptorSetLayoutBinding> getBindings() { return m_bindings; }
    VulkanDevice *getDevice() { return m_device; }

    VkDescriptorSet allocateSet();
    VkResult freeSet(VkDescriptorSet set);

private:
    VulkanDevice *m_device;
    std::vector<VkDescriptorSetLayoutBinding> m_bindings;
    VulkanDescriptorPool *m_pool;
};

}  // namespace aph

#endif  // DESCRIPTORSET_H_