//
// Created by Nikita on 21.03.2021.
//

#ifndef SR_COMMON_MACROS_H
#define SR_COMMON_MACROS_H

#ifdef _WINDOWS_
    #error "Windows.h was included before macros.h"
#endif

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE

#if defined(_MSVC_LANG)
    static_assert(sizeof(size_t) == 8, "The engine supports only 64-bit systems!");

    /// TODO: убрать подавления и перенести туда где они нужны
    #pragma warning(disable: 4553)
    #pragma warning(disable: 4552)
    #pragma warning(disable: 5033)
    #pragma warning(disable: 4067)
    #pragma warning(disable: 4828)
#endif

#ifdef ANDROID
    #define SR_ANDROID
#endif

#ifdef __linux__
    #define SR_LINUX
#endif

#ifdef __clang__
    #define SR_CLANG
#endif

#ifdef SR_ANDROID
    #pragma clang diagnostic ignored "-Wunused-private-field"
    #pragma clang diagnostic ignored "-Wdeprecated-volatile"
    #pragma clang diagnostic ignored "-Wdefaulted-function-deleted"
    #pragma clang diagnostic ignored "-Winconsistent-missing-override"
#endif

#define CRT_SECURE_NO_WARNINGS

#ifndef _SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING
    #define _SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING
#endif

#ifndef _HAS_AUTO_PTR_ETC
    #define _HAS_AUTO_PTR_ETC 1
#endif

#define TRUE 1
#define FALSE 0

#ifdef _MSVC_LANG
    #define SR_MSVC
#endif

#ifdef __clang__
    #define SR_CLANG
#endif


#ifdef __GNUC__
    #define SR_GCC
#endif

#if defined(__MINGW64__) || defined(__MINGW32__)
    #define SR_MINGW
#endif

#ifdef __has_cpp_attribute
    #define SR_HAS_ATTRIBUTE __has_cpp_attribute
#else
    #define SR_HAS_ATTRIBUTE(x) (0)
#endif

#if SR_HAS_ATTRIBUTE(likely) || defined(SR_MSVC)
    #define SR_LIKELY_ATTRIBUTE [[likely]]
    #define SR_UNLIKELY_ATTRIBUTE [[unlikely]]
#else
    #define SR_LIKELY_ATTRIBUTE
    #define SR_UNLIKELY_ATTRIBUTE
#endif

#ifdef __GNUC__
    #define SR_LIKELY(x) __builtin_expect((x), 1)
    #define SR_UNLIKELY(x) __builtin_expect((x), 0)
#else
    #define SR_LIKELY(x) (x)
    #define SR_UNLIKELY(x) (x)
#endif

/// если использовать только один из, то Debug будет определяться,
/// но может возникнуть ситуация, что в разных частях движка будут неправильно работать макросы.
/// Привет, Visual Studio
/// #if defined(NDEBUG) || defined(_DEBUG)

#if defined(SR_MSVC)
    #define SR_MSC_VERSION _MSC_VER
    #if defined(_DEBUG)
        #define SR_DEBUG
    #elif defined(NDEBUG)
        #define SR_RELEASE
    #else
        #define SR_DEBUG
    #endif
#elif defined(SR_LINUX)
    #if defined(NDEBUG)
        #define SR_RELEASE
    #else
        #define SR_DEBUG
    #endif
#elif defined(SR_MINGW)
    #if defined(NDEBUG)
        #define SR_RELEASE
    #else
        #define SR_DEBUG
    #endif
#else
    #ifdef SR_ANDROID
        #define SR_RELEASE /// TODO: wtf
    #endif
#endif

#define SR_SAFE_DELETE_PTR(ptr) \
    if (ptr) {                  \
        delete ptr;             \
        ptr = nullptr;          \
    }                           \

#define SR_SAFE_DELETE_ARRAY_PTR(ptr) \
    if (ptr) {                        \
        delete[] ptr;                 \
        ptr = nullptr;                \
    }                                 \

#define SR_COMBINE_Utils(X, Y) X##Y
#define SR_COMBINE(X, Y) SR_COMBINE_Utils(X, Y)
#define SR_FASTCALL_ATTRIBUTE __attribute__((fastcall))
#define SR_CONSTEXPR constexpr
#define SR_INLINE inline

