// Copyright (c) 2003-2018 Intel Corporation
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

#include "umc_decrypt.h"
#include "mfx_utils_logging.h"

namespace UMC {

// This increments the lower 64 bit counter of an 128 bit IV.
void ctr128_inc64(uint8_t* counter) {
  uint32_t n = 16;
  do {
    if (++counter[--n] != 0)
      return;
  } while (n > 8);
}

std::vector<SubsampleEntry> EncryptedRangesToSubsampleEntry(
    const uint8_t* start,
    const uint8_t* end,
    const Ranges<const uint8_t*>& encrypted_ranges)
{
    std::vector<SubsampleEntry> subsamples(encrypted_ranges.size());
    const uint8_t* cur = start;
    for (size_t i = 0; i < encrypted_ranges.size(); ++i) {
        const uint8_t* encrypted_start = encrypted_ranges.start(i);
        if (encrypted_start < cur)
            MFX_LOG_ERROR("Encrypted range started before the current buffer pointer.\n");
        subsamples[i].clear_bytes = encrypted_start - cur;
        const uint8_t* encrypted_end = encrypted_ranges.end(i);
        subsamples[i].cypher_bytes = encrypted_end - encrypted_start;
        cur = encrypted_end;
        if (cur > end)
            MFX_LOG_ERROR("Encrypted range is outside the buffer range.\n");
    }
    // If there is more data in the buffer but not covered by encrypted_ranges,
    // then it must be in the clear.
    if (cur < end) {
        SubsampleEntry entry;
        entry.clear_bytes = end - cur;
        entry.cypher_bytes = 0;
        subsamples.push_back(std::move(entry));
    }
    return subsamples;
}

} // namespace UMC