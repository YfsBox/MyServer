//
// Created by 杨丰硕 on 2022/7/15.
//

#include <iostream>
#include <list>

class A {
public:
    A() { std::cout<<"...\n";}
    ~A() {std::cout<<",,,\n";}
};

int main() {
    {
        A a;
    }
    std::list<int> L;
    int tmp = L.front();
    {
        A b;
    }
}
