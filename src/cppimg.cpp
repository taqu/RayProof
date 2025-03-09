#include "Core.h"
#define CPPIMG_IMPLEMENTATION
#ifndef CPPIMG_MALLOC
#define CPPIMG_MALLOC(size) LRAY_MALLOC(size)
#endif
#ifndef CPPIMG_FREE
#define CPPIMG_FREE(ptr) LRAY_FREE(ptr)
#endif
#include "cppimg.h"
