#ifndef COCO_PHASE_FUNC_HPP_
#define COCO_PHASE_FUNC_HPP_

#include <vector>
#include <iostream>
#include <string>
#include <algorithm>
#include <stdlib.h>
#include "WorkspacePhase.hpp"
#include "../core/Filters.hpp"
#include "../core/mpfit.h"
#include "../lc/Model.hpp"
#include "../vmath/loadtxt.hpp"
#include "../vmath/convert.hpp"
#include "../vmath/stat.hpp"

void readRecon(shared_ptr<WorkspacePhase>);
void syntheticFlux(shared_ptr<WorkspacePhase>);
void fitPhase(shared_ptr<WorkspacePhase>);
void fit(shared_ptr<WorkspacePhase>);


#endif
