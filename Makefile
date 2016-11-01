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

CORE = src/utils.cpp src/SN.cpp src/Workspace.cpp src/priors.cpp
MODELS = src/Model.cpp
SOLVERS = src/Solver.cpp src/MultiNest.cpp
LCEXEC = src/LCFit.cpp

LCFIT = ${CORE:.cpp=.o} ${CCORE:.c=.o} ${SOLVERS:.cpp=.o} ${MODELS:.cpp=.o} ${LCEXEC:.cpp=.o}

all: lcfit

lcfit: $(LCFIT)
	$(CXX) $(LCFIT) $(LDFLAGS) -o $@

clean:
	rm -f *.o src/*.o modelfit lc
