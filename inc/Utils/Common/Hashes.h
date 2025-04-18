//
// Created by Monika on 03.05.2022.
//

#ifndef SR_ENGINE_UTILS_HASHES_H
#define SR_ENGINE_UTILS_HASHES_H

#include <Utils/stdInclude.h>
#include <Utils/Types/MerkleTree.h>

#include <openssl/sha.h>
#include <xxHash/xxhash.h>

namespace SR_UTILS_NS {
    namespace Hash::Detail {
        static constexpr unsigned int crc_table[256] = {
                0x00000000, 0x77073096, 0xee0e612c, 0x990951ba, 0x076dc419, 0x706af48f,
                0xe963a535, 0x9e6495a3, 0x0edb8832, 0x79dcb8a4, 0xe0d5e91e, 0x97d2d988,
                0x09b64c2b, 0x7eb17cbd, 0xe7b82d07, 0x90bf1d91, 0x1db71064, 0x6ab020f2,
                0xf3b97148, 0x84be41de, 0x1adad47d, 0x6ddde4eb, 0xf4d4b551, 0x83d385c7,
                0x136c9856, 0x646ba8c0, 0xfd62f97a, 0x8a65c9ec, 0x14015c4f, 0x63066cd9,
                0xfa0f3d63, 0x8d080df5, 0x3b6e20c8, 0x4c69105e, 0xd56041e4, 0xa2677172,
                0x3c03e4d1, 0x4b04d447, 0xd20d85fd, 0xa50ab56b, 0x35b5a8fa, 0x42b2986c,
                0xdbbbc9d6, 0xacbcf940, 0x32d86ce3, 0x45df5c75, 0xdcd60dcf, 0xabd13d59,
                0x26d930ac, 0x51de003a, 0xc8d75180, 0xbfd06116, 0x21b4f4b5, 0x56b3c423,
                0xcfba9599, 0xb8bda50f, 0x2802b89e, 0x5f058808, 0xc60cd9b2, 0xb10be924,
                0x2f6f7c87, 0x58684c11, 0xc1611dab, 0xb6662d3d, 0x76dc4190, 0x01db7106,
                0x98d220bc, 0xefd5102a, 0x71b18589, 0x06b6b51f, 0x9fbfe4a5, 0xe8b8d433,
                0x7807c9a2, 0x0f00f934, 0x9609a88e, 0xe10e9818, 0x7f6a0dbb, 0x086d3d2d,
                0x91646c97, 0xe6635c01, 0x6b6b51f4, 0x1c6c6162, 0x856530d8, 0xf262004e,
                0x6c0695ed, 0x1b01a57b, 0x8208f4c1, 0xf50fc457, 0x65b0d9c6, 0x12b7e950,
                0x8bbeb8ea, 0xfcb9887c, 0x62dd1ddf, 0x15da2d49, 0x8cd37cf3, 0xfbd44c65,
                0x4db26158, 0x3ab551ce, 0xa3bc0074, 0xd4bb30e2, 0x4adfa541, 0x3dd895d7,
                0xa4d1c46d, 0xd3d6f4fb, 0x4369e96a, 0x346ed9fc, 0xad678846, 0xda60b8d0,
                0x44042d73, 0x33031de5, 0xaa0a4c5f, 0xdd0d7cc9, 0x5005713c, 0x270241aa,
                0xbe0b1010, 0xc90c2086, 0x5768b525, 0x206f85b3, 0xb966d409, 0xce61e49f,
                0x5edef90e, 0x29d9c998, 0xb0d09822, 0xc7d7a8b4, 0x59b33d17, 0x2eb40d81,
                0xb7bd5c3b, 0xc0ba6cad, 0xedb88320, 0x9abfb3b6, 0x03b6e20c, 0x74b1d29a,
                0xead54739, 0x9dd277af, 0x04db2615, 0x73dc1683, 0xe3630b12, 0x94643b84,
                0x0d6d6a3e, 0x7a6a5aa8, 0xe40ecf0b, 0x9309ff9d, 0x0a00ae27, 0x7d079eb1,
                0xf00f9344, 0x8708a3d2, 0x1e01f268, 0x6906c2fe, 0xf762575d, 0x806567cb,
                0x196c3671, 0x6e6b06e7, 0xfed41b76, 0x89d32be0, 0x10da7a5a, 0x67dd4acc,
                0xf9b9df6f, 0x8ebeeff9, 0x17b7be43, 0x60b08ed5, 0xd6d6a3e8, 0xa1d1937e,
                0x38d8c2c4, 0x4fdff252, 0xd1bb67f1, 0xa6bc5767, 0x3fb506dd, 0x48b2364b,
                0xd80d2bda, 0xaf0a1b4c, 0x36034af6, 0x41047a60, 0xdf60efc3, 0xa867df55,
                0x316e8eef, 0x4669be79, 0xcb61b38c, 0xbc66831a, 0x256fd2a0, 0x5268e236,
                0xcc0c7795, 0xbb0b4703, 0x220216b9, 0x5505262f, 0xc5ba3bbe, 0xb2bd0b28,
                0x2bb45a92, 0x5cb36a04, 0xc2d7ffa7, 0xb5d0cf31, 0x2cd99e8b, 0x5bdeae1d,
                0x9b64c2b0, 0xec63f226, 0x756aa39c, 0x026d930a, 0x9c0906a9, 0xeb0e363f,
                0x72076785, 0x05005713, 0x95bf4a82, 0xe2b87a14, 0x7bb12bae, 0x0cb61b38,
                0x92d28e9b, 0xe5d5be0d, 0x7cdcefb7, 0x0bdbdf21, 0x86d3d2d4, 0xf1d4e242,
                0x68ddb3f8, 0x1fda836e, 0x81be16cd, 0xf6b9265b, 0x6fb077e1, 0x18b74777,
                0x88085ae6, 0xff0f6a70, 0x66063bca, 0x11010b5c, 0x8f659eff, 0xf862ae69,
                0x616bffd3, 0x166ccf45, 0xa00ae278, 0xd70dd2ee, 0x4e048354, 0x3903b3c2,
                0xa7672661, 0xd06016f7, 0x4969474d, 0x3e6e77db, 0xaed16a4a, 0xd9d65adc,
                0x40df0b66, 0x37d83bf0, 0xa9bcae53, 0xdebb9ec5, 0x47b2cf7f, 0x30b5ffe9,
                0xbdbdf21c, 0xcabac28a, 0x53b39330, 0x24b4a3a6, 0xbad03605, 0xcdd70693,
                0x54de5729, 0x23d967bf, 0xb3667a2e, 0xc4614ab8, 0x5d681b02, 0x2a6f2b94,
                0xb40bbe37, 0xc30c8ea1, 0x5a05df1b, 0x2d02ef8d
        };

