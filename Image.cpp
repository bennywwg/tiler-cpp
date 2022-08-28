#include "Image.hpp"

#include <tiffio.h>
#include <png.h>

#include <sstream>

namespace Tiler {

    /*
    Image LoadPNG(std::vector<uint8_t> const& data, bool expand) {
        if (data.size() < 8) return Image();

        if (png_sig_cmp(data.data(), 0, 8)) {
            //tkLog("png_sig_cmp failed");
            return Image();
        }

        png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);

        if (!png_ptr) {
            //tkLog("png_create_read_struct failed");
            return Image();
        }

        png_infop info_ptr = png_create_info_struct(png_ptr);
        if (!info_ptr) {
            //tkLog("png_create_info_struct failed");
            png_destroy_read_struct(&png_ptr, NULL, NULL);
            return Image();
        }

        if (setjmp(png_jmpbuf(png_ptr))) {
            //tkLog("Error during init_io");
            png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
            return Image();
        }

        pngMemoryReader memoryReader { data.data(), 8 };

        png_set_read_fn(png_ptr, &memoryReader, pngReadFunc);
        png_set_sig_bytes(png_ptr, 8);

        png_read_info(png_ptr, info_ptr);

        if (png_get_color_type(png_ptr, info_ptr) == PNG_COLOR_TYPE_PALETTE) {
            //png_set_palette_to_rgb(png_ptr);
        }

        Image res;

        png_set_interlace_handling(png_ptr);
        if(expand) png_set_expand(png_ptr);
        png_read_update_info(png_ptr, info_ptr);
        
        res.bitDepth = png_get_bit_depth(png_ptr, info_ptr);
        res.numChannels = png_get_channels(png_ptr, info_ptr);
        res.width = png_get_image_width(png_ptr, info_ptr);
        res.height = png_get_image_height(png_ptr, info_ptr);

        if (res.bitDepth == 0) {
            //tkLog("bitDepth was 0");
            png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
            return Image();
        }

        //tkAssert(res.bitDepth == 8 || res.bitDepth == 16);

        // set error handler with... jumps? yikes.
        if (setjmp(png_jmpbuf(png_ptr))) {
            //tkLog("Error during read_image");
            png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
            return Image();
        }

        res.data.resize(res.width * res.height * res.numChannels * res.bitDepth / 8);

        std::vector<png_bytep> rowPointers;
        for (int row = 0; row < res.height; ++row) {
            rowPointers.push_back(res.data.data() + row * res.numChannels * (res.bitDepth / 8) * res.width);
        }

        png_read_image(png_ptr, rowPointers.data());

        png_destroy_read_struct(&png_ptr, &info_ptr, NULL);

        return res;
    }

    */

    struct pngMemoryReader {
        uint8_t const* dataStart;
        uint64_t currentPos;
    };

    void pngReadFunc(png_structp png_ptr, png_bytep outBytes, png_size_t byteCountToRead) {
        png_voidp io_ptr = png_get_io_ptr(png_ptr);
        //tkAssert(io_ptr);

        pngMemoryReader& status = *reinterpret_cast<pngMemoryReader*>(io_ptr);

        memcpy(outBytes, status.dataStart + status.currentPos, byteCountToRead);

        status.currentPos += byteCountToRead;
    }

    void pngWriteFunc(png_structp png_ptr, png_bytep inBytes, png_size_t byteCountToRead) {
        png_voidp io_ptr = png_get_io_ptr(png_ptr);
        //tkAssert(io_ptr);

        std::vector<uint8_t>& data = *reinterpret_cast<std::vector<uint8_t>*>(io_ptr);

        size_t oldSize = data.size();
        data.resize(oldSize + byteCountToRead);

        memcpy(data.data() + oldSize, inBytes, byteCountToRead);
    }

    void pngFlushFunc(png_structp) { }

    Image EncodePNG(ImageType type, std::vector<uint8_t> const& data) {
        int y;

        png_structp png = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
        if (!png) abort();

        png_infop info = png_create_info_struct(png);
        if (!info) abort();

        if (setjmp(png_jmpbuf(png))) abort();

        Image res;
        res.Type = type;

        if (!type.IsRGB8()) {
            res.Status = ImageResultStatus::BadEncodeType;
            return res;
        }

        //pngMemoryReader reader { data.data(), 0 };

        //png_set_read_fn(png, &reader, pngReadFunc);
        png_set_write_fn(png, &res.Data, pngWriteFunc, pngFlushFunc);

        // Output is 8bit depth, RGBA format.
        png_set_IHDR(
            png,
            info,
            type.Size.x, type.Size.y,
            8,
            PNG_COLOR_TYPE_RGB,
            PNG_INTERLACE_NONE,
            PNG_COMPRESSION_TYPE_DEFAULT,
            PNG_FILTER_TYPE_DEFAULT
        );
        png_write_info(png, info);

        // To remove the alpha channel for PNG_COLOR_TYPE_RGB format,
        // Use png_set_filler().
        //png_set_filler(png, 0, PNG_FILLER_AFTER);

        std::vector<png_bytep> rowPointers;
        rowPointers.resize(type.Size.y);
        for (uint32_t i = 0; i < rowPointers.size(); ++i) {
            rowPointers[i] = const_cast<uint8_t*>(&data[i * type.Size.x * 3]);
        }

        png_write_image(png, const_cast<uint8_t**>(rowPointers.data()));
        png_write_end(png, NULL);

        png_destroy_write_struct(&png, &info);

        return res;
    }

    /*Image DecodeTIFF(std::vector<uint8_t> const& data) {
        std::istringstream input_TIFF_stream;
        

        TIFF* tif = TIFFStreamOpen("TifFromMem", &input_TIFF_stream);

        size_t directoryCount = 0;
        while(TIFFReadDirectory(tif)) {
            ++directoryCount;
        }

        uint32_t width, height, bps;
        TIFFGetField(tif, TIFFTAG_IMAGEWIDTH, &width);
	    TIFFGetField(tif, TIFFTAG_IMAGELENGTH, &height);
        TIFFGetField(tif, TIFFTAG_BITSPERSAMPLE, &bps);

        

        for (uint32_t strip = 0; strip < TIFFNumberOfStrips(tif); strip++) {
  		    TIFFReadEncodedStrip(tif, strip, buf, (tsize_t) -1);
        }

        TIFFClose(tif);
    }*/
}
