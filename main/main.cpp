#include <iostream>
#include "tempogram.hpp"


int main() {

    arma::vec A = linspace<vec>(0, 9, 10);
    std::cout << "A:\n" << A(span(2,5)) << "\n";
    std::cout << "AA:\n" << A << "\n";

    return 0;
}