#pragma once

#include <functional>

namespace elaine
{

struct Defer {
    Defer(std::function<void()> func) : func_(func) {}
    ~Defer() {
        func_();
    }
    std::function<void()> func_;
};

#define _defer(lambda, y) x##y(lambda)
#define __defer(lambda, y) _defer(lambda, y)
#define defer(lambda)    Defer __defer(lambda, __COUNTER__);

}