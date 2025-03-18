#ifndef INC_LRAY_CONTEXT_H_
#define INC_LRAY_CONTEXT_H_
#include <mutex>
#include "Core.h"
#include "Random.h"
#include "Sampler.h"

namespace lray
{
class Context
{
public:
    static void initialize();
    static void terminate();
    static Context& get();

    ISampler& getScreenSampler();
    ISampler& getMaterialSampler();
private:
    Context(const Context&) = delete;
    Context& operator=(const Context&) = delete;
    Context();
    ~Context();
    static Context* instance_;
    std::mutex mutex_;
    Random32 random_;
    SamplerRandom screenSampler_;
    SamplerRandom materialSampler_;
};
}
#endif //INC_LRAY_CONTEXT_H_
