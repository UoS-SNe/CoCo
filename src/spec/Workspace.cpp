#include "Workspace.hpp"

using namespace std;


Workspace::Workspace() {
    // Initialise all variables with defaults designed to halt the code if empty
    SpecListFile_ = "None";
    filterPath_ = "data/filters";

    fileList_.resize(0);
    filterList_.resize(0);
}
