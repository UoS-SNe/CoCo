CC = clang
CXX = clang++
CFLAGS = -c -O3
CXXFLAGS = -c -g -std=c++11
LDFLAGS = -lgsl -lnest3 -lgfortran -llapack -Wl,-no_compact_unwind

.cpp.o:
	$(CXX) $(CXXFLAGS) $< -o $@

.cc.o:
	$(CXX) $(CXXFLAGS) $< -o $@

.c.o:
	$(CC) $(CFLAGS) $< -o $@

CORE = src/core/utils.cpp src/core/Filters.cpp src/core/Cosmology.cpp \
src/core/SN.cpp

SOLVERS = src/core/Solver.cpp src/solvers/priors.cpp src/solvers/MNest.cpp
MODELS = src/core/Model.cpp src/models/Firth17.cpp

LCEXEC = src/LCFit.cpp

# SPEC = src/spec/WorkspaceSpec.cpp src/spec/MultiNest.cpp
# SPECEXEC = src/SpecFit.cpp

# PHASE = src/phase/WorkspacePhase.cpp src/phase/func.cpp
# PHASEEXEC = src/SpecPhase.cpp
#
# SIM =
# SIMEXEC = src/LCSim.cpp


LCFIT = ${CORE:.cpp=.o} ${SOLVERS:.cpp=.o} ${MODELS:.cpp=.o} ${LCEXEC:.cpp=.o}
# SPECFIT = ${CORE:.cpp=.o} ${SOLVERS:.cpp=.o} ${MODELS:.cpp=.o} ${SPEC:.cpp=.o} \
# ${SPECEXEC:.cpp=.o}
# SPECPHASE = ${CORE:.cpp=.o} ${SOLVERS:.cpp=.o} ${LC:.cpp=.o} ${MPFIT:.cpp=.o} ${PHASE:.cpp=.o} ${PHASEEXEC:.cpp=.o}
# LCSIM = ${CORE:.cpp=.o} ${SOLVERS:.cpp=.o} ${LC:.cpp=.o} ${SIM:.cpp=.o} ${SIMEXEC:.cpp=.o}

all: lcfit # specfit specphase lcsim

lcfit: $(LCFIT)
	$(CXX) $(LCFIT) $(LDFLAGS) -o $@

# specfit: $(SPECFIT)
# 	$(CXX) $(SPECFIT) $(LDFLAGS) -o $@

# specphase: $(SPECPHASE)
# 	$(CXX) $(SPECPHASE) $(LDFLAGS) -o $@

# lcsim: $(LCSIM)
# 	$(CXX) $(LCSIM) $(LDFLAGS) -o $@


clean:
	rm -f *.o src/*.o src/solvers/*.o src/models/*.o src/core/*.o
	rm -f lcfit specfit specphase lcsim
