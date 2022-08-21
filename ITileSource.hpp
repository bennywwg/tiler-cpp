#include "TileData.hpp"

#include <memory>

namespace Tiler {
    enum class TileSourceResult {
        Success,
        Failure
    };

    class ITileReader {
    public:
        virtual TileSourceResult ReadTileData(uint8_t* dest, TileCoord coord) = 0;
    };

    class ITileWriter {
    public:
        virtual TileSourceResult WriteTileData(TileCoord coord, const uint8_t* src) = 0;
    };

    class ITileSource : public ITileReader, public ITileWriter { };

    using TileReaderPtr = std::shared_ptr<ITileReader>;
    using TileWriterPtr = std::shared_ptr<ITileWriter>;
    using TileSourcePtr = std::shared_ptr<ITileSource>;
}