#include "Random.h"
#include <cassert>

#ifndef _MSC_VER
#    define RESTRICT __restrict
#elif defined(__GNUC__) || defined(__clang__)
#    define RESTRICT __restrict
#else
#    define RESTRICT
#endif

namespace lray
{
namespace
{
    void rshift128(rand_impl::w128* RESTRICT out, const rand_impl::w128* RESTRICT in, int32_t shift)
    {
        uint64_t th, tl, oh, ol;

        th = ((uint64_t)in->u_[3] << 32) | ((uint64_t)in->u_[2]);
        tl = ((uint64_t)in->u_[1] << 32) | ((uint64_t)in->u_[0]);

        oh = th >> (shift * 8);
        ol = tl >> (shift * 8);
        ol |= th << (64 - shift * 8);
        out->u_[1] = (uint32_t)(ol >> 32);
        out->u_[0] = (uint32_t)ol;
        out->u_[3] = (uint32_t)(oh >> 32);
        out->u_[2] = (uint32_t)oh;
    }

    void lshift128(rand_impl::w128* RESTRICT out, const rand_impl::w128* RESTRICT in, int32_t shift)
    {
        uint64_t th, tl, oh, ol;

        th = ((uint64_t)in->u_[3] << 32) | ((uint64_t)in->u_[2]);
        tl = ((uint64_t)in->u_[1] << 32) | ((uint64_t)in->u_[0]);

        oh = th << (shift * 8);
        ol = tl << (shift * 8);
        oh |= tl >> (64 - shift * 8);
        out->u_[1] = (uint32_t)(ol >> 32);
        out->u_[0] = (uint32_t)ol;
        out->u_[3] = (uint32_t)(oh >> 32);
        out->u_[2] = (uint32_t)oh;
    }

    void do_recursion(rand_impl::w128* r, rand_impl::w128* a, rand_impl::w128* RESTRICT b, rand_impl::w128* RESTRICT c, rand_impl::w128* RESTRICT d)
    {
        rand_impl::w128 x;
        rand_impl::w128 y;
        lshift128(&x, a, rand_impl::SFMT_SL2);
        rshift128(&y, c, rand_impl::SFMT_SR2);
        r->u_[0] = a->u_[0] ^ x.u_[0] ^ ((b->u_[0] >> rand_impl::SFMT_SR1) & rand_impl::SFMT_MSK1)
                   ^ y.u_[0] ^ (d->u_[0] << rand_impl::SFMT_SL1);
        r->u_[1] = a->u_[1] ^ x.u_[1] ^ ((b->u_[1] >> rand_impl::SFMT_SR1) & rand_impl::SFMT_MSK2)
                   ^ y.u_[1] ^ (d->u_[1] << rand_impl::SFMT_SL1);
        r->u_[2] = a->u_[2] ^ x.u_[2] ^ ((b->u_[2] >> rand_impl::SFMT_SR1) & rand_impl::SFMT_MSK3)
                   ^ y.u_[2] ^ (d->u_[2] << rand_impl::SFMT_SL1);
        r->u_[3] = a->u_[3] ^ x.u_[3] ^ ((b->u_[3] >> rand_impl::SFMT_SR1) & rand_impl::SFMT_MSK4)
                   ^ y.u_[3] ^ (d->u_[3] << rand_impl::SFMT_SL1);
    }

    inline uint32_t rotr32(uint32_t x, uint32_t r)
    {
        return (x >> r) | (x << (static_cast<uint32_t>(-static_cast<int32_t>(r)) & 31));
    }

