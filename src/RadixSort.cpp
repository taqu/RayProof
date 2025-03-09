#include "RadixSort.h"
#include <cstring>
#include <utility>

namespace lray
{
namespace
{
    void sort_block(
        u32 size,
        u32* keys,
        u32* values,
        u32* work_keys,
        u32* work_values,
        u32* work_no,
        u32 block_size,
        u32 block_shift)
    {
        static constexpr u32 BlockSize = 64;
        u32 block_mask = block_size-1;
        u32 blocks = (size+block_size-1)/block_size;
        // counts
        u32 counts[BlockSize] = {};
        for(u32 j = 0; j < blocks; ++j) {
            u32 base = j * block_size;
            for (u32 k = 0; k < block_size; ++k) {
                u32 id = base + k;
                if (size <= id) {
                    break;
                }
                u32 key = (keys[id] >> block_shift)&block_mask;
                work_no[id] = counts[key];
                counts[key] += 1;
            }
        }
        // prefix sum
        u32 prefix[BlockSize] = {};
        prefix[0] = counts[0];
        for (u32 j = 1; j < block_size; ++j) {
            prefix[j] = prefix[j - 1] + counts[j];
        }
        // reorder
        for(u32 j = 0; j < blocks; ++j) {
            u32 base = j * block_size;
            for (u32 k = 0; k < block_size; ++k) {
                u32 id = base + k;
                if (size <= id) {
                    break;
                }
                u32 key = (keys[id] >> block_shift)&block_mask;
                u32 start = prefix[key] - counts[key];
                u32 dst = start + work_no[id];
                assert(dst<size);
                work_keys[dst] = keys[id];
                work_values[dst] = values[id];
            }
        }
    }
} // namespace

void radix_sort(u32 size, u32* keys, u32* values)
{
    u32* tmp_work_keys = new u32[size];
    u32* tmp_work_values = new u32[size];
    u32* work_keys = tmp_work_keys;
    u32* work_values = tmp_work_values;
    u32* work_no = new u32[size];
    for(u32 i = 0; i < 5; ++i) {
        u32 shift = i*6;
        sort_block(size, keys, values, work_keys, work_values, work_no, 64, shift);
        std::swap(work_keys, keys);
        std::swap(work_values, values);
    }
    sort_block(size, keys, values, work_keys, work_values, work_no, 4, 30);
    delete[] work_no;
    delete[] tmp_work_values;
    delete[] tmp_work_keys;
}
} // namespace lray