#include "catch_amalgamated.hpp"
#include <vector>
#include <random>
#include "RadixSort.h"
#include "Random.h"

TEST_CASE("Radix Sort" "[Radix]")
{
	using namespace lray;
	{
        u32 keys[] = {3, 1, 10, 5, 8};
        u32 values[] = {3, 1, 10, 5, 8};
        radix_sort(5, keys, values);
        u32 result[] = {1, 3, 5, 8, 10};
        for(u32 i = 0; i < 5; ++i) {
            CHECK(result[i] == keys[i]);
            CHECK(result[i] == values[i]);
        }
	}
    {
        static constexpr u32 Samples = 10000;
        Random random;
        {
            std::random_device device;
            random.srand(device());
        }
        std::vector<u32> keys;
        std::vector<u32> values;
        keys.reserve(Samples);
        values.reserve(Samples);
        for(u32 k = 0; k < 10; ++k) {
            keys.clear();
            values.clear();
            for(u32 i = 0; i < Samples; ++i) {
                u32 x = random();
                keys.push_back(x);
                values.push_back(x);
            }
            radix_sort(Samples, &keys[0], &values[0]);
            CHECK(keys[0] == values[0]);
            for(u32 i = 1; i < Samples; ++i) {
                CHECK(keys[i] == values[i]);
                CHECK(keys[i - 1] <= keys[i]);
            }
        }
	}
}
