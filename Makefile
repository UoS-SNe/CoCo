CC = clang
CXX = clang++
CFLAGS = -c -O3
CXXFLAGS = -c -O3 -std=c++11

LDFLAGS = -L /opt/local/lib -lgsl -L /usr/local/lib -lmultinest -L /usr/local/gfortran/lib -lgfortran -llapack -Wl,-no_compact_unwind 

.cpp.o:
	$(CXX) $(CXXFLAGS) $< -o $@

.cc.o:
	$(CXX) $(CXXFLAGS) $< -o $@

.c.o:
	$(CC) $(CFLAGS) $< -o $@

CORE = src/core/utils.cpp src/core/Filters.cpp src/core/Cosmology.cpp \
src/core/SN.cpp

MPFIT = src/solvers/mpfit.c
SOLVERS = src/core/Solver.cpp src/solvers/priors.cpp src/solvers/MNest.cpp \
src/solvers/MPFitter.cpp
MODELS = src/core/Model.cpp src/models/Karpenka12.cpp src/models/Bazin09.cpp \
src/models/Kessler10.cpp src/models/Firth17Complex.cpp src/models/SpecMangle.cpp \
src/models/LinearMangle.cpp

LCEXEC = src/LCFit.cpp
SPECEXEC = src/SpecFit.cpp
PHASEEXEC = src/SpecPhase.cpp
SIMEXEC = src/LCSim.cpp


LCFIT = ${CORE:.cpp=.o} ${MPFIT:.c=.o} ${SOLVERS:.cpp=.o} ${MODELS:.cpp=.o} ${LCEXEC:.cpp=.o}
SPECFIT = ${CORE:.cpp=.o} ${MPFIT:.c=.o} ${SOLVERS:.cpp=.o} ${MODELS:.cpp=.o} ${SPECEXEC:.cpp=.o}
SPECPHASE = ${CORE:.cpp=.o} ${MPFIT:.c=.o} ${SOLVERS:.cpp=.o} ${MODELS:.cpp=.o} ${PHASEEXEC:.cpp=.o}
LCSIM = ${CORE:.cpp=.o} ${MPFIT:.c=.o} ${SOLVERS:.cpp=.o} ${MODELS:.cpp=.o} ${SIMEXEC:.cpp=.o}

all: lcfit specfit specphase lcsim

lcfit: $(LCFIT)
	$(CXX) $(LCFIT) $(LDFLAGS) -o $@

specfit: $(SPECFIT)
	$(CXX) $(SPECFIT) $(LDFLAGS) -o $@

specphase: $(SPECPHASE)
	$(CXX) $(SPECPHASE) $(LDFLAGS) -o $@

lcsim: $(LCSIM)
	$(CXX) $(LCSIM) $(LDFLAGS) -o $@


clean:
	rm -f *.o src/*.o src/solvers/*.o src/models/*.o src/core/*.o
	rm -f lcfit specfit specphase lcsim
