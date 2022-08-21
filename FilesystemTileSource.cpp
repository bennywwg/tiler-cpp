/*#include "FilesystemTileSource.hpp"
#include "Image.hpp"

#include <fstream>
#include <vector>

namespace Tiler {
    FilesystemTileSource::FilesystemTileSource(FilesystemTileSource::TileCoordFormatFunction const& InFormatFunction)
    : FormatFunction(InFormatFunction)
    { }
    FilesystemTileSource::Result FilesystemTileSource::ReadTileData(uint8_t* dest, TileCoord coord) {
        std::string URI;
        if(FormatFunction(URI, coord) == TileCoordFormatResult::Valid) {
            std::ifstream file(URI, std::ios::binary | std::ios::ate);
            std::streamsize size = file.tellg();
            file.seekg(0, std::ios::beg);

            std::vector<char> buffer(size);
            if (!file.read(buffer.data(), size))
            {
                return Result::Failure;
            }

            // TODO: Decompress file
        } else {
            return Result::Success;
        }
    }
    FilesystemTileSource::Result FilesystemTileSource::WriteTileData(TileCoord coord, const uint8_t* src) {
        
    }
}*/
