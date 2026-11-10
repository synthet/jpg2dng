// JPEG-derived RGB DNG. Requires Adobe DNG SDK 1.7.1 and its libjpeg.
#include <array>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <memory>
#include <setjmp.h>
#include <stdexcept>
#include <string>
#include <vector>

extern "C" {
#include "jpeglib.h"
}

#include "dng_auto_ptr.h"
#include "dng_camera_profile.h"
#include "dng_exif.h"
#include "dng_file_stream.h"
#include "dng_host.h"
#include "dng_image_writer.h"
#include "dng_matrix.h"
#include "dng_negative.h"
#include "dng_pixel_buffer.h"
#include "dng_simple_image.h"
#include "dng_tag_values.h"
#include "dng_tone_curve.h"

struct jpeg_error_state {
    jpeg_error_mgr base;
    jmp_buf jump;
    char message[JMSG_LENGTH_MAX];
};

struct JpegCameraTags {
    std::string make;
    std::string model;
};

static void jpeg_error_exit(j_common_ptr info) {
    auto *state = reinterpret_cast<jpeg_error_state *>(info->err);
    (*info->err->format_message)(info, state->message);
    longjmp(state->jump, 1);
}

static std::vector<uint8_t> decode_jpeg(const char *path, uint32_t &width, uint32_t &height) {
    FILE *file = nullptr;
    if (fopen_s(&file, path, "rb") != 0 || !file) throw std::runtime_error("cannot open JPEG");
    jpeg_decompress_struct jpg{};
    jpeg_error_state err{};
    jpg.err = jpeg_std_error(&err.base);
    err.base.error_exit = jpeg_error_exit;
    if (setjmp(err.jump)) {
        jpeg_destroy_decompress(&jpg);
        fclose(file);
        throw std::runtime_error(err.message);
    }
    jpeg_create_decompress(&jpg);
    jpeg_stdio_src(&jpg, file);
    jpeg_read_header(&jpg, TRUE);
    jpg.out_color_space = JCS_RGB;
    jpeg_start_decompress(&jpg);
    if (jpg.output_components != 3) throw std::runtime_error("JPEG did not decode to RGB");
    width = jpg.output_width;
    height = jpg.output_height;
    std::vector<uint8_t> rgb(static_cast<size_t>(width) * height * 3);
    while (jpg.output_scanline < jpg.output_height) {
        JSAMPROW row = rgb.data() + static_cast<size_t>(jpg.output_scanline) * width * 3;
        jpeg_read_scanlines(&jpg, &row, 1);
    }
    jpeg_finish_decompress(&jpg);
    jpeg_destroy_decompress(&jpg);
    fclose(file);
    return rgb;
}

static uint16_t read_u16(const uint8_t *p, bool little_endian) {
    if (little_endian) return static_cast<uint16_t>(p[0] | (p[1] << 8));
    return static_cast<uint16_t>((p[0] << 8) | p[1]);
}

static uint32_t read_u32(const uint8_t *p, bool little_endian) {
    if (little_endian)
        return static_cast<uint32_t>(p[0] | (p[1] << 8) | (p[2] << 16) | (p[3] << 24));
    return static_cast<uint32_t>((p[0] << 24) | (p[1] << 16) | (p[2] << 8) | p[3]);
}

static std::string read_exif_ascii(const uint8_t *tiff, size_t tiff_len, uint32_t offset, uint32_t count,
                                   bool little_endian) {
    if (count == 0 || offset >= tiff_len) return {};
    size_t end = static_cast<size_t>(offset) + count;
    if (end > tiff_len) end = tiff_len;
    std::string value(reinterpret_cast<const char *>(tiff + offset), end - offset);
    while (!value.empty() && (value.back() == '\0' || value.back() == ' ')) value.pop_back();
    return value;
}

