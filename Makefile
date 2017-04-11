CC = gcc
CXX = g++
CFLAGS = -c -fPIC -O3
CXXFLAGS = -c -O3 -fPIC -std=c++0x -I $(GSL_DIR)/include
LDFLAGS = -L $(GSL_DIR)/lib -lgsl -lgslcblas
.cpp.o:
	$(CXX) $(CXXFLAGS) $< -o $@

.c.o:
	$(CC) $(CFLAGS) $< -o $@

CORE = src/core/utils.cpp src/core/Filters.cpp src/core/Cosmology.cpp \
src/core/SN.cpp

MPFIT = src/solvers/mpfit.c
SOLVERS = src/core/Solver.cpp src/solvers/priors.cpp src/solvers/MPFitter.cpp
MODELS = src/core/Model.cpp src/models/Karpenka12.cpp src/models/Bazin09.cpp \
src/models/Kessler10.cpp src/models/Firth17Complex.cpp src/models/SpecMangle.cpp \
src/models/LinearMangle.cpp

SIMEXEC = src/LCSim.cpp
PYCOCO = python/CoCo.cpp

LCSIM = ${CORE:.cpp=.o} ${MPFIT:.c=.o} ${SOLVERS:.cpp=.o} ${MODELS:.cpp=.o} \
${PYCOCO:.cpp=.o} ${SIMEXEC:.cpp=.o}
LIB = ${CORE:.cpp=.o} ${MPFIT:.c=.o} ${SOLVERS:.cpp=.o} ${MODELS:.cpp=.o}

all: lcsim lib

lcsim: $(LCSIM)
	$(CXX) $(LCSIM) $(LDFLAGS) -o $@

lib: $(LIB)
	$(CXX) $(LIB) $(LDFLAGS) -shared -o libcoco.so

clean:
	rm -f *.o src/*.o src/solvers/*.o src/models/*.o src/core/*.o
	rm -f lcsim
