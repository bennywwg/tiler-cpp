#include "Image.hpp"

#include <iostream>
#include <algorithm>

namespace Tiler {
    template<typename T>
    struct LowerUpperBounds {
        T Lower, Upper;

        template<typename U>
        inline LowerUpperBounds<U> Cast() const {
            return { static_cast<U>(Lower), static_cast<U>(Upper) };
        }
    };

    struct BucketHistogram {
        std::vector<size_t> Buckets;
        size_t TotalSamples = 0;

        inline void AddSample(double val) {
            ++TotalSamples;
            ++Buckets[glm::clamp<size_t>(static_cast<size_t>(val * Buckets.size()), 0, Buckets.size() - 1)];
        }

        // TODO: Check the math
        inline double FindPercent(double percent) {
            const size_t samplesThreshold = glm::clamp<size_t>(static_cast<size_t>(percent * TotalSamples), 0, TotalSamples);
            size_t samplesSearched = 0;
            for (size_t i = 0; i < Buckets.size(); ++i) {
                if (samplesSearched == samplesThreshold) {
                    return i / static_cast<double>(Buckets.size());
                } else if (samplesSearched + Buckets[i] < samplesThreshold) {
                    samplesSearched += Buckets[i];
                } else {
                    return (i + (samplesThreshold - samplesSearched) / static_cast<double>(Buckets[i]))
                    / static_cast<double>(Buckets.size());
                }
            }    
            return 1.0;
        }
    };

    glm::vec3 colorMap(float scalar) {
        // From "Why we use bad color maps and what you can do about it" (Kenneth Moreland)
        // Page 5, Figure 8
        std::vector<glm::vec3> colors = {
            glm::vec3(  0.0f,   0.0f,   0.0f) / 255.0f,
            glm::vec3(  0.0f,  24.0f, 168.0f) / 255.0f,
            glm::vec3( 99.0f,   0.0f, 228.0f) / 255.0f,
            glm::vec3(220.0f,  20.0f,  60.0f) / 255.0f,
            glm::vec3(255.0f, 117.0f,  56.0f) / 255.0f,
            glm::vec3(238.0f, 210.0f,  20.0f) / 255.0f,
            glm::vec3(255.0f, 255.0f, 255.0f) / 255.0f
        };
        std::vector<float> positions = { 0.0f, 0.22f, 0.35f, 0.47f, 0.65f, 0.84f, 1.0f };
        if (scalar <= 0.0) {
            return colors[0];
        } else if (scalar >= 1.0) {
            return colors.back();
        }
        for (size_t i = 0; i < colors.size(); ++i) {
            if (scalar >= positions[i] && scalar < positions[i + 1]) {
                return glm::mix(colors[i], colors[i + 1], (scalar - positions[i]) / (positions[i + 1] - positions[i]));
            }
        }
        return colors.back();
    }

    glm::uvec3 castFloat3(glm::vec3 color) {
        return glm::uvec3(
            glm::clamp<uint32_t>(static_cast<uint32_t>(color.x * 255.0f), 0, 255),
            glm::clamp<uint32_t>(static_cast<uint32_t>(color.y * 255.0f), 0, 255),
            glm::clamp<uint32_t>(static_cast<uint32_t>(color.z * 255.0f), 0, 255)
        );
    }

    template<typename T>
    LowerUpperBounds<T> ComputeUpperLower(Image const& img) {
        LowerUpperBounds<T> res;
        if constexpr (std::is_integral<T>::value) {
            res.Lower = std::numeric_limits<T>::max();
            res.Upper = std::numeric_limits<T>::min();
        } else {
            res.Lower = std::numeric_limits<T>::max();
            res.Upper = -std::numeric_limits<T>::max();
        }

        const size_t texelStride = img.Type.Type.NumChannels * (img.Type.Type.Type.NumBits / 8);
        const size_t xyStride = img.Type.Size.x * img.Type.Size.y * texelStride;
        const size_t yStride = img.Type.Size.x * texelStride;
        for (uint32_t z = 0; z < img.Type.Size.z; ++z) {
            size_t zOffset = z * xyStride;
            for (uint32_t y = 0; y < img.Type.Size.y; ++y) {
                size_t yOffset = zOffset + y * yStride;
                for (uint32_t x = 0; x < img.Type.Size.x; ++x) {
                    T const& val = *reinterpret_cast<const T*>(&img.Data[x + yOffset]);
                    if (val < res.Lower) res.Lower = val;
                    if (val > res.Upper) res.Upper = val;
                }
            }   
        }
        
        return res;
    }

