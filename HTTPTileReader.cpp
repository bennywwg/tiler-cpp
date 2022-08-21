#include "HTTPTileReader.hpp"
#include "Image.hpp"
#include "ReadURI.hpp"

#include <fstream>
#include <vector>

namespace Tiler {
    HTTPTileReader::HTTPTileReader(HTTPTileReader::TileCoordFormatFunction const& InFormatFunction)
    : URIFormatFunction(InFormatFunction)
    { }
    TileSourceResult HTTPTileReader::ReadTileData(uint8_t* dest, TileCoord coord) {
        std::string URI;
        if(URIFormatFunction(URI, coord) == TileCoordFormatResult::Valid) {
            std::vector<uint8_t> buffer = LoadEntireURIBinary(URI);

            if (buffer.empty()) {
                return TileSourceResult::Failure;
            }
            
            memcpy(dest, buffer.data(), buffer.size());

            return TileSourceResult::Success;
        } else {
            return TileSourceResult::Failure;
        }
    }
}
