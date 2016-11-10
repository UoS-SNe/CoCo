#include "Workspace.hpp"

using namespace std;


Workspace::Workspace() {
    // Initialise all variables with defaults designed to halt the code if empty
    LCListFile_ = "None";
    fileList_.resize(0);
    filterList_.resize(0);

    SNID_ = 0;
    FLTID_ = 0;
}
