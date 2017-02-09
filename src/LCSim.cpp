#include <vector>
#include <iostream>
#include <string>

#include "vmath/loadtxt.hpp"

#include "core/utils.hpp"


struct Workspace {

};


void help() {
    std::cout << "CoCo - LCSim: \n";
    std::cout << "Developed and implemented by Szymon Prajs (S.Prajs@soton.ac.uk).\n";
    std::cout << "Currently maintained by Szymon Prajs and Rob Firth.\n";
    std::cout << "Originally developed by Natasha Karpenka\n";
    std::cout << "\nUsage:\n";
    std::cout << "lcsim *.list\n";
    std::cout << "./lcsim SN_name redshift abs_mag MJD_peak MJD+filters.list\n\n";
    std::cout << "or\n";
    std::cout << "*.list file must have the following columns:\n";
    std::cout << "SN_name redshift abs_mag MJD_peak MJD+filters.list\n";
    std::cout << std::endl;
}


// Assign input options to workspace parameters
void applyOptions(std::vector<std::string> &options, std::shared_ptr<Workspace> w) {
    if (options.size() < 2 || options[0] == "-h" || options[0] == "--help") {
        help();
        exit(0);
    }

    // If the first parameter is a list then break it down into std::vectors
    if (options[0].substr(options[0].find_last_of(".") + 1) == "list") {

    } else {
        // If a list isn't present and at least 4 parameters are not given, exit.
        if (options.size() < 4) {
            help();
            exit(0);
        }
    }


    // Go though each option and assign the correct properties
    std::vector<std::string> command;
    for (size_t i = 0; i < options.size(); ++i) {
        // Deal with flags by loading pairs of options into commands
        if (options[i] == "-f") {
            if (i+1 < options.size()) {
                command = {options[i], options[i+1]};
                i++;  // skip the next option as it's already assigned above

            } else {
                std::cout << options[i] << " is not a valid flag" << std::endl;
            }

        } else if (options[i] == "-h" || options[i] == "--help"){
            help();
            continue;

        } else {
            utils::split(options[i], '=', command);
        }

        // Assign properties based on commands
        if (command.size() != 2) {
            std::cout << command[0] << " is not a valid command." << std::endl;
            continue;

        } else if (command[0] == "-f" ||
                   command[0] == "--filters" ||
                   command[0] == "--filter" ) {
            // w->filterList_ = utils::split(command[1], ',');

        } else {
            std::cout << command[0] << " is not a valid command." << std::endl;
        }
    }
}


/* Automatically fill in all unassigned properties with defaults */
void fillUnassigned(std::shared_ptr<Workspace> w) {
    // TODO - Load or create an array of MJDs at which to generate the photometry.
}


int main(int argc, char *argv[]) {
    std::vector<std::string> options;
    std::shared_ptr<Workspace> w(new Workspace());

    utils::getArgv(argc, argv, options);
    applyOptions(options, w);
    fillUnassigned(w);

	return 0;
}
