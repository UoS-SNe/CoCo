#ifndef COCO_PHASE_FUNC_HPP_
#define COCO_PHASE_FUNC_HPP_

#include "WorkspacePhase.hpp"


void readRecon(shared_ptr<WorkspacePhase>);
void syntheticFlux(shared_ptr<WorkspacePhase>);
void fitPhase(shared_ptr<WorkspacePhase>);
void normaliseSpec(shared_ptr<WorkspacePhase>,double);
void fit(shared_ptr<WorkspacePhase>);


#endif