        template<int size, int idx = 0, class dummy = void>
        struct MM {
            static constexpr unsigned int crc32(const char *str, unsigned int prev_crc = 0xFFFFFFFF) {
                return MM<size, idx + 1>::crc32(str, (prev_crc >> 8) ^ crc_table[(prev_crc ^ str[idx]) & 0xFF]);
            }
        };

        template<int size, class dummy>
        struct MM<size, size, dummy> {
            static constexpr unsigned int crc32(const char *str, unsigned int prev_crc = 0xFFFFFFFF) {
                return prev_crc ^ 0xFFFFFFFF;
            }
        };

        constexpr auto crc32(const char *in) {
            auto crc = 0xFFFFFFFFu;

            for (auto i = 0u; auto c = in[i]; ++i) {
                crc = crc_table[(crc ^ c) & 0xFF] ^ (crc >> 8);
            }

            return ~crc;
        }

        template<class T> constexpr std::string_view GetTypeName() {
            return SR_FUNCTION_SIGNATURE;
        }

        template<typename T>
        concept SHA256HashType =
            std::constructible_from<T, std::string> &&
            requires(T type) { type.to_string(); } ||
            requires(T type) { type.ToString(); };

        static constexpr SRHashType InitialHash() noexcept { return 0xcbf29ce484222325; }
        static constexpr SRHashType MagicPrime() noexcept { return 0x100000001b3; }

        template<size_t S, size_t N> struct Encoder {
            static constexpr SRHashType EncodeChar(SRHashType hash, const char (&text)[S]) noexcept {
                hash = (hash ^ static_cast<SRHashType>(static_cast<uint8_t>(text[S - N - 1]))) * MagicPrime();
                return Encoder<S, N - 1>::EncodeChar(hash, text);
            }
        };

        template<size_t S> struct Encoder<S, 0> {
            static constexpr SRHashType EncodeChar(SRHashType hash, const char (&)[S]) noexcept {
                return hash;
            }
        };
    }

    template<size_t S> SR_NODISCARD static constexpr uint64_t ComputeHashConstexpr(const char (&text)[S]) noexcept {
        return static_cast<uint64_t>(Hash::Detail::Encoder<S, S - 1>::EncodeChar(Hash::Detail::InitialHash(), text));
    }

