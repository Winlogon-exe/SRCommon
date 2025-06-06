 //
// Created by Nikita on 27.11.2020.
//

#include <Utils/ECS/GameObject.h>
#include <Utils/ECS/Transform3D.h>
#include <Utils/ECS/Component.h>
#include <Utils/ECS/LayerManager.h>

#include <Utils/World/Scene.h>

#include <Utils/Math/Vector3.h>
#include <Utils/Math/Quaternion.h>
#include <Utils/Math/Matrix4x4.h>
#include <Utils/Math/Mathematics.h>

#include <Codegen/GameObject.generated.hpp>

namespace SR_UTILS_NS {
    GameObject::GameObject(const ObjectNameT name, SR_HTYPES_NS::SharedPtr<Transform> pTransform)
        : Super(name)
    {
        if (!pTransform) {
            pTransform = Transform3D::MakeShared<Transform3D, Transform>();
        }
        SetLayer(SR_UTILS_NS::LayerManager::GetDefaultLayer());
        SetTransform(pTransform);
    }

    GameObject::~GameObject() {
        m_transform.AutoFree();
    }

    GameObject::Ptr GameObject::GetOrCreateChild(StringAtom name) {
        if (auto&& pChild = Find(name).DynamicCast<GameObject>()) {
            return pChild;
        }
        return CreateChild(name);
    }

    GameObject::Ptr GameObject::CreateChild(StringAtom name) {
        auto&& pChild = GetScene()->InstanceGameObject(name);
        if (!Super::AddChild(pChild.StaticCast<SceneObject>())) {
            SRHalt("Something went wrong!");
        }
        return pChild.StaticCast<GameObject>();
    }

    SR_HTYPES_NS::Marshal::Ptr GameObject::SaveLegacy(SavableContext data) const {
        if (!(data.pMarshal = Entity::SaveLegacy(data))) {
            return data.pMarshal;
        }

        SavableContext transformSaveData;
        transformSaveData.flags = data.flags;
        transformSaveData.pMarshal = nullptr;

        data.pMarshal->Write<uint16_t>(GetEntityVersion());

        if (auto&& pPrefab = GetPrefab(); pPrefab && IsPrefabOwner()) {
            data.pMarshal->Write<bool>(true);
            data.pMarshal->Write<std::string>(pPrefab->GetResourcePath().ToStringRef());
            data.pMarshal->Write<std::string>(GetName());
            //data.pMarshal->Write<uint64_t>(m_tag.GetHash());
            //data.pMarshal->Write<uint64_t>(m_layer.GetHash());
            data.pMarshal->Write<bool>(IsEnabled());

            auto&& pTransformMarshal = GetTransform()->SaveLegacy(transformSaveData);
            data.pMarshal->Write<uint64_t>(pTransformMarshal->Size());
            data.pMarshal->Append(pTransformMarshal);

            return data.pMarshal;
        }
        else {
            data.pMarshal->Write<bool>(false);
        }

        data.pMarshal->Write(IsEnabled());
        data.pMarshal->Write(GetName().ToStringRef());

        data.pMarshal->Write<uint64_t>(GetTag().GetHash());
        data.pMarshal->Write<uint64_t>(GetLayer().GetHash());

        auto&& pTransformMarshal = GetTransform()->SaveLegacy(transformSaveData);
        data.pMarshal->Write<uint64_t>(pTransformMarshal->Size());
        data.pMarshal->Append(pTransformMarshal);

        /// save components

        data.pMarshal = SaveComponents(data);

        /// save children

        uint16_t childrenNum = 0;
        for (auto&& pChild : GetChildrenRef()) {
            if (pChild->HasSerializationFlags(ObjectSerializationFlags::DontSave)) {
                continue;
            }
            ++childrenNum;
        }

        data.pMarshal->Write(static_cast<uint16_t>(childrenNum));
        for (auto&& pChild : GetChildrenRef()) {
            if (pChild->HasSerializationFlags(ObjectSerializationFlags::DontSave)) {
                continue;
            }

            data.pMarshal = pChild->SaveLegacy(data);
        }

        return data.pMarshal;
    }

    Transform* GameObject::GetTransform() noexcept {
        return m_transform.Get();
    }

    const Transform* GameObject::GetTransform() const noexcept {
        return m_transform.Get();
    }

    void GameObject::SetTransform(const SR_HTYPES_NS::SharedPtr<Transform>& pTransform) {
        if (!pTransform) {
            SRHalt("pTransform is nullptr!");
            return;
        }

        if (pTransform->GetMeasurement() == Measurement::Holder && GetParent()) {
            SRHalt("Incorrect HOLDER transform usage!");
            return;
        }

        if (m_transform == pTransform) {
            SR_WARN("GameObject::SetTransform() : invalid transform!");
        }
        else {
            m_transform.AutoFree();
            m_transform = pTransform;
            m_transform->SetGameObject(this);
            SetDirty(true);
        }

        for (auto&& pComponent : m_components) {
            pComponent->OnTransformSet();
        }
    }

    void GameObject::OnHierarchyChanged() {
        Super::OnHierarchyChanged();
        if (m_transform) {
            m_transform->OnHierarchyChanged();
        }
    }

    void GameObject::OnAttached() {
        if (GetParentTransform()) {
            m_transform->UpdateTree();
        }
        else {
            SR_WARN("GameObject::OnAttached() : GameObject doesn't have parent to get transform!");
        }
    }

