CC = gcc
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

CORE = src/core/utils.cpp src/core/priors.cpp src/core/SN.cpp
LC = src/lc/Workspace.cpp src/lc/Model.cpp src/lc/Solver.cpp src/lc/MultiNest.cpp
LCEXEC = src/LCFit.cpp

SPEC =
SPECEXEC = src/SpecFit.cpp

LCFIT = ${CORE:.cpp=.o} ${LC:.cpp=.o} ${LCEXEC:.cpp=.o}
SPECFIT = ${CORE:.cpp=.o} ${SPEC:.cpp=.o} ${SPECEXEC:.cpp=.o}

all: lcfit specfit

lcfit: $(LCFIT)
	$(CXX) $(LCFIT) $(LDFLAGS) -o $@

specfit: $(SPECFIT)
	$(CXX) $(SPECFIT) $(LDFLAGS) -o $@


clean:
	rm -f *.o src/*.o src/lc/*.o src/core/*.o
	rm -f lcfit specfit