static void read_ifd_tags(const uint8_t *tiff, size_t tiff_len, uint32_t ifd_offset, bool little_endian,
                          JpegCameraTags &tags) {
    if (ifd_offset + 2 > tiff_len) return;
    const uint16_t entries = read_u16(tiff + ifd_offset, little_endian);
    size_t pos = static_cast<size_t>(ifd_offset) + 2;
    if (pos + entries * 12 > tiff_len) return;
    for (uint16_t i = 0; i < entries; ++i) {
        const uint8_t *entry = tiff + pos + i * 12;
        const uint16_t tag = read_u16(entry, little_endian);
        const uint16_t type = read_u16(entry + 2, little_endian);
        const uint32_t count = read_u32(entry + 4, little_endian);
        const uint32_t value_offset = read_u32(entry + 8, little_endian);
        if (type != 2 || count == 0) continue;
        std::string text;
        if (count <= 4) {
            char inline_buf[5] = {};
            std::memcpy(inline_buf, entry + 8, 4);
            text = inline_buf;
            while (!text.empty() && (text.back() == '\0' || text.back() == ' ')) text.pop_back();
        } else {
            text = read_exif_ascii(tiff, tiff_len, value_offset, count, little_endian);
        }
        if (tag == 271 && tags.make.empty()) tags.make = text;
        if (tag == 272 && tags.model.empty()) tags.model = text;
    }
}

static void parse_exif_tiff(const uint8_t *tiff, size_t tiff_len, JpegCameraTags &tags) {
    if (tiff_len < 8) return;
    const bool little_endian = tiff[0] == 'I' && tiff[1] == 'I';
    const bool big_endian = tiff[0] == 'M' && tiff[1] == 'M';
    if (!little_endian && !big_endian) return;
    if (read_u16(tiff + 2, little_endian) != 42) return;
    const uint32_t ifd0 = read_u32(tiff + 4, little_endian);
    read_ifd_tags(tiff, tiff_len, ifd0, little_endian, tags);
}

static JpegCameraTags read_jpeg_make_model(const char *path) {
    JpegCameraTags tags;
    FILE *file = nullptr;
    if (fopen_s(&file, path, "rb") != 0 || !file) return tags;
    if (fseek(file, 0, SEEK_END) != 0) {
        fclose(file);
        return tags;
    }
    const long file_size = ftell(file);
    if (file_size <= 0 || file_size > 64L * 1024 * 1024) {
        fclose(file);
        return tags;
    }
    std::vector<uint8_t> data(static_cast<size_t>(file_size));
    rewind(file);
    if (fread(data.data(), 1, data.size(), file) != data.size()) {
        fclose(file);
        return tags;
    }
    fclose(file);

    size_t i = 0;
    while (i + 4 < data.size()) {
        if (data[i] != 0xFF) {
            ++i;
            continue;
        }
        const uint8_t marker = data[i + 1];
        if (marker == 0xD8) {
            i += 2;
            continue;
        }
        if (marker == 0xD9) break;
        if (marker >= 0xD0 && marker <= 0xD7) {
            i += 2;
            continue;
        }
        const uint16_t segment_len = static_cast<uint16_t>((data[i + 2] << 8) | data[i + 3]);
        if (segment_len < 2 || i + 2 + segment_len > data.size()) break;
        if (marker == 0xE1) {
            const size_t payload = i + 4;
            const size_t payload_len = segment_len - 2;
            if (payload_len >= 6 && std::memcmp(data.data() + payload, "Exif\0\0", 6) == 0)
                parse_exif_tiff(data.data() + payload + 6, payload_len - 6, tags);
        }
        i += 2 + segment_len;
    }
    return tags;
}

static uint16_t linearize(uint8_t value) {
    const double c = value / 255.0;
    const double linear = c <= 0.04045 ? c / 12.92 : std::pow((c + 0.055) / 1.055, 2.4);
    return static_cast<uint16_t>(std::lround(linear * 65535.0));
}

static void set_color_matrix(dng_matrix &matrix, const double values[9]) {
    for (int r = 0; r < 3; ++r)
        for (int c = 0; c < 3; ++c) matrix[r][c] = values[r * 3 + c];
}

