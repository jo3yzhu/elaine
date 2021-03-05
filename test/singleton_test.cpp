#include <cassert>
#include <iostream>

#include "util/singleton.h"

using namespace elaine;

class Foo {
public:
    Foo() {

    }
};

class Bar {
public:
    Bar(int x, char y) {
        x_ = x;
        y_ = y;
    }
    int x_;
    char y_;
};


int main(int argc, char* argv[]) {
    auto a = Singleton<Foo>::GetInstance();
    auto c = Singleton<Foo>::GetInstance();
    assert(a == c);

    auto b = Singleton<Bar>::GetInstance(1, 'a'); 
    auto d = Singleton<Bar>::GetInstance(3, 'c');
    auto e = Singleton<Bar>::GetInstance(3, 'c');
    assert(b == d);
    assert(d == e);

    std::cout << d->x_ << "," << d->y_ << std::endl; // 1, a
    std::cout << b->x_ << "," << d->y_ << std::endl; // 1, a
    std::cout << e->x_ << "," << d->y_ << std::endl; // 1, a

    return 0;
}