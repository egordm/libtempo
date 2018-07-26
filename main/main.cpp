#include <iostream>
#include "tempogram.hpp"


int main() {

    arma::vec A(10);
    A.ones();
    arma::vec B = linspace<vec>(0, 9, 10);

    auto test = tempogram::novelty_curve_to_tempogram_dft(A, B, 10, 10, 10);

    std::cout << "ddd" << std::endl;


    return 0;
}