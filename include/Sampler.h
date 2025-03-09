#ifndef INC_RAY_SAMPLER_H_
#define INC_RAY_SAMPLER_H_
#include "Vector.h"
#include "Random.h"

namespace ray
{
class ISampler
{
public:
    virtual void next() = 0;
    virtual f32 sample() = 0;
    virtual Vector2 sample2() = 0;
    virtual Vector3 sample3() = 0;
};

/**
http://extremelearning.com.au/unreasonable-effectiveness-of-quasirandom-sequences/
*/
class SamplerR2 : public ISampler
{
public:
    inline static constexpr f32 R2_G0 = f32(1.0 / 1.61803398874989484820458683436563);
	inline static constexpr f32 R2_G1 = f32(1.0 / 1.32471795724474602596090885447809);
	inline static constexpr f32 R2_G2 = f32(1.0 / 1.22074408460575947536168534910883);
	inline static constexpr f32 R2_Delta = f32(0.76);
	inline static constexpr f32 R2_I0 = f32(0.3);
	inline static constexpr f32 R2_SqrtPi = f32(1.77245385091);
	//inline static constexpr f32 Magic = f32(0.618033988749894);

    SamplerR2(u32 seed, f32 lambda=1.0e-4f);
    ~SamplerR2();
    virtual void next() override;
    virtual f32 sample() override;
    virtual Vector2 sample2() override;
    virtual Vector3 sample3() override;
private:
    RandomPCG32 rand_;
    f32 lambda_;
    uint32_t n_;
};

Vector2 randomOnDisk(f32 x0, f32 x1);

} // namespace ray
#endif // INC_RAY_SAMPLER_H_
