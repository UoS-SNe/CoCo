CC = clang
CXX = clang++
CFLAGS = -c -O3
CXXFLAGS = -c -O3 -std=c++11
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
MODELS = src/core/Model.cpp src/models/Firth17.cpp src/models/SpecMangle.cpp

LCEXEC = src/LCFit.cpp
SPECEXEC = src/SpecFit.cpp
# PHASEEXEC = src/SpecPhase.cpp
# SIMEXEC = src/LCSim.cpp


LCFIT = ${CORE:.cpp=.o} ${SOLVERS:.cpp=.o} ${MODELS:.cpp=.o} ${LCEXEC:.cpp=.o}
SPECFIT = ${CORE:.cpp=.o} ${SOLVERS:.cpp=.o} ${MODELS:.cpp=.o} ${SPECEXEC:.cpp=.o}
# SPECPHASE = ${CORE:.cpp=.o} ${SOLVERS:.cpp=.o} ${MODELS:.cpp=.o} ${PHASEEXEC:.cpp=.o}
# LCSIM = ${CORE:.cpp=.o} ${SOLVERS:.cpp=.o} ${MODELS:.cpp=.o} ${SIMEXEC:.cpp=.o}

all: lcfit specfit # specphase lcsim

lcfit: $(LCFIT)
	$(CXX) $(LCFIT) $(LDFLAGS) -o $@

specfit: $(SPECFIT)
	$(CXX) $(SPECFIT) $(LDFLAGS) -o $@

# specphase: $(SPECPHASE)
# 	$(CXX) $(SPECPHASE) $(LDFLAGS) -o $@

# lcsim: $(LCSIM)
# 	$(CXX) $(LCSIM) $(LDFLAGS) -o $@


clean:
	rm -f *.o src/*.o src/solvers/*.o src/models/*.o src/core/*.o
	rm -f lcfit specfit specphase lcsim
