#ifndef VKLMODEL_H_
#define VKLMODEL_H_

#include "idObject.h"

namespace aph
{

class Scene;

enum class ObjectType : uint8_t
{
    UNATTACHED,
    LIGHT,
    CAMERA,
    MESH,
    SCENENODE,
};

class Object : public IdObject
{
public:
    template <typename TObject, typename... Args>
    static std::shared_ptr<TObject> Create(Args &&...args)
    {
        auto instance = std::make_shared<TObject>(std::forward<Args>(args)...);
        return instance;
    }
    Object(IdType id, ObjectType type) : IdObject{ id }, m_type{ type } {}
    virtual ~Object() = default;

    ObjectType getType() { return m_type; }

protected:
    ObjectType m_type{};
};

class UniformObject : public Object
{
public:
    UniformObject(IdType id, ObjectType type) : Object{ id, type } {}
    ~UniformObject() override = default;

    virtual void load() = 0;
    virtual void update(float deltaTime) = 0;

    virtual void *getData() { return data.get(); }
    virtual uint32_t getDataSize() { return dataSize; }

protected:
    size_t dataSize{ 0 };
    std::shared_ptr<void> data{};

    bool updated{ false };
};

}  // namespace aph

#endif  // VKLMODEL_H_
