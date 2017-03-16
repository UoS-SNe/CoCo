cdef extern from "CoCo.hpp":
    cdef cppclass CoCo:
        CoCo() except +


cdef class pyCoCo:
    cdef CoCo* thisptr # hold a C++ instance
    def __cinit__(self):
        self.thisptr = new CoCo()

    def __dealloc__(self):
        del self.thisptr
