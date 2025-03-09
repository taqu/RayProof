#include "Context.h"
#include <random>

namespace lray
{
Context* Context::instance_ = nullptr;

void Context::initialize()
{
    assert(nullptr == instance_);
    instance_ = new Context;
}

void Context::terminate()
{
    delete instance_;
    instance_ = nullptr;
}

Context& Context::get()
{
    return *instance_;
}

Context::Context()
{
    std::random_device device;
    random_.srand(device());
}

Context::~Context()
{
}

}
