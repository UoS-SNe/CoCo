#include <vector>
#include <iostream>
#include <string>
#include "core/utils.hpp"
#include "core/LC.hpp"
#include "vmath/loadtxt.hpp"
#include "lc/WorkspaceLC.hpp"
#include "lc/Model.hpp"
#include "lc/MultiNest.hpp"

using namespace std;


void help() {
    cout << "CoCo - LCSim: \n";
    cout << "Originally developed by Natasha Karpenka, ";
    cout << "and implemented by Szymon Prajs (S.Prajs@soton.ac.uk).\n";
    cout << "Currently maintained by Szymon Prajs and Rob Firth.\n";
    cout << "\nUsage:\n";
    cout << "lcsim *.list\n";
    cout << "or\n";
    cout << "./lcsim SN_name redshift abs_mag filters\n\n";
    cout << "*.list file must have the following columns:\n";
    cout << "SN_name redshift abs_mag filters\n";
    cout << endl;
}


/* Assign input options to workspace parameters */
void applyOptions(vector<string> &options, shared_ptr<WorkspaceLC> w) {
    // Number of inputs to skip before reading extra options
    short skipOptions = 1;

    if (options.size() < 2 || options[0] == "-h" || options[0] == "--help") {
        help();
        exit(0);
    }

    // First option is a SN name or list of SN names and redshifts
    w->LCListFile_ = options[0];
    w->filterVector_.clear();

    // If the first parameter is a list then break it down into vectors
    if (options[0].substr(options[0].find_last_of(".") + 1) == "list") {
        vmath::loadtxt<string>(w->LCListFile_, 4, w->infoList_);
        w->snNameList_ = w->infoList_[0];
        w->zList_ = castString<double>(w->infoList_[1]);
        w->absMag_ = castString<double>(w->infoList_[2]);
        for (auto flt : w->infoList_[3]) {
            w->filterVector_.push_back(split(flt, ','));
        }

    } else {
        // If a list isn't present and at least 4 parameters are not given, exit.
        if (options.size() < 4) {
            help();
            exit(0);
        }

        skipOptions = 4;
        w->snNameList_ = {options[0]};
        w->zList_ = {atof(options[1].c_str())};
        w->absMag_ = {atof(options[2].c_str())};
        w->filterVector_.push_back(split(options[3], ','));
    }


    // Go though each option and assign the correct properties
    vector<string> command;
    for (size_t i = skipOptions; i < options.size(); ++i) {
        // Deal with flags by loading pairs of options into commands
        if (options[i] == "-f") {
            if (i+1 < options.size()) {
                command = {options[i], options[i+1]};
                i++;  // skip the next option as it's already assigned above

            } else {
                cout << options[i] << " is not a valid flag" << endl;
            }

        } else if (options[i] == "-h" || options[i] == "--help"){
            help();
            continue;

        } else {
            split(options[i], '=', command);
        }

        // Assign properties based on commands
        if (command.size() != 2) {
            cout << command[0] << " is not a valid command." << endl;
            continue;

        } else if (command[0] == "-f" ||
                   command[0] == "--filters" ||
                   command[0] == "--filter" ) {
            w->filterList_ = split(command[1], ',');

        } else {
            cout << command[0] << " is not a valid command." << endl;
        }
    }
}


/* Automatically fill in all unassigned properties with defaults */
void fillUnassigned(shared_ptr<WorkspaceLC> w) {
    // Make a list of unique supernovae
    w->uniqueSNList_ = w->snNameList_;
    removeDuplicates<string>(w->uniqueSNList_);
}


int main(int argc, char *argv[]) {
    vector<string> options;
    shared_ptr<WorkspaceLC> w(new WorkspaceLC());

    getArgv(argc, argv, options);
    applyOptions(options, w);
    fillUnassigned(w);

	return 0;
}
