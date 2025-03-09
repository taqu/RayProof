#ifndef INC_LRAY_RANDOM_H_
#define INC_LRAY_RANDOM_H_
#include <bit>
#include <cstdint>
#include <cassert>

namespace lray
{
#ifdef _MSC_VER
struct __uint128_t
{

    __uint128_t(){}

    explicit __uint128_t(uint64_t x)
        : low_(x)
        , high_(0)
    {
    }

    __uint128_t(uint64_t low, uint64_t high)
        : low_(low)
        , high_(high)
    {
    }

    operator uint32_t() const
    {
        return static_cast<uint32_t>(low_);
    }

    operator uint64_t() const
    {
        return low_;
    }
    __uint128_t(const __uint128_t&) = default;
    __uint128_t& operator=(const __uint128_t&) = default;

    __uint128_t& operator^=(const __uint128_t& x);

    uint64_t low_;
    uint64_t high_;
};

__uint128_t operator+(const __uint128_t& x0, const __uint128_t& x1);
__uint128_t operator+(const __uint128_t& x0, const uint64_t& x1);
__uint128_t operator+(const uint64_t& x0, const __uint128_t& x1);

__uint128_t operator*(const __uint128_t& x0, const __uint128_t& x1);
__uint128_t operator*(const __uint128_t& x0, const uint64_t& x1);
__uint128_t operator*(const uint64_t& x0, const __uint128_t& x1);

__uint128_t operator>>(const __uint128_t& x0, uint32_t x1);
#endif

namespace rand_impl
{
    static constexpr uint32_t SFMT_MEXP = 607;
    static constexpr uint32_t SFMT_N = (SFMT_MEXP / 128 + 1);
    static constexpr uint32_t SFMT_POS1 = 2;
    static constexpr uint32_t SFMT_SL1 = 15;
    static constexpr uint32_t SFMT_SL2 = 3;
    static constexpr uint32_t SFMT_SR1 = 13;
    static constexpr uint32_t SFMT_SR2 = 3;
    static constexpr uint32_t SFMT_MSK1 = 0xfdff37ffU;
    static constexpr uint32_t SFMT_MSK2 = 0xef7f3f7dU;
    static constexpr uint32_t SFMT_MSK3 = 0xff777b7dU;
    static constexpr uint32_t SFMT_MSK4 = 0x7ff7fb2fU;
    static constexpr uint32_t SFMT_PARITY1 = 0x00000001U;
    static constexpr uint32_t SFMT_PARITY2 = 0x00000000U;
    static constexpr uint32_t SFMT_PARITY3 = 0x00000000U;
    static constexpr uint32_t SFMT_PARITY4 = 0x5986f054U;

    union w128
    {
        uint32_t u_[4];
        uint64_t u64_[2];
    };

    template<class T>
    float frandom_downey_opt32(T& random)
    {
        constexpr int32_t lowExp = 0;
        constexpr int32_t highExp = 127;
        const uint32_t u = random.rand();
        const uint32_t b = u & 0xFFU;
        int32_t exponent = highExp - 1;
        if(0 == b) {
            exponent -= 8;
            while(true) {
                const uint32_t bits = random.rand();
                if(0 == bits) {
                    exponent -= 32;
                    if(exponent < lowExp) {
                        exponent = lowExp;
                        break;
                    }
                } else {
                    int32_t c = std::countr_zero(bits);
                    exponent -= c;
                    break;
                }
            }
        } else {
            int32_t c = std::countr_zero(b);
            exponent -= c;
        }
        const uint32_t mantissa = (u >> 8) & 0x7FFFFFUL;
        if(0 == mantissa && (u >> 31)) {
            ++exponent;
        }
        return std::bit_cast<float, uint32_t>((exponent << 23) | mantissa);
    }

    template<class T>
    double frandom_downey_opt64(T& random)
    {
        constexpr int32_t lowExp = 0;
        constexpr int32_t highExp = 1023;
        const uint64_t u = random.rand();
        const uint64_t b = u & 0xFFU;
        int64_t exponent = highExp - 1;
        if(0 == b) {
            exponent -= 8;
            while(true) {
                const uint64_t bits = random.rand();
                if(0 == bits) {
                    exponent -= 64;
                    if(exponent < lowExp) {
                        exponent = lowExp;
                        break;
                    }
                } else {
                    int32_t c = std::countr_zero(bits);
                    exponent -= c;
                    break;
                }
            }
        } else {
            int32_t c = std::countr_zero(b);
            exponent -= c;
        }
        const uint64_t mantissa = (u >> 8) & 0xF'FFFF'FFFF'FFFFULL;
        if(0 == mantissa && (u >> 63)) {
            ++exponent;
        }
        return std::bit_cast<double, uint64_t>((exponent << 52) | mantissa);
    }

