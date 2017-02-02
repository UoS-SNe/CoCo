#include <iostream>
#include <vector>
#include <string>

#include "vmath/convert.hpp"
#include "vmath/loadtxt.hpp"

#include "core/Filters.hpp"
#include "core/SN.hpp"
#include "core/utils.hpp"


struct Workspace {
    // Code inputs
    std::string zeroFilter_;
    std::string inputLCList_;

    // Filter info
    std::string filterPath_;
    std::shared_ptr<Filters> filters_;

    // SN data
    std::vector<std::string> lcList_;
    std::vector<double> z_;
    std::unordered_map<std::string, SN> sn_;
};


void help() {
    std::cout << "CoCo - SpecPhase: \n";
    std::cout << "Originally writen by Natasha Karpenka, ";
    std::cout << "currently maintained by Szymon Prajs (S.Prajs@soton.ac.uk) ";
    std::cout << "and Rob Firth.\n";
    std::cout << "\nUsage:\n";
    std::cout << "specphase filter_name\n";
    std::cout << std::endl;
}


// Assign input options to workspace parameters
void applyOptions(std::vector<std::string> &options, std::shared_ptr<Workspace> w) {
    if (options.size() < 1 || options[0] == "-h" || options[0] == "--help") {
        help();
        exit(0);

    } else if (options.size() == 2)  {
        w->inputLCList_ = options[0];
        w->zeroFilter_ = options[1];

        if (utils::fileExists(w->inputLCList_)) {
            std::vector< std::vector<std::string> > temp;
            vmath::loadtxt<std::string>(w->inputLCList_, 2, temp);
            w->lcList_ = temp[0];
            vmath::castString<double>(temp[1], w->z_);
        }

    } else if (options.size() == 3) {
            w->lcList_ = {options[1]};
            w->z_ = {atof(options[2].c_str())};

    } else {
        std::cout << "Options are not currently implemented\n";
        std::cout << "Program will continue executing" << std::endl;
    }
}


// Automatically fill in all unassigned properties with defaults
void fillUnassigned(std::shared_ptr<Workspace> w) {
    // Do a sanity check for the LC files
    if (w->lcList_.size() == 0) {
        std::cout << "Something went seriously wrong.";
        std::cout << "Please consider report this bug on our project GitHub page";
        std::cout << std::endl;
        exit(0);
    }

	// Load the light curves
	for (size_t i = 0; i < w->lcList_.size(); ++i) {
		if (utils::fileExists("recon/" + w->lcList_[i] + ".dat")) {
            SN sn;
            sn.name_ = utils::baseName(w->lcList_[i]);
            sn.z_ = w->z_[i];
			w->sn_[sn.name_] = sn;
		}
	}
}


// Scan recon folder for mangled spectra and assign to the correct SN
void scanRecon(shared_ptr<Workspace> w) {
    std::vector<std::string> files;
    utils::dirlist("recon", files);

    std::string snname;
    double mjd;
    for (auto &file : files) {
        if (utils::fileExtention(file) == "spec") {
            snname = utils::split(utils::baseName(file), '_').front();
            mjd = atof(utils::split(utils::baseName(file), '_').back().c_str());
            if (w->sn_.find(snname) != w->sn_.end()) {
                w->sn_[snname].addSpec(file, mjd);
            }
        }
    }
}


int main (int argc, char* argv[]) {
    std::vector<std::string> options;
    std::shared_ptr<Workspace> w(new Workspace());

    utils::getArgv(argc, argv, options);
    applyOptions(options, w);

    // Read in filters and find the ID of the filter used to determine the phase
    w->filterPath_ = "data/filters";
    w->filters_ = std::shared_ptr<Filters>(new Filters(w->filterPath_));

    // run SpecPhase pipeline
    scanRecon(w);

    return 0;
}
