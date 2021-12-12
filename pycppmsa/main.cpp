#include <iostream>
#include "ad3_msa.h"

int main() {
    int n_vertices = 5;
    auto v = msa(
            n_vertices,
            [&](int head, int mod)
            {
                std::cout << head << ", " << mod << "\n";
                if (head == 0)
                    return -1000;
                else
                    return 0;
            }
            );
    for (auto i : v)
        std::cout << i << "\t";
    std::cout << "\n";
}