// CoCo - Supernova templates and simulations package
// Copyright (C) 2016, 2017  Szymon Prajs
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// Contact author: S.Prajs@soton.ac.uk


#include "CoCo.hpp"

#include <iostream>

#include "../src/vmath/algebra.hpp"
#include "../src/vmath/loadtxt.hpp"
#include "../src/vmath/convert.hpp"

#include "../src/core/utils.hpp"
#include "../src/models/Karpenka12.hpp"
#include "../src/solvers/MPFitter.hpp"


CoCo::CoCO() {
    // TEMP
    filtersDir_ = "/Users/szymon/Projects/CoCo/data/filters";
    reconRoot_ = "/Users/szymon/Projects/CoCo";

    // Initialise shared data structures
    filters_ = std::shared_ptr<Filters>(new Filters(filtersDir_));
    cosmology_ = std::shared_ptr<Cosmology>(new Cosmology(0));

    // Find all phase files and load SN templates
    std::vector<std::string> phaseFiles = utils::dirlist(reconRoot_ + "/recon/");
    for (auto it = phaseFiles.begin(); it != phaseFiles.end(); ) {
        if (utils::split(*it, '.').back() != "phase") {
            it.remove();
        } else {
            std::vector< std::vector<std::string> > phaseFile =
              vmath::loadtxt<std::string>(reconRoot_ + "/recon/" + (*it), 2);

            for (size_t i = 0; i < phaseFile[0].size(); ++i) {
                templateSNe_[snName].addSpec(phaseFile[0][i],
                                             atof(phaseFile[1][i].c_str()));
                templateSNe_[snName].z_ = 0.0;
            }

            it++
        }
    }


}


void CoCo::simulate() {

}


// Split mjdSim_ into individual vectors for each filter
std::vector<double> mjdRange(std::string flt,
                             const std::vector<double> &mjd,
                             const std::vector<std::string> &filters) {
    std::vector<double> res;
    for (size_t i = 0; i < mjd.size(); ++i) {
        if (filters[i] == flt) {
            res.push_back(mjd[i]);
        }
    }

    return res;
}


// Simulate light curves
// This function needs to be thread safe. Workspace can only be used for read
// never write as this will cause race conditions. Each loop must have its
// own instance of the Cosmology class. Filters is a bit more of an issue.
// This will have to be addressed correctly later (TODO - Issue #23)
void simulate(std::shared_ptr<Workspace> w) {
    // Until the function is multi-threaded with openMP the cosmology class is safe here
    std::shared_ptr<Cosmology> cosmology(new Cosmology());
    double offset = 1;
    for (size_t i = 0; i < w->templateList_.size(); ++i) {
        // Make a working copy of the template spectra
        SN sn = w->templateSNe_[w->templateList_[i]];

        // Apply host galaxy reddening
        sn.applyReddening(w->Ebv_Host_[i], w->R_v_[i]);

        // Move the spectra to new redshift
        sn.redshift(w->z_[i], cosmology, true);
        sn.moveMJD((1.0 + w->z_[i]), w->mjdPeak_[i]);

        // offset absolute magnitude
        offset = pow(10, -0.4 * (w->absMag_[i]));
        sn.scaleSpectra(offset);

        // Apply Milky Way extinction
        sn.applyReddening(w->Ebv_MW_[i], 3.1);

        // synthesise LC for every unique filter
        std::vector<std::string> uniqueFilters = w->simFilters_[i];
        utils::removeDuplicates(uniqueFilters);
        sn.synthesiseLC(uniqueFilters, w->filters_);


        for (auto &lc : sn.lc_) {
            // Initialise model
            std::shared_ptr<Karpenka12> karpenka12(new Karpenka12);
            karpenka12->x_ = vmath::sub<double>(lc.second.mjd_, lc.second.mjdMin_);
            karpenka12->y_ = vmath::div<double>(lc.second.flux_, lc.second.normalization_);
            karpenka12->sigma_ = std::vector<double>(lc.second.flux_.size(), 1);
            std::shared_ptr<Model> model = std::dynamic_pointer_cast<Model>(karpenka12);

            // Initialise solver
            MPFitter solver(model);
            std::vector<double> xTemp = mjdRange(lc.second.filter_, w->mjdSim_[i], w->simFilters_[i]);
            solver.xRecon_ = vmath::sub<double>(xTemp, lc.second.mjdMin_);

            // Perform fitting
            solver.analyse();
            solver.xRecon_ = vmath::add<double>(solver.xRecon_, lc.second.mjdMin_);
            solver.bestFit_ = vmath::mult<double>(solver.bestFit_, lc.second.normalization_);
        }
    }
}