    SR_NODISCARD static constexpr uint64_t ComputeHash(const char* text) noexcept {
        uint64_t hash = Hash::Detail::InitialHash();
        for (size_t i = 0; text[i] != '\0'; ++i) {
            hash = (hash ^ static_cast<uint64_t>(static_cast<uint8_t>(text[i]))) * Hash::Detail::MagicPrime();
        }
        return hash;
    }

    SR_NODISCARD static constexpr uint64_t ComputeHash(const std::string_view text) noexcept {
        uint64_t hash = Hash::Detail::InitialHash();
        for (const char i : text) {
            hash = (hash ^ static_cast<uint64_t>(static_cast<uint8_t>(i))) * Hash::Detail::MagicPrime();
        }
        return hash;
    }

    SR_INLINE_STATIC constexpr uint64_t SR_FNV_OFFSET_BASIS = 14695981039346656037ULL;
    SR_INLINE_STATIC constexpr uint64_t SR_FNV_PRIME = 1099511628211ULL;

    SR_NODISCARD constexpr SR_INLINE_STATIC uint64_t FNV1AAppendBytes(uint64_t value, const unsigned char* const first, const uint64_t count) noexcept {
        for (uint64_t i = 0; i < count; ++i) {
            value ^= static_cast<uint64_t>(first[i]);
            value *= SR_FNV_PRIME;
        }

        return value;
    }

    template<class T> SR_NODISCARD SR_INLINE_STATIC constexpr uint64_t FNV1AAppendValue(const uint64_t value, const T& keyValue) noexcept {
        return FNV1AAppendBytes(value, &reinterpret_cast<const unsigned char&>(keyValue), sizeof(T));
    }

    template<class T> SR_NODISCARD constexpr uint64_t HashArrayRepresentation(const T* const first, const size_t count) noexcept {
        return FNV1AAppendBytes(SR_FNV_OFFSET_BASIS, reinterpret_cast<const unsigned char*>(first), count * sizeof(T));
    }

    template<class T> SR_NODISCARD constexpr uint64_t HashRepresentation(const T& keyVal) noexcept {
        return FNV1AAppendValue(SR_FNV_OFFSET_BASIS, keyVal);
    }

    template <class T> struct SRHash;

    template <class T, bool Enabled> struct SRConditionallyEnabledHash {
        SR_NODISCARD constexpr  size_t operator()(const T& keyVal) const
        noexcept(noexcept(SR_UTILS_NS::SRHash<T>::DoHash(keyVal))) {
            return SR_UTILS_NS::SRHash<T>::DoHash(keyVal);
        }
    };

    template <class T> struct SRConditionallyEnabledHash<T, false> {
        constexpr SRConditionallyEnabledHash() = delete;
        constexpr SRConditionallyEnabledHash(const SRConditionallyEnabledHash&) = delete;
        constexpr SRConditionallyEnabledHash(SRConditionallyEnabledHash&&) = delete;
        constexpr SRConditionallyEnabledHash& operator=(const SRConditionallyEnabledHash&) = delete;
        constexpr SRConditionallyEnabledHash& operator=(SRConditionallyEnabledHash&&) = delete;
    };

    template<class T> struct SRHash : SRConditionallyEnabledHash<T, !std::is_const_v<T> && !std::is_volatile_v<T> && (std::is_enum_v<T> || std::is_integral_v<T> || std::is_pointer_v<T>)> {
        constexpr static uint64_t DoHash(const T& value) noexcept {
            return FNV1AAppendValue(SR_FNV_OFFSET_BASIS, value);
        }
    };

    template<typename T> constexpr uint64_t CalculateHash(const T& value) {
        constexpr SRHash<T> h;
        return h(value);
    }

    template<typename T> constexpr uint64_t HashCombine(const T& value, uint64_t hash = 0) {
        return hash ^ CalculateHash<T>(value) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
    }

    SR_MAYBE_UNUSED constexpr static uint64_t CombineTwoHashes(uint64_t hash1, uint64_t hash2) {
        return hash2 ^ hash1 + 0x9e3779b9 + (hash2 << 6) + (hash2 >> 2);
    }

    template <class T, class... Types> constexpr bool IsAnyOfV = std::disjunction_v<std::is_same<T, Types>...>;
    template <class T> SR_INLINE constexpr bool IsECharT = IsAnyOfV<T, char, wchar_t, char8_t, char16_t, char32_t>;

