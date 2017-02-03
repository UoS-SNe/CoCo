#include "Cosmology.hpp"

#include <math.h>

#include <memory>
#include <iostream>

#include "../vmath/integrate.hpp"



Cosmology::Cosmology(double z) {
    set(z);
}


void Cosmology::set(double z) {
    if (z > 0) {
        z_ = z;
        a_ = 1.0/(1.0+z);
        comDis_ = findComDis(z);
        lumDis_ = findLumDis(z);
        comVol_ = findComVol(z);

    } else {
        z_ = 0;
        a_ = 1.0;
        comDis_ = 1.0e-5;
        lumDis_ = 1.0e-5;
        comVol_ = 4.1888e-24;
    }

    lumDisCGS_ = lumDis_ * 3.08568e24;
}


double Cosmology::E(double z1) {
    return 1.0 / sqrt(WV_ + WM_ * pow((1.0 + z1),3));
}


double Cosmology::findComDis(double z) {
    std::vector<double> steps(10000);

    for (int i = 0; i <= 10000; ++i) {
        steps[i] = E(z * i / 10000.0);
    }

    double DH = SI_C/H0_;
    double DM = vmath::trapz<double>(steps,0.0001*z);
    return DH*DM/1000;
}


double Cosmology::findLumDis(double z) {
    return findComDis(z)*(1+z);
}


double Cosmology::findComVol(double z) {
    return pow(findComDis(z) / 1000, 3) * 4 * M_PI /  3;
}


std::vector<double> Cosmology::findDZ(double step) {
    int dzSize = int(z_/step);
    std::vector<double> res(dzSize);

    for (int i = 0; i < dzSize; ++i) {
        res[i] = (i + 1) * step;
    }

    return res;
}


std::vector<double> Cosmology::findDV(std::vector<double> &dz) {
    int dzSize = dz.size();
    std::vector<double> res(dzSize);

    res[0] = findComVol(dz[1]);
    for (int i = 1; i < dzSize; ++i) {
        res[i] = findComVol(dz[i]) - findComVol(dz[i-1]);
    }

    return res;
}
