#include "Matrix34.h"
#include <immintrin.h>

namespace lray
{
Matrix34 mul(const Matrix34& m0, const Matrix34& m1)
{
    __m128 rm0 = _mm_loadu_ps(&(m1.m_[0][0]));
    __m128 rm1 = _mm_loadu_ps(&(m1.m_[1][0]));
    __m128 rm2 = _mm_loadu_ps(&(m1.m_[2][0]));

    Matrix34 m;
    __m128 t0, t1, t2;
    for(u32 c = 0; c < 3; ++c) {
        t0 = _mm_set1_ps((m0.m_[c][0]));
        t1 = _mm_set1_ps((m0.m_[c][1]));
        t2 = _mm_set1_ps((m0.m_[c][2]));

        t0 = _mm_mul_ps(t0, rm0);
        t1 = _mm_mul_ps(t1, rm1);
        t2 = _mm_mul_ps(t2, rm2);

        t0 = _mm_add_ps(t0, t1);
        t0 = _mm_add_ps(t0, t2);

        _mm_storeu_ps(&(m.m_[c][0]), t0);
        m.m_[c][3] += m0.m_[c][3];
    }
    return m;
}
}
