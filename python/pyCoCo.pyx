import cython
from libcpp.string cimport string
from libcpp.vector cimport vector
from libcpp.unordered_map cimport unordered_map
import numpy as np
cimport numpy as np

np.import_array()

cdef extern from "python/CoCo.hpp":
    cdef cppclass CoCo:
        vector[double] mjd_
        vector[double] flux_
        vector[double] fluxErr_
        unordered_map[string, vector[double]] paramMap_
        CoCo(string,string) except +
        void init()
        void simulate(string,double,double,double,double,double,double,
            vector[double],
            vector[string],
            vector[double],
            double) except +
        void simulate(string,double,double,double,double,double,double,
            vector[double],
            vector[string]) except +
        void spec_photometry(string,double,string) except +

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

    def simulate_debug(self,
                            string name,
                            double z,
                            double absMag,
                            double Ebv_MW,
                            double Ebv_Host,
                            double R_v,
                            double mjdPeak,
                            np.ndarray[double, ndim=1, mode="c"] mjdSim not None,
                            np.ndarray flt not None,
                            np.ndarray[double, ndim=1, mode="c"] params not None,
                            double lum_dist):

        self.thisptr.simulate(name,z,absMag,Ebv_MW,Ebv_Host,R_v,mjdPeak,
            <vector[double]> mjdSim,
            <vector[string]> flt,
            <vector[double]> params)

        cdef np.ndarray[double, ndim=2] res = np.zeros((2,mjdSim.shape[0]), dtype=np.float64)
        res[0] = self.thisptr.flux_
        res[1] = self.thisptr.fluxErr_

        return res

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

    def get_fit_params(self):
        return self.thisptr.paramMap_

    def spec_photometry(self,
                        string name,
                        double z,
                        string flt):

        self.thisptr.spec_photometry(name,z,flt)

        cdef np.ndarray[double, ndim=2] res = np.zeros((2,self.thisptr.flux_.size()), dtype=np.float64)
        res[0] = self.thisptr.mjd_
        res[1] = self.thisptr.flux_

        return res
