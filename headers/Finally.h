#pragma once
#include <functional>

/*
 * Simple wrapper that will execute a callback on exit of scope.
 */
class Finally
{
private:
    std::function<void()> _cb;

public:
    Finally(std::function<void()> cb) : _cb(cb) {}
    ~Finally()
    {
        _cb();
    }
};