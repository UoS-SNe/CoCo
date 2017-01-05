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
    cout << "Originally writen by Natasha Karpenka, ";
    cout << "currently maintained by Szymon Prajs (S.Prajs@soton.ac.uk) ";
    cout << "and Rob Firth.\n";
    cout << "\nUsage:\n";
    cout << "lcsim *.list\n";
    cout << "or\n";
    cout << "./lcsim SN_name rsedshift\n\n";
    cout << "*.list file must have the following columns:\n";
    cout << "SN_name redshift\n";
    cout << endl;
}


/* Assign input options to workspace parameters */
void applyOptions(vector<string> &options, shared_ptr<WorkspaceLC> w) {
    if (options.size() < 1 || options[0] == "-h" || options[0] == "--help") {
        help();
        exit(0);
    }

    // First option is a SN name or list of SN names and redshifts
    w->LCListFile_ = options[0];
    if (options[0].substr(options[0].find_last_of(".") + 1) == "list") {
        vmath::loadtxt<string>(w->LCListFile_, 2, w->infoList_);
        w->snNameList_ = w->infoList_[0];
        w->zList_ = castString<double>(w->infoList_[1]);

    } else if (options.size() >= 2)  {
        // For any other extension just assign the file as the only LC
        w->snNameList_ = {options[0]};
        w->zList_ = {atof(options[1].c_str())};
    }


    // Go though each option and assign the correct properties
    vector<string> command;
    for (size_t i = 1; i < options.size(); ++i) {
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
    // Do a sanity check for the LC files
    if (w->fileList_.size() == 0) {
        cout << "Something went seriously wrong.";
        cout << "Please consider report this bug on our GitHub page";
        cout << endl;
        exit(0);
    }

    // Make a list of unique supernovae
    w->uniqueSNList_ = w->snNameList_;
    removeDuplicates<string>(w->uniqueSNList_);

    // Make a filter list
    if (w->filterList_.size() == 0) {
        // TODO - Look for filters in LC files
    }
}


int main(int argc, char *argv[]) {
    help();

	return 0;
}
