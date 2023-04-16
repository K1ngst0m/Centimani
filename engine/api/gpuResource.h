#ifndef GPU_RESOURCE_H_
#define GPU_RESOURCE_H_

#include "common/common.h"

namespace aph
{
enum BufferUsageFlagBits
{
    BUFFER_USAGE_TRANSFER_SRC_BIT          = 0x00000001,
    BUFFER_USAGE_TRANSFER_DST_BIT          = 0x00000002,
    BUFFER_USAGE_UNIFORM_TEXEL_BUFFER_BIT  = 0x00000004,
    BUFFER_USAGE_STORAGE_TEXEL_BUFFER_BIT  = 0x00000008,
    BUFFER_USAGE_UNIFORM_BUFFER_BIT        = 0x00000010,
    BUFFER_USAGE_STORAGE_BUFFER_BIT        = 0x00000020,
    BUFFER_USAGE_INDEX_BUFFER_BIT          = 0x00000040,
    BUFFER_USAGE_VERTEX_BUFFER_BIT         = 0x00000080,
    BUFFER_USAGE_INDIRECT_BUFFER_BIT       = 0x00000100,
    BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT = 0x00020000,
    BUFFER_USAGE_FLAG_BITS_MAX_ENUM        = 0x7FFFFFFF
};
using BufferUsageFlags = uint32_t;

enum ImageUsageFlagBits
{
    IMAGE_USAGE_TRANSFER_SRC_BIT             = 0x00000001,
    IMAGE_USAGE_TRANSFER_DST_BIT             = 0x00000002,
    IMAGE_USAGE_SAMPLED_BIT                  = 0x00000004,
    IMAGE_USAGE_STORAGE_BIT                  = 0x00000008,
    IMAGE_USAGE_COLOR_ATTACHMENT_BIT         = 0x00000010,
    IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT = 0x00000020,
    IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT     = 0x00000040,
    IMAGE_USAGE_INPUT_ATTACHMENT_BIT         = 0x00000080,
    IMAGE_USAGE_FLAG_BITS_MAX_ENUM           = 0x7FFFFFFF
};
using ImageUsageFlags = uint32_t;

enum MemoryPropertyFlagBits
{
    MEMORY_PROPERTY_DEVICE_LOCAL_BIT     = 0x00000001,
    MEMORY_PROPERTY_HOST_VISIBLE_BIT     = 0x00000002,
    MEMORY_PROPERTY_HOST_COHERENT_BIT    = 0x00000004,
    MEMORY_PROPERTY_HOST_CACHED_BIT      = 0x00000008,
    MEMORY_PROPERTY_LAZILY_ALLOCATED_BIT = 0x00000010,
    MEMORY_PROPERTY_PROTECTED_BIT        = 0x00000020,
    MEMORY_PROPERTY_FLAG_BITS_MAX_ENUM   = 0x7FFFFFFF
};
using MemoryPropertyFlags = uint32_t;

enum SampleCountFlagBits
{
    SAMPLE_COUNT_1_BIT              = 0x00000001,
    SAMPLE_COUNT_2_BIT              = 0x00000002,
    SAMPLE_COUNT_4_BIT              = 0x00000004,
    SAMPLE_COUNT_8_BIT              = 0x00000008,
    SAMPLE_COUNT_16_BIT             = 0x00000010,
    SAMPLE_COUNT_32_BIT             = 0x00000020,
    SAMPLE_COUNT_64_BIT             = 0x00000040,
    SAMPLE_COUNT_FLAG_BITS_MAX_ENUM = 0x7FFFFFFF
};
using SampleCountFlags = uint32_t;

enum ShaderStageFlagBits
{
    SHADER_STAGE_VS_BIT             = 0x00000001,
    SHADER_STAGE_TCS_BIT            = 0x00000002,
    SHADER_STAGE_TES_BIT            = 0x00000004,
    SHADER_STAGE_GS_BIT             = 0x00000008,
    SHADER_STAGE_FS_BIT             = 0x00000010,
    SHADER_STAGE_CS_BIT             = 0x00000020,
    SHADER_STAGE_ALL_GRAPHICS       = 0x0000001F,
    SHADER_STAGE_ALL                = 0x7FFFFFFF,
    SHADER_STAGE_FLAG_BITS_MAX_ENUM = 0x7FFFFFFF
};
using ShaderStageFlags = uint32_t;

enum class ImageTiling
{
    OPTIMAL               = 0,
    LINEAR                = 1,
    IMAGE_TILING_MAX_ENUM = 0x7FFFFFFF
};

enum class ImageViewType
{
    _1D                      = 0,
    _2D                      = 1,
    _3D                      = 2,
    _CUBE                    = 3,
    _1D_ARRAY                = 4,
    _2D_ARRAY                = 5,
    _CUBE_ARRAY              = 6,
    IMAGE_VIEW_TYPE_MAX_ENUM = 0x7FFFFFFF
};

enum class ImageViewDimension
{
    _1D                           = 0,
    _2D                           = 1,
    _3D                           = 2,
    _CUBE                         = 3,
    _1D_ARRAY                     = 4,
    _2D_ARRAY                     = 5,
    _CUBE_ARRAY                   = 6,
    IMAGE_VIEW_DIMENSION_MAX_ENUM = 0x7FFFFFFF
};

enum class ImageType
{
    _1D                 = 0,
    _2D                 = 1,
    _3D                 = 2,
    IMAGE_TYPE_MAX_ENUM = 0x7FFFFFFF
};

enum class ImageLayout
{
    UNDEFINED                                  = 0,
    GENERAL                                    = 1,
    COLOR_ATTACHMENT_OPTIMAL                   = 2,
    DEPTH_STENCIL_ATTACHMENT_OPTIMAL           = 3,
    DEPTH_STENCIL_READ_ONLY_OPTIMAL            = 4,
    SHADER_READ_ONLY_OPTIMAL                   = 5,
    TRANSFER_SRC_OPTIMAL                       = 6,
    TRANSFER_DST_OPTIMAL                       = 7,
    PREINITIALIZED                             = 8,
    DEPTH_READ_ONLY_STENCIL_ATTACHMENT_OPTIMAL = 1000117000,
    DEPTH_ATTACHMENT_STENCIL_READ_ONLY_OPTIMAL = 1000117001,
    DEPTH_ATTACHMENT_OPTIMAL                   = 1000241000,
    DEPTH_READ_ONLY_OPTIMAL                    = 1000241001,
    STENCIL_ATTACHMENT_OPTIMAL                 = 1000241002,
    STENCIL_READ_ONLY_OPTIMAL                  = 1000241003,
    READ_ONLY_OPTIMAL                          = 1000314000,
    ATTACHMENT_OPTIMAL                         = 1000314001,
    PRESENT_SRC                                = 1000001002,
    VIDEO_DECODE_DST                           = 1000024000,
    VIDEO_DECODE_SRC                           = 1000024001,
    VIDEO_DECODE_DPB                           = 1000024002,
    SHARED_PRESENT                             = 1000111000,
    FRAGMENT_DENSITY_MAP_OPTIMAL               = 1000218000,
    FRAGMENT_SHADING_RATE_ATTACHMENT_OPTIMAL   = 1000164003,
    VK_IMAGE_LAYOUT_MAX_ENUM                   = 0x7FFFFFFF
};

enum class ComponentSwizzle
{
    IDENTITY = 0,
    ZERO     = 1,
    ONE      = 2,
    R        = 3,
    G        = 4,
    B        = 5,
    A        = 6,
    MAX_ENUM = 0x7FFFFFFF
};

struct ComponentMapping
{
    ComponentSwizzle r = { ComponentSwizzle::R };
    ComponentSwizzle g = { ComponentSwizzle::G };
    ComponentSwizzle b = { ComponentSwizzle::B };
    ComponentSwizzle a = { ComponentSwizzle::A };
};

struct ImageSubresourceRange
{
    uint32_t baseMipLevel   = { 0 };
    uint32_t levelCount     = { 1 };
    uint32_t baseArrayLayer = { 0 };
    uint32_t layerCount     = { 1 };
};

struct Extent3D
{
    uint32_t width  = { 0 };
    uint32_t height = { 0 };
    uint32_t depth  = { 0 };
};

struct DummyCreateInfo
{
    uint32_t typeId;
};

template <typename T_Handle, typename T_CreateInfo = DummyCreateInfo>
class ResourceHandle
{
public:
    ResourceHandle()
    {
        if constexpr(std::is_same<T_CreateInfo, DummyCreateInfo>::value)
        {
            m_createInfo.typeId = typeid(T_Handle).hash_code();
        }
    }
    T_Handle&       getHandle() { return m_handle; }
    const T_Handle& getHandle() const { return m_handle; }
    T_CreateInfo&   getCreateInfo() { return m_createInfo; }

protected:
    T_Handle     m_handle     = {};
    T_CreateInfo m_createInfo = {};
};

}  // namespace aph

#endif  // RESOURCE_H_