    GameObject::Ptr GameObject::Load(SR_HTYPES_NS::Marshal& marshal, const ScenePtr& scene) {
        SR_TRACY_ZONE;

        SR_UTILS_NS::SceneObject::Ptr pSceneObject;

        /// для экономии памяти стека при рекурсивном создании объектов, кладем все переменные в эту область видимости.
        {
            auto&& entityId = marshal.Read<uint64_t>();
            auto&& version = marshal.Read<uint16_t>();

            const uint16_t newVersion = SR_UTILS_NS::GameObject::VERSION;

            if (version != newVersion) {
                SR_INFO("GameObject::Load() : game object has different version! Trying to migrate from " +
                        SR_UTILS_NS::ToString(version) + " to " + SR_UTILS_NS::ToString(newVersion) + "..."
                );

                static const auto GAME_OBJECT_HASH_NAME = SR_HASH_STR("GameObject");

                if (!Migration::Instance().Migrate(GAME_OBJECT_HASH_NAME, marshal, version, newVersion)) {
                    SR_ERROR("GameObject::Load() : failed to migrate game object!");
                    return pSceneObject.DynamicCast<GameObject>();
                }
            }

            if (marshal.Read<bool>()) { /// is prefab
                auto&& prefabPath = marshal.Read<std::string>();
                auto&& objectName = marshal.Read<std::string>();
                auto&& isEnabled = marshal.Read<bool>();

                SR_MAYBE_UNUSED auto&& transformBlockSize = marshal.Read<uint64_t>();
                auto&& pTransform = Transform::Load(marshal);

                if (!pTransform) {
                    SRHalt("Failed to load transform!");
                    return pSceneObject.DynamicCast<GameObject>();
                }

                if (auto&& pPrefab = Prefab::Load(prefabPath)) {
                    /// Здесь загружается тег, и слой
                    pSceneObject = pPrefab->Instance(scene);
                    pPrefab->CheckResourceUsage();
                }

                if (!pSceneObject) {
                    SR_LOG("GameObject::Load() : failed to load prefab!\n\tPath: " + prefabPath);
                    delete pTransform;
                    return pSceneObject.DynamicCast<GameObject>();
                }

                pSceneObject->SetName(objectName);
                pSceneObject->SetEnabled(isEnabled);

                if (auto&& pGameObject = pSceneObject.DynamicCast<GameObject>()) {
                    pGameObject->SetTransform(pTransform);
                }
                else {
                    SRHalt("GameObject::Load() : failed to cast prefab to game object!");
                    return pSceneObject.DynamicCast<GameObject>();
                }

                return pSceneObject.DynamicCast<GameObject>();
            }

            auto&& enabled = marshal.Read<bool>();
            auto&& name = marshal.Read<std::string>();

            auto&& tag = SR_HASH_TO_STR_ATOM(marshal.Read<uint64_t>());
            auto&& layer = SR_HASH_TO_STR_ATOM(marshal.Read<uint64_t>());
            if (layer.Empty()) {
                layer = LayerManager::GetDefaultLayer();
            }

            if (entityId == UINT64_MAX) {
                pSceneObject = new GameObject(name);
            }
            else {
                SR_UTILS_NS::EntityManager::Instance().GetReserved(entityId, [&pSceneObject, name]() -> SR_UTILS_NS::Entity::Ptr {
                    pSceneObject = new GameObject(name);
                    return pSceneObject.DynamicCast<SR_UTILS_NS::Entity>();
                });
            }

            if (!pSceneObject.Valid()) {
                SRHalt("GameObject::Load() : failed to create new game object!");
                return SR_UTILS_NS::GameObject::Ptr();
            }

            if (scene) {
                scene->RegisterSceneObject(pSceneObject);
            }

            /// ----------------------

            pSceneObject->SetEnabled(enabled);

            SR_MAYBE_UNUSED auto&& transformBlockSize = marshal.Read<uint64_t>();

            if (auto&& pGameObject = pSceneObject.DynamicCast<GameObject>()) {
                pGameObject->SetTransform(SR_UTILS_NS::Transform::Load(marshal));
            }
            else {
                SRHalt("GameObject::Load() : failed to cast prefab to game object!");
                return pSceneObject.DynamicCast<GameObject>();
            }

            pSceneObject->SetTag(tag);
            pSceneObject->SetLayer(layer);

            /// ----------------------

            auto&& components = ComponentManager::Instance().LoadComponents(marshal);
            for (auto&& pComponent : components) {
                pSceneObject->AddComponent(pComponent);
            }
        }

        auto&& childrenCount = marshal.Read<uint16_t>();
        for (uint16_t i = 0; i < childrenCount; ++i) {
            if (auto&& pChild = Load(marshal, scene)) {
                pSceneObject->AddChild(pChild.StaticCast<SceneObject>());
            }
        }

        return pSceneObject.DynamicCast<GameObject>();
    }

    SceneObject::Ptr GameObject::Copy(const SceneObject::ScenePtr& pScene, const SceneObject::Ptr& pObject) const {
        const GameObject::Ptr pGameObject = new GameObject(GetName(), GetTransform()->Copy());
        return Super::Copy(pScene, pGameObject.StaticCast<SceneObject>()).StaticCast<SceneObject>();
    }

    Transform* GameObject::GetParentTransform() const noexcept {
        if (GetParent() && GetParent()->GetSceneObjectType() == SceneObjectType::GameObject) {
            return GetParent().StaticCast<GameObject>()->GetTransform();
        }
        return nullptr;
    }
}