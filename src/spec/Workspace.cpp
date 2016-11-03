#include "Workspace.hpp"

using namespace std;


Workspace::Workspace() {
    // Initialise all variables with defaults designed to halt the code if empty
    SpecListFile_ = "None";
    filterPath_ = "data/filters";

    filterList_.resize(0);
}