    template<typename T>
    LowerUpperBounds<T> ComputeUpperLowerGeneric(Image const& img) {
        LowerUpperBounds<T> res;

        if (img.Type.Type.Type.Floating) {
            if (img.Type.Type.Type.NumBits == 32) {
                res = ComputeUpperLower<float>(img).Cast<T>();
            } else if (img.Type.Type.Type.NumBits == 64) {
                res = ComputeUpperLower<double>(img).Cast<T>();
            } 
        } else {
            if (img.Type.Type.Type.Signed) {
                if (img.Type.Type.Type.NumBits == 8) {
                    res = ComputeUpperLower<int8_t>(img).Cast<T>();
                } else if (img.Type.Type.Type.NumBits == 16) {
                    res = ComputeUpperLower<int16_t>(img).Cast<T>();
                } else if (img.Type.Type.Type.NumBits == 32) {
                    res = ComputeUpperLower<int32_t>(img).Cast<T>();
                } else if (img.Type.Type.Type.NumBits == 64) {
                    res = ComputeUpperLower<int64_t>(img).Cast<T>();
                }
            } else {
                if (img.Type.Type.Type.NumBits == 8) {
                    res = ComputeUpperLower<uint8_t>(img).Cast<T>();
                } else if (img.Type.Type.Type.NumBits == 16) {
                    res = ComputeUpperLower<uint16_t>(img).Cast<T>();
                } else if (img.Type.Type.Type.NumBits == 32) {
                    res = ComputeUpperLower<uint32_t>(img).Cast<T>();
                } else if (img.Type.Type.Type.NumBits == 64) {
                    res = ComputeUpperLower<uint64_t>(img).Cast<T>();
                }
            }
        }

        return res;
    }

    template<typename InType, typename OutType, uint32_t OutNumChannels>
    Image MapImage(Image const& img, std::function<std::array<OutType, OutNumChannels>(const InType*)> const& mapFunc) {
        Image res;
        res.Type
            =ImageTypeBuilder()
            .Size(img.Type.Size)
            .Texel<OutType>()
            .Channels(OutNumChannels)
            .Build();
        res.Data.resize(res.RawSize());

        for (uint32_t z = 0; z < img.Type.Size.z; ++z) {
        for (uint32_t y = 0; y < img.Type.Size.y; ++y) {
        for (uint32_t x = 0; x < img.Type.Size.x; ++x) {
            auto rhs = mapFunc(&img.At<InType>(x, y, z));
            res.TexelAt<OutType, OutNumChannels>(x, y, z) = PODLVal<sizeof(OutType) * OutNumChannels> { reinterpret_cast<uint8_t*>(&rhs)};
        } } }

        return res;
    }

    template<typename InType>
    void IterateImage(Image const& img, std::function<void(const InType*)> const& itFunc) {
        for (uint32_t z = 0; z < img.Type.Size.z; ++z) {
        for (uint32_t y = 0; y < img.Type.Size.y; ++y) {
        for (uint32_t x = 0; x < img.Type.Size.x; ++x) {
            itFunc(&img.At<InType>(x, y, z));
        } } }
    }

    Image MakePreview_u16(Image const& img) {
        auto bounds = ComputeUpperLowerGeneric<float>(img);

        BucketHistogram hist;
        hist.Buckets.resize(1000);

        IterateImage<uint16_t>(img, [bounds, &hist](const uint16_t* inTexel) {
            float val = (static_cast<float>(*inTexel) - bounds.Lower) / (bounds.Upper - bounds.Lower);
            hist.AddSample(val);
        });

        std::cout << "bounds are " << bounds.Lower << ", " << bounds.Upper << "\n";

        double oldLower = bounds.Lower;
        bounds.Lower = static_cast<float>(hist.FindPercent(0.01) * (bounds.Upper - oldLower) + oldLower);
        bounds.Upper = static_cast<float>(hist.FindPercent(0.99) * (bounds.Upper - oldLower) + oldLower);
        std::cout << "new bounds are " << bounds.Lower << ", " << bounds.Upper << "\n";

        return MapImage<uint16_t, uint8_t, 3>(img, [bounds](const uint16_t* inTexel) -> std::array<uint8_t, 3> {
            float val = (static_cast<float>(*inTexel) - bounds.Lower) / (bounds.Upper - bounds.Lower);
            auto mapped = castFloat3(colorMap(val));
            return {
                static_cast<uint8_t>(mapped.x),
                static_cast<uint8_t>(mapped.y),
                static_cast<uint8_t>(mapped.z)
            };
        });
    }

    void SwapEndian(Image& img) {
        const size_t bytes = img.Type.Type.Type.NumBits / 8;
        for (size_t i = 0; i < img.Data.size(); i += bytes) {
            for (size_t b = 0; b < bytes / 2; ++b) {
                std::swap(img.Data[i + b], img.Data[i + bytes - 1 - b]);
            }
        }
    }
}