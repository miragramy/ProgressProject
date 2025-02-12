#pragma once
#include <functional>

class Finally
{
private:
    std::function<void()> _cb;

public:
    Finally(std::function<void()> cb) : _cb(cb) {}
    ~Finally(){
        _cb();
    }
};