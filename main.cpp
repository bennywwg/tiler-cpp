#include "HTTPTileReader.hpp"
#include "Image.hpp"
#include "Conversion.hpp"

#define FMT_HEADER_ONLY
#include <fmt/core.h>

#include <iostream>
#include <fstream>

using namespace Tiler;

int main(int argc, char** argv) {
    /*std::vector<uint8_t> data;
    for(int i = 0; i < 30000; ++i) {
        data.push_back(i % 256);
    }
    Image res = EncodePNG(ImageType::RGB8(100, 100), data);

    std::cout << "Size is " << res.Data.size() << "\n";

    std::ofstream file("myfile.png", std::ios::binary);
    file.write(reinterpret_cast<char*>(res.Data.data()), res.Data.size());*/

    
    TileReaderPtr reader(new HTTPTileReader([](std::string& res, TileCoord coord) -> HTTPTileReader::TileCoordFormatResult {
        if (coord.x < 0 || coord.y < 0 || coord.z < 0) {
            return HTTPTileReader::TileCoordFormatResult::Invalid;
        }
        res = fmt::format("https://spkit.org/datasets/srtm/remapped/{:03}_{:03}_{:03}.hgt", coord.x, coord.y, coord.z);
        return HTTPTileReader::TileCoordFormatResult::Valid;
    }));
    
    Image tile = {
        .Type = ImageTypeBuilder().Size(1201,1201).Texel<uint16_t>().Channels(1).Build(),
        .Data = std::vector<uint8_t>(2884802)
    };

    /*for (size_t i = 0; i < 2884802; ++i) {
        tile.Data[i] = (i + 500.0f) / 10000.0f;
    }*/

    //uint8_t* data = new uint8_t[2884802];

    reader->ReadTileData(tile.Data.data(), TileCoord(278, 133, 000));

    SwapEndian(tile);

    Image raw = MakePreview_u16(tile);

    std::cout << "Here\n";

    Image res = EncodePNG(ImageType::RGB8(1201, 1201), raw.Data);

    std::cout << "Size is " << res.Data.size() << "\n";

    std::ofstream file("myfile.png", std::ios::binary);
    file.write(reinterpret_cast<char*>(res.Data.data()), res.Data.size());

    return 0;
}