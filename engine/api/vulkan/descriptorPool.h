#ifndef DESCRIPTORPOOL_H_
#define DESCRIPTORPOOL_H_

#include "common/spinlock.h"
#include "api/gpuResource.h"
#include "vkUtils.h"

namespace aph
{
class VulkanDescriptorSetLayout;
class VulkanDescriptorPool : ResourceHandle<VkDescriptorPool>
{
public:
    VulkanDescriptorPool(VulkanDescriptorSetLayout *layout);
    ~VulkanDescriptorPool();

    VkDescriptorSet allocateSet();

    VkResult freeSet(VkDescriptorSet descriptorSet);

private:
    VulkanDescriptorSetLayout *_layout;
    std::vector<VkDescriptorPoolSize> _poolSizes;
    uint32_t _maxSetsPerPool = 50;

    std::vector<VkDescriptorPool> _pools;
    std::vector<uint32_t> _allocatedSets;
    uint32_t _currentAllocationPoolIndex = 0;
    std::unordered_map<VkDescriptorSet, uint32_t> _allocatedDescriptorSets;

    SpinLock _spinLock;
};
}  // namespace aph

#endif  // DESCRIPTORPOOL_H_
