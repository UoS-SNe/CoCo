#include <vector>
#include <string>

#include "core/Filters.hpp"
#include "phase/WorkspacePhase.hpp"
#include "phase/func.hpp"
#include "core/utils.hpp"

using namespace std;


void help() {
    cout << "CoCo - SpecPhase: \n";
    cout << "Originally writen by Natasha Karpenka, ";
    cout << "currently maintained by Szymon Prajs (S.Prajs@soton.ac.uk) ";
    cout << "and Rob Firth.\n";
    cout << "\nUsage:\n";
    cout << "specphase filter_name\n";
    cout << endl;
}


/* Assign input options to workspace parameters */
void applyOptions(vector<string> &options, shared_ptr<WorkspacePhase> w) {
    if (options.size() < 1 || options[0] == "-h" || options[0] == "--help") {
        help();
        exit(0);

    } else if (options.size() == 1)  {
        // For any other extension just assign the file as the only LC
        w->filter_ = options[0];

    } else {
        cout << "Options are not currently implemented\n";
        cout << "Program will continue executing" << endl;
    }
}


int main (int argc, char* argv[]) {
    vector<string> options;
    shared_ptr<WorkspacePhase> w(new WorkspacePhase());

    getArgv(argc, argv, options);
    applyOptions(options, w);

    // Read in filters and find the ID of the filter used to determine the phase
    w->filters_ = shared_ptr<Filters>(new Filters(w->filterPath_));

    readRecon(w);
    syntheticFlux(w);

    return 0;
}