    inline uint64_t rotr64(uint64_t x, uint32_t r)
{
    return (x >> r) | (x << (static_cast<uint32_t>(-static_cast<int32_t>(r)) & 63));
}
} // namespace

#ifdef _MSC_VER
__uint128_t& __uint128_t::operator^=(const __uint128_t& x)
{
    low_ ^= x.low_;
    high_ ^= x.high_;
    return *this;
}

__uint128_t operator+(const __uint128_t& x0, const __uint128_t& x1)
{
    __uint128_t r = {0,0};
    r.low_ = x0.low_ + x1.low_;
    r.high_ = x0.high_ + x1.high_;
    r.high_ += (0xFFFF'FFFF'FFFF'FFFFULL - x0.low_ < x1.low_)? 1 : 0;
    return r;
}

__uint128_t operator+(const __uint128_t& x0, const uint64_t& x1)
{
    __uint128_t r = {0,0};
    r.low_ = x0.low_ + x1;
    r.high_ += (0xFFFF'FFFF'FFFF'FFFFULL - x0.low_ < x1)? 1 : 0;
    return r;
}

__uint128_t operator+(const uint64_t& x0, const __uint128_t& x1)
{
    __uint128_t r = {0,0};
    r.low_ = x1.low_ + x0;
    r.high_ += (0xFFFF'FFFF'FFFF'FFFFULL - x1.low_ < x0)? 1 : 0;
    return r;
}

__uint128_t operator*(const __uint128_t& x0, const __uint128_t& x1)
{
    __uint128_t r = {0,0};
    r.low_ += _umul128(x0.low_, x1.low_, &r.high_);
    uint64_t high;
    r.low_ += _umul128(x0.high_, x1.low_, &high);
    r.low_ += _umul128(x0.low_, x1.high_, &high);
    return r;
}

__uint128_t operator*(const __uint128_t& x0, const uint64_t& x1)
{
    __uint128_t r = {0,0};
    r.low_ += _umul128(x0.low_, x1, &r.high_);
    uint64_t high;
    r.low_ += _umul128(x0.high_, x1, &high);
    return r;
}

__uint128_t operator*(const uint64_t& x0, const __uint128_t& x1)
{
    __uint128_t r = {0,0};
    r.low_ += _umul128(x1.low_, x0, &r.high_);
    uint64_t high;
    r.low_ += _umul128(x1.high_, x0, &high);
    return r;
}

__uint128_t operator>>(const __uint128_t& x0, uint32_t x1)
{
    assert(x1<=128);
    if(64<=x1){
        uint64_t low = x0.high_>>(x1-64);
        return {0,low};
    }
    __uint128_t r;
    r.low_ = (x0.low_>>x1) | ((x0.high_ & ((1ULL<<x1)-1)) << (64-x1));
    r.high_ = x0.high_ >> x1;
    return r;
}
#endif

namespace rand_impl
{
    /**
     * @brief This function certificate the period of 2^{MEXP}
     * @param sfmt SFMT internal state
     */
    void period_certification(uint32_t* psfmt32)
    {
        uint32_t inner = 0;
        const uint32_t parity[4] = {SFMT_PARITY1, SFMT_PARITY2, SFMT_PARITY3, SFMT_PARITY4};

        for(uint32_t i = 0; i < 4; ++i) {
            inner ^= psfmt32[i] & parity[i];
        }
        for(uint32_t i = 16; 0 < i; i >>= 1) {
            inner ^= inner >> i;
        }
        inner &= 1;
        /* check OK */
        if(inner == 1) {
            return;
        }
        /* check NG, and modification */
        uint32_t work;
        for(uint32_t i = 0; i < 4; ++i) {
            work = 1;
            for(uint32_t j = 0; j < 32; ++j) {
                if((work & parity[i]) != 0) {
                    psfmt32[i] ^= work;
                    return;
                }
                work = work << 1;
            }
        }
    }
} // namespace rand_impl

//--- Random32
//-----------------------------------------------------------------
Random32::Random32()
{
    srand(1234);
}

Random32::Random32(uint32_t x)
{
    srand(x);
}

Random32::~Random32()
{
}

void Random32::srand(uint32_t x)
{
    uint32_t* psfmt32 = &state_[0].u_[0];

    psfmt32[idxof(0)] = x;
    for(uint32_t i = 1; i < SFMT_N32; ++i) {
        psfmt32[idxof(i)] = 1812433253UL * (psfmt32[idxof(i - 1)] ^ (psfmt32[idxof(i - 1)] >> 30)) + i;
    }
    idx_ = SFMT_N32;
    rand_impl::period_certification(psfmt32);
}

uint32_t Random32::rand()
{
    if(SFMT_N32 <= idx_) {
        rand_impl::w128* r1 = &(state_[rand_impl::SFMT_N - 2]);
        rand_impl::w128* r2 = &(state_[rand_impl::SFMT_N - 1]);
        uint32_t i;
        for(i = 0; i < rand_impl::SFMT_N - rand_impl::SFMT_POS1; ++i) {
            do_recursion(&state_[i], &state_[i], &state_[i + rand_impl::SFMT_POS1], r1, r2);
            r1 = r2;
            r2 = &state_[i];
        }
        for(; i < rand_impl::SFMT_N; ++i) {
            do_recursion(&state_[i], &state_[i], &state_[i + rand_impl::SFMT_POS1 - rand_impl::SFMT_N], r1, r2);
            r1 = r2;
            r2 = &state_[i];
        }
        idx_ = 1;
        return state_[0].u_[0];
    }
    return (&state_[0].u_[0])[idx_++];
}

uint32_t Random32::range(uint32_t maxx)
{
    return rand_impl::range(*this, maxx);
}

float Random32::frand()
{
    return rand_impl::frandom_downey_opt32(*this);
}

//--- Random64
//-----------------------------------------------------------------
Random64::Random64()
{
    srand(1234);
}

Random64::Random64(uint64_t x)
{
    srand(x);
}

Random64::~Random64()
{
}

void Random64::srand(uint64_t x)
{
    uint64_t* psfmt64 = &state_[0].u64_[0];
    psfmt64[idxof(0)] = x;
    for(uint32_t i = 1; i < SFMT_N64; ++i) {
        psfmt64[idxof(i)] = 1812433253ULL * (psfmt64[idxof(i - 1)] ^ (psfmt64[idxof(i - 1)] >> 60)) + i;
    }
    idx_ = SFMT_N32;
    rand_impl::period_certification(&state_[0].u_[0]);
}

uint64_t Random64::rand()
{
    if(SFMT_N32 <= idx_) {
        rand_impl::w128* r1 = &(state_[rand_impl::SFMT_N - 2]);
        rand_impl::w128* r2 = &(state_[rand_impl::SFMT_N - 1]);
        uint32_t i;
        for(i = 0; i < rand_impl::SFMT_N - rand_impl::SFMT_POS1; ++i) {
            do_recursion(&state_[i], &state_[i], &state_[i + rand_impl::SFMT_POS1], r1, r2);
            r1 = r2;
            r2 = &state_[i];
        }
        for(; i < rand_impl::SFMT_N; ++i) {
            do_recursion(&state_[i], &state_[i], &state_[i + rand_impl::SFMT_POS1 - rand_impl::SFMT_N], r1, r2);
            r1 = r2;
            r2 = &state_[i];
        }
        idx_ = 2;
        return state_[0].u64_[0];
    }
    uint64_t idx = idx_ >> 1;
    idx_ += 2;
    return (&state_[0].u64_[0])[idx];
}

uint64_t Random64::range(uint64_t maxx)
{
    return rand_impl::range(*this, maxx);
}

double Random64::frand()
{
    #if 0
    uint64_t x = rand();
    return x * (1.0 / 4294967295.0);
    #else
    return rand_impl::frandom_downey_opt64(*this);
    #endif
}

//--- RandomPCG32
//-----------------------------------------------------------------
RandomPCG32::RandomPCG32()
{
    srand(1234U);
}

RandomPCG32::RandomPCG32(uint32_t x)
{
    srand(x);
}

RandomPCG32::~RandomPCG32()
{
}

void RandomPCG32::srand(uint32_t x)
{
    uint32_t* state32 = reinterpret_cast<uint32_t*>(&state_);
    state32[0] = x;
    state32[1] = 1812433253UL * (state32[0] ^ (state32[0] >> 30)) + 1;
}

void RandomPCG32::srand(uint64_t x)
{
    state_ = x;
}

uint32_t RandomPCG32::rand()
{
    uint64_t x = state_;
    state_ = state_ * Multiplier + Increment;
    uint32_t count = static_cast<uint32_t>(x >> 59);
    x ^= x >> 18;
    return rotr32(static_cast<uint32_t>(x >> 27), count);
}

uint32_t RandomPCG32::range(uint32_t maxx)
{
    return rand_impl::range(*this, maxx);
}

float RandomPCG32::frand()
{
    return rand_impl::frandom_downey_opt32(*this);
}

//--- RandomPCG32_128
//-----------------------------------------------------------------
RandomPCG32_128::RandomPCG32_128()
{
    srand(1234U, 5678U);
}

RandomPCG32_128::RandomPCG32_128(uint32_t x0, uint32_t x1)
{
    srand(x0, x1);
}

RandomPCG32_128::~RandomPCG32_128()
{
}

void RandomPCG32_128::srand(uint32_t x0, uint32_t x1)
{
    state0_.srand(x0);
    state1_.srand(x1);
    x1_ = state1_.rand();
}

void RandomPCG32_128::srand(uint64_t x0, uint64_t x1)
{
    state0_.srand(x0);
    state1_.srand(x1);
    x1_ = state1_.rand();
}

uint32_t RandomPCG32_128::rand()
{
    uint32_t x = state0_.rand();
    if(0 == x)[[unlikely]]{
        x1_ = state1_.rand();
    }
    return x + x1_;
}

uint32_t RandomPCG32_128::range(uint32_t maxx)
{
    return rand_impl::range(*this, maxx);
}

float RandomPCG32_128::frand()
{
    return rand_impl::frandom_downey_opt32(*this);
}

//--- RandomPCG64
//-----------------------------------------------------------------
RandomPCG64::RandomPCG64()
{
    srand(1234ULL);
}

RandomPCG64::RandomPCG64(uint64_t x)
{
    srand(x);
}

RandomPCG64::~RandomPCG64()
{
}

void RandomPCG64::srand(uint64_t x)
{
#ifdef _MSC_VER
    state_.low_ = x;
    state_.high_ = 1812433253UL * (state_.low_ ^ (state_.low_ >> 60)) + 1;
#else
    uint64_t* p64 = (uint64_t*)&state_;
    p64[0] = x;
    p64[1] = 1812433253ULL * (p64[0] ^ (p64[0] >> 60)) + 1;
#endif
}

void RandomPCG64::srand(uint64_t x0, uint64_t x1)
{
#ifdef _MSC_VER
    state_ = __uint128_t(x0, x1);
#else
    uint64_t* p64 = (uint64_t*)&state_;
    p64[0] = x0;
    p64[1] = x1;
#endif
}

uint64_t RandomPCG64::rand()
{
    __uint128_t x = state_;
    state_ = state_ * Multiplier + Increment;
    uint32_t count = static_cast<uint32_t>(x >> 122U);
    x ^= x >> 35U;
    return rotr64(static_cast<uint32_t>(x >> 58U), count);
}

uint64_t RandomPCG64::range(uint64_t maxx)
{
    return rand_impl::range(*this, maxx);
}

double RandomPCG64::frand()
{
    return rand_impl::frandom_downey_opt64(*this);
}

} // namespace ray