    template<Hash::Detail::SHA256HashType HashType, typename DataTypePtr>
        requires std::is_pointer_v<DataTypePtr>
        HashType sha256(const DataTypePtr data, uint32_t size) {
        std::array<uint8_t, SHA256_DIGEST_LENGTH> hash{};
        SHA256_CTX ctx;

        SHA256_Init(&ctx);
        SHA256_Update(&ctx, data, size);
        SHA256_Final(hash.data(), &ctx);

        std::stringstream sstream;
        for (auto&& i : hash)
            sstream << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(i);

        return HashType{ sstream.str() };
    }

    template<Hash::Detail::SHA256HashType T = SR_HTYPES_NS::HashT<32>>
    T sha256(const std::string& msg) {
        return sha256<T>(msg.data(), msg.size());
    }
}

template <class Elem, class Alloc> struct SR_UTILS_NS::SRHash<std::basic_string<Elem, std::char_traits<Elem>, Alloc>> : SR_UTILS_NS::SRConditionallyEnabledHash<std::basic_string<Elem, std::char_traits<Elem>, Alloc>, IsECharT<Elem>> {
    SR_NODISCARD constexpr static size_t DoHash(const std::basic_string<Elem, std::char_traits<Elem>, Alloc>& keyVal) noexcept {
        return HashArrayRepresentation(keyVal.c_str(), keyVal.size());
    }
};

template <class Elem> struct SR_UTILS_NS::SRHash<std::basic_string_view<Elem>> : SR_UTILS_NS::SRConditionallyEnabledHash<std::basic_string_view<Elem>, IsECharT<Elem>> {
    SR_NODISCARD constexpr static size_t DoHash(const std::basic_string_view<Elem> keyVal) noexcept {
        return HashArrayRepresentation(keyVal.data(), keyVal.size());
    }
};

template<> struct SR_UTILS_NS::SRHash<float> {
    SR_NODISCARD constexpr size_t operator()(const float keyVal) const noexcept {
        return HashRepresentation(keyVal == 0.0F ? 0.0F : keyVal);
    }
};

template<> struct SR_UTILS_NS::SRHash<double> {
    SR_NODISCARD constexpr size_t operator()(const double keyVal) const noexcept {
        return HashRepresentation(keyVal == 0.0 ? 0.0 : keyVal);
    }
};

template<> struct SR_UTILS_NS::SRHash<long double> {
    SR_NODISCARD constexpr size_t operator()(const long double keyVal) const noexcept {
        return HashRepresentation(keyVal == 0.0L ? 0.0L : keyVal);
    }
};

template<> struct SR_UTILS_NS::SRHash<nullptr_t> {
    SR_NODISCARD constexpr size_t operator()(nullptr_t) const noexcept {
        void* null = nullptr;
        return HashRepresentation(null);
    }
};

#define SR_COMBINE_HASHES(x1, x2) (SR_UTILS_NS::CombineTwoHashes(x1, x2))

#define SR_GET_TYPE_NAME(x) SR_UTILS_NS::Hash::Detail::GetTypeName<x>()

#define SR_HASH(x) (SR_UTILS_NS::CalculateHash(x))
#define SR_HASH_STR(x) (SR_UTILS_NS::CalculateHash<std::string>(x))
#define SR_HASH_STR_VIEW(x) (SR_UTILS_NS::CalculateHash<std::string_view>(x))

#define SR_COMPILE_TIME_CRC32_STR(x) (static_cast<uint64_t>(SR_UTILS_NS::Hash::Detail::MM<sizeof(x)-1>::crc32(x)))
#define SR_RUNTIME_TIME_CRC32_STR(x) (static_cast<uint64_t>(SR_UTILS_NS::Hash::Detail::crc32(x)))

#define SR_COMPILE_TIME_CRC32_TYPE_NAME_DETAIL(x) (static_cast<uint64_t>(SR_UTILS_NS::Hash::Detail::MM<x.size()>::crc32(x.data())))

#define SR_COMPILE_TIME_CRC32_TYPE_NAME(x) (SR_COMPILE_TIME_CRC32_TYPE_NAME_DETAIL(SR_GET_TYPE_NAME(x)))
#define SR_RUNTIME_TIME_CRC32_TYPE_NAME(x) (SR_COMPILE_TIME_CRC32_TYPE_NAME_DETAIL(SR_GET_TYPE_NAME(x)))

#define SR_COMPILE_TIME_CRC32_STD_STR(x) (SR_COMPILE_TIME_CRC32_STR(x.c_str()))
#define SR_RUNTIME_TIME_CRC32_STD_STR(x) (SR_RUNTIME_TIME_CRC32_STR(x.c_str()))

#endif //SR_ENGINE_UTILS_HASHES_H
