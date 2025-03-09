#ifndef INC_LRAY_MATRIX34_H_
#define INC_LRAY_MATRIX34_H_
#include "Common.h"

namespace lray
{
class Matrix34
{
public:
	f32 m_[3][4];
};

Matrix34 mul(const Matrix34& m0, const Matrix34& m1);
} // namespace lray
#endif //INC_LRAY_MATRIX34_H_