#if defined(SR_ANDROID)
    #define SR_FASTCALL
    #define SR_FORCE_INLINE SR_INLINE
#elif defined(SR_GCC)
    #define SR_FASTCALL
    #define SR_FORCE_INLINE SR_INLINE
#else
    #define SR_FASTCALL __fastcall
    #define SR_FORCE_INLINE __forceinline
#endif

#define SR_CLOCKS_PER_SEC 1000

#ifdef SR_GCC
    #define SR_NODISCARD __attribute__ ((__warn_unused_result__))
#else
    #define SR_NODISCARD [[nodiscard]]
#endif

#define SR_FALLTHROUGH [[fallthrough]]
#define SR_MAYBE_UNUSED [[maybe_unused]]
#define SR_DEPRECATED_EX(text) [[deprecated(text)]]
#define SR_DEPRECATED [[deprecated]]
#define SR_MAYBE_UNUSED_VAR [[maybe_unused]] auto&& SR_COMBINE(unused_var, __LINE__) =
#define SR_INLINE_STATIC SR_INLINE static
#define SR_NULL 0
#define SR_MARSHAL_USE_LIST 1
#define SR_MARSHAL_ENCODE_AND_DECODE 0
#define SR_INVALID_STR_POS (-1)
#define SR_ID_INVALID (-1)
#define SR_SHADER_PROGRAM int32_t
#define SR_NULL_SHADER (-1)
#define SR_VERTEX_DESCRIPTION size_t
#define GLM_ENABLE_EXPERIMENTAL
#define SR_NOOP (void)0
#define SR_FLT_EPSILON FLT_EPSILON
#define SR_NORETURN [[noreturn]]

#define SR_MAX_BONES_ON_VERTEX 16
#define SR_HUMANOID_MAX_BONES 128

#define SR_FAST_CONSTRUCTOR SR_FORCE_INLINE SR_CONSTEXPR

#ifdef SR_USE_VULKAN
    #define VK_PROTOTYPES
#endif

#ifdef SR_USE_OPENGL
    #define IMGUI_IMPL_OPENGL_LOADER_GLFW
    //#define GL_GLEXT_PROTOTYPES
    //#define GL_VERSION_1_0
    //#define GL_VERSION_1_1
    //#define GL_VERSION_1_3
#endif

#define SR_UNUSED_VARIABLE(x) do { (void)(x); } while (0)

#ifdef WIN32
    #define SR_WIN32_BOOL true
    #define SR_WIN32
    #define WIN32_WINNT 0x0A00
    #define WIN32_LEAN_AND_MEAN /// Исключите редко используемые компоненты из заголовков Windows
    #define GLFW_EXPOSE_NATIVE_WIN32
    #define VK_USE_PLATFORM_WIN32_KHR
#else
    #define SR_WIN32_BOOL false
#endif

#ifdef WIN32
    #define SR_SIMD_SUPPORT 1
#else
    #define SR_SIMD_SUPPORT 0
#endif

#define SR_MACRO_CONCAT_UTIL(a, b) a ## b
#define SR_MACRO_CONCAT(a, b) SR_MACRO_CONCAT_UTIL(a, b)

#define SR_LINE __LINE__

#define SR_AF_INET 2 /// internetwork: UDP, TCP, etc.
#define SR_SOCK_STREAM 1 /// stream socket
#define SR_INADDR_ANY (ULONG)0x00000000