static void convert(const char *source, const char *destination, bool display) {
    uint32_t width = 0, height = 0;
    const auto rgb = decode_jpeg(source, width, height);
    const JpegCameraTags camera = read_jpeg_make_model(source);

    dng_host host;
    AutoPtr<dng_negative> negative(host.Make_dng_negative());
    const char *unique_model = display ? "sRGB D65" : "sRGB LinearRaw";
    negative->SetModelName(unique_model);
    negative->SetLocalName(unique_model);
    if (auto *exif = negative->Metadata().GetExif()) {
        if (!camera.make.empty()) exif->fMake.Set_ASCII(camera.make.c_str());
        else exif->fMake.Set_ASCII(display ? "sRGB" : "Adobe");
        if (!camera.model.empty()) exif->fModel.Set_ASCII(camera.model.c_str());
        else exif->fModel.Set_ASCII(display ? "D65" : "DNG");
        exif->fSoftware.Set_ASCII(display ? "jpg2dng Adobe SDK 1.7.1 display v5"
                                          : "jpg2dng Adobe SDK 1.7.1");
    }
    negative->SetColorChannels(3);
    negative->SetActiveArea(dng_rect(0, 0, height, width));
    negative->SetDefaultCropSize(width, height);
    negative->SetDefaultCropOrigin(0, 0);
    negative->SetBlackLevel(0.0);
    negative->SetWhiteLevel(65535);

    dng_vector neutral(3);
    neutral[0] = neutral[1] = neutral[2] = 1.0;
    negative->SetCameraNeutral(neutral);

    AutoPtr<dng_camera_profile> profile(new dng_camera_profile());
    profile->SetCalibrationIlluminant1(lsD65);
    dng_matrix matrix(3, 3);
    const double linear_srgb[9] = {3.2405, -1.5371, -0.4985, -0.9693, 1.8760, 0.0416, 0.0556, -0.2040, 1.0572};
    if (display) {
        profile->SetName("sRGB D65");
        set_color_matrix(matrix, linear_srgb);
        profile->SetToneCurve(dng_tone_curve()); // Identity rendering curve.
    } else {
        profile->SetName("Linear sRGB D65");
        set_color_matrix(matrix, linear_srgb);
    }
    profile->SetColorMatrix1(matrix);
    negative->AddProfile(profile);

    AutoPtr<dng_image> image(host.Make_dng_image(dng_rect(0, 0, height, width), 3, ttShort));
    dng_pixel_buffer pixels;
    dynamic_cast<dng_simple_image *>(image.Get())->GetPixelBuffer(pixels);
    const auto table = [] {
        std::array<uint16_t, 256> result{};
        for (int i = 0; i < 256; ++i) {
            result[i] = linearize(static_cast<uint8_t>(i));
        }
        return result;
    }();
    for (uint32_t y = 0; y < height; ++y)
        for (uint32_t x = 0; x < width; ++x)
            for (uint32_t c = 0; c < 3; ++c)
                *pixels.DirtyPixel_uint16(y, x, c) = table[rgb[(static_cast<size_t>(y) * width + x) * 3 + c]];
    negative->SetStage1Image(image);
    dng_file_stream output(destination, true);
    dng_image_writer writer;
    writer.WriteDNG(host, output, *negative.Get(), nullptr, dngVersion_SaveDefault, false);
}

int main(int argc, char **argv) {
    bool display = false;
    int argi = 1;
    if (argi < argc && std::strcmp(argv[argi], "--display") == 0) {
        display = true;
        ++argi;
    }
    if (argc - argi != 2) {
        std::cerr << "usage: jpg2dng [--display] input.jpg output.dng\n";
        return 2;
    }
    try {
        convert(argv[argi], argv[argi + 1], display);
        return 0;
    } catch (const std::exception &error) {
        std::cerr << "jpg2dng: " << error.what() << '\n';
        return 1;
    }
}
