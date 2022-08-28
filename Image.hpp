#pragma once

#include "TileData.hpp"

#include <vector>
#include <array>
#include <stdexcept>

namespace Tiler {
    template<size_t Size>
    struct PODLVal {
        uint8_t* data;

        void operator=(PODLVal<Size> const& rhs) {
            memcpy(data, rhs.data, Size);
        }
    };

    struct NumberType {
        bool        Signed = false; // Only true for integral types
        bool        Floating = false;
        uint32_t    NumBits = 8;
    };

    struct TexelType {
        NumberType  Type;
        uint32_t    NumChannels = 3;
    };

    struct ImageType {
        TexelType   Type;
        glm::uvec3  Size = glm::uvec3(1, 1, 1);

        inline bool IsRGB8() const {
            if (Size.z != 1) return false;
            if (Type.NumChannels != 3) return false;
            if (Type.Type.Signed != false) return false;
            if (Type.Type.Floating != false) return false;
            if (Type.Type.NumBits != 8) return false;
            return true;
        }
        inline bool IsRGBA8() const {
            if (Size.z != 1) return false;
            if (Type.NumChannels != 4) return false;
            if (Type.Type.Signed != false) return false;
            if (Type.Type.Floating != false) return false;
            if (Type.Type.NumBits != 8) return false;
            return true;
        }

        inline static ImageType RGB8(uint32_t Width, uint32_t Height) {
            return { { { false, false, 8 }, 3 }, glm::uvec3(Width, Height, 1) };
        }
    };

    enum class ImageResultStatus {
        Success,
        BadEncodeType,
        UnknownEncodeFailure  
    };

    struct Image {
        ImageType Type;

        ImageResultStatus Status;
        
        std::vector<uint8_t> Data;

        template<typename T>
        inline T& At(uint32_t x, uint32_t y, uint32_t z, uint32_t c = 0) {
            return *reinterpret_cast<T*>(&Data[
                (Type.Type.NumChannels * ((z * Type.Size.y + y) * Type.Size.x + x) + c)
                * (Type.Type.Type.NumBits / 8)
            ]);
        }

        template<typename T>
        inline const T& At(uint32_t x, uint32_t y, uint32_t z, uint32_t c = 0) const {
            return *reinterpret_cast<const T*>(&Data[
                (Type.Type.NumChannels * ((z * Type.Size.y + y) * Type.Size.x + x) + c)
                * (Type.Type.Type.NumBits / 8)
            ]);
        }

        template<typename T, uint32_t NumChannels>
        inline PODLVal<sizeof(T) * NumChannels> TexelAt(uint32_t x, uint32_t y, uint32_t z) {
            if (Type.Type.NumChannels != NumChannels) throw std::runtime_error("BAD");
            return { reinterpret_cast<uint8_t*>(&Data[
                ((z * Type.Size.y + y) * Type.Size.x + x)
                * Type.Type.NumChannels
                * (Type.Type.Type.NumBits / 8)
            ]) };
        }

        inline size_t RawSize() const {
            return
                static_cast<size_t>(Type.Size.x) *
                static_cast<size_t>(Type.Size.y) *
                static_cast<size_t>(Type.Size.z) *
                static_cast<size_t>(Type.Type.NumChannels) *
                (Type.Type.Type.NumBits / 8);
        }
    };

    struct ImageTypeBuilder {
        ImageType Type;

        inline ImageTypeBuilder& Size(uint32_t x, uint32_t y, uint32_t z = 1) {
            return Size(glm::uvec3(x, y, z));
        }

        inline ImageTypeBuilder& Size(glm::uvec3 const& Size) {
            Type.Size = Size;
            return *this;
        }

        template<typename T>
        inline ImageTypeBuilder& Texel() {
            Type.Type.Type.Floating = std::is_floating_point<T>::value;
            Type.Type.Type.Signed = !std::is_unsigned<T>::value;
            Type.Type.Type.NumBits = sizeof(T) * 8;
            return *this;
        }

        inline ImageTypeBuilder& Channels(uint32_t channels) {
            Type.Type.NumChannels = channels;
            return *this;
        }

        inline ImageType Build() const {
            return Type;
        }
    };

    Image EncodePNG(ImageType type, std::vector<uint8_t> const& data);
    //Image DecodePNG(std::vector<uint8_t> const& data, bool expand);
    //Image DecodeTIFF(std::vector<uint8_t> const& data);
}