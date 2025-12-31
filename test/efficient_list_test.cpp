//
// Created by silve on 2025-12-31.
//

#include "efficient_list.hpp"
#include <iostream>
#include <sstream>

void print(const stl::he_list<int> &lst) {
    if (lst.empty()) {
        std::cout << std::endl;
        return;
    }

    auto iter = lst.begin();
    std::cout << *iter++;
    for (; iter != lst.end(); ++iter) {
        std::cout << " " << *iter;
    }
    std::cout << std::endl;
}

int main() {
    stl::he_list<int> lst{3, 6, 9, 9, 10};
    print(lst);

    lst.erase(2);
    print(lst);

    lst.insert(2, 100);
    print(lst);
}
