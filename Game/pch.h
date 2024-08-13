//
// pch.h
// Header for standard system include files.
//

#pragma once
#pragma warning(disable : 4996)

#include <winsdkver.h>
#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0601
#endif
#include <sdkddkver.h>

// Use the C++ standard templated min/max
#define NOMINMAX

// Include <mcx.h> if you need this
#define NOMCX

// Include <winsvc.h> if you need this
#define NOSERVICE

// WinHelp is deprecated
#define NOHELP

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <windowsx.h>

#include <wrl/client.h>

#include <d3d11_1.h>
#include <dxgi1_6.h>

#include <DirectXMath.h>
#include <DirectXColors.h>
#include <DirectXTex.h>

#include <dwrite.h>
#include <d2d1.h>

#include <algorithm>
#include <cmath>
#include <codecvt>
#include <cstdint>
#include <cstdio>
#include <cwchar>
#include <exception>
#include <iterator>
#include <memory>
#include <stdexcept>
#include <system_error>
#include <tuple>
#include <comdef.h>
#include <SimpleMath.h>

#ifdef _DEBUG
#include <dxgidebug.h>
#endif

using Microsoft::WRL::ComPtr;

#include <cstdint>
#include <memory>
#include <optional>
#include <string>
#include <limits>
#include <filesystem>
#include <fstream>
#include <optional>

#define CAST static_cast
#define RCAST reinterpret_cast
#define CCAST const_cast
#define DCAST dynamic_cast

using u8   = uint8_t;
using u16  = uint16_t;
using u32  = uint32_t;
using u64  = uint64_t;
using i8   = int8_t;
using i16  = int16_t;
using i32  = int32_t;
using i64  = int64_t;
using f32  = float;
using f64  = double;
using wstr = std::wstring;
using str  = std::string;

using Exception    = std::exception;
using RuntimeError = std::runtime_error;

namespace FileSystem = std::filesystem;
using Path = std::filesystem::path;

static constexpr std::nullopt_t kNone = std::nullopt;
static constexpr auto None            = nullptr;
static constexpr auto Ignore          = std::ignore;

template<class T>
using Shared = std::shared_ptr<T>;

template<class T>
using Option = std::optional<T>;

template<class T>
using Unique = std::unique_ptr<T>;

template<class T>
using Vector = std::vector<T>;

constexpr auto Inf32 = std::numeric_limits<float>::infinity();
constexpr auto Inf64 = std::numeric_limits<double>::infinity();

using DirectX::SimpleMath::Vector2;
using DirectX::SimpleMath::Vector3;
using DirectX::SimpleMath::Vector4;
using DirectX::SimpleMath::Matrix;

namespace IO {
    inline Option<str> Read(const Path& filename) {
        if (!exists(filename) || is_directory(filename)) {
            return kNone;
        }

        std::ifstream file(filename);
        if (!file.is_open()) {
            return kNone;
        }

        str content((std::istreambuf_iterator(file)), std::istreambuf_iterator<char>());
        file.close();

        return content;
    }

    inline Option<Vector<u8>> ReadAllBytes(const Path& filename) {
        if (!exists(filename) || is_directory(filename)) {
            return kNone;
        }

        std::ifstream file(filename, std::ios::binary);
        if (!file.is_open()) {
            return kNone;
        }

        Vector<u8> bytes(std::istreambuf_iterator(file), {});
        file.close();

        return bytes;
    }

    inline Option<Vector<str>> ReadAllLines(const Path& filename) {
        if (!exists(filename) || is_directory(filename)) {
            return kNone;
        }

        std::ifstream file(filename);
        if (!file.is_open()) {
            return kNone;
        }

        Vector<str> lines;
        str line;
        while (std::getline(file, line)) {
            lines.push_back(line);
        }

        file.close();

        return lines;
    }

    inline Option<Vector<u8>>
    ReadBlock(const Path& filename, const u32 blockOffset, const size_t blockSize) {
        if (!exists(filename) || is_directory(filename)) {
            return kNone;
        }

        std::ifstream file(filename, std::ios::binary);
        if (!file.is_open()) {
            return kNone;
        }

        file.seekg(blockOffset, std::ios::beg);
        if (!file.good()) {
            return kNone;
        }

        Vector<u8> buffer(blockSize);
        file.read(RCAST<char*>(buffer.data()), CAST<std::streamsize>(blockSize));

        if (file.gcount() != CAST<std::streamsize>(blockSize)) {
            return kNone;
        }

        return buffer;
    }

    inline bool Write(const Path& filename, const str& content) {
        std::ofstream outfile(filename);
        if (!outfile.is_open()) {
            return false;
        }

        outfile.write(content.c_str(), CAST<std::streamsize>(content.length()));
        outfile.close();

        return true;
    }

    inline bool WriteAllBytes(const Path& filename, const Vector<u8>& bytes) {
        std::ofstream outfile(filename, std::ios::binary);
        if (!outfile.is_open()) {
            return false;
        }

        outfile.write(RCAST<const char*>(bytes.data()), CAST<std::streamsize>(bytes.size()));
        outfile.close();

        return true;
    }

    inline bool WriteAllLines(const Path& filename, const Vector<str>& lines) {
        std::ofstream outfile(filename);
        if (!outfile.is_open()) {
            return false;
        }

        for (const auto& line : lines) {
            outfile << line << std::endl;
        }

        outfile.close();

        return true;
    }
} // namespace IO

namespace Utilities {
    inline void DebugPrint(const char* fmt, ...) {
        char buffer[256];
        va_list args;
        va_start(args, fmt);
        vsnprintf(buffer, sizeof(buffer), fmt, args);
        va_end(args);
        OutputDebugStringA(buffer);
    }

    template<typename T>
    void RemoveAt(std::vector<T>& vec, i32 idx) {
        if (idx >= 0 && idx < vec.size()) {
            vec.erase(vec.begin() + idx);
        }
    }
}

namespace DX {
    class ComError final : public std::exception {
    public:
        explicit ComError(HRESULT hr) noexcept : result(hr) {}

        [[nodiscard]] const char* what() const noexcept override {
            const _com_error err(result);
            static char errMsg[256] = {};
            sprintf_s(errMsg,
                      "Failure with HRESULT of %08X.\nError: %ws\n",
                      CAST<u32>(result),
                      err.ErrorMessage());
            return errMsg;
        }

    private:
        HRESULT result;
    };

    // Helper utility converts D3D API failures into exceptions.
    inline void ThrowIfFailed(HRESULT hr) {
        if (FAILED(hr)) {
            throw ComError(hr);
        }
    }
} // namespace DX

inline void WideToANSI(const std::wstring& value, std::string& converted) {
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
    converted = converter.to_bytes(value);
}

inline void ANSIToWide(const std::string& value, std::wstring& converted) {
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
    converted = converter.from_bytes(value);
}