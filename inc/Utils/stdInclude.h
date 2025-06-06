//
// Created by Monika on 07.04.2022.
//

#ifndef SR_ENGINE_STDINCLUDE_H
#define SR_ENGINE_STDINCLUDE_H

#include <Utils/macros.h>

#include <cfloat>
#include <span>
#include <limits>
#include <type_traits>
#include <cstdio>
#include <iosfwd>
#include <regex>
#include <stdexcept>
#include <string_view>
#include <shared_mutex>
#include <cstdarg>
#include <initializer_list>
#include <codecvt>
#include <cstddef>
#include <unordered_set>
#include <stack>
#include <cctype>
#include <locale>
#include <cstring>
#include <variant>
#include <optional>
#include <memory>
#include <fstream>
#include <vector>
#include <ostream>
#include <queue>
#include <mutex>
#include <string>
#include <cassert>
#include <cmath>
#include <ranges>
#include <atomic>
#include <utility>
#include <array>
#include <map>
#include <functional>
#include <set>
#include <exception>
#include <unordered_map>
#include <algorithm>
#include <any>
#include <thread>
#include <cstdlib>
#include <sstream>
#include <list>
#include <ctime>
#include <iostream>
#include <ratio>
#include <chrono>
#include <random>
#include <cstdint>
#include <iomanip>
#include <concepts>
#include <condition_variable>
#include <numeric>
#include <numbers>

#ifdef SR_SUPPORT_PARALLEL
    #include <omp.h>
#endif

#if !defined(SR_ANDROID) && defined(SR_CXX_20)
    #include <forward_list>
#endif

#ifdef SR_MINGW
    #include <iomanip>
#endif

#ifdef SR_LINUX
    #include <cstdarg>
    #include <sys/stat.h>
    #include <signal.h>
#endif

#if defined(SR_WIN32)
    #include <direct.h>
#endif

#undef min
#undef max

/// C++17 - 201703L
/// C++14 - 201402L
/// C++11 - 201103L
/// C++98 - 199711L

inline std::string_view SRGetClassName(std::string_view func_signature) {
    // Для GCC/Clang
#ifdef __GNUC__
    auto start = func_signature.find("] ") + 2;
    auto end = func_signature.find_last_of(";");
#else  // Для MSVC
    auto start = func_signature.find("SRGetClassName<") + 15;
    auto end = func_signature.find_last_of('>');
#endif
    return func_signature.substr(start, end - start);
}

#ifdef _MSC_VER
    #define SR_GET_CLASS_NAME() SRGetClassName(__FUNCSIG__)
#else
    #define SR_GET_CLASS_NAME() SRGetClassName(__PRETTY_FUNCTION__)
#endif

#define SR_GET_COMPILE_TIME_CLASS_NAME(T) SR_UTILS_NS::GetCompileTimeTypeName<T>()

#define SR_IGNORE_UNUSED(...) SR_UTILS_NS::IgnoreUnused(__VA_ARGS__)

namespace SR_UTILS_NS {
    using namespace std::literals::string_literals;
    using namespace std::literals::string_view_literals;

    template<size_t N1, size_t N2> constexpr auto CompileTimeConcatStrings(const char(&s1)[N1], const char(&s2)[N2]) {
        char result[N1 + N2 - 1] = {};
        for (size_t i = 0; i < N1 - 1; ++i) {
            result[i] = s1[i];
        }
        for (size_t i = 0; i < N2; ++i) {
            result[N1 - 1 + i] = s2[i];
        }
        return result;
    }

    template<typename T> constexpr const char* GetCompileTimeTypeName() {
    #ifdef _MSC_VER
        return __FUNCSIG__;
    #else
        return __PRETTY_FUNCTION__;
    #endif
    }

    template <typename... T> SR_CONSTEXPR void IgnoreUnused(const T&...) { }

    template <class Alloc, class ValueType>
    using RebindAllocT = typename std::allocator_traits<Alloc>::template rebind_alloc<ValueType>;

    using SRHashType = uint64_t;

    #ifdef SR_LINUX
        using TimePointType = std::chrono::time_point<std::chrono::system_clock>;
    #else
        #ifdef SR_MINGW
            using TimePointType = std::chrono::high_resolution_clock::time_point;
        #else
            using TimePointType = std::chrono::time_point<std::chrono::steady_clock>;
        #endif
    #endif

    template<class T, class U = T> SR_NODISCARD static SR_FORCE_INLINE T SR_FASTCALL Exchange(T& obj, U&& new_value) noexcept {
        T old_value = std::move(obj);
        obj = std::forward<U>(new_value);
        return old_value;
    }

    template<template<class> class T, class U>
    struct IsDerivedFrom {
    private:
        template<class V> static decltype(static_cast<const T<V>&>(std::declval<U>()), std::true_type{}) test(const T<V>&); /// NOLINT
        static std::false_type test(...);                                                                                   /// NOLINT

    public:
        static constexpr bool value = decltype(IsDerivedFrom::test(std::declval<U>()))::value;

    };

    template<typename T> constexpr bool IsVolatile() {
        return std::is_volatile<T>::value;
    }

    template<typename T> constexpr bool IsLogical() {
        if (!IsVolatile<T>()) {
            return IsLogical<volatile T>();
        }

        return std::is_same_v<T, volatile bool>;
    }
}

#define SR_EXCHANGE(x, y) SR_UTILS_NS::Exchange(x, y)

#if 0
    namespace std {
        template<class T, class U = T>
        T exchange(T &obj, U &&new_value) {
            T old_value = std::move(obj);
            obj = std::forward<U>(new_value);
            return old_value;
        }
    }
#endif

#endif //SR_ENGINE_STDINCLUDE_H
