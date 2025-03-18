#include "Sampler.h"
#include <cmath>

namespace lray
{
SamplerRandom::SamplerRandom(Random32& rand)
    :rand_(rand)
{
}

SamplerRandom::~SamplerRandom()
{
}

f32 SamplerRandom::sample()
{
	return rand_.frand();
}

Vector2 SamplerRandom::sample2()
{
	f32 x0 = rand_.frand();
	f32 x1 = rand_.frand();
	return {x0,x1};
}

Vector3 SamplerRandom::sample3()
{
	f32 x0 = rand_.frand();
	f32 x1 = rand_.frand();
	f32 x2 = rand_.frand();
	return {x0,x1,x2};
}

void SamplerRandom::sampleN(uint32_t n, f32* samples)
{
	assert(nullptr != samples);
	for(uint32_t i=0; i<n; ++i){
		samples[i] = rand_.frand();
	}
}

#if 0
SamplerR2::SamplerR2(u32 seed, f32 lambda)
    :rand_(seed)
	,lambda_(lambda)
	,n_(0)
{
}

SamplerR2::~SamplerR2()
{
}

void SamplerR2::next()
{
	++n_;
}

f32 SamplerR2::sample()
{
	f32 fn = f32(n_);
	f32 x = 0.5f + R2_G0*fn;
	f32 p = x - std::floorf(x);
	f32 u = rand_.frand();
	f32 k = lambda_ * R2_Delta * R2_SqrtPi / (4.0f * std::sqrtf(fn+R2_I0));
	p += k*u;
	p -= std::floorf(p);
	return p;
}

Vector2 SamplerR2::sample2()
{
	f32 fn = f32(n_);
	f32 a1 = R2_G1;
	f32 a2 = R2_G1 * R2_G1;
	f32 x = 0.5f + a1*fn;
	f32 y = 0.5f + a2*fn;
	Vector2 p = {x - std::floorf(x), y-std::floorf(y)};
	Vector2 u = {rand_.frand(), rand_.frand()};
	f32 k = lambda_ * R2_Delta * R2_SqrtPi / (4.0f * std::sqrtf(fn+R2_I0));
	p += k*u;
	p.x_ -= std::floorf(p.x_);
	p.y_ -= std::floorf(p.y_);
	return p;
}

Vector3 SamplerR2::sample3()
{
	f32 fn = f32(n_);
	f32 a1 = R2_G2;
	f32 a2 = R2_G2 * R2_G2;
	f32 a3 = R2_G2 * R2_G2;
	f32 x = 0.5f + a1*fn;
	f32 y = 0.5f + a2*fn;
	f32 z = 0.5f + a3*fn;
	Vector3 p = {x - std::floorf(x), y-std::floorf(y), z-std::floorf(z)};
	Vector3 u = {rand_.frand(), rand_.frand(), rand_.frand()};
	f32 k = lambda_ * R2_Delta * R2_SqrtPi / (4.0f * std::sqrtf(fn+R2_I0));
	p += k*u;
	p.x_ -= std::floorf(p.x_);
	p.y_ -= std::floorf(p.y_);
	p.z_ -= std::floorf(p.z_);
	return p;
}
#endif

} // namespace lray
