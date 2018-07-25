#include <iostream>
#include "tempogram.h"


int main() {

    arma::Mat<double> A = arma::randu(4,4);
    std::cout << "A:\n" << A << "\n";

    return 0;
}