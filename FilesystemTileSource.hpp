/*#include "ITileSource.hpp"

#include <functional>

namespace Tiler {
    class FilesystemTileSource : public ITileSource {
    public:
        enum class TileCoordFormatResult {
            Valid,
            Invalid
        };

        using TileCoordFormatFunction = std::function<TileCoordFormatResult(std::string&,TileCoord)>;

        TileCoordFormatFunction     FormatFunction;

        ImageType                   Type;

        FilesystemTileSource(TileCoordFormatFunction const& InFormatFunction);

        virtual Result ReadTileData(uint8_t* dest, TileCoord coord) override;
        virtual Result WriteTileData(TileCoord coord, const uint8_t* src) override;
    };
}*/