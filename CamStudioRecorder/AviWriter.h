/**
 * Copyright(C) 2018  Steven Hoving
 *
 * This program is free software : you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.If not, see < https://www.gnu.org/licenses/>.
 */

#pragma once

#include "Profile.h" // for sVideoOpts

#include <vfw.h>
#include <string>

class avi_writer
{
public:
    avi_writer(const std::string video_filename, const int fps, const BITMAPINFOHEADER &frame_info,
        const sVideoOpts &video_options);

    ~avi_writer();

    void stop();

    void write(DWORD frametime, BITMAPINFOHEADER *alpbi);

    uint64_t total_bytes_written() const;
    uint64_t total_samples_written() const;

private:
    uint64_t total_bytes_written_{ 0 };
    uint64_t total_samples_written_{ 0 };

    PAVIFILE avi_file_{ 0 };
    PAVISTREAM avi_stream_{ nullptr };
    PAVISTREAM avi_stream_compressed_{ nullptr };
    AVISTREAMINFO avi_stream_info_{};
    AVICOMPRESSOPTIONS avi_compress_options_{};

    std::string video_filename_;
    int fps_;
    BITMAPINFOHEADER frame_info_;
    sVideoOpts video_options_;
};