#define SR_XML_NS SpaRcle::Utils::Xml
#define SR_YAML_NS SpaRcle::Utils::Yaml
#define SR_PHYSICS_NS SpaRcle::Physics
#define SR_PTYPES_NS SR_PHYSICS_NS::Types
#define SR_PHYSICS_UTILS_NS SR_PHYSICS_NS::Utils
#define SR_UTILS_NS SpaRcle::Utils
#define SR_SRLM_NS SR_UTILS_NS::SRLM
#define SR_NETWORK_NS SR_UTILS_NS::Network
#define SR_PLATFORM_NS SpaRcle::Utils::Platform
#define SR_MATH_NS SpaRcle::Utils::Math
#define SR_GRAPH_NS SpaRcle::Graphics
#define SR_SRSL_NS SpaRcle::Graphics::SRSL2
#define SR_GRAPH_UI_NS SpaRcle::Graphics::UI
#define SR_GRAPH_GUI_NS SpaRcle::Graphics::GUI
#define SR_ANIMATIONS_NS SpaRcle::Graphics::Animations
#define SR_HTYPES_NS SR_UTILS_NS::Types
#define SR_GTYPES_NS SR_GRAPH_NS::Types
#define SR_WORLD_NS SpaRcle::Utils::World
#define SR_CORE_NS SpaRcle::Core
#define SR_CORE_UI_NS SpaRcle::Core::UI
#define SR_CORE_GUI_NS SpaRcle::Core::GUI
#define SR_SCRIPTING_NS SpaRcle::Scripting
#define SR_AUDIO_NS SpaRcle::Audio
#define SR_UTILS_GUI_NS SR_UTILS_NS::GUI

#define SR_COUNT_ARGS_IMPL2(                                                                                            \
    _1, _2, _3, _4, _5, _6, _7, _8, _9, _10,                                                                            \
    _11, _12, _13, _14, _15, _16, _17, _18, _19, _20,                                                                   \
    _21, _22, _23, _24, _25, _26, _27, _28, _29, _30,                                                                   \
    _31, _32, _33, _34, _35, _36, _37, _38, _39, _40,                                                                   \
    _41, _42, _43, _44, _45, _46, _47, _48, _49, _50,                                                                   \
    _51, _52, _53, _54, _55, _56, _57, _58, _59, _60,                                                                   \
    _61, _62, _63, _64, N, ...) N

#define SR_COUNT_ARGS_IMPL(...) SR_COUNT_ARGS_IMPL2(__VA_ARGS__)
#define SR_COUNT_ARGS(...) SR_COUNT_ARGS_IMPL(__VA_ARGS__,                                                              \
    64, 63, 62, 61, 60, 59, 58, 57, 56, 55, 54, 53, 52, 51,                                                             \
    50, 49, 48, 47, 46, 45, 44, 43, 42, 41, 40, 39, 38, 37,                                                             \
    36, 35, 34, 33, 32, 31, 30, 29, 28, 27, 26, 25, 24, 23,                                                             \
    22, 21, 20, 19, 18, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8,                                                           \
    7, 6, 5, 4, 3, 2, 1, 0)

#define SR_GLOBAL_LOCK static std::mutex codegenGlobalMutex##__LINE__; std::lock_guard<std::mutex> codegenLock##__LINE__(codegenGlobalMutex##__LINE__);

#define SR_STATIC_ASSERT2(expr, msg) static_assert(expr, msg)

#if defined(SR_MINGW) || (SR_MSC_VERSION > 1929) || defined(SR_ANDROID) || defined(SR_LINUX)
    #define SR_STATIC_ASSERT(msg) static_assert(msg)
#else
    #define SR_STATIC_ASSERT(msg) static_assert(false, msg)
#endif

#ifdef SR_LINUX
    #define SR_DLL_EXPORT
#else
    #ifdef SR_DLL_EXPORTS
        #ifdef SR_ANDROID
            #define SR_DLL_EXPORT
        #else
            #define SR_DLL_EXPORT __declspec(dllexport)
        #endif
    #else
        #ifdef SR_ANDROID
            #define SR_DLL_EXPORT
        #else
            #define SR_DLL_EXPORT __declspec(dllimport)
        #endif
    #endif
#endif

#if defined(SR_MSVC)
    #define SR_STRCMPI _strcmpi
    #define SR_STRNCPY strncpy_s
#else
    #define SR_STRCMPI strcasecmp
    #define SR_STRNCPY strncpy
#endif

#ifdef SR_MSVC
    #define SR_FUNCTION_SIGNATURE __FUNCSIG__
#else
#define SR_FUNCTION_SIGNATURE __PRETTY_FUNCTION__
#endif

#define SR_OFFSETOF(s,m) ((::size_t)&reinterpret_cast<char const volatile&>((((s*)0)->m)))

#endif //SR_COMMON_MACROS_H
