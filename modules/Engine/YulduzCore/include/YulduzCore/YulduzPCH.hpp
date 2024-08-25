#pragma once

#include <iostream>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <cstdarg>
#include <cstdio>
#include <cstdint>
#include <iomanip>
#include <source_location>
#include <vector>
#include <array>
#include <span>
#include <string>
#include <functional>
#include <algorithm>
#include <unordered_map>
#include <unordered_set>
#include <map>
#include <set>
#include <tuple>
#include <utility>
#include <chrono>
#include <memory>
#include <thread>
#include <format>
#include <type_traits>
#include <typeindex>
#include <typeinfo>

namespace fs = std::filesystem;

#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/quaternion.hpp>

#include <GLFW/glfw3.h>

#include <wgpu/wgpu_impl_glfw.h>
#include <wgpu/wgpu.h>

#if defined(_WIN32) || defined(_WIN64)
#define YULDUZ_PLATFORM_WINDOWS
#elif defined(__APPLE__) || defined(__MACH__)
#define YULDUZ_PLATFORM_OSX
#define YULDUZ_PLATFORM_UNIX
#elif defined(__linux__)
#define YULDUZ_PLATFORM_LINUX
#define YULDUZ_PLATFORM_UNIX
#else
#error Unsupported platform!
#endif

// YulduzCore configuration
#ifndef YULDUZ_SOURCE_DIR
#define YULDUZ_SOURCE_DIR ""
#endif

#ifndef YULDUZ_BUILD_TYPE_RELEASE
#define YULDUZ_BUILD_TYPE_DEBUG
#endif

#define YULDUZ_ENUM_CLASS_BITWISE_OPERATORS(Type, Other)                         \
    inline Type operator|(Type a, Type b) {                                      \
        return static_cast<Type>(static_cast<Other>(a) | static_cast<Other>(b)); \
    }                                                                            \
    inline Type operator|=(Type& a, Type b) {                                    \
        return a = a | b;                                                        \
    }                                                                            \
    inline Type operator&(Type a, Type b) {                                      \
        return static_cast<Type>(static_cast<Other>(a) & static_cast<Other>(b)); \
    }                                                                            \
    inline Type operator&=(Type& a, Type b) {                                    \
        return a = a & b;                                                        \
    }                                                                            \
    inline Type operator^(Type a, Type b) {                                      \
        return static_cast<Type>(static_cast<Other>(a) ^ static_cast<Other>(b)); \
    }                                                                            \
    inline Type operator^=(Type& a, Type b) {                                    \
        return a = a ^ b;                                                        \
    }                                                                            \
    inline Type operator~(Type a) {                                              \
        return static_cast<Type>(~static_cast<Other>(a));                        \
    }

#define YULDUZ_ENUM_UINT_BITWISE_OPERATORS(Type) YULDUZ_ENUM_CLASS_BITWISE_OPERATORS(Type, std::uint32_t)

namespace Yulduz {
    template <typename I, typename T>
    class SparseVector {
       private:
        template <bool IsConst>
        class IteratorImpl {
           public:
            using iterator_category = std::forward_iterator_tag;
            using value_type = T;
            using difference_type = std::ptrdiff_t;
            using pointer = typename std::conditional<IsConst, const T*, T*>::type;
            using reference = typename std::conditional<IsConst, const T&, T&>::type;
            using vector_iterator = typename std::conditional<IsConst,
                                                              typename std::vector<T>::const_iterator,
                                                              typename std::vector<T>::iterator>::type;

            IteratorImpl(vector_iterator it) : m_Current{it} {}

            reference operator*() const { return *m_Current; }
            pointer operator->() const { return &(*m_Current); }

            IteratorImpl& operator++() {
                ++m_Current;
                return *this;
            }

            IteratorImpl operator++(int) {
                IteratorImpl tmp = *this;
                ++(*this);
                return tmp;
            }

            bool operator==(const IteratorImpl& other) const { return m_Current == other.m_Current; }
            bool operator!=(const IteratorImpl& other) const { return m_Current != other.m_Current; }

           private:
            vector_iterator m_Current;
        };

       public:
        using Iterator = IteratorImpl<false>;
        using ConstIterator = IteratorImpl<true>;

        SparseVector() = default;
        ~SparseVector() = default;

        void add(const I& index, T&& data) {
            if (m_LookUp.find(index) != m_LookUp.end()) {
                throw std::runtime_error("Index already exists in SparseVector");
            }

            m_LookUp[index] = m_Data.size();
            m_Data.emplace_back(std::forward<T>(data));
            m_Indices.push_back(index);
        }

        void set(const I& index, T&& data) {
            auto it = m_LookUp.find(index);
            if (it != m_LookUp.end()) {
                m_Data[it->second] = std::forward<T>(data);
            } else {
                m_LookUp[index] = m_Data.size();
                m_Data.emplace_back(std::forward<T>(data));
                m_Indices.push_back(index);
            }
        }

        void remove(const I& index) {
            auto it = m_LookUp.find(index);
            if (it == m_LookUp.end()) return;

            const std::size_t i = it->second;
            const std::size_t lastI = m_Data.size() - 1;
            if (i != lastI) {
                m_Data[i] = std::move(m_Data.back());
                m_Indices[i] = std::move(m_Indices.back());
                m_LookUp[m_Indices[i]] = i;
            }

            m_LookUp.erase(index);
            m_Data.pop_back();
            m_Indices.pop_back();
        }

        bool contains(const I& index) const {
            return m_LookUp.find(index) != m_LookUp.end();
        }

        T* get(const I& index) {
            auto it = m_LookUp.find(index);
            if (it == m_LookUp.end()) return nullptr;
            return &m_Data[it->second];
        }

        const T* getConst(const I& index) const {
            auto it = m_LookUp.find(index);
            if (it == m_LookUp.end()) return nullptr;
            return &m_Data[it->second];
        }

        T& operator[](const I& index) {
            auto it = m_LookUp.find(index);
            if (it == m_LookUp.end()) {
                throw std::out_of_range("Index not found in SparseVector");
            }
            return m_Data[it->second];
        }

        const T& operator[](const I& index) const {
            auto it = m_LookUp.find(index);
            if (it == m_LookUp.end()) {
                throw std::out_of_range("Index not found in SparseVector");
            }
            return m_Data[it->second];
        }

        Iterator begin() { return Iterator(m_Data.begin()); }
        Iterator end() { return Iterator(m_Data.end()); }

        ConstIterator begin() const { return ConstIterator(m_Data.begin()); }
        ConstIterator end() const { return ConstIterator(m_Data.end()); }

        ConstIterator cbegin() const { return ConstIterator(m_Data.cbegin()); }
        ConstIterator cend() const { return ConstIterator(m_Data.cend()); }

        std::size_t size() const {
            return m_Data.size();
        }

       private:
        std::vector<T> m_Data;
        std::vector<I> m_Indices;
        std::unordered_map<I, std::size_t> m_LookUp;
    };
}  // namespace Yulduz