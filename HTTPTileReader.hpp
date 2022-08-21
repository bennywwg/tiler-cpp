#pragma once

#include "ITileSource.hpp"
#include "Image.hpp"

#include <functional>

namespace Tiler {
    class HTTPTileReader : public ITileReader {
    public:
        enum class TileCoordFormatResult {
            Valid,
            Invalid
        };

        using TileCoordFormatFunction = std::function<TileCoordFormatResult(std::string&,TileCoord)>;

        TileCoordFormatFunction     URIFormatFunction;

        ImageType                   Type;

        HTTPTileReader(TileCoordFormatFunction const& InFormatFunction);

        virtual TileSourceResult ReadTileData(uint8_t* dest, TileCoord coord) override;
    };
}