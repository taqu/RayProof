#include "Sampler.h"
#include <cmath>

namespace ray
{
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

Vector2 randomOnDisk(f32 x0, f32 x1)
{
// http://psgraphics.blogspot.ch/2011/01/improved-code-for-concentric-map.html
	f32 r0 = 2.0f*x0 - 1.0f;
	f32 r1 = 2.0f*x1 - 1.0f;
	f32 absR0 = abs(r0);
	f32 absR1 = abs(r1);
	f32 phi;
	f32 r;
	if(absR0 <= F32_EPSILON && absR1 <= F32_EPSILON){
		phi = 0.0f;
		r = 0.0f;
	}else if(absR1 < absR0){
		phi = (F32_PI/4.0f) * (r1/r0);
		r = r0;
	} else {
		r = r1;
		phi = (F32_PI/2.0f) - (r0/r1)*(F32_PI/4.0f);
	}

	return Vector2{r * std::cosf(phi), r * std::sinf(phi)};
}
} // namespace ray
