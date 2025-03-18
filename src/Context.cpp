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

ISampler& Context::getScreenSampler()
{
    return screenSampler_;
}

ISampler& Context::getMaterialSampler()
{
    return materialSampler_;
}

Context::Context()
    :screenSampler_(random_)
    ,materialSampler_(random_)
{
    std::random_device device;
    random_.srand(device());
}

Context::~Context()
{
}

}
