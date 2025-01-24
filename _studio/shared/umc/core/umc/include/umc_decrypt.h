// Copyright (c) 2008-2019 Intel Corporation
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#ifndef __UMC_DECRYPT_H_
#define __UMC_DECRYPT_H_

#include <vector>
#include "umc_ranges.h"
#include "mfxstructures.h"

namespace UMC {

const int kDecryptionKeySize = 16;

void ctr128_inc64(uint8_t* counter);

std::vector<SubsampleEntry> EncryptedRangesToSubsampleEntry(
    const uint8_t* start,
    const uint8_t* end,
    const Ranges<const uint8_t*>& encrypted_ranges);

union pavp_header_stream_t {
    uint32_t dw;
    struct {
        uint32_t pavp_session_index : 7;
        uint32_t app_type : 1;
        uint32_t reserved : 23;
        uint32_t valid : 1;
    } fields;
};

union pavp_42_header_stream_t {
    uint32_t dw;
    struct {
        uint32_t valid : 1;
        uint32_t app_type : 1;
        uint32_t stream_id : 16;
        uint32_t reserved : 14;
    } fields;
};

constexpr uint32_t FIRMWARE_API_VERSION_2_1 = ((2 << 16) | (1));
constexpr uint32_t FIRMWARE_API_VERSION_2_4 = ((2 << 16) | (4));
constexpr uint32_t FIRMWARE_API_VERSION_4_2 = ((4 << 16) | (2));

struct pavp_cmd_header_t {
    uint32_t api_version = FIRMWARE_API_VERSION_2_4; // TODO: check android version
    uint32_t command_id;
    union {
        uint32_t status;
        pavp_header_stream_t stream_id;
        pavp_42_header_stream_t stream_id_42;
    };
    uint32_t buffer_len;
};

constexpr uint32_t wv20_get_wrapped_title_keys = 0x00C20022;

struct wv20_get_wrapped_title_keys_in {
    pavp_cmd_header_t header;
    uint32_t session_id;
};

struct wv20_get_wrapped_title_keys_out {
    pavp_cmd_header_t header;
    uint32_t num_keys;
    uint32_t title_key_obj_offset;
    uint32_t buffer_size;
    uint8_t buffer[];
};

constexpr uint32_t PAVP_HECI_IO_BUFFER_SIZE = 16 * 1024;

struct wrapped_title_key_t {
    uint32_t key_id_size;
    uint32_t key_id_offset;
    uint32_t enc_title_key_offset;
};

struct PAVP_SET_STREAM_KEY_PARAMS {
    uint32_t StreamType;
    uint32_t EncryptedDecryptKey[4];
    union {
        uint32_t EncryptedEncryptKey[4];
        uint32_t EncryptedDecryptRotationKey[4];
    };
};

} // namespace UMC

#endif  // __UMC_DECRYPT_H_