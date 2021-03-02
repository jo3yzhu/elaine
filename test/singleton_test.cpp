#include "singleton.h"

using namespace elaine;

class Foo {
public:
    Foo() {

    }
};

class Bar {
public:
    Bar(int x, char y, Foo f) {

    }
};


int main(int argc, char *argv[]) {
    auto f = Singleton<Foo>::GetInstance();
    auto b = Singleton<Bar>::GetInstance(1, 'a', *f);
    return 0;
}