//
// Created by Monika on 12.03.2022.
//

#include <Utils/Types/Marshal.h>
#include <Utils/Common/StringUtils.h>
#include <Utils/Resources/ResourceManager.h>
#include <Utils/Profile/TracyContext.h>

namespace SR_HTYPES_NS {
    Marshal::Marshal(std::ifstream& ifs)
        : Super(ifs)
    { }

    Marshal::Marshal(const std::string& str)
        : Super(str)
    { }

    Marshal::Marshal(const char *pData, uint64_t size)
        : Super(pData, size)
    { }

    void Marshal::Append(Marshal&& marshal) {
        if (marshal && marshal.Size() > 0) {
            Super::Write(marshal.Super::View(), marshal.Size());
        }
    }

    void Marshal::Append(Marshal::Ptr& pMarshal) {
        if (pMarshal && *pMarshal && pMarshal->Size() > 0) {
            Super::Write(pMarshal->Super::View(), pMarshal->Size());
        }

        SR_SAFE_DELETE_PTR(pMarshal);
    }

    void Marshal::Append(std::unique_ptr<Marshal>&& pMarshal) {
        if (pMarshal && *pMarshal && pMarshal->Size() > 0) {
            Super::Write(pMarshal->Super::View(), pMarshal->Size());
        }
    }

    bool Marshal::Save(const Path& path) const {
        if (!path.Make()) {
            return false;
        }

        std::ofstream file;
        file.open(path.ToString(), std::ios::binary);
        if (!file.is_open()) {
            return false;
        }

        file.write(Super::View(), Size());
        file.close();

        return true;
    }

    Marshal::Ptr Marshal::LoadPtr(const Path& path) {
        SR_TRACY_ZONE;
        SR_TRACY_ZONE_TEXT(path.ToStringRef());

        std::ifstream file(path.ToString(), std::ios::binary);
        if (!file.is_open()) {
            return nullptr;
        }

        auto&& pMarshal = new Marshal(file);

        if (!pMarshal->Valid()) {
            delete pMarshal;
            pMarshal = nullptr;
        }

        file.close();

        return pMarshal;
    }

    Marshal Marshal::Load(const Path& path) {
        SR_TRACY_ZONE;
        SR_TRACY_ZONE_TEXT(path.ToStringRef());

        std::ifstream file(path.ToString(), std::ios::binary);
        if (!file.is_open()) {
            return Marshal();
        }

        Marshal marshal(file);
        file.close();

        return marshal;
    }

    Marshal Marshal::Copy() const {
        return *this;
    }

    Marshal::Ptr Marshal::CopyPtr() const {
        return new Marshal(*this);
    }

    Marshal Marshal::LoadFromMemory(const std::string& data) {
        return Marshal(data);
    }

    Marshal Marshal::LoadFromBase64(const std::string& base64) {
        return LoadFromMemory(SR_UTILS_NS::StringUtils::Base64Decode(base64));
    }

    Marshal Marshal::ReadBytes(uint64_t count) noexcept {
        if (GetPosition() + count > GetCapacity()) {
            SRHalt("Invalid range!");
            return Marshal(); /// NOLINT
        }

        auto&& marshal = Marshal(Super::View() + GetPosition(), count);
        Skip(count);
        return marshal;
    }

    Marshal::Ptr Marshal::ReadBytesPtr(uint64_t count) noexcept {
        if (GetPosition() + count > GetCapacity()) {
            SRHalt("Invalid range!");
            return nullptr;
        }

        auto&& pMarshal = new Marshal(Super::View() + GetPosition(), count);

        Skip(count);

        return pMarshal;
    }

    Marshal Marshal::FullCopy() const {
        Marshal copy = *this;
        copy.SetPosition(GetPosition());
        return std::move(copy);
    }
}