    template<class T>
    uint32_t range(T& r, uint32_t maxx)
    {
        uint32_t t = (static_cast<uint32_t>(-static_cast<int32_t>(maxx))) % maxx;
        uint64_t m;
        uint32_t l;
        do {
            uint32_t x = r.rand();
            m = uint64_t(x) * uint64_t(maxx);
            l = uint32_t(m);
        } while(l < t);
        return m >> 32;
    }

    template<class T>
    uint64_t range(T& r, uint64_t maxx)
    {
        uint64_t x = r.rand();
#if _MSC_VER
        __uint128_t m = __uint128_t(x) * __uint128_t(maxx);
        uint64_t l = (uint64_t)m;
        if(l < maxx) {
            uint64_t t = (static_cast<uint64_t>(-static_cast<int64_t>(maxx))) % maxx;
            while(l < t) {
                x = r.rand();
                m = __uint128_t(x) * __uint128_t(maxx);
                l = (uint64_t)m;
            }
        }
        return m.high_;
#else
        __uint128_t m = (__uint128_t)x * (__uint128_t)maxx;
        uint64_t l = (uint64_t)m;
        if(l < maxx) {
            uint64_t t = -maxx % maxx;
            while(l < t) {
                x = r.rand();
                m = (__uint128_t)x * (__uint128_t)maxx;
                l = (uint64_t)m;
            }
        }
        return (uint64_t)(m >> 64);
#endif
    }

} // namespace rand_impl

//--- Random32
//-----------------------------------------------------------------
class Random32
{
public:
    inline static constexpr uint32_t SFMT_N32 = (rand_impl::SFMT_N * 4);
    Random32();
    explicit Random32(uint32_t x);
    ~Random32();

    void srand(uint32_t x);
    uint32_t rand();
    uint32_t range(uint32_t maxx);
    float frand();

private:
    friend class Random64;
    inline static uint32_t idxof(uint32_t i)
    {
        return i;
    }

    rand_impl::w128 state_[rand_impl::SFMT_N];
    uint32_t idx_;
};

//--- Random64
//-----------------------------------------------------------------
class Random64
{
public:
    inline static constexpr uint32_t SFMT_N32 = (rand_impl::SFMT_N * 4);
    inline static constexpr uint32_t SFMT_N64 = (rand_impl::SFMT_N * 2);
    Random64();
    explicit Random64(uint64_t x);
    ~Random64();

    void srand(uint64_t x);
    uint64_t rand();
    uint64_t range(uint64_t maxx);
    double frand();

private:
    inline static uint32_t idxof(uint32_t i)
    {
        return i;
    }
    rand_impl::w128 state_[rand_impl::SFMT_N];
    uint32_t idx_;
};

//--- RandomPCG32
//-----------------------------------------------------------------
class RandomPCG32
{
public:
    inline static constexpr uint64_t Multiplier = 6364136223846793005ULL;
    inline static constexpr uint64_t Increment = 1442695040888963407ULL;

    RandomPCG32();
    explicit RandomPCG32(uint32_t x);
    ~RandomPCG32();

    void srand(uint32_t x);
    void srand(uint64_t x);
    uint32_t rand();
    uint32_t range(uint32_t maxx);
    float frand();

private:
    uint64_t state_;
};

//--- RandomPCG32_128
//-----------------------------------------------------------------
class RandomPCG32_128
{
public:
    RandomPCG32_128();
    RandomPCG32_128(uint32_t x0, uint32_t x1);
    ~RandomPCG32_128();

    void srand(uint32_t x0, uint32_t x1);
    void srand(uint64_t x0, uint64_t x1);
    uint32_t rand();
    uint32_t range(uint32_t maxx);
    float frand();

private:
    RandomPCG32 state0_;
    RandomPCG32 state1_;
    uint32_t x1_;
};

//--- RandomPCG64
//-----------------------------------------------------------------
class RandomPCG64
{
public:
    inline static constexpr uint64_t Multiplier = 6364136223846793005ULL;
    inline static constexpr uint64_t Increment = 1442695040888963407ULL;

    RandomPCG64();
    explicit RandomPCG64(uint64_t x);
    ~RandomPCG64();
    void srand(uint64_t x);
    void srand(uint64_t x0, uint64_t x1);
    uint64_t rand();
    uint64_t range(uint64_t maxx);
    double frand();

private:
    __uint128_t state_;
};

//--- RandomUtil
//--------------------------------------------
struct RandomUtil
{
    template<class T, class U>
    static void shuffle(uint32_t size, T* array, U& rand)
    {
        assert(0<size);
        uint32_t lastIndex = size - 1;
        for(uint32_t i = 0; i < lastIndex; ++i) {
            uint32_t index = rand.range(i, size);
            if(i != index) {
                std::swap(array[i], array[index]);
            }
        }
    }
};

} // namespace ray
#endif // INC_LRAY_RANDOM_H_
