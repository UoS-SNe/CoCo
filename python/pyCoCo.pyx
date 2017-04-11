import cython
from libcpp.string cimport string
from libcpp.vector cimport vector
import numpy as np
cimport numpy as np

np.import_array()

cdef extern from "python/CoCo.hpp":
    cdef cppclass CoCo:
        vector[double] flux_
        vector[double] fluxErr_
        CoCo(string,string) except +
        void init()
        void simulate(string,double,double,double,double,double,double,
            vector[double],
            vector[string]) except +

@cython.boundscheck(False)
@cython.wraparound(False)
cdef class pyCoCo:
    cdef CoCo* thisptr # hold a C++ instance
    def __cinit__(self, string flt, string root):
        self.thisptr = new CoCo(flt, root)

    def __dealloc__(self):
        del self.thisptr

    def init(self):
        self.thisptr.init()

    def simulate(self,
                 string name,
                 double z,
                 double absMag,
                 double Ebv_MW,
                 double Ebv_Host,
                 double R_v,
                 double mjdPeak,
                 np.ndarray[double, ndim=1, mode="c"] mjdSim not None,
                 np.ndarray flt not None):

        self.thisptr.simulate(name,z,absMag,Ebv_MW,Ebv_Host,R_v,mjdPeak,
            <vector[double]> mjdSim,
            <vector[string]> flt)

        cdef np.ndarray[double, ndim=2] res = np.zeros((2,mjdSim.shape[0]), dtype=np.float64)
        res[0] = self.thisptr.flux_
        res[1] = self.thisptr.fluxErr_

        return res